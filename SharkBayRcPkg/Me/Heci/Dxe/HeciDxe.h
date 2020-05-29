/** @file

  Definitions for HECI driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _HECI_DRV_H
#define _HECI_DRV_H

#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>

#include <MeAccess.h>
#include <HeciRegs.h>
#include <RcFviDxeLib.h>


#define HECI_PRIVATE_DATA_SIGNATURE         SIGNATURE_32 ('H', 'e', 'c', 'i')
#define HECI_ROUND_UP_BUFFER_LENGTH(Length) ((UINT32) ((((Length) + 3) / 4) * 4))

///
/// Driver Produced Protocol Prototypes
///
#include <Protocol/Heci.h>
#include <Protocol/MeRcInfo.h>
///
/// Driver Consumed Protocol Prototypes
///
#include <Protocol/MebxProtocol.h>


extern FVI_ELEMENT_AND_FUNCTION       mMeFviElementsData[];
extern FVI_DATA_HUB_CALLBACK_CONTEXT  mMeFviVersionData;
extern UINTN                          mMeFviElements;

typedef union {
  UINT32  Data32;
  UINT16  Data16[2];
  UINT8   Data8[4];
} DATA32_UNION;

///
/// HECI private data structure
///
typedef struct {
  UINTN                   Signature;
  EFI_HANDLE              Handle;
  UINT64                  HeciMBAR;
  UINT16                  DeviceInfo;
  UINT32                  RevisionInfo;
  EFI_HECI_PROTOCOL       HeciCtlr;
  volatile UINT32         *HpetTimer;
  EFI_ME_RC_INFO_PROTOCOL MeRcInfo;
  UINT8                   MeFwImageType;
} HECI_INSTANCE;
#endif
