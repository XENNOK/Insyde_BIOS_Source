/** @file

  Definitions for HECI driver

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_HECI_DXE_H_
#define _SPS_HECI_DXE_H_

#include <IndustryStandard/Pci.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>

#include <Library/SpsMeChipsetLib.h>
#include <SpsMeAccess.h>


#define HECI_PRIVATE_DATA_SIGNATURE         SIGNATURE_32 ('H', 'e', 'c', 'i')
#define HECI_ROUND_UP_BUFFER_LENGTH(Length) ((UINT32) ((((Length) + 3) / 4) * 4))

///
/// Driver Produced Protocol Prototypes
///
#include <Protocol/SpsHeci.h>

///
/// HECI private data structure
///
typedef struct {
  UINTN                   Signature;
  EFI_HANDLE              Handle;
  UINT64                  HeciMBAR;
  UINT16                  DeviceInfo;
  UINT32                  RevisionInfo;
  SPS_HECI_PROTOCOL       HeciCtlr;
  volatile UINT32         *HpetTimer;
  UINT8                   MeFwImageType;
} HECI_INSTANCE;
#endif
