/** @file
 H2O IPMI SDR module mathematics functions header file.

 This header file provides mathematics related functions prototype that used to
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


#ifndef _IPMI_SDR__MATH_H_
#define _IPMI_SDR__MATH_H_

//
// Constant definitions
//
#define M_PI            3.14159265358979323846264338327950288
#define M_2PI           6.28318530717958647692528676655900576
#define M_3PI_4         2.35619449019234492884698253745962716
#define M_PI_2          1.57079632679489661923132169163975144
#define M_3PI_8         1.17809724509617246442349126872981358
#define M_PI_4          0.78539816339744830961566084581987572
#define M_PI_8          0.39269908169872415480783042290993786
#define M_1_PI          0.31830988618379067153776752674502872
#define M_2_PI          0.63661977236758134307553505349005744
#define M_4_PI          1.27323954473516268615107010698011488
#define M_E             2.71828182845904523536028747135266250
#define M_LOG2E         1.44269504088896340735992468100189213
#define M_LOG10E        0.43429448190325182765112891891660508
#define M_LN2           0.69314718055994530941723212145817657
#define M_LN10          2.30258509299404568401799145468436421
#define M_SQRT2         1.41421356237309504880168872420969808
#define M_1_SQRT2       0.70710678118654752440084436210484904
#define M_EULER         0.57721566490153286060651209008240243
#define M_1_2           0.50000000000000000000000000000000000
#define M_1_3           0.33333333333333333333333333333333333


typedef double DOUBLE;


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
);


/**
 Compute exponential function.

 Returns the base-e exponential function of x, which is the e number raised to the power x.

 @param[in]         x                   Floating point value.
 
 @return Exponential value of x.
*/
DOUBLE
MathExp (
  IN      DOUBLE x
);


/**
 Returns the base-2 logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-2 logarithm of x, for values of x greater than zero.
*/
DOUBLE
MathLog2 (
  IN      DOUBLE x
);


/**
 Returns the base-10 logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-10 logarithm of x, for values of x greater than zero.
*/
DOUBLE
MathLog10 (
  IN      DOUBLE x
);


/**
 Returns base-2 raised to the power exponent.

 @param[in]         x                   Floating point value.
 
 @return 2 ^ exponent
*/
DOUBLE
MathPow2 (
  IN      DOUBLE x
);


/**
 Returns base-10 raised to the power exponent.

 @param[in]         x                   Floating point value.
 
 @return 10 ^ exponent
*/
DOUBLE
MathPow10 (
  IN      DOUBLE x
);


/**
 Returns the inverse of a number of x.

 @param[in]         x                   Floating point value.
 
 @return 1 / x
*/
DOUBLE
MathInverse (
  IN      DOUBLE x
);


/**
 Returns the square of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x * x
*/
DOUBLE
MathSquare (
  IN      DOUBLE x
);


/**
 Returns the Cube of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x * x * x
*/
DOUBLE
MathCube (
  IN      DOUBLE x
);


/**
 Returns the square root of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x ^ (1/2)
*/
DOUBLE
MathSquareRoot (
  IN      DOUBLE x
);


/**
 Returns the cube root of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x ^ (1/3)
*/
DOUBLE
MathCubeRoot (
  IN      DOUBLE x
);


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
);


#endif

