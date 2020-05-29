/** @file

  Header file for chipset IDER ATA controller driver.

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

#ifndef _IDER_ATA_CONTROLLER_H
#define _IDER_ATA_CONTROLLER_H

#include <MeAccess.h>
#include <IndustryStandard/Pci.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>

#include <Library/Amt/Dxe/AmtLib.h>

///
/// Constant definition
///
#include <Protocol/PciIo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/IderControllerDriver.h>
#include <Protocol/ComponentName.h>

///
/// Global Variables definitions
///
extern EFI_COMPONENT_NAME_PROTOCOL  mIdeRControllerName;
extern EFI_DRIVER_BINDING_PROTOCOL  mIdeRControllerDriverBinding;
extern EFI_COMPONENT_NAME2_PROTOCOL mIdeRControllerName2;

///
/// Symbol definition, for PCI IDE configuration field
///
#define PCI_SUB_CLASS_IDE 0x01

#define IDER_MAX_CHANNEL  0x01  ///< Max channels number of single sata controller
#define IDER_MAX_DEVICES  0x02  ///< Max devices number of single sata channel
#define IDER_ENUMER_ALL   TRUE

///
/// PIO and DMA Mode Timing and Control Registers definition
///
#define PCI_COMMAND_REGISTER                0x04
#define PCI_BUS_MASTER_IDE_BASE             0x20
#define IDE_TIMING_REGISTER_1               0x40
#define IDE_TIMING_REGISTER_2               0x42
#define IDE_SLAVE_TIMING_REGISTER           0x44
#define ULTRA_DMA_CONTROL_REGISTER          0x48
#define ULTRA_DMA_TIMING_REGISTER           0x4A
#define IDE_IO_CONFIG_REGISTER              0x54

#define R_PCI_SVID                          0x2C

///
/// IDE Register Field definition
///
#define ENABLE_DECODE_PRIMARY     0x8000

#define BME_BUS_MASTER_ENABLE_BIT BIT2

///
/// Ide Timing Register 1/2 Field offset 0x40-41 / 0x42-43
///
#define DTE0_DRIVE_0_DMA_TIMING_ENABLE  bit3
#define DTE1_DRIVE_1_DMA_TIMING_ENABLE  bit7

///
/// UDMA Control Register field offset 0x48
///
#define PSDE0_PRIMARY_DRIVE_0_UDMA_ENABLE   bit0
#define PSDE1_PRIMARY_DRIVE_1_UDMA_ENABLE   bit1
#define SSDE0_SECONDARY_DRIVE_0_UDMA_ENABLE bit2
#define SSDE1_SECONDARY_DRIVE_1_UDMA_ENABLE bit3

///
/// UDMA Timing Register
///
#define CT4_RP6   0x00
#define CT3_RP5   0x01
#define CT2_RP4   0x02

#define CT3_RP8   0x01
#define CT2_RP8   0x02

#define CT3_RP16  0x01

///
/// Ide I/O Configuration Register Field offset 0x54
///
#define PCB0_PRIMARY_DRIVE_0_BASE_CLOCK   bit0
#define PCB1_PRIMARY_DRIVE_1_BASE_CLOCK   bit1
#define SCB0_SECONDARY_DRIVE_0_BASE_CLOCK bit2
#define SCB1_SECONDARY_DRIVE_1_BASE_CLOCK bit3
#define ATA_FAST_PCB0_PRIMARY_DRIVE_0     bit12
#define ATA_FAST_PCB1_PRIMARY_DRIVE_1     bit13
#define ATA_FAST_SCB0_SECONDARY_DRIVE_0   bit14
#define ATA_FAST_SCB1_SECONDARY_DRIVE_1   bit15

///
/// PCH timing register structure
///
#pragma pack(1)

typedef struct _IDER_TIMING_REG {
  ///
  /// PIO/MDMA/SDMA timing control for drive 0
  ///
  UINT16  Time0 : 1;
  UINT16  Ie0 : 1;
  UINT16  Ppe0 : 1;
  UINT16  Dte0 : 1;

  ///
  /// PIO/MDMA/SDMA timing control for drive 1
  ///
  UINT16  Time1 : 1;
  UINT16  Ie1 : 1;
  UINT16  Ppe1 : 1;
  UINT16  Dte1 : 1;

  ///
  /// PIO/MDMA/SDMA timing
  ///
  UINT16  RecoveryTime : 2;
  UINT16  Reserved0 : 2;
  UINT16  IoRdySample : 2;
  UINT16  Sitre : 1;
  UINT16  IdeDecode : 1;

} IDER_TIMING_REG;

typedef struct _IDER_SLAVE_TIMING_REG {
  ///
  /// PIO/MDMA/SDMA timing control for primary slave device
  ///
  UINT16  PrimaryRecoveryTime : 2;
  UINT16  PrimaryIoRdySample : 2;

  ///
  /// PIO/MDMA/SDMA timing control for secondary slave device
  ///
  UINT16  SecondaryRecoveryTime : 2;
  UINT16  SecondaryIoRdySample : 2;

} IDER_SLAVE_TIMING_REG;

#pragma pack()

#define IDER_CONTROLLER_SIGNATURE SIGNATURE_32 ('I', 'D', 'E', 'R')

///
/// Ide controller driver private data structure
///
typedef struct _EFI_IDE_CONTROLLER_PRIVATE_DATA {
  ///
  /// Standard signature used to identify Ide controller private data
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
  /// The highest disqulified mode for each attached Ide device.
  /// Per ATA/ATAPI spec, if a mode is not supported, the modes higher than
  /// it should not be supported
  ///
  EFI_ATA_COLLECTIVE_MODE           DisqulifiedModes[IDER_MAX_CHANNEL][IDER_MAX_DEVICES];

  ///
  /// A copy of EFI_IDENTIFY_DATA data for each attached Ide device and its flag
  ///
  EFI_IDENTIFY_DATA                 IdentifyData[IDER_MAX_CHANNEL][IDER_MAX_DEVICES];
  BOOLEAN                           IdentifyValid[IDER_MAX_CHANNEL][IDER_MAX_DEVICES];
} EFI_IDE_CONTROLLER_PRIVATE_DATA;

#define IDE_CONTROLLER_PRIVATE_DATA_FROM_THIS(a) \
  CR ( \
  a, \
  EFI_IDE_CONTROLLER_PRIVATE_DATA, \
  IdeInit, \
  IDER_CONTROLLER_SIGNATURE \
  )

///
/// IDE-R controller IDE_CONTROLLER_INIT protocol declaration
///

/**
  This function can be used to obtain information about a specified channel.
  It's usually used by IDE Bus driver during enumeration process.

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              Channel number (0 based, either 0 or 1)
  @param[in] Enabled              TRUE if the channel is enabled. If the channel is disabled,
                                  then it will no be enumerated.
  @param[in] MaxDevices           The Max number of IDE devices that the bus driver can expect
                                  on this channel. For ATA/ATAPI, this number is either 1 or 2.

  @retval EFI_SUCCESS             Information copied
  @retval EFI_INVALID_PARAMETER   Invalid channel
**/
EFI_STATUS
EFIAPI
IdeInitGetChannelInfo (
  IN   EFI_IDE_CONTROLLER_INIT_PROTOCOL *This,
  IN   UINT8                            Channel,
  OUT  BOOLEAN                          *Enabled,
  OUT  UINT8                            *MaxDevices
  );

/**
  This function is called by IdeBus driver before executing certain actions.
  This allows IDE Controller Init to prepare for each action.

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Phase                Phase indicator defined by IDE_CONTROLLER_INIT protocol
  @param[in] Channel              Channel number (0 based, either 0 or 1)

  @retval EFI_SUCCESS             Preparation done
  @retval EFI_INVALID_PARAMETER   Invalid channel
  @exception EFI_UNSUPPORTED      Invalid phase
**/
EFI_STATUS
EFIAPI
IdeInitNotifyPhase (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  EFI_IDE_CONTROLLER_ENUM_PHASE     Phase,
  IN  UINT8                             Channel
  );

/**
  This function is called by IdeBus driver to submit EFI_IDENTIFY_DATA data structure
  obtained from IDE deivce. This structure is used to set IDE timing

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] IdentifyData         A pointer to EFI_IDENTIFY_DATA data structure

  @retval EFI_SUCCESS             Data submitted
  @retval EFI_INVALID_PARAMETER   Invalid channel
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

  @retval EFI_SUCCESS             Disqulified Modes recorded
  @retval EFI_INVALID_PARAMETER   Invalid channel or invalid BadModes pointer
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
  @param[in] SupportedModes       Modes collection supported by IDE device

  @retval EFI_SUCCESS             Disqulified Modes recorded
  @retval EFI_INVALID_PARAMETER   Invalid channel or invalid SupportedModes pointer
  @retval EFI_NOT_READY           IdentifyData is not valid
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
**/
EFI_STATUS
EFIAPI
IdeInitCalculateMode (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  OUT EFI_ATA_COLLECTIVE_MODE           **SupportedModes
  );

/**
  This function is called by IdeBus driver to set appropriate timing on IDE
  controller according supported operation mode

  @param[in] This                 The EFI_IDE_CONTROLLER_INIT_PROTOCOL instance.
  @param[in] Channel              IDE channel number (0 based, either 0 or 1)
  @param[in] Device               IDE device number
  @param[in] Modes                IDE device mode

  @retval EFI_SUCCESS             Disqulified Modes recorded
  @retval EFI_INVALID_PARAMETER   Invalid channel or invalid Modes pointer
  @retval EFI_NOT_READY           IdentifyData is not valid
  @exception EFI_UNSUPPORTED      Failed to set PIO/MDMA/SDMA timing
**/
EFI_STATUS
EFIAPI
IdeInitSetTiming (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL  *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN  EFI_ATA_COLLECTIVE_MODE           *Modes
  );

//
// IDE-R Controller Binding protocol declaration
//

/**
  This function checks to see if the driver supports a device specified by
  "Controller handle" parameter. It is called by DXE Core StartImage() or
  ConnectController() routines. The driver uses 'device path' and/or
  'services' from the Bus I/O abstraction attached to the controller handle
  to determine if the driver support this controller handle.

  Note: In the BDS (Boot Device Selection) phase, the DXE core enumerate all
  devices (or, controller) and assigns GUIDs to them.

  @param[in] This                 a pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested.
  @param[in] RemainingDevicePath  A pointer to the device path. Ignored by device
                                  driver but used by bus driver

  @retval EFI_SUCCESS             Have device to support
  @retval EFI_NOT_FOUND           Relative environment not ready
  @exception EFI_UNSUPPORTED      The device doesn't support
**/
EFI_STATUS
EFIAPI
IdeRControllerSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  This routine is called right after the .Supported() called and return
  EFI_SUCCESS. Notes: The supported protocols are checked but the Protocols
  are closed.

  @param[in] This                 a pointer points to the Binding Protocol instance
  @param[in] Controller           The handle of controller to be tested. Parameter
                                  passed by the caller
  @param[in] RemainingDevicePath  A pointer to the device path. Should be ignored by
                                  device driver

  @retval EFI_SUCCESS             The driver ready and initial complete.
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
  @retval EFI_DEVICE_ERROR        The device doesn't initial.
**/
EFI_STATUS
EFIAPI
IdeRControllerStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  );

/**
  Stop.

  @param[in] This                 Pointer to driver binding protocol
  @param[in] Controller           Controller handle to connect
  @param[in] NumberOfChildren     Number of children handle created by this driver
  @param[in] ChildHandleBuffer    Buffer containing child handle created

  @retval EFI_SUCCESS             Driver disconnected successfully from controller
  @exception EFI_UNSUPPORTED      Cannot find BIOS_VIDEO_DEV structure
**/
EFI_STATUS
EFIAPI
IdeRControllerStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL *This,
  IN  EFI_HANDLE                  Controller,
  IN  UINTN                       NumberOfChildren,
  IN  EFI_HANDLE                  *ChildHandleBuffer
  );

#endif /// _IDER_ATA_CONTROLLER_H
