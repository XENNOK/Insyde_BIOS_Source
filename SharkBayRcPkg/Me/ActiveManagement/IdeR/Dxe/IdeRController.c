/** @file

  This driver module produces IDE_CONTROLLER_INIT protocol and will be used by
  IDE Bus driver to support platform dependent timing information. This driver
  is responsible for early initialization of IDE Redirect controller.

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

#include "IdeRController.h"
///
///  EFI_DRIVER_BINDING_PROTOCOL instance
///
EFI_DRIVER_BINDING_PROTOCOL mIdeRControllerDriverBinding = {
  IdeRControllerSupported,
  IdeRControllerStart,
  IdeRControllerStop,
  1,    // Version
  NULL, // ImageHandle
  NULL  // DriverBindingHandle
};

/**
  This function is used to calculate the best PIO mode supported by
  specific IDE device

  Since the LPT IDE-R doesn't support changing the timing
  registers because they are RO 0x00, we'll just return PIO mode 2.

  @param[in] IdentifyData         The identify data of specific IDE device
  @param[in] DisPioMode           Disqualified PIO modes collection
  @param[in] SelectedMode         Available PIO modes collection

  @retval EFI_SUCCESS             SelectedMode calculated.
**/
EFI_STATUS
CalculateBestPioMode (
  IN  EFI_IDENTIFY_DATA           *IdentifyData,
  IN  UINT16                      *DisPioMode OPTIONAL,
  OUT UINT16                      *SelectedMode
  )
{
  ///
  /// ATA_PIO_MODE_2;
  ///
  *SelectedMode = 2;

  return EFI_SUCCESS;
}

/**
  This function is used to calculate the best UDMA mode supported by
  specific IDE device

  Since the LPT IDE-R doesn't support changing the timing
  registers because they are RO 0x00, we'll just return DMA mode 2.

  @param[in] IdentifyData         The identify data of specific IDE device
  @param[in] DisUDmaMode          Disqualified UDMA modes collection
  @param[in] SelectedMode         Available UMDA modes collection

  @retval EFI_SUCCESS             SelectedMode calculated.
**/
EFI_STATUS
CalculateBestUdmaMode (
  IN  EFI_IDENTIFY_DATA           *IdentifyData,
  IN  UINT16                      *DisUDmaMode OPTIONAL,
  OUT UINT16                      *SelectedMode
  )
{
  ///
  /// ATA_UDMA_MODE_2;
  ///
  *SelectedMode = 2;

  return EFI_SUCCESS;
}

/**
  This function is used to set appropriate PIO timing on Ide
  controller according supported PIO modes

  @param[in] Channel              IDE channel number (0 based, either 0 or 1).
                                  For LPT IDE-R there is only one (See IDER_MAX_CHANNEL).
  @param[in] Device               IDE device number
  @param[in] PciIo                Pointer to PciIo protocol opened by Ide controller driver
  @param[in] IdentifyData         The identify struct submitted by IDE device
  @param[in] Modes                The PIO mode collection supported by IDE device

  @retval EFI_SUCCESS             PIO timing initialized or no need to program PIO mode
**/
EFI_STATUS
IdeInitSetPioTiming (
  IN  UINT8                       Channel,
  IN  UINT8                       Device,
  IN  EFI_PCI_IO_PROTOCOL         *PciIo,
  IN  EFI_IDENTIFY_DATA           *IdentifyData,
  IN  EFI_ATA_COLLECTIVE_MODE     *Modes
  )
{
  ///
  /// Since the Cantiga version of the ME IDER doesn't support registers 40-4F (they are RO),
  /// there is no need to program PIO mode.
  ///
  return EFI_SUCCESS;
}

/**
  This function is used to set appropriate UDMA timing on Ide
  controller according supported UDMA modes

  @param[in] Channel              IDE channel number (0 based, either 0 or 1).
                                  For LPT IDE-R there is only one (See IDER_MAX_CHANNEL).
  @param[in] Device               IDE device number
  @param[in] PciIo                Pointer to PciIo protocol opened by Ide controller driver
  @param[in] Modes                The UDMA mode collection supported by IDE device

  @retval                         Status code returned by PciIo operations
**/
EFI_STATUS
IdeInitSetUdmaTiming (
  IN  UINT8                       Channel,
  IN  UINT8                       Device,
  IN  EFI_PCI_IO_PROTOCOL         *PciIo,
  IN  EFI_ATA_COLLECTIVE_MODE     *Modes
  )
{
  ///
  /// Since the Cantiga version of the ME IDE-R doesn't support registers 40-C7 (they are RO),
  /// there is no need to program PIO/UDMA mode.
  ///
  EFI_STATUS  Status;
  UINT16      PciCommandReg;
  UINT8       BusMasterIdeStatusReg;

  ///
  /// PCI Command Register, offset 0x4, default 00
  ///
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint16,
                        PCI_COMMAND_REGISTER,
                        1,
                        &PciCommandReg
                        );
  ///
  /// Now set the PCH IDE Bus Master Enable bit, one bit for PCH controller
  /// If BME bit is not set, set it
  ///
  if (!(PciCommandReg & BME_BUS_MASTER_ENABLE_BIT)) {
    PciCommandReg |= BME_BUS_MASTER_ENABLE_BIT;

    Status = PciIo->Pci.Write (
                          PciIo,
                          EfiPciIoWidthUint16,
                          PCI_COMMAND_REGISTER, // offset 0x4
                          1,
                          &PciCommandReg
                          );
  }

  Status = PciIo->Io.Read (
                      PciIo,
                      EfiPciIoWidthUint8,
                      4,
                      (Channel == 0 ? 0x2 : 0xA),
                      1,
                      &BusMasterIdeStatusReg
                      );

  BusMasterIdeStatusReg = (UINT8) (BusMasterIdeStatusReg | (Device == 0 ? BIT5 : BIT6));

  Status = PciIo->Io.Write (
                      PciIo,
                      EfiPciIoWidthUint8,
                      4,
                      (Channel == 0 ? 0x2 : 0xA),
                      1,
                      &BusMasterIdeStatusReg
                      );

  return Status;
}

/**
  This function is called after IdeBus driver submits its EFI_IDENTIFY_DATA data struct
  to IDE controller driver. The main purpose is to detect IDE
  cable type.

  @param[in] Channel              IDE channel number (0 based, either 0 or 1).
                                  For LPT IDE-R there is only one (See IDER_MAX_CHANNEL).
  @param[in] Device               IDE device number
  @param[in] PciIo                Pointer to PciIo protocol instance opened by Ide driver
  @param[in] IdentifyData         A pointer to EFI_IDENTIFY_DATA data structure

  @retval EFI_SUCCESS             Cable type detected
**/
EFI_STATUS
IdeDetectCableType (
  IN  UINT8                  Channel,
  IN  UINT8                  Device,
  IN  EFI_PCI_IO_PROTOCOL    *PciIo,
  IN  EFI_IDENTIFY_DATA      *IdentifyData
  )
{
  return EFI_SUCCESS;
}

/**
  This function is called AFTER IdeBus driver submits its EFI_IDENTIFY_DATA data struct.
  The main objective of this function is to adjust best calculated UDMA mode
  according to current cable type. LPT IDE-R is hardcoded to 40 pin UDMA-2/33 mode.
  Note that the cable reporting bits should be set prior to this function call

  @param[in] Channel              IDE channel number (0 based, either 0 or 1).
                                  For LPT IDE-R there is only one (See IDER_MAX_CHANNEL).
  @param[in] Device               IDE device number
  @param[in] PciIo                Pointer to PciIo protocol instance opened by Ide driver
  @param[in] Modes                The current best supported mode calculated by this driver

  @retval EFI_SUCCESS             UdmaMode copied
**/
EFI_STATUS
AdjustUdmaModeByCableType (
  IN     UINT8                   Channel,
  IN     UINT8                   Device,
  IN     EFI_PCI_IO_PROTOCOL     *PciIo,
  IN OUT EFI_ATA_COLLECTIVE_MODE *Modes
  )
{
  Modes->UdmaMode.Mode = 2;
  return EFI_SUCCESS;
}

///
/// Interface functions of IDE_CONTROLLER_INIT protocol
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
  )
{
  ///
  /// Channel number (0 based, either 0 or 1)
  /// For LPT IDE-R there is only one (See IDER_MAX_CHANNEL).
  ///
  if (Channel < IDER_MAX_CHANNEL) {
    *Enabled    = TRUE;
    *MaxDevices = IDER_MAX_DEVICES;
    return EFI_SUCCESS;

  } else {
    return EFI_INVALID_PARAMETER;
  }
}

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
  )
{
  if (Channel >= IDER_MAX_CHANNEL) {
    return EFI_INVALID_PARAMETER;
  }

  switch (Phase) {

  case EfiIdeBeforeChannelEnumeration:
  case EfiIdeAfterChannelEnumeration:
  case EfiIdeBeforeChannelReset:
  case EfiIdeAfterChannelReset:
  case EfiIdeBusBeforeDevicePresenceDetection:
  case EfiIdeBusAfterDevicePresenceDetection:
  case EfiIdeResetMode:
    ///
    /// Do nothing at present
    ///
    break;

  default:
    return EFI_UNSUPPORTED;
    break;
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_IDE_CONTROLLER_PRIVATE_DATA *IdePrivateData;

  if (Channel >= IDER_MAX_CHANNEL || Device >= IDER_MAX_DEVICES) {
    return EFI_INVALID_PARAMETER;
  }

  IdePrivateData = IDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (IdePrivateData);

  ///
  /// Make a local copy of device's IdentifyData and mark the valid flag
  ///
  if (IdentifyData != NULL) {
    CopyMem (
      &(IdePrivateData->IdentifyData[Channel][Device]),
      IdentifyData,
      sizeof (EFI_IDENTIFY_DATA)
      );

    IdePrivateData->IdentifyValid[Channel][Device] = TRUE;

    ///
    /// Detect cable type and set cable type reg once we get identify data
    ///
    IdeDetectCableType (
      Channel,
      Device,
      IdePrivateData->PciIo,
      &(IdePrivateData->IdentifyData[Channel][Device])
      );

  } else {
    IdePrivateData->IdentifyValid[Channel][Device] = FALSE;
  }

  return EFI_SUCCESS;
}

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
  )
{
  EFI_IDE_CONTROLLER_PRIVATE_DATA *IdePrivateData;

  if (Channel >= IDER_MAX_CHANNEL || Device >= IDER_MAX_DEVICES || BadModes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  IdePrivateData = IDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (IdePrivateData);

  ///
  /// Record the disqualified modes per channel per device. From ATA/ATAPI spec,
  /// if a mode is not supported, the modes higher than it is also not
  /// supported
  ///
  CopyMem (
    &(IdePrivateData->DisqulifiedModes[Channel][Device]),
    BadModes,
    sizeof (EFI_ATA_COLLECTIVE_MODE)
    );

  return EFI_SUCCESS;
}

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
  )
{
  EFI_IDE_CONTROLLER_PRIVATE_DATA *IdePrivateData;
  EFI_IDENTIFY_DATA               *IdentifyData;
  BOOLEAN                         IdentifyValid;
  EFI_ATA_COLLECTIVE_MODE         *DisqulifiedModes;
  UINT16                          SelectedMode;
  EFI_STATUS                      Status;

  if (Channel >= IDER_MAX_CHANNEL || Device >= IDER_MAX_DEVICES || SupportedModes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  IdePrivateData  = IDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (IdePrivateData);

  IdentifyData      = &(IdePrivateData->IdentifyData[Channel][Device]);
  DisqulifiedModes  = &(IdePrivateData->DisqulifiedModes[Channel][Device]);
  IdentifyValid     = IdePrivateData->IdentifyValid[Channel][Device];

  ///
  /// Make sure we've got the valid identify data of the device from SubmitData()
  ///
  if (!IdentifyValid) {
    return EFI_NOT_READY;
  }

  *SupportedModes = AllocateZeroPool (sizeof (EFI_ATA_COLLECTIVE_MODE));
  if (*SupportedModes == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = CalculateBestPioMode (
            IdentifyData,
            (DisqulifiedModes->PioMode.Valid ? ((UINT16 *) &(DisqulifiedModes->PioMode.Mode)) : NULL),
            &SelectedMode
            );
  if (!EFI_ERROR (Status)) {
    (*SupportedModes)->PioMode.Valid  = TRUE;
    (*SupportedModes)->PioMode.Mode   = SelectedMode;

  } else {
    (*SupportedModes)->PioMode.Valid = FALSE;
  }

  Status = CalculateBestUdmaMode (
            IdentifyData,
            (DisqulifiedModes->UdmaMode.Valid ? ((UINT16 *) &(DisqulifiedModes->UdmaMode.Mode)) : NULL),
            &SelectedMode
            );
  if (!EFI_ERROR (Status)) {
    (*SupportedModes)->UdmaMode.Valid = TRUE;
    (*SupportedModes)->UdmaMode.Mode  = SelectedMode;

  } else {
    (*SupportedModes)->UdmaMode.Valid = FALSE;
  }
  ///
  /// It is only referenced here.
  ///
  (*SupportedModes)->ExtModeCount = 1;
  ///
  /// The modes other than PIO and UDMA are not supported by Ide controller
  ///
  return EFI_SUCCESS;
}

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
  )
{
  EFI_IDE_CONTROLLER_PRIVATE_DATA *IdePrivateData;

  if (Channel >= IDER_MAX_CHANNEL || Device >= IDER_MAX_DEVICES || Modes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  IdePrivateData = IDE_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (IdePrivateData);

  ///
  /// Make sure we've got the valid identify data of the device from SubmitData()
  ///
  if (!(IdePrivateData->IdentifyValid[Channel][Device])) {
    return EFI_NOT_READY;
  }
  ///
  /// Set UMDA timing
  ///
  if (Modes->UdmaMode.Valid) {
    IdeInitSetUdmaTiming (
      Channel,
      Device,
      IdePrivateData->PciIo,
      Modes
      );
  }
  ///
  /// Set PIO/MDMA/SDMA timing (They generally share the same timing values)
  ///
  if (Modes->PioMode.Valid) {
    IdeInitSetPioTiming (
      Channel,
      Device,
      IdePrivateData->PciIo,
      &(IdePrivateData->IdentifyData[Channel][Device]),
      Modes
      );

  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

///
/// IDE-R Controller Binding protocol declaration
///

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
  )
{
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *ParentDevicePath;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  PCI_TYPE00                      PciData;

  ///
  /// Ide Controller is a device driver, and should ingore the
  /// "RemainingDevicePath" according to EFI spec
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID *) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    ///
    /// EFI_ALREADY_STARTED is also an error
    ///
    return Status;
  }
  ///
  /// Close the protocol because we don't use it here
  ///
  gBS->CloseProtocol (
        Controller,                   // handle of the controller
        &gEfiDevicePathProtocolGuid,  // Porotcol to be closed
        This->DriverBindingHandle,    // agent of opening the protocol
        Controller
        );

  ///
  /// Now test the EfiPciIoProtocol
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Now further check the PCI header: Base class (offset 0x0B) and
  /// Sub Class (offset 0x0A). This controller should be an Ide controller
  ///
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint8,
                        0,
                        sizeof (PciData),
                        &PciData
                        );

  if (EFI_ERROR (Status)) {
    gBS->CloseProtocol (
          Controller,
          &gEfiPciIoProtocolGuid,
          This->DriverBindingHandle,
          Controller
          );
    return EFI_UNSUPPORTED;
  }
  ///
  /// Examine IDE-R PCI Configuration table fields
  ///
  if ((PciData.Hdr.ClassCode[2] != PCI_CLASS_MASS_STORAGE) ||
      (PciData.Hdr.ClassCode[1] != PCI_SUB_CLASS_IDE) ||
      (PciData.Hdr.VendorId != V_ME_IDER_VENDOR_ID) ||
      !IS_PCH_LPT_IDER_DEVICE_ID(PciData.Hdr.DeviceId)
      ) {

    Status = EFI_UNSUPPORTED;
  }
  ///
  /// Close the I/O Abstraction(s) used to perform the supported test
  ///
  gBS->CloseProtocol (
        Controller,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  return Status;
}

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
  )
{
  EFI_STATUS                      Status;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  EFI_IDE_CONTROLLER_PRIVATE_DATA *IdePrivateData;
  UINT64                          CommandVal;

  ///
  /// Now test and open the EfiPciIoProtocol
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  ///
  /// Status == 0 - A normal execution flow, SUCCESS and the program proceeds.
  /// Status == ALREADY_STARTED - A non-zero Status code returned. It indicates
  ///           that the protocol has been opened and should be treated as a
  ///           normal condition and the program proceeds. The Protocol will not
  ///           opened 'again' by this call.
  /// Status != ALREADY_STARTED - Error status, terminate program execution
  ///
  if (EFI_ERROR (Status)) {
    ///
    /// EFI_ALREADY_STARTED is also an error
    ///
    return Status;
  }
  ///
  /// Allocate Ide private data structure
  ///
  IdePrivateData = AllocatePool (sizeof (EFI_IDE_CONTROLLER_PRIVATE_DATA));
  ASSERT (IdePrivateData != NULL);
  if (IdePrivateData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Initialize IdeR controller private data
  ///
  ZeroMem (IdePrivateData, sizeof (EFI_IDE_CONTROLLER_PRIVATE_DATA));
  IdePrivateData->Signature               = IDER_CONTROLLER_SIGNATURE;
  IdePrivateData->PciIo                   = PciIo;
  IdePrivateData->IdeInit.GetChannelInfo  = IdeInitGetChannelInfo;
  IdePrivateData->IdeInit.NotifyPhase     = IdeInitNotifyPhase;
  IdePrivateData->IdeInit.SubmitData      = IdeInitSubmitData;
  IdePrivateData->IdeInit.DisqualifyMode  = IdeInitDisqualifyMode;
  IdePrivateData->IdeInit.CalculateMode   = IdeInitCalculateMode;
  IdePrivateData->IdeInit.SetTiming       = IdeInitSetTiming;
  IdePrivateData->IdeInit.EnumAll         = IDER_ENUMER_ALL;
  IdePrivateData->IdeInit.ChannelCount    = IDER_MAX_CHANNEL;

  //
  // Get device capabilities
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationSupported,
                    0,
                    &CommandVal
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Enable Command Register
  //
  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationEnable,
                    CommandVal & EFI_PCI_DEVICE_ENABLE,
                    NULL
                    );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Install IDE_CONTROLLER_INIT protocol & private data to this instance
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiIderControllerDriverProtocolGuid,
                  IdePrivateData,
                  &gEfiIdeControllerInitProtocolGuid,
                  &(IdePrivateData->IdeInit),
                  NULL
                  );

  return Status;
}

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
  )
{
  EFI_STATUS                      Status;
  EFI_IDE_CONTROLLER_PRIVATE_DATA *IdePrivateData;

  ///
  /// Get private data
  ///
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiIderControllerDriverProtocolGuid,
                  (VOID **) &IdePrivateData,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Close protocols opened by Ide controller driver
  ///
  Status = gBS->CloseProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  This->DriverBindingHandle,
                  Controller
                  );

  gBS->UninstallMultipleProtocolInterfaces (
        Controller,
        &gEfiIderControllerDriverProtocolGuid,
        IdePrivateData,
        &gEfiIdeControllerInitProtocolGuid,
        &(IdePrivateData->IdeInit),
        NULL
        );

  FreePool (IdePrivateData);

  return EFI_SUCCESS;
}

///
/// IDE-R Controller Driver Entry Point
///

/**
  Chipset Ide Driver EntryPoint function. It follows the standard EFI driver
  model. It's called by StartImage() of DXE Core

  @param[in]                      ImageHandle - While the driver image loaded be the ImageLoader(),
                                  an image handle is assigned to this driver binary,
                                  all activities of the driver is tied to this ImageHandle
  @param[in]                      SystemTable - A pointer to the system table, for all BS(Boo Services) and
                                  RT(Runtime Services)

  @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
InitializeIdeRControllerDriver (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                           Status;
  
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &mIdeRControllerDriverBinding,
             ImageHandle,
             &mIdeRControllerName,
             &mIdeRControllerName2
             );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
