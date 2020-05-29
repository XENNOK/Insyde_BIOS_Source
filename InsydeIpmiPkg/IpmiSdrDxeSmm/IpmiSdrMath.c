/** @file
 H2O IPMI SDR module mathematics functions implement code.

 This c file provides mathematics related functions so that SDR module can
 calculate sensor value from sensor raw data.
 
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <IpmiSdrMath.h>


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754) base raised to the power exponent.

 @param[in]         x                   Base floating point value.
 @param[in]         y                   Exponent floating point value.
 
 @return Base ^ Exponent.
*/
DOUBLE
Ieee754Pow (
  IN  DOUBLE x,
  IN  DOUBLE y
  );


/**
 Compute exponential function.

 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754) 
 base-e exponential function of x, which is the e number raised to the power x.

 @param[in]         x                   Floating point value.
 
 @return Exponential value of x.
*/
DOUBLE
Ieee754Exp (
  IN  DOUBLE x
  );


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 base-e(2.71828...) logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-e(2.71828...) logarithm of x, for values of x greater than zero.
*/
DOUBLE
Ieee754Log (
  IN  DOUBLE x
  );


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 base-10 logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-10 logarithm of x, for values of x greater than zero.
*/
DOUBLE
Ieee754Log10 (
  IN  DOUBLE x
  );


/**
 Returns the natural logarithm of x.

 The natural logarithm is the base-e logarithm, the inverse of the natural
 exponential function (exp). For base-10 logarithms, a specific function log10 exists.

 @param[in]         x                   Floating point value.
 
 @return Natural logarithm of x.
*/
DOUBLE
MathLn (
  IN      DOUBLE x
  )
{
  return Ieee754Log (x);
}


/**
 Compute exponential function.

 Returns the base-e exponential function of x, which is the e number raised to the power x.

 @param[in]         x                   Floating point value.
 
 @return Exponential value of x.
*/
DOUBLE
MathExp (
  IN  DOUBLE x
  )
{
  return Ieee754Exp (x);
}


/**
 Returns the base-10 logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-10 logarithm of x, for values of x greater than zero.
*/
DOUBLE
MathLog10 (
  IN      DOUBLE x
  )
{
  return Ieee754Log10 (x);
}


/**
 Returns the base-2 logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-2 logarithm of x, for values of x greater than zero.
*/
DOUBLE
MathLog2 (
  IN      DOUBLE x
  )
{
  return Ieee754Log (x) / M_LN2;
}


/**
 Returns base-2 raised to the power exponent.

 @param[in]         x                   Floating point value.
 
 @return 2 ^ exponent
*/
DOUBLE
MathPow2 (
  IN      DOUBLE x
  )
{
  return Ieee754Pow (2, x);
}


/**
 Returns base-10 raised to the power exponent.

 @param[in]         x                   Floating point value.
 
 @return 10 ^ exponent
*/
DOUBLE
MathPow10 (
  IN      DOUBLE x
  )
{
  return Ieee754Pow (10, x);
}


/**
 Returns the inverse of a number of x.

 @param[in]         x                   Floating point value.
 
 @return 1 / x
*/
DOUBLE
MathInverse (
  IN      DOUBLE x
  )
{
  return 1.0 / x;
}


/**
 Returns the square of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x * x
*/
DOUBLE
MathSquare (
  IN      DOUBLE x
  )
{
  return x * x;
}


/**
 Returns the Cube of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x * x * x
*/
DOUBLE
MathCube (
  IN      DOUBLE x
  )
{
  return x * x * x;
}


/**
 Returns the square root of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x ^ (1/2)
*/
DOUBLE
MathSquareRoot (
  IN      DOUBLE x
  )
{
  return Ieee754Pow (x, M_1_2);
}


/**
 Returns the cube root of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x ^ (1/3)
*/
DOUBLE
MathCubeRoot (
  IN      DOUBLE x
  )
{
  return Ieee754Pow (x, M_1_3);
}


/**
 Returns base raised to the power exponent.

 @param[in]         Base                Floating point value.
 @param[in]         Exponent            Floating point value.
 
 @return Base ^ Exponent
*/
DOUBLE
MathPower (
  IN      DOUBLE Base,
  IN      DOUBLE Exponent
  )
{
  return Ieee754Pow (Base, Exponent);
}

