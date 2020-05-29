//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TXT_MISC_H_
#define _TXT_MISC_H_


/*
 *  These three "plus overflow" functions take a "x" value
 *    and add the "y" value to it and if the two values are
 *    greater than the size of the variable type, they will
 *    overflow the type and end up with a smaller value and
 *    return TRUE - that they did overflow.  i.e.
 *    x + y <= variable type maximum.
 */
STATIC BOOLEAN PlusOverflowU32(UINT32 x, UINT32 y)
{
    return ((((UINT32)(~0)) - x) < y);
}

/*
 * This checks to see if two numbers multiplied together are larger
 *   than the type that they are.  Returns TRUE if OVERFLOWING.
 *   If the first parameter "x" is greater than zero and
 *   if that is true, that the largest possible value 0xFFFFFFFF / "x"
 *   is less than the second parameter "y".  If "y" is zero then
 *   it will also fail because no unsigned number is less than zero.
 */
STATIC BOOLEAN MultiplyOverflowU32(UINT32 x, UINT32 y)
{
    return (x > 0) ? ((((UINT32)(~0))/x) < y) : FALSE;
}

#endif    /* _TXT_MISC_H_ */
