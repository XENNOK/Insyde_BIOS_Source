/** @file
  Header file for chipset Serial ATA controller driver.

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SataController.h

@brief
  Header file for chipset Serial ATA controller driver.

**/
#ifndef _SERIAL_ATA_CONTROLLER_H_
#define _SERIAL_ATA_CONTROLLER_H_

#include <IndustryStandard/Pci22.h>

#include <Protocol/IdeControllerInit.h>
#include <Protocol/PciIo.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/ComponentName.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/LegacyBios.h>
#include <PchAccess.h>
#include <PchSata.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/PchPlatformLib.h>
//
// Global Variables definitions
//
extern EFI_DRIVER_BINDING_PROTOCOL  mSataControllerDriverBinding;
extern EFI_COMPONENT_NAME_PROTOCOL  mSataControllerName;
extern UINTN                        mPchGpioBase;

///
/// {BB929DA9-68F7-4035-B22C-A3BB3F23DA55}
///
///
/// Define the  protocol GUID
///
#define PCH_SATA_CONTROLLER_DRIVER_GUID \
  { \
    0xbb929da9, 0x68f7, 0x4035, \
    { \
      0xb2, 0x2c, 0xa3, 0xbb, 0x3f, 0x23, 0xda, 0x55 \
    } \
  };

#define PCH_SATA_ENUMER_ALL   FALSE

#define PCH_SATA_MASTER_DRIVE 0x00
#define PCH_SATA_SLAVE_DRIVE  0x01

///
/// SATA controller driver private data structure
///
#define SATA_CONTROLLER_SIGNATURE SIGNATURE_32 ('S', 'A', 'T', 'A')

typedef struct _EFI_SATA_CONTROLLER_PRIVATE_DATA {
  ///
  /// Standard signature used to identify SATA controller private data
  ///
  UINT32                            Signature;

  ///
  /// Protocol instance of IDE_CONTROLLER_INIT produced by this driver
  ///
  EFI_IDE_CONTROLLER_INIT_PROTOCOL  IdeInit;

  ///
  /// copy of protocol pointers used by this driver
  ///
  EFI_PCI_IO_PROTOCOL               *PciIo;

  ///
  /// The highest disqulified mode for each attached SATA device.
  /// Per ATA/ATAPI spec, if a mode is not supported, the modes higher than
  /// it should not be supported
  ///
  EFI_ATA_COLLECTIVE_MODE           DisqulifiedModes[LPTH_AHCI_MAX_PORTS][PCH_IDE_1_MAX_DEVICES];

  ///
  /// A copy of IDENTIFY data for each attached SATA device and its flag
  ///
  EFI_IDENTIFY_DATA                 IdentifyData[LPTH_AHCI_MAX_PORTS][PCH_IDE_1_MAX_DEVICES];
  BOOLEAN                           IdentifyValid[LPTH_AHCI_MAX_PORTS][PCH_IDE_1_MAX_DEVICES];
//[-start-120402-IB06150206-add]//
  EFI_GUID                          *InstallGuid;
  VOID                              *InstallProtocol;
//[-end-120402-IB06150206-add]//
} EFI_SATA_CONTROLLER_PRIVATE_DATA;

#define SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS(a) \
  CR ( \
  a, \
  EFI_SATA_CONTROLLER_PRIVATE_DATA, \
  IdeInit, \
  SATA_CONTROLLER_SIGNATURE \
  )

//
// Driver binding functions declaration
//

/**
  This function checks to see if the driver supports a device specified by
  "Controller handle" parameter. It is called by DXE Core StartImage() or
  ConnectController() routines. The driver uses 'device path' and/or
  'services' from the Bus I/O abstraction attached to the controller handle
  to determine if the driver support this controller handle.
  Note: In the BDS (Boot Device Selection) phase, the DXE core enumerate all
  devices (or, controller) and assigns GUIDs to them.

  @param[in] This                 A pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested.
  @param[in] RemainingDevicePath  A pointer to the device path. Ignored by device
                                  driver but used by bus driver

  @retval EFI_SUCCESS             The device is supported
  @exception EFI_UNSUPPORTED      The device is not supported
**/
EFI_STATUS
EFIAPI
SataControllerSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN EFI_HANDLE                        Controller,
  IN EFI_DEVICE_PATH_PROTOCOL          *RemainingDevicePath
  );

/**
  This routine is called right after the .Supported() is called and returns
  EFI_SUCCESS. Notes: The supported protocols are checked but the Protocols
  are closed.

  @param[in] This                 A pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested. Parameter
                                  passed by the caller
  @param[in] RemainingDevicePath  A pointer to the device path. Should be ignored by
                                  device driver

  @retval EFI_SUCCESS             The device is started
  @retval Other values            Something error happened
**/
EFI_STATUS
EFIAPI
SataControllerStart (
  IN EFI_DRIVER_BINDING_PROTOCOL        *This,
  IN EFI_HANDLE                         Controller,
  IN EFI_DEVICE_PATH_PROTOCOL           *RemainingDevicePath
  );

/**
  Stop managing the target device

  @param[in] This                 A pointer pointing to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be stopped
  @param[in] NumberOfChildren     Number of child devices
  @param[in] ChildHandleBuffer    Buffer holding child device handles

  @retval EFI_SUCCESS             The target device is stopped
**/
EFI_STATUS
EFIAPI
SataControllerStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL       *This,
  IN  EFI_HANDLE                        Controller,
  IN  UINTN                             NumberOfChildren,
  IN  EFI_HANDLE                        *ChildHandleBuffer
  );

//
// IDE controller init functions declaration
//

/**
  This function can be used to obtain information about a specified channel.
  It's usually used by IDE Bus driver during enumeration process.

  @param[in] This                 the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              Channel number (0 based, either 0 or 1)
  @param[out] Enabled             TRUE if the channel is enabled. If the channel is disabled,
                                  then it will no be enumerated.
  @param[out] MaxDevices          The Max number of IDE devices that the bus driver can expect
                                  on this channel. For ATA/ATAPI, this number is either 1 or 2.

  @retval EFI_SUCCESS             Function completes successfully
  @retval Other Values            Something error happened
  @retval EFI_INVALID_PARAMETER   The Channel parameter is invalid
**/
EFI_STATUS
EFIAPI
IdeInitGetChannelInfo (
  IN   EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN   UINT8                               Channel,
  OUT  BOOLEAN                             *Enabled,
  OUT  UINT8                               *MaxDevices
  );

/**
  This function is called by IdeBus driver before executing certain actions.
  This allows IDE Controller Init to prepare for each action.

  @param[in] This                 the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Phase                phase indicator defined by IDE_CONTROLLER_INIT protocol
  @param[out] Channel             Channel number (0 based, either 0 or 1)

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel parameter is out of range
  @exception EFI_UNSUPPORTED      Phase is not supported
**/
EFI_STATUS
EFIAPI
IdeInitNotifyPhase (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  EFI_IDE_CONTROLLER_ENUM_PHASE     Phase,
  OUT UINT8                             Channel
  );

/**
  This function is called by IdeBus driver to submit EFI_IDENTIFY_DATA data structure
  obtained from IDE deivce. This structure is used to set IDE timing

  @param[in] This                 the EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] IdentifyData         A pointer to EFI_IDENTIFY_DATA data structure

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel or Device parameter is out of range
**/
EFI_STATUS
EFIAPI
IdeInitSubmitData (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_IDENTIFY_DATA                 *IdentifyData
  );

/**
  This function is called by IdeBus driver to disqualify unsupported operation
  mode on specfic IDE device

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] BadModes             Operation mode indicator

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel parameter or Devicde parameter is out of range,
                                  or BadModes is NULL
**/
EFI_STATUS
EFIAPI
IdeInitDisqualifyMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *BadModes
  );

/**
  This function is called by IdeBus driver to calculate the best operation mode
  supported by specific IDE device

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in, out] SupportedModes  Modes collection supported by IDE device

  @retval EFI_SUCCESS             Function completes successfully
  @retval EFI_INVALID_PARAMETER   Channel parameter or Device parameter is out of range;
                                  Or SupportedModes is NULL
  @retval EFI_NOT_READY           Identify data is not valid
  @retval EFI_OUT_OF_RESOURCES    SupportedModes is out of range
**/
EFI_STATUS
EFIAPI
IdeInitCalculateMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE        **SupportedModes
  );

/**
  This function is called by IdeBus driver to set appropriate timing on IDE
  controller according supported operation mode

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] Modes                Operation modes

  @retval EFI_SUCCESS             This function always returns EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
IdeInitSetTiming (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *Modes
  );

#endif
