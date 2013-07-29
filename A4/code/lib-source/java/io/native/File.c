/**************************************************************************************
*  File.c
*
*  This file defines all native methods that have to be implemented to support
*  java/lang/File class.
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
* Checks is it can be read from this file by checking the access attributes.
* If underlying file system doesn't implement access attributes then this function
* should always return true.
* On entry param[1] will contain reference to String with path to a file.
*
* Java prototype:
*           native private boolean canRead0(String path);
*
****************************************************************************/
int16 java_io_File_canRead0(int32 param[], int32 *retval)
{
    /* Add your code here */

    *retval = true;
    return NO_EXCEP;
}


/****************************************************************************
* Checks is it can be written to this file by checking the access attributes.
* If underlying file system doesn't implement access attributes then this function
* should always return true.
* On entry param[1] will contain reference to String with path to a file.
*
* Java prototype:
*           native private boolean canWrite0(String path);
*
****************************************************************************/
int16 java_io_File_canWrite0(int32 param[], int32 *retval)
{
    /* Add your code here */

    *retval = true;
    return NO_EXCEP;
}


/****************************************************************************
* Deletes a file.
* On entry param[1] will contain reference to String with path to a file.
* On return function should indicate whether deletetion was successfull via
* retval parameter.
*
* Java prototype:
*           native private boolean delete0(String path);
*
****************************************************************************/
int16 java_io_File_delete0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Deletes a directory.
* On entry param[1] will contain reference to String with path to a directory.
* On return function should indicate whether directory was removed via
* retval parameter.
* If underlying file system doesn't implement directories then this function
* should always return true.
*
* Java prototype:
*           native private boolean rmdir0(String path);
*
****************************************************************************/
int16 java_io_File_rmdir0(int32 param[], int32 *retval)
{
    /* Add your code here */

    *retval = true;
    return NO_EXCEP;
}


/****************************************************************************
* Checks if the file exists.
* On entry param[1] will contain reference to String with path to a file.
* On return function should indicate whether file exists via retval parameter.
*
* Java prototype:
*           native private boolean exists0(String path);
*
****************************************************************************/
int16 java_io_File_exists0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Checks if item is a directory.
* On entry param[1] will contain reference to String with path to an item.
* On return function should indicate whether item is directory via
* retval parameter.
* If underlying file system doesn't implement directories then this function
* should always return false.
*
* Java prototype:
*           native private boolean isDir0(String path);
*
****************************************************************************/
int16 java_io_File_isDir0(int32 param[], int32 *retval)
{
    /* Add your code here */

    *retval = false;
    return NO_EXCEP;
}


/****************************************************************************
* Checks if item is a file.
* On entry param[1] will contain reference to String with path to an item.
* On return function should indicate whether item is file via retval parameter.
*
* Java prototype:
*           native private boolean isFile0(String path);
*
****************************************************************************/
int16 java_io_File_isFile0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Get last time modified.
* On entry param[1] will contain reference to String with path to a file.
* Function should return (via retval parameter) time as 32-bit time value.
* If underlying file system doesn't implement file time stamping then this function
* should always return zero.
*
* Java prototype:
*           native private int lastModified0(String path);
*
****************************************************************************/
int16 java_io_File_lastModified0(int32 param[], int32 *retval)
{
    /* Add your code here */

    *retval = 0;
    return NO_EXCEP;
}


/****************************************************************************
* Retrieves the size of a file.
* On entry param[1] will contain reference to String with path to a file.
* On return function should provide file size via retval parameter.
*
* Java prototype:
*           native private int length0(String path);
*
****************************************************************************/
int16 java_io_File_length0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Creates a directory.
* On entry param[1] will contain reference to String with path to a directory.
* On return function should indicate whether directory was created via
* retval parameter.
* If underlying file system doesn't implement access attributes then this function
* should always return false.
*
* Java prototype:
*           native private boolean mkdir0(String path);
*
****************************************************************************/
int16 java_io_File_mkdir0(int32 param[], int32 *retval)
{
    /* Add your code here */

    *retval = false;
    return NO_EXCEP;
}


/****************************************************************************
* Renames source file to destination.
* On entry param[1] will contain reference to String with path to a source file.
*          param[2] will contain reference to String with path to a dest file.
* On return function should indicate whether source file was renamed via
* retval parameter.
*
* Java prototype:
*           native private boolean renameTo0(String source, String dest);
*
****************************************************************************/
int16 java_io_File_renameTo0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Indicates max number of characters that file or directory name can contain.
* On return function should provide max name size via retval parameter.
*
* Java prototype:
*           native private int maxNameLen0();
*
****************************************************************************/
int16 java_io_File_maxNameLen0(int32 param[], int32 *retval)
{
    /* Change to whatever is appropriate for implemented file system */
    *retval = 64;
    return NO_EXCEP;
}


/****************************************************************************
* Retrieves the number of files/dirs in the specified path.
* On entry param[1] will contain reference to String with path to a directory.
* On return function should indicate the count of files/dirs via retval parameter.
* If underlying file system doesn't implement directories then param[1] can
* be ignored.
*
* Java prototype:
*           native private int getDirItemCount0(String path);
*
****************************************************************************/
int16 java_io_File_getDirItemCount0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}


/****************************************************************************
* Retrieves the name of file/directory at specified index and copies the name
* into provided char array.
* On entry param[1] will contain reference to String with path to a directory.
*          param[2] will contain index of file/dir for to retrieve name
*          param[3] will contain reference to char array where file/dir name is
*                   to be copied
* On return function should indicate the name length of file/dir (number of chars
* copied into char array.
* If underlying file system doesn't implement directories then param[1] can
* be ignored.
*
* Java prototype:
*           native private int getDirItem0(String path, int index, char[] name);
*
****************************************************************************/
int16 java_io_File_getDirItem0(int32 param[], int32 *retval)
{
    /* Add your code here */

    return NO_EXCEP;
}

