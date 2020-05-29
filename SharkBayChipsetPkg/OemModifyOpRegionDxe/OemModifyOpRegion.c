/** @file
  This driver will initial and update PrePostHotkey Variable.

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

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
//[-start-121107-IB10820154-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121107-IB10820154-add]//
//[-start-130110-IB11410040-add]//
//[-start-130812-IB06720232-modify]//
//#include <Library/EcLib.h>
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-modify]//
//[-end-130110-IB11410040-add]//

//[-start-121107-IB10820154-modify]//
STATIC
VOID
EFIAPI
OemSetIgdOpRegion (
  IN EFI_EVENT                Event,
  IN VOID                     *Context
  )
{
//[-start-130809-IB06720232-modify]//
  EFI_STATUS                    Status;
  IGD_OPREGION_PROTOCOL         *IgdOpRegion;
//[-start-121120-IB08050186-add]//
//  BOOLEAN                       LidState;
//[-end-121120-IB08050186-add]//
  EFI_STATUS                    EcGetLidState;
  BOOLEAN                       LidIsOpen;

  EcGetLidState = EFI_SUCCESS;
//[-start-121120-IB08050186-add]//
  LidIsOpen = TRUE;
//[-end-121120-IB08050186-add]//

  //
  //  Locate the Global NVS Protocol.
  //
  Status = gBS->LocateProtocol (
                  &gIgdOpRegionProtocolGuid,
                  NULL,
                  (VOID **)&IgdOpRegion
                  );
  ASSERT (Status == EFI_SUCCESS);

//[-start-121120-IB08050186-modify]//
  OemSvcEcGetLidState (&EcGetLidState, &LidIsOpen);

  ASSERT (!EFI_ERROR (EcGetLidState));

  //
  // Follow EC.asl to set CLID = 3 (both Integrated LFP and External LFP lid are open) when Lid is open.
  // MBox1.CLID is descripted in "IGD OpRegion/Software SCI for Sandy Bridge/ Ivy Bridge BIOS Specification".
  //
  IgdOpRegion->OpRegion->MBox1.CLID = PcdGet32 ( PcdLidStatus );
  if (!EFI_ERROR (EcGetLidState)) {
    if (!LidIsOpen) {
      //
      // If get lid state form EC successfully and lid is closed.
      //
      IgdOpRegion->OpRegion->MBox1.CLID = 0;
    }
  } else {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "OemSvcEcGetLidState ERROR in OemModifyOpRegion! Status is %r.\n", EcGetLidState));
  }
//[-end-121120-IB08050186-modify]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcSetIgdOpRegion( IgdOpRegion );
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcSetIgdOpRegion, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
//[-end-130809-IB06720232-modify]//
}
//[-end-121107-IB10820154-modify]//
/**

  Modify Igd OpRegion

  @param     ImageHandle     EFI_HANDLE
  @param     SystemTable     EFI_SYSTEM_TABLE pointer

  @retval     EFI_SUCCESS     
 
**/
EFI_STATUS
EFIAPI
OemModifyOpRegionInit (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_EVENT                     IddOpRegionEvent;
  VOID                          *NotifyReg;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  OemSetIgdOpRegion,
                  NULL,
                  &IddOpRegionEvent
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;

  }

  Status = gBS->RegisterProtocolNotify (
                  &gIgdOpRegionProtocolGuid,
                  IddOpRegionEvent,
                  &NotifyReg
                  );
  return EFI_SUCCESS;
}


