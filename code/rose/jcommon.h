/**************************************************************************************
*  jcommon.h
*
*  Forward definitions of all global variables and functions.
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

#ifndef _GLOBALS_H
#define _GLOBALS_H

#include "jvm.h"
#include <windows.h>

/*
** Global variables definitions.
*/
extern uint16      excep_count;
extern uint16      vm_excep;
extern uint16      vm_excep1;
extern uint16      vm_status;
extern uint16      thr_count;
extern uint16      thr_switch;
extern uint16      vm_args_count;

extern ref_t       *excep_refs[RUNTIME_EXCEPTIONS];

  /* VM context */
extern uint8       *vm_pc;
extern value_t     *vm_sp;

extern uint32      frame_size;
extern uint32      inst_size;
extern vm_config_t *config;
extern classlock_t *class_locks;
extern thread_t    *ev_thread;
extern uint16      ev_index;
extern timer_t     *sw_timers;
extern uint8       *heap_ptr;
extern uint8       *frame_ptr;
extern value_t     *vm_args;
extern class_t     **clinit_list;
extern value_t     *static_start;
extern strings_t   *strings;
extern ref_t       *refs;
extern ref_t       *app_refs;
extern uint8       *objdata_start;
extern uint8       *bc_item;
extern value_t      main_args;

  /* ===== Used by ROSE ====== */

extern long long   vm_time_ms;         /* msec since VM starts */
extern long long   vm_time_frq;        /* freq. of high performance counter */
extern LARGE_INTEGER  vm_timer_start;  /* start time of H.P. counter */
extern thread_t    *thr_active;
extern thread_t    *threads;
extern mutex_t     *mutexes;
extern condition_t *conditions;
extern semaphore_t *semaphores;

  /* ===== end ====== */

extern const native_func_ptr sys_native_tbl[];
extern const bc_handler bytecode_table[];

#define VM_STATIC_END   ((uint8 *)strings)
#define VM_STRINGS_END  ((uint8 *)threads)
#define VM_THREADS_END  ((uint8 *)refs)
#define VM_REFS_END     ((uint8 *)objdata_start)

/*
** Function prototypes from J_GC
*/
#if ENABLE_GC
bool walkAll(void);
void compactHeapForced(void);
#endif

/*
** Function prototypes from J_VM
*/
void removeFromStrTable(ref_t *string);
ref_t *createObject(class_t *cl);
bool run(method_t *);
bool run_init(ref_t *ref, ref_t *strRef);
void fillInStackTrace(void);
int32 expandByte(int8 bval);
int32 expandShort(int16 sval);
void setupMethArgs(method_t *method);
ref_t *findInStrTable(uint16 hash);
ref_t *createStringObject(uint16 hash, uint32 size, int8 *src);
bool isSubclass(class_t *S, class_t *T);
bool handle_excep(void);
jint getStringCount(int32 param);
jchar *getStringArray(int32 param);
jint getArraySize(int32 param);
void *getArray(int32 param);
ref_t *doSimpleArray(uint8 type, uint32 length);
uint32 strLen(int8 *str);
bool run_clinit(class_t *class_ptr);

/*
** Function prototypes from J_BCODE
*/
uint8 *getLocation(uint16 index);

  /*============================*/
  /* UVic ROSE addition         */
  /* (UVic/CS MHMC 22/Aug/2002) */
  /*============================*/

/*
 * Function prototypes from Timer
 */
bool InitVMTimer(void);
void StartVMTimer(void);
void UpdateVMTimer(void);
int32 GetVMTime();
/*
 * Function prototypes from J_THREAD
 */
bool InitROSE(void);
void AddReady(thread_t* thread, bool front);
void PreemptIfNecessary(void);
void Dispatch(void);
void Reschedule(void);       /* yield() */
void SetLevel(int32 level);
void VMTick(void);

  /*============================*/
  /* UVic ROSE native extension */
  /* (UVic/CS MHMC 22/Aug/2002) */
  /*============================*/
int32 MutexInit();
void  MutexLock( int32 mutex_id );
void  MutexUnLock( int32 mutex_id );
int32 CondInit();
void  CondWait( int32 cond_id, int32 mutex_id );
void  CondSignal( int32 cond_id );
int32 SemInit( int32 i );
void  SemWait( int32 sem_id );
void  SemSignal( int32 sem_id );



  /* ===== End ===== */

bool initMainThread(void);
void deleteThread(thread_t *thread);
bool createThread(ref_t *ref, int32 level);
thread_t *findThread(ref_t *ref);

void acquireObjectLock(thread_t *thread, ref_t *ref);
void acquireClassLock(thread_t *thread);
bool releaseLocks(void);
bool releaseObjectLock(ref_t *ref);
thread_t *getWaitingThread(ref_t *ref);

/*
** Function prototypes from J_MEM
*/
void clearMem(uint8 *mem, int32 len);
void copyMem(uint8 *to, uint8 *from, int32 len);
ref_t *findFreeRef(void);
ref_t *allocHeapMem(uint32 size);
frame_t *createFrame(frame_t* parent, method_t *method);
bool releaseFrame(value_t *retval);

/*
** Function prototypes from J_FLOAT
*/
#if ENABLE_FLOAT
#define NaN   0x7FC00000
float32 floatAdd(float32 a, float32 b);
float32 floatSub(float32 a, float32 b);
float32 floatMul(float32 a, float32 b);
float32 floatDiv(float32 a, float32 b);
float32 floatMod(float32 a, float32 b);
bool floatEQ(float32 a, float32 b);
bool floatNE(float32 a, float32 b);
bool floatGT(float32 a, float32 b);
bool isNaN(float32 value);
float32 floatNeg(float32 value);
float32 int2float(int32 value);
int32 float2int(float32 value);
bool stringToFloat(uint8 *buff, float32 *value);
int16 floatToString(float value, uint8 *buff, uint32 length);
#endif

#if ENABLE_TEXT_NOTIFY
extern const char *const notify_msg[];
extern const char *const excep_msg[];
extern const char *const mem_excep_msg[];
extern const char *const int_excep_msg[];
#endif

#if ENABLE_STRING_EXCEPTIONS
extern const uint8 *const excep_str_table[];
#endif

#endif

