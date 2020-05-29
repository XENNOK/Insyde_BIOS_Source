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

/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  2005 - 2010 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  AsfSupport.c

Abstract:

  Support routines for ASF boot options in the BDS

--*/
//[-start-120625-IB03530375-modify]//
#include <AsfSupport.h>

#include <OEMEfiStatusCode.h>
#include <Library/Amt/include/Amt.h>
#include <Library/Amt/Dxe/AmtLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/LoadedImage.h>

#include <Protocol/IderControllerDriver.h>

VOID
WriteBootToOsPerformanceData (
  VOID
  );

#pragma pack(push, 1)
typedef struct {
  UINT32  Attributes;
  UINT16  FilePathListLength;
} EFI_LOAD_OPTION;
#pragma pack(pop)

//
// Global variables
//
EFI_ASF_BOOT_OPTIONS            *mAsfBootOptions;

static EFI_DEVICE_PATH_PROTOCOL EndDevicePath[] = {
  END_DEVICE_PATH_TYPE,
  END_ENTIRE_DEVICE_PATH_SUBTYPE,
  END_DEVICE_PATH_LENGTH,
  0
};

//
// Legacy Device Order
//
typedef struct {
  UINT32  Type;
  UINT16  Length;
  UINT16  Device[1];
} LEGACY_DEVICE_ORDER;

#define LEGACY_DEV_ORDER_GUID \
  { \
    0xA56074DB, 0x65FE, 0x45F7, 0xBD, 0x21, 0x2D, 0x2B, 0xDD, 0x8E, 0x96, 0x52 \
  }

EFI_GUID  gLegacyDeviceOrderGuid = LEGACY_DEV_ORDER_GUID;

/**
  Retrieve the ASF boot options previously recorded by the ASF driver.
  Initialize Boot Options global variable and AMT protocol

  @retval EFI_STATUS    

**/
EFI_STATUS
BdsAsfInitialization (
  IN  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL  *Asf;

  mAsfBootOptions = NULL;

  //
  // Amt Library Init
  //
  Status = AmtLibInit ();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Info : Error init AmtLibInit -> %r\n", Status));
    return Status;
  }
  //
  // Get Protocol for ASF
  //
  Status = gBS->LocateProtocol (
                  &gEfiAlertStandardFormatProtocolGuid,
                  NULL,
                  (VOID **)&Asf
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Info : Error gettings ASF protocol -> %r\n", Status));
    return Status;
  }

  Status = Asf->GetBootOptions (Asf, &mAsfBootOptions);

  return Status;
}


/**
  This function will create a BootOption from the give device path and 
  description string.

  @param  DevicePath    The device path which the option represent
  @param  Description   The description of the boot option

  @return BDS_COMMON_OPTION   A BDS_COMMON_OPTION pointer

**/
BDS_COMMON_OPTION *
BdsCreateBootOption (
  IN  EFI_DEVICE_PATH_PROTOCOL       *DevicePath,
  IN  CHAR16                         *Description
  )
{
  BDS_COMMON_OPTION *Option;

  Option = AllocateZeroPool (sizeof (BDS_COMMON_OPTION));
  if (Option == NULL) {
    return NULL;
  }

  Option->Signature   = BDS_LOAD_OPTION_SIGNATURE;
  Option->DevicePath  = AllocateZeroPool (GetDevicePathSize (DevicePath));
  CopyMem (Option->DevicePath, DevicePath, GetDevicePathSize (DevicePath));

  Option->Attribute   = LOAD_OPTION_ACTIVE;
  Option->Description = AllocateZeroPool (StrSize (Description));
  CopyMem (Option->Description, Description, StrSize (Description));

  return Option;
}


/**
  This function will create a SHELL BootOption to boot.

  @return Shell Device path for booting.

**/
EFI_DEVICE_PATH_PROTOCOL *
BdsCreateShellDevicePath (
  VOID
  )
{
  UINTN                             FvHandleCount;
  EFI_HANDLE                        *FvHandleBuffer;
  UINTN                             Index;
  EFI_STATUS                        Status;
  EFI_FIRMWARE_VOLUME2_PROTOCOL     *Fv;
  EFI_FV_FILETYPE                   Type;
  UINTN                             Size;
  EFI_FV_FILE_ATTRIBUTES            Attributes;
  UINT32                            AuthenticationStatus;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH ShellNode;

  DevicePath  = NULL;
  Status      = EFI_SUCCESS;

  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiFirmwareVolumeProtocolGuid,
        NULL,
        &FvHandleCount,
        &FvHandleBuffer
        );

  for (Index = 0; Index < FvHandleCount; Index++) {
    gBS->HandleProtocol (
          FvHandleBuffer[Index],
          &gEfiFirmwareVolume2ProtocolGuid,
          (VOID **) &Fv
          );

    Status = Fv->ReadFile (
                  Fv,
                  PcdGetPtr(PcdShellFile),
                  NULL,
                  &Size,
                  &Type,
                  &Attributes,
                  &AuthenticationStatus
                  );
    if (EFI_ERROR (Status)) {
      //
      // Skip if no shell file in the FV
      //
      continue;
    } else {
      //
      // Found the shell
      //
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // No shell present
    //
    if (FvHandleCount) {
      gBS->FreePool (FvHandleBuffer);
    }

    return NULL;
  }
  //
  // Build the shell boot option
  //
  DevicePath = DevicePathFromHandle (FvHandleBuffer[Index]);

  //
  // Build the shell device path
  //
  EfiInitializeFwVolDevicepathNode (&ShellNode, PcdGetPtr(PcdShellFile));
  DevicePath = AppendDevicePathNode (DevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &ShellNode);

  if (FvHandleCount) {
    gBS->FreePool (FvHandleBuffer);
  }

  return DevicePath;
}


/**
  This function will create a PXE BootOption to boot.

  @param  DeviceIndex   PXE handle index

  @return PXE Device path for booting.

**/
static
EFI_DEVICE_PATH_PROTOCOL *
BdsCreatePxeDevicePath (
  IN UINT16     DeviceIndex
  )
{
  UINTN                     Index;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     NumberLoadFileHandles;
  EFI_HANDLE                *LoadFileHandles;
  VOID                      *ProtocolInstance;

  DevicePath  = NULL;
  Status      = EFI_SUCCESS;

  //
  // We want everything connected up for PXE
  //
  BdsLibConnectAllDriversToAllControllers ();

  //
  // Parse Network Boot Device
  //
  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiSimpleNetworkProtocolGuid,
        NULL,
        &NumberLoadFileHandles,
        &LoadFileHandles
        );
  for (Index = 0; Index < NumberLoadFileHandles; Index++) {
    Status = gBS->HandleProtocol (
                    LoadFileHandles[Index],
                    &gEfiLoadFileProtocolGuid,
                    (VOID **) &ProtocolInstance
                    );
    if (EFI_ERROR (Status)) {
      //
      // try next handle
      //
      continue;
    } else {
      if (Index == DeviceIndex) {
        //
        // Found a PXE handle
        //
        break;
      } else {
        Status = EFI_UNSUPPORTED;
      }
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // No PXE present
    //
    if (NumberLoadFileHandles) {
      gBS->FreePool (LoadFileHandles);
    }

    return NULL;
  }
  //
  // Build the PXE device path
  //
  DevicePath = DevicePathFromHandle (LoadFileHandles[Index]);

  if (NumberLoadFileHandles) {
    gBS->FreePool (LoadFileHandles);
  }

  return DevicePath;
}


/**
  This function will get Bus, Device and Function.

  @param  Bus           AMT Bus
  @param  Device        AMT Device
  @param  Function      AMT Function

**/
static
VOID
GetAmtBusDevFcnVal (
  OUT  UINT32   *Bus,
  OUT  UINT32   *Device,
  OUT  UINT32   *Function
  )
{
  UINT32                          Index;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo;
  UINT64                          Address;
  DATA32_UNION                    Data32Union;
  EFI_STATUS                      Status;

  *Bus      = 0;
  *Device   = 0;
  *Function = 0;

  //
  // Locate root bridge IO protocol
  //
  Status = gBS->LocateProtocol (&gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **)&PciRootBridgeIo);
  ASSERT_EFI_ERROR (Status);

  //
  // Need to fill in IDER bus dev function so find this for Tekoa i82573E here.
  //
  for (Index = 0; Index < 0x255; Index++) {
    Address = EFI_PCI_ADDRESS (
                Index,
                ME_DEVICE_NUMBER,     //
                IDER_FUNCTION_NUMBER, // fun 2 IDER capability
                PCI_VENDOR_ID_OFFSET
                );
    Status = PciRootBridgeIo->Pci.Read (
                                    PciRootBridgeIo,
                                    EfiPciWidthUint32,
                                    Address,
                                    1,
                                    &Data32Union
                                    );
    if ((Data32Union.Data16[0] == V_ME_IDER_VENDOR_ID) && 
      (Data32Union.Data16[1] == V_ME_IDER_DEVICE_ID)) {
      //
      // LOM i82573E is always Device 0 and function 2 so or this 8 bit value of 0x02
      // into bus number discovered.  Bus # upper byte and Dev-Fcn lower byte.
      //
      *Bus      = Index;
      *Device   = ME_DEVICE_NUMBER;
      *Function = IDER_FUNCTION_NUMBER;
      break;
    }
  }
}


/**
  Boot HDD by BIOS Default Priority

  @param  DevType       Boot device whose device type

  @return

**/
UINTN
GetFirstIndexByType (
  UINT16 DevType
  )
{
  EFI_STATUS          Status;
  UINTN               LegacyDevOrderSize;
  LEGACY_DEVICE_ORDER *LegacyDevOrder;
  UINTN               Index;
  UINT8               *p;
  UINTN               o;

  Index               = 0;
  o                   = 0;

  LegacyDevOrderSize  = 0;
  LegacyDevOrder      = NULL;

  Status = gRT->GetVariable (
                  L"LegacyDevOrder",
                  &gLegacyDeviceOrderGuid,
                  NULL,
                  &LegacyDevOrderSize,
                  NULL
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    LegacyDevOrder = AllocateZeroPool (LegacyDevOrderSize);
    if (LegacyDevOrder != NULL) {
      Status = gRT->GetVariable (
                      L"LegacyDevOrder",
                      &gLegacyDeviceOrderGuid,
                      NULL,
                      &LegacyDevOrderSize,
                      LegacyDevOrder
                      );
      if (!EFI_ERROR (Status)) {
        p = (UINT8 *) LegacyDevOrder;
        o = 0;
        for (o = 0; o < LegacyDevOrderSize; o += sizeof (LegacyDevOrder->Type) + LegacyDevOrder->Length) {
          LegacyDevOrder = (LEGACY_DEVICE_ORDER *) (p + o);
          if (LegacyDevOrder->Type == DevType) {
            Index = LegacyDevOrder->Device[0];
          }
        }
      }
    }
  }

  return Index;
}


/**
  Update the table with our required boot device

  @param  DeviceIndex     Boot device whose device index
  @param  DevType         Boot device whose device type
  @param  BbsCount        Number of BBS_TABLE structures
  @param  BbsTable        BBS entry

  @retval EFI_STATUS

**/
static
EFI_STATUS
RefreshBbsTableForBoot (
  IN     UINT16        DeviceIndex,
  IN     UINT16        DevType,
  IN     UINT16        BbsCount,
  IN OUT BBS_TABLE     *BbsTable
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT16      BootDeviceIndex;
  UINT16      TempIndex;

  Status          = EFI_NOT_FOUND;
  TempIndex       = 1;

  BootDeviceIndex = DeviceIndex;

  //
  // Find the first present boot device whose device type
  // matches the DevType, we use it to boot first. This is different
  // from the other Bbs table refresh since we are looking for the device type
  // index instead of the first device to match the device type.
  //
  // And set other present boot devices' priority to BBS_UNPRIORITIZED_ENTRY
  // their priority will be set by LegacyBiosPlatform protocol by default
  //
  if (DeviceIndex > 0) {
    for (Index = 0; Index < BbsCount; Index++) {
      if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
        continue;
      }

      BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;

      if (BbsTable[Index].DeviceType == DevType) {
        if (TempIndex++ == DeviceIndex) {
          if (BbsTable[Index].BootPriority != 0) {
            ReportStatusCode (
            EFI_PROGRESS_CODE,
            EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BOOT_OPTION_CHANGE
            );          
          }              
          if ((DevType == BBS_BEV_DEVICE) && !((BbsTable[Index].Class == 0x2) && (BbsTable[Index].SubClass == 0x0))) {
            //
            // For FORCE_PXE, check Class and SubClass to ensure the device is LAN.
            //
            continue;
          }
          BbsTable[Index].BootPriority  = 0;
          Status                        = EFI_SUCCESS;
          continue;
        }
      }
    }
  } else {
    //
    // Boot HDD by BIOS Default Priority
    //
    Index                         = GetFirstIndexByType (DevType);
    BbsTable[Index].BootPriority  = 0;
    Status                        = EFI_SUCCESS;
  }

  return Status;
}


/**
  Update the table with IdeR boot device

  @param  DeviceIndex     Boot device whose device index
  @param  DevType         Boot device whose device type
  @param  BbsCount        Number of BBS_TABLE structures
  @param  BbsTable        BBS entry

  @retval EFI_STATUS

**/
static
EFI_STATUS
RefreshBbsTableForIdeRBoot (
  IN     UINT16        DeviceIndex,
  IN     UINT16        DevType,
  IN     UINT16        BbsCount,
  IN OUT BBS_TABLE     *BbsTable
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT32      Bus;
  UINT32      Device;
  UINT32      Function;
  BOOLEAN     IderDeviceFound;

  Status          = EFI_NOT_FOUND;

  IderDeviceFound = FALSE;

  GetAmtBusDevFcnVal (&Bus, &Device, &Function);

  //
  // Find the first present boot device whose device type
  // matches the DevType, we use it to boot first.
  //
  // And set other present boot devices' priority to BBS_DO_NOT_BOOT_FROM
  // their priority will be set by LegacyBiosPlatform protocol by default
  //
  for (Index = 0; Index < BbsCount; Index++) {
    if (BBS_IGNORE_ENTRY == BbsTable[Index].BootPriority) {
      continue;
    }

    BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;

    if ((BbsTable[Index].Bus == Bus) && (BbsTable[Index].Device == Device) && (BbsTable[Index].Function == Function)) {
      if (DeviceIndex == 0 && IderDeviceFound != TRUE) {
        BbsTable[Index].BootPriority  = 0;
        IderDeviceFound               = TRUE;
        Status                        = EFI_SUCCESS;
      } else {
        DeviceIndex--;
      }

      continue;
    }
  }

  return Status;
}


/**
  Build the BBS Device Path for this boot selection

  @param  DevType         Boot device whose device type
  @param  DeviceIndex     Boot device whose device index
  @param  IdeRBoot        If IdeRBoot is TRUE then check Ider device

  @retval EFI_STATUS

**/
static
EFI_DEVICE_PATH_PROTOCOL *
BdsCreateLegacyBootDevicePath (
  IN UINT16     DeviceType,
  IN UINT16     DeviceIndex,
  IN BOOLEAN    IdeRBoot
  )
{
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  BBS_BBS_DEVICE_PATH       BbsDevicePathNode;
  BBS_TABLE                 *BbsTable;
  UINT16                    HddCount;
  UINT16                    BbsCount;
  HDD_INFO                  *LocalHddInfo;
  UINT16                    Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_STATUS                Status;

  HddCount      = 0;
  BbsCount      = 0;
  LocalHddInfo  = NULL;

  Index         = DeviceIndex;

  //
  // Make sure the Legacy Boot Protocol is available
  //
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  //
  // Get BBS table instance
  //
  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &LocalHddInfo,
                         &BbsCount,
                         &BbsTable
                         );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  //
  // For debug
  //
  PrintBbsTable (BbsTable);

  //
  // Update the table with our required boot device
  //
  if (IdeRBoot) {
    Status = RefreshBbsTableForIdeRBoot (
              Index,
              DeviceType,
              BbsCount,
              BbsTable
              );
  } else {
    Status = RefreshBbsTableForBoot (
              Index,
              DeviceType,
              BbsCount,
              BbsTable
              );
  }
  //
  // For debug
  //
  PrintBbsTable (BbsTable);

  if (EFI_ERROR (Status)) {
    //
    // Device not found - do normal boot
    //
    return NULL;
  }
  //
  // Build the BBS Device Path for this boot selection
  //
  DevicePath = AllocateZeroPool (sizeof (EFI_DEVICE_PATH_PROTOCOL));
  if (DevicePath == NULL) {
    return NULL;
  }

  BbsDevicePathNode.Header.Type     = BBS_DEVICE_PATH;
  BbsDevicePathNode.Header.SubType  = BBS_BBS_DP;
  SetDevicePathNodeLength (&BbsDevicePathNode.Header, sizeof (BBS_BBS_DEVICE_PATH));
  BbsDevicePathNode.DeviceType  = DeviceType;
  BbsDevicePathNode.StatusFlag  = 0;
  BbsDevicePathNode.String[0]   = 0;

  DevicePath = AppendDevicePathNode (
                EndDevicePath,
                (EFI_DEVICE_PATH_PROTOCOL *) &BbsDevicePathNode
                );
  if (NULL == DevicePath) {
    return NULL;
  }

  return DevicePath;
}


/**
  Boot the legacy system with the boot option

  @param  Option        The legacy boot option which have BBS device path

  @retval EFI_UNSUPPORTED   There is no legacybios protocol, do not support legacy boot.
  @retval Other             Return the status of LegacyBios->LegacyBoot ().

**/
EFI_STATUS
AsfDoLegacyBoot (
  IN  BDS_COMMON_OPTION           *Option
  )
{
  EFI_STATUS                Status;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&LegacyBios);
  if (EFI_ERROR (Status)) {
    //
    // If no LegacyBios protocol we do not support legacy boot
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Write boot to OS performance data to a file
  //
  WRITE_BOOT_TO_OS_PERFORMANCE_DATA;

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Legacy Boot: %S\n", Option->Description));
  return LegacyBios->LegacyBoot (
                      LegacyBios,
                      (BBS_BBS_DEVICE_PATH *) Option->DevicePath,
                      Option->LoadOptionsSize,
                      Option->LoadOptions
                      );
}


/**
  Build Device Path via BootOrder variable for this boot selection

  @param  DevType         Boot device whose device type
  @param  DeviceIndex     Boot device whose device index
  @param  IdeRBoot        If IdeRBoot is TRUE then check Ider device

  @retval EFI_STATUS

**/
static
EFI_DEVICE_PATH_PROTOCOL *
BdsCreateUefiBootDevicePath (
  IN UINT16     DeviceType,
  IN UINT16     DeviceIndex,
  IN BOOLEAN    IdeRBoot
  )
{
  UINT16                    *BootOrder;
  UINT8                     *BootOptionVar;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  EFI_STATUS                Status;
  UINTN                     Index;
  UINT16                    BootOption[BOOT_OPTION_MAX_CHAR];
  UINTN                     OptionDevicePathSize;
  UINTN                     DevicePathType;  
  EFI_DEVICE_PATH_PROTOCOL  *OptionDevicePath;
  UINT8                     *TempPtr;
  CHAR16                    *Description;

  Status           = EFI_SUCCESS;
  BootOrder        = NULL;
  OptionDevicePath = NULL;
  BootOrderSize    = 0;

  BootOrder = BdsLibGetVariableAndSize (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                &BootOrderSize
                );
  if (BootOrder == NULL) {
    return OptionDevicePath;
  }

  Index = 0;
  while (Index < BootOrderSize / sizeof (UINT16)) {
    UnicodeSPrint (BootOption, sizeof (BootOption), L"Boot%04x", BootOrder[Index]);
    BootOptionVar = BdsLibGetVariableAndSize (
                      BootOption,
                      &gEfiGlobalVariableGuid,
                      &BootOptionSize
                      );
    if (NULL == BootOptionVar) {
      gBS->FreePool (BootOrder);
      return OptionDevicePath;
    }

    TempPtr = BootOptionVar;
    TempPtr += sizeof (UINT32) + sizeof (UINT16);
    Description = (CHAR16 *) TempPtr;
    TempPtr += StrSize ((CHAR16 *) TempPtr);
    OptionDevicePath = (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
    OptionDevicePathSize = GetDevicePathSize (OptionDevicePath);

    DevicePathType = BdsLibGetBootTypeFromDevicePath (OptionDevicePath);    

    DEBUG ((EFI_D_ERROR , "Bootable Device %d  -  Type: %d    SubType: %d    DevicePathType: %x\n", BootOrder[Index], OptionDevicePath->Type, OptionDevicePath->SubType, DevicePathType));    
    switch (DeviceType) {

    case FORCE_HARDDRIVE:      
    case FORCE_SAFEMODE:
      if (DevicePathType == BDS_EFI_MEDIA_HD_BOOT) {
        return OptionDevicePath;
      }  
      break;
  
    case FORCE_DIAGNOSTICS:
      if (DevicePathType == BDS_EFI_MESSAGE_USB_DEVICE_BOOT) {
        return OptionDevicePath;
      }  
      break;
  
    case FORCE_CDDVD:
      if (DevicePathType == BDS_EFI_MESSAGE_SATA_BOOT || DevicePathType == BDS_EFI_MESSAGE_ATAPI_BOOT || DevicePathType == BDS_EFI_MEDIA_CDROM_BOOT) {
        return OptionDevicePath;
      }
      break;
    }
    

    gBS->FreePool (BootOptionVar);
    Index++;
  }

  //
  // No any device is found, return NULL
  //
  return NULL;
}


/**
  Compare Device Patch

  @param  dp1         Device Patch 1
  @param  dp2         Device Patch 2

  @retval TRUE        Device path is the same
  @retval FALSE       Device path is not the same

**/
BOOLEAN
CompareDP (
  EFI_DEVICE_PATH_PROTOCOL *dp1,
  EFI_DEVICE_PATH_PROTOCOL *dp2
  )
{
  UINTN   s1;
  UINTN   s2;
  BOOLEAN st;

  st  = FALSE;
  s1  = GetDevicePathSize (dp1);
  s2  = GetDevicePathSize (dp2);

  if (s1 == s2) {
    if (CompareMem (dp1, dp2, s1) == 0) {
      st = TRUE;
    }
  }

  return st;
}


/**
  Get the currect boot device

  @param  DevicePath    The currect boot device path

  @return The currect boot device

**/
UINT16
GetBootCurrent (
  IN  EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  UINT16                    *BootOrder;
  EFI_LOAD_OPTION           *BootOption;
  UINTN                     BootOrderSize;
  UINTN                     BootOptionSize;
  CHAR16                    BootVarName[9];
  UINTN                     i;
  EFI_STATUS                Status;
  BOOLEAN                   st;
  UINT16                    BootCurrent;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;

  BootOrder     = NULL;
  BootOption    = NULL;
  BootCurrent   = 0;
  BootOrderSize = 0;

  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BootOrderSize,
                  NULL
                  );

  if (Status == EFI_BUFFER_TOO_SMALL) {
    BootOrder = AllocateZeroPool (BootOrderSize);
    if (BootOrder != NULL) {
      Status = gRT->GetVariable (
                      L"BootOrder",
                      &gEfiGlobalVariableGuid,
                      NULL,
                      &BootOrderSize,
                      BootOrder
                      );

      if (!EFI_ERROR (Status)) {
        for (i = 0; i < BootOrderSize / sizeof (UINT16); i++) {
          UnicodeSPrint (BootVarName, sizeof (BootVarName), L"Boot%04d", BootOrder[i]);
          BootOptionSize = 0;
          Status = gRT->GetVariable (
                          BootVarName,
                          &gEfiGlobalVariableGuid,
                          NULL,
                          &BootOptionSize,
                          NULL
                          );
          if (Status == EFI_BUFFER_TOO_SMALL) {
            BootOption = AllocateZeroPool (BootOptionSize);
            if (BootOption != NULL) {
              Status = gRT->GetVariable (
                              BootVarName,
                              &gEfiGlobalVariableGuid,
                              NULL,
                              &BootOptionSize,
                              BootOption
                              );
              if (!EFI_ERROR (Status)) {
                Dp = (EFI_DEVICE_PATH_PROTOCOL *) (
                //
                // skip the header
                //
                (UINT8 *) (BootOption + 1)
                //
                // skip the string
                //
                + (StrLen ((CHAR16 *) (BootOption + 1)) + 1) * sizeof (CHAR16));
                st = CompareDP (Dp, DevicePath);
                if (st == TRUE) {
                  BootCurrent = BootOrder[i];
                }
              }

              gBS->FreePool (BootOption);
            }
          }
        }
      }

      gBS->FreePool (BootOrder);
    }
  }

  return BootCurrent;
}


/**
  Process the boot option follow the EFI 1.1 specification and 
  special treat the legacy boot option with BBS_DEVICE_PATH.

  @param  Option          The boot option need to be processed
  @param  DevicePath      The device path which describe where to load 
                          the boot image or the legcy BBS device path 
                          to boot the legacy OS
  @param  ExitDataSize    Returned directly from gBS->StartImage ()
  @param  ExitData        Returned directly from gBS->StartImage ()

  @retval EFI_SUCCESS     Status from gBS->StartImage (),
                          or BdsBootByDiskSignatureAndPartition ()
  @retval EFI_NOT_FOUND   If the Device Path is not found in the system

**/
EFI_STATUS
AsfBootViaBootOption (
  IN  BDS_COMMON_OPTION             * Option,
  IN  EFI_DEVICE_PATH_PROTOCOL      * DevicePath,
  OUT UINTN                         *ExitDataSize,
  OUT CHAR16                        **ExitData OPTIONAL
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_HANDLE                ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
  EFI_EVENT                 ReadyToBootEvent;
  EFI_ACPI_S3_SAVE_PROTOCOL *AcpiS3Save;

  *ExitDataSize = 0;
  *ExitData     = NULL;

  //
  // Notes: put EFI64 ROM Shadow Solution
  //
  EFI64_SHADOW_ALL_LEGACY_ROM ();

  //
  // Notes: this code can be remove after the s3 script table
  // hook on the event EFI_EVENT_SIGNAL_READY_TO_BOOT or
  // EFI_EVENT_SIGNAL_LEGACY_BOOT
  //
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **)&AcpiS3Save);
  if (!EFI_ERROR (Status)) {
    AcpiS3Save->S3Save (AcpiS3Save, NULL);
  }
  //
  // If it's Device Path that starts with a hard drive path,
  // this routine will do the booting.
  //
  Status = BdsBootByDiskSignatureAndPartition (
            Option,
            (HARDDRIVE_DEVICE_PATH *) DevicePath,
            Option->LoadOptionsSize,
            Option->LoadOptions,
            ExitDataSize,
            ExitData
            );
  if (!EFI_ERROR (Status)) {
    //
    // If we found a disk signature and partition device path return success
    //
    return EFI_SUCCESS;
  }
  //
  // Signal the EFI_EVENT_SIGNAL_READY_TO_BOOT event
  //
  Option->BootCurrent = GetBootCurrent (DevicePath);

  //
  // Set Boot Current
  //
  gRT->SetVariable (
        L"BootCurrent",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof (UINT16),
        &Option->BootCurrent
        );

  //
  // Abstract: Modified for EFI 2.0 required
  //
  Status = EfiCreateEventReadyToBootEx (
            TPL_CALLBACK,
            NULL,
            NULL,
            &ReadyToBootEvent
            );
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent (ReadyToBootEvent);
    gBS->CloseEvent (ReadyToBootEvent);
  }

  if ((DevicePathType (Option->DevicePath) == BBS_DEVICE_PATH) &&
      (DevicePathSubType (Option->DevicePath) == BBS_BBS_DP)
      ) {
    //
    // Check to see if we should legacy BOOT. If yes then do the legacy boot
    //
    return AsfDoLegacyBoot (Option);
  }
  //
  // Drop the TPL level from EFI_TPL_DRIVER to EFI_TPL_APPLICATION
  //
  gBS->RestoreTPL (TPL_APPLICATION);

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Booting EFI 1.1 way %S\n", Option->Description));

  Status = gBS->LoadImage (
                  TRUE,
                  gImageHandle,
                  DevicePath,
                  NULL,
                  0,
                  &ImageHandle
                  );

  //
  // If we didn't find an image, we may need to load the default
  // boot behavior for the device.
  //
  if (EFI_ERROR (Status)) {
    //
    // Find a Simple File System protocol on the device path. If the remaining
    // device path is set to end then no Files are being specified, so try
    // the removable media file name.
    //
    TempDevicePath = DevicePath;
    Status = gBS->LocateDevicePath (
                    &gEfiSimpleFileSystemProtocolGuid,
                    &TempDevicePath,
                    &Handle
                    );
    if (!EFI_ERROR (Status) && IsDevicePathEnd (TempDevicePath)) {
      FilePath = FileDevicePath (Handle, EFI_REMOVABLE_MEDIA_FILE_NAME);
      if (FilePath) {
        Status = gBS->LoadImage (
                        TRUE,
                        gImageHandle,
                        FilePath,
                        NULL,
                        0,
                        &ImageHandle
                        );
        if (EFI_ERROR (Status)) {
          //
          // The DevicePath failed, and it's not a valid
          // removable media device.
          //
          goto Done;
        }
      }
    } else {
      Status = EFI_NOT_FOUND;
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // It there is any error from the Boot attempt exit now.
    //
    goto Done;
  }
  //
  // Provide the image with it's load options
  //
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
  ASSERT_EFI_ERROR (Status);

  if (Option->LoadOptionsSize != 0) {
    ImageInfo->LoadOptionsSize  = Option->LoadOptionsSize;
    ImageInfo->LoadOptions      = Option->LoadOptions;
  }
  //
  // Before calling the image, enable the Watchdog Timer for
  // the 5 Minute period
  //
  gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

  Status = gBS->StartImage (ImageHandle, ExitDataSize, ExitData);
  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Image Return Status = %r\n", Status));

  //
  // Clear the Watchdog Timer after the image returns
  //
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

Done:
  //
  // Clear Boot Current
  //
  gRT->SetVariable (
        L"BootCurrent",
        &gEfiGlobalVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        0,
        &Option->BootCurrent
        );

  //
  // Raise the TPL level back to EFI_TPL_DRIVER
  //
  gBS->RaiseTPL (TPL_DRIVER);

  return Status;
}


/**
  Found out ASF boot options.

  @param  DevicePath    The device path which describe where to load 
                        the boot image or the legcy BBS device path 
                        to boot the legacy OS

  @retval EFI_STATUS

**/
EFI_STATUS
BdsAsfBoot (
  OUT  EFI_DEVICE_PATH_PROTOCOL        **DevicePath
  )
{
  BOOLEAN                            EfiBoot;
  UINT8                              AsfEventData[3];

  EfiBoot = FALSE;
  if (BdsLibGetBootType () == EFI_BOOT_TYPE) {
    EfiBoot = TRUE;
  }

  //
  // First we check ASF boot options Special Command
  //
  switch (mAsfBootOptions->SpecialCommand) {
  //
  // No additional special command is included; the Special Command Parameter has no
  // meaning.
  //
  case NOP:
    break;

  //
  // The Special Command Parameter can be used to specify a PXE
  // parameter. When the parameter value is 0, the system default PXE device is booted. All
  // other values for the PXE parameter are reserved for future definition by this specification.
  //
  case FORCE_PXE:
    if (mAsfBootOptions->SpecialCommandParam != 0) {
      //
      // ASF spec says 0 currently only option
      //
      return EFI_UNSUPPORTED;
    }

    //
    // Set ASF Event Data
    //
    AsfEventData[0] = 0x01; // Network
    AsfEventData[1] = 0x00;
    AsfEventData[2] = 0x00;
    gRT->SetVariable (
        L"AsfEventData",
        &gEfiGenericVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof (AsfEventData),
        &AsfEventData
        );

    ReportStatusCode (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BOOT_OPTION_CHANGE
    );          
    if (EfiBoot == TRUE) {
      *DevicePath = BdsCreatePxeDevicePath (mAsfBootOptions->SpecialCommandParam);
    } else {
      *DevicePath = BdsCreateLegacyBootDevicePath (BBS_EMBED_NETWORK, mAsfBootOptions->SpecialCommandParam, FALSE);
    }
    break;

  //
  // The Special Command Parameter identifies the boot-media index for
  // the managed client. When the parameter value is 0, the default hard-drive is booted, when the
  // parameter value is 1, the primary hard-drive is booted; when the value is 2, the secondary
  // hard-drive is booted - and so on.
  //
  case FORCE_HARDDRIVE:
  //
  // The Special Command Parameter identifies the boot-media
  // index for the managed client. When the parameter value is 0, the default hard-drive is
  // booted, when the parameter value is 1, the primary hard-drive is booted; when the value is 2,
  // the secondary hard-drive is booted - and so on.
  //
    //
    // Set ASF Event Data
    //
    AsfEventData[0] = 0x02; // New_HDD
    AsfEventData[1] = 0x00;
    AsfEventData[2] = 0x00;
    gRT->SetVariable (
           L"AsfEventData",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
           sizeof (AsfEventData),
           &AsfEventData
           );

    ReportStatusCode (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BOOT_OPTION_CHANGE
    );          
  case FORCE_SAFEMODE:
    if (!EfiBoot) {
      *DevicePath = BdsCreateLegacyBootDevicePath (BBS_TYPE_HARDDRIVE, mAsfBootOptions->SpecialCommandParam, FALSE);
    } else {
      *DevicePath = BdsCreateUefiBootDevicePath (FORCE_HARDDRIVE, mAsfBootOptions->SpecialCommandParam, FALSE);
    }    
    break;

  //
  // The Special Command Parameter can be used to specify a
  // diagnostic parameter. When the parameter value is 0, the default diagnostic media is booted.
  // All other values for the diagnostic parameter are reserved for future definition by this
  // specification.
  //
  case FORCE_DIAGNOSTICS:
    if (mAsfBootOptions->SpecialCommandParam != 0) {
      //
      // ASF spec says 0 currently only option
      //
      return EFI_UNSUPPORTED;
    }

    *DevicePath = BdsCreateShellDevicePath ();

    //
    // We want everything connected up for shell
    //
    BdsLibConnectAllDriversToAllControllers ();
    break;

  //
  // The Special Command Parameter identifies the boot-media index for
  // the managed client. When the parameter value is 0, the default CD/DVD is booted, when the
  // parameter value is 1, the primary CD/DVD is booted; when the value is 2, the secondary
  // CD/DVD is booted - and so on.
  //
  case FORCE_CDDVD:
    //
    // Set ASF Event Data
    //
    AsfEventData[0] = 0x03; // Removable Device
    AsfEventData[1] = 0x00;
    AsfEventData[2] = 0x00;
    gRT->SetVariable (
        L"AsfEventData",
        &gEfiGenericVariableGuid,
        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
        sizeof (AsfEventData),
        &AsfEventData
        );

    ReportStatusCode (
    EFI_PROGRESS_CODE,
    EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BOOT_OPTION_CHANGE
    );          
    if (!EfiBoot) {
      *DevicePath = BdsCreateLegacyBootDevicePath (BBS_TYPE_CDROM, mAsfBootOptions->SpecialCommandParam, FALSE);
    } else {
      *DevicePath = BdsCreateUefiBootDevicePath (FORCE_CDDVD, mAsfBootOptions->SpecialCommandParam, FALSE);
    }
    break;

  default:
    return EFI_UNSUPPORTED;
    break;
  }

  return EFI_SUCCESS;
}


/**
  Check IdeR boot device and Asf boot device

  @param  DevicePath    The device path which describe where to load 
                        the boot image or the legcy BBS device path 
                        to boot the legacy OS

  @retval EFI_STATUS

**/
EFI_STATUS
BdsAmtBoot (
  OUT  EFI_DEVICE_PATH_PROTOCOL        **DevicePath
  )
{
  EFI_STATUS                         Status;
  BOOLEAN                            EfiBoot;

  EfiBoot = FALSE;
  if (BdsLibGetBootType () == 2) {
    EfiBoot = TRUE;
  }

  //
  // OEM command values; the interpretation of the Special Command and associated Special
  // Command Parameters is defined by the entity associated with the Enterprise ID.
  //
  if (ActiveManagementEnableIdeR ()) {
    //
    // Check if any media exist in Ider device
    //
    if (BdsCheckIderMedia ()) {
      if (!EfiBoot) {
        *DevicePath = BdsCreateLegacyBootDevicePath (
                        BBS_TYPE_CDROM,
                        (mAsfBootOptions->SpecialCommandParam & IDER_BOOT_DEVICE_MASK) >> IDER_BOOT_DEVICE_SHIFT,
                        TRUE
                        );
      } else {
        *DevicePath = BdsCreateUefiBootDevicePath (
                        FORCE_CDDVD, 
                        (mAsfBootOptions->SpecialCommandParam & IDER_BOOT_DEVICE_MASK) >> IDER_BOOT_DEVICE_SHIFT,
                        TRUE
                        );
      }
    }

    Status = EFI_SUCCESS;
  } else {
    Status = BdsAsfBoot (DevicePath);
  }

  return Status;
}

static
BOOLEAN
IsHddChanged (
  IN  VOID
  )
{
  UINTN                     HandleNum;
  EFI_HANDLE                *HandleBuf;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DPath;
  UINTN                     Index;
  UINTN                     Index2;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  UINT32                    IdeChannel;
  UINT32                    IdeDevice;
  EFI_ATA_IDENTIFY_DATA     *IdentifyData;
  UINT32                    IdentifyDataSize;
  UINTN                     HddSerialNumberSize;
  CHAR8                     HddSerialNumber[20];
  BOOLEAN                   HddChanged;

  IdentifyData     = NULL;        
  IdentifyDataSize = (UINT32)sizeof(EFI_ATA_IDENTIFY_DATA);           
  HddChanged       = FALSE;

  //
  // Make sure the Legacy Boot Protocol is available
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleNum,
                  &HandleBuf
                  );
  if (EFI_ERROR (Status)) {
    return HddChanged;
  }

  for (Index = 0; Index < HandleNum; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuf[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DPath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuf[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlkIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuf[Index],
                    &gEfiDiskInfoProtocolGuid,
                    (VOID **)&DiskInfo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    DiskInfo->WhichIde (DiskInfo, &IdeChannel, &IdeDevice);

    Status = gBS->AllocatePool ( 
                    EfiBootServicesData,
                    IdentifyDataSize,
                    (VOID **)&IdentifyData
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = DiskInfo->Identify (
                       DiskInfo,
                       IdentifyData,
                       &IdentifyDataSize
                       );
    if (EFI_ERROR (Status)) {
      continue;
    }

    HddSerialNumberSize = 20;
    Status = gRT->GetVariable(
                    L"HddSerialNumber",
                    &gEfiGenericVariableGuid,
                    NULL,
                    &HddSerialNumberSize,
                    HddSerialNumber
                    );
    if (!EFI_ERROR (Status)) {
      for (Index2 = 0; Index2 < 20; Index2++) {
        if (HddSerialNumber[Index2] != IdentifyData->SerialNo[Index2]) {
          HddChanged = TRUE;
          break;
        }
      }
    }
    if (HddChanged == TRUE || EFI_ERROR (Status)) {
      gRT->SetVariable (
              L"HddSerialNumber",
              &gEfiGenericVariableGuid,
              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
              20,
              IdentifyData->SerialNo
              );
    }
    break;
  }

  if (IdentifyData != NULL) {
    gBS->FreePool (IdentifyData);
  }
  if (HandleBuf != NULL) {
    gBS->FreePool (HandleBuf);
  }
  return HddChanged;
}


/**
  Process ASF boot options and if available, attempt the boot 

  @retval EFI_SUCCESS   The command completed successfully
  @retval Other         Error

**/
EFI_STATUS
BdsBootViaAsf (
  IN  VOID
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  BDS_COMMON_OPTION         *BootOption;
  UINTN                     ExitDataSize;
  CHAR16                    *ExitData;
  UINT8                     AsfEventData[3];

  Status      = EFI_SUCCESS;
  DevicePath  = NULL;

  //
  // Check if ASF Boot Options is present.
  //
  if (mAsfBootOptions->SubCommand != ASF_BOOT_OPTIONS_PRESENT) {
    //
    // Set ASF Event Data
    //
    if (IsHddChanged ()) {
      AsfEventData[0] = 0x02; // New_HDD
      AsfEventData[1] = 0x00;
      AsfEventData[2] = 0x00;
      gRT->SetVariable (
             L"AsfEventData",
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
             sizeof (AsfEventData),
             &AsfEventData
             );
  
      ReportStatusCode (
        EFI_PROGRESS_CODE,
        EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BOOT_OPTION_CHANGE
        );          
    } else {
      ReportStatusCode (
        EFI_PROGRESS_CODE,
        EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BOOT_OPTION_EXPECTED
        );
    }
    
    return EFI_NOT_FOUND;
  }

  switch (mAsfBootOptions->IanaId) {

  case ASF_INTEL_CONVERTED_IANA:
    Status = BdsAmtBoot (&DevicePath);
    break;

  case ASF_INDUSTRY_CONVERTED_IANA:
    Status = BdsAsfBoot (&DevicePath);
    break;
  }

  //
  // If device path was set, the we have a boot option to use
  //
  if (DevicePath == NULL) {
    return EFI_UNSUPPORTED;
  }

  BootOption = BdsCreateBootOption (DevicePath, L"ASF Boot");
  if (BootOption == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = AsfBootViaBootOption (BootOption, BootOption->DevicePath, &ExitDataSize, &ExitData);
  if (EFI_ERROR (Status)) {
    gBS->FreePool (BootOption);
  }

  gBS->FreePool (DevicePath);

  return Status;
}


/**
  This will return if Media in IDE-R is present.

  @retval TRUE          Media is present.
  @retval FALSE         Media is not present.

**/
BOOLEAN
BdsCheckIderMedia (
  IN  VOID
  )
{
  UINTN                     HandleNum;
  EFI_HANDLE                *HandleBuf;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DPath;
  UINTN                     Index;
  UINTN                     EventIndex;
  EFI_INPUT_KEY             Key;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  EFI_BLOCK_IO_MEDIA        *BlkMedia;
  VOID                      *Buffer;
  UINT8                     IdeBootDevice;
  UINT32                    IdeChannel;
  UINT32                    IdeDevice;

//[-start-140619-IB05080432-add]//
  HandleBuf = NULL;
//[-end-140619-IB05080432-add]//
  IdeBootDevice = ActiveManagementIderBootDeviceGet ();

  DEBUG ((EFI_D_INFO | EFI_D_LOAD, "Ide Channel Device Index = %d\n", IdeBootDevice));

  //
  // Make sure the Legacy Boot Protocol is available
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleNum,
                  &HandleBuf
                  );
//[-start-140620-IB05080432-modify]//
  if (EFI_ERROR (Status) || (HandleBuf == NULL)) {
    return FALSE;
  }
//[-end-140620-IB05080432-modify]//

  for (Index = 0; Index < HandleNum; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuf[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DPath
                    );
    if (!EFI_ERROR (Status)) {
      Status = gBS->LocateDevicePath (
                      &gEfiIderControllerDriverProtocolGuid,
                      &DPath,
                      &Handle
                      );
      if (!EFI_ERROR (Status)) {
        Status = gBS->HandleProtocol (
                        HandleBuf[Index],
                        &gEfiBlockIoProtocolGuid,
                        (VOID **)&BlkIo
                        );
        if (EFI_ERROR (Status)) {
          continue;          
        }
        
        Status = gBS->HandleProtocol (
                        HandleBuf[Index],
                        &gEfiDiskInfoProtocolGuid,
                        (VOID **)&DiskInfo
                        );
        if (EFI_ERROR (Status)) {
          continue;          
        }

        DiskInfo->WhichIde (DiskInfo, &IdeChannel, &IdeDevice);

        if (IdeBootDevice != (UINT8) (IdeChannel * 2 + IdeDevice)) {
          continue;
        }

        if (BlkIo->Media->MediaPresent) {
          if (HandleBuf != NULL) {
            gBS->FreePool (HandleBuf);
          }

          return TRUE;
        } else {
          while (TRUE) {
            BlkMedia  = BlkIo->Media;
            Buffer    = AllocatePool (BlkMedia->BlockSize);
            if (Buffer) {
              BlkIo->ReadBlocks (
                      BlkIo,
                      BlkMedia->MediaId,
                      0,
                      BlkMedia->BlockSize,
                      Buffer
                      );
              gBS->FreePool (Buffer);
            }

            if (BlkMedia->MediaPresent) {
              if (HandleBuf != NULL) {
                gBS->FreePool (HandleBuf);
              }

              return TRUE;
            }

            gST->ConOut->OutputString (
                          gST->ConOut,
                          L"Boot disk missing, please insert boot disk and press ENTER\r\n"
                          );
            Key.ScanCode    = 0;
            Key.UnicodeChar = 0;
            gBS->RestoreTPL (TPL_APPLICATION);
            while (!(Key.ScanCode == 0 && Key.UnicodeChar == L'\r')) {
              Status = gBS->WaitForEvent (1, &(gST->ConIn->WaitForKey), &EventIndex);
              gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
            }

            gBS->RaiseTPL (TPL_DRIVER);
          }

          if (HandleBuf != NULL) {
            gBS->FreePool (HandleBuf);
          }

          return FALSE;
        }
      }
    }
  }

  if (HandleBuf != NULL) {
    gBS->FreePool (HandleBuf);
  }

  return FALSE;
}

BOOLEAN
BdsCheckAsfBootCmd (
  IN  VOID
  )
{
  if (mAsfBootOptions->SpecialCommand == NOP) {
    return FALSE;
  }

  return TRUE;
}
//[-end-120625-IB03530375-modify]//
