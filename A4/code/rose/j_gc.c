/**************************************************************************************
*  j_gc.c
*
*  This file contains code implementing garbage collection.
*  Garbage collection can be triggered by one of the following events:
*  1. Insufficient memory for a new method frame
*  2. Insufficient memory for a new object on the heap
*  3. Reference table has not any free slots
*  4. Strings table has not any free slots
*  5. User application invoked java/lang/System.gc() method.
*
**************************************************************************************
*
*  The contents of this file are subject to the RTJ COMPUTING non-commercial
*  source code license agreement ("License"); you may not use this file except in
*  compliance with the License. A copy of the License (license.terms) can be
*  found in the root of installation folder or on the web at
*  http://www.rtjcom.com/Files/license.terms
*
*  Software distributed under the License is distributed on an "AS IS"
*  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
*  License for the specific language governing rights and limitations
*  under the License.
*
*  Copyright (c) 2000 RTJ Computing. All right reserved.
*
*  Developer(s): Peter Gasparik <peterg@rtjcom.com>
**************************************************************************************/
#include "jvm.h"
#include "jcommon.h"

#if ENABLE_GC

/* Forward declarations */
static void walkThreadTable(void);
static void walkBlock(uint8 *start, uint8 *end);
static void walkStatic(void);
static void walkFrames(void);
static void walkInstances(void);
static void walkArrays(void);
static void compactHeap(void);
static bool isReference(uint32 val);
static void freeUnused(void);
static bool updateRef(objhdr_t *newloc, objhdr_t *oldloc);
static void compactFrames(void);

/****************************************************************************
* Garbage collection involves walking trough the heap and marking all objects
* that are referenced by:
* 1. static fields
* 2. method local variables and entries on method stack
* 3. instance fields of classes that were marked in step 1, 2 or 3
* 4. elements of arrays (marked in step 1 or 2) created by anewarray bytecode
*
* Heap objects that weren't marked during 'walking' are considered as
* garbage and can be reused during heap allocation process or can be
* reclaimed during heap compaction.
* At the beginning of heap walk it is assumed that all objects are marked
* with while color -- done by AllocHeapMem when new memory is allocated.
* Instances of system run-time exception are always marked as black during
* VM initialisation and remain live for the whole duration of VM session.
* Encountered array are marked with black color as they
* don't need revisiting and represents live objects.
* If class instance objects are found on the heap they are marked with
* gray color. This means that these objects are marked for consequent
* walk to see if class instance fields contain pointers to other objects on
* the heap. Once are class instance fields revisited they are marked with
* black color meaning that no further revisiting is needed.
* The same apply to arrays of objects.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
bool walkAll(void)
{
    /* nothing to be done if frame for current thread hasn't been created yet */
    if (thr_active->curr_frame != NULL)
    {
        /* save SP, PC in current frame */
        thr_active->curr_frame->pc = vm_pc;
        thr_active->curr_frame->sp = vm_sp;

        /* clear GC flags in status uint8 */
        vm_status &= (uint8) ~(OBJ_ARR_FLAG | GRAY_FLAG);

        /* execute individual GC processes */
        walkThreadTable();
        walkStatic();
        walkFrames();
        walkInstances();
        walkArrays();
        freeUnused();
        compactHeap();

        if (vm_excep != NO_EXCEP)
            return false;
    }
    vm_status |= GC_DONE_FLAG;
    return true;
}


/****************************************************************************
* Walks trough the entries in the thread table and marks all active thread
* objects with gray color indicating that these objects will require
* revisiting.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void walkThreadTable(void)
{
    uint16 i;

    for (i=0; i < config->threads; i++)
    {
        if (threads[i].state != DEAD)
            threads[i].objref->ptr->flag |= OF_GRAY;
    }
}


/****************************************************************************
* Walks trough the region of memory containg either static fields,
* locals & stack, instance fields or arrays of references.
* Referenced heap objects are marked with:
* 1. gray color  - for class instance objects and arrays of objects, which will
*                  need to be revisited
* 2. black color - for array objects.
*
* Input:        start   starting address
*               end     end address (exclusive)
* Output:       none
* Return:       none
****************************************************************************/
static void walkBlock(uint8 *start, uint8 *end)
{
    value_t *value;
    ref_t   *ref;

    while (start < end)
    {
        value = (value_t *)start;
        if (isReference(value->val))
        {
            ref = GET_REF(value->val);
            if (ref->ptr->flag & OF_CLASS)
            {
                if (!(ref->ptr->flag & OF_BLACK))
                {
                    ref->ptr->flag |= OF_GRAY;
                    vm_status |= GRAY_FLAG;
                }
            }
            else if (!(ref->ptr->flag & OF_BLACK))
            {
                ref->ptr->flag |= OF_BLACK;
                if (((arrhdr_t *)ref->ptr)->type == T_OBJREF)
                    vm_status |= OBJ_ARR_FLAG;
            }
        }

        start += sizeof(value_t);
    }
}


/****************************************************************************
* Walks trough the static fields to see if contain references to objects
* on the heap.
* Referenced heap objects are marked with gray or black color.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void walkStatic(void)
{
    walkBlock((uint8 *)static_start, VM_STATIC_END);
}


/****************************************************************************
* Walks trough the local variables and stack entries of method frames to
* see if contain references to objects on the heap. Method frames of all
* threads are checked during this process.
* Referenced heap objects are marked with gray or black color.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void walkFrames(void)
{
    uint16 i;
    frame_t *frame;

    for (i=0; i < config->threads; i++)
    {
        if (threads[i].state != DEAD)
        {
            frame = threads[i].curr_frame;
            while (frame)
            {
                walkBlock((uint8 *)frame->locals, (uint8 *)frame->sp);
                frame = frame->prev;
            }
        }
    }
}


/****************************************************************************
* Walks trough the instance fields of all class objects to see if contain
* references to other objects on the heap. This process will repeat until
* there are still new objects that need walking trough.
* Referenced heap objects are marked with gray or black color.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void walkInstances(void)
{
    uint16 i;

    while (vm_status & GRAY_FLAG)
    {
        vm_status &= (uint8) ~GRAY_FLAG;
        for (i=0; i < config->references; i++)
        {
            if (refs[i].ptr != NULL)
            {
                if ((refs[i].ptr->flag & (OF_GRAY | OF_CLASS)) == (OF_GRAY | OF_CLASS))
                {
                    /* re-visited objects mark with black */
                    refs[i].ptr->flag |= OF_BLACK;
                    walkBlock((uint8 *)(refs[i].ptr) + sizeof(objhdr_t),
                              (uint8 *)(refs[i].ptr) + refs[i].ptr->size);
                }
            }
        }
    }
}


/****************************************************************************
* Walks trough the elements of arrays created with anewarray bytecode to
* see if contain references to other objects on the heap.
* Referenced heap objects are marked with gray or black color.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void walkArrays(void)
{
    uint16 i;

    while (vm_status & OBJ_ARR_FLAG)
    {
        vm_status &= ~OBJ_ARR_FLAG;
        for (i=0; i < config->references; i++)
        {
            if (refs[i].ptr && (refs[i].ptr->flag & OF_ARRAY) && ((arrhdr_t *)(refs[i].ptr))->type == T_OBJREF)
            {
                walkBlock((uint8 *)refs[i].ptr + sizeof(arrhdr_t),
                          (uint8 *)refs[i].ptr + refs[i].ptr->size);
                walkInstances();
            }
        }
    }
}


/****************************************************************************
* Checks if the 32-bit value pointed to by X is reference to an object or not.
* 32-bit value is considered to be reference if:
* 1. upper 16-bit has value REFERENCE
* 2. Lower 16-bits points to entry in references table
* 3. REF_PTR field of that entry points to object on the heap
* 4. OBJ_FLAG field of that object is either OF_ARRAY or OF_CLASS
* NOTE: more checks could be included if above ones are not sufficient.
*
* Input:        val     pointer to 32-bit value that has to be inspected
* Output:       none
* Return:               true if value on the heap is reference, otherwise
*                       false is returned
*
****************************************************************************/
static bool isReference(uint32 val)
{
    ref_t *ref;

    if ((val & REFERENCE) == REFERENCE)
    {
        ref = GET_REF(val);
        if ((uint8 *)ref >= (uint8 *)refs && (uint8 *)ref < VM_REFS_END)
        {
            if ((uint8 *)ref->ptr >= objdata_start && (uint8 *)ref->ptr < frame_ptr)
            {
                if (ref->ptr->flag & (OF_CLASS | OF_ARRAY))
                    return true;
            }
        }

    }
    return false;
}


/****************************************************************************
* Makes all unused heap objects (those not having OF_GRAY or OF_BLACK flag
* set) free -- available to AllocHeapMem for reuse. Gray and black objects
* are made white for next GC run.
* Object are made free by clearing OBJ_FLAG and REF_PTR in appropriate
* reference.
*
* Input:        none
* Output:       none
* Output:       none
****************************************************************************/
static void freeUnused(void)
{
    uint16 i;

    for (i=0; i < config->references; i++)
    {
        /* DO NOT garbage collect system references */
        if (&refs[i] < app_refs)
            continue;

        if (refs[i].ptr && refs[i].ptr->flag)
        {
            if ((refs[i].ptr->flag & (OF_GRAY | OF_BLACK)) == 0)
            {
                removeFromStrTable(&refs[i]);
                refs[i].ptr->flag = 0;
                refs[i].ptr = 0;
                refs[i].class_ptr = 0;
            }
            else
                refs[i].ptr->flag &= (uint8) ~(OF_GRAY | OF_BLACK);
        }
    }
}


/****************************************************************************
* Compacts the heap if amount of free space drops below COMPACT_TRESHOLD.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void compactHeap(void)
{
#if 1
    if ((frame_ptr - heap_ptr) < COMPACT_TRESHOLD)
#endif
    {
        compactHeapForced();
    }
}


/****************************************************************************
* Compacts the heap by 'sliding' live objects (class instance data, arrays)
* to the begining of heap.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
void compactHeapForced(void)
{
    uint8 *to, *from;
    uint32 i, size = 0;

    /* find first empty memory block on the heap */
    to = objdata_start;
    while (to < heap_ptr)
    {
        size = ((objhdr_t *)to)->size;
        if (((objhdr_t *)to)->flag == 0)
            break;

        to += size;
    }

    /* find next live object */
    from = to + size;
    while (from < heap_ptr)
    {
        size = ((objhdr_t *)from)->size;
        if (((objhdr_t *)from)->flag != 0)
        {
            /* copy object from old location to new one */
            for (i=0; i < size; i++)
                to[i] = from[i];
            /* update corresponding reference */
            if (!updateRef((objhdr_t *)to, (objhdr_t *)from))
                return;
            to += size;
        }
        from += size;
    }

    heap_ptr = to;
    compactFrames();
}


/****************************************************************************
* Updates reference that matches input data with new location of the object.
*
* Input:        newloc     object's new location
*               oldloc     object's old location
* Output:       none
* Return:                   false if old reference can't be found,
*                           true references exchange suceeded
****************************************************************************/
static bool updateRef(objhdr_t *newloc, objhdr_t *oldloc)
{
    uint16 i;
    bool replaced = false;

    for (i=0; i < config->references; i++)
    {
        /* skip system references */
        if (&refs[i] < app_refs)
            continue;

        if (refs[i].ptr == oldloc)
        {
            refs[i].ptr = newloc;
            replaced = true;
        }
    }
    if (!replaced)
    {
        vm_excep = INTERR_gcFail;
        return false;
    }
    return true;
}


/****************************************************************************
* Performs very simple compaction of the frames memory by simply shifting
* end of frames pointer (frame_ptr) to the nearest used frame slot.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
static void compactFrames(void)
{
    while (((frame_t *)frame_ptr)->method == 0)
    {
        frame_ptr += frame_size;
    }
}

#endif


