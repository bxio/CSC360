/**************************************************************************************
*  FileOutputStream.c
*
*  This file defines all native methods that have to be implemented to support
*  java/lang/FileOutputStream class.
*  Actual implementation is target device dependent and therefore must be
*  implemented by the application developer.
*
*  Application developer should use the following functions to get access to
*  arrays and String object:
*       vmGetStringCount
*       vmGetStringArray
*       vmGetArraySize
*       vmGetArray
*  These functions are accessible via vm_config_t structure (see Win32 demo
*  application for examples on how to use them).
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

#include "javavm.h"

/****************************************************************************
* Closes the opened file handle.
* On entry param[1] will contain native file handle.
* On return function should indicate if handle was closed via retval parameter:
*       0   - handle closed successfully
*       -1  - closing has failed
*
* Java prototype:
*           private native int close0(int handle);
*
****************************************************************************/
int16 java_io_FileOutputStream_close0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Opens the named file.
* On entry param[1] will contain reference to String containg the name of a file.
*          param[2] will contain flag indicating whether to open for appending
* On return function should provide file handle via retval parameter:
*       >= 0    - file handle
*       -1      - opening has failed
*
* Java prototype:
*           private native int open0(String name, boolean append);
*
****************************************************************************/
int16 java_io_FileOutputStream_open0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Writes the data to the file.
* On entry param[1] will contain native file handle.
*          param[2] will contain reference to source byte array
*          param[3] will contain offset to source array
*          param[4] will contain number of bytes to write (copy from source array)
* On return function should indicate whether write was successful via retval parameter:
*       0   - write successful
*       -1  - writing has failed
*
* Java prototype:
*           private native int write0(int handle, byte[] buf, int offset, int len);
*
****************************************************************************/
int16 java_io_FileOutputStream_write0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}

