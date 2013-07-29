/**************************************************************************************
*  FileInputStream.c
*
*  This file defines all native methods that have to be implemented to support
*  java/lang/FileInputStream class.
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
int16 java_io_FileInputStream_close0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Opens the named file.
* On entry param[1] will contain reference to String containg the name of a file.
* On return function should provide file handle via retval parameter:
*       >= 0    - file handle
*       -1      - opening has failed
*
* Java prototype:
*           private native int open0(String name);
*
****************************************************************************/
int16 java_io_FileInputStream_open0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Reads the data from the file.
* On entry param[1] will contain native file handle.
*          param[2] will contain reference to destination byte array
*          param[3] will contain offset to destination array
*          param[4] will contain number of bytes to read (copy to dest. array)
* On return function should provide number of bytes read via retval parameter:
*       >= 0    - actual number of bytes copied into destination array
*       -1      - reading has failed
*
* Java prototype:
*           private native int read0(int handle, byte[] buf, int offset, int len);
*
****************************************************************************/
int16 java_io_FileInputStream_read0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Skips the specified number of bytes.
* On entry param[1] will contain native file handle.
*          param[2] will contain number of bytes to skip
* On return function should provide number of bytes read via retval parameter:
*       >= 0    - actual number of bytes skipped
*       -1      - skipping has failed
*
* Java prototype:
*           private native int skip0(int handle, int num_bytes);
*
****************************************************************************/
int16 java_io_FileInputStream_skip0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Retrieves the file length.
* On entry param[1] will contain native file handle.
* On return function should provide file length via retval parameter:
*       >= 0    - length of the file
*       -1      - get file length failed
*
* Java prototype:
*           private native int getFileLength0(int handle);
*
****************************************************************************/
int16 java_io_FileInputStream_getFileLength0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}

