/*
** This file was automatically generated by the ClassLinker ver. 1.2.3 and contains
** definitions required for linking native methods.
**
** DO NOT modify order of methods in this file as this action can cause
** improper invocation of native methods.
**
** Thu May 23 15:34:00 PDT 2013
*/


#include "javavm.h"

const uint8 *java_app_start = (uint8 *)0x0;

/*
** If you are using banked memory model remove comments from the following
** line prior invoking the ClassLinker.
*/
#if 0
const uint8 *bank_start = (uint8 *)0x0;
#endif

/*
** Dummy function declaration used as last intialiser in the native_tbl to keep
** compilers happy if no native methods exists for the given java application
** (native_tbl initialisers list must not be empty).
*/
#pragma argsused
int16 __dummy_func(int32 param[], int32 *retval) { return NO_EXCEP; }




const native_func_ptr native_tbl[] =
{
        __dummy_func
};

