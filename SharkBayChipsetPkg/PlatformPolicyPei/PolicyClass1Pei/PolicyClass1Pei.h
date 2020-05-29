/** @file
  
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_POLICY_CLASS_1_H_
#define _PEI_POLICY_CLASS_1_H_
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
//[-start-130110-IB11410040-add]//
#include <Ppi/MasterBootMode.h>
#include <Ppi/SmbusPolicy.h>
//[-end-130110-IB11410040-add]//

//[-start-121120-IB03780473-add]//
EFI_STATUS
MePolicyPeiEntry (
  IN      EFI_FFS_FILE_HEADER         *FfsHeader,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );

//[-start-130125-IB11410046-modify]//
EFI_STATUS
AmtPolicyPeiEntry (
  IN      EFI_FFS_FILE_HEADER         *FfsHeader,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );
//[-end-130125-IB11410046-modify]//

EFI_STATUS
AzaliaPolicyPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );

EFI_STATUS
PchPolicyPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );

EFI_STATUS
CpuPolicyPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );

EFI_STATUS
StallPeiEntry (
  IN      EFI_PEI_FILE_HANDLE         FileHandle,
  IN      CONST EFI_PEI_SERVICES      **PeiServices
  );

EFI_STATUS
RecoveryPchUsbPolicyCallBack (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  );

EFI_STATUS
SaPolicyCallBack (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  );
//[-end-121120-IB03780473-add]//

//[-start-121126-IB03780468-add]//
EFI_STATUS
CpuPolicyCallBack (
  IN  EFI_PEI_SERVICES          **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN  VOID                            *Ppi
  );
//[-end-121126-IB03780468-add]//

#endif
