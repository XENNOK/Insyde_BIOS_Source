//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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
//;   GopPolicy.c
//;

#include <GopPolicy.h>
//[-start-130812-IB06720232-add]//
#include <Library/BaseOemSvcChipsetLib.h>
//[-end-130812-IB06720232-add]//
//[-start-121003-IB10820132-remove]//
//EFI_GUID        gVbtFileGuid = VBT_FILE_GUID;
//[-end-121003-IB10820132-remove]//
VOID            *VbtFilePtr = NULL;
UINTN           VbtFileSize = 0;

EFI_STATUS
EFIAPI
GopPolicyEntry (
  IN  EFI_HANDLE                         ImageHandle,
  IN  EFI_SYSTEM_TABLE                   *SystemTable
  )
/*++

  Routine Description:

    Initialize the GOP Policy.

  Arguments:

    ImageHandle - Pointer to the loaded image protocol for this driver.
    SystemTable - Pointer to the EFI System Table.

  Returns:

    EFI_SUCCESS          - The driver installes/initialized correctly.
    EFI_OUT_OF_RESOURCES - Unable to allocated memory.

--*/
{
  PLATFORM_GOP_POLICY_PROTOCOL       *PlatformGopPolicyPtr = NULL;
//[-start-121220-IB10820206-add]//
  EFI_STATUS                         Status;
//[-end-121220-IB10820206-add]//

  PlatformGopPolicyPtr = AllocateZeroPool (sizeof (PLATFORM_GOP_POLICY_PROTOCOL));
  if (PlatformGopPolicyPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PlatformGopPolicyPtr->Revision             = PLATFORM_GOP_POLICY_PROTOCOL_REVISION_01;
  PlatformGopPolicyPtr->GetPlatformLidStatus = GetPlatformLidStatus;
  PlatformGopPolicyPtr->GetVbtData           = GetVbtData;
//[-start-121220-IB10820206-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdatePlatformGopPolicy (PlatformGopPolicyPtr);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdatePlatformGopPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return Status;
  }
//[-end-121220-IB10820206-add]//

  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gPlatformGopPolicyProtocolGuid,
                EFI_NATIVE_INTERFACE,
                PlatformGopPolicyPtr
                );

}

//[-start-121120-IB08050186-modify]//
//[-start-121109-IB10820156-modify]//
/**
  Return LFP lid status is open or closed.

  @param[out] CurrentLidStatus      Current LFP Lid state. 0 = Lid closed. 1 = Lid open

  @retval EFI_SUCCESS               Always returns EFI_SUCCESS.
  @retval Others                    Get Lid state from EC failed.
**/
EFI_STATUS
GetPlatformLidStatus (
  OUT LID_STATUS             *CurrentLidStatus
)
{
//[-start-130809-IB06720232-modify]//
  EFI_STATUS    EcGetLidState;
  BOOLEAN       LidIsOpen;

  EcGetLidState = EFI_SUCCESS;
  LidIsOpen = TRUE;

  OemSvcEcGetLidState (&EcGetLidState, &LidIsOpen);

  ASSERT (!EFI_ERROR (EcGetLidState));

  *CurrentLidStatus = LidOpen;  
  if (!EFI_ERROR (EcGetLidState)) {
    if (!LidIsOpen) {
      //
      // If get lid state form EC successfully and lid is closed.
      //
      *CurrentLidStatus = LidClosed;
    }
  } else {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "OemSvcEcGetLidState ERROR in GopPolicy! Status is %r.\n", EcGetLidState));
  }

  return EcGetLidState;
//[-end-130809-IB06720232-modify]//
}
//[-end-121109-IB10820156-modify]//
//[-end-121120-IB08050186-modify]//

EFI_STATUS
GetVbtData (
  OUT EFI_PHYSICAL_ADDRESS   *VbtAddress,
  OUT UINT32                 *VbtSize
)
/*++

Routine Description:

  Get Intel video BIOS VBT information (i.e. Pointer to VBT and VBT size).
  BY READING THE VBT FILE FROM FFS FILE SYSTEM
  The VBT (Video BIOS Table) is a block of customizable data that is built
  within the video BIOS and edited by customers.

Arguments:

  VbtAddress ?VPhysical address of the VBT data
  VbtSize    - Size of the VBT data

Returns:

  EFI_SUCCESS     - Video BIOS VBT information returned.
  EFI_UNSUPPORTED - Could not find VBT information (*VbtAddress = NULL).

--*/
{
//[-start-140627-IB05080432-remove]//
//  static UINTN                  TableHandle = 0;
//[-end-140627-IB05080432-remove]//
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol = NULL;
  
  Status = EFI_SUCCESS;
  if ((VbtFilePtr == NULL) && (VbtFileSize == 0)) {
    //
    // Locate FV protocol.
    // There is little chance we can't find an FV protocol
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiFirmwareVolumeProtocolGuid,
                    NULL,
                    &NumberOfHandles,
                    &HandleBuffer
                    );
    ASSERT_EFI_ERROR (Status);
    DEBUG ((EFI_D_ERROR, "NumberOfHandles: 0x%X\n", NumberOfHandles));
    ASSERT((NumberOfHandles != 0));
    
    //
    // Looking for FV with VBT storage file
    //
    for (Index = 0; Index < NumberOfHandles; Index++) {
      //
      // Get the protocol on this handle
      // This should not fail because of LocateHandleBuffer
      //
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiFirmwareVolumeProtocolGuid,
                      (VOID **)&FwVol
                      );
      ASSERT_EFI_ERROR (Status);

      //
      // See if it has the VBT storage file
      //
      VbtFileSize = 0;
      FvStatus    = 0;
      VbtFilePtr  = NULL;   //ask ReadFile to allocate
      Status = FwVol->ReadFile (
                        FwVol,
                        &gVbtFileGuid,
                        &VbtFilePtr,
                        &VbtFileSize,
                        &FileType,
                        &Attributes,
                        &FvStatus
                        );

      //
      // If we found it, then we are done
      //
      if (!EFI_ERROR (Status)) {
        break;
      }
    }

    //
    // Free any allocated buffers
    //
    (gBS->FreePool) (HandleBuffer);
  }
  
  *VbtAddress = (EFI_PHYSICAL_ADDRESS) VbtFilePtr;
  *VbtSize = (UINT32) VbtFileSize;

  VBTPlatformHook (VbtAddress, VbtSize);

  return Status; 
}

//[-start-130401-IB05400394-add]//
EFI_STATUS
ReSortChildDeviceList (
  IN OUT  EFI_PHYSICAL_ADDRESS        *VBTAddress,
  IN      UINT8                       TargetIndex
  )
{
  UINT8        PrimaryDisplay;
  UINT8        SecondaryDisplay;
  UINT8        Index;

  PrimaryDisplay   = VBT8Read (*VBTAddress, VBT_CHILD_DEVICE_PRIMARY (TargetIndex));
  SecondaryDisplay = VBT8Read (*VBTAddress, VBT_CHILD_DEVICE_SECONDARY (TargetIndex));

  Index = TargetIndex;
  while ( Index > 0 ) {
    VBT8Write (*VBTAddress, VBT_CHILD_DEVICE_PRIMARY (Index), VBT8Read (*VBTAddress, VBT_CHILD_DEVICE_PRIMARY (Index - 1)));
    VBT8Write (*VBTAddress, VBT_CHILD_DEVICE_SECONDARY (Index), VBT8Read (*VBTAddress, VBT_CHILD_DEVICE_SECONDARY (Index - 1)));

    Index = Index - 1;
  }
  VBT8Write (*VBTAddress, VBT_CHILD_DEVICE_PRIMARY (Index), PrimaryDisplay);
  VBT8Write (*VBTAddress, VBT_CHILD_DEVICE_SECONDARY (Index), SecondaryDisplay);

  return EFI_SUCCESS;
}
//[-end-130401-IB05400394-add]//

EFI_STATUS
VBTPlatformHook (
  IN OUT  EFI_PHYSICAL_ADDRESS        *VBTAddress,
  IN OUT  UINT32                      *VBTSize
  )
{
//[-start-130710-IB05160465-modify]//
  EFI_STATUS                          Status;
  EFI_SETUP_UTILITY_PROTOCOL          *SetupUtility;
  CHIPSET_CONFIGURATION               *SetupVariable;
  EFI_GUID                            SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
//[-start-130401-IB05400394-add]//
  BOOLEAN                             Match;
  UINT8                               Index;
//[-end-130401-IB05400394-add]//

  Status = gBS->LocateProtocol ( &gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility );
  ASSERT_EFI_ERROR ( Status );
  SetupVariable = ( CHIPSET_CONFIGURATION * )( SetupUtility->SetupNvData );

  if (SetupVariable->LFPConfiguration == 0x03) {
//[-start-120223-IB05300285-remove]//
//    //
//    // Change to eDP VBT
//    //
//    VBT8AndThenOr ( *VBTAddress, 0x017A, 0x00, 0xC6 );
//    VBT8AndThenOr ( *VBTAddress, 0x017B, 0x00, 0x78 );
//    VBT8AndThenOr ( *VBTAddress, 0x0188, 0x00, 0x0A );
//    VBT8AndThenOr ( *VBTAddress, 0x0190, 0x00, 0x02 );
//    VBT8AndThenOr ( *VBTAddress, 0x0191, 0x00, 0x40 );
//
//[-end-120223-IB05300285-remove]//
    //
    // Update Active LFP Setting
    //
    VBT8AndThenOr (*VBTAddress, R_VBT_LFP_CONFIG, (~B_VBT_LFP_CONFIG), (V_VBT_LFP_CONFIG_eDP << N_VBT_LFP_CONFIG));
  }

  switch (SetupVariable->BacklightControl) {
    case 0x00:
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_PWM << N_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE));
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY_INVERTED << N_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY));
      break;

    case 0x01:
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_I2C << N_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE));
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY_INVERTED << N_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY));
      break;

    case 0x02:
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_PWM << N_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE));
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY_NORMAL << N_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY));
      break;

    case 0x03:
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_I2C << N_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE));
      VBT8AndThenOr (*VBTAddress, R_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY, (~B_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY ), ( V_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY_NORMAL << N_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY));
      break;

    default:
      break;
  }

//[-start-130401-IB05400394-add]//
  Match = FALSE;
  for (Index = 0; Index <= VBT_MAX_NUM_OF_CHILD_DEVICE; ++Index) {
    if (SetupVariable->IGDBootType == VBT8Read (*VBTAddress, VBT_CHILD_DEVICE_PRIMARY (Index))) {
      if (SetupVariable->IGDBootTypeSecondary == VBT8Read (*VBTAddress, VBT_CHILD_DEVICE_SECONDARY (Index))) {
        Match = TRUE;
        break;
      }
    }
  }


  if (Match) {
    //
    // To re-sort Child Device List
    //
    ReSortChildDeviceList (VBTAddress, Index);
  } else {
    SetupVariable->IGDBootType          = SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT;
    SetupVariable->IGDBootTypeSecondary = SCU_IGD_BOOT_TYPE_DISABLE;
    Status = gRT->SetVariable (
                    L"Setup",
                    &SystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//[-start-130709-IB05160465-modify]//
                    PcdGet32 (PcdSetupConfigSize),
//[-end-130709-IB05160465-modify]//
                    SetupVariable
                    );
  }
//[-end-130401-IB05400394-add]//
//[-end-130710-IB05160465-modify]//

  return Status;
}
