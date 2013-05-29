/**************************************************************************************
*  jvm.h
*
*  This file contains all internal definitions for the simpleRTJ.
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
*  Developer(s): Peter Gasparik <peterg@rtjcomp.com>
**************************************************************************************/

#ifndef _JVM_H
#define _JVM_H

#include "javavm.h"
#include "jvm_cfg.h"


#define MAGIC                   0xCAFEBABEL     /**< magic number word */
#define SW_VER                  0x0123          /**< ClassLinker's software version */
#define SYS_BASE_INDEX          0xF000          /**< base index for system native methods */

#define RUNTIME_EXCEPTIONS      11              /**< numb. of supported run-time exceptions */
#define MIN_OBJDATA_SIZE        1024            /**< min. size for objects data on the heap */
#define COMPACT_TRESHOLD        (MIN_OBJDATA_SIZE/2)        /**< treshold value for GC compactor */

#define MAX_TRACES              8               /**< max. number of stack trace levels that can be recorded */
#define STACKTRACE_SIZE         (sizeof(trace_t)+(sizeof(trace_tbl_t)*(MAX_TRACES-1)))

#if ENABLE_GC
  #define ALLOC_TRY             3               /**< number of retries for multiple object allocation */
#else
  #define ALLOC_TRY             1
#endif
/*
** Flags in jvm_status register
*/
#define WIDE_FLAG                 1U    /**< wide bytecode has been executed */
#define OBJREF_FLAG               2U    /**< objref present in method arguments */
#define RUN_SPECIAL_FLAG          4U    /**< set when executing <clinit> or <init> methods */
#define RETVAL_FLAG               8U    /**< set for native methods declared to return value */
#define THR_SWITCH_FLAG          16U    /**< set to indicate pending thread switch */
#define GC_DONE_FLAG             32U    /**< set by GC when finished */
#define OBJ_ARR_FLAG             64U    /**< set by GC if array of objects has been encountered */
#define GRAY_FLAG               128U    /**< set by GC when object was marked with gray */

/*
** Flags in the object_t.flag field
*/
#define OF_ARRAY          1U        /**< heap object is an array */
#define OF_CLASS          2U        /**< heap object is a class */
#define OF_DAEMON         4U        /**< class object is daemon thread */
#define OF_GRAY          64U        /**< heap object has been visited by GC */
#define OF_BLACK        128U        /**< heap object is live -- not to be collected by GC */

/*
** Thread states
*/
#define DEAD		 0U	    /**< thread is dead */
#define READY		 1U	    /**< thread is ready to run */
#define RUNNING		 2U	    /**< thread is running */
#define SLEEP	  	 3U	    /**< thread is in the sleep queue */
#define SUSPEND		 4U	    /**< thread is suspended */
#define WAIT_LOCK    	 5U         /**< thread is waiting for object's lock to be released */
#define WAIT_NOTIFY 	 6U         /**< thread is waiting to be notified */
#define BLOCK_ON_MUTEX   7U         /**< waiting on Mutex */
#define BLOCK_ON_COND    8U         /**< waiting on Condition */
#define BLOCK_ON_SEM     9U         /**< waiting on Semaphore */

/*
** Class access flags
*/
#define ACC_INTERFACE   0x0200U
#define ACC_RETVAL      0x4000U
#define ACC_ARRAY_TYPE  0x8000U

/*
** Method access flags
*/
#define ACC_NATIVE      0x0100U
#define ACC_ABSTRACT    0x0400U
#define ACC_SYNCH       0x0020U
#define ACC_STATIC      0x0008U

/*
** Java types
*/
#define T_BOOLEAN         4U
#define T_CHAR            5U
#define T_FLOAT           6U
#define T_DOUBLE          7U
#define T_BYTE            8U
#define T_SHORT           9U
#define T_INT            10U
#define T_LONG           11U
#define T_OBJREF        100U        /**< used when anewarray creates object arrays */

/*
** Java const types
*/
#define C_INT           ((uint16)0x4000)
#define C_FLOAT         ((uint16)0x8000)
#define C_STRING        ((uint16)0xC000)

/*
** Software timers
*/
#define TMR_EXPIRED     1

/*
** As software timers are implemented within the JVM code it is assumed that
** the Timer event handler is always installed at zero index using the
** AsyncEvent.addHandler method.
*/
#define TMR_FIRE_COUNTER    0

/**
** Actions required after execution of some uint8 codes
*/
enum bc_actions
{
    ACTION_NONE,                    /**< no special action required */
    ACTION_RETURN,                  /**< return from method */
    ACTION_INVOKE,                  /**< invoke java function */
    ACTION_NATIVE,                  /**< invoke native method */
    ACTION_NEW,                     /**< create class instance */
    ACTION_THROW                    /**< handle thrown exception */
};

/**
** Field types: B, C, S, I, F, Z, L, [
*/
enum field_type
{
    FT_BYTE = 0,
    FT_CHAR,
    FT_SHORT,
    FT_INT,
    FT_FLOAT,
    FT_BOOLEAN,
    FT_CLASS,
    FT_ARRAY
};

/**
** Classfile declarations
*/
typedef _PACKED_ struct method_T     method_t;
typedef _PACKED_ struct field_T      field_t;
typedef _PACKED_ struct class_T      class_t;
typedef _PACKED_ struct app_T        app_t;
typedef _PACKED_ struct class_arr_T  class_arr_t;
typedef _PACKED_ struct method_tbl_T method_tbl_t;
typedef _PACKED_ struct const_T      const_t;
typedef _PACKED_ struct frame_T      frame_t;
typedef _PACKED_ struct thread_T     thread_t;
typedef _PACKED_ struct mutex_T      mutex_t;
typedef _PACKED_ struct condition_T  condition_t;
typedef _PACKED_ struct semaphore_T  semaphore_t;
typedef _PACKED_ union value_T       value_t;
typedef _PACKED_ struct ref_T        ref_t;
typedef _PACKED_ struct excep_T      excep_t;
typedef _PACKED_ struct iface_T      iface_t;

typedef int16 (*bc_handler)(void);


/**
** All references present on the heap and method frames stack
** are masked with the REFERENCE value to simplify recognition of
** references during the garbage collection.
*/
#define REFERENCE       0xC0000000L
#define PUT_REF(x)      ((uint32)x | REFERENCE)
#define GET_REF(x)      ((ref_t *)(x & ~REFERENCE))

#define ABS_LOC(x)      ((uint8 *)((uint32)x + (uint32)config->app_start))
#define REL_LOC(x)      ((uint8 *)((uint32)x - (uint32)config->app_start))
#define CLASS_LOC(x)    ((class_t *)((uint32)x + (uint32)config->app_start))
#define METHOD_LOC(x)   ((method_t *)((uint32)x + (uint32)config->app_start))
#define FIELD_LOC(x)    ((field_t *)((uint32)x + (uint32)config->app_start))
#define CONST_LOC(x)    ((const_t *)((uint32)x + (uint32)config->app_start))

#define CONVERT(x)      ((uint32)x)
#define APP             ((app_t *)config->app_start)

#if ENABLE_ODDADR_READ
  #define GET_PC16        *((int16 *)vm_pc)
  #define GET_PC32        *((int32 *)vm_pc)
#else
  #if LSB_FIRST
    #define GET_PC16      ((int16)((*(vm_pc + 1) << 8) | *vm_pc))
    #define GET_PC32      ((int32)((*(vm_pc + 3) << 24) | (*(vm_pc + 2) << 16) | (*(vm_pc + 1) << 8) | *vm_pc))
  #else
    #define GET_PC16      ((int16)((*vm_pc << 8) | *(vm_pc + 1)))
    #define GET_PC32      ((int32)((*vm_pc << 24) | (*(vm_pc + 1) << 16) | (*(vm_pc + 2) << 8) | *(vm_pc + 1)))
  #endif
#endif

#define ALIGN4(x)       ((((uint32)x) & 3) ? (uint32)((uint32)x + 4 - (((uint32)x) & 3))  : (uint32)x)

#if ENABLE_PRAGMA_PACK
#pragma pack(2)
#endif

union value_T
{
    int32   i;
    float32 f;
    uint32  val;                /**< 32-bit value of unspecified type */
};

/**
** Application file header
*/
struct app_T
{
    uint32      magic;              /**< magic number */
    uint32      version;            /**< ClassLinker's version number */
    uint8      *csum_start;         /**< start offset for checksum */
    uint32      csum_len;           /**< number of bytes for checksum */
    uint32      csum_lrc;           /**< checksum */
    uint8       id[12];             /**< appl. version string */
    uint8       desc[32];           /**< appl. description string */
    uint32      static_size;        /**< size static fields */
    uint8      *excep_list;         /**< pointer to runtime exception table */
    uint8      *main;               /**< pointer to main() method */
    uint8      *thread;             /**< pointer to java/lang/Thread class */
    uint8      *object;             /**< pointer to java/lang/Object class */
    uint8      *cloneable;          /**< pointer to java/io/Cloneable interface */
    uint8      *serializable;       /**< pointer to java/io/Serializable interface */
    uint8      *string;             /**< pointer to java/lang/String class */
    uint8      *string_value;       /**< pointer to String.value field in String class */
    uint8      *string_count;       /**< pointer to String.count field in String class */
    uint8      *string_offset;      /**< pointer to String.offset field in String class */
    uint32      class_count;        /**< number of classes in this application */
    uint32      frame_size;         /**< size of the largest method frame in bytes */
    uint32      inst_size;          /**< size of the largest class instance in bytes */
    uint32      class_tbl[1];       /**< array of offsets to classes */
};

/**
** Method structure
*/
struct method_T
{
    class_t *class_ptr;         /**< pointer to class containing this method */
    uint16  flags;              /**< method flags */
    uint16  locals;             /**< size of locals */
    uint16  hash;               /**< hashcode calc. over meth. signature and name */
    uint16  nargs;              /**< number of arguments this mehod takes (in 32-bit words) */
    uint16  blen_idx;           /**< bytecodes length or index to native method lookup table */
    uint16  unused;             /**< just to make header size multiple of four */
};

/**
** Exceptions list
*/
struct excep_T
{
    int32   count;                  /**< number of system runtime exceptions */
    uint32  class_ptr[1];           /**< list of pointers to exception classes */
};


/**
** Class structure
*/
struct class_T
{
    uint16      flags;              /**< class modifier flags */
    uint16      index;              /**< class location in the class offsets table */
    uint8      *ifaces;             /**< pointer to interfaces offsets table */
    uint8      *super;              /**< pointer of super class/interface */
    uint8      *run;                /**< pointer to run() method, if exists */
    uint8      *clinit;             /**< pointer to <clinit> method */
    uint32      meth_count;         /**< number of methods defined in this class */
    uint8      *meth_tbl;           /**< pointer to hashcode/method offsets table */
    uint8      *field_tbl;          /**< pointer to fields table */
};

/**
** Array class structure
*/
struct class_arr_T
{
    uint16      flags;              /**< always 0x0000 */
    uint16      type;               /**< class array type: T_BOOLEAN, T_CHAR, ... */
    class_t    *class_ptr;          /**< pointer to array's class if type=T_OBJREF */
};


struct method_tbl_T
{
    uint32      hash;               /**< hashcode calculated over method name and signature */
    uint8      *method;             /**< pointer to method */
};


/**
** Fields structure.
** Field's location is contained in lower 12 bits, field type in next 3 bits and
** bit 15 is set if the field is static.
*/
struct field_T
{
    uint32 type_index;
};
#define FLD_INDEX(x)    (uint16)(x & 0x0FFF)
#define FLD_TYPE(x)     (uint16)(x & 0x7000)
#define FLD_STATIC(x)   (uint16)(x & 0x8000)


/**
** Constant structure
** Length is located in lower 14 bits and type in the next 2 bits.
*/
struct const_T
{
    uint32  type_len;
};
#define CONST_TYPE(x)       (uint16)(x & 0xC000)
#define CONST_LEN(x)        (uint16)(x & 0x3FFF)
#define CONST_STR_HASH(x)   *((uint16 *)((uint8 *)x + sizeof(const_t)))
#define CONST_STR(x)        (int8 *)((uint8 *)x + sizeof(const_t) + sizeof(uint16))
#define CONST_VALUE(x)      *((uint32 *)((uint8 *)x + sizeof(const_t)))


/**
** List of interfaces implemented by a class
*/
struct iface_T
{
    uint32  count;                  /**< number of interfaces */
    uint8    *iface[1];             /**< offset to first interface class, not present if count=0 */
};

/**
** First three bytes of each object allocated on the heap are used to
** store object's lenght (two bytes) and flags.
*/
typedef _PACKED_ struct
{
    uint32      size;               /**< size of the object on the heap */
    uint16      locks;              /**< number of locks applied to this object */
    uint16      flag;               /**< object's status flags */
    uint16      thread_id;          /**< ID of thread that has lock on this object */
} objhdr_t;

/**
** Header for array objects on the heap
*/
typedef _PACKED_ struct
{
    objhdr_t    obj;                /**< header common to all objects created on the heap */
    uint32      count;              /**< max. number of elements in the array object */
    uint16      type;               /**< array type */
} arrhdr_t;

/**
** Header for class objects on the heap.
** Currently, no additional fields are defined for class objects.
*/
typedef objhdr_t classhdr_t;

/**
** First three bytes of each object allocated on the heap are used to
** store object's lenght (two bytes) and flags.
*/
typedef _PACKED_ struct
{
    uint16      thread_id;          /**< ID of thread that has lock on this class */
    uint16      locks;              /**< number of locks applied to this class */
} classlock_t;

/*
* Reference (handle) to an object
*/
struct ref_T
{
    objhdr_t    *ptr;               /**< pointer to either class or array object header */
    class_t     *class_ptr;         /**< pointer to class structure if object is class instance */
};

/*
** String hashcode/refernce table
*/
typedef _PACKED_ struct
{
    uint16      unused;             /**< just to make structure size multiple of four */
    uint16      hash;               /**< string's hashcode */
    ref_t      *ref;                /**< pointer to reference of String object */
} strings_t;


/*
** Exception table
*/
typedef _PACKED_ struct
{
    uint16      start;              /**< start PC */
    uint16      end;                /**< end PC */
    uint16      handler;            /**< handler PC */
    uint16      excep_idx;          /**< const. pool at this index is exception class that can be caught */
} excep_entry_t;


typedef _PACKED_ struct
{
    uint16          entries;        /**< number of excep. table etries */
    excep_entry_t   excep[1];       /**< list of exceptions that can be caught */
} excep_tbl_t;


/*
** Method frame header
*/
struct frame_T
{
    frame_t     *prev; /**< previous frame; NULL if this is first frame */
    frame_t     *next; /**< next frame; NULL is this is last frame */
    value_t     *sp;                /**< stack pointer */
    uint8       *pc;                /**< program counter */
    method_t    *method;            /**< pointer to method being executed */
    class_t     *class_ptr;         /**< pointer to the class containing this method */
    value_t      locals[1];         /**< start of method locals */
};
#define FRAME_HDR_SIZE      (sizeof(frame_t) - sizeof(value_t))

/*
** Each thread table entry has the following structure.
*/
struct thread_T
{
/*===== Fields used by ROSE ===== */
    uint16	 state;		    /**< current state of thread (DEAD,READY,RUNNING,...) */
    ref_t       *objref;            /**< pointer to Thread object reference */
    frame_t     *curr_frame;        /**< pointer to current frame */
    int32        pri;		    /**< priority, 0=highest priority */
    int32        ticks;             /**< remaining tick count */
    struct thread_T* next;	    /**< next thread in queue */
	int32 startTime;
	int32 deltaTime;

/*===== Remaining fields not used by ROSE ===== */
    uint16       id;                /**< thread number, used by object locking functions */
    uint16       flag;              /**< thread flags */
    int32        sleep;             /**< 32-bit thread sleep counter */
    int32        wait;              /**< 32-bit monitor wait counter */
    ref_t       *wait_obj;          /**< object's reference on which this thread is waiting or trying to get lock */
    uint16       wait_locks;        /**< number of locks associated with object on which this thread waits */
    uint16       wait_pos;          /**< thread's queue position when waiting for the lock release */

};


/*==========================
 * UVic Additions
 * (UVic/CS MHMC 22/Aug/2002
 *==========================
 */
struct mutex_T {
    bool      used;      /**< true if this mutex is used */
    thread_t  *owner;    /**< owner of this mutex */
    thread_t  *blockQ;   /**< queue of waiting threads */
};


struct condition_T {
    bool      used;      /**< true if this condition is used */
    thread_t  *blockQ;   /**< queue of waiting threads */
};


struct semaphore_T {
    bool      used;      /**< true if this semaphore is used */
    int32     val;       /**< current value of this semaphore */
    thread_t  *blockQ;   /**< queue of waiting threads */
};


/* ======== END =========== */

/**
** Software timers.
*/
typedef _PACKED_ struct
{
    uint32      flag;               /**< expired flag, set when timer's timeout value expires */
    int32       time;               /**< 32-bit timeout value */
} timer_t;


#if ENABLE_PRAGMA_PACK
#pragma pack()
#endif

#endif



