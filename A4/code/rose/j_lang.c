/**************************************************************************************
*  j_lang.c
*
*  This file contains native methods for system paclakges defined in the ClassLinker's
*  control file.
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
#define __timer_t_defined
#include <math.h>
#include <stdio.h>
#include <errno.h>

#include "jvm.h"
#include "jcommon.h"

/****************************************************************************
* Copies array items from src starting at src_position to dst at
* dst_position. Number of bytes to copy specifies length parameter.
* Method will generate several exceptions if parameters are not correct.
*
* Java prototype:
*           void arraycopy(Object src, int src_position, Object dst,
*                          int dst_position, int length);
*
****************************************************************************/
static int16 java_lang_System_arraycopy(int32 par[], int32 *rv)
{
    uint16 count = 0;
    ref_t *ref;
    arrhdr_t *src, *dst;
    uint8 *srcData, *dstData;

    /* verify parameters */
    if (par[4] == 0)
        return NO_EXCEP;

    if (par[1] < 0 || par[3] < 0 || par[4] < 0)
        return EXCEP_IndexOutOfBounds;


    ref = GET_REF(par[0]);
    if (ref == NULL)
        return EXCEP_NullPointer;

    src = (arrhdr_t *)ref->ptr;

    ref = GET_REF(par[2]);
    if (ref == NULL)
        return EXCEP_NullPointer;

    dst = (arrhdr_t *)ref->ptr;

    if (!(src->obj.flag & OF_ARRAY) || !(dst->obj.flag & OF_ARRAY) ||
        (uint32)(par[1]+par[4]) > src->count || (uint32)(par[3]+par[4]) > dst->count ||
        src->type != dst->type)
    {
		//fprintf(stderr, "ARRAY_STORY: %d\n", (int)!(dst->obj.flag & OF_ARRAY));
		fprintf(stderr, "ARRAY_STORY: %d\n", (int)!(dst->obj.flag & OF_ARRAY));
        return EXCEP_ArrayStore;
    }
    /* prepare pointers for copying */
    srcData = (uint8 *)src + sizeof(arrhdr_t);
    dstData = (uint8 *)dst + sizeof(arrhdr_t);
    switch (src->type)
    {
        case T_BOOLEAN:
        case T_BYTE:
#if !ENABLE_UNICODE
        case T_CHAR:
#endif
            srcData += (uint16)par[1];
            dstData += (uint16)par[3];
            count = (uint16)par[4];
            break;

#if ENABLE_UNICODE
        case T_CHAR:
#endif
        case T_SHORT:
            srcData += (uint16)(par[1] << 1);
            dstData += (uint16)(par[3] << 1);
            count = (uint16)(par[4] << 1);
            break;

        case T_INT:
        case T_FLOAT:
        case T_OBJREF:
            srcData += (uint16)(par[1] << 2);
            dstData += (uint16)(par[3] << 2);
            count = (uint16)(par[4] << 2);
            break;
    }
    /* copy from source to destination, starting from the end of array */
    while (count-- > 0)
        dstData[count] = srcData[count];

    return NO_EXCEP;
}

/****************************************************************************
* Returns amount of free memory on the heap for application objects.
*
* Java prototype:
*           int freeMemory();
*
****************************************************************************/
static int16 java_lang_System_freeMemory(int32 par[], int32 *rv)
{
    *rv = (frame_ptr - heap_ptr);
    return NO_EXCEP;
}


/****************************************************************************
* Returns total amount of memory on the heap for application objects..
*
* Java prototype:
*           int16 totalMemory();
*
****************************************************************************/
static int16 java_lang_System_totalMemory(int32 par[], int32 *rv)
{
    *rv = frame_ptr - objdata_start;
    return NO_EXCEP;
}


/****************************************************************************
* Invokes garbage collector. Method returns once the GC finishes its work.
*
* Java prototype:
*           void gc();
*
****************************************************************************/
static int16 java_lang_System_gc(int32 par[], int32 *rv)
{
#if ENABLE_GC
    walkAll();
    compactHeapForced();
#endif
    return vm_excep;
}


/****************************************************************************
* Returns time in ms since VM started running.
*
* Java prototype:
*           static int currentTimeMillis();
*
****************************************************************************/
static int16 java_lang_System_currentTimeMillis(int32 par[], int32 *rv)
{
    *rv = vm_time_ms;
    return NO_EXCEP;
}

/****************************************************************************
* Prints the specified string
*
* Java prototype:
*           static void print(String str);
*
****************************************************************************/
static int16 java_lang_System_print(int32 par[], int32 *rv)
{
    jchar *arr;
    int   i,count;
    
    arr = getStringArray(par[0]);
    if(arr == NULL)
	return EXCEP_NullPointer;

    count = getStringCount(par[0]);

    for(i = 0; i < count; i++)
	printf("%c",arr[i]);

    return NO_EXCEP;
}

/****************************************************************************
* Prints the specified string and a CR
*
* Java prototype:
*           static void println(String str);
*
****************************************************************************/
static int16 java_lang_System_println(int32 par[], int32 *rv)
{
    jchar *arr;
    int   i,count;
    
    arr = getStringArray(par[0]);
    if(arr == NULL)
	return EXCEP_NullPointer;

    count = getStringCount(par[0]);

    for(i = 0; i < count; i++)
	printf("%c",arr[i]);

    printf("\n");

    return NO_EXCEP;
}

/****************************************************************************
* Reads in a line of input and returns it as a String object
*
* Java prototype:
*           static String readln();
*
****************************************************************************/
static int16 java_lang_System_readln(int32 par[], int32 *rv)
{
    char chrs[256] = "\0";
    ref_t* str = NULL;
    int count;
    
    fgets(chrs,256,stdin);
    count = strlen(chrs);

    if(count > 1)
   	str = createStringObject(count,count-1,chrs);

    *rv = (int32) str;
    return NO_EXCEP;
}

/*
** Support function for getPropertyLen and getProperty0 functions.
*/
static void getKey(int32 ref, int8 *key, int keyLen)
{
    jchar *arr;
    jint i, count;

    count = getStringCount(ref);
    if (count > (jint)keyLen - 1)
        count = keyLen - 1;
    arr = getStringArray(ref);
    for (i=0; i < count; i++)
        key[i] = (int8)arr[i];
    key[count] = 0;
}

/****************************************************************************
* Returns the length of specified system property. If application native code
* doesn't support properties (vm_config_t.getProperty is NULL) then zero is
* returned.
*
* Java prototype:
*           static int getPropertyLen(String key);
*
****************************************************************************/
static int16 java_lang_System_getPropertyLen(int32 par[], int32 *rv)
{
    int8 key[64];
    int8 *prop;

    if (GET_REF(par[0]) == NULL)
        return EXCEP_NullPointer;

    *rv = 0;
    if (config->getProperty != NULL)
    {
        getKey(par[0], key, sizeof(key));
        prop = config->getProperty(key);
        if (prop != NULL)
            *rv = strLen(prop);
    }

    return NO_EXCEP;
}


/****************************************************************************
* Returns the length of specified system property. If application native code
* doesn't support properties (vm_config_t.getProperty is NULL) then zero is
* returned.
*
* Java prototype:
*           static boolean getProperty0(String key, byte[] dest);
*
****************************************************************************/
static int16 java_lang_System_getProperty0(int32 par[], int32 *rv)
{
    int8 key[64];
    int16 i, propLen;
    int8 *prop;
    jint destLen;
    jbyte *dest;

    if (GET_REF(par[0]) == NULL || GET_REF(par[1]) == NULL)
        return EXCEP_NullPointer;

    if (config->getProperty != NULL)
    {
        destLen = getArraySize(par[1]);
        dest = (jbyte *)getArray(par[1]);
        getKey(par[0], key, sizeof(key));
        prop = config->getProperty(key);
        propLen = (int16)strLen(prop);
        for (i=0; i < propLen && i < (int16)destLen; i++)
            dest[i] = (jbyte)prop[i];
        *rv = true;
    }
    else
        *rv = false;

    return NO_EXCEP;
}

/****************************************************************************
* Goes trough the current thread frame stack and fills in stack_trace
* buffer for the current thread.
*
* Java prototype:
*           void fillInStackTrace();
*
****************************************************************************/
static int16 java_lang_Throwable_fillInStackTrace(int32 par[], int32 *rv)
{
    fillInStackTrace();
    return NO_EXCEP;
}


/****************************************************************************
* Copies stack_trace buffer into provided destination buffer. Number of
* copied bytes is limited by the size of byte array Buffer or the size of
* stack_trace buffer.
*
* Java prototype:
*           void getStackTrace(byte[] Buffer);
*
****************************************************************************/
static int16 java_lang_Throwable_getStackTrace(int32 par[], int32 *rv)
{
    ref_t    *ref;
    arrhdr_t *dst;
    uint32   i;
    trace_tbl_t *dst_buf;

    /* verify destination buffer */
    ref = GET_REF(par[1]);
    if (ref == NULL)
        return EXCEP_NullPointer;

    dst = (arrhdr_t *)ref->ptr;
    if (dst->count < (uint32)(config->stack_trace->count * sizeof(trace_tbl_t)))
        return EXCEP_IndexOutOfBounds;

    dst_buf = (trace_tbl_t *)(dst + sizeof(arrhdr_t));
    for (i=0; i < config->stack_trace->count; i++)
        dst_buf[i] = config->stack_trace->trace[i];

    return NO_EXCEP;
}


/****************************************************************************
* Returns object reference as hashcode value for this object.
*
* Java prototype:
*           int hashCode();
*
****************************************************************************/
static int16 java_lang_Object_hashCode(int32 par[], int32 *rv)
{
    ref_t *ref = GET_REF(par[0]);
    *rv = (uint32)ref;
    return NO_EXCEP;
}


/****************************************************************************
* Creates a clone (an exact copy) of the object passed in trough the
* input parameters. Function returns a reference to the clone.
*
* Java prototype:
*           Object clone0(Object obj);
*
****************************************************************************/
static int16 java_lang_Object_clone0(int32 par[], int32 *rv)
{
    ref_t *ref, *clone;

    ref = GET_REF(par[1]);
    clone = allocHeapMem(ref->ptr->size);
    if (clone == NULL)
        return vm_excep;

    copyMem((uint8 *)clone->ptr + sizeof(objhdr_t), (uint8 *)ref->ptr + sizeof(objhdr_t), ref->ptr->size - sizeof(objhdr_t));
    clone->class_ptr = ref->class_ptr;
    clone->ptr->locks = 0;
    clone->ptr->flag = ref->ptr->flag;
    clone->ptr->thread_id = 0;

    *rv = PUT_REF(clone);

    return NO_EXCEP;
}


/****************************************************************************
* Converts float number into integer value.
*
* Java prototype:
*           static int floatToIntBits(float value);
*
****************************************************************************/
static int16 java_lang_Float_floatToIntBits(int32 par[], int32 *rv)
{
#if ENABLE_FLOAT
    *rv = par[0];
#endif
    return NO_EXCEP;
}


/****************************************************************************
* Converts integer number into float value.
*
* Java prototype:
*           static float intBitsToFloat(int bits);
*
****************************************************************************/
static int16 java_lang_Float_intBitsToFloat(int32 par[], int32 *rv)
{
#if ENABLE_FLOAT
    *rv = par[0];
#endif
    return NO_EXCEP;
}


/****************************************************************************
* Parses input stream of ASCII characters terminated with zero and returns
* float value.
*
* Java prototype:
*           static float parseFloat(byte[] s);
*
****************************************************************************/
static int16 java_lang_Float_parseFloat(int32 par[], int32 *rv)
{
#if ENABLE_FLOAT
    float32 val;
    arrhdr_t *arr;

    arr = (arrhdr_t *)(GET_REF(par[0]))->ptr;
    if (stringToFloat((uint8 *)arr + sizeof(arrhdr_t), &val))
      *rv = *((uint32 *)&val);
    else
      *rv = NaN;
#endif
    return NO_EXCEP;
}


/****************************************************************************
* Converts specified float value into ASCII characters.
*
* Java prototype:
*           static int toString0(float f, byte dest[]);
*
****************************************************************************/
static int16 java_lang_Float_toString0(int32 par[], int32 *rv)
{
#if ENABLE_FLOAT
    float32 fval;
    arrhdr_t *arr;

    *((int32 *)&fval) = par[0];
    arr = (arrhdr_t *)(GET_REF(par[1]))->ptr;
    if (arr->count > 0)
        *rv = floatToString(fval, (uint8 *)arr + sizeof(arrhdr_t), arr->count);
    else
        *rv = 0;
#endif
    return NO_EXCEP;
}


#if UNUSED

/****************************************************************************
* Clears all event counters.
*
* Input:    none
* Output:   none
* returns:  none
****************************************************************************/
static void clearEventCounters(void)
{
    uint16 i;

    for (i=0; i < config->events; i++)
        config->ev_counters[i] = 0;
}

/****************************************************************************
* Returns the number of event fire counters as specified in the configuration
* structure.
*
* Java prototype:       int getTimersCount();
****************************************************************************/
static int16 javax_events_Events_getEventsCount(int32 par[], int32 *rv)
{
    *rv = config->events;

    return NO_EXCEP;
}


/****************************************************************************
* Registers current thread with the VM's thread execution scheduler. When an event
* is recognised thread scheduler will instantly resume execution of this thread
* so that appropriate fire() methods of AsyncEvent classes can be invoked.
*
* Java prototype:       void eventThreadRegister();
****************************************************************************/
static int16 javax_events_Events_eventThreadRegister(int32 par[], int32 *rv)
{
    if (config->events > 0 && config->ev_counters != NULL)
    {
        ev_thread = findThread(GET_REF(par[0]));
        if (ev_thread == NULL)
            return EXCEP_NullPointer;

        ev_index = 0;
        clearEventCounters();
    }
    return NO_EXCEP;
}


/****************************************************************************
* Get an ID of the next pending event.
*
* Java prototype:       int getEvent();
****************************************************************************/
static int16 javax_events_Events_getEvent(int32 par[], int32 *rv)
{
    uint16 i;

    *rv = -1;
    if (ev_thread != NULL)
    {
        for (i=0; i < config->events; i++)
        {
            if (config->ev_counters[ev_index] > 0)
            {
                /*
                ** Found pending event fire counter -- get out of the loop and
                ** allow to set ev_index to the next fire counter.
                */
                i = config->events;
                *rv = ev_index;
                config->ev_counters[ev_index]--;
            }
            /* move to the next event fire counter */
            if (++ev_index == config->events)
                ev_index = 0;
        }
    }
    return NO_EXCEP;
}


/****************************************************************************
* This method will clear fire counter for the event specified by its ID.
* If ALL_EVENTS (-1) is specified then all fire counters are cleared.
*
* Java prototype:       void clearPendingFireCount(int eventId);
****************************************************************************/
static int16 javax_events_Events_clearPendingFireCount(int32 par[], int32 *rv)
{
    if (ev_thread != NULL)
    {
        if (par[1] >= 0)
            config->ev_counters[par[1]] = 0;
        else
            clearEventCounters();
    }
    return NO_EXCEP;
}


/****************************************************************************
* Returns id of the handler for which timeout value has expired. -1 is returned
* if no timer has expired.
*
* Java prototype:       int getExpiredId();
****************************************************************************/
static int16 javax_events_Timer_getExpiredId(int32 par[], int32 *rv)
{
    uint16 i;

    *rv = -1;
    if (sw_timers != NULL)
    {
        for (i=0; i < config->timers; i++)
        {
            if (sw_timers[i].flag == TMR_EXPIRED)
            {
                /* expired timer found -- clear the expired flag and return it's index */
                sw_timers[i].flag = 0;
                *rv = i;
                break;
            }
        }
    }
    return NO_EXCEP;
}


/****************************************************************************
* Starts the timer with specified ID and timeout value.
*
* Java prototype:       start0(int timerId, int timeout);
****************************************************************************/
static int16 javax_events_Timer_start0(int32 par[], int32 *rv)
{
    if (sw_timers != NULL)
    {
        sw_timers[par[1]].time = par[2];
        sw_timers[par[1]].flag = 0;
    }

    return NO_EXCEP;
}


/****************************************************************************
* Returns the number of software timers as specified in the configuration
* structure.
*
* Java prototype:       int getTimersCount();
****************************************************************************/
static int16 javax_events_Timer_getTimersCount(int32 par[], int32 *rv)
{
    *rv = config->timers;

    return NO_EXCEP;
}

#endif /* UNUSED */



int16 java_lang_Math_log(int32 param[], int32 *retval)
{
    value_t v;

    v.i = param[0];
    v.f = (float32)log((double)v.f);
    *retval = v.i;

    return NO_EXCEP;
}

int16 java_lang_Math_exp(int32 param[], int32 *retval)
{
    value_t v;

    v.i = param[0];
    v.f = (float32)exp((double)v.f);
    *retval = v.i;

    return NO_EXCEP;
}

int16 java_lang_Math_sin(int32 param[], int32 *retval)
{
    value_t v;

    v.i = param[0];
    v.f = (float32)sin((double)v.f);
    *retval = v.i;

    return NO_EXCEP;
}

int16 java_lang_Math_cos(int32 param[], int32 *retval)
{
    value_t v;

    v.i = param[0];
    v.f = (float32)cos((double)v.f);
    *retval = v.i;

    return NO_EXCEP;
}

/*====================================================
 * These are UVic ROSE "native" extension to this JVM
 * (UVic/CS MHMC 22/Aug/2002)
 *====================================================
 */


/****************************************************************************
* Starts execution of the new thread. There must be enough room in thr_table
* and run() method must be defined.
*
* Java prototype:
*           void start();
****************************************************************************/
static int16 uvic_posix_Thread_start(int32 par[], int32 *rv)
{
    createThread(GET_REF(par[0]),par[1]);
    return vm_excep;
}


/****************************************************************************
* Stops the execution of the thread. Information about this thread will be
* removed from thr_table.
*
* Java prototype:
*           void stop();
*
****************************************************************************/
static int16 uvic_posix_Thread_stop(int32 par[], int32 *rv)
{
    deleteThread(findThread(GET_REF(par[0])));
    return vm_excep;
}


/****************************************************************************
* Allows other threads to run by forcing the thread switch.
*
* Java prototype:
*           void yield();
*
****************************************************************************/
static int16 uvic_posix_Thread_yield(int32 par[], int32 *rv)
{
    Reschedule();
    return NO_EXCEP;
}

/****************************************************************************
* Change the current thread's level
*
* Java prototype:
*           static void set( int level );
*
****************************************************************************/
static int16 uvic_posix_Thread_set(int32 par[], int32 *rv)
{
      /* This is a "static" method; hence, the first parameter is 0 */
    SetLevel( par[0] );
    return NO_EXCEP;
}

/****************************************************************************
* Returns the current number of non-dead threads.
*
* Java prototype:
*           void yield();
*
****************************************************************************/
static int16 uvic_posix_Thread_count(int32 par[], int32 *rv)
{
    *rv = GetThreadCount();
    return NO_EXCEP;
}

/* Java prototype: int init_mutex() */
int16 uvic_posix_Mutex_init_mutex( int32 param[], int32 *retval)
{
    *retval = MutexInit();
    return NO_EXCEP;
}


/* Java prototype: void lock_mutex( int mutex ) */
int16 uvic_posix_Mutex_lock_mutex( int32 param[], int32 *retval)
{
    MutexLock( param[1] );
    return NO_EXCEP;
}


/* Java prototype: void unlock_mutex( int mutex ) */
int16 uvic_posix_Mutex_unlock_mutex( int32 param[], int32 *retval)
{
    MutexUnLock( param[1] );
    return NO_EXCEP;
}


/* Java prototype: int init_cond() */
int16 uvic_posix_Condition_init_cond( int32 param[], int32 *retval)
{
    *retval = CondInit();
    return NO_EXCEP;
}


/* Java prototype: void wait_cond(int cond, int mutex ) */
int16 uvic_posix_Condition_wait_cond( int32 param[], int32 *retval)
{
    CondWait( param[1], param[2] );
    return NO_EXCEP;
}


/* Java prototype: void signal_cond(int cond ) */
int16 uvic_posix_Condition_signal_cond( int32 param[], int32 *retval)
{
    CondSignal( param[1] );
    return NO_EXCEP;
}


/* Java prototype: void broadcast_cond(int cond ) */
int16 uvic_posix_Condition_broadcast_cond( int32 param[], int32 *retval)
{
    CondBroadcast( param[1] );
    return NO_EXCEP;
}


/* Java prototype: int init_sem( int i ) */
int16 uvic_posix_Semaphore_init_sem( int32 param[], int32 *retval)
{
    *retval = SemInit( param[1] );
    return NO_EXCEP;
}


/* Java prototype: void wait_sem( int sem ) */
int16 uvic_posix_Semaphore_wait_sem( int32 param[], int32 *retval)
{
    SemWait( param[1] );
    return NO_EXCEP;
}


/* Java prototype: void signal_sem( int sem ) */
int16 uvic_posix_Semaphore_signal_sem( int32 param[], int32 *retval)
{
    SemSignal( param[1] );
    return NO_EXCEP;
}

/*====================================================
 * Native methods for disk extension
 * (UVic/CS PMM 12/Oct/2002)
 *====================================================
 */

/* Java prototype: static void block_disk( int timeout ) */
int16 uvic_disk_Disk_block_disk( int32 param[], int32* retval)
{
	IOBlock(param[0]);
	return NO_EXCEP;
}

/* Java prototype: static void save_disk( int id, int[] buffer, int length ) */
int16 uvic_disk_Disk_save_disk(int32 param[], int32* retval)
{
    int32 id = param[0];
    int32 *disk = (int32 *)getArray(param[1]);
    int32 size = param[2];
	char buffer[300];
	if (sprintf(buffer, "vdisk_%03d", id) < 0)
	{
		fprintf(stderr, "Could not load buffer [%d].\n", id);
		return EXCEP_NullPointer;
	}
	FILE* fd = fopen(buffer, "w");
	if (fd == NULL)
	{
        fprintf(stderr, "Error opening to disk for saving [%d].\n", errno);
        return EXCEP_NullPointer;
    }
    int i;
    for (i = 0; i < size; i++)
    {
        fprintf(fd, "[%d] - %d\n", i, disk[i]);
        if (ferror(fd))
        {
            fprintf(stderr, "Error writing to file.\n");
            fclose(fd);
            return EXCEP_NullPointer;
        }
    }
    
    fclose(fd);
	return NO_EXCEP;
}

/* Java prototype: static void load_disk( int id, int[] buffer, int length ) */
int16 uvic_disk_Disk_load_disk(int32 param[], int32* retval)
{
    int32 id = param[0];
    int32 *disk = (int32 *)getArray(param[1]);
    int32 size = param[2];
	char buffer[300];
	if (sprintf(buffer, "vdisk_%03d", id) < 0)
	{
        fprintf(stderr, "Could not load buffer [%d].\n", id);
		return EXCEP_NullPointer;
	}
	FILE* fd = fopen(buffer, "r");
	if (fd == NULL)
    {
        fprintf(stderr, "Error opening to disk for reading [%d].\n", errno);
        return NO_EXCEP;
    }
    int i;
    int index = -1;
    int value = -1;
    for (i = 0; i < size; i++)
    {
        fscanf(fd, "[%d] - %d\n", &index, &value);
        if (ferror(fd))
        {
            fprintf(stderr, "Error reading file.\n");
            fclose(fd);
            return EXCEP_NullPointer;
        }
        disk[index] = value;
    }
    
    fclose(fd);
    return NO_EXCEP;
}


/*=================================
 * End UVic ROSE "native" extension
 *=================================
 */

/*
** System native method lookup table.
** IMPORTANT:
**   Order of methods this table must exactly match the method order in
**   the ClassLinker.control file. Otherwise, incorrect native method might
**   be invoked and can eventually cause the system to crash!
*/
const native_func_ptr sys_native_tbl[] =
{
    /* java.lang.System methods */
    java_lang_System_arraycopy,
    java_lang_System_freeMemory,
    java_lang_System_totalMemory,
    java_lang_System_gc,
    java_lang_System_currentTimeMillis,
    java_lang_System_print,
    java_lang_System_println,
    java_lang_System_readln,
    java_lang_System_getPropertyLen,
    java_lang_System_getProperty0,
    /* java.lang.Throwable methods */
    java_lang_Throwable_fillInStackTrace,
    java_lang_Throwable_getStackTrace,
    /* java.lang.Object methods */
    java_lang_Object_hashCode,
    java_lang_Object_clone0,
    /* java.lang.Float methods */
    java_lang_Float_floatToIntBits,
    java_lang_Float_intBitsToFloat,
    java_lang_Float_parseFloat,
    java_lang_Float_toString0,

#if UNUSED
    /* javax.events package */
    javax_events_Events_getEventsCount,
    javax_events_Events_eventThreadRegister,
    javax_events_Events_getEvent,
    javax_events_Events_clearPendingFireCount,
    javax_events_Timer_getExpiredId,
    javax_events_Timer_start0,
    javax_events_Timer_getTimersCount,
#endif /* UNUSED */

    /*=============================*/
    /* uvic.posix package          */
    /* (UVic/CS MHMC 22/Aug/2002)  */
    /*=============================*/
    uvic_posix_Thread_start,
    uvic_posix_Thread_stop,
    uvic_posix_Thread_yield,
    uvic_posix_Thread_set,
	uvic_posix_Thread_count,
    uvic_posix_Mutex_init_mutex,
    uvic_posix_Mutex_lock_mutex,
    uvic_posix_Mutex_unlock_mutex,
    uvic_posix_Condition_init_cond,
    uvic_posix_Condition_wait_cond,
    uvic_posix_Condition_signal_cond,
    uvic_posix_Condition_broadcast_cond,
    uvic_posix_Semaphore_init_sem,
    uvic_posix_Semaphore_wait_sem,
    uvic_posix_Semaphore_signal_sem,
	
	/*=============================*/
    /* uvic.disk package          */
    /* (UVic/CS PMM 12/Nov/2002)  */
    /*=============================*/
	uvic_disk_Disk_block_disk,
    uvic_disk_Disk_save_disk,
    uvic_disk_Disk_load_disk,
	
};
