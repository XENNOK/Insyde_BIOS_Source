/** @file

   The definition of PEI Post Message HOB.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_POST_MESSAGE_HOB_H_
#define _PEI_POST_MESSAGE_HOB_H_

typedef struct {
  //
  // EFI Status Code Type and Code Value
  //
  UINT32    CodeType;
  UINT32    CodeValue; 
  UINT8     DataLength;
  UINT8     Data[5];
} PEI_POST_MESSAGE_DATA_HOB;

extern EFI_GUID gH2OPeiPostMessageHobGuid;

#endif
