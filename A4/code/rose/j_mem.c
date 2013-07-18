/**************************************************************************************
*  j_mem.c
*
*  This file contains code implementing heap management functions such as memory allocation,
*  method frames allocation and dealocation, etc.
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


/****************************************************************************
* Clears the specified memory area.
*
* Input:        mem     starting address
*               len     length of memory block
* Output:       none
* Return:       none
****************************************************************************/
void clearMem(uint8 *mem, int32 len)
{
    while (len > 0)
        mem[--len] = 0;
}


/****************************************************************************
* Copies data from one memory location to another.
*
* Input:        to      destination address
*               from    starting address
*               len     length of memory block
* Output:       none
* Return:       none
****************************************************************************/
void copyMem(uint8 *to, uint8 *from, int32 len)
{
    int32 i;
    for (i=len-1; i >= 0; i--)
        to[i] = from[i];
}

/****************************************************************************
* Tries to find free slot in references table.
*
* Input:        none
* Output:       none
* Return:       pointer to free reference slot
****************************************************************************/
ref_t *findFreeRef(void)
{
    uint16 i;

    for (i=0; i < config->references; i++)
    {
        if (refs[i].ptr == NULL)
            return &refs[i];
    }
    vm_excep1 = MEMERR_NoRefs;
    return NULL;
}

/****************************************************************************
* Allocates memory on the heap.
* Allocation starts by searching the heap memory for previously freed memory block that
* has same or bigger size that amount required. If such block is not found
* than a new memory block is acquired from the heap end.
* If not enough memory is available for the new object, garbage collector is
* invoked to free some memory.
* If there is still not enough memory after GC is run then MEMERR_NoHeap
* exception is thrown.
* GC is also invoked if there is no free entry in the references table. If no
* reference entry is available after GC is run then MEMERR_NoRefs
* exception is thrown.
*
* Input:        size      size of the memory to be allocated
* Output:       none
* Return:       pointer to reference of the object allocated on the heap if
*               successfull, otherwise NULL returned and appropriate
*               exception is set in vm_excep
****************************************************************************/
ref_t *allocHeapMem(uint32 size)
{
    uint8 *mem, i;
    objhdr_t *obj = NULL;
    ref_t *ref = NULL;

    vm_status &= ~GC_DONE_FLAG;

    /* make size multiple of four */
    size = ALIGN4(size);

    for (i=0; i < ALLOC_TRY; i++)
    {
        /* try to find existing free memory block that could accomodate new object */
        mem = objdata_start;
        while (mem < heap_ptr)
        {
            obj = (objhdr_t *)mem;
            if (obj->flag == 0 && (uint32)(obj->size) >= size)
            {
                size = obj->size;
                break;
            }
            if (obj->size == 0)
            {
                vm_excep = MEMERR_AllocFail;
                return NULL;
            }
            mem += obj->size;
        }
        /* allocate new memory block if necessary */
        if (mem >= heap_ptr)
        {
            /* check if we have enough memory for the new object */
            if ((uint32)(frame_ptr - heap_ptr) < (uint32)size)
            {
                vm_excep1 = MEMERR_NoHeap;
#if ENABLE_GC
                if (vm_status & GC_DONE_FLAG)
                {
                    i = ALLOC_TRY;
                    break;
                }
                if (!walkAll())
                    return NULL;
#endif
                continue;
            }
            mem = heap_ptr;
            obj = (objhdr_t *)mem;
        }
        /* find free entry in references table */
        ref = findFreeRef();
        if (ref == NULL)
        {
#if ENABLE_GC
            /* if free reference not found - run GC */
            if (vm_status & GC_DONE_FLAG)
            {
                i = ALLOC_TRY;
                break;
            }
            if (!walkAll())
                return NULL;
#endif
            continue;
        }
        obj->size = size;
        ref->ptr = obj;
        if (mem == heap_ptr)
            heap_ptr += size;

        /* clear object's memory excluding the size field */
        clearMem(mem + sizeof(obj->size), size - sizeof(obj->size));
        break;
    }
    if (i == ALLOC_TRY)
        vm_excep = vm_excep1;

    return ref;
}


/****************************************************************************
* Creates frame for the method which offset is specified in input parameter.
* Frame size consists of local variables, operand stack and working data
* required by the JVM. Size of frame is retrieved from APP_MAX_FRAME. This
* value should be big enugh to hold the largest method frame. FR_DATA_SIZE is
* not included in APP_MAX_FRAME and added on top of this value.
*
* NOTE: Frames are allocated from the end of the heap.
*
* Process of creating a new frame consits of:
* 1. Search from the end of the heap for a free method frame. Free frame is
*    identified by FR_METHOD_PTR being zero. Search ends when we reach
*    location pointed to by frame_ptr.
* 2. If free frame wasn't found check if there is enough room on the heap for
*    a new frame: frame_ptr-frame_size > heap_ptr
* 3. If there is not enough memory for new frame garbage collector is invoked to
*    free some memory on the heap. If not enough memory is available after GC is run
*    then exception MEMERR_NoFrames is thrown.
*
* Input:        method  - pointer to method definition
* Output:       none
* Return:       pointer to new frame, NULL otherwise
****************************************************************************/
frame_t* createFrame(frame_t* parent, method_t *method)
{
    uint8 *mem;
    frame_t *frame = NULL;

    /* save parent frame's PC & SP */
    if(parent != NULL)
    {
	parent->pc = vm_pc;
	parent->sp = vm_sp;
    }
    
    vm_status &= ~GC_DONE_FLAG;
    while (true)
    {
        /* search for empty slot within already allocated frames */
        mem = config->heap_end - frame_size;
        while (mem >= frame_ptr)
        {
            frame = (frame_t *)mem;
            if (frame->method == NULL)
                break;
            mem -= frame_size;
        }
        /* allocate memory for new frame if required */
        if (mem < frame_ptr)
        {
            /* if we have dropped below object heap - run GC */
            if (mem <= heap_ptr)
            {
#if ENABLE_GC
                if (vm_status & GC_DONE_FLAG)
                {
                    vm_excep = MEMERR_NoFrames;
                      return NULL;
                }
                if (!walkAll())
                return NULL;
                    continue;
#else
                vm_excep = MEMERR_NoFrames;
                return NULL;
#endif
            }
            frame_ptr = mem;
            frame = (frame_t *)mem;
        }
        /* clear the frame and then setup frame's fields */
        clearMem(mem, frame_size);

        frame->method = method;
        frame->class_ptr = CLASS_LOC(method->class_ptr);
        frame->prev = parent;
        if (parent != NULL)
          parent->next = frame;
        frame->next = NULL;

        /* setup SP and PC */
        frame->sp = (value_t *)(mem + method->locals + FRAME_HDR_SIZE);
        frame->pc = (uint8 *)method + sizeof(method_t);

        break;
    }
    return frame;
}


/****************************************************************************
* Restores previous method frame in the current thread.
*
* Input:        none
* Output:       method's return value if exists
* Return:       true if prev. frame has been restored, othrewise false is
*               returned.
****************************************************************************/
bool releaseFrame(value_t *ret_val)
{
    /* mark this frame as free */
    thr_active->curr_frame->method = NULL;

    if (thr_active->curr_frame->prev == NULL)
        return false;

    /* update link pointers */
    thr_active->curr_frame = thr_active->curr_frame->prev;
    thr_active->curr_frame->next = NULL;

    /* copy return value in case it exists */
    *ret_val = *(--vm_sp);

    /* update SP and PC */
    vm_sp = thr_active->curr_frame->sp;
    vm_pc = thr_active->curr_frame->pc;

    return true;
}

