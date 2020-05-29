/** @file

  Header file for the Active Management Driver.

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

#ifndef _ACTIVE_MANAGEMENT_H_
#define _ACTIVE_MANAGEMENT_H_

#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/Amt/include/Amt.h>
#include <Library/Amt/Dxe/AmtLib.h>
///
/// Used during initialization
///
#include <Protocol/AlertStandardFormat.h>
///
/// Driver Produced Protocols
///
#include <Protocol/ActiveManagement.h>
#include <MeLib.h>

///
/// Private data structure definitions for the driver
///
#define ACTIVE_MANAGEMENT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('A', 'M', 'T', 'P')

typedef struct {
  UINTN                               Signature;
  EFI_HANDLE                          Handle;
  EFI_ACTIVE_MANAGEMENT_PROTOCOL      ActiveManagementProtocol;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;
} ACTIVE_MANAGEMENT_INSTANCE;

#define ACTIVE_MANAGEMENT_INSTANCE_FROM_ACTIVE_MANAGEMENT_PROTOCOL(a) \
  CR ( \
  a, \
  ACTIVE_MANAGEMENT_INSTANCE, \
  ActiveManagementProtocol, \
  ACTIVE_MANAGEMENT_PRIVATE_DATA_SIGNATURE \
  )

///
/// Function prototypes used by the AMT protocol.
///
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
;

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
  );
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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );


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
  );

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
  );


#endif
