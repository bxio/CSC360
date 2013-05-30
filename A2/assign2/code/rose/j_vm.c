/**************************************************************************************
*  j_vm.c
*
*  This file contains main bytecode execution loop as well as various support
*  functions.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "jvm.h"
#include "jcommon.h"

#define RETURN_FAIL   0
#define METH_RETURN   1
#define RUN_RETURN    2

extern native_func_ptr native_tbl[];

static void vmStart(vm_config_t *cfg);
static int16 getExceptionIndex(int16 *detail);
static bool createExceptions(void);
static bool execAll_clinit(void);
static bool check_clinit(class_t *class_ptr);
static void mark_clinit(uint8 *clinit);
static bool verifyApp(void);
static void usrFunc(void (*f)(uint16, char *), uint16 code);
static void setField(ref_t *ref, field_t *field, uint32 value);
static bool handle_native(void);
static int16 handle_return(frame_t *init_frame);
static bool handle_new(void);
static bool handle_throw(ref_t *excep);
static uint8 *findExcepEntry(ref_t *excep_ref);
static bool addToStrTable(uint16 hash, ref_t *string);
static uint32 getSysTime(void);
static void updateEventCounter(uint16 index);
static void notify(uint16 code, char *msg);
static int8 *getProperty(int8 *key);

static vm_config_t vmconfig;

#if ENABLE_MAIN_ARGS
static bool createMainArgs(void);
#endif

/****************************************************************************
* Main entry point for virtual machine. The machine configuration is set 
* and control is passed to the interpreter.
*
* Input:	command line parameters
* Output:	none
* Return:	error code
****************************************************************************/ 
int main(int argc, char* argv[])
{
	int		result	= 0;
	FILE*		appFile = NULL;
	uint8*		appCode = NULL;
	long		appSize = 0;
	uint8* 		heap 	= NULL;
	uint16*		events 	= NULL;
	uint16		ts;
	

	if (!InitVMTimer()) return(0);

	/* check parameters */
	if(argc < 3)
	{
		puts("Usage: rose <quantum> <java app>.bin");
		puts("e.g. rose 10 test.bin");
		goto EXIT_ERROR;
	}

        ts = (uint16)atoi(argv[1]);

	appFile = fopen(argv[2],"rb");
	if(appFile == NULL)
	{
		printf("Unable to open %s\n",argv[1]);
		goto EXIT_ERROR;
	}

	/* allocate heap and event space */
	heap = malloc(sizeof(uint8) * MAX_HEAPSIZE);
	if(heap == NULL) 
	{
		puts("Unable to allocate heap space");
		goto EXIT_ERROR;
	}
	
	events =  malloc(sizeof(uint16) * MAX_EVENTS);
	if(events == NULL) 
	{
		puts("Unable to allocate event space");
		goto EXIT_ERROR;
	}

	/* find the size of the java app and allocate space for it */
	fseek(appFile,0,SEEK_END);

	appSize = ftell(appFile);
	if(appSize <= 0)
	{
		puts("Unable to load java app");
		goto EXIT_ERROR;
	}

	/* load the java app into memory */
	appCode = malloc(sizeof(uint8) * appSize);
	if(appCode == NULL)
	{
		printf("Unable to allocate %ld bytes for java app\n",appSize);
		goto EXIT_ERROR;
	}

	fseek(appFile,0,SEEK_SET);
	if(fread(appCode,1,appSize,appFile) != appSize)
	{
		printf("Unable to load java app %s\n",argv[1]);
		goto EXIT_ERROR;
	}

	/* initialize the config parameters */
	vmconfig.time_slice 	= ts;
	vmconfig.references 	= MAX_REFERENCES;
	vmconfig.strings	= MAX_STRINGS;
	vmconfig.threads	= MAX_THREADS;
	vmconfig.timers		= MAX_TIMERS;
	vmconfig.events		= MAX_EVENTS;
	vmconfig.priLevel	= MAX_PRI_LEVELS;
	vmconfig.app_start	= appCode;
	vmconfig.heap_start	= heap;
	vmconfig.heap_end	= &heap[MAX_HEAPSIZE-1];
	vmconfig.ev_counters 	= events;
	vmconfig.native_tbl 	= native_tbl;
	vmconfig.usrNotify	= notify;
#if ENABLE_WDT
	vmconfig.clearWdt	= clearWdt;
#endif
	vmconfig.getProperty = getProperty;
#if ENABLE_MAIN_ARGS
	vmconfig.args_count	= argc - 2;
	vmconfig.args	= (int8**)&argv[2];
#endif

	vmStart(&vmconfig);  /* boot up the Java VM */

	goto EXIT_NOERROR;
EXIT_ERROR:
	result = 1;
EXIT_NOERROR:
	if(appFile 	!= NULL) fclose(appFile);
	if(heap 	!= NULL) free(heap);
	if(events 	!= NULL) free(events);
	if(appCode	!= NULL) free(appCode);

	return result;
}

/****************************************************************************
* Initialises all variables and starts the virtual machine interpreter.
* Order of data on the application heap:
* 1. stack trace
* 2. static fields
* 3. strings table
* 4. threads table
* 5. mutexes table
* 6. conditions table
* 7. references table
* 8. objects data
*
* Input:        config  pointer to configuration data
* Output:       none
* Return:       none
****************************************************************************/
void vmStart(vm_config_t *cfg)
{
    int16 excep, detail;
    uint8 *temp_ptr;
    bool retval;

    /* setup pointers in configuration structure */
    config = cfg;
    config->vmGetStringCount = getStringCount;
    config->vmGetStringArray = getStringArray;
    config->vmGetArraySize = getArraySize;
    config->vmGetArray = getArray;
    config->vmGetSysTime = getSysTime;
    config->vmUpdateEventCounter = updateEventCounter;

/*
    usrFunc(config->usrNotify, NOTIFY_VM_INIT);
*/

    /* Verify that the application is present */
    if (!verifyApp())
    {
        usrFunc(config->usrNotify, NOTIFY_VM_NOAPP);
        return;
    }

    printf( "Starting ROSE v.1.1 (Quantum = %d msec) ...\n", config->time_slice);

    /* Allocate memory on the heap for class locks */
    heap_ptr = (uint8 *)ALIGN4(config->heap_start);
    class_locks = (classlock_t *)heap_ptr;
    heap_ptr += (APP->class_count * sizeof(classlock_t));

    /* Allocate memory on the heap for software timers */
    sw_timers = NULL;
    if (config->timers > 0)
    {
        sw_timers = (timer_t *)heap_ptr;
        heap_ptr += (config->timers * sizeof(timer_t));
    }

    /* 1. Allocate memory on the heap for stack trace buffer */
    config->stack_trace = (trace_t *)heap_ptr;
    heap_ptr += STACKTRACE_SIZE;

    /* 2. Allocate memory on the heap for static fields */
    static_start = (value_t *)heap_ptr;
    heap_ptr += APP->static_size;

    /* 3. Allocate memory on the heap for strings table */
    strings = (strings_t *)heap_ptr;
    heap_ptr += config->strings * sizeof(strings_t);

    /* 4. Allocate memory on the heap for threads table */
    threads = (thread_t *)heap_ptr;
    heap_ptr += config->threads * sizeof(thread_t);

    /* 5. Allocate memory on the heap for mutexes table */
    mutexes = (mutex_t *)heap_ptr;
    heap_ptr += MAX_MUTEXES * sizeof(mutex_t);

    /* 6. Allocate memory on the heap for conditions table */
    conditions = (condition_t *)heap_ptr;
    heap_ptr += MAX_CONDITIONS * sizeof(condition_t);

    /* 7. Allocate memory on the heap for conditions table */
    semaphores = (semaphore_t *)heap_ptr;
    heap_ptr += MAX_SEMAPHORES * sizeof(semaphore_t);

    /* 8. Allocate memory on the heap for object references */
    refs = (ref_t *)heap_ptr;
    app_refs = refs;
    heap_ptr += config->references * sizeof(ref_t);

    /* 9. Thre rest of the heap is for objects data */
    objdata_start = heap_ptr;

    /* check if remaining heap is big enough */
    if (heap_ptr + MIN_OBJDATA_SIZE > config->heap_end)
    {
        usrFunc(config->usrNotify, NOTIFY_INSUF_HEAP);
        return;
    }
    clearMem(config->heap_start, (int32)(heap_ptr - config->heap_start));

    /* Initialise end of frames ptr., frame and instance sizes */
    frame_ptr = config->heap_end;
    frame_size = (uint16)(APP->frame_size + FRAME_HDR_SIZE);
    inst_size = (uint16)(APP->inst_size + sizeof(classhdr_t));

    vm_excep = 0;


    /* initialise ROSE runtime concurrency support */
    if (InitROSE())
    {
        /* Create all run-time exception objects */
        if (createExceptions())
        {
            /*
            ** Adjust start of references table. All references allocated so
            ** far are system ones and should not be used during GC.
            */
            app_refs = findFreeRef();
            if (app_refs != NULL)
            {
                /*
                ** Run class initialisation methods <clinit>.
                ** Before running <clinit> methods allocate space at the end of heap
                ** for table listing classes for which <clinit> has been run.
                ** When finished, the heap end is restored.
                */
                temp_ptr = config->heap_end;
                config->heap_end -= (APP->class_count * sizeof(clinit_list[0]));
                clinit_list = (class_t **)config->heap_end;
                frame_ptr = config->heap_end;

                clearMem(frame_ptr, temp_ptr - frame_ptr);
                retval = execAll_clinit();
                config->heap_end = temp_ptr;
                frame_ptr = temp_ptr;
                if (retval)
                {
                    /* run <init>() on thread with main() method */
                    if (initMainThread())
                    {
/*
                        usrFunc(config->usrNotify, NOTIFY_VM_START);
*/

                        /*
                        ** Initialise remaining params and start user applic. running
                        ** by invoking the main() method.
                        */

			StartVMTimer();

#if  ENABLE_MAIN_ARGS
                        if (!createMainArgs())
                        {
                            if (vm_excep == 0)
                                vm_excep = INTERR_ArgsCreate;
                        }
                        else
                        {
                            vm_status = 0;
                            run(METHOD_LOC(APP->main));
                        }
#else
                        vm_args_count = 0;
                        vm_status = 0;
                        run(METHOD_LOC(APP->main));
#endif
                    }
                }
            }
            else
                vm_excep = MEMERR_NoRefs;
        }
    }

    /* execution should get here only if exception was generated */
/*
    if (vm_excep == 0)
        vm_excep = INTERR_ProgEnd;

    excep = getExceptionIndex(&detail);
    usrFunc(config->usrNotify, (uint16)((excep << 8) | detail));
*/
    if (vm_excep != 0) {
      excep = getExceptionIndex(&detail);
      usrFunc(config->usrNotify, (uint16)((excep << 8) | detail));
    } else 
      printf( "\nROSE terminated successfully (Time = %lld msec).\n", vm_time_ms );

}


/****************************************************************************
* Subroutine will call FillInStackTrace to get list of methods on the
* current thread stack. Then, it will convert jvm_excep value into index to
* run-time exception table. For OutOfMemory and Internal error detail code
* is derived from jvm_excep.
*
* Input:        none
* Output:       detail  detail code for OutOfMemory and Internal errors
* Return:       index to exception table
****************************************************************************/
static int16 getExceptionIndex(int16 *detail)
{
    *detail = 0;
    if (vm_excep >= INTERR)
    {
        *detail = vm_excep;
        if (vm_excep >= MEMERR)
            return ERROR_OutOfMemory;

        return ERROR_Internal;
    }

    return vm_excep;
}


/****************************************************************************
* Subroutine clears the stack trace buffer and then it fills it with
* stack trace. Stack trace contains a list of excecuted methods and
* corresponding values of PC counters.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
void fillInStackTrace(void)
{
    int16 i = 0;
    frame_t *frame;

    frame = thr_active->curr_frame;
    if (frame != NULL)
    {
        frame->pc = vm_pc;
        frame->sp = vm_sp;

        config->stack_trace->count = 0;
        while (frame != NULL && i < MAX_TRACES)
        {
            config->stack_trace->trace[i].method = REL_LOC(frame->method);
            config->stack_trace->trace[i].pc = REL_LOC(frame->pc);
            frame = frame->prev;
            config->stack_trace->count++;
            i++;
        }
    }
}


/****************************************************************************
* Creates all run-time exception objects. For each exception creates
* a String object and optionally initialised to appropriate excep. text.
* It then calls constructor <init>().
* for each exception object.
*
* Input:        none
* Output:       none
* Return:       true if successful, otherwise false is returned.
****************************************************************************/
static bool createExceptions(void)
{
    int16   i;
    excep_t *elist;
    ref_t   *ref;
    const_t *cons;
#if ENABLE_STRING_EXCEPTIONS
    ref_t   *strRef;
#endif

    /*
    ** RUN_SPECIAL_FLAG is set so the run() will return when it
    ** encounters return bytecode.
    */
    vm_status |= RUN_SPECIAL_FLAG;

    elist = (excep_t *)ABS_LOC(APP->excep_list);
    if (elist->count != RUNTIME_EXCEPTIONS)
    {
        vm_excep = INTERR_RuntimeExcepCount;
        return false;
    }

    for (i=0; i < RUNTIME_EXCEPTIONS; i++)
    {
        ref = createObject(CLASS_LOC(elist->class_ptr[i]));
#if ENABLE_STRING_EXCEPTIONS
        cons = (const_t *)excep_str_table[i];
        strRef = createStringObject(CONST_STR_HASH(cons), CONST_LEN(cons->type_len), CONST_STR(cons));
        if (ref == NULL || strRef == NULL || run_init(ref, strRef) == false)
            return false;
#else
        if (ref == NULL || run_init(ref, NULL) == false)
            return false;
#endif
        excep_refs[i] = ref;
    }

    return true;
}


/****************************************************************************
* Executes <clinit> methods for all classes in the class table.
*
* Input:        none
* Output:       none
* Return:       true if successful, otherwise false is returned.
****************************************************************************/
static bool execAll_clinit(void)
{
    uint16 i;

    /*
    ** RUN_SPECIAL_FLAG is set so the Run subroutine will
    ** return when it encounters return bytecode.
    */
    vm_status |= RUN_SPECIAL_FLAG;

    /* if no classes present -- just return */
    if (APP->class_count == 0)
        return true;

    for (i=0; i < (uint16)APP->class_count; i++)
    {
        if (APP->class_tbl[i] != 0)
        {
            if (run_clinit(CLASS_LOC(APP->class_tbl[i])) == false)
                return false;
        }
    }

    return true;
}


/****************************************************************************
* Runs <clinit> method for specified class. If class is a subclass then
* the super class' <clinit> is run first by recursively calling itself.
*
* Input:        class_ptr   pointer to class structure
* Output:       none
* Return:       true if successful, otherwise false is returned.
****************************************************************************/
bool run_clinit(class_t *class_ptr)
{
    /* skip array type classes */
    if (class_ptr->flags & ACC_ARRAY_TYPE)
        return true;

    /* run superclass' <clinit> if exists */
    if (class_ptr->super != NULL)
    {
        if (run_clinit(CLASS_LOC(class_ptr->super)) == false)
            return false;
    }

    if (class_ptr->clinit != NULL && check_clinit(class_ptr))
    {
        vm_args_count = 0;
        mark_clinit(class_ptr->clinit);         /* mark this <clinit> as being executed */

        if (run(METHOD_LOC(class_ptr->clinit)) == false)
            return false;
    }
    return true;
}


/****************************************************************************
* Checks if the specified <clinit> has been executed. It goes trough the
* list of executed <clinit> methods and checks if the specified address is
* listed or not.
*
* Input:        class_ptr   pointer to class structure
* Output:       none
* Return:       true if <clinit> has not been executed,
*               otherwise false is returned.
****************************************************************************/
static bool check_clinit(class_t *class_ptr)
{
    uint16 i;

    for (i=0; i < (uint16)APP->class_count; i++)
    {
        if (clinit_list[i] == (class_t *)class_ptr->clinit)
            return false;
    }

    return true;
}


/****************************************************************************
* Includes specified <clinit> address in the list of executed <clinit>
* methods. It then updates clinit_count variable.
*
* Input:        clinit   location of <clinit>
* Output:       none
* Return:       none
****************************************************************************/
static void mark_clinit(uint8 *clinit)
{
    uint16 i;

    for (i=0; i < (uint16)APP->class_count; i++)
    {
        if (clinit_list[i] == NULL)
        {
            clinit_list[i] = (class_t *)clinit;
            return;
        }
    }
}


/****************************************************************************
* Executes the method's bytecodes.
* First, it creates new method frame and initialises pc and sp counters. Then,
* it executes all bytecodes of the method.
* Global variable bc_action can take following values after executing
* the bytecode:
*       0               - no special action required
*       ACTION_RETURN   - method should finish its execution (return statement
*                         encountered)
*       ACTION_INVOKE   - invoke another java method
*       ACTION_NATIVE   - invoke native method
*       ACTION_NEW      - create instance of class
*       ACTION_THROW    - checked exception has been thrown
*
* If on return from method there are no other frames on the stack then the
* current thread is terminated. If  there are no other threads to run then
* EXCEP_ProgEnd is generated.
*
* If RUN_SPECIAL_FLAG is set in vm_status then this method will return after
* encountering the return bytecodes.
*
* Input:        method   pointer to method to execute
* Output:       none
* Return:       true if successful, otherwise false is returned.
****************************************************************************/
bool run(method_t *method)
{
    int16    		i, bc_action;
    method_t 		*exec_method = method;
    frame_t 		*init_frame = NULL;
    bc_handler 		handler;
    bool     		go = true;
    long long  expired_ms;

    /* method execution loop */
    while (true)
    {
        thr_active->curr_frame = createFrame(thr_active->curr_frame,exec_method);
        if(thr_active->curr_frame == NULL)
	{
            handle_excep();
            if (vm_excep1 != 0)
                vm_excep = vm_excep1;
            break;
        }
	
	vm_pc = thr_active->curr_frame->pc;
	vm_sp = thr_active->curr_frame->sp;

        if ((exec_method->flags & (ACC_SYNCH | ACC_STATIC)) == ACC_SYNCH)
        {
            /* acquire lock for object on which is method being executed */
            acquireObjectLock(thr_active, GET_REF(vm_args[0].val));
        }
        else if ((exec_method->flags & (ACC_SYNCH | ACC_STATIC)) == (ACC_SYNCH | ACC_STATIC))
        {
            /* acquire class wide lock */
            acquireClassLock(thr_active);
        }

 	/* remember method's frame so we know when to return from run() */
        if (init_frame == NULL)
            init_frame = thr_active->curr_frame;

        /* copy args (if present) to local variables */
        for (i=0; i < vm_args_count; i++)
            thr_active->curr_frame->locals[i] = vm_args[i];


	  /* establish the next time slice expiration time */
        expired_ms = vm_time_ms + config->time_slice;

        /* bytecodes execution loop */
        while (true)
        {
#if ENABLE_WDT
            config->clearWdt();
#endif

	    UpdateVMTimer();

	    if(vm_time_ms >= expired_ms) {
	        VMTick();
	          /* establish the next time slice expiration time */
                expired_ms = vm_time_ms + config->time_slice;
	    }

	    
            bc_item = NULL;
            handler = bytecode_table[*vm_pc];
            vm_pc++;
            bc_action = handler();

            if (bc_action < 0)
            {
                if (!handle_excep())
                  return false;
            }
	    
            switch (bc_action)
            {
                case 0:
                    /* no special handling required -- goto next bytecode */
                    continue;

                case ACTION_RETURN:
                    /* release synchronization locks -- if any */
                    if (!releaseLocks())
                        return false;
                    i = handle_return(init_frame);
                    if (i == RUN_RETURN)
                        return true;
                    go = (bool)(i != RETURN_FAIL);
                    break;

                case ACTION_INVOKE:
                    exec_method = (method_t *)bc_item;
                    break;

                case ACTION_NATIVE:
                    go = handle_native();
                    break;

                case ACTION_NEW:
                    go = handle_new();
                    break;

                case ACTION_THROW:
                    go = handle_throw((ref_t *)bc_item);
                    break;
            }
            if (bc_action == ACTION_INVOKE)
                break;

            if (!go)
                return false;
        }
    }
    return false;
}


/****************************************************************************
* Supporting function for run().
* Performs preparation procedures for searching the exception handler.
*
* Input:        method  pointer to currently executing method
* Output:       none
* Return:       true    if code execution should continue
*               false   if exception has been generated and currently
*                       running method should return
****************************************************************************/
bool handle_excep(void)
{
    int16 excep, detail;

    excep = getExceptionIndex(&detail);
    if (excep == 0 || excep > RUNTIME_EXCEPTIONS)
    {
        fillInStackTrace();
        return false;
    }

    /* save current run-time exception code */
    vm_excep1 = vm_excep;
    vm_excep = 0;

    return handle_throw(excep_refs[excep-1]);
}


/****************************************************************************
* Supporting function for run().
* Gets native method lookup table index. For system menthods (java/lang)
* index is adjusted by SYS_BASE_INDEX value. Then, appropriate offset is
* calculated either into system or application lookup tables.
* 'C' native must have following prototype:
*              void method(int32 natParams[], int32 *retValue);
*
* Functions expects that the bc_item has been setup by the invocation bytecode.
*
* Input:        none
* Output:       none
* Return:       true    if code execution should continue
*               false   if exception has been generated and currently
*                       running method should return
****************************************************************************/
static bool handle_native(void)
{
    int16 index;
    method_t *native;
    value_t retval;


    native = (method_t *)bc_item;

    /* check if it is system native method */
    if (native->blen_idx >= SYS_BASE_INDEX)
    {
        /* run system native method */
        index = (int16)(native->blen_idx - SYS_BASE_INDEX);
        vm_excep = (uint8)sys_native_tbl[index]((int32 *)vm_args, (int32 *)&retval);
    }
    else
    {
        /* run user application native method */
        vm_excep = (uint8)config->native_tbl[native->blen_idx]((int32 *)vm_args, (int32 *)&retval);
    }

    if (vm_excep != 0)
        return handle_excep();

    if (vm_status & RETVAL_FLAG)
        *vm_sp++ = retval;

    return true;
}


/****************************************************************************
* Supporting function for run().
* Handles returns from methods.
*
* Input:        init_frame  pointer first frame created by run()
* Output:       none
* Return:       true    if code execution should continue
*               false   if there are no more methods on frames stack
*
****************************************************************************/
static int16 handle_return(frame_t *init_frame)
{
    value_t rv;

    /*
    ** Special handling if RUN_SPECIAL_FLAG is set.
    ** run() will return when init_frame equals current one
    */
    if ((vm_status & RUN_SPECIAL_FLAG) && (init_frame == thr_active->curr_frame))
    {
        if (!releaseFrame(&rv))
            thr_active->curr_frame = NULL;
        return RUN_RETURN;
    }

    if (releaseFrame(&rv))
    {
        if (vm_status & RETVAL_FLAG)
            *vm_sp++ = rv;

        return METH_RETURN;
    }

    /* no more frames on the stack -- terminate this thread */
    deleteThread(thr_active);

    if (thr_count > 0)
      return METH_RETURN;

    return RETURN_FAIL;
}


/****************************************************************************
* Supporting function for run().
* Creates new instance of object specified by the supplied class structure.
*
* Functions expects that the bc_item has been setup by the 'new' bytecode.
*
* Input:        none
* Output:       none
* Return:       true    if code execution should continue
*               false   if exception has been generated and currently
*                       running method should return
****************************************************************************/
static bool handle_new(void)
{
    ref_t *ref;
    class_t *class_ptr;

    class_ptr = (class_t *)bc_item;

    /* Run <clinit> on the new object if RUN_SPECIAL_FLAG is set */
    if (vm_status & RUN_SPECIAL_FLAG)
    {
        if (!run_clinit(class_ptr))
            return false;
    }

    ref = createObject(class_ptr);
    if (ref == NULL)
        return handle_excep();

    /* put object's reference onto stack */
    (*vm_sp++).val = PUT_REF(ref);

    return true;
}


/****************************************************************************
* Supporting function for run().
* Searches trough the currently excuting method exception table to see if
* it has appropriate handlers. If not then exception is re-thrown in calling
* method. Process repeats until exception handler is found or there are no
* more calling methods.
*
* Input:        excep   pointer to reference of thrown exception object
* Output:       none
* Return:       true    if exception has been catched and exception handler
*                       code has to be executed
*               false   if exception hasn't been caught and method should
*                       return
****************************************************************************/
static bool handle_throw(ref_t *excep)
{
    uint8 *new_pc;
    value_t rv;

    fillInStackTrace();
    while (true)
    {
        new_pc = findExcepEntry(excep);
        if (new_pc != NULL)
        {
            /* reset stack pointer */
            vm_sp = (value_t *)((uint8 *)thr_active->curr_frame + FRAME_HDR_SIZE +
                                thr_active->curr_frame->method->locals);

            /* push thrown exception object onto stack */
            (*vm_sp++).val = PUT_REF(excep);

            /* start excep. handler to run */
            vm_pc = new_pc;
            return true;
        }

        /* release synchronization locks -- if any */
        if (!releaseLocks())
            break;

        /* restore previous method's frame so exception could be re-thrown */
        if (releaseFrame(&rv))
        {
            /*
            ** Prev. metod's frame restored - re-throw exception in this method by
            ** pushing exception object onto stack and running bc_athorw
            */
            /*
            (*vm_sp++).val = PUT_REF(excep);
            BC_athrow();
            */
            continue;
        }

        /* restore run-time exception -- if exists */
        if (vm_excep1 != 0)
            vm_excep = vm_excep1;
        else
            vm_excep = INTERR_UnhandledExcep;

        break;
    }
    return false;
}


/****************************************************************************
* Creates new java object on the heap.
*
* Input:        class_ptr   ptr. to class structure
* Output:       none
* Return:       pointer to object's reference if successful, othrewise
*               NULL is returned.
****************************************************************************/
ref_t *createObject(class_t *class_ptr)
{
    ref_t *ref;

    ref = allocHeapMem(inst_size);

    if (ref != NULL)
    {
        ref->class_ptr = class_ptr;
        ref->ptr->flag |= OF_CLASS;
    }

    return ref;
}


/****************************************************************************
* Runs first method in method table -- which is always <init>(). If strRef is
* not NULL then second initialisation method is run --  <init>(java/lang/String)
* It is guaranteed by the ClassLinker that <init>() is always first.
*
* Input:        ref     pointer to reference of the object for which init
*                       should be run
*               strRef  pointer to string object
* Output:       none
* Return:       true if successful, otherwise false is returned.
****************************************************************************/
bool run_init(ref_t *ref, ref_t *strRef)
{
    value_t param[2];
    method_tbl_t *meth_tbl;

    param[0].val = PUT_REF(ref);
    vm_args_count = 1;
    if (strRef != NULL)
    {
        param[1].val = PUT_REF(strRef);
        vm_args_count++;
    }
    vm_args = param;
    vm_status |= RUN_SPECIAL_FLAG;

    meth_tbl = (method_tbl_t *)ABS_LOC(ref->class_ptr->meth_tbl);
    if (strRef == NULL)
        return run(METHOD_LOC(meth_tbl[0].method));
    return run(METHOD_LOC(meth_tbl[1].method));
}


/****************************************************************************
* Creates new java/langString object.
* Calling process should test for possible exceptions.
*
* Input:        cons    ptr. to string constant info. structure
* Output:       none
* Return:       pointer to String's object reference if successful, othrewise
*               NULL is returned.
****************************************************************************/
ref_t *createStringObject(uint16 hash, uint32 length, int8 *src)
{
    ref_t *str_ref, *arr_ref = NULL;
    jchar *dest;
    arrhdr_t *arr_hdr;
    uint32 i, size;

    /* for loop allows to re-allocate objects in case GC was run during allocation process */
    for (i=0; i < ALLOC_TRY; i++)
    {
        /* allocate memory for new String object and initialise header */
        str_ref = allocHeapMem(inst_size);
        if (str_ref == NULL)
            return NULL;

        str_ref->class_ptr = CLASS_LOC(APP->string);
        str_ref->ptr->flag |= OF_CLASS;

        if (!addToStrTable(hash, str_ref))
            return NULL;
        /* re-allocate objects if GC was run and str_ref has been freed */
        if ((vm_status & GC_DONE_FLAG) == 0)
        {
            /* allocate memory for string array object and initialise header */
            size = length;
#if ENABLE_UNICODE
            size <<= 1;
#endif
            size += sizeof(arrhdr_t);
            if (size < inst_size)
                size = inst_size;

            arr_ref = allocHeapMem(size);
            if (arr_ref == NULL)
                return NULL;

            arr_ref->ptr->flag |= OF_ARRAY;

            if ((vm_status & GC_DONE_FLAG) == 0)
                break;
        }

        removeFromStrTable(str_ref);
    }
    if (i == ALLOC_TRY)
    {
        vm_excep = vm_excep1;
        return NULL;
    }

    arr_hdr = (arrhdr_t *)arr_ref->ptr;
    arr_hdr->count = length;
    arr_hdr->type = (uint8)T_CHAR;

    /* copy characters */
    dest = (jchar *)((uint8 *)arr_hdr + sizeof(arrhdr_t));
    for (i=0; i < length; i++)
        dest[i] = src[i];

    /* initialise String's count, offset and value fields */
    setField(str_ref, FIELD_LOC(APP->string_count), length);
    setField(str_ref, FIELD_LOC(APP->string_offset), 0);
    setField(str_ref, FIELD_LOC(APP->string_value), PUT_REF(arr_ref));

    return str_ref;
}


/****************************************************************************
* Sets the instance field to specified value;
*
* Input:        ref     pointer to object's reference
*               field   pointer to field info structure
*               value   new value for the field
* Output:       none
* Return:       none
****************************************************************************/
static void setField(ref_t *ref, field_t *field, uint32 value)
{
    value_t *fields;

    fields = (value_t *)((uint8 *)ref->ptr + sizeof(classhdr_t));
    fields[FLD_INDEX(field->type_index)].i = value;
}


/****************************************************************************
* Checks if the class S is subclass of class T.
* Checking is done by comparing s and t. If they don't match then
* location of super class S is retrieved and check is performed again.
* This is repeated until match is found or there are no more super classes
* of class S.
*
* Input:        S       pointer to class S
*               T       pointer to class T
* Output:       none
* Return:       true if S is subclass of T, otherwise false is returned
****************************************************************************/
bool isSubclass(class_t *S, class_t *T)
{
    while (true)
    {
        if (S == T)
            return true;

        if (S == NULL || S->super == NULL)
            break;

        S = CLASS_LOC(S->super);
    }
    return false;
}



/****************************************************************************
* Goes trough the exception table of the current method and checks if there
* is an entry with matching values for the PC. If found then it checks if
* thrown exception object is subclass of exception type specified by the
* table entry.
*
* Input:        excep_ref   reference to thrown exception object
* Output:       none
* Return:       new PC if mathing table entry found, otherwise NULL is
*               returned
****************************************************************************/
static uint8 *findExcepEntry(ref_t *excep_ref)
{
    uint16 i;
    method_t *method;
    excep_tbl_t *excep_tbl;
    uint16 rel_pc;

    method = thr_active->curr_frame->method;
    excep_tbl = (excep_tbl_t *)((uint8 *)method + method->blen_idx + sizeof(method_t));

    rel_pc = (uint16)(vm_pc - 1 - ((uint8 *)thr_active->curr_frame->method + sizeof(method_t)));
    for (i=0; i < excep_tbl->entries; i++)
    {
        /* Check if current PC is within the range: start <= rel_pc < end */
        if (rel_pc >= excep_tbl->excep[i].start && rel_pc < excep_tbl->excep[i].end)
        {
            /*
            ** Check if it's finally clause (-1) or if thrown exception is class or
            ** subclass of objet specified in table entry.
            */
            if (excep_tbl->excep[i].excep_idx == (uint16)0xFFFF ||
                isSubclass(excep_ref->class_ptr, (class_t *)getLocation(excep_tbl->excep[i].excep_idx)))
            {
                return (uint8 *)((uint8 *)method + sizeof(method_t) + excep_tbl->excep[i].handler);
            }
        }
    }

    return NULL;
}


/****************************************************************************
* Expand signed uint8 value into java integer value.
*
* Input:        bval    signed uint8 value
* Output:       none
* Return:       sign expanded uint8 value
****************************************************************************/
int32 expandByte(int8 bval)
{
    if (bval & 0x80)
        return (0xFFFFFF00L | bval);

    return bval;
}


/****************************************************************************
* Expand signed short value (16 bit) into java integer value.
*
* Input:        sval    signed short value
* Output:       none
* Return:       sign expanded short value
****************************************************************************/
int32 expandShort(int16 sval)
{
    if (sval & 0x8000)
        return (0xFFFF0000L | sval);

    return sval;
}


/****************************************************************************
* Sets the pointer to method arguments and calculates the number of arguments
* that method must take.
*
* Input:        method  pointer to method being invoked
* Output:       none
* Return:       sign expanded short value
****************************************************************************/
void setupMethArgs(method_t *method)
{
    vm_args_count = 0;
    vm_args = NULL;

    vm_args_count += (uint8) method->nargs;
    if (vm_status & OBJREF_FLAG)
        vm_args_count++;

    vm_sp -= vm_args_count;
    vm_args = vm_sp;
}


/****************************************************************************
* Adds new entry to vm_strings_start. if table if full EXCEP_StrTableFull
* is thrown.
*
* Input:        hash    string's hashcode
*               string  reference to string object
* Output:       none
* Return:       true if successfull, otherwise false is returned
****************************************************************************/
static bool addToStrTable(uint16 hash, ref_t *string)
{
    uint16 i;

    vm_status &= ~GC_DONE_FLAG;

    while (true)
    {
        /* find first empty slot in the table */
        for (i=0; i < config->strings; i++)
        {
            if (strings[i].ref == NULL)
                break;
        }
        if (i < config->strings)
            break;

#if ENABLE_GC
        /* if not found and GC has been run -- generate exception */
        if (vm_status & GC_DONE_FLAG)
        {
            vm_excep = MEMERR_StrTableFull;
            return false;
        }

        /* run GC at this point to see if some table entries are freed */
        if (!walkAll())
            return false;
#else
        vm_excep = MEMERR_StrTableFull;
        return false;
#endif
    }

    strings[i].ref = string;
    strings[i].hash = hash;

    return true;
}


/****************************************************************************
* Removes entry containg matching object's reference.
*
* Input:        string  reference to string object
* Output:       none
* Return:       none
****************************************************************************/
void removeFromStrTable(ref_t *string)
{
    uint16 i;

    for (i=0; i < config->strings; i++)
    {
        if (strings[i].ref == string)
        {
            strings[i].ref = NULL;
            strings[i].hash = (uint16)0;
            return;
        }
    }
}


/****************************************************************************
* Tries to find matching entry in vm_strings_start. Searching will
* stop when entry with matching hashcode is found and refernce
* to corresponding string object is returned.
*
* Input:        hash    string's hashcode
* Output:       none
* Return:       reference to coresponding string object if successfull,
*               NULL if no matching entry has been found.
****************************************************************************/
ref_t *findInStrTable(uint16 hash)
{
    uint16 i;

    for (i=0; i < config->strings; i++)
    {
        if (strings[i].hash == hash)
            return strings[i].ref;
    }
    return NULL;
}


/****************************************************************************
* Resolves the reference to the specified String object and
* returns the pointer to array of chars starting at offset
* specified by String.offset field.
*
* Input:        string  pointer to reference of the String object
* Output:       none
* Return:       pointer to int8 array of this String object
****************************************************************************/
jchar *getStringArray(int32 param)
{
    ref_t *arrobj, *string;
    arrhdr_t *arrhdr;
    value_t *fields;
    uint32 offs;

    string = GET_REF(param);
    if (string != NULL)
    {
        /* get pointer to string object instance data */
        fields = (value_t *)((uint8 *)string->ptr + sizeof(classhdr_t));
        arrobj = GET_REF(fields[FLD_INDEX(FIELD_LOC(APP->string_value)->type_index)].i);
        if (arrobj != NULL)
        {
            offs = fields[FLD_INDEX(FIELD_LOC(APP->string_offset)->type_index)].i;
#if ENABLE_UNICODE
            offs <<= 1;
#endif
            arrhdr = (arrhdr_t *)arrobj->ptr;
            return (jchar *)((uint8 *)arrhdr + sizeof(arrhdr_t) + offs);
        }
    }
    return NULL;
}

/****************************************************************************
* Resolves the reference to the specified String object and
* returns the number of characters contained in this String
* object.
*
* Input:        string  pointer to reference of the String object
* Output:       none
* Return:       length of the string
****************************************************************************/
jint getStringCount(int32 param)
{
    ref_t *string;
    value_t *fields;

    string = GET_REF(param);
    if (string != NULL)
    {
        fields = (value_t *)((uint8 *)string->ptr + sizeof(classhdr_t));
        return fields[FLD_INDEX(FIELD_LOC(APP->string_count)->type_index)].i;
    }
    return 0;
}

/****************************************************************************
* Resolves the reference to the specified String object and
* returns the pointer to array of chars starting at offset
* specified by String.offset field.
*
* Input:        string  pointer to reference of the String object
* Output:       none
* Return:       pointer to int8 array of this String object
****************************************************************************/
jint getArraySize(int32 param)
{
    ref_t *ref;

    ref = GET_REF(param);
    if (ref != NULL && ref->ptr != NULL)
        return ((arrhdr_t *)ref->ptr)->count;

    return 0;
}

/****************************************************************************
* Resolves the reference to the specified String object and
* returns the pointer to array of chars starting at offset
* specified by String.offset field.
*
* Input:        string  pointer to reference of the String object
* Output:       none
* Return:       pointer to int8 array of this String object
****************************************************************************/
void *getArray(int32 param)
{
    ref_t *ref;

    ref = GET_REF(param);
    if (ref != NULL && ref->ptr != NULL)
        return (uint8 *)ref->ptr + sizeof(arrhdr_t);

    return NULL;
}

/****************************************************************************
* This routine checks if the application at vm_app_start has the correct
* magic number, ClassLinker version and calculates LRC over specified area
* of memory.
*
* Input:        none
* Output:       none
* Return:       true if successful, otherwise false is returned
****************************************************************************/
static bool verifyApp(void)
{
    uint32 i;
    uint8 *ptr;
    uint8 sum = 0;

    if (APP->magic == MAGIC)
    {
        if (APP->version == SW_VER)
        {
            ptr = APP->csum_start + (uint32)config->app_start;
            for (i=0; i < APP->csum_len; i++)
            {
                sum ^= ptr[i];
            }
            if (sum == (uint8)APP->csum_lrc)
                return true;
        }
    }
    return false;
}


/****************************************************************************
* Creates a new array of specified type. After creating the array its elements
* are initialised to zero.
* EXCEP_NegativeArraySize is thrown if size of array is negative.
* EXCEP_NoHeap will be thrown if there is not enough room on the heap to
* hold the new array.
*
* Input:        type    - type of the array
*               size    - number of elements in array (this is not size in bytes)
* Output:       none
* Return:       Reference to a new array object. NULL is exception has been
*               detected.
****************************************************************************/
ref_t *doSimpleArray(uint8 type, uint32 length)
{
    ref_t *ref;
    arrhdr_t *arr;
    uint32 size;

    size = length;
    switch (type)
    {
        case T_BOOLEAN:
        case T_BYTE:
#if !ENABLE_UNICODE
        case T_CHAR:
#endif
            break;

#if ENABLE_UNICODE
        case T_CHAR:
#endif
        case T_SHORT:
            size <<= 1;
            break;

        case T_INT:
        case T_FLOAT:
        case T_OBJREF:
            size <<= 2;
            break;

        default:
            vm_excep = INTERR_UnsupportedType;
            return NULL;
    }
    size += (uint32)sizeof(arrhdr_t);
    if (size < inst_size)
        size = inst_size;

    /* allocate memory for the new array */
    ref = allocHeapMem(size);
    if (ref == NULL)
        return NULL;

    ref->ptr->flag |= OF_ARRAY;
    arr = (arrhdr_t *)ref->ptr;
    arr->type = type;
    arr->count = length;
    return ref;
}


/****************************************************************************
* Creates a String array and initialises its members to arguments passed in
* vm_config_t. If number of args is zero then args array will have zero length.
*
* Input:        none
* Output:       none
* Return:       none
****************************************************************************/
#if  ENABLE_MAIN_ARGS
static bool createMainArgs(void)
{
    ref_t *aref;
    uint32 *sref;
    uint16 i;

    if (config->args_count > 0 && config->args == NULL)
        return false;

    vm_args_count = 0;
    aref = doSimpleArray(T_OBJREF, config->args_count);
    if (aref == NULL)
        return false;
    main_args.val = PUT_REF(aref);

    /* setup the pointer to string array */
    sref = (uint32 *)((uint8 *)aref->ptr + sizeof(arrhdr_t));
    for (i=0; i < config->args_count; i++)
    {
        sref[i] = PUT_REF(createStringObject((uint16)(i+1), strLen(config->args[i]), config->args[i]));
        if (GET_REF(sref[i]) == NULL)
            return false;
    }

    /* There is always only one parameter for the main method: String[] */
    vm_args = &main_args;
    vm_args_count = 1;
    return true;
}
#endif

/*
* Returns the length of zero terminated string.
*/
uint32 strLen(int8 *str)
{
    uint32 len = 0;

    if (str != NULL)
    {
        while (str[len] != 0)
            len++;
    }
    return len;
}

/*
* Returns current VM system time.
*/
static uint32 getSysTime(void)
{
    return vm_time_ms;
}

/*
* Updates specified event counter and forces event thread to run
* immediatelly.
*/
static void updateEventCounter(uint16 index)
{
    config->ev_counters[index]++;
}


/*
* Runs the specified function specified in config.
*/
static void usrFunc(void (*f)(uint16, char *), uint16 code)
{
#if ENABLE_TEXT_NOTIFY
    uint16 subcode;
#endif
    if (f != NULL)
    {
#if ENABLE_TEXT_NOTIFY
        subcode = (uint16)(code & 0xFF);
        if (subcode > NOTIFY_VM)
            f(code, (char *)notify_msg[subcode-NOTIFY_VM-1]);
        else if (subcode > MEMERR)
            f(code, (char *)mem_excep_msg[subcode-MEMERR-1]);
        else if (subcode > INTERR)
            f(code, (char *)int_excep_msg[subcode-INTERR-1]);
        else
            f(code, (char *)excep_msg[(code >> 8)-1]);
#else
        f(code, NULL);
#endif
    }
}



void notify(uint16 code, char *msg)
{
	uint16 i, subcode;

	subcode = (uint16)(code & 0xFF);
	code >>= 8;

	if (subcode > NOTIFY_VM)
		printf("Notify: 0x%02X - %s\n", subcode, msg ? msg : "");
	else
	{
		printf("Exception: 0x%02X: 0x%02X - %s\n", code, subcode, msg ? msg : "");
		if (vmconfig.stack_trace != NULL && vmconfig.stack_trace->count > 0)
		{
			puts("Stack trace:");
			for (i=0; i < (uint16)vmconfig.stack_trace->count; i++)
				printf("Method:%06X   PC:%04X\n", (uint32)(vmconfig.stack_trace->trace[i].method), (uint32)(vmconfig.stack_trace->trace[i].pc));
		}
	}
}

int8 *getProperty(int8 *key)
{
	if (strcmp(key, "file.separator") == 0)
		return "\\";
	else if (strcmp(key, "path.separator") == 0)
		return ";";
	else if (strcmp(key, "line.separator") == 0)
		return "\r\n";
	else if (strcmp(key, "user.language") == 0)
		return "au";
	else if (strcmp(key, "user.region") == 0)
		return "AU";
	return NULL;
}
