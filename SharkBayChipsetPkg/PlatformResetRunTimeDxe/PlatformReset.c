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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
/**

Copyright (c)  2011 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  PlatformReset.c

@brief:

  Provide the ResetSystem AP.

**/
//[-start-120712-IB10820084-modify]//
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Reset.h>
#include <Protocol/PchReset.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/MePlatformGetResetType.h>
//[-end-120712-IB10820084-modify]//
#include <MeLib.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/DebugLib.h>
//[-start-121109-IB10820159-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121109-IB10820159-add]/


PCH_RESET_PROTOCOL  *mPchReset;

/**

 Reset the system.

 @param[in]         ResetType           Warm or cold
 @param[in]         ResetStatus         Possible cause of reset
 @param[in]         DataSize            Size of ResetData in bytes
 @param[in]         ResetData           Optional Unicode string

 @retval Does not return if the reset takes place.

*/
VOID
EFIAPI
PlatformResetSystem (
  IN EFI_RESET_TYPE   ResetType,
  IN EFI_STATUS       ResetStatus,
  IN UINTN            DataSize,
  IN CHAR16           *ResetData OPTIONAL
  )
{
  EFI_STATUS                          Status;
  ME_PLATFORM_GET_RESET_TYPE_PROTOCOL *MePlatformGetResetType;
  PCH_RESET_TYPE                      OverridePchResetType;
  PCH_RESET_TYPE                      PchResetType;
  UINTN                               NumberMePlatformGetResetHandles;
  EFI_HANDLE                          *MePlatformGetResetHandles;
  UINTN                               Index;
  EFI_SMM_BASE2_PROTOCOL               *SmmBase2;
  BOOLEAN                             InSmm = FALSE;

//[-start-121109-IB10820159-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcHookPlatformReset (
             ResetType,
             ResetStatus
             );
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcHookPlatformReset, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
  if ( Status == EFI_SUCCESS ) {
    return;
  }
//[-end-121109-IB10820159-add]//
  PchResetType = ResetType;
  OverridePchResetType = ResetType;

  if (!EfiAtRuntime ()) {
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gMePlatformGetResetTypeGuid,
                    NULL,
                    &NumberMePlatformGetResetHandles,
                    &MePlatformGetResetHandles
                    );
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < NumberMePlatformGetResetHandles; Index++) {
        Status = gBS->HandleProtocol (
                        MePlatformGetResetHandles[Index],
                        &gMePlatformGetResetTypeGuid,
                        (VOID **) &MePlatformGetResetType
                        );
        if (!EFI_ERROR (Status)) {
          PchResetType = MePlatformGetResetType->GetResetType (ResetType);
          DEBUG ((EFI_D_INFO, "Returned Pch ResetType is: %x\n", PchResetType));
          if (PchResetType >= MaxRestReq) {
            DEBUG ((EFI_D_ERROR, "Platform Reset failed, invalid parameter\n"));
            ASSERT (FALSE);
          }
          if (OverridePchResetType < PchResetType) {
            DEBUG ((EFI_D_INFO, "Previous Pch ResetType is: %x\n", OverridePchResetType));
            OverridePchResetType = PchResetType;
          }
          DEBUG ((EFI_D_INFO, "Current Pch ResetType is: %x\n", OverridePchResetType));
        }
      }
    }
    PchResetType = OverridePchResetType;
    if ((PchResetType == GlobalReset) || (PchResetType == GlobalResetWithEc)) {
      ///
      /// Let ME do global reset if Me Fw is available
      ///
      Status = HeciSendCbmResetRequest (CBM_RR_REQ_ORIGIN_BIOS_POST, CBM_HRR_GLOBAL_RESET);
      if (!EFI_ERROR (Status)) {
        ///
        /// ME Global Reset should fail after EOP is sent.
        /// Go to use PCH Reset
        ///
        gBS->Stall (1000000);
      }
    }
  }

//[-start-120410-IB07360187-modify]//
  if (PchResetType != WarmReset) {
    mPchReset->Reset (mPchReset, PchResetType);
    ASSERT (FALSE);
  } else {
    Status = gBS->LocateProtocol (
                    &gEfiSmmBase2ProtocolGuid, 
                    NULL, 
                    (VOID **) &SmmBase2
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // Check to see if we are in SMM
    //
    SmmBase2->InSmm (SmmBase2, &InSmm);
    if (!InSmm) {
      mPchReset->Reset (mPchReset, WarmReset);
      ASSERT (FALSE);
    } else {
      //
      // When the system is going to do warm reset in SMM,
      // system will reset CPU by PCH hard reset instead of PCH soft reset.
      //
      // Hard reset will force all threads exit SMM to aviod BSP entering EFI_DEAD_LOOP in PchReset,
      // but all APs will be waiting in SmmDispatcher.
      //
    mPchReset->Reset (mPchReset, ColdReset);
    ASSERT (FALSE);
    }
  }
//[-end-120410-IB07360187-modify]//
}

/**

 Entry point of Platform Reset driver.

 @param[in]         ImageHandle         Standard entry point parameter
 @param[in]         SystemTable         Standard entry point parameter

 @retval            EFI_SUCCESS         Reset RT protocol installed
 @retval All other error conditions encountered result in an ASSERT

*/
EFI_STATUS
EFIAPI
InitializePlatformReset (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (&gPchResetProtocolGuid, NULL, (VOID **) &mPchReset);
  ASSERT_EFI_ERROR (Status);

  ///
  /// Make sure the Reset Architectural Protocol is not already installed in the system
  ///
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiResetArchProtocolGuid);

  ///
  /// Hook the runtime service table
  ///
  SystemTable->RuntimeServices->ResetSystem = PlatformResetSystem;

  ///
  /// Now install the Reset RT AP on a new handle
  ///
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiResetArchProtocolGuid,
                  NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**

 Entry point of Platform Reset driver.

 Fixup internal data pointers so that the services can be called in virtual mode.

 @param[in]         Event               The event registered.
 @param[in]         Context             Event context. Not used in this event handler.

*/
VOID
PchResetVirtualddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &mPchReset);
}
