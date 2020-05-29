/** @file

   The definition of PEI Event Handler HOB.

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

#ifndef _PEI_EVENT_LOG_HOB_H_
#define _PEI_EVENT_LOG_HOB_H_

typedef struct {
  UINT8   Type;
  UINT8   Length;

  UINT8   Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;

  UINT8   Data[8];
} PEI_EVENT_LOG_ORGANIZATION;

#define PEI_EVENT_LOG_BASE_LENGTH                0x08

extern EFI_GUID gH2OPeiEventLogHobGuid;

#endif
