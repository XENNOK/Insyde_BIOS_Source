/** @file
  PlatformBdsLib

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "BootDevicesDisplay.h"
#include <PortNumberMap.h>

STATIC
VOID
ConvertChar16ToChar8 (
  IN CHAR8      *Dest,
  IN CHAR16     *Src
  )
{
  while (*Src) {
    *Dest++ = (UINT8) (*Src++);
  }

  *Dest = 0;
}

EFI_STATUS
UpdateBootMessage (
  )
{
  EFI_LEGACY_REGION_PROTOCOL        *LegacyRegion;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  EFI_COMPATIBILITY16_TABLE         *Table = NULL;
  EFI_IA32_REGISTER_SET             Regs;
  EFI_STATUS                        Status;
  UINT8                             *Ptr;
  BOOT_MESSAGE                      *BootMsg = NULL;
  BOOLEAN                           Find = FALSE;
  CHAR16                            *NewString = NULL;
  UINTN                             StringLen = 0;
  UINT8                             *NewStringAddress;
  
  //
  // See if the Legacy Region Protocol is available
  //
  Status = gBS->LocateProtocol (&gEfiLegacyRegionProtocolGuid, NULL, (VOID **)&LegacyRegion);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Ptr = (UINT8 *) ((UINTN) 0xF0000); 
       Ptr < (UINT8 *) ((UINTN) 0x100000); 
       Ptr += 0x10) {
    //
    // "boot device error string (BDES)"
    //
    if (*(UINT32 *) Ptr == SIGNATURE_32 ('B', 'D', 'E', 'S')) {
      BootMsg = (BOOT_MESSAGE *) Ptr;
      Find = TRUE;
      break;
    }
  }

  if (Find) {
    NewString = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_DEVICE_ERROR_MESSAGE));
//[-start-140623-IB05080432-modify]//
    if (NewString != NULL) {
      StringLen = StrLen (NewString);
    }
//[-end-140623-IB05080432-modify]//
    if (NewString == NULL || StringLen == 0) {
      return EFI_NOT_READY;
    }
    
    Status = LegacyRegion->UnLock (LegacyRegion, 0xF0000, 0x10000, NULL);
    if (StringLen > BOOT_FAIL_STRING_LENTH) {
      for (Ptr = (UINT8 *) ((UINTN) 0xFE000); 
           Ptr < (UINT8 *) ((UINTN) 0x100000); 
           Ptr += 0x10) {
        if (*(UINT32 *) Ptr == SIGNATURE_32 ('I', 'F', 'E', '$')) {
          Table   = (EFI_COMPATIBILITY16_TABLE *) Ptr;
          break;
        }
      }
           
//[-start-140619-IB05080432-add]//
      if (Table == NULL) {
        return EFI_NOT_READY;
      }
//[-end-140619-IB05080432-add]//
           
      Status = LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x10000, NULL);
      ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
      Regs.X.AX = Legacy16GetTableAddress;
      Regs.X.BX = 0x0002;
      Regs.X.CX = (UINT16)StringLen;
      Regs.X.DX = 1;
      LegacyBios->FarCall86 (
                    LegacyBios,
                    Table->Compatibility16CallSegment,
                    Table->Compatibility16CallOffset,
                    &Regs,
                    NULL,
                    0);

      NewStringAddress = (UINT8 *)(UINTN)(Regs.X.DS*16 + Regs.X.BX);

      BootMsg->StrSegment = (UINT16)(((UINTN)NewStringAddress >> 4) & 0xF000);
      BootMsg->StrOffset  = (UINT16)(UINTN)NewStringAddress;

      ZeroMem ((CHAR8 *)NewStringAddress, StringLen);
      ConvertChar16ToChar8 (NewStringAddress, NewString);
      Status = LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x10000, NULL);
    } else {
      ZeroMem (&BootMsg->String, BOOT_FAIL_STRING_LENTH);
      ConvertChar16ToChar8 ((UINT8 *)&BootMsg->String, NewString);
    }
    
    Status = LegacyRegion->Lock (LegacyRegion, 0xF0000, 0x10000, NULL);
  }

  return Status;
}


/**
  Check this SATA port number is whther support

  @param  PortNum       SATA port number

  @retval EFI_SUCCESS        platform supports this SATA port number
  @retval EFI_UNSUPPORTED    platform unsupports this SATA port number

**/
STATIC
EFI_STATUS
CheckSataPort (
  IN UINTN                          PortNum
  )
{
  UINTN                             Index;
  PORT_NUMBER_MAP                   *PortMappingTable;   //retrieved from Pcd
  PORT_NUMBER_MAP                   EndEntry;
  UINTN                             NoPorts;

  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
    return EFI_UNSUPPORTED;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if (PortMappingTable[Index].PortNum == PortNum) {
      return EFI_SUCCESS;
    }
  }
  
  return EFI_UNSUPPORTED;
}


/**
  According Port number and Bbs table to channel number, device number, and
  Bbs Table Index.

  @param  LocalBbsTable   Pointer to BbsTable.
  @param  PortNum         SATA port number
  @param  Channel         Pointer to channel number
  @param  Device          Pointer to device number
  @param  BbsTableIndex   Pointer to Bbs Table index

  @retval EFI_SUCCESS             Get Channel, Device and Bbs table index successful
  @retval EFI_INVALID_PARAMETER   Input parameter is invalid
  @retval EFI_NOT_FOUND           Cannot found associate Channel, device and Bbs table index

**/
STATIC
EFI_STATUS
GetChannelDevice (
  IN     BBS_TABLE           *LocalBbsTable,
  IN     UINTN               PortNum,
     OUT UINT8               *Channel,
     OUT UINT8               *Device,
     OUT UINTN               *BbsTableIndex
  )
{
  PORT_NUMBER_MAP                 *PortMappingTable;
  PORT_NUMBER_MAP                 EndEntry;
  UINTN                           TotalPortCnt;
  UINTN                           Index;
  UINTN                           PortCnt;
  EFI_STATUS                      Status;
  UINT8                           SlaveMaster;
  UINT8                           PrimarySecondary;

  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));
  
  if (BbsTableIndex == NULL || Channel == NULL || Device == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get Port maping table to check this port is supported
  //  
  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  TotalPortCnt = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[TotalPortCnt], sizeof (PORT_NUMBER_MAP)) != 0) {
    TotalPortCnt++;
  }

  if (TotalPortCnt == 0) {
    return EFI_UNSUPPORTED;
  }

  for (PortCnt = 0; PortCnt < TotalPortCnt; PortCnt++) {
    if (PortMappingTable[PortCnt].PortNum == PortNum) {
      break;
    }
  }
  if (PortCnt == TotalPortCnt) {
    return EFI_NOT_FOUND;
  }

  //
  // Get channel number and device number from Bbs table
  //
  Status = EFI_NOT_FOUND;
  for (Index = 1; Index < 9; Index++) {
    if (LocalBbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
      continue;
    }
    //
    // Caculate SlaveMaster and PrimarySecondary from BbsTableIndex
    //
    SlaveMaster = (UINT8) ((Index - 1) % 2);
    PrimarySecondary = (UINT8) ((Index - 1) / 2 % 2);

    if ((LocalBbsTable[Index].Bus == PortMappingTable[PortCnt].Bus) &&
        (LocalBbsTable[Index].Device == PortMappingTable[PortCnt].Device) &&
        (LocalBbsTable[Index].Function == PortMappingTable[PortCnt].Function) &&
        (SlaveMaster == PortMappingTable[PortCnt].SlaveMaster) &&
        (PrimarySecondary == PortMappingTable[PortCnt].PrimarySecondary)) {
      *Channel = (UINT8) ((Index - 1) / 2);
      *Device = (UINT8) ((Index - 1) % 2);
      *BbsTableIndex = Index;
      Status = EFI_SUCCESS;
      break;
    }
  }

  return Status;
}


/**
  Check the specific BBS Table entry is SATA device

  @param  LocalBbsTable     Pointer to BBS table start address
  @param  BbsTableIndex     The specific Bbs table index

  @retval TRUE          It is SATA device
  @retval FALSE         It isn't SATA device

**/
BOOLEAN
IsSataDevice (
  IN BBS_TABLE              *LocalBbsTable,
  IN UINTN                  BbsTableIndex
  )
{
  PORT_NUMBER_MAP       *PortMappingTable;
  PORT_NUMBER_MAP       EndEntry;
  UINTN                 TotalPortCnt;
  UINTN                 Index;

  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  //
  // Bbs table index 1~8 is used for SATA device
  //
  if ((BbsTableIndex > 0) &&  (BbsTableIndex < 9)){
    return TRUE;
  }

  //
  // check the PCI bus device function is matched for SATA controller
  //
  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  TotalPortCnt = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[TotalPortCnt], sizeof (PORT_NUMBER_MAP)) != 0) {
    TotalPortCnt++;
  }

  if (TotalPortCnt == 0) {
    return FALSE;
  }
  
  for (Index = 0; Index < TotalPortCnt; Index++) {
    if ((LocalBbsTable[BbsTableIndex].Bus == PortMappingTable[Index].Bus) &&
        (LocalBbsTable[BbsTableIndex].Device == PortMappingTable[Index].Device) &&
        (LocalBbsTable[BbsTableIndex].Function == PortMappingTable[Index].Function)) {
      return TRUE;
    }
  }
  
  return FALSE;
}


/**
  Check the specific BBS Table entry is USB device

  @param  LocalBbsTable     Pointer to BBS table start address
  @param  BbsTableIndex     The specific Bbs table index

  @retval TRUE              It is USB device
  @retval FALSE             It isn't USB device

**/
BOOLEAN
IsUsbDevice (
  IN BBS_TABLE              *LocalBbsTable,
  IN UINTN                  BbsTableIndex
  )
{
  if ((LocalBbsTable[BbsTableIndex].Class == PCI_CLASS_SERIAL) &&
      (LocalBbsTable[BbsTableIndex].SubClass == PCI_CLASS_SERIAL_USB)) {
    return TRUE;
  }
  
  return FALSE;
}


/**
  Show all of sata devices' information on summary screen

  @param  LocalBbsTable      Pointer to BBS table start address
  @param  LocalHddInfo       Pointer to Hdd info statt address
  @param  HddCount           Hdd count in Hdd info array
  @param  SetupNVRam         Pointer to CHIPSET_CONFIGURATION variable
  @param  ShowedDeviceNum    IN  : Showed device number on summary screen before show SATA device
                             OUT : Showed device number on summary screen after show SATA device

  @retval EFI_SUCCESS             show SATA device successful
  @retval EFI_ABORTED             skip showing device
  @retval EFI_INVALID_PARAMETER   input parameter is invalid

**/
EFI_STATUS
ShowSataDevice (
  IN      BBS_TABLE              *LocalBbsTable,
  IN      HDD_INFO               *LocalHddInfo,
  IN      UINT16                 HddCount,
  IN      CHIPSET_CONFIGURATION   *SetupNVRam,
  IN OUT  UINTN                  *ShowedDeviceNum
  )
{
  UINT8                                 Channel;
  UINT8                                 Device;
  CHAR16                                *SataPortStr;
  CHAR16                                ModelNametring[100];
  CHAR16                                PortNoStr[10];
  UINTN                                 PortNum;
  UINTN                                 DeviceCnt;
  UINTN                                 BbsTableIndex;
  EFI_STATUS                            Status;
//[-start-130626-IB10130054-add]//
  BOOLEAN                               CheckBbsTableIndex[MAX_DISPLAY_DEVICE_COUNT + 1];
//[-end-130626-IB10130054-add]//
  
  //
  // check parameter is valid
  //
  if ((LocalBbsTable == NULL) || (LocalHddInfo == NULL) ||
      (SetupNVRam == NULL) || (ShowedDeviceNum == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // check showed device number is lareger than MAX_DISPLAY_DEVICE_COUNT
  //
  if (*ShowedDeviceNum >= MAX_DISPLAY_DEVICE_COUNT) {
    return EFI_ABORTED;
  }


  BbsTableIndex = 1;
  DeviceCnt = *ShowedDeviceNum;
  SataPortStr = PlatformBdsGetStringById (STRING_TOKEN (STR_SERIAL_ATA_PORT));
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);
  
//[-start-130626-IB10130054-modify]//
  //
  // Init CheckBbsTableIndex
  //
  for (PortNum = 0; PortNum <= 8; PortNum++) {
   CheckBbsTableIndex[PortNum] = FALSE;
  }
  //
  // SATA Configuration
  //
  for (PortNum = 0; PortNum < 8; PortNum++) {
    //
    // IDE mode or AHCI mode without option Rom
    //
    //if ((SetupNVRam->SataCnfigure == IDE_MODE) ||
     //  ((SetupNVRam->SataCnfigure == AHCI_MODE) && (SetupNVRam->AhciOptionRomSupport == 0))) {
      //
      // Check this port is whether support
      //
      //Status = CheckSataPort (PortNum);
      //if (EFI_ERROR (Status)) {
        //continue;
      //}
      //
      // According port number to get channel, device and BbsTableIndex
      //
      Status = GetChannelDevice (LocalBbsTable, PortNum, &Channel, &Device, &BbsTableIndex);
      if (EFI_ERROR (Status)) {
        //continue;
      //}
    //
    // AHIC mode with option ROM or RAID mode
    //
    //} else {
      Channel = (UINT8) (PortNum / 2);
      Device = (UINT8) (PortNum % 2);
      BbsTableIndex = PortNum + 1;
    }
    if (CheckBbsTableIndex[BbsTableIndex]) {
      continue;
    }
    CheckBbsTableIndex[BbsTableIndex] = TRUE;
//[-end-130626-IB10130054-modify]//
    //
    // Show SATA device
    //
    if (HddCount > (Channel * 2 + Device)) {
      //
      // only can show MAX_DISPLAY_DEVICE_COUNT devices to prevent covering other information
      //
      if (DeviceCnt > MAX_DISPLAY_DEVICE_COUNT) {
        BdsLibOutputStrings (gST->ConOut, L"\n\r", L"...", NULL);
        break;
      }
      BdsBuildLegacyDevNameString (
        &LocalBbsTable[BbsTableIndex],
        LocalHddInfo,
        BbsTableIndex,
        sizeof (ModelNametring),
        ModelNametring
        );
      if (StrLen (ModelNametring) == 0) {
        continue;
      }
      UnicodeValueToString (PortNoStr, 0, PortNum, 0);
      BdsLibOutputStrings (gST->ConOut, L"\n\r", SataPortStr, PortNoStr, L" : ", ModelNametring, NULL);
      DeviceCnt++;
    }
  }

  //
  // Set showed Device number
  //
  *ShowedDeviceNum = DeviceCnt;
  gBS->FreePool (SataPortStr);

  return EFI_SUCCESS;
}


/**
  show all of un-SATA device information on summary screen

  @param  LocalBbsTable      Pointer to BBS table start address
  @param  BbsTotalCount      Total BBS count in BBS table
  @param  LocalHddInfo       Pointer to Hdd info statt address
  @param  ShowedDeviceNum    IN  : Showed device number on summary screen before show SATA device
                             OUT : Showed device number on summary screen after show SATA device

  @retval EFI_SUCCESS             show all of other device successful
  @retval EFI_ABORTED             skip showing device
  @retval EFI_INVALID_PARAMETER   input parameter is invalid

**/
EFI_STATUS
ShowUnSataDevice (
  IN      BBS_TABLE        *LocalBbsTable,
  IN      UINT16           BbsTotalCount,
  IN      HDD_INFO         *LocalHddInfo,
  IN OUT  UINTN            *ShowedDeviceNum
  )
{
  CHAR16          *UsbDeviceStr;
  CHAR16          *OtherDeviceStr;
  CHAR16          ModelNametring[100];
  CHAR16          DeviceNoStr[10];
  UINTN           BbsTableIndex;
  UINTN           UsbDeviceNum;
  UINTN           OtherDeviceNum;
  UINTN           TotalDeviceNum;

  //
  // check parameter is valid
  //
  if ((LocalBbsTable == NULL) || (LocalHddInfo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // check showed device number is lareger than MAX_DISPLAY_DEVICE_COUNT
  //
  if (*ShowedDeviceNum >= MAX_DISPLAY_DEVICE_COUNT) {
    return EFI_ABORTED;
  }
  //
  // Initial local variable
  //
  UsbDeviceStr = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_USB_DEVICE));
  UsbDeviceNum = 0;
  TotalDeviceNum = *ShowedDeviceNum;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE);

  //
  // Show USB device
  //
  for (BbsTableIndex = 0; BbsTableIndex < BbsTotalCount; BbsTableIndex++) {
    if ((LocalBbsTable[BbsTableIndex].BootPriority == BBS_IGNORE_ENTRY) ||
        (LocalBbsTable[BbsTableIndex].BootPriority == BBS_DO_NOT_BOOT_FROM)) {
      continue;
    }
    //
    // skip SATA device
    //
    if (IsSataDevice (LocalBbsTable, BbsTableIndex)) {
      continue;
    }

    //
    // Show USB device
    //
    if (IsUsbDevice (LocalBbsTable, BbsTableIndex)) {
      //
      // only can show MAX_DISPLAY_DEVICE_COUNT devices to prevent cover other information
      //
      if (TotalDeviceNum >= MAX_DISPLAY_DEVICE_COUNT) {
        BdsLibOutputStrings (gST->ConOut, L"\n\r", L"...", NULL);
        break;
      }
      UsbDeviceNum++;
      TotalDeviceNum++;
      BdsBuildLegacyDevNameString (
        &LocalBbsTable[BbsTableIndex],
        LocalHddInfo, BbsTableIndex,
        sizeof (ModelNametring),
        ModelNametring
        );
      UnicodeValueToString (DeviceNoStr, 0, UsbDeviceNum, 0);
      BdsLibOutputStrings (gST->ConOut, L"\n\r", UsbDeviceStr, DeviceNoStr, L" : ", ModelNametring, NULL);
    }
  }

  gBS->FreePool (UsbDeviceStr);

  //
  // check showed device number is lareger than MAX_DISPLAY_DEVICE_COUNT
  //
  if (TotalDeviceNum >= MAX_DISPLAY_DEVICE_COUNT) {
    *ShowedDeviceNum = TotalDeviceNum;
    return EFI_ABORTED;
  }

  //
  // Initial local variable for other device use
  //
  OtherDeviceNum = 0;
  OtherDeviceStr = PlatformBdsGetStringById (STRING_TOKEN (STR_BOOT_OTHER_DEVICE));
  //
  // Show other device
  //
  for (BbsTableIndex = 0; BbsTableIndex < BbsTotalCount; BbsTableIndex++) {
    if ((LocalBbsTable[BbsTableIndex].BootPriority == BBS_IGNORE_ENTRY) ||
        (LocalBbsTable[BbsTableIndex].BootPriority == BBS_DO_NOT_BOOT_FROM)) {
      continue;
    }
    //
    // skip SATA device
    //
    if (IsSataDevice (LocalBbsTable, BbsTableIndex)) {
      continue;
    }
    //
    // Show other device
    //
    if (!IsUsbDevice (LocalBbsTable, BbsTableIndex)) {
      //
      // only can show MAX_DISPLAY_DEVICE_COUNT devices to prevent cover other information
      //
      if (TotalDeviceNum >= MAX_DISPLAY_DEVICE_COUNT) {
        BdsLibOutputStrings (gST->ConOut, L"\n\r", L"...", NULL);
        break;
      }
      OtherDeviceNum++;
      TotalDeviceNum++;
      BdsBuildLegacyDevNameString (
        &LocalBbsTable[BbsTableIndex],
        LocalHddInfo, BbsTableIndex,
        sizeof (ModelNametring),
        ModelNametring
        );
      UnicodeValueToString (DeviceNoStr, 0, OtherDeviceNum, 0);
      BdsLibOutputStrings (gST->ConOut, L"\n\r", OtherDeviceStr, DeviceNoStr, L" : ", ModelNametring, NULL);
    }
  }
  //
  // Set showed Device number
  //
  *ShowedDeviceNum = TotalDeviceNum;
  gBS->FreePool (OtherDeviceStr);

  return EFI_SUCCESS;
}


/**
  Show all of attached devices information on summary screen

  @retval EFI_SUCCESS   Show all devices on summary screen successful.

**/
EFI_STATUS
ShowAllDevice (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT16                                BbsTotalCount;
  BBS_TABLE                             *LocalBbsTable;
  HDD_INFO                              *LocalHddInfo;
  UINT16                                HddCount;
  UINTN                                 ShowedDeviceNum;
  EFI_LEGACY_BIOS_PROTOCOL              *LegacyBios;
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;
  CHIPSET_CONFIGURATION                  *SetupNVRam;
  
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &LocalHddInfo,
                         &BbsTotalCount,
                         &LocalBbsTable
                         );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Locate gEfiSetupUtilityProtocolGuid to get SetupNvData
  //
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SetupNVRam = (CHIPSET_CONFIGURATION *) (SetupUtility->SetupNvData);

  BdsLibOutputStrings (gST->ConOut, L"\n\r", NULL);
  ShowedDeviceNum = 0;

  //
  // Show SATA devices
  //
  ShowSataDevice (LocalBbsTable, LocalHddInfo, HddCount, SetupNVRam, &ShowedDeviceNum);

  //
  // Show other devices
  //
  ShowUnSataDevice (LocalBbsTable, BbsTotalCount, LocalHddInfo, &ShowedDeviceNum);

  return EFI_SUCCESS;
}

