/** @file
 H2O IPMI SDR module float point helper functions.

 This c file provides calculating functions of IEEE754 defined float point.
 
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


#define GET_HI_DWORD(x)     *(1+(INT32 *)&x)
#define GET_LO_DWORD(x)     *(INT32 *)&x
#define GET_HI_DWORD_PTR(x) *(1+(INT32 *)x)
#define GET_LO_DWORD_PTR(x) *(INT32 *)x


//
// When using floating-point processors need to include this symbol.
//
int _fltused;
typedef double DOUBLE;


STATIC CONST DOUBLE
  L1  =  5.99999999999994648725e-01,  // 0x3FE33333, 0x33333303
  L2  =  4.28571428578550184252e-01,  // 0x3FDB6DB6, 0xDB6FABFF
  L3  =  3.33333329818377432918e-01,  // 0x3FD55555, 0x518F264D
  L4  =  2.72728123808534006489e-01,  // 0x3FD17460, 0xA91D4101
  L5  =  2.30660745775561754067e-01,  // 0x3FCD864A, 0x93C9DB65
  L6  =  2.06975017800338417784e-01,  // 0x3FCA7E28, 0x4A454EEF
  P1  =  1.66666666666666019037e-01,  // 0x3FC55555, 0x5555553E
  P2  = -2.77777777770155933842e-03,  // 0xBF66C16C, 0x16BEBD93
  P3  =  6.61375632143793436117e-05,  // 0x3F11566A, 0xAF25DE2C
  P4  = -1.65339022054652515390e-06,  // 0xBEBBBD41, 0xC5D26BF1
  P5  =  4.13813679705723846039e-08,  // 0x3E663769, 0x72BEA4D0
  Lg1 =  6.666666666666735130e-01,    // 3FE55555 55555593
  Lg2 =  3.999999999940941908e-01,    // 3FD99999 9997FA04
  Lg3 =  2.857142874366239149e-01,    // 3FD24924 94229359
  Lg4 =  2.222219843214978396e-01,    // 3FCC71C5 1D8E78AF
  Lg5 =  1.818357216161805012e-01,    // 3FC74664 96CB03DE
  Lg6 =  1.531383769920937332e-01,    // 3FC39A09 D078C69F
  Lg7 =  1.479819860511658591e-01,    // 3FC2F112 DF3E5244
  zero        =  0.0,
  one         =  1.0,
  two         =  2.0,
  huge        =  1.79769e+308,
  tiny        =  2.22507e-308,
  lg2         =  6.93147180559945286227e-01,    // 0x3FE62E42, 0xFEFA39EF
  lg2_h       =  6.93147182464599609375e-01,    // 0x3FE62E43, 0x00000000
  lg2_l       = -1.90465429995776804525e-09,    // 0xBE205C61, 0x0CA86C39
  cp          =  9.61796693925975554329e-01,    // 0x3FEEC709, 0xDC3A03FD =2/(3ln2)
  cp_h        =  9.61796700954437255859e-01,    // 0x3FEEC709, 0xE0000000 =(float)cp
  cp_l        = -7.02846165095275826516e-09,    // 0xBE3E2FE0, 0x145B01F5 =tail of cp_h
  ivln2       =  1.44269504088896338700e+00,    // 0x3FF71547, 0x652B82FE =1/ln2
  ivln2_h     =  1.44269502162933349609e+00,    // 0x3FF71547, 0x60000000 =24b 1/ln2
  ivln2_l     =  1.92596299112661746887e-08,    // 0x3E54AE0B, 0xF85DDF44 =1/ln2 tail
  ivln10      =  4.34294481903251816668e-01,    // 0x3FDBCB7B, 0x1526E50E
  log10_2hi   =  3.01029995663611771306e-01,    // 0x3FD34413, 0x509F6000
  log10_2lo   =  3.69423907715893078616e-13,    // 0x3D59FEF3, 0x11F12B36
  ln2_hi      =  6.93147180369123816490e-01,    // 3fe62e42 fee00000
  ln2_lo      =  1.90821492927058770002e-10,    // 3dea39ef 35793c76
  two53       =  9007199254740992.0,            // 0x43400000, 0x00000000
  two54       =  1.80143985094819840000e+16,    // 0x43500000, 0x00000000
  twom54      =  5.55111512312578270212e-17,    // 0x3C900000, 0x00000000
  twom1000    =  9.33263618503218878990e-302,   // 2**-1000=0x01700000,0
  o_threshold =  7.09782712893383973096e+02,    // 0x40862E42, 0xFEFA39EF
  u_threshold = -7.45133219101941108420e+02,    // 0xc0874910, 0xD52D3051
  ovt         =  8.0085662595372944372e-0017,   // -(1024-log2(ovfl+.5ulp))
  invln2      =  1.44269504088896338700e+00,    // 0x3ff71547, 0x652b82fe
  dp_h[]      = { 0.0, 5.84962487220764160156e-01,},  // 0x3FE2B803, 0x40000000
  dp_l[]      = { 0.0, 1.35003920212974897128e-08,},  // 0x3E4CFDEB, 0x43CFD006
  bp[]        = { 1.0, 1.5,},
  halF[2]     = { 0.5, -0.5,},
  ln2HI[2]    = { 6.93147180369123816490e-01,     // 0x3fe62e42, 0xfee00000
                 -6.93147180369123816490e-01, },  // 0xbfe62e42, 0xfee00000
  ln2LO[2]    = { 1.90821492927058770002e-10,     // 0x3dea39ef, 0x35793c76
                 -1.90821492927058770002e-10, };  // 0xbdea39ef, 0x35793c76


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 composes a floating point value with the magnitude of x and the sign of y.

 @param[in]         x                   Floating point value.
 @param[in]         y                   Floating point value.
 
 @return x with the sign of y.
*/
DOUBLE 
Ieee754CopySign (
  IN  DOUBLE  x, 
  IN  DOUBLE  y
  )
{
  GET_HI_DWORD (x) = (GET_HI_DWORD (x) & 0x7fffffff) | (GET_HI_DWORD (y) & 0x80000000);
  return x;
}


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 calculates the absolute value of a number of x.

 @param[in]         x                   The floating point number that you want to 
                                        calculates the absolute value.
 
 @return |x|
*/
DOUBLE
Ieee754Fabs (
  IN  DOUBLE  x
  )
{
  GET_HI_DWORD (x) &= 0x7fffffff;
  return x;
}


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 exponent a radix-independent of a number of x.

 @param[in]         x                   The floating point number that you want to multiply by the exponent.
 @param[in]         n                   The exponent to apply to the radix of the machine's floating-point arithmetic.
 
 @return x * (r ^ n). Where r is the radix of the machine's floating point arithmetic.
*/
DOUBLE
Ieee754Scalbn (
  IN  DOUBLE  x,
  IN  INT32   n
  )
{
  INT32  k;
  INT32  hx;
  INT32  lx;

  hx = GET_HI_DWORD (x);
  lx = GET_LO_DWORD (x);
  k = (hx & 0x7ff00000) >> 20;            // extract exponent 

  if (k == 0) {                           // 0 or subnormal x 

    if ( (lx | (hx & 0x7fffffff)) == 0) {
      return x;  // +-0 
    }

    x *= two54;
    hx = GET_HI_DWORD (x);
    k = ( (hx & 0x7ff00000) >> 20) - 54;

    if (n < -50000) {
      return tiny * x;  // underflow
    }
  }

  if (k == 0x7ff) {
    return x + x;  // NaN or Inf 
  }

  k = k + n;

  if (k >  0x7fe) {
    return huge * Ieee754CopySign (huge, x);  // overflow  
  }

  if (k > 0) {                            // normal result 
    GET_HI_DWORD (x) = (hx & 0x800fffff) | (k << 20);
    return x;
  }

  if (k <= -54) {
    if (n > 50000) {    // in case integer overflow in n+k 
      return huge * Ieee754CopySign (huge, x);  // overflow

    } else {
      return tiny * Ieee754CopySign (tiny, x);  // underflow
    }
  }
  
  k += 54;                                // subnormal result 
  GET_HI_DWORD (x) = (hx & 0x800fffff) | (k << 20);
  return x * twom54;
}


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754) square root of a number of x.

 @param[in]         x                   Floating point value.
 
 @return x ^ (1/2)
*/
DOUBLE 
Ieee754Sqrt (
  IN  DOUBLE  x
  )
{
  DOUBLE  z;
  INT32   sign;
  UINT32  r;
  UINT32  t1;
  UINT32  s1;
  UINT32  ix1;
  UINT32  q1;
  INT32   ix0;
  INT32   s0;
  INT32   q;
  INT32   m;
  INT32   t;
  INT32   i;

  ix0 = GET_HI_DWORD (x);         // high word of x 
  ix1 = GET_LO_DWORD (x);         // low word of x 

  //
  // take care of Inf and NaN 
  //
  if ( (ix0 & 0x7ff00000) == 0x7ff00000) {

    //
    // ieee_sqrt(NaN)=NaN, ieee_sqrt(+inf)=+inf
    //                     ieee_sqrt(-inf)=sNaN
    //
    return x * x + x;               
  }

  //
  // take care of zero 
  //
  sign = (INT32) 0x80000000;
  if (ix0 <= 0) {
    if ( ( (ix0 & (~sign)) | ix1) == 0) {

      //
      // ieee_sqrt(+-0) = +-0 
      //
      return x;      

    } else if (ix0 < 0) {

      //
      // ieee_sqrt(-ve) = sNaN 
      //
      return (x - x) / (x - x);  

    }
  }
  
  //
  // normalize x 
  //
  m = (ix0 >> 20);
  if (m == 0) {                           
    //
    // subnormal x 
    //
    
    while (ix0 == 0) {
      m -= 21;
      ix0 |= (ix1 >> 11);
      ix1 <<= 21;
    }

    for (i = 0; (ix0 & 0x00100000) == 0; i++) {
      ix0 <<= 1;
    }
    m -= i - 1;
    ix0 |= (ix1 >> (32 - i));
    ix1 <<= i;
  }

  m -= 1023;      // unbias exponent 
  ix0 = (ix0 & 0x000fffff) | 0x00100000;

  if (m & 1) {    // odd m, DOUBLE x to make it even 
    ix0 += ix0 + ( (ix1 & sign) >> 31);
    ix1 += ix1;
  }

  m >>= 1;        // m = [m/2] 

  //
  // generate ieee_sqrt(x) bit by bit 
  //
  ix0 += ix0 + ( (ix1 & sign) >> 31);
  ix1 += ix1;
  q = q1 = s0 = s1 = 0;   // [q,q1] = ieee_sqrt(x) 
  r = 0x00200000;         // r = moving bit from right to left 

  while (r != 0) {
    t = s0 + r;
    if (t <= ix0) {
      s0   = t + r;
      ix0 -= t;
      q   += r;
    }
    ix0 += ix0 + ( (ix1 & sign) >> 31);
    ix1 += ix1;
    r >>= 1;
  }

  r = sign;
  while (r != 0) {
    t1 = s1 + r;
    t  = s0;

    if ( (t < ix0) || ( (t == ix0) && (t1 <= ix1))) {
      s1  = t1 + r;

      if ( ( (t1 & sign) == (UINT32)(sign)) && (s1 & sign) == 0) {
        s0 += 1;
      }
      ix0 -= t;

      if (ix1 < t1) {
        ix0 -= 1;
      }
      ix1 -= t1;
      q1  += r;
    }
    ix0 += ix0 + ( (ix1 & sign) >> 31);
    ix1 += ix1;
    r >>= 1;
  }

  //
  // use floating add to find out rounding direction 
  //
  if ( (ix0 | ix1) != 0) {
    z = one - tiny;         // trigger inexact flag 
    if (z >= one) {
      z = one + tiny;

      if (q1 == (UINT32) 0xffffffff) {
        q1 = 0;
        q += 1;

      } else if (z > one) {
        if (q1 == (UINT32) 0xfffffffe) {
          q += 1;
        }
        q1 += 2;

      } else {
        q1 += (q1 & 1);
      }
    }
  }
  
  ix0 = (q >> 1) + 0x3fe00000;
  ix1 =  q1 >> 1;

  if ( (q & 1) == 1) {
    ix1 |= sign;
  }

  ix0 += (m << 20);
  GET_HI_DWORD (z) = ix0;
  GET_LO_DWORD (z) = ix1;
  return z;
}


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
  )
{
  DOUBLE  z;
  DOUBLE  ax;
  DOUBLE  z_h;
  DOUBLE  z_l;
  DOUBLE  p_h;
  DOUBLE  p_l;
  DOUBLE  y1;
  DOUBLE  t1;
  DOUBLE  t2;
  DOUBLE  r;
  DOUBLE  s;
  DOUBLE  t;
  DOUBLE  u;
  DOUBLE  v;
  DOUBLE  w;
  INT32   i0;
  INT32   i1;
  INT32   i;
  INT32   j;
  INT32   k;
  INT32   yisint;
  INT32   n;
  INT32   hx;
  INT32   hy;
  INT32   ix;
  INT32   iy;
  UINT32  lx;
  UINT32  ly;

  i0 = ( (* (INT32 *) &one) >> 29) ^ 1;
  i1 = 1 - i0;
  hx = GET_HI_DWORD (x);
  lx = GET_LO_DWORD (x);
  hy = GET_HI_DWORD (y);
  ly = GET_LO_DWORD (y);
  ix = hx & 0x7fffffff;
  iy = hy & 0x7fffffff;

  //
  // y==zero: x**0 = 1 
  //
  if ( (iy | ly) == 0) {
    return one;
  }

  //
  // +-NaN return x+y 
  //
  if (ix > 0x7ff00000 || ( (ix == 0x7ff00000) && (lx != 0)) ||
      iy > 0x7ff00000 || ( (iy == 0x7ff00000) && (ly != 0))) {
    return x + y;
  }

  //
  // determine if y is an odd INT32 when x < 0
  // * yisint = 0       ... y is not an integer
  // * yisint = 1       ... y is an odd INT32
  // * yisint = 2       ... y is an even INT32
  //  
  yisint  = 0;
  if (hx < 0) {
    if (iy >= 0x43400000) {
      yisint = 2;                   // even integer y 

    } else if (iy >= 0x3ff00000) {
      k = (iy >> 20) - 0x3ff;       // exponent 

      if (k > 20) {
        j = ly >> (52 - k);
        if ( (UINT32)(j << (52 - k)) == ly) {
          yisint = 2 - (j & 1);
        }

      } else if (ly == 0) {
        j = iy >> (20 - k);
        if ( (j << (20 - k)) == iy) {
          yisint = 2 - (j & 1);
        }
      }
    }
  }

  //
  // special value of y 
  //
  if (ly == 0) {
    if (iy == 0x7ff00000) {                   // y is +-inf 
      if ( ( (ix - 0x3ff00000) | lx) == 0) {
        return  y - y;                        // inf**+-1 is NaN 

      } else if (ix >= 0x3ff00000) {          // (|x|>1)**+-inf = inf,0 
        return (hy >= 0) ? y : zero;

      } else {                                // (|x|<1)**-,+inf = inf,0 
        return (hy < 0) ? -y : zero;
      }
    }
    if (iy == 0x3ff00000) {                   // y is  +-1 
      if (hy < 0) {
        return one / x;

      } else {
        return x;
      }
    }
    if (hy == 0x40000000) {
      return x * x;                           // y is  2 
    }
    if (hy == 0x3fe00000) {                   // y is  0.5 
      if (hx >= 0) {                          // x >= +0 
        return Ieee754Sqrt (x);
      }
    }
  }

  ax   = Ieee754Fabs (x);

  //
  // special value of x 
  //
  if (lx == 0) {
    if (ix == 0x7ff00000 || ix == 0 || ix == 0x3ff00000) {
      z = ax;                               // x is +-0,+-inf,+-1
      if (hy < 0) {
        z = one / z;                        // z = (1/|x|) 
      }

      if (hx < 0) {
        if ( ( (ix - 0x3ff00000) | yisint) == 0) {
          z = (z - z) / (z - z);            // (-1)**non-INT32 is NaN 

        } else if (yisint == 1) {
          z = -1.0 * z;                     // (x<0)**odd = -(|x|**odd) 
        }
      }
      return z;
    }
  }

  n = (hx >> 31) + 1;

  //
  // (x<0)**(non-INT32) is NaN 
  //
  if ( (n | yisint) == 0) {
    return (x - x) / (x - x);
  }

  s = one;                          // s (sign of result -ve**odd) = -1 else = 1 
  if ( (n | (yisint - 1)) == 0) {
    s = -one;                       // (-ve)**(odd INT32) 
  }

  //
  // |y| is huge 
  //
  if (iy > 0x41e00000) {            // if |y| > 2**31 
    if (iy > 0x43f00000) {          // if |y| > 2**64, must o/uflow 

      if (ix <= 0x3fefffff) {
        return (hy < 0) ? huge * huge : tiny * tiny;
      }

      if (ix >= 0x3ff00000) {
        return (hy > 0) ? huge * huge : tiny * tiny;
      }

    }
    //
    // over/underflow if x is not close to one 
    //
    if (ix < 0x3fefffff) {
      return (hy < 0) ? s * huge * huge : s * tiny * tiny;
    }

    if (ix > 0x3ff00000) {
      return (hy > 0) ? s * huge * huge : s * tiny * tiny;
    }

    //
    // now |1-x| is tiny <= 2**-20, suffice to compute
    //   log(x) by x-x^2/2+x^3/3-x^4/4 
    //
    t = ax - one;                         // t has 20 trailing zeros 
    w = (t * t) * (0.5 - t * (0.3333333333333333333333 - t * 0.25));
    u = ivln2_h * t;                      // ivln2_h has 21 sig. bits 
    v = t * ivln2_l - w * ivln2;
    t1 = u + v;
    GET_LO_DWORD (t1) = 0;
    t2 = v - (t1 - u);

  } else {
    DOUBLE ss;
    DOUBLE s2;
    DOUBLE s_h;
    DOUBLE s_l;
    DOUBLE t_h;
    DOUBLE t_l;
    
    n = 0;

    //
    // take care subnormal number 
    //
    if (ix < 0x00100000) {
      ax *= two53;
      n -= 53;
      ix = GET_HI_DWORD (ax);
    }

    
    n  += ( (ix) >> 20) - 0x3ff;
    j  = ix & 0x000fffff;
    //
    // determine interval 
    //
    ix = j | 0x3ff00000;        // normalize ix 
    if (j <= 0x3988E) {
      k = 0;                    // |x|<sqrt(3/2) 

    } else if (j < 0xBB67A) {
      k = 1;                    // |x|<sqrt(3)   

    } else {
      k = 0;
      n += 1;
      ix -= 0x00100000;
    }
    GET_HI_DWORD (ax) = ix;

    //
    // compute ss = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) 
    //
    u = ax - bp[k];                     // bp[0]=1.0, bp[1]=1.5 
    v = one / (ax + bp[k]);
    ss = u * v;
    s_h = ss;
    GET_LO_DWORD (s_h) = 0;

    
    //
    // t_h=ax+bp[k] High 
    //
    t_h = zero;
    GET_HI_DWORD (t_h) = ( (ix >> 1) | 0x20000000) + 0x00080000 + (k << 18);
    t_l = ax - (t_h - bp[k]);
    s_l = v * ( (u - s_h * t_h) - s_h * t_l);

    //
    // compute log(ax) 
    //
    s2 = ss * ss;
    r = s2 * s2 * (L1 + s2 * (L2 + s2 * (L3 + s2 * (L4 + s2 * (L5 + s2 * L6)))));
    r += s_l * (s_h + ss);
    s2  = s_h * s_h;
    t_h = 3.0 + s2 + r;
    GET_LO_DWORD (t_h) = 0;
    t_l = r - ( (t_h - 3.0) - s2);

    //
    // u+v = ss*(1+...) 
    //
    u = s_h * t_h;
    v = s_l * t_h + t_l * ss;

    //
    // 2/(3log2)*(ss+...) 
    //
    p_h = u + v;
    GET_LO_DWORD (p_h) = 0;
    p_l = v - (p_h - u);
    z_h = cp_h * p_h;                       // cp_h+cp_l = 2/(3*log2) 
    z_l = cp_l * p_h + p_l * cp + dp_l[k];

    //
    // log2(ax) = (ss+..)*2/(3*log2) = n + dp_h + z_h + z_l 
    //
    t = (DOUBLE) n;
    t1 = ( ( (z_h + z_l) + dp_h[k]) + t);
    GET_LO_DWORD (t1) = 0;
    t2 = z_l - ( ( (t1 - t) - dp_h[k]) - z_h);
  }

  //
  // split up y into y1+y2 and compute (y1+y2)*(t1+t2) 
  //
  y1  = y;
  GET_LO_DWORD (y1) = 0;
  p_l = (y - y1) * t1 + y * t2;
  p_h = y1 * t1;
  z = p_l + p_h;
  j = GET_HI_DWORD (z);
  i = GET_LO_DWORD (z);
  if (j >= 0x40900000) {                          // z >= 1024 

    if ( ( (j - 0x40900000) | i) != 0) {        // if z > 1024 
      return s * huge * huge;  // overflow 

    } else {
      if (p_l + ovt > z - p_h) {
        return s * huge * huge;  // overflow 
      }
    }
    
  } else if ( (j & 0x7fffffff) >= 0x4090cc00) {   // z <= -1075 

    if ( ( (j - 0xc090cc00) | i) != 0) { // z < -1075 
      return s * tiny * tiny;  // underflow 

    } else {
      if (p_l <= z - p_h) {
        return s * tiny * tiny;  // underflow 
      }
    }

  }

  //
  // * compute 2**(p_h+p_l)
  // 
  i = j & 0x7fffffff;
  k = (i >> 20) - 0x3ff;
  n = 0;
  if (i > 0x3fe00000) {           // if |z| > 0.5, set n = [z+0.5] 

    n = j + (0x00100000 >> (k + 1));
    k = ( (n & 0x7fffffff) >> 20) - 0x3ff; // new k for n 
    t = zero;
    GET_HI_DWORD (t) = (n&~ (0x000fffff >> k));
    n = ( (n & 0x000fffff) | 0x00100000) >> (20 - k);
    if (j < 0) {
      n = -n;
    }
    p_h -= t;

  }
  t = p_l + p_h;
  GET_LO_DWORD (t) = 0;
  u = t * lg2_h;
  v = (p_l - (t - p_h)) * lg2 + t * lg2_l;
  z = u + v;
  w = v - (z - u);
  t  = z * z;
  t1  = z - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
  r  = (z * t1) / (t1 - two) - (w + z * w);
  z  = one - (r - z);
  j  = GET_HI_DWORD (z);
  j += (n << 20);
  if ( (j >> 20) <= 0) {
    z = Ieee754Scalbn (z, n);  // subnormal output 
  } else {
    GET_HI_DWORD (z) += (n << 20);
  }
  return s * z;
}


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
  ) 
{
  DOUBLE y;
  DOUBLE hi;
  DOUBLE lo;
  DOUBLE c;
  DOUBLE t;
  INT32  k;
  INT32  xsb;
  UINT32 hx;

  hx  = GET_HI_DWORD (x);         // high word of x 
  xsb = (hx >> 31) & 1;           // sign bit of x 
  hx &= 0x7fffffff;               // high word of |x| 

  //
  // filter out non-finite argument 
  //
  if (hx >= 0x40862E42) {                 // if |x|>=709.78... 
    if (hx >= 0x7ff00000) {
      if ( ( (hx & 0xfffff) | GET_LO_DWORD (x)) != 0) {
        return x + x;                     // NaN 

      } else {
        return (xsb == 0) ? x : 0.0;
      }                                   // exp(+-inf)={inf,0} 

    }
    
    if (x > o_threshold) {
      return huge * huge;                 // overflow 
    }
    
    if (x < u_threshold) {
      return twom1000 * twom1000;         // underflow 
    }
    
  }

  //
  // argument reduction 
  //
  hi = 0;
  lo = 0;
  k = 0;
  if (hx > 0x3fd62e42) {          // if  |x| > 0.5 ln2 
    if (hx < 0x3FF0A2B2) {        // and |x| < 1.5 ln2 
      hi = x - ln2HI[xsb];
      lo = ln2LO[xsb];
      k = 1 - xsb - xsb;

    } else {
      k  = (INT32) (invln2 * x + halF[xsb]);
      t  = k;
      hi = x - t * ln2HI[0];      // t*ln2HI is exact here 
      lo = t * ln2LO[0];
    }
    x  = hi - lo;

  } else if (hx < 0x3e300000)  {  // when |x|<2**-28 
    if (huge + x > one) {
      return one + x;             // trigger inexact 
    }

  } else {
    k = 0;
  }

  //
  // x is now in primary range 
  //
  t  = x * x;
  c  = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
  if (k == 0) {
    return one - ( (x * c) / (c - 2.0) - x);

  } else {
    y = one - ( (lo - (x * c) / (2.0 - c)) - hi);
  }

  if (k >= -1021) {
    GET_HI_DWORD (y) += (k << 20);            // add k to y's exponent 
    return y;

  } else {
    GET_HI_DWORD (y) += ( (k + 1000) << 20);  // add k to y's exponent 
    return y * twom1000;
  }
}


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 base-e(2.71828...) logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-e(2.71828...) logarithm of x, for values of x greater than zero.
*/
DOUBLE
Ieee754Log (
  IN  DOUBLE x
  )
{ 
  DOUBLE  hfsq;
  DOUBLE  f;
  DOUBLE  s;
  DOUBLE  z;
  DOUBLE  R;
  DOUBLE  w;
  DOUBLE  t1;
  DOUBLE  t2;
  DOUBLE  dk;
  INT32   k;
  INT32   hx;
  INT32   i;
  INT32   j;
  UINT32  lx;


  hx = GET_HI_DWORD (x);          // high word of x 
  lx = GET_LO_DWORD (x);          // low  word of x 

  k = 0;
  if (hx < 0x00100000) {                  // x < 2**-1022  
    if ( ( (hx & 0x7fffffff) | lx) == 0) {
      GET_HI_DWORD(x) = 0xfff00000;
      return x;  // log(+-0)=-inf 
    }

    if (hx < 0) {
      GET_HI_DWORD(x) = 0x7FF00000;
      GET_LO_DWORD(x) = 0x1;      
      return x;  // log(-#) = NaN 
    }
    k -= 54;
    x *= two54; // subnormal number, scale up x 
    hx = GET_HI_DWORD (x);              // high word of x 
  }

  if (hx >= 0x7ff00000) {
    return x + x;
  }

  k += (hx >> 20) - 1023;
  hx &= 0x000fffff;
  i = (hx + 0x95f64) & 0x100000;
  GET_HI_DWORD (x) = hx | (i ^ 0x3ff00000); // normalize x or x/2 
  k += (i >> 20);
  f = x - 1.0;
  if ( (0x000fffff & (2 + hx)) < 3) { // |f| < 2**-20 

    if (f == zero) {
      if (k == 0) {
        return zero;

      } else {
        dk = (DOUBLE) k;
        return dk * ln2_hi + dk * ln2_lo;
      }
    }

    R = f * f * (0.5 - 0.33333333333333333 * f);

    if (k == 0) {
      return f - R;

    } else {
      dk = (DOUBLE) k;
      return dk * ln2_hi - ( (R - dk * ln2_lo) - f);
    }
  }

  s = f / (2.0 + f);
  dk = (DOUBLE) k;
  z = s * s;
  i = hx - 0x6147a;
  w = z * z;
  j = 0x6b851 - hx;
  t1 = w * (Lg2 + w * (Lg4 + w * Lg6));
  t2 = z * (Lg1 + w * (Lg3 + w * (Lg5 + w * Lg7)));
  i |= j;
  R = t2 + t1;
  if (i > 0) {
    hfsq = 0.5 * f * f;
    if (k == 0) {
      return f - (hfsq - s * (hfsq + R));

    } else {
      return dk * ln2_hi - ( (hfsq - (s * (hfsq + R) + dk * ln2_lo)) - f);
    }

  } else {
    if (k == 0) {
      return f - s * (f - R);

    } else {
      return dk * ln2_hi - ( (s * (f - R) - dk * ln2_lo) - f);
    }
  }
 
}


/**
 Returns The IEEE Standard for Floating-Point Arithmetic (IEEE 754)
 base-10 logarithm of x.

 @param[in]         x                   Floating point value.
 
 @return base-10 logarithm of x, for values of x greater than zero.
*/
DOUBLE 
Ieee754Log10 (
  IN  DOUBLE x
  )
{
  DOUBLE y;
  DOUBLE z;
  INT32  i;
  INT32  k;
  INT32  hx;
  UINT32 lx;

  hx = GET_HI_DWORD (x);  // high word of x 
  lx = GET_LO_DWORD (x);  // low word of x 

  k = 0;
  if (hx < 0x00100000) {                    // x < 2**-1022  
    if ( ( (hx & 0x7fffffff) | lx) == 0) {
      GET_HI_DWORD(x) = 0xfff00000;
      return x;                             // log(+-0)=-inf 
    }

    if (hx < 0) {
      GET_HI_DWORD(x) = 0x7FF00000;
      GET_LO_DWORD(x) = 0x1;      
      return x;                             // log(-#) = NaN 
    }
    k -= 54;
    x *= two54;                             // subnormal number, scale up x 
    hx = GET_HI_DWORD (x);                  // high word of x 
  }

  if (hx >= 0x7ff00000) {
    return x + x;
  }

  k += (hx >> 20) - 1023;
  i  = ( (UINT32) k & 0x80000000) >> 31;
  hx = (hx & 0x000fffff) | ( (0x3ff - i) << 20);
  y  = (DOUBLE) (k + i);
  GET_HI_DWORD (x) = hx;
  z  = y * log10_2lo + ivln10 * Ieee754Log (x);
  return  z + y * log10_2hi;
}

