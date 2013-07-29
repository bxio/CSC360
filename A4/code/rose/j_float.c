/**************************************************************************************
*  j_float.c
*
*  This file contains code implementing 32-bit floating point routines.
*
*  Note that if compiler's floating point routines are not desired or not present
*  then these can be (re)implemented in the functions listed on top of this file.
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
#include "jcommon.h"


#if ENABLE_FLOAT
/****************************************************************************
* The folowing functions are provided to enable implementation of custom floating point
* routines in case that the compiler used code is not desired or not present in
* the FP libraries.
****************************************************************************/
float32 floatAdd(float32 a, float32 b)
{
    return (a + b);
}
float32 floatSub(float32 a, float32 b)
{
    return (a - b);
}
float32 floatMul(float32 a, float32 b)
{
    return (a * b);
}
float32 floatDiv(float32 a, float32 b)
{
    return (a / b);
}
float32 floatMod(float32 a, float32 b)
{
    return (float2int(a) % float2int(b));
}
bool floatEQ(float32 a, float32 b)
{
    if (a == b)
        return true;
    return false;
}
bool floatNE(float32 a, float32 b)
{
    if (a != b)
        return true;
    return false;
}
bool floatGT(float32 a, float32 b)
{
    if (a > b)
        return true;
    return false;
}
bool floatGE(float32 a, float32 b)
{
    if (a >= b)
        return true;
    return false;
}
bool floatLS(float32 a, float32 b)
{
    if (a < b)
        return true;
    return false;
}
bool floatLE(float32 a, float32 b)
{
    if (a <= b)
        return true;
    return false;
}
bool isNaN(float32 value)
{
    uint32 v;

    v = *((uint32 *)(&value));
    if (v == NaN)
        return true;
    return false;
}
float32 floatNeg(float32 value)
{
    return -value;
}
float32 int2float(int32 value)
{
    return (float32)value;
}
int32 float2int(float32 value)
{
    return (int32)value;
}

/****************************************************************************
* Returns true if input character is digit.
****************************************************************************/
bool isDigit(uint8 value)
{
  return (bool)(value >= '0' && value <= '9');
}

/****************************************************************************
* Returns the "integer" part of a float as float number.
*
* Input:        value   floating point number
* Output:       none
* Return:       integer part of the input value
****************************************************************************/
static float32 fint(float32 value)
{
    float32 sign = 1.0f, count, b;

    if (floatLS(value, 1.0f) && floatGT(value, -1.0f))
        return 0.0f;

    if (floatLS(value, 0.0f))
    {
        sign = -1.0f;
        value = floatNeg(value);
    }
    count = fint(floatDiv(value, 32768.0f));
    b = floatSub(value, floatMul(count, 32768.0f));
    b = int2float(float2int(b));
    b = floatAdd(floatMul(count, 32768.0f), b);
    return floatMul(sign, b);
}

/****************************************************************************
* Parses zero terminated byte array and converts character into 32-bit
* floating point number.
*
* Input:        buff    pointer to input byte array
* Output:       value   pointer to float
* Return:       floating number if successfull
****************************************************************************/
bool stringToFloat(uint8 *buff, float32 *value)
{
    int8    sign, expsign;
    int16   exp, eexp;

    while (*buff == ' ' || *buff == '\t')
        buff++;

    expsign = sign = 0;
    if (*buff == '-')
    {
        buff++;
        sign = 1;
    }
    else if (*buff == '+')
        buff++;

    if (!isDigit(*buff))
        return false;

    *value = 0;
    exp = 0;
    while (isDigit(*buff))
        *value = floatAdd(floatMul(10.0f, *value), int2float(*buff++ - '0'));
    if (*buff == '.')
    {
        buff++;
        while (isDigit(*buff))
        {
            exp--;
            *value = floatAdd(floatMul(10.0f, *value), int2float(*buff - '0'));
            buff++;
        }
    }
    eexp = 0;
    if (*buff == 'e' || *buff == 'E')
    {
        if (*++buff == '-')
        {
            expsign = 1;
            buff++;
        }
        if (*buff == '+')
            buff++;
        while (*buff >= '0' && *buff <= '9')
            eexp = (int16)(eexp * 10 + *buff++ - '0');
        if (expsign)
            eexp = (int16)-eexp;
    }
    exp += eexp;
    while (exp < 0)
    {
        *value = floatDiv(*value, 10.0f);
        exp++;
    }
    while (exp > 0)
    {
        *value = floatMul(*value, 10.0f);
        exp--;
    }
    if(sign)
        *value = floatNeg(*value);
    return true;
}


/****************************************************************************
* Converts 32-bit floating number into array of ASCII characters.
*
* Input:        value   floating number that will be converted into string
*               buff    pointer to destination byte array
*               bufflen size of destination array
* Output:       none
* Return:       number of bytes copied into dest. array
****************************************************************************/
int16 floatToString(float32 value, uint8 *buff, uint32 bufflen)
{
#define FLOAT_SIZE  11
    float32 d[6], c, f = 1e-37f;
    int16 e2 = -38;
    int16 i, n, idx = 0;

    if (bufflen < FLOAT_SIZE+1)
        return 0;

    if (isNaN(value))
    {
        buff[0] = 'N';
        buff[1] = 'a';
        buff[2] = 'N';
        return 3;
    }

    d[0] = 1e0f;
    d[1] = 1e1f;
    d[2] = 1e2f;
    d[3] = 1e3f;
    d[4] = 1e4f;
    d[5] = 1e5f;

    if (floatEQ(value, 0.0f))
    {
        for (i=0; i < FLOAT_SIZE; i++)
            buff[i] = '0';
        buff[1] = '.';
        buff[7] = 'E';
        buff[8] = '+';
        return FLOAT_SIZE;
    }

    if (floatLS(value, 0.0f))
    {
        idx = 1;
        buff[0] = '-';
        value = floatNeg(value);
    }

    buff[idx+1] = '.';
    buff[idx+7] = 'E';

    while (floatLE(f, value))
    {
        f = floatMul(f, 10.0f);
        e2++;
    }
    value = floatMul(floatDiv(1.0f, f), value);
    value = floatMul(1e6f, value);

    if (floatEQ(fint(floatAdd(floatDiv(value, d[5]), 0.00001f)), 10.0f))
    {
        value = floatDiv(value, 10.0f);
        e2++;
    }

    if (e2 < 0)
        buff[idx+8] = '-';
    else
        buff[idx+8] = '+';

    if (e2 < 0)
        e2 = (int16)-e2;
    buff[idx+9] = (uint8)((e2 / 10) + '0');
    buff[idx+10] = (uint8)((e2 % 10) + '0');

    for (n=5, i=0; n >= 0; n--, i++)
    {
        c = fint(floatAdd(floatDiv(value, d[n]), 0.00001f));
        buff[idx+i] = (uint8)(float2int(c) + '0');
        if (n == 5)
            i++;
        value = floatSub(value, floatMul(c, d[n]));
    }
    return (buff[0] == '-') ? (int16)(FLOAT_SIZE + 1) : (int16)FLOAT_SIZE;
}

#endif


