/** @file

  Defines and prototypes for the ActiveManagement driver.
  This driver implements the ActiveManagement protocol for iAMT.
  It provides some functions to get Boot Options from ASF.

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

#include "ActiveManagement.h"

///
/// Global variables
///
EFI_ASF_BOOT_OPTIONS        *mAsfBootOptions;

ACTIVE_MANAGEMENT_INSTANCE  ActiveManagementInstance = {
  ACTIVE_MANAGEMENT_PRIVATE_DATA_SIGNATURE,
  NULL,
  {
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetIderState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetEnforceSecureBootState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetSolState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetRemoteFlashState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetBiosSetupState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetBiosPauseState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetConsoleLockState,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetKvmState,
    (EFI_ACTIVE_MANAGEMENT_IDER_BOOT_DEVICE_SELECTED) GetIderBootDeviceSelectd,
    (EFI_ACTIVE_MANAGEMENT_ASF_BOOT_OPTIONS_GET) GetAsfBootOptions,
    (EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) GetProgressMsgRequest
  },
  NULL
};

///
/// Function implementations
///
/**
  Check if ASF boot options is present.

  @param[in] None.

  @retval True                    ASF boot option is present.
  @retval False                   ASF boot option is not present
**/
BOOLEAN
IsBootOptionsPresent (
  VOID
  )
{
  return mAsfBootOptions->SubCommand == ASF_BOOT_OPTIONS_PRESENT;
}

/**
  Check if LANA ID of ASF boot options is Industry ID.

  @param[in] None.

  @retval True                    IANA ID of ASF boot options is Industry ID.
  @retval False                   IANA ID of ASF boot options is not Industry ID.
**/
BOOLEAN
IsIndustryIanaId (
  VOID
  )
{
  volatile BOOLEAN  RetVal;

  RetVal = FALSE;
  if (IsBootOptionsPresent ()) {
    if (mAsfBootOptions->IanaId == ASF_INDUSTRY_CONVERTED_IANA) {
      RetVal = TRUE;
    }
  }

  return RetVal;
}

/**
  Check if LANA ID of ASF boot options is Intel ID.

  @param[in] None.

  @retval True                    IANA ID of ASF boot options is Intel ID.
  @retval False                   IANA ID of ASF boot options is not Intel ID.
**/
BOOLEAN
IsIntelIanaId (
  VOID
  )
{
  volatile BOOLEAN  RetVal;

  RetVal = FALSE;

  if (IsBootOptionsPresent ()) {
    if (mAsfBootOptions->IanaId == ASF_INTEL_CONVERTED_IANA) {
      RetVal = TRUE;
    }
  }

  return RetVal;
}

/**
  Check if it is Intel ASF boot options.

  @param[in] None.

  @retval True                    It is Intel ASF boot options.
  @retval False                   It is not Intel ASF boot options.
**/
BOOLEAN
IsIntelAmtBootOptions (
  VOID
  )
{
  BOOLEAN RetVal;

  RetVal = FALSE;

  if (IsIntelIanaId ()) {
    if (mAsfBootOptions->SpecialCommand == ASF_INTEL_OEM_CMD) {
      RetVal = TRUE;
    }
  }

  return RetVal;
}

/**
  Check the Special Command Parameter of Intel ASF boot options

  @param[in] Options              Special Command Parameter bit we want to check
                                  Bit 0: Set if IDER is to be used on the next boot. Parameter 2 is set
                                  to the driver number to be used.
                                  Bit 1: Set if Secure Boot is enforced over IDER
                                  Bit 2: Set if the BIOS is to be re-flashed on the next boot
                                  Bit 3: Set if the BIOS is to boot into the BIOS set-up screen.
                                  Bit 4: Boot into BIOS Pause on the next boot is supported
                                  Bit 5: Set if the BIOS is to participate in KVM session
  @param[in] CurrentState         Return the state of result
                                  True - Special Command Parameter bit in Options is enabled.
                                  False - Special Command Parameter bit in Options is disabled.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetSpecialParamState (
  IN     UINT16   Options,
  IN OUT BOOLEAN  *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsIntelAmtBootOptions ()) {
    if ((mAsfBootOptions->SpecialCommandParam & Options) == Options) {
      *CurrentState = TRUE;
    }
  }

  return EFI_SUCCESS;
}

/**
  Check the OEM Parameter of Intel ASF boot options

  @param[in] Options              OEM Parameter bit we want to check
                                  Bit 0: Set if SOL is to be used on the next boot.
  @param[in] CurrentState         Return the state of result
                                  True : OEM Parameter bit in Options is enabled.
                                  False : OEM Parameter bit in Options is disabled.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetOemParamatersState (
  IN     UINT16   Options,
  IN OUT BOOLEAN  *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsIntelAmtBootOptions ()) {
    if ((mAsfBootOptions->OemParameters & Options) == Options) {
      *CurrentState = TRUE;
    }
  }

  return EFI_SUCCESS;
}

/**
  Check the OEM Parameter of Intel ASF boot options

  @param[in] Options              OEM Parameter bit we want to check
                                  Bit 0: Set if SOL is to be used on the next boot.
  @param[in] CurrentState         Return the state of result
                                  True : OEM Parameter bit in Options is enabled.
                                  False : OEM Parameter bit in Options is disabled.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetBootOptionsMaskState (
  IN     UINT16   Options,
  IN OUT BOOLEAN  *CurrentState
  )
{
  *CurrentState = FALSE;
  if (IsBootOptionsPresent ()) {
    if ((mAsfBootOptions->BootOptions & Options) == Options) {
      *CurrentState = TRUE;
    }
  }

  return EFI_SUCCESS;
}

/**
  This will return IDE Redirection Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of IDE Redireciton Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetIderState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (USE_IDER, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return IDE Redirection boot device to boot

  @param[in] This                 The address of protocol
  @param[in] IdeBootDevice        Return the boot device number to boot
                                  Bits 0-1: If IDER boot is selected in Perimeter 1 then Bits 1,2 define the drive on the IDER controller to be used as the boot driver.
                                  Bit 1  Bit0
                                     0    0    Primary Master Drive
                                     0    1    Primary Slave Drive
                                     1    0    Secondary Master Drive
                                     1    1    Secondary Slave Drive
                                  Bits 2-7: Reserved set to 0

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetIderBootDeviceSelectd (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT UINT8                            *IdeBootDevice
  )
{
  *IdeBootDevice = (UINT8) ((mAsfBootOptions->SpecialCommandParam & IDER_BOOT_DEVICE_MASK) >> IDER_BOOT_DEVICE_SHIFT);
  return EFI_SUCCESS;
}

/**
  This will return Enforce Secure Boot over IDER Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of Enforce Secure Boot over IDER Boot Option

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetEnforceSecureBootState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (ENFORCE_SECURE_BOOT, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Serial-over-Lan Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of Serial-over-Lan Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetSolState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetOemParamatersState (USE_SOL, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Remote Flash Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of Remote Flash Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetRemoteFlashState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (REFLASH_BIOS, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return BIOS Setup Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of BIOS Setup Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetBiosSetupState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (BIOS_SETUP, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return BIOS Pause Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of BIOS Pause Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetBiosPauseState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (BIOS_PAUSE, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return Console Lock Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of BIOS Pause Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetConsoleLockState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetBootOptionsMaskState (LOCK_KEYBOARD, CurrentState);
  return EFI_SUCCESS;
}

/**
  This will return KVM Boot Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of KVM Boot Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetKvmState (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetSpecialParamState (USE_KVM, CurrentState);
  return EFI_SUCCESS;
}

/**
  Return current ASF Boot Options

  @param[in] This                 Pointer to the EFI_ACTIVE_MANAGEMENT_PROTOCOL instance.
  @param[in] AsfBootOptions       ASF Boot Options

  @retval EFI_SUCCESS             Boot options updated
**/
EFI_STATUS
EFIAPI
GetAsfBootOptions (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT EFI_ASF_BOOT_OPTIONS             **AsfBootOptions
  )
{
  *AsfBootOptions = mAsfBootOptions;
  return EFI_SUCCESS;
}

/**
  Disable these two driver of Sol & Ider

  @param[in] None
**/
VOID
SolIderDisable (
  VOID
  )
{
  IderDisable ();
  SolDisable ();
}

/**
  Disable Usbr

  @param[in] None
**/
VOID
UsbrDisable (
  VOID
  )
{
  Usbr1Disable ();
  Usbr2Disable ();
}

/**
  This will return progress event Option.
  True if the option is enabled.

  @param[in] This                 The address of protocol
  @param[in] CurrentState         Return the state of progress event Opiton

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetProgressMsgRequest (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  )
{
  GetBootOptionsMaskState (FORCE_PROGRESS_EVENTS, CurrentState);
  return EFI_SUCCESS;
}

/**
  Entry point for the Active Management Driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
ActiveManagementEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS  Status;

  ///
  /// Check policy if AMT is supported
  ///
  if (!AmtSupported () || !AsfSupported ()) {
    SolIderDisable ();
    UsbrDisable ();
    return EFI_UNSUPPORTED;
  }
  ///
  /// Get Protocol for ASF
  ///
  Status = gBS->LocateProtocol (
                  &gEfiAlertStandardFormatProtocolGuid,
                  NULL,
                  (VOID **) &(ActiveManagementInstance.Asf)
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Get ASF Boot Options
  ///
  Status = ActiveManagementInstance.Asf->GetBootOptions (ActiveManagementInstance.Asf, &mAsfBootOptions);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Install the EFI_ACTIVE_MANAGEMENT_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(ActiveManagementInstance.Handle),
                  &gEfiActiveManagementProtocolGuid,
                  &(ActiveManagementInstance.ActiveManagementProtocol),
                  NULL
                  );

  return Status;
}
