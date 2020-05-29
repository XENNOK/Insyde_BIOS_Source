/** @file

  Definitions for HECI SMM driver

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

#ifndef _SPS_HECI_SMM_H_
#define _SPS_HECI_SMM_H_

#include <IndustryStandard/Pci.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/IoLib.h>
#include <SpsMeAccess.h>

///
/// Driver Produced Protocol Prototypes
///
#include <Protocol/SmmRuntime.h>
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
} HECI_INSTANCE_SMM;
#endif
