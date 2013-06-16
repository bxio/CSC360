/**************************************************************************************
*  javavm.h
*
*  This file contains all data type definitions, definitions for notifications, errors
*  and exceptions that can be generated during the execution of a java application
*  as well as definition for vm_config_t structure.
*
*  This header file should be sufficient to include in the native code source files. All
*  interfacing between the VM and the native code should be done via vm_config_t structure.
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

#ifndef _JAVAVM_H
#define _JAVAVM_H


/**************************************************************************************
** Data type definitions used across the simpleRTJ implementation.
** This implementation assumes that 'C' compiler treats
** - int   as 16-bit numbers
** - long  as 32-bit numbers
** - float as 32-bit IEEE384 floating numbers
**
** If your C compiler treats above mentioned data types differently you will need
** to adjust the following data types.
**************************************************************************************/

#define false           0
#define true            1

#ifndef NULL
  #define NULL          ((void *)0)
#endif

#ifndef INVALID
#define INVALID         -1
#endif

#define LSB_FIRST                 true  /* set to true for little endian ordering */
#define ENABLE_PRAGMA_PACK        true  /* set to true for #pragma pack(n) directive ot to false for packed modifier */
#define ENABLE_UNICODE            true  /* if enabled chars are 2 bytes long, otherwise chars are one byte long */
#define ENABLE_WDT                false /* set if vm_config_t provides address of the clear watchdog timer function */
#define ENABLE_THREADS            true  /* Enable threads support for java applications */
#define ENABLE_GC                 true  /* Set to zero if garbage collector is not required */
#define ENABLE_FLOAT              true  /* Generate code with floating point enabled */
#define ENABLE_MULTIANEWARRAY     true  /* Include code for multianewarray bytecode */
#define ENABLE_STRING_EXCEPTIONS  true  /* Generate code with run-time exception objects that have description strings included */
#define ENABLE_TEXT_NOTIFY        true  /* If enabled, usrNotify and usrException functions will also provide a short message */
#define ENABLE_ODDADR_READ        false /* set if CPU allows reading 16/32-bit values from odd addresses */
#define ENABLE_MAIN_ARGS          true  /* when enabled, vm_config_t must provide arguments for java application main method */
#define ENABLE_TIMER		  true  /* enables use of timer for thread pre-emption */

#if ENABLE_PRAGMA_PACK
  #define _PACKED_
#else
  #define _PACKED_          /*packed*/  /* replace with packing modifier appropriate to your compiler */
#endif

/*
** Typical 32-bit target with short=16bits & int/long=32bits.
** Make sure that the data type sizes match your compiler.
*/
typedef signed char     int8;
typedef unsigned char   uint8;
typedef signed short    int16;
typedef unsigned short  uint16;
typedef signed int      int32;
typedef unsigned int    uint32;
typedef float           float32;
typedef uint32          bool;

/*
** Java primitive data types
*/
typedef bool            jbool;
typedef int8            jbyte;
typedef int16           jshort;
typedef int32           jint;
typedef float32         jfloat;
#if ENABLE_UNICODE
  typedef uint16        jchar;
#else
  typedef uint8         jchar;
#endif


/*
** If your compiler warns about unused function arguments and there is no
** command line switch available to suppress this warning then use
** NOT_USED macro for unreferenced function arguments.
** You may need to modify this macro to work with the appropriate compiler.
*/
#if 0
  #define NOT_USED(junk)  { (volatile typeof(junk))junk = junk; }
#endif



/**************************************************************************************
* Notification codes supplied in vmNotify
**************************************************************************************/
enum _system_notifications
{
    NOTIFY_VM = 0x80,               /*      starting index for notification messages */
    NOTIFY_VM_INIT,                 /* 0x81 java VM intialisation started */
    NOTIFY_VM_START,                /* 0x82 java VM started program execution */
    NOTIFY_VM_NOAPP,                /* 0x83 java VM can't find application file */
    NOTIFY_INSUF_HEAP,              /* 0x84 java VM has insuficient amount of heap memory */
    NOTIFY_TIMER_SLOW		    /* 0x85 java VM timer resolution is too small */
};

/**************************************************************************************
* Java run-time error and exception types as reported in usrException.
* Order of exceptions must match the order in ClassLinker.control file.
**************************************************************************************/
enum _system_exceptions
{
    NO_EXCEP = 0,                   /*    no pending exception */
    ERROR_Internal,                 /* 1  something went wrong; see detail code */
    ERROR_OutOfMemory,              /* 2  VM run out of memory; see detail code */
    ERROR_NoSuchMethod,             /* 3  missing method definition */
    ERROR_AbstractMethod,           /* 4  method is abstract - no code present */
    EXCEP_NullPointer,              /* 5  null pointer discovered */
    EXCEP_IndexOutOfBounds,         /* 6  array index is out of bound for this array */
    EXCEP_ArrayStore,               /* 7  unable to store data type in this array */
    EXCEP_Arithmetic,               /* 8  arithmetic error occured */
    EXCEP_ClassCast,                /* 9  unable to cast object to specified type */
    EXCEP_NegativeArraySize,        /* 10 specified array size has negative value */
    EXCEP_IllegalMonitState         /* 11 thrown when current thread is not owner on object lock */
};


/**************************************************************************************
* Detail codes on ERROR_InternalError as reported in vmException
**************************************************************************************/
enum _internal_errors
{
    INTERR = 0x30,                  /*      starting index for internal errors */
    INTERR_RuntimeExcepCount,       /* 0x31 invalid number of runtime exceptions */
    INTERR_ProgEnd,                 /* 0x32 application has terminated */
    INTERR_InvBytecode,             /* 0x33 encountered unsupported byte code */
    INTERR_ThreadSetupFail,         /* 0x34 unable to find run() */
    INTERR_ThreadLimit,             /* 0x35 max # of threads was reached */
    INTERR_UnhandledExcep,          /* 0x36 checked exception without handler */
    INTERR_UnsupportedType,         /* 0x37 operand type not supported for this operation */
    INTERR_gcFail,                  /* 0x38 GC failed to update references, possible memory corruption */
    INTERR_ArgsCreate               /* 0x39 Unable to create main method Strings[] argument */
};


/**************************************************************************************
* Detail codes on ERROR_OutOfMemory as reported in vmException
**************************************************************************************/
enum _memory_errors
{
    MEMERR = 0x50,              /*      starting index for memory errors */
    MEMERR_NoHeap,              /* 0x51 no more memory on the heap */
    MEMERR_NoRefs,              /* 0x52 no more memory in refereneces table */
    MEMERR_NoFrames,            /* 0x53 no more memory for method frames */
    MEMERR_StrTableFull,        /* 0x54 string hashcode/reference table is full */
    MEMERR_AllocFail            /* 0x55 memory allocation failed due to currupt data */
};


/**************************************************************************************
** Macros for retrieveing upper and lower 16-bit values from a 32-bit parameter
**************************************************************************************/
#define HI_PAR(x)   ((x >> 16) & 0xFFFF)
#define LO_PAR(x)   (x & 0xFFFF)

/*
** Native method definition.
*/
typedef int16 (*native_func_ptr)(int32 param[], int32 *retval);


#if ENABLE_PRAGMA_PACK
#pragma pack(2)
#endif

/*
** Stack trace definitions
*/
typedef _PACKED_ struct
{
    uint8       *method;            /* method offset */
    uint8       *pc;                /* PC counter */
} trace_tbl_t;

typedef _PACKED_ struct
{
    uint32      count;              /* number of entries in stack trace */
    trace_tbl_t trace[1];           /* stack trace entries */
} trace_t;

/**************************************************************************************
* Java VM configuration parameters.
*
**************************************************************************************/
typedef _PACKED_ struct
{
    /*
    ** Configuration parameters and callback functions.
    ** These should be filled by the startup code before vmStart() is invoked.
    */
    uint16      time_slice;         /* time in ms specifying time slicing interval */
    uint16      references;         /* max. number of references */
    uint16      strings;            /* max. entries in strings table */
    uint16      threads;            /* max. entries in thread table */
    uint8       *app_start;         /* java application start addres */
    uint8       *heap_start;        /* start addr. of the heap */
    uint8       *heap_end;          /* end addr. of the heap - exclusive */
    uint16      timers;             /* number of software timers JVM should create */
    uint16      events;             /* number of event fire counters */
    uint16	priLevel;	    /* number of scheduling level */
    uint16      *ev_counters;       /* pointer to event fire counters table */
    native_func_ptr *native_tbl;    /* pointer to native method lookup table */
    void        (*usrNotify)(uint16 code, char *msg);   /* can be NULL if not implemented */
#if ENABLE_WDT
    void        (*clearWdt)(void);  /* address of the clear watchdog timer function */
#endif
    int8        *(*getProperty)(int8 *key); /* address of function for retrieving system properties */
#if  ENABLE_MAIN_ARGS
    uint16      args_count;         /* number of arguments passed to java application main method */
    int8        **args;             /* list of arguments passed to java application main method */
#endif
    /*
    ** VM information parameters and interfacing functions.
    ** Params are filled by the JVM during startup sequence and can be used during
    ** the native methods execution.
    */
/*  void        (*vmTimeSlice)(void);  */
    jint        (*vmGetStringCount)(int32);
    jchar*      (*vmGetStringArray)(int32);
    jint        (*vmGetArraySize)(int32);
    void*       (*vmGetArray)(int32);
    uint32      (*vmGetSysTime)(void);
    void        (*vmUpdateEventCounter)(uint16 index);
    trace_t     *stack_trace;

} vm_config_t;

#if ENABLE_PRAGMA_PACK
#pragma pack()
#endif

#endif


