/**************************************************************************************
*  j_bcode.c
*
*  This file contains bytecode implementations supported by the simpleRTJ.
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


/****************************************************************************
* _getLocation  - returns abs. location of class' constant pool item
*                 at specified index
* getLocation8  - returns const. pool item location based on 8-bit bytecodes'
*                 argument. PC is advanced by one.
* getLocation16 - returns const. pool item location based on 16-bit bytecodes'
*                 argument. PC is advanced by two.
****************************************************************************/
uint8 *getLocation(uint16 index)
{
    uint8 *ptr;

    /* convert index to offset */
    index <<= (sizeof(uint32) / 2);

    /* get pointer to current frame's class */
    ptr = (uint8 *)thr_active->curr_frame->class_ptr;
    ptr += sizeof(class_t) + index;

    /* calculate bytecode's argument location */
    return ABS_LOC(*((uint32 *)ptr));
}

static uint8 *getLocation16(void)
{
    uint8 *loc;

    loc = getLocation(GET_PC16);
    vm_pc += 2;
    return loc;
}

static uint8 *getLocation8(void)
{
    return getLocation(*vm_pc++);
}


/****************************************************************************
* Generates invalid bytecode exception.
****************************************************************************/
int16 BC_unused(void)
{
    vm_excep = INTERR_InvBytecode;
    return -1;
}

/****************************************************************************
* Does nothing.
****************************************************************************/
int16 BC_nop(void)
{
    return ACTION_NONE;
}

/****************************************************************************
* Jumps to a offset specified by the 16 bit argument {PC+1:PC+2]
* NOTE: on entry PC has already been advanced to the next bytecode
****************************************************************************/
int16 BC_goto(void)
{
    vm_pc = vm_pc - 1 + GET_PC16;

    return ACTION_NONE;
}

/****************************************************************************
* Jumps to a offset specified by the 32 bit argument {PC+1~PC+4]
* NOTE: on entry PC has already been advanced to the next bytecode
****************************************************************************/
int16 BC_goto_w(void)
{
    vm_pc = vm_pc - 1 + GET_PC32;
    return ACTION_NONE;
}

/****************************************************************************
* Loads integer -1, 0, 1, 2, 3, 4 and 5 onto stack.
****************************************************************************/
int16 BC_iconst_m1(void)
{
    vm_sp->i = -1;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iconst_0(void)
{
    vm_sp->i = 0;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iconst_1(void)
{
    vm_sp->i = 1;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iconst_2(void)
{
    vm_sp->i = 2;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iconst_3(void)
{
    vm_sp->i = 3;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iconst_4(void)
{
    vm_sp->i = 4;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iconst_5(void)
{
    vm_sp->i = 5;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Pushes signed byte / short value onto stack.
****************************************************************************/
int16 BC_bipush(void)
{
    vm_sp->i = expandByte(*vm_pc++);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_sipush(void)
{
    vm_sp->i = expandShort(GET_PC16);
    vm_pc += 2;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Get word from local variable n (or implicit one), put it onto stack and
* increment SP.
****************************************************************************/
int16 _iload(uint8 index)
{
    vm_sp->i = thr_active->curr_frame->locals[index].i;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_iload(void)
{
    return _iload(*vm_pc++);
}
int16 BC_iload_0(void)
{
    return _iload(0);
}
int16 BC_iload_1(void)
{
    return _iload(1);
}
int16 BC_iload_2(void)
{
    return _iload(2);
}
int16 BC_iload_3(void)
{
    return _iload(3);
}

/****************************************************************************
* Get word from stack, put it into local variable n (or implicit one) and
* decrement SP.
****************************************************************************/
int16 _istore(uint8 index)
{
    vm_sp--;
    thr_active->curr_frame->locals[index].i = vm_sp->i;
    return ACTION_NONE;
}
int16 BC_istore(void)
{
    return _istore(*vm_pc++);
}
int16 BC_istore_0(void)
{
    return _istore(0);
}
int16 BC_istore_1(void)
{
    return _istore(1);
}
int16 BC_istore_2(void)
{
    return _istore(2);
}
int16 BC_istore_3(void)
{
    return _istore(3);
}

/****************************************************************************
* Common code for array storing/loading bytecodes. Performs checking
* of reference and bounds violation. SP is moved two words down.
*
* return:   array index or -1 if exception occured
****************************************************************************/
static int32 arrCheck(ref_t **ref)
{
    int32 index;

    /* get array index and reference */
    vm_sp--;
    index = vm_sp->i;
    vm_sp--;
    *ref = GET_REF(vm_sp->val);
    if (*ref == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }
    if (index < 0 || index >= (int32)((arrhdr_t *)(*ref)->ptr)->count)
    {
        vm_excep = EXCEP_IndexOutOfBounds;
        return -1;
    }
    return index;
}

/****************************************************************************
* Loads int/float/reference value from the array.
* Top of frame stack contains two params: array index and reference.
****************************************************************************/
int16 BC_iaload(void)
{
    int32 index;
    ref_t *ref;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    vm_sp->i = ((int32 *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index];
    vm_sp++;

    return ACTION_NONE;
}

/****************************************************************************
* Loads byte value from the array and sign expand to an integer value.
* Top of frame stack contains two params: array index and reference.
****************************************************************************/
int16 BC_baload(void)
{
    int32 index;
    ref_t *ref;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    vm_sp->i = expandByte(((int8 *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index]);
    vm_sp++;

    return ACTION_NONE;
}

/****************************************************************************
* Loads short value from the array and sign expand to an integer value.
* Top of frame stack contains two params: array index and reference.
****************************************************************************/
int16 BC_saload(void)
{
    int32 index;
    ref_t *ref;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    vm_sp->i = expandShort(((int16 *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index]);
    vm_sp++;

    return ACTION_NONE;
}

/****************************************************************************
* Loads char value from the array and zero expand to an integer value.
* Top of frame stack contains two params: array index and reference.
****************************************************************************/
int16 BC_caload(void)
{
    int32 index;
    ref_t *ref;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    vm_sp->i = ((jchar *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index];
    vm_sp++;

    return ACTION_NONE;
}

/****************************************************************************
* Stores int/float/reference value in the array.
* Top of frame stack contains three params: value, array index and
* array reference.
****************************************************************************/
int16 BC_iastore(void)
{
    int32 index;
    ref_t *ref;
    value_t value;

    /* get int/float/reference value */
    vm_sp--;
    value.val = vm_sp[0].val;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    ((int32 *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index] = value.i;

    return ACTION_NONE;
}

/****************************************************************************
* Stores byte value in the array.
* Top of frame stack contains three params: byte value, array index and
* reference to array.
****************************************************************************/
int16 BC_bastore(void)
{
    int32 index;
    ref_t *ref;
    value_t value;

    /* get byte value */
    vm_sp--;
    value.val = vm_sp[0].val;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    ((int8 *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index] = (int8)value.i;

    return ACTION_NONE;
}

/****************************************************************************
* Stores char/short value in the array.
* Top of frame stack contains three params: char value, array index and
* reference to array.
****************************************************************************/
int16 BC_sastore(void)
{
    int32 index;
    ref_t *ref;
    value_t value;

    /* get short value */
    vm_sp--;
    value.val = vm_sp[0].val;

    index = arrCheck(&ref);
    if (index < 0)
        return -1;

    ((int16 *)((uint8 *)ref->ptr + sizeof(arrhdr_t)))[index] = (int16)value.i;

    return ACTION_NONE;
}



/****************************************************************************
* Pops two/one word from operand stack.
****************************************************************************/
int16 BC_pop2(void)
{
    vm_sp -= 2;
    return ACTION_NONE;
}
int16 BC_pop(void)
{
    vm_sp--;
    return ACTION_NONE;
}

/****************************************************************************
* Duplicates top word on the operand stack.
****************************************************************************/
int16 BC_dup(void)
{
    value_t value;

    vm_sp--;
    value.val = vm_sp[0].val;
    vm_sp++;
    vm_sp[0].val = value.val;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Duplicates top word on the operand stack and place it two words down.
****************************************************************************/
int16 BC_dup_x1(void)
{
    vm_sp -= 2;
    vm_sp[2].val = vm_sp[1].val;
    vm_sp[1].val = vm_sp[0].val;
    vm_sp[0].val = vm_sp[2].val;
    vm_sp += 3;
    return ACTION_NONE;
}

/****************************************************************************
* Duplicates top word on the operand stack and puts it three words down.
****************************************************************************/
int16 BC_dup_x2(void)
{
    vm_sp -= 3;
    vm_sp[3].val = vm_sp[2].val;
    vm_sp[2].val = vm_sp[1].val;
    vm_sp[1].val = vm_sp[0].val;
    vm_sp[0].val = vm_sp[3].val;
    vm_sp += 4;
    return ACTION_NONE;
}

/****************************************************************************
* Duplicates top two word on the operand stack and puts it three words down.
****************************************************************************/
int16 BC_dup2_x1(void)
{
    vm_sp -= 3;
    vm_sp[4].val = vm_sp[2].val;
    vm_sp[3].val = vm_sp[1].val;
    vm_sp[2].val = vm_sp[0].val;
    vm_sp[1].val = vm_sp[4].val;
    vm_sp[0].val = vm_sp[3].val;
    vm_sp += 5;
    return ACTION_NONE;
}

/****************************************************************************
* Duplicates top two word on the operand stack and puts it three words down.
****************************************************************************/
int16 BC_dup2_x2(void)
{
    vm_sp -= 4;
    vm_sp[5].val = vm_sp[3].val;
    vm_sp[4].val = vm_sp[2].val;
    vm_sp[3].val = vm_sp[1].val;
    vm_sp[2].val = vm_sp[0].val;
    vm_sp[1].val = vm_sp[5].val;
    vm_sp[0].val = vm_sp[4].val;
    vm_sp += 6;
    return ACTION_NONE;
}

/****************************************************************************
* Duplicates two top words on the operand stack.
****************************************************************************/
int16 BC_dup2(void)
{
    vm_sp -= 2;
    vm_sp[2].val = vm_sp[0].val;
    vm_sp[3].val = vm_sp[1].val;
    vm_sp += 4;
    return ACTION_NONE;
}

/****************************************************************************
* Swaps top two words on the stack.
****************************************************************************/
int16 BC_swap(void)
{
    value_t value;

    vm_sp -= 2;
    value.val = vm_sp[0].val;
    vm_sp[0].val = vm_sp[1].val;
    vm_sp[1].val = value.val;
    vm_sp += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Take two words on top of operand stack add them together and pushes
* result onto stack.
****************************************************************************/
int16 BC_iadd(void)
{
    vm_sp -= 2;
    vm_sp[0].i += vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Take two words on top of operand stack subtract top word from top second
* word. It then pushes result onto stack.
****************************************************************************/
int16 BC_isub(void)
{
    vm_sp -= 2;
    vm_sp[0].i -= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Takes the value from operand stack and pushes back result of equation:
* 0 - value
****************************************************************************/
int16 BC_ineg(void)
{
    vm_sp--;
    vm_sp[0].i = -vm_sp[0].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Left shift on integer. Two words value1 and value2 are poped from operand
* stack and shifts value1 to left by the number of bits specified in the
* lowest 5 bits of value2. Result is then pushed onto the stack.
****************************************************************************/
int16 BC_ishl(void)
{
    vm_sp -= 2;
    vm_sp[0].i <<= (uint8)(vm_sp[1].i & 0x1F);
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Arithmetic right shift on integer. Two words value1 and value2 are poped
* from operand stack and right shifts value1 by number of bits specified in
* the lowest 5 bits of value2. Right shift is performed with sign extension.
* Result is then pushed onto the stack.
****************************************************************************/
int16 BC_ishr(void)
{
    uint8 count;

    vm_sp -= 2;
    count = (uint8)(vm_sp[1].i & 0x1F);
    while (count-- > 0)
    {
        if (vm_sp[0].i < 0)
        {
            vm_sp[0].i >>= 1;
            vm_sp[0].i |= 0x80000000;
        }
        else
            vm_sp[0].i >>= 1;
    }
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Logical right shift on integer. Two words value1 and value2 are poped
* from operand stack and right shifts value1 by number of bits specified in
* the lowest 5 bits of value2. Right shift is performed with zero extension.
* Result is then pushed onto the stack.
****************************************************************************/
int16 BC_iushr(void)
{
    vm_sp -= 2;
    vm_sp[0].i = (uint32)vm_sp[0].i >> (uint8)(vm_sp[1].i & 0x1F);
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Pops value1 and value2 from top of operand stack and performs bitwise AND
* operation. Result is pushed onto the stack.
****************************************************************************/
int16 BC_iand(void)
{
    vm_sp -= 2;
    vm_sp[0].i &= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Pops value1 and value2 from top of operand stack and performs bitwise OR
* operation. Result is pushed onto the stack.
****************************************************************************/
int16 BC_ior(void)
{
    vm_sp -= 2;
    vm_sp[0].i |= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Pops value1 and value2 from top of operand stack and performs bitwise XOR
* operation. Result is pushed onto the stack.
****************************************************************************/
int16 BC_ixor(void)
{
    vm_sp -= 2;
    vm_sp[0].i ^= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Sets the flag indicating that next bytecode will have 16 bits index to
* the constant pool.
****************************************************************************/
int16 BC_wide(void)
{
    vm_status |= WIDE_FLAG;
    return ACTION_NONE;
}

/****************************************************************************
* Increments local variable at index specified by [PC+1] by increment value
* specified by [PC+2]. If WIDE_FLAG is set then index is [PC+1:PC+2] and
* increment value is [PC+3]. Increment value is signed 8-bit number.
****************************************************************************/
int16 BC_iinc(void)
{
    uint16 index;
    int16 increment;

    /* get bytecode's parameters */
    if (vm_status & WIDE_FLAG)
    {
        vm_status &= ~WIDE_FLAG;
        index = GET_PC16;
        vm_pc += 2;
        increment = GET_PC16;
        vm_pc += 2;
    }
    else
    {
        index = *vm_pc++;
        increment = *((int8 *)vm_pc);
        vm_pc++;
    }

    /* increment local variable */
    thr_active->curr_frame->locals[index].i += increment;
    return ACTION_NONE;
}

/****************************************************************************
* Data type conversions.
****************************************************************************/
int16 BC_i2b(void)
{
    vm_sp--;
    vm_sp[0].i = expandByte((int8)vm_sp[0].i);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_i2c(void)
{
    vm_sp--;
    vm_sp[0].i = vm_sp[0].i & 0xFFFF;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_i2s(void)
{
    vm_sp--;
    vm_sp[0].i = expandShort((int16)vm_sp[0].i);
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Compares top word on the stack with zero(NULL). If condition is not met
* a jump to  offset specified by [PC+1:PC+2] is made.
* NULL is represented as zero.
****************************************************************************/
int16 BC_ifeq(void)
{
    vm_sp--;
    if (vm_sp[0].i == 0)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}
int16 BC_ifne(void)
{
    vm_sp--;
    if (vm_sp[0].i != 0)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Compares top word on the stack with zero. If it is less then zero
* a jump to  offset specified by [PC+1:PC+2] is made.
****************************************************************************/
int16 BC_iflt(void)
{
    vm_sp--;
    if (vm_sp[0].i < 0)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Compares top word on the stack with zero. If it is less or equal to zero
* a jump to  offset specified by [PC+1:PC+2] is made.
****************************************************************************/
int16 BC_ifle(void)
{
    vm_sp--;
    if (vm_sp[0].i <= 0)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Compares top word on the stack with zero. If it is greater than zero
* a jump to  offset specified by [PC+1:PC+2] is made.
****************************************************************************/
int16 BC_ifgt(void)
{
    vm_sp--;
    if (vm_sp[0].i > 0)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Compares top word on the stack with zero. If it is greater or equal to zero
* a jump to  offset specified by [PC+1:PC+2] is made.
****************************************************************************/
int16 BC_ifge(void)
{
    vm_sp--;
    if (vm_sp[0].i >= 0)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Compares last two words on stack. If condition is not met a jump to
* offset specified by [PC+1:PC+2] is made.
****************************************************************************/
int16 BC_if_icmpeq(void)
{
    vm_sp -= 2;
    if (vm_sp[0].i == vm_sp[1].i)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}
int16 BC_if_icmpne(void)
{
    vm_sp -= 2;
    if (vm_sp[0].i != vm_sp[1].i)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}
int16 BC_if_icmplt(void)
{
    vm_sp -= 2;
    if (vm_sp[0].i < vm_sp[1].i)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}
int16 BC_if_icmple(void)
{
    vm_sp -= 2;
    if (vm_sp[0].i <= vm_sp[1].i)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}
int16 BC_if_icmpge(void)
{
    vm_sp -= 2;
    if (vm_sp[0].i >= vm_sp[1].i)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}
int16 BC_if_icmpgt(void)
{
    vm_sp -= 2;
    if (vm_sp[0].i > vm_sp[1].i)
        return BC_goto();

    vm_pc += 2;
    return ACTION_NONE;
}

/****************************************************************************
* Returns with one word on the stack.
****************************************************************************/
int16 BC_ireturn(void)
{
    vm_status |= RETVAL_FLAG;
    return ACTION_RETURN;
}

/****************************************************************************
* Returns without a value.
****************************************************************************/
int16 BC_return(void)
{
    vm_status &= ~RETVAL_FLAG;
    return ACTION_RETURN;
}


/****************************************************************************
* Called by getfield and putfield to calculate location of instance fields.
*
* Input:        none
* Output:       none
* Returns:      NULL if objref is NULL, otherwise pointer to field value in
*               class instance data is returned.
****************************************************************************/
value_t *findField(void)
{
    field_t *field;
    ref_t *ref;
    value_t *value;

    /* get field definition from bytecode's operand */
    field = (field_t *)getLocation16();
    /* let SP to point to objref */
    vm_sp--;
    ref = GET_REF(vm_sp->val);
    if (ref == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return NULL;
    }

    value = (value_t *)((uint8 *)ref->ptr + sizeof(classhdr_t));
    return &value[FLD_INDEX(field->type_index)];
}


/****************************************************************************
* Getstatic gets the value of the static field and puts it on the stack.
*
* Static field is specified by the 16-bit bytecode operand [PC+1:PC+2] which
* represents index to the offsets table of the current class. Field definition
* specified by the offset table index specifies actual location of the field
* in the static memory area.
****************************************************************************/
int16 BC_getstatic(void)
{
    field_t *field;

    field = (field_t *)getLocation16();
    vm_sp[0].val = static_start[FLD_INDEX(field->type_index)].val;
    vm_sp++;
    return ACTION_NONE;
}


/****************************************************************************
* Putstatic gets the value on the stack and puts it into the static field.
*
* Static field is specified by the 16-bit bytecode operand [PC+1:PC+2] which
* represents index to the offsets table of the current class. Field definition
* specified by the offset table index specifies actual location of the field
* in the static memory area.
****************************************************************************/
int16 BC_putstatic(void)
{
    field_t *field;

    field = (field_t *)getLocation16();
    vm_sp--;
    static_start[FLD_INDEX(field->type_index)].val = vm_sp[0].val;
    return ACTION_NONE;
}


/****************************************************************************
* Gets the field value of the specified class object and puts it onto the
* operand stack. On invokation, operand stack contains objref.
****************************************************************************/
int16 BC_getfield(void)
{
    value_t *value;

    value = findField();
    if (value == NULL)
        return -1;

    vm_sp[0].val = value->val;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Puts the value into the field of the specified class object. Operand stack
* contains objref and value.
****************************************************************************/
int16 BC_putfield(void)
{
    value_t *value;

    vm_sp--;
    value = findField();
    if (value == NULL)
        return -1;

    value->val = vm_sp[1].val;
    return ACTION_NONE;
}


/****************************************************************************
* First, finds the definition of the method to be invoked.
* If the method is native and the calling bytecode is invokespecial or
* invokevirtual, then 'objectref' is ignored.
* Next, it checks if the invoked method is native or not and sets the
* return action code.
*
* Output:       item    - pointer to method definition
****************************************************************************/
int16 invokeVirt(method_t *method);
int16 invokeStat(method_t *method);

int16 BC_invokeinterface(void)
{
    method_t *method;

    vm_status |= OBJREF_FLAG;
    method = (method_t *)getLocation16();

    /* skip over nargs and zero byte */
    vm_pc += 2;
    return invokeVirt(method);
}

int16 BC_invokevirtual(void)
{
    vm_status |= OBJREF_FLAG;
    return invokeVirt((method_t *)getLocation16());
}

int16 BC_invokespecial(void)
{
    method_t *method;

    vm_status |= OBJREF_FLAG;
    method = (method_t *)getLocation16();
    setupMethArgs(method);
    if (GET_REF(vm_args[0].val) == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }
    return invokeStat(method);
}

int16 BC_invokestatic(void)
{
    method_t *method;

    method = (method_t *)getLocation16();
    /* Run <clinit> on method's class if RUN_SPECIAL_FLAG is set */
    if (vm_status & RUN_SPECIAL_FLAG)
    {
        if (!run_clinit(CLASS_LOC(method->class_ptr)))
            return -1;
    }
    vm_status &= ~OBJREF_FLAG;
    setupMethArgs(method);
    return invokeStat(method);
}

/* Helper function for method invoking bytecodes */
int16 invokeVirt(method_t *method)
{
    uint16 i;
    class_t *class_ptr, *class_objref;
    method_tbl_t *method_tbl;

    setupMethArgs(method);
    if (GET_REF(vm_args[0].val) == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }

    /*
    ** Check if specified method is from the class referenced by objref. If it is
    ** the perform quick static invoke. Otherwise, find appropriate method by looking
    ** through the method list of the objref class and its super classes if required.
    ** Methods of java/lang/Object class can also be invoked on arrays in which case
    ** objref is NULL.
    */
    class_ptr = CLASS_LOC(method->class_ptr);
    class_objref = GET_REF(vm_args[0].val)->class_ptr;
    if (class_objref != NULL && class_ptr != class_objref)
    {
        /*
        ** Find method in the class referenced by objref and
        ** if required traverse trough superclasses.
        */
        while (true)
        {
            method_tbl = (method_tbl_t *)ABS_LOC(class_objref->meth_tbl);
            for (i=0; i < (uint16)class_objref->meth_count; i++)
            {
                if (method->hash == (uint16)method_tbl[i].hash)
                {
                    method = METHOD_LOC(method_tbl[i].method);
                    break;
                }
            }
            /* if found -- break the loop */
            if (i < (uint16)class_objref->meth_count)
                break;
            /* check the superclass' method table */
            if (class_objref->super == NULL)
            {
                vm_excep = ERROR_NoSuchMethod;
                return -1;
            }
            class_objref = CLASS_LOC(class_objref->super);
        }
    }
    return invokeStat(method);
}

/* Helper function for method invoking bytecodes */
int16 invokeStat(method_t *method)
{
    bc_item = (uint8 *)method;

    if (method->flags & ACC_ABSTRACT)
    {
        vm_excep = ERROR_AbstractMethod;
        return -1;
    }
    if (method->flags & ACC_NATIVE)
    {
        vm_status &= ~RETVAL_FLAG;
        if (method->flags & ACC_RETVAL)
            vm_status |= RETVAL_FLAG;
        return ACTION_NATIVE;
    }
    return ACTION_INVOKE;
}


/****************************************************************************
* Creates a new object. Two bytes following the bytecode contains index to
* the current class offset table.
*
* Output:       X       pointer to class
****************************************************************************/
int16 BC_new(void)
{
    bc_item = getLocation16();
    return ACTION_NEW;
}


/****************************************************************************
* Creates a new array object. Object reference is put onto java stack.
* Calling rutine must increment SP when appropriate.
*
* Input:    type        - array type as specified by the bytecode's operand
*           size_idx    - value at vm_sp[size_idx] contains number of array elements
* Output:   none
* Returns:  pointer to reference of the new array if successful, otherwise
*                   NULL is returned.
****************************************************************************/
static ref_t *createSimpleArray(uint8 type, uint8 size_idx)
{
    if (vm_sp[size_idx].i < 0)
    {
        vm_excep = EXCEP_NegativeArraySize;
        return NULL;
    }
    return doSimpleArray(type, (uint32)vm_sp[size_idx].i);
}


/****************************************************************************
* Creates a new array on the heap. Bytecode's parameter contains type of
* array to create and stack holds size of the array. Once the array is created
* it is initialised to zero values.
* EXCEP_NegativeArraySize is thrown if size of array is negative.
* EXCEP_NoHeap will be thrown if there is not enough room on the heap to
* hold the new array.
****************************************************************************/
int16 BC_newarray(void)
{
    ref_t *ref;

    vm_sp--;
      ref = createSimpleArray(*vm_pc++, 0);
    vm_sp->val = PUT_REF(ref);
    vm_sp++;

    if (ref == NULL)
        return -1;

    return ACTION_NONE;
}


/****************************************************************************
* Creates a new array of object references. Bytecode's parameter contains
* offset to class definition and stack holds the size of array. Once the
* array is created all elements are initialised to null.
* EXCEP_NegativeArraySize is thrown if size of array is negative.
* EXCEP_NoHeap will be thrown if there is not enough room on the heap to
* hold the new array.
****************************************************************************/
int16 BC_anewarray(void)
{
    ref_t *ref;
    class_t *class_ptr;

    class_ptr = (class_t *)getLocation16();
    vm_sp--;
    ref = createSimpleArray(T_OBJREF, 0);
    vm_sp->val = PUT_REF(ref);
    vm_sp++;

    if (ref == NULL)
        return -1;

    ref->class_ptr = class_ptr;
    return ACTION_NONE;
}

/****************************************************************************
* Creates multidimensional array by recursively calling itself untill all
* dimensions are created and array elements are initialised.
****************************************************************************/
#if ENABLE_MULTIANEWARRAY
ref_t *createMultiArray(uint8 type, uint8 dims, uint8 size_idx)
{
    ref_t *ref, *ref1;
    uint32 *arr;
    int16 size;

    if (dims == 0)
        return NULL;

    if (dims == 1)
        return createSimpleArray(type, size_idx);

    ref = createSimpleArray(T_OBJREF, size_idx);
    if (ref == NULL)
        return NULL;
    ref->class_ptr = CLASS_LOC(APP->object);

    arr = (uint32 *)((uint8 *)ref->ptr + sizeof(arrhdr_t));
    size = (int16)vm_sp[size_idx].i;
    while (--size >= 0)
    {
        ref1 = createMultiArray(type, (uint8)(dims-1), (uint8)(size_idx+1));
        arr[size] = PUT_REF(ref1);
    }

    return ref;
}
#endif

/****************************************************************************
* Creates a new multidimensional array. Bytecode's parameters contain
* offset to array class type definition and number of dimensions. Stack holds
* the size of each dimension.
* During the array creation the elements of first dimension are initialised to
* contain references to the subarray of next dimension and so on. Array elements of
* dimension-1 will contain references to arrays of the type specified in the
* array class type.
* If number of dimensions is one then ordinary single-dimensional array is created.
* EXCEP_NegativeArraySize is thrown if size of any dimension is negative.
* EXCEP_NoHeap will be thrown if there is not enough room on the heap to
* hold the new array.
****************************************************************************/
int16 BC_multianewarray(void)
{
#if ENABLE_MULTIANEWARRAY
    class_arr_t *class_arr;
    int8 dims, i;
    ref_t *ref = NULL;

    /* get class array type and # of dimensions from bytecode's parameters */
    class_arr = (class_arr_t *)getLocation16();
    dims = *vm_pc++;
    vm_sp -= dims;

    for (i=0; i < ALLOC_TRY; i++)
    {
        ref = createMultiArray((uint8)class_arr->type, dims, 0);
        if ((vm_status & GC_DONE_FLAG) == 0)
            break;
    }
    vm_sp->val = PUT_REF(ref);
    vm_sp++;
    if (ref == NULL)
        return -1;

    if (class_arr->type == T_OBJREF)
        ref->class_ptr = CLASS_LOC(class_arr->class_ptr);
    return ACTION_NONE;
#else
    return BC_unused();
#endif
}

/****************************************************************************
* Retrieves the length of the array and pushes it onto stack. On entry,
* stack contains reference to an array. arraylength will generate NullPointer
* exception if array reference is NULL.
****************************************************************************/
int16 BC_arraylength(void)
{
    ref_t *ref;

    vm_sp--;
    ref = GET_REF(vm_sp->val);
    if (ref == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }
    vm_sp->i = (int32)((arrhdr_t *)ref->ptr)->count;
    vm_sp++;
    return ACTION_NONE;
}


/****************************************************************************
* Gets the signed 16 bit offset operand and calculates starting address of
* the subroutine. Then, it pushes return address onto the top of the
* operand stack. Exceution continues with the first bytecode of the
* subroutine.
****************************************************************************/
int16 BC_jsr(void)
{
    /* push return address on stack */
    vm_sp->i = (int32)vm_pc + 2;
    vm_sp++;

    vm_pc = vm_pc - 1 + GET_PC16;
    return ACTION_NONE;
}

/****************************************************************************
* Gets the signed 32 bit offset operand and calculates starting address of
* the subroutine. Then, it pushes return address onto the top of the
* operand stack. Exceution continues with the first bytecode of the
* subroutine.
****************************************************************************/
int16 BC_jsr_w(void)
{
    /* push return address on stack */
    vm_sp->i = (int32)vm_pc + 4;
    vm_sp++;

    vm_pc = vm_pc - 1 + GET_PC32;
    return ACTION_NONE;
}

/****************************************************************************
* Returns from local subroutine. Return address is retrieved from local
* variable specified by the bytecode argument.
****************************************************************************/
int16 BC_ret(void)
{
    vm_pc = (uint8 *)(thr_active->curr_frame->locals[*vm_pc].val);
    return ACTION_NONE;
}



/****************************************************************************
* If item specified by the index is integer or float then the value is
* pushed onto the operand stack. If item is String then ldc creates an
* instance of a String class, initialises it and pushes reference to
* instance of this String onto operand stack.
* ldc takes 8bit index as operand.
* ldc_w takes 16bit index as operand.
****************************************************************************/
int16 ldc(const_t *cons);

int16 BC_ldc_w(void)
{
    return ldc((const_t *)getLocation16());
}
int16 BC_ldc(void)
{
    return ldc((const_t *)getLocation8());
}

int16 ldc(const_t *cons)
{
    ref_t *ref;
    switch (CONST_TYPE(cons->type_len))
    {
        case C_INT:
        case C_FLOAT:
            vm_sp[0].val = CONST_VALUE(cons);
            vm_sp++;
            break;

        case C_STRING:
            /* first check if string with this hashcode has been already loaded */
            ref = findInStrTable(CONST_STR_HASH(cons));
            if (ref == NULL)
            {
                ref = createStringObject(CONST_STR_HASH(cons), CONST_LEN(cons->type_len), CONST_STR(cons));
                if (ref == NULL)
                    return -1;
            }
            vm_sp->val = PUT_REF(ref);
            vm_sp++;
            break;

        default:
            vm_excep = INTERR_UnsupportedType;
            return -1;
    }
    return ACTION_NONE;
}


/****************************************************************************
* Skips over padding zeroes. It is used by tableswitch and lookupswitch
* bytecodes.
*
* Input:        none
* Output:       none
* Return:       address of the first argument following the padding zeroes
****************************************************************************/
static int32 *skipPadding(void)
{
    uint8 lsb = (uint8)((uint32)vm_pc & 3);

    if (lsb > 0)
        return (int32 *)(vm_pc + 4 - lsb);

    return (int32 *)vm_pc;
}

/****************************************************************************
* Gets the index from top of operand stack. First it compares index against
* low and high values which are parameters of the tableswitch bytecode. If
* index is out of the range then a jump to default offset is performed.
* Otherwise, if the index is within the specified range, low value is
* subtracted from index to calculate offset into the table. Table entry at
* calculated offset specifies value which is added to the start of
* tableswitch bytecode to get the destination address.
* Excecution then continues at this address.
****************************************************************************/
#define TS_DEF          0                   /* default offset */
#define TS_LOW          1                   /* low value */
#define TS_HIGH         2                   /* high value */
#define TS_OFFSET       3                   /* first table offset */

int16 BC_tableswitch(void)
{
    int32 *params;
    int16 index;

    params = skipPadding();
    vm_sp--;
    if (vm_sp->i < params[TS_LOW] || vm_sp->i > params[TS_HIGH])
    {
        vm_pc = (vm_pc - 1) + CONVERT(params[TS_DEF]);
        return ACTION_NONE;
    }
    /* calculate index to corresponding offset and setup destination PC*/
    index = (uint16)(vm_sp->i - params[TS_LOW]);
    vm_pc = (vm_pc - 1) + CONVERT(params[TS_OFFSET+index]);
    return ACTION_NONE;
}


/****************************************************************************
* Gets the index from top of operand stack. Then, it retrieves number of
* value/offset pairs. It then compares index agains each value in
* value/offset pairs. If match is found then the destination address is
* calculated by adding offset from matching value to the start of
* lookupswitch bytecode and execution continues at this address.
* If no entry matches the specified index then a jump to default address
* is performed.
****************************************************************************/
#define LS_DEF          0                   /* default offset */
#define LS_COUNT        1                   /* number of value/offset pairs */
#define LS_VALUE        2                   /* value to compare against */
#define LS_OFFSET       3                   /* offset for matching value */

int16 BC_lookupswitch(void)
{
    int32 *params;
    int16 count, index;

    params = skipPadding();
    vm_sp--;

    index = 0;
    count = (int16)params[LS_COUNT];
    while (count-- > 0)
    {
        if (vm_sp->i == params[LS_VALUE+index])
        {
            vm_pc = (vm_pc - 1) + CONVERT(params[LS_OFFSET+index]);
            return ACTION_NONE;
        }
        index += (int16)2;
    }
    /* value not found -- jump to default offset */
    vm_pc = (vm_pc - 1) + CONVERT(params[LS_DEF]);
    return ACTION_NONE;
}


/****************************************************************************
* Performs signed multiply of the value1 and value2 on top of the operand
* stack. Signed result is then pushed onto operand stack.
****************************************************************************/
int16 BC_imul(void)
{
    vm_sp -= 2;
    vm_sp[0].i *= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}


/****************************************************************************
* Performs signed divide of value1 by value2 on top of the operand stack.
* Result is pushed on operand stack. If value2 is zero EXCEP_Arithmetic is
* thrown.
****************************************************************************/
int16 BC_idiv(void)
{
    vm_sp -= 2;
    if (vm_sp[1].i == 0)
    {
        vm_excep = EXCEP_Arithmetic;
        return -1;
    }
    vm_sp[0].i /= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Performs signed divide of value1 by value2 on top of the operand stack.
* Result, the remainer of the division, is pushed on operand stack.
* If value2 is zero EXCEP_Arithmetic is thrown.
****************************************************************************/
int16 BC_irem(void)
{
    vm_sp -= 2;
    if (vm_sp[1].i == 0)
    {
        vm_excep = EXCEP_Arithmetic;
        return -1;
    }
    vm_sp[0].i %= vm_sp[1].i;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Takes the reference to exception Throwable class (or its subclass)
* from the stack.
*
* Output:       item - ptr. to reference of instance of exception class
* Returns:      ACTION_THROW indicating that further processing of athrow is
*               required.
****************************************************************************/
int16 BC_athrow(void)
{
    vm_sp--;
    bc_item = (uint8 *)GET_REF(vm_sp->i);
    if (bc_item == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }
    vm_pc--;
    /* setRelatPC(); */
    return ACTION_THROW;
}


bool checkInstanceOf(ref_t *S, class_t *T);
/****************************************************************************
* Checks if the object (S), which reference is on the stack, can be cast to the
* class (T) referenced by the bytecode's parameters.
* The following rules apply for casting to be successful:
* - if objref is null
* - if S is class and is same as T or is subclass of T
* - if T is interface then S must implement T
* - if S is array:
*       - if T is class then it must be java/lang/Object
*       - if T is an array type then S must be of same primitive type or
*         if array elements are refernce type then S must cast to T using the
*         above rules
****************************************************************************/
int16 BC_checkcast(void)
{
    class_t *T;
    ref_t *S;

    T = (class_t *)getLocation16();
    vm_sp--;
    S = GET_REF(vm_sp->val);
    if (S != NULL)
    {
        if (!checkInstanceOf(S, T))
        {
            vm_excep = EXCEP_ClassCast;
            return -1;
        }
    }
    vm_sp++;
    return ACTION_NONE;
}


/****************************************************************************
* First it pops the objref from operand stack.
* Checks if the object (S), which reference is on the stack, is instance of the
* class (T) referenced by the bytecode's parameters. If S is instance of T then
* it pushes 1 onto stack, otherwise 0 is pushed.
* The following rules apply for casting to be successful:
* - if objref is not null
* - if S is class and is same as T or is subclass of T
* - if T is interface then S must implement T
* - if S is array:
*       - if T is class then it must be java/lang/Object
*       - if T is an array type then S must be of same primitive type or
*         if array elements are refernce type then S must cast to T using the
*         above rules
****************************************************************************/
int16 BC_instanceof(void)
{
    class_t *T;
    ref_t *S;

    T = (class_t *)getLocation16();
    vm_sp--;
    S = GET_REF(vm_sp->val);
    if (S == NULL)
    {
        vm_sp->i = 0;
    }
    else
    {
        vm_sp->i = checkInstanceOf(S, T);
    }
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Performs instanceof/checkcast check according to the rules described in
* headers of appropriate bytecodes.
*
* Input:        S   - reference to object S on the heap
*               T   - pointer to class T definition
* Output:       none
* Returns:      false if check fails, otherwise true is returned
****************************************************************************/
bool checkInstanceOf(ref_t *S, class_t *T)
{
    uint16 arr_type;
    iface_t *ifaces;
    uint8 *loc_T;
    uint16 i;

    if (S->ptr->flag & OF_CLASS)
    {
        /* handle S as class */
        if (T->flags & ACC_INTERFACE)
        {
            if (S->class_ptr->ifaces == NULL)
                return false;
            ifaces = (iface_t *)ABS_LOC(S->class_ptr->ifaces);
            if (ifaces->count == 0)
                return false;

            loc_T = REL_LOC(T);
            for (i=0; i < (uint16)ifaces->count; i++)
            {
                if (loc_T == ifaces->iface[i])
                    return true;
            }
        }
        else
            return isSubclass(S->class_ptr, T);
    }
    else
    {
        /* handle S as array object */
        if (T->flags & ACC_ARRAY_TYPE)
        {
            arr_type = ((arrhdr_t *)S->ptr)->type;
            if (arr_type == T_OBJREF)
                return isSubclass(S->class_ptr, CLASS_LOC(((class_arr_t *)T)->class_ptr));

            if (arr_type == ((class_arr_t *)T)->type)
                return true;
        }
        else if (T->flags & ACC_INTERFACE)
        {
            /* T must be either java.io.Serializable or java.lang.Cloneable */
            return isSubclass(T, CLASS_LOC(APP->cloneable)) ||
                   isSubclass(T, CLASS_LOC(APP->serializable));
        }
        else
        {
            /* T is class type - it must be java/lang/Object */
            if (ABS_LOC(APP->object) == (uint8 *)T)
                return true;
        }
    }

    return false;
}

/****************************************************************************
* Acquire lock on the object whose reference is present on stack
****************************************************************************/
int16 BC_monitorenter(void)
{
    ref_t *ref;

    vm_sp--;
    ref = GET_REF(vm_sp->val);
    if (ref == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }
    acquireObjectLock(thr_active, ref);
    return ACTION_NONE;
}

/****************************************************************************
* Release lock on the object whose reference is present on stack
****************************************************************************/
int16 BC_monitorexit(void)
{
    ref_t *ref;

    vm_sp--;
    ref = GET_REF(vm_sp->val);
    if (ref == NULL)
    {
        vm_excep = EXCEP_NullPointer;
        return -1;
    }
    if (!releaseObjectLock(ref))
        return -1;
    return ACTION_NONE;
}


#if ENABLE_FLOAT
/****************************************************************************
* Load floating constant 0.0, 1.0 and 2.0 on stack
****************************************************************************/
int16 BC_fconst_0(void)
{
    vm_sp->f = 0.0f;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_fconst_1(void)
{
    vm_sp->f = 1.0f;
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_fconst_2(void)
{
    vm_sp->f = 2.0f;
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Floating point arithmetic
****************************************************************************/
int16 BC_fadd(void)
{
    vm_sp -= 2;
    vm_sp[0].f = floatAdd(vm_sp[0].f, vm_sp[1].f);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_fsub(void)
{
    vm_sp -= 2;
    vm_sp[0].f = floatSub(vm_sp[0].f, vm_sp[1].f);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_fmul(void)
{
    vm_sp -= 2;
    vm_sp[0].f = floatMul(vm_sp[0].f, vm_sp[1].f);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_fdiv(void)
{
    vm_sp -= 2;
    vm_sp[0].f = floatDiv(vm_sp[0].f, vm_sp[1].f);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_frem(void)
{
    vm_sp -= 2;
    vm_sp[0].f = floatMod(vm_sp[0].f, vm_sp[1].f);
    vm_sp++;
    return ACTION_NONE;
}

/****************************************************************************
* Common functionality for BC_fcmpg and BC_fcmpl.
****************************************************************************/
static int16 fltcmp(void)
{
    if (floatEQ(vm_sp[0].f, vm_sp[1].f))
    {
        vm_sp[0].i = 0;
    }
    else if (floatGT(vm_sp[0].f, vm_sp[1].f))
    {
        vm_sp[0].i = 1;
    }
    else
    {
        vm_sp[0].i = -1;
    }
    vm_sp++;
    return ACTION_NONE;
}
/****************************************************************************
* Floating compare:
*   BC_fcmpg   should return -1 on NaN
*   BC_fcmpl   should return 1 on NaN
****************************************************************************/
int16 BC_fcmpg(void)
{
    vm_sp -= 2;
    if (isNaN(vm_sp[0].f) || isNaN(vm_sp[1].f))
    {
        vm_sp[0].i = -1;
        vm_sp++;
        return ACTION_NONE;
    }
    return fltcmp();
}
int16 BC_fcmpl(void)
{
    vm_sp -= 2;
    if (isNaN(vm_sp[0].f) || isNaN(vm_sp[1].f))
    {
        vm_sp[0].i = 1;
        vm_sp++;
        return ACTION_NONE;
    }
    return fltcmp();
}
/****************************************************************************
* Floating point conversions
****************************************************************************/
int16 BC_i2f(void)
{
    vm_sp--;
    vm_sp[0].f = int2float(vm_sp[0].i);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_f2i(void)
{
    vm_sp--;
    vm_sp[0].i = float2int(vm_sp[0].f);
    vm_sp++;
    return ACTION_NONE;
}
int16 BC_fneg(void)
{
    vm_sp--;
    vm_sp[0].f = floatNeg(vm_sp[0].f);
    vm_sp++;
    return ACTION_NONE;
}
#else
int16 BC_fconst_0(void) { return BC_unused(); }
int16 BC_fconst_1(void) { return BC_unused(); }
int16 BC_fconst_2(void) { return BC_unused(); }
int16 BC_fcmpl(void) { return BC_unused(); }
int16 BC_fcmpg(void) { return BC_unused(); }
int16 BC_i2f(void) { return BC_unused(); }
int16 BC_f2i(void) { return BC_unused(); }
int16 BC_frem(void) { return BC_unused(); }
int16 BC_fneg(void) { return BC_unused(); }
int16 BC_fsub(void) { return BC_unused(); }
int16 BC_fdiv(void) { return BC_unused(); }
int16 BC_fadd(void) { return BC_unused(); }
int16 BC_fmul(void) { return BC_unused(); }
#endif

const bc_handler bytecode_table[256] =
{
    BC_nop,                         /* 0                */
    BC_iconst_0,                    /* 1 BC_aconst_null */
    BC_iconst_m1,                   /* 2                */
    BC_iconst_0,                    /* 3                */
    BC_iconst_1,                    /* 4                */
    BC_iconst_2,                    /* 5                */
    BC_iconst_3,                    /* 6                */
    BC_iconst_4,                    /* 7                */
    BC_iconst_5,                    /* 8                */
    BC_unused,                      /* 9 BC_lconst_0    */
    BC_unused,                      /* 10 BC_lconst_1   */
    BC_fconst_0,                    /* 11               */
    BC_fconst_1,                    /* 12               */
    BC_fconst_2,                    /* 13               */
    BC_unused,                      /* 14 BC_dconst_0   */
    BC_unused,                      /* 15 BC_dconst_1   */
    BC_bipush,                      /* 16               */
    BC_sipush,                      /* 17               */
    BC_ldc,                         /* 18               */
    BC_ldc_w,                       /* 19               */
    BC_unused,                      /* 20 BC_ldc2_w     */
    BC_iload,                       /* 21               */
    BC_unused,                      /* 22 BC_lload      */
    BC_iload,                       /* 23 BC_fload      */
    BC_unused,                      /* 24 BC_dload      */
    BC_iload,                       /* 25 BC_aload      */
    BC_iload_0,                     /* 26               */
    BC_iload_1,                     /* 27               */
    BC_iload_2,                     /* 28               */
    BC_iload_3,                     /* 29               */
    BC_unused,                      /* 30 BC_lload_0    */
    BC_unused,                      /* 31 BC_lload_1    */
    BC_unused,                      /* 32 BC_lload_2    */
    BC_unused,                      /* 33 BC_lload_3    */
    BC_iload_0,                     /* 34 BC_fload_0    */
    BC_iload_1,                     /* 35 BC_fload_1    */
    BC_iload_2,                     /* 36 BC_fload_2    */
    BC_iload_3,                     /* 37 BC_fload_3    */
    BC_unused,                      /* 38 BC_dload_0    */
    BC_unused,                      /* 39 BC_dload_1    */
    BC_unused,                      /* 40 BC_dload_2    */
    BC_unused,                      /* 41 BC_dload_3    */
    BC_iload_0,                     /* 42 BC_aload_0    */
    BC_iload_1,                     /* 43 BC_aload_1    */
    BC_iload_2,                     /* 44 BC_aload_2    */
    BC_iload_3,                     /* 45 BC_aload_3    */
    BC_iaload,                      /* 46               */
    BC_unused,                      /* 47 BC_laload     */
    BC_iaload,                      /* 48 BC_faload     */
    BC_unused,                      /* 49 BC_daload     */
    BC_iaload,                      /* 50 BC_aaload     */
    BC_baload,                      /* 51               */
    BC_caload,                      /* 52               */
    BC_saload,                      /* 53               */
    BC_istore,                      /* 54               */
    BC_unused,                      /* 55 BC_lstore     */
    BC_istore,                      /* 56 BC_fstore     */
    BC_unused,                      /* 57 BC_dstore     */
    BC_istore,                      /* 58 BC_astore     */
    BC_istore_0,                    /* 59               */
    BC_istore_1,                    /* 60               */
    BC_istore_2,                    /* 61               */
    BC_istore_3,                    /* 62               */
    BC_unused,                      /* 63 BC_lstore_0   */
    BC_unused,                      /* 64 BC_lstore_1   */
    BC_unused,                      /* 65 BC_lstore_2   */
    BC_unused,                      /* 66 BC_lstore_3   */
    BC_istore_0,                    /* 67 BC_fstore_0   */
    BC_istore_1,                    /* 68 BC_fstore_1   */
    BC_istore_2,                    /* 69 BC_fstore_2   */
    BC_istore_3,                    /* 70 BC_fstore_3   */
    BC_unused,                      /* 71 BC_dstore_0   */
    BC_unused,                      /* 72 BC_dstore_1   */
    BC_unused,                      /* 73 BC_dstore_2   */
    BC_unused,                      /* 74 BC_dstore_3   */
    BC_istore_0,                    /* 75 BC_astore_0   */
    BC_istore_1,                    /* 76 BC_astore_1   */
    BC_istore_2,                    /* 77 BC_astore_2   */
    BC_istore_3,                    /* 78 BC_astore_3   */
    BC_iastore,                     /* 79               */
    BC_unused,                      /* 80 BC_lastore    */
    BC_iastore,                     /* 81 BC_fastore    */
    BC_unused,                      /* 82 BC_dastore    */
    BC_iastore,                     /* 83 BC_aastore    */
    BC_bastore,                     /* 84               */
#if ENABLE_UNICODE
    BC_sastore,                     /* 85               */
#else
    BC_bastore,                     /* 85               */
#endif
    BC_sastore,                     /* 86               */
    BC_pop,                         /* 87               */
    BC_pop2,                        /* 88               */
    BC_dup,                         /* 89               */
    BC_dup_x1,                      /* 90               */
    BC_dup_x2,                      /* 91               */
    BC_dup2,                        /* 92               */
    BC_dup2_x1,                     /* 93               */
    BC_dup2_x2,                     /* 94               */
    BC_swap,                        /* 95               */
    BC_iadd,                        /* 96               */
    BC_unused,                      /* 97 BC_ladd       */
    BC_fadd,                        /* 98               */
    BC_unused,                      /* 99 BC_dadd       */
    BC_isub,                        /* 100              */
    BC_unused,                      /* 101 BC_lsub      */
    BC_fsub,                        /* 102              */
    BC_unused,                      /* 103 BC_dsub      */
    BC_imul,                        /* 104              */
    BC_unused,                      /* 105 BC_lmul      */
    BC_fmul,                        /* 106              */
    BC_unused,                      /* 107 BC_dmul      */
    BC_idiv,                        /* 108              */
    BC_unused,                      /* 109 BC_ldiv      */
    BC_fdiv,                        /* 110              */
    BC_unused,                      /* 111 BC_ddiv      */
    BC_irem,                        /* 112              */
    BC_unused,                      /* 113 BC_lrem      */
    BC_frem,                        /* 114              */
    BC_unused,                      /* 115 BC_drem      */
    BC_ineg,                        /* 116              */
    BC_unused,                      /* 117 BC_lneg      */
    BC_fneg,                        /* 118              */
    BC_unused,                      /* 119 BC_dneg      */
    BC_ishl,                        /* 120              */
    BC_unused,                      /* 121 BC_lshl      */
    BC_ishr,                        /* 122              */
    BC_unused,                      /* 123 BC_lshr      */
    BC_iushr,                       /* 124              */
    BC_unused,                      /* 125 BC_lushr     */
    BC_iand,                        /* 126              */
    BC_unused,                      /* 127 BC_land      */
    BC_ior,                         /* 128              */
    BC_unused,                      /* 129 BC_lor       */
    BC_ixor,                        /* 130              */
    BC_unused,                      /* 131 BC_lxor      */
    BC_iinc,                        /* 132              */
    BC_unused,                      /* 133 BC_i2l       */
    BC_i2f,                         /* 134              */
    BC_unused,                      /* 135 BC_i2d       */
    BC_unused,                      /* 136 BC_l2i       */
    BC_unused,                      /* 137 BC_l2f       */
    BC_unused,                      /* 138 BC_l2d       */
    BC_f2i,                         /* 139              */
    BC_unused,                      /* 140 BC_f2l       */
    BC_unused,                      /* 141 BC_f2d       */
    BC_unused,                      /* 142 BC_d2i       */
    BC_unused,                      /* 143 BC_d2l       */
    BC_unused,                      /* 144 BC_d2f       */
    BC_i2b,                         /* 145              */
    BC_i2c,                         /* 146              */
    BC_i2s,                         /* 147              */
    BC_unused,                      /* 148 BC_lcmp      */
    BC_fcmpl,                       /* 149              */
    BC_fcmpg,                       /* 150              */
    BC_unused,                      /* 151 BC_dcmpl     */
    BC_unused,                      /* 152 BC_dcmpg     */
    BC_ifeq,                        /* 153              */
    BC_ifne,                        /* 154              */
    BC_iflt,                        /* 155              */
    BC_ifge,                        /* 156              */
    BC_ifgt,                        /* 157              */
    BC_ifle,                        /* 158              */
    BC_if_icmpeq,                   /* 159              */
    BC_if_icmpne,                   /* 160              */
    BC_if_icmplt,                   /* 161              */
    BC_if_icmpge,                   /* 162              */
    BC_if_icmpgt,                   /* 163              */
    BC_if_icmple,                   /* 164              */
    BC_if_icmpeq,                   /* 165 BC_if_acmpeq */
    BC_if_icmpne,                   /* 166 BC_if_acmpne */
    BC_goto,                        /* 167              */
    BC_jsr,                         /* 168              */
    BC_ret,                         /* 169              */
    BC_tableswitch,                 /* 170              */
    BC_lookupswitch,                /* 171              */
    BC_ireturn,                     /* 172              */
    BC_unused,                      /* 173 BC_lreturn   */
    BC_ireturn,                     /* 174 BC_freturn   */
    BC_unused,                      /* 175 BC_dreturn   */
    BC_ireturn,                     /* 176 BC_areturn   */
    BC_return,                      /* 177              */
    BC_getstatic,                   /* 178              */
    BC_putstatic,                   /* 179              */
    BC_getfield,                    /* 180              */
    BC_putfield,                    /* 181              */
    BC_invokevirtual,               /* 182              */
    BC_invokespecial,               /* 183              */
    BC_invokestatic,                /* 184              */
    BC_invokeinterface,             /* 185              */
    BC_unused,                      /* 186              */
    BC_new,                         /* 187              */
    BC_newarray,                    /* 188              */
    BC_anewarray,                   /* 189              */
    BC_arraylength,                 /* 190              */
    BC_athrow,                      /* 191              */
    BC_checkcast,                   /* 192              */
    BC_instanceof,                  /* 193              */
    BC_monitorenter,                /* 194              */
    BC_monitorexit,                 /* 195              */
    BC_wide,                        /* 196              */
    BC_multianewarray,              /* 197  */
    BC_ifeq,                        /* 198 BC_ifnull    */
    BC_ifne,                        /* 199 BC_ifnonnull */
    BC_goto_w,                      /* 200              */
    BC_jsr_w,                       /* 201              */
    BC_unused,                      /* 202              */
    BC_unused,                      /* 203              */
    BC_unused,                      /* 204              */
    BC_unused,                      /* 205              */
    BC_unused,                      /* 206              */
    BC_unused,                      /* 207              */
    BC_unused,                      /* 208              */
    BC_unused,                      /* 209              */
    BC_unused,                      /* 210              */
    BC_unused,                      /* 211              */
    BC_unused,                      /* 212              */
    BC_unused,                      /* 213              */
    BC_unused,                      /* 214              */
    BC_unused,                      /* 215              */
    BC_unused,                      /* 216              */
    BC_unused,                      /* 217              */
    BC_unused,                      /* 218              */
    BC_unused,                      /* 219              */
    BC_unused,                      /* 220              */
    BC_unused,                      /* 221              */
    BC_unused,                      /* 222              */
    BC_unused,                      /* 223              */
    BC_unused,                      /* 224              */
    BC_unused,                      /* 225              */
    BC_unused,                      /* 226              */
    BC_unused,                      /* 227              */
    BC_unused,                      /* 228              */
    BC_unused,                      /* 229              */
    BC_unused,                      /* 230              */
    BC_unused,                      /* 231              */
    BC_unused,                      /* 232              */
    BC_unused,                      /* 233              */
    BC_unused,                      /* 234              */
    BC_unused,                      /* 235              */
    BC_unused,                      /* 236              */
    BC_unused,                      /* 237              */
    BC_unused,                      /* 238              */
    BC_unused,                      /* 239              */
    BC_unused,                      /* 240              */
    BC_unused,                      /* 241              */
    BC_unused,                      /* 242              */
    BC_unused,                      /* 243              */
    BC_unused,                      /* 244              */
    BC_unused,                      /* 245              */
    BC_unused,                      /* 246              */
    BC_unused,                      /* 247              */
    BC_unused,                      /* 248              */
    BC_unused,                      /* 249              */
    BC_unused,                      /* 250              */
    BC_unused,                      /* 251              */
    BC_unused,                      /* 252              */
    BC_unused,                      /* 253              */
    BC_unused,                      /* 254              */
    BC_unused                       /* 255              */
};





