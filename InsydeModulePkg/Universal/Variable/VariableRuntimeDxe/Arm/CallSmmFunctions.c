/** @file
  Dummy implementation for SMM related functions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include <Uefi.h>

INT8
IhisiVatsCall (
  IN     UINT8            *InPutBuff,
  IN     UINT8            *OutPutBuff,
  IN     UINT8            SubFunNum,
  IN     UINT16           SmiPort
  )
{
  return 0;
}

UINT8
SmmSecureBootCall (
  IN     UINT8            *InPutBuff,
  IN     UINTN            DataSize,
  IN     UINT8            SubFunNum,
  IN     UINT16           SmiPort
  )
{
  return 0;
}