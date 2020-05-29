/** @file
 H2O IPMI Misc module implement code.

 H2O IPMI Misc DXE module implementation file.
 This modules contains these functions:
 1. Create SMBIOS Type 38 Record
 2. Create ACPI SPMI Table
 3. Send BIOS Version String to BMC
 4. Process IPMI Boot Option Request
 5. Stop IPMI Watchdog Timer Handler

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include "IpmiMiscDxe.h"


H2O_IPMI_INTERFACE_PROTOCOL             *mIpmi = NULL;
H2O_IPMI_GET_BOOT_OPTION                mGetBootOpt;


/**
 Create SMBIOS Type 38 Record

 @retval EFI_SUCCESS                    Create SMBIOS type 38 success.
 @retval !EFI_SUCCESS                   Create SMBIOS type 38 fail.
*/
EFI_STATUS
CreateSmbiosType38 (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_SMBIOS_PROTOCOL                   *Smbios;
  SMBIOS_TABLE_TYPE38                   *Type38;
  EFI_SMBIOS_HANDLE                     SmbiosHandle;


  //
  // Follow PI specification, use SMBIOS protocol to add record
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Allocate buffer for SMBIOS_TABLE_TYPE38 and two consecutive zeros behind.
  // Because Smbios add function will looking for the two consecutive zeros to check the string limit.
  //
  Type38 = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE38) + 2);

  //
  // Fill header
  //
  Type38->Hdr.Type = EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION;
  Type38->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE38);
  Type38->Hdr.Handle = 0;

  //
  // Fill data
  //
  Type38->InterfaceType = mIpmi->GetIpmiInterfaceType (mIpmi);
  Type38->IPMISpecificationRevision = mIpmi->GetIpmiVersion (mIpmi);
  Type38->I2CSlaveAddress = 0x20;
  Type38->NVStorageDeviceAddress = 0xFF;
  Type38->BaseAddress = (UINT64)(mIpmi->GetIpmiBaseAddress (mIpmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX)) |
                                 mIpmi->GetIpmiBaseAddressType (mIpmi);
  Type38->InterruptNumber = 0x00;
  switch (mIpmi->GetIpmiBaseAddressOffset (mIpmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX)) {
  case 1:
    Type38->BaseAddressModifier_InterruptInfo = 0x00;
    break;

  case 4:
    Type38->BaseAddressModifier_InterruptInfo = 0x40;
    break;

  case 16:
    Type38->BaseAddressModifier_InterruptInfo = 0x80;
    break;

  default:
    Type38->BaseAddressModifier_InterruptInfo = 0x00;
    break;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;

  //
  // Reserved for OEM to implement something additional requirements before add SMBIOS record.
  //
  OemIpmiBeforeAddSmbiosRecord (&SmbiosHandle, Type38);

  Status = Smbios->Add (Smbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)Type38);

  if (Type38 != NULL) {
    FreePool (Type38);
  }

  return Status;

}


/**
 Create ACPI SPMI Description Table.

 @retval EFI_SUCCESS                    Create SPMI table success.
 @retval !EFI_SUCCESS                   Create SPMI table fail.
*/
EFI_STATUS
CreateSpmiTable (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTable;
  UINT8                                 *Buffer;
  UINTN                                 Size;
  UINTN                                 ChecksumOffset;
  UINTN                                 SpmiKey;

  //
  // Default SPMI Table value
  //
  EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE Spmi = {
    EFI_ACPI_5_0_SERVER_PLATFORM_MANAGEMENT_INTERFACE_TABLE_SIGNATURE,
    sizeof (EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_DESCRIPTION_TABLE),
    EFI_ACPI_SERVER_PROCESSOR_MANAGEMENT_INTERFACE_TABLE_REVISION,
    0,
    EFI_ACPI_OEM_ID,
    EFI_ACPI_OEM_TABLE_ID,
    EFI_ACPI_OEM_REVISION,
    EFI_ACPI_CREATOR_ID,
    EFI_ACPI_CREATOR_REVISION,
    0x01,
    0x01,
    0x0150,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00000000,
    0x00,
    0x00,
    0x00,
    0x00,
    0x0000000000000000,
    0x00,
    0x00,
    0x00,
    0x00
    };

  //
  // Since we're here, we can locate EFI_ACPI_TABLE_PROTOCOL to generate SPMI table
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // SPMI table content
  //
  Spmi.Reserved_37 = 0x01;
  Spmi.InterfaceType = mIpmi->GetIpmiInterfaceType (mIpmi);
  Spmi.SpecificationRevision = (UINT16)(((mIpmi->GetIpmiVersion (mIpmi)) & 0xF0) << 4) |
                                (UINT16)((mIpmi->GetIpmiVersion (mIpmi)) & 0x0F);
  Spmi.BaseAddress.AddressSpaceId = mIpmi->GetIpmiBaseAddressType (mIpmi);
  Spmi.BaseAddress.Address = (UINT64)(mIpmi->GetIpmiBaseAddress (mIpmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX));
  Spmi.BaseAddress.RegisterBitWidth = (UINT8)(8 * mIpmi->GetIpmiBaseAddressOffset (mIpmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX));
  Spmi.BaseAddress.RegisterBitOffset = 0;
  Spmi.BaseAddress.Reserved = 1;

  //
  // Reserved for OEM to implement something additional requirements before install ACPI Table.
  //
  OemIpmiBeforeInstallAcpiTable (&Spmi);

  //
  // Update SPMI table checksum
  //
  Buffer = (UINT8*)&Spmi;
  Size = Spmi.Header.Length;
  ChecksumOffset = OFFSET_OF (EFI_ACPI_DESCRIPTION_HEADER, Checksum);
  Buffer[ChecksumOffset] = CalculateCheckSum8 (Buffer, Size);
  SpmiKey = 0;

  Status = AcpiTable->InstallAcpiTable (
                        AcpiTable,
                        &Spmi,
                        Spmi.Header.Length,
                        &SpmiKey
                        );

  return Status;

}


/**
 Send "Set System Info Parameters" command to BMC and set BIOS version.

*/
VOID
SetBiosVersion (
  VOID
  )
{
  EFI_STATUS                            Status;
  CHAR16                                *BiosVersionStr;
  UINT8                                 SendData[0x20];
  UINT8                                 RecvData[0x10];
  UINT8                                 RecvDataLength;
  UINT8                                 Count;
  UINT8                                 Selector;
  CHAR16                                *Source;
  UINT8                                 *Destination;
  UINT8                                 MaxLength;
  H2O_IPMI_CMD_HEADER                   Request;


  BiosVersionStr = NULL;
  BiosVersionStr = (CHAR16 *) PcdGetPtr (PcdFirmwareVersionString);

  if (BiosVersionStr == NULL || StrLen (BiosVersionStr) == 0) {
    DEBUG ((EFI_D_ERROR, "[IpmiMiscDxe] Cannot Get Bios Version String.\n"));
    return;
  }
  DEBUG ((EFI_D_ERROR, "[IpmiMiscDxe] BiosVersionStr: %s\n", BiosVersionStr));

  //
  // Byte 0: Parameter
  //
  Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
  Request.Cmd = H2O_IPMI_CMD_SET_SYSTEM_INFO_PARM;
  Request.Lun = H2O_IPMI_BMC_LUN;
  Source = BiosVersionStr;

  for (Selector = 0; *Source != L'\0'; ++Selector) {
    ZeroMem (SendData, sizeof (SendData));
    SendData[0] = MSG_SYSTEM_FIRMWARE_VERSION;

    if (Selector == 0) {
      //
      // Byte 2: Encoding
      // Byte 3: Length
      // These two bytes are only available when Selector is 0
      //
      SendData[2] = 0;
      SendData[3] = (UINT8)StrLen (BiosVersionStr);
      Destination = (UINT8*)&SendData[4];
      MaxLength = 14;
    } else {
      Destination = (UINT8*)&SendData[2];
      MaxLength = 16;
    }

    SendData[1] = Selector;

    for (Count = 0; ((*Source != L'\0') && (Count < MaxLength)); ++Count) {
      *Destination = (UINT8)*Source;
      ++Destination;
      ++Source;
    }

    Count = 18;

    Status = mIpmi->ExecuteIpmiCmd (
                      mIpmi,
                      Request,
                      SendData,
                      Count,
                      RecvData,
                      &RecvDataLength,
                      NULL
                      );

    DEBUG ((EFI_D_ERROR, "[IpmiMiscDxe] SetBiosVersion, Selector %d %r\n", Selector, Status));

    if (EFI_ERROR (Status)) {
      return;
    }
  }

}


/**
 Start BMC watchdog timer callback function.

 This callback function that used to restart BMC watchdog timer when H2O_IPMI_WATCHDOG_TIMER_START_GUID is triggered.

 @param[in]         Event               The Event associated with callback.
 @param[in]         Context             Context registered when Event was created.

*/
VOID
EFIAPI
StartBmcWatchdogTimerCallback (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  UINT8                                 RecvBuf;
  H2O_IPMI_CMD_HEADER                   Request;

  //
  // Restart the watchdog timer from the initial countdown value
  // that was specified in the Set Watchdog Timer command.
  //
  Request.Lun = H2O_IPMI_BMC_LUN;
  Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
  Request.Cmd = H2O_IPMI_CMD_RESET_WATCHDOG_TIMER;

  Status = mIpmi->ExecuteIpmiCmd (
                  mIpmi,
                  Request,
                  NULL,
                  0,
                  &RecvBuf,
                  &RecvSize,
                  NULL
                  );

  DEBUG ((EFI_D_ERROR, "[IpmiMiscDxe] Reset BMC Watchdog Timer: %r\n", Status));

}


/**
 Stop BMC watchdog timer callback funciton.

 This callback function that used to stop BMC watchdog timer when H2O_IPMI_WATCHDOG_TIMER_STOP_GUID is triggered.

 @param[in]         Event               The Event associated with callback.
 @param[in]         Context             Context registered when Event was created.

*/
VOID
EFIAPI
StopBmcWatchdogTimerCallback (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  UINT8                                 RecvBuf;
  H2O_IPMI_GET_WATCHDOG_TIMER           OrigWdt;
  H2O_IPMI_SET_WATCHDOG_TIMER           NewWdt;
  H2O_IPMI_CMD_HEADER                   Request;


  //
  // Get current watchdog timer
  //
  Request.Lun = H2O_IPMI_BMC_LUN;
  Request.NetFn = H2O_IPMI_NETFN_APPLICATION;
  Request.Cmd = H2O_IPMI_CMD_GET_WATCHDOG_TIMER;

  Status = mIpmi->ExecuteIpmiCmd (
                    mIpmi,
                    Request,
                    NULL,
                    0,
                    &OrigWdt,
                    &RecvSize,
                    NULL
                    );

  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Do nothing if current watchdog timer is not running
  //
  if ((OrigWdt.TimerRuning) == 0) {
    return;
  }

  //
  // Use original watchdog timer as default value
  //
  CopyMem (&NewWdt, &OrigWdt, sizeof (H2O_IPMI_SET_WATCHDOG_TIMER));
  NewWdt.NotStopTimer = 0;

  Request.Cmd = H2O_IPMI_CMD_SET_WATCHDOG_TIMER;
  Status = mIpmi->ExecuteIpmiCmd (
                    mIpmi,
                    Request,
                    &NewWdt,
                    sizeof (H2O_IPMI_SET_WATCHDOG_TIMER),
                    &RecvBuf,
                    &RecvSize,
                    NULL
                    );

  DEBUG ((EFI_D_ERROR, "[IpmiMiscDxe] Set BMC Watchdog Timer: %r\n", Status));

}


/**
 Free all allocated memory.

 @param[in]         OrigBootOrder       A pointer to buffer that stores original boot order.
 @param[in]         NewBootOrder        A pointer to buffer that stores new boot order.
 @param[in]         BootOrderList       A pointer to buffer that stores boot order list.
 @param[in]         BootOrderListCount  Number of boot order list.

*/
VOID
FreeAllResource (
  IN  VOID                              *OrigBootOrder, OPTIONAL
  IN  VOID                              *NewBootOrder, OPTIONAL
  IN  UINT8                             **BootOrderList, OPTIONAL
  IN  UINT8                             BootOrderListCount
  )
{
  UINT8                                 Index;

  if (OrigBootOrder != NULL) {
    FreePool (OrigBootOrder);
  }

  if (NewBootOrder != NULL) {
    FreePool (NewBootOrder);
  }

  if (BootOrderList != NULL) {
    for (Index = 0; Index < BootOrderListCount; ++Index) {
      if (BootOrderList[Index] != NULL) {
        FreePool (BootOrderList[Index]);
      }
    }

    FreePool (BootOrderList);
  }

}



/**
 Check if this is specified EFI boot device or not.

 @param[in]         DevicePath          A pointer to boot device's device path.
 @param[in]         Type                The specified device's type in device path.
 @param[in]         SubType             The specified device's sub-type in device path.
 @param[in]         BootDeviceSelector  The Boot Flags Data 2 value of Boot Device Selector.

 @retval TRUE: This EFI boot device is what we want.
         FALSE: This EFI boot device is not what we want.
*/
BOOLEAN
CheckEfiBootOrderOption (
  IN  EFI_DEVICE_PATH_PROTOCOL          *DevicePath,
  IN  UINT8                             Type,
  IN  UINT8                             SubType,
  IN  UINT8                             BootDeviceSelector
  )
{
  EFI_DEVICE_PATH_PROTOCOL              *TempDevicePath;

  TempDevicePath = DevicePath;

  while (!IsDevicePathEnd (TempDevicePath)) {
    if ((DevicePathType (TempDevicePath) == Type) && (DevicePathSubType (TempDevicePath) == SubType)) {
      return TRUE;
    }

    if ((FixedPcdGet8 (PcdIpmiUsbFlashDiskClassify) == 1) && (BootDeviceSelector == ForceDefaultHardDisk)) {
      //
      // When USB Flash Disk as Hard-drive, USB Flash Device Path Type = Media Device Path(Type 4),
      // Device Path Sub-Type = Hard Drive(Sub-Type 1).
      //
      if ((DevicePathType (TempDevicePath) == MEDIA_DEVICE_PATH) &&
          (DevicePathSubType (TempDevicePath) == MEDIA_HARDDRIVE_DP)) {
        return TRUE;
      }
    }

    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return FALSE;

}


/**
 Search specified EFI boot device and return it's number.

 @param[in]         GetBootOpt          Boot option setting from BMC.
 @param[in]         OrigBootOrder       Original BootOrder variable data.
 @param[in]         OrigBootOrderCount  The number of BootOption in original BootOrder variable.
 @param[in]         BootOptionList      Array of BootOption pointer, each points to a BootOption.

 @return The specified boot device number, 0xFFFF means not found.
*/
UINT16
SearchEfiBootDevice (
  IN  H2O_IPMI_GET_BOOT_OPTION          *GetBootOpt,
  IN  UINT16                            *OrigBootOrder,
  IN  UINT8                             OrigBootOrderCount,
  IN  UINT8                             **BootOptionList
  )
{
  UINT8                                 Type;
  UINT8                                 SubType;
  UINT8                                 Index;
  UINT8                                 *TempPtr;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  UINT16                                BootDeviceNumber;

  Type = MEDIA_DEVICE_PATH;
  SubType = MEDIA_FILEPATH_DP;

  //
  // Find out the boot device setting in BMC
  //
  switch (GetBootOpt->BootDeviceSelector) {
  case ForcePxe:
    Type = MESSAGING_DEVICE_PATH;
    SubType = MSG_MAC_ADDR_DP;
    break;

  case ForceDefaultHardDisk:
    Type = MEDIA_DEVICE_PATH;
    SubType = MEDIA_FILEPATH_DP;
    break;

  case ForceDefaultCD:
    Type = MEDIA_DEVICE_PATH;
    SubType = MEDIA_CDROM_DP;
    break;

  case ForcePrimaryRemovableMedia:
    Type = MESSAGING_DEVICE_PATH;
    SubType = MSG_USB_DP;
    break;

  default:
    return INVALID_BOOT_NUMBER;
    break;

  }

  //
  // Find the first type we need
  //
  BootDeviceNumber = INVALID_BOOT_NUMBER;
  for (Index = 0; Index < OrigBootOrderCount; ++Index) {

    if (OrigBootOrder[Index] != INVALID_BOOT_NUMBER) {

      //
      // Get DevicePath first
      //
      TempPtr = BootOptionList[Index];
      TempPtr += sizeof (UINT32) + sizeof (UINT16);
      DevicePath = (EFI_DEVICE_PATH_PROTOCOL*)(TempPtr + StrSize ((CHAR16 *) TempPtr));

      if (CheckEfiBootOrderOption (DevicePath, Type, SubType, GetBootOpt->BootDeviceSelector)) {
        BootDeviceNumber = OrigBootOrder[Index];
        OrigBootOrder[Index] = INVALID_BOOT_NUMBER;

        break;
      }
    }
  }

  return BootDeviceNumber;

}


/**
 Check if this legacy boot device is what we want or not.

 @param[in]         BootOptionBbsTable  BBS table of the boot option to be checked.
 @param[in]         DeviceTypePtr       The specified device's type in BBS_TABLE.
 @param[in]         ClassPtr            The class code of the specified device.
 @param[in]         SubClassPtr         The subclass code of the specified device.
 @param[in]         BootDeviceSelector  The Boot Flags Data 2 value of Boot Device Selector.

 @retval TRUE: This device is what we want.
         FALSE: This device is not what we want.
*/
BOOLEAN
CheckLegacyBootOrderOption (
  IN  BBS_TABLE                         *BootOptionBbsTable,
  IN  UINT16                            *DeviceTypePtr,
  IN  UINT8                             *ClassPtr,
  IN  UINT8                             *SubClassPtr,
  IN  UINT8                             BootDeviceSelector
  )
{

  if (DeviceTypePtr != NULL && *DeviceTypePtr != BootOptionBbsTable->DeviceType) {
    return FALSE;
  }

  if (((FixedPcdGet8 (PcdIpmiUsbFlashDiskClassify) == 1) && (BootDeviceSelector == ForceDefaultHardDisk)) ||
      ((FixedPcdGet8 (PcdIpmiUsbCdromClassify) == 1) && (BootDeviceSelector == ForceDefaultCD))) {
    //
    // Deal with if Usb Flash Disk classified as Hard Disk or Usb Cd-Rom classified as CD/DVD.
    //
    if (*ClassPtr == BootOptionBbsTable->Class) {
      //
      // Possibility 1: Hard Disk & CD/DVD (Class = PCI_CLASS_MASS_STORAGE, SubClass = NULL)
      //
      return TRUE;
    }

    if (PCI_CLASS_SERIAL == BootOptionBbsTable->Class && PCI_CLASS_SERIAL_USB == BootOptionBbsTable->SubClass) {
      //
      // Possibility 2: Usb Flash Disk & Usb Cd-Rom (Class = PCI_CLASS_SERIAL, SubClass = PCI_CLASS_SERIAL_USB)
      //
      return TRUE;
    }

    return FALSE;

  } else {

    if (ClassPtr != NULL && *ClassPtr != BootOptionBbsTable->Class) {
      return FALSE;
    }

    if (SubClassPtr != NULL && *SubClassPtr != BootOptionBbsTable->SubClass) {
      return FALSE;
    }

  }

  return TRUE;

}


/**
 Search specified legacy boot device and return it's number.

 @param[in]         GetBootOpt          Boot option setting from BMC.
 @param[in]         OrigBootOrder       Original BootOrder variable data.
 @param[in]         OrigBootOrderCount  The number of BootOption in original BootOrder variable.
 @param[in]         BootOptionList      Array of BootOption pointer, each points to a BootOption.

 @return The specified boot device number, 0xFFFF means not found.
*/
UINT16
SearchLegacyBootDevice (
  IN  H2O_IPMI_GET_BOOT_OPTION          *GetBootOpt,
  IN  UINT16                            *OrigBootOrder,
  IN  UINT8                             OrigBootOrderCount,
  IN  UINT8                             **BootOptionList
  )
{
  UINT16                                DeviceType;
  UINT16                                *DeviceTypePtr;
  UINT8                                 Class;
  UINT8                                 *ClassPtr;
  UINT8                                 SubClass;
  UINT8                                 *SubClassPtr;
  UINT8                                 Index;
  UINT8                                 *TempPtr;
  BBS_TABLE                             *BootOptionBbsTable;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath;
  UINT16                                BootDeviceNumber;

  DeviceTypePtr = &DeviceType;
  ClassPtr = &Class;
  SubClassPtr = &SubClass;

  //
  // Find out the boot device setting in BMC
  //
  switch (GetBootOpt->BootDeviceSelector) {
  case ForcePxe:
    DeviceTypePtr = NULL;
    *ClassPtr = PCI_CLASS_NETWORK;
    SubClassPtr = NULL;
    break;

  case ForceDefaultHardDisk:
    *DeviceTypePtr = BBS_HARDDISK;
    *ClassPtr = PCI_CLASS_MASS_STORAGE;
    SubClassPtr = NULL;
    break;

  case ForceDefaultCD:
    *DeviceTypePtr = BBS_CDROM;
    *ClassPtr = PCI_CLASS_MASS_STORAGE;
    SubClassPtr = NULL;
    break;

  case ForcePrimaryRemovableMedia:
    DeviceTypePtr = NULL;
    *ClassPtr = PCI_CLASS_SERIAL;
    *SubClassPtr = PCI_CLASS_SERIAL_USB;
    break;

  default:
    return INVALID_BOOT_NUMBER;
    break;

  }

  //
  // Find the first type we need
  //
  BootDeviceNumber = INVALID_BOOT_NUMBER;
  for (Index = 0; Index < OrigBootOrderCount; ++Index) {

    if (OrigBootOrder[Index] != INVALID_BOOT_NUMBER) {
      //
      // Get BBS table first
      //
      TempPtr = BootOptionList[Index];
      TempPtr += sizeof (UINT32) + sizeof (UINT16);
      DevicePath = (EFI_DEVICE_PATH_PROTOCOL*)(TempPtr + StrSize ((CHAR16 *) TempPtr));
      BootOptionBbsTable = (BBS_TABLE*)((UINT8*)DevicePath + GetDevicePathSize (DevicePath));

      if (CheckLegacyBootOrderOption (
            BootOptionBbsTable,
            DeviceTypePtr,
            ClassPtr,
            SubClassPtr,
            GetBootOpt->BootDeviceSelector
            )) {
        BootDeviceNumber = OrigBootOrder[Index];
        OrigBootOrder[Index] = INVALID_BOOT_NUMBER;

        break;
      }
    }
  }

  return BootDeviceNumber;
}


/**
 Search specified boot device and return it's number.

 @param[in]         GetBootOpt          Boot option setting from BMC.
 @param[in]         OrigBootOrder       Original BootOrder variable data.
 @param[in]         OrigBootOrderCount  The number of BootOption in original BootOrder variable.
 @param[in]         BootOptionList      Array of BootOption pointer, each points to a BootOption.

 @return The specified boot device number, 0xFFFF means not found.
*/
UINT16
SearchBootDevice (
  IN  H2O_IPMI_GET_BOOT_OPTION          *GetBootOpt,
  IN  UINT16                            *OrigBootOrder,
  IN  UINT8                             OrigBootOrderCount,
  IN  UINT8                             **BootOptionList
  )
{

  if (GetBootOpt->BiosBootType == BOOT_OPTION_BOOT_TYPE_LEGACY) {
    return SearchLegacyBootDevice (
             GetBootOpt,
             OrigBootOrder,
             OrigBootOrderCount,
             BootOptionList
             );
  } else {
    return SearchEfiBootDevice (
             GetBootOpt,
             OrigBootOrder,
             OrigBootOrderCount,
             BootOptionList
             );
  }

}


/**
 Change boot order according the BMC's setting.

 @param[in]         GetBootOpt          Boot option setting from BMC.

*/
VOID
MakeBootOrder (
  IN  H2O_IPMI_GET_BOOT_OPTION          *GetBootOpt
  )
{
  UINT16           *OrigBootOrder;
  UINTN            OrigBootOrderSize;
  UINT8            OrigBootOrderCount;
  UINT16           *NewBootOrder;
  UINT8            **BootOptionList;
  UINTN            BootOptionListSize;
  UINT8            Index;
  UINT8            NewBootOrderIndex;
  CHAR16           BootOrderName[16];
  UINT8            *BootOption;
  UINTN            BootOptionSize;
  EFI_STATUS       Status;
  UINT16           BootDeviceNumber;
  CHAR16           *BootOrderStr;
  EFI_GUID         *BootOrderGuid;


  //
  // UEFI spcification defines that boot devices order must store in "BootOrder"
  // variable. But some implements will use other variable name to store real
  // boot devices order. Get this variable name from PCD that defined at build time.
  //
  BootOrderStr = NULL;
  BootOrderStr = (CHAR16 *) PcdGetPtr (PcdIpmiRealBootOrderString);

  if (BootOrderStr == NULL || StrLen (BootOrderStr) == 0) {
    DEBUG ((EFI_D_ERROR, "[IpmiMiscDxe] Cannot Get Boot Devices Order String.\n"));
    return;
  }


  //
  // Get BootOrder variable first
  //
  BootOrderGuid = &gEfiGlobalVariableGuid;
  if (PcdGet8 (PcdIpmiRealBootOrderGuid) == 1) {
    BootOrderGuid = &gEfiGenericVariableGuid;
  }

  Status = GetVariable2 (BootOrderStr, BootOrderGuid, (VOID**)&OrigBootOrder, &OrigBootOrderSize);
  if (EFI_ERROR (Status) || OrigBootOrder == NULL || OrigBootOrderSize == 0) {
    return;
  }

  //
  // Allocate memory for all BootOption
  //
  OrigBootOrderCount = (UINT8)(OrigBootOrderSize / sizeof (UINT16));
  BootOptionListSize = OrigBootOrderCount * sizeof (UINT8*);
  BootOptionList = AllocateZeroPool (BootOptionListSize);

  if (BootOptionList == NULL) {
    FreeAllResource (OrigBootOrder, NULL, NULL, 0);
    return;
  }

  //
  // Get all BootOption
  //
  ZeroMem (BootOrderName, sizeof (BootOrderName));
  for (Index = 0; Index < OrigBootOrderCount; ++Index) {
    UnicodeSPrint (BootOrderName, sizeof (BootOrderName), L"Boot%04x", OrigBootOrder[Index]);

    Status = GetVariable2 (BootOrderName, &gEfiGlobalVariableGuid, (VOID**)&BootOption, &BootOptionSize);
    if (!EFI_ERROR (Status) && BootOption != NULL) {
      BootOptionList[Index] = BootOption;
    }
  }

  //
  // Find the only one boot device specified by boot device selector
  //
  BootDeviceNumber = SearchBootDevice (
                       GetBootOpt,
                       OrigBootOrder,
                       OrigBootOrderCount,
                       BootOptionList
                       );

  //
  // If we cannot find the specified boot device, keep original BootOrder
  //
  NewBootOrder = NULL;
  if (BootDeviceNumber != INVALID_BOOT_NUMBER) {
    //
    // If Persistent bit not set, keep original BootOrder
    //
    if (!GetBootOpt->Persistent) {
      PcdSet16 (PcdIpmiBootNext, BootDeviceNumber);
    } else {
      //
      // Move the specified boot device to be the first boot device
      //
      NewBootOrder = AllocateZeroPool (OrigBootOrderSize);

      if (NewBootOrder == NULL) {
        FreeAllResource (
          OrigBootOrder,
          NewBootOrder,
          BootOptionList,
          OrigBootOrderCount
          );
        return;
      }
      NewBootOrder[0] = BootDeviceNumber;

      //
      // Move rest original boot order to new boot order
      //
      NewBootOrderIndex = 1;
      for (Index = 0; Index < OrigBootOrderCount; ++Index) {
        if (OrigBootOrder[Index] != INVALID_BOOT_NUMBER) {
          NewBootOrder[NewBootOrderIndex] = OrigBootOrder[Index];
          ++NewBootOrderIndex;
        }
      }

      //
      // Update "BootOrder" variable
      //
      Status = gRT->SetVariable (
                      BootOrderStr,
                      BootOrderGuid,
                      BOOT_VAR_ATTR,
                      NewBootOrderIndex * sizeof (UINT16),
                      NewBootOrder
                      );
    }

  }

  FreeAllResource (OrigBootOrder, NewBootOrder, BootOptionList, OrigBootOrderCount);

}

/**
 This callback will be called when H2O_IPMI_BOOT_OPTION_GUID is triggered and
 will process enumerated boot options.

 @param[in]         Event               The Event associated with callback.
 @param[in]         Context             Context registered when Event was created.

*/
VOID
EFIAPI
IpmiBootOptionCallback (
  IN  EFI_EVENT                         Event,
  IN  VOID                              *Context
  )
{

  EFI_STATUS       Status;
  
  switch (mGetBootOpt.BootDeviceSelector) {
    
  case ForceSetupUtility:
    Status = OemIpmiEnterSetup ();
    if (Status == EFI_UNSUPPORTED) {
      IpmiEnterSetup ();
    }
    break;

  default:
    MakeBootOrder (&mGetBootOpt);
    break;

  };


  gBS->CloseEvent (Event);

}


/**
 Platform boot according boot options in BMC.

 This function will exam BMC settings using "Get System Boot Options Command",
 parameter 5 (boot flags) defined in IPMI specification. This parameter allows
 user to assign specified boot device to boot, lock buttons, and so on. This
 function is a handler that handle these requirements.

*/
VOID
ProcessIpmiBootOption (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 RecvSize;
  UINT8                                 RecvData[4];
  UINT8                                 RequestData[6];
  H2O_IPMI_GET_CHASSIS_STATUS           *ChassisStatus;
  H2O_IPMI_CMD_HEADER                   Request;
  EFI_GUID                              *BootOptionEventGuid;


  //
  // Use H2OIpmiInterfaceProtocol to send "Get System Boot Option" command
  //
  Request.NetFn = H2O_IPMI_NETFN_CHASSIS;
  Request.Cmd = H2O_IPMI_CMD_GET_SYSTEM_BOOT_OPTIONS;
  Request.Lun = H2O_IPMI_BMC_LUN;

  ZeroMem (RequestData, sizeof (RequestData));
  RequestData[0] = BOOT_OPTION_PARA_BOOT_FLAGS;
  Status = mIpmi->ExecuteIpmiCmd (
                    mIpmi,
                    Request,
                    RequestData,
                    3,
                    &mGetBootOpt,
                    &RecvSize,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Use the 'Set System Boot Options' command to clear the Boot Options valid flag in Data1, bit 7.
  // All other bits should be zero.
  //
  Request.Cmd = H2O_IPMI_CMD_SET_SYSTEM_BOOT_OPTIONS;
  Status = mIpmi->ExecuteIpmiCmd (
                    mIpmi,
                    Request,
                    RequestData,
                    6,
                    RecvData,
                    &RecvSize,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Check Boot Flag Valid bit
  //
  if (mGetBootOpt.BootFlagsValid != BOOT_OPTION_BOOT_FLAG_VALID) {

    //
    // ipmitool will set parameter 4, Data 2, bit 0
    // It is a way to info BIOS that it has set boot option data
    //
    Request.Cmd = H2O_IPMI_CMD_GET_SYSTEM_BOOT_OPTIONS;
    RequestData[0] = BOOT_OPTION_PARA_BOOT_INFO_ACK;
    Status = mIpmi->ExecuteIpmiCmd (
                      mIpmi,
                      Request,
                      RequestData,
                      3,
                      RecvData,
                      &RecvSize,
                      NULL
                      );
    if (EFI_ERROR (Status) || (RecvData[3] & 0x01) == 0) {
      return;
    }

  }

  //
  // If code can get here, it means users may use ipmitool to set boot option
  // we need to clear parameter 4, Data 2, bit 0
  //
  Request.Cmd = H2O_IPMI_CMD_SET_SYSTEM_BOOT_OPTIONS;
  RequestData[0] = BOOT_OPTION_PARA_BOOT_INFO_ACK;
  RequestData[1] = 0x01;
  RequestData[2] = 0x00;
  Status = mIpmi->ExecuteIpmiCmd (
                    mIpmi,
                    Request,
                    RequestData,
                    3,
                    RecvData,
                    &RecvSize,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Check users want to lock Reset/Power/Sleep(Standby) buttons or not
  // According to IPMI specification, this only applies to Persistent bit = 1
  //
  RequestData[0] = 0;
  if (mGetBootOpt.Persistent) {
    //
    // We need to get front panel button capabilities and disable/enable status
    //
    Request.Cmd = H2O_IPMI_CMD_GET_CHASSIS_STATUS;
    Status = mIpmi->ExecuteIpmiCmd (
                      mIpmi,
                      Request,
                      NULL,
                      0,
                      RecvData,
                      &RecvSize,
                      NULL
                      );
    if (!EFI_ERROR (Status)) {
      ChassisStatus = (H2O_IPMI_GET_CHASSIS_STATUS*)RecvData;

      //
      // Reset can be locked and should be locked?
      //
      if (mGetBootOpt.LockResetBtn && ChassisStatus->ResetBtnDisableAllow) {
        RequestData[0] |= DISABLE_BUTTON_RESET;
      }

      //
      // Power can be locked and should be locked?
      //
      if (mGetBootOpt.LockPowerBtn && ChassisStatus->PowerBtnDisableAllow) {
        RequestData[0] |= DISABLE_BUTTON_POWER;
      }

      //
      // Sleep(Standby) can be locked and should be locked?
      //
      if (mGetBootOpt.LockSleepBtn && ChassisStatus->StandbyBtnDisableAllow) {
        RequestData[0] |= DISABLE_BUTTON_STANDBY;
      }

      Request.Cmd = H2O_IPMI_CMD_SET_FRONT_PANEL_BTN_ENABLE;
      mIpmi->ExecuteIpmiCmd (mIpmi, Request, RequestData, 1, RecvData, &RecvSize, NULL);

    }

  }


  //
  // Examine Data2, Bit 6
  // Disable PS/2 style keyboard and mouse in ACPI tables.
  //
  if (mGetBootOpt.LockKeyboard == BOOT_OPTION_LOCK_KEYBOARD) {
    Status = OemIpmiSetAcpiKeyboard (FALSE);
    if (Status == EFI_UNSUPPORTED) {
      IpmiSetAcpiKeyboard (FALSE);
    }
  }


  //
  // Examine Boot Device Selector to see if
  // we should change BootOrder variable or not
  //
  if (mGetBootOpt.BootDeviceSelector != NoOverride) {
    BootOptionEventGuid = (EFI_GUID*)PcdGetPtr (PcdIpmiBootOptionEventGuid);
    EfiNamedEventListen (BootOptionEventGuid, TPL_CALLBACK, IpmiBootOptionCallback, NULL, NULL);
  }
  

}


/**
 Entrypoint of this module.

 This function is the entry point of this module.
 This driver do below things:
 1. Create SMBIOs Type 38 Record
 2. Create SPMI ACPI Table
 3. Update BMC status for ASL code
 4. Provide event handler to stop BMC watchdog
 5. Handle IPMI boot option request

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Module is loaded successfully.
 @retval EFI_UNSUPPORTED                Cannot locate H2O_IPMI_INTERFACE_PROTOCOL.
*/
EFI_STATUS
EFIAPI
IpmiMiscDxeEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_EVENT                             ReadyToBootEvent;
  UINTN                                 GuidCount;
  EFI_GUID                              *GuidListPtr;
  UINT8                                 Index;


  //
  // If we cannot locate H2OIpmiInterfaceProtocol, it means there is something
  // wrong with BMC. Only update status for OS.
  //
  Status = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&mIpmi);
  if (EFI_ERROR (Status) || H2O_IPMI_VERSION_DEFAULT == mIpmi->GetIpmiVersion (mIpmi)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Give OEM or Platform a chance to update policy again
  //
  Status = OemIpmiUpdatePolicy ();
  if (Status == EFI_UNSUPPORTED) {
    IpmiUpdatePolicy ();
  }

  //
  // If we can run here, it means BMC is OK.
  //
  Status = OemIpmiUpdateNvsVariable (mIpmi);
  if (Status == EFI_UNSUPPORTED) {
    IpmiUpdateNvsVariable (mIpmi);
  }

  //
  // Create SMBIOS Type 38 Record
  // This is necessary so we don't have switch to turn off
  //
  CreateSmbiosType38 ();

  //
  // Create SPMI ACPI table based on policy setting
  //
  if (PcdGet8 (PcdIpmiSpmiEnable) == H2O_IPMI_FUNCTION_ENABLE) {
    CreateSpmiTable ();
  }

  //
  // Set BIOS version string to BMC based on policy setting
  //
  if (PcdGet8 (PcdIpmiSetBiosVersion) == H2O_IPMI_FUNCTION_ENABLE) {
    SetBiosVersion ();
  }

  //
  // Create an event handler for stop watchdog based on policy setting
  //
  if (PcdGet8 (PcdBmcWdtEnable)) {

    //
    // Get "Start" Watchdog Timer event GUID list & creates named events listen for event GUID list.
    //
    GuidCount = (FixedPcdGetPtrSize (PcdIpmiWdtStartEventGuidList)) / (sizeof (EFI_GUID));
    GuidListPtr = (EFI_GUID*)PcdGetPtr (PcdIpmiWdtStartEventGuidList);
    for (Index = 0; Index < GuidCount; Index++) {
      EfiNamedEventListen ((GuidListPtr + Index), TPL_CALLBACK, StartBmcWatchdogTimerCallback, NULL, NULL);
    }

    //
    // Get "Stop" Watchdog Timer event GUID list & creates named events listen for event GUID list.
    //
    GuidCount = (FixedPcdGetPtrSize (PcdIpmiWdtStopEventGuidList)) / (sizeof (EFI_GUID));
    GuidListPtr = (EFI_GUID*)PcdGetPtr (PcdIpmiWdtStopEventGuidList);
    for (Index = 0; Index < GuidCount; Index++) {
      EfiNamedEventListen ((GuidListPtr + Index), TPL_CALLBACK, StopBmcWatchdogTimerCallback, NULL, NULL);
    }

    //
    // Create event to "Stop" Watchdog Timer when gEfiEventReadyToBootGuid be installed.
    //
    EfiCreateEventReadyToBootEx (
      TPL_CALLBACK,
      StopBmcWatchdogTimerCallback,
      NULL,
      &ReadyToBootEvent
      );

  }

  //
  // Process IPMI boot option request based on policy setting
  //
  if (PcdGet8 (PcdIpmiBootOption)) {
    ProcessIpmiBootOption ();
  }

  //
  // Reserved for OEM to implement something additional requirements before IPMI Misc driver end.
  //
  OemIpmiBeforeMiscDriverEnd ();

  return EFI_SUCCESS;

}

