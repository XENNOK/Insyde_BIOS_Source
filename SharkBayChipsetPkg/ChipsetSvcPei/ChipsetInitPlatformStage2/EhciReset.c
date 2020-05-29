/** @file
  To reset the EHCI.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesLib.h>
#include <PchAccess.h>
#include <Ppi/Stall.h>

//[-start-120920-IB06460442-add]//
EFI_STATUS
ResetEhciController (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_STALL_PPI                    *StallPpi,
  IN UINTN                                ControllerAddress
  );
//[-end-120920-IB06460442-add]//

//[-start-120920-IB06460442-add]//
/**
 Reset the EHCI controoler and clear temporary MMIO resource.

 @param [in]   PeiServices        General purpose services available to every PEIM.
 @param [in]   StallPpi           A pointer to stall PPI.
 @param [in]   ControllerAddress  The address of EHCI controller.

 @retval EFI_SUCCESS

**/
EFI_STATUS
ResetEhciController (
  IN CONST EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_STALL_PPI                    *StallPpi,
  IN UINTN                                ControllerAddress
  )
{
  UINT32            *BaseAddress;
  UINT32            Command;
  UINT16            Data;

  Data = 0xFFF9;

  BaseAddress = (UINT32*) (ControllerAddress + R_PCH_EHCI_MEM_BASE);
  BaseAddress = BaseAddress + (*(UINT32*)(UINTN)BaseAddress & 0xff);
  //
  // Halt HC first
  //
  Command = *(UINT32*)(UINTN)BaseAddress;
  Command &= ~0x01;
  *(UINT32*)(UINTN)BaseAddress = Command;
  //
  // Stall 1 millisecond
  //
  StallPpi->Stall (PeiServices, StallPpi, 1000);
  //
  // HCReset
  //
  Command = *(UINT32*)(UINTN)BaseAddress;
  Command |= 0x02;
  *(UINT32*)(UINTN)BaseAddress = Command;

  //
  // Disable EHCI controller
  //
  MmioAnd16 (ControllerAddress + R_PCH_EHCI_COMMAND_REGISTER, Data);
  //
  // Clear address range for EHCI controller
  //
  MmioAnd32 (ControllerAddress + R_PCH_EHCI_MEM_BASE, 0);

  return EFI_SUCCESS;
}
//[-end-120920-IB06460442-add]//
