/** @file

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

#include <IoTrapFunction.h>
#include <ChipsetSetupConfig.h>
#include <PchAccess.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>

#include <Protocol/SmmIoTrapDispatch.h>
#include <Protocol/SetupUtility.h>
#include <Protocol/GlobalNvsArea.h>

EFI_GUID mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

/**
 Enable ICH6M IO Trap setting, use IO Port 808(IO_TRAP_BASE_ADDRESS).
 Follow ICH6M BIOS Porting Guide Rev0.7

 @retval None

**/
EFI_STATUS
EnableIoTrap ()
{
  //Step 1.Set RCBA, enable RCBA - Already done
  //Step 2.Program "Address[15:2]" in ICH_RCRB_IO_TRAP_1[15:2] - IO_TRAP_BASE_ADDRESS
  //Step 3.Program "Address[7:2] Mask" in ICH_RCRB_IO_TRAP_1[23:18] - IO_TRAP_ADDRESS_MASK
  //Step 4.Program "Byte Enable" in ICH_RCRB_IO_TRAP_1_HIGH[3:0] - IO_TRAP_BYTE_ENABLE
  //Step 5.Program "Byte Enable Mask" in ICH_RCRB_IO_TRAP_1_HIGH[7:4] - IO_TRAP_BYTE_ENABLE_MASK
  //Step 6.Program "Read/Write Mask" in ICH_RCRB_IO_TRAP_1_HIGH[17:16] - Write only:00b
  //Step 7.Program "Trap and SMI# Enable" bit in ICH_RCRB_IO_TRAP_1[0] - bit0

  PchMmRcrb32AndThenOr ( R_PCH_RCRB_IO_TRAP_0 + 4, 0, 0x000200F0 );
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_IO_TRAP_0, 0, 0x00040801 );

  return EFI_SUCCESS;
}

/**
 Store the OS type for on screen branding use on the next boot.

 @param [in]   OsType           The ASL operating system value

 @retval None

**/
void
SaveOsType (
  IN UINT16  OsType
 )
{
//[-start-130710-IB05160465-modify]//
  VOID                        *SystemConfiguration;
  UINT32                      Attributes;
  UINTN                       DataSize;
  EFI_STATUS                  Status;

  Attributes  = EFI_VARIABLE_NON_VOLATILE;

  DataSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (DataSize);
//[-start-140625-IB05080432-add]//
  if (SystemConfiguration == NULL) {
    return;
  }
//[-end-140625-IB05080432-add]//

  //
  // Locate the setup configuration table
  //
  // BUGBUG: This shouldn't work.  It is illegal to RT services from SMM during RT.
  //
  Status = gST->RuntimeServices->GetVariable (
                  L"Setup",
                  &mSystemConfigurationGuid,
                  &Attributes,
                  &DataSize,
                  SystemConfiguration
                  );
  ASSERT_EFI_ERROR (Status);
//[-start-140625-IB05080432-add]//
  if (EFI_ERROR(Status)) {
    FreePool (SystemConfiguration);
    return;
  }
//[-end-140625-IB05080432-add]//

  //
  // Update the OS Type
  //
  ((CHIPSET_CONFIGURATION *)SystemConfiguration)->OperatingSystem = OsType;

  //
  // Save the OS information back to the System Configuration
  //
  Status = gST->RuntimeServices->SetVariable (
                  L"Setup",
                  &mSystemConfigurationGuid,
                  Attributes,
                  DataSize,
                  SystemConfiguration
                  );
  ASSERT_EFI_ERROR (Status);

  FreePool (SystemConfiguration);
//[-end-130710-IB05160465-modify]//
}

/**
 SMI callbacks for functions that requires calling Int 10 such as
 get display device, display switch to ACPI mode, display switch to Legacy mode, and
 setting brightness level.

 @param [in]   DispatchHandle   EFI Handle
 @param [in]   DispatchContext  Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

 @retval None

**/
void
IchnIoTrap0SmiFunctionCallback (
  IN  EFI_HANDLE                              DispatchHandle,
  IN  EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT  *DispatchContext
  )
{
  //
  // IoTrap Function
  //
  
switch (mGlobalNvsArea->SmiFunction){
    default :
      //
      // Clear SMIF
      //
      mGlobalNvsArea->SmiFunction = 0;
      break;
  }

  return;
}
