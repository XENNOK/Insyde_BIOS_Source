/** @file

Hydra Function Library

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/

#include "HydraLibInternal.h"
#include <Library/HydraLib.h>

/**

  pow function is calculation hexadecimal to the power of MakeUpDataNum
  @param[in]  MakeUpDataNum   Make Up data numbers
  
  @return PowResult

*/
UINT32
Pow(
  IN UINT32    MakeUpDataNum
  )
{
  UINT32 PowResult;
  
  PowResult = 1;

  while (MakeUpDataNum != 0){
    PowResult *= 16;
    --MakeUpDataNum;
  }
  return PowResult;
}

/**
 Hydra Test Func
*/
VOID
HydrakeyRead(
  VOID
  )
{
	 Print(L"This is HydraLib\n");
}
