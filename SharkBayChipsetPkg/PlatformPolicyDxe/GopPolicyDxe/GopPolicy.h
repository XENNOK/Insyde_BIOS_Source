//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   GopPolicy.h
//;

#ifndef _GOP_POLICY_DXE_H_
#define _GOP_POLICY_DXE_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
//[-start-121029-IB10820145-modify]//
#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/PcdLib.h>
//[-end-121029-IB10820145-modify]//
#include <Protocol/PlatformGopPolicy.h>
#include <Protocol/FirmwareVolume.h>
//[-start-130710-IB05160465-modify]//
#include <Protocol/SetupUtility.h>
//[-end-130710-IB05160465-modify]//
#include <ChipsetSetupConfig.h>
#include <VBTAccess.h>
//[-start-130110-IB11410040-add]//
//[-start-130809-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130809-IB06720232-remove]//
//[-end-130110-IB11410040-add]//
//[-start-130401-IB05400394-add]//
#include <PlatformGopVbt.h>
//[-end-130401-IB05400394-add]//

//
// VBT GUID
///
//[-start-121003-IB10820132-modify]//
extern EFI_GUID gVbtFileGuid;
//[-end-121003-IB10820132-modify]//

EFI_STATUS
GetPlatformLidStatus (
  OUT LID_STATUS             *CurrentLidStatus
);

EFI_STATUS
GetVbtData (
  OUT EFI_PHYSICAL_ADDRESS   *VbtAddress,
  OUT UINT32                 *VbtSize
);

EFI_STATUS
VBTPlatformHook (
  IN OUT  EFI_PHYSICAL_ADDRESS        *VBTAddress,
  IN OUT  UINT32                      *VBTSize
  );

#endif

