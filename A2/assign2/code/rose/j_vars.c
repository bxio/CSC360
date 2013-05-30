/**************************************************************************************
*  j_vars.c
*
*  This file defines all global variables that are used by the simpleRTJ.
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
#include <windows.h>

uint16      excep_count;                    /* number of run-time exception objects */
uint16      vm_excep;                       /* JVM runtime exception */
uint16      vm_excep1;                      /* saved JVM runtime exception */
uint16      vm_status;                      /* VM status register */
uint16      thr_count;                      /* number of active threads in the thr_table */
uint16      vm_args_count;                  /* number of method arguments */

ref_t      *excep_refs[RUNTIME_EXCEPTIONS]; /* allocate for runtime exception references */
uint8      *vm_pc;                          /* program counter for current method */
value_t    *vm_sp;                          /* stack pointer for current method */
uint32      frame_size;                     /* max. size of the method frames */
uint32      inst_size;                      /* max. size of the class instance */

vm_config_t *config;                        /* VM configuration structure */
classlock_t *class_locks;                   /* class locks acquired by synchronized static methods */

thread_t    *ev_thread;                     /* pointer to thread handling asynch. events */
uint16       ev_index;                      /* index of the next event fire counter that will be examined */
timer_t     *sw_timers;                     /* counters for software timers */
uint8      *heap_ptr;                       /* ptr. to next free heap memory */
uint8      *frame_ptr;                      /* ptr. to next free frames memory */

value_t    *vm_args;                        /* pointer to args in calling meth. frame stack */
value_t     vm_retval;                      /* 32-bit return value */
ref_t      *vm_appref_start;                /* start of user application references */
class_t   **clinit_list;                    /* start of list of classes with executed <clinit> methods */
value_t     *static_start;                  /* start of static fields */
strings_t   *strings;                       /* start of strings table */
ref_t       *refs;                          /* start of system ref. table */
ref_t       *app_refs;                      /* start of user appl. ref. table, follows system ref. table */
uint8       *objdata_start;                 /* start of objects data heap memory */
uint8       *bc_item;                       /* set by some bytecodes to corresp. methods or classes */

/* ========= Globals used by ROSE ========= */

thread_t    *thr_active;       /* pointer to thr_table for active thread */
thread_t    *threads;          /* threads table */
mutex_t     *mutexes;          /* Mutex table */
condition_t *conditions;       /* Condition table */
semaphore_t *semaphores;       /* Semaphore table */

long long      vm_time_ms;     /* total msec since VM starts */
long long      vm_time_frq;    /* High Performance Counter frequency */
LARGE_INTEGER  vm_timer_start;/* performance counter when VM starts */

/* ========= end =========== */


value_t      main_args;                     /* stores the reference to String array that is passed to main method */

#if ENABLE_STRING_EXCEPTIONS
/*
** Each entry has the following format:
**       string type(0xC0) + string length
**       string hashcode
**       exception text (padded with zeroes for even length)
*/
#if LSB_FIRST
  const uint8 str_InternalError[]             = { 0x0E, 0xC0, 0x00, 0x00, 0x86, 0x19, 'I','n','t','e','r','n','a','l',' ','E','r','r','o','r' };
  const uint8 str_OutOfMemoryError[]          = { 0x09, 0xC0, 0x00, 0x00, 0xC2, 0x8F, 'N','o',' ','m','e','m','o','r','y', 0 };
  const uint8 str_NoSuchMethodError[]         = { 0x0E, 0xC0, 0x00, 0x00, 0x1E, 0xF0, 'N','o',' ','m','e','t','h','o','d',' ','d','e','f','.' };
  const uint8 str_AbstractMethodError[]       = { 0x0F, 0xC0, 0x00, 0x00, 0xE1, 0x51, 'A','b','s','t','r','a','c','t',' ','m','e','t','h','o','d',0 };
  const uint8 str_NullPointerException[]      = { 0x0C, 0xC0, 0x00, 0x00, 0x29, 0x97, 'N','u','l','l',' ','p','o','i','n','t','e','r' };
  const uint8 str_IndexOutOfBoundsException[] = { 0x09, 0xC0, 0x00, 0x00, 0x97, 0x22, 'B','a','d',' ','i','n','d','e','x',0 };
  const uint8 str_ArrayStoreException[]       = { 0x0B, 0xC0, 0x00, 0x00, 0x1C, 0x20, 'A','r','r','a','y',' ','s','t','o','r','e',0 };
  const uint8 str_ArithmeticException[]       = { 0x0B, 0xC0, 0x00, 0x00, 0x99, 0x8A, 'A','r','i','t','h','m','e','n','t','i','c',0 };
  const uint8 str_ClassCastException[]        = { 0x0A, 0xC0, 0x00, 0x00, 0x64, 0xE8, 'C','l','a','s','s',' ','c','a','s','t' };
  const uint8 str_NegativeArrSizeException[]  = { 0x0E, 0xC0, 0x00, 0x00, 0x63, 0x07, 'N','e','g','a','t','i','v','e',' ','a','r','r','a','y' };
  const uint8 str_IllegalMonStateException[]  = { 0x10, 0xC0, 0x00, 0x00, 0x6B, 0x76, 'B','a','d',' ','M','o','n','i','t','.',' ','S','t','a','t','e' };
#else
  const uint8 str_InternalError[]             = { 0x00, 0x00, 0xC0, 0x0E, 0x19, 0x86, 'I','n','t','e','r','n','a','l',' ','E','r','r','o','r' };
  const uint8 str_OutOfMemoryError[]          = { 0x00, 0x00, 0xC0, 0x09, 0x8F, 0xC2, 'N','o',' ','m','e','m','o','r','y',0 };
  const uint8 str_NoSuchMethodError[]         = { 0x00, 0x00, 0xC0, 0x0E, 0xF0, 0x1E, 'N','o',' ','m','e','t','h','o','d',' ','d','e','f','.' };
  const uint8 str_AbstractMethodError[]       = { 0x00, 0x00, 0xC0, 0x0F, 0x51, 0xE1, 'A','b','s','t','r','a','c','t',' ','m','e','t','h','o','d',0 };
  const uint8 str_NullPointerException[]      = { 0x00, 0x00, 0xC0, 0x0C, 0x97, 0x29, 'N','u','l','l',' ','p','o','i','n','t','e','r' };
  const uint8 str_IndexOutOfBoundsException[] = { 0x00, 0x00, 0xC0, 0x09, 0x22, 0x97, 'B','a','d',' ','i','n','d','e','x',0 };
  const uint8 str_ArrayStoreException[]       = { 0x00, 0x00, 0xC0, 0x0B, 0x20, 0x1C, 'A','r','r','a','y',' ','s','t','o','r','e',0 };
  const uint8 str_ArithmeticException[]       = { 0x00, 0x00, 0xC0, 0x0B, 0x8A, 0x99, 'A','r','i','t','h','m','e','n','t','i','c',0 };
  const uint8 str_ClassCastException[]        = { 0x00, 0x00, 0xC0, 0x0A, 0xE8, 0x64, 'C','l','a','s','s',' ','c','a','s','t' };
  const uint8 str_NegativeArrSizeException[]  = { 0x00, 0x00, 0xC0, 0x0E, 0x07, 0x63, 'N','e','g','a','t','i','v','e',' ','a','r','r','a','y' };
  const uint8 str_IllegalMonStateException[]  = { 0x00, 0x00, 0xC0, 0x10, 0x76, 0x6B, 'B','a','d',' ','M','o','n','i','t','.',' ','S','t','a','t','e' };
#endif

const uint8 *const excep_str_table[] =
{
    str_InternalError,
    str_OutOfMemoryError,
    str_NoSuchMethodError,
    str_AbstractMethodError,
    str_NullPointerException,
    str_IndexOutOfBoundsException,
    str_ArrayStoreException,
    str_ArithmeticException,
    str_ClassCastException,
    str_NegativeArrSizeException,
    str_IllegalMonStateException
};
#endif

#if ENABLE_TEXT_NOTIFY
const char *const notify_msg[] =
{
    "Initialising VM",
    "Starting Java app",
    "Application verify fail",
    "Insuf. heap"
};

const char *const excep_msg[] =
{
    "Internal",
    "Memory",
    "No such method",
    "Abstract method",
    "Null pointer",
    "Index out of bounds",
    "Array store",
    "Arithmetic",
    "Class cast",
    "Negative array size",
    "Illegal monitor state"
};

const char *const mem_excep_msg[] =
{
    "No heap",
    "No refs",
    "No frames",
    "No strings",
    "Mem alloc"
};

const char *const int_excep_msg[] =
{
    "Invalid run-time excep. count",
    "Program has ended",
    "Invalid bytecode",
    "Thread setup failed",
    "Thread limit",
    "Unhandled excep.",
    "Unsupported data type",
    "GC failed",
    "Args create fail"
};
#endif



