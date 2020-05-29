/** @file
    Boot menu relative functions

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/HiiExLib.h>
#include "BootConfig.h"
#include "BootMenuEx.h"

#ifndef IMAGE_DEV_BEV
#define IMAGE_DEV_BEV (0)
#endif
#ifndef IMAGE_DEV_CDROM
#define IMAGE_DEV_CDROM (0)
#endif
#ifndef IMAGE_DEV_FLOPPY
#define IMAGE_DEV_FLOPPY (0)
#endif
#ifndef IMAGE_DEV_HARDDISK
#define IMAGE_DEV_HARDDISK (0)
#endif
#ifndef IMAGE_DEV_NETWORK
#define IMAGE_DEV_NETWORK (0)
#endif
#ifndef IMAGE_DEV_PCMCIA
#define IMAGE_DEV_PCMCIA (0)
#endif
#ifndef IMAGE_DEV_USB
#define IMAGE_DEV_USB (0)
#endif
#ifndef IMAGE_DEV_EFI_CDROM
#define IMAGE_DEV_EFI_CDROM (0)
#endif
#ifndef IMAGE_DEV_EFI_FLOPPY
#define IMAGE_DEV_EFI_FLOPPY (0)
#endif
#ifndef IMAGE_DEV_EFI_HARDDISK
#define IMAGE_DEV_EFI_HARDDISK (0)
#endif
#ifndef IMAGE_DEV_EFI_MISC
#define IMAGE_DEV_EFI_MISC (0)
#endif
#ifndef IMAGE_DEV_EFI_NETWORK
#define IMAGE_DEV_EFI_NETWORK (0)
#endif
#ifndef IMAGE_DEV_EFI_OS
#define IMAGE_DEV_EFI_OS (0)
#endif
#ifndef IMAGE_DEV_EFI_SCSI
#define IMAGE_DEV_EFI_SCSI (0)
#endif
#ifndef IMAGE_DEV_EFI_SHELL
#define IMAGE_DEV_EFI_SHELL (0)
#endif
#ifndef IMAGE_DEV_EFI_USB
#define IMAGE_DEV_EFI_USB (0)
#endif

typedef struct {
  UINT32            DevType;
  EFI_IMAGE_ID      ImageId;
} DEVICE_TYPE_IMAGE_MAP_TABLE;

//
// These definitions are referenced from GenericBdsLib library
//
#define EFI_DEV_ACPI_FLOPPY              0x0201

#define EFI_DEV_MESSAGE_ATAPI            0x0301
#define EFI_DEV_MESSAGE_SCSI             0x0302
#define EFI_DEV_MESSAGE_USB_DEVICE       0x0305
#define EFI_DEV_MESSAGE_SATA             0x0312
#define EFI_DEV_MESSAGE_MAC              0x030b
#define EFI_DEV_MESSAGE_MISC             0x03FF

#define EFI_DEV_MEDIA_HD                 0x0401
#define EFI_DEV_MEDIA_CDROM              0x0402
#define EFI_DEV_MEDIA_FV_FILEPATH        0x0406

#define LEGACY_DEV_BBS                   0x0501
#define DEV_TYPE_UNSUPPORT               0xFFFF

UINT32
EFIAPI
GetBootDevTypeFromDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH          *Acpi;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;

  if (NULL == DevicePath) {
    return DEV_TYPE_UNSUPPORT;
  }

  TempDevicePath = DevicePath;

  while (!IsDevicePathEndType (TempDevicePath)) {
    switch (DevicePathType (TempDevicePath)) {

    case BBS_DEVICE_PATH:
       return LEGACY_DEV_BBS;

    case MEDIA_DEVICE_PATH:
      if (DevicePathSubType (TempDevicePath) == MEDIA_HARDDRIVE_DP) {
        return EFI_DEV_MEDIA_HD;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_CDROM_DP) {
        return EFI_DEV_MEDIA_CDROM;
      } else if (DevicePathSubType (TempDevicePath) == MEDIA_PIWG_FW_FILE_DP) {
        return EFI_DEV_MEDIA_FV_FILEPATH;
      }
      break;

    case ACPI_DEVICE_PATH:
      Acpi = (ACPI_HID_DEVICE_PATH *) TempDevicePath;
      if (EISA_ID_TO_NUM (Acpi->HID) == 0x0604) {
        return EFI_DEV_ACPI_FLOPPY;
      }
      break;

    case MESSAGING_DEVICE_PATH:
      //
      // Get the last device path node
      //
      LastDeviceNode = NextDevicePathNode (TempDevicePath);
      if (DevicePathSubType(LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP) {
        //
        // if the next node type is Device Logical Unit, which specify the Logical Unit Number (LUN),
        // skip it
        //
        LastDeviceNode = NextDevicePathNode (LastDeviceNode);
      }

      switch (DevicePathSubType (TempDevicePath)) {

      case MSG_ATAPI_DP:
        return EFI_DEV_MESSAGE_ATAPI;

      case MSG_USB_DP:
        return EFI_DEV_MESSAGE_USB_DEVICE;

      case MSG_SCSI_DP:
        return EFI_DEV_MESSAGE_SCSI;

      case MSG_SATA_DP:
        return EFI_DEV_MESSAGE_SATA;

      case MSG_MAC_ADDR_DP:
      case MSG_VLAN_DP:
      case MSG_IPv4_DP:
      case MSG_IPv6_DP:
        return EFI_DEV_MESSAGE_MAC;

      default:
        return EFI_DEV_MESSAGE_MISC;
      }
      break;

    default:
      break;
    }

    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return DEV_TYPE_UNSUPPORT;
}

EFI_STATUS
GetImageIdByBbsDeviceType (
  IN  UINT16                    BbsDeviceType,
  OUT EFI_IMAGE_ID              *ImageId
  )
{
  UINT32                        Index;
  UINT32                        Count;
  DEVICE_TYPE_IMAGE_MAP_TABLE   Table[] = {{BBS_FLOPPY       , IMAGE_DEV_FLOPPY},
                                           {BBS_HARDDISK     , IMAGE_DEV_HARDDISK},
                                           {BBS_CDROM        , IMAGE_DEV_CDROM},
                                           {BBS_PCMCIA       , IMAGE_DEV_PCMCIA},
                                           {BBS_USB          , IMAGE_DEV_USB},
                                           {BBS_EMBED_NETWORK, IMAGE_DEV_NETWORK},
                                           {BBS_BEV_DEVICE   , IMAGE_DEV_BEV},
                                          };


  Count = sizeof (Table) / sizeof (DEVICE_TYPE_IMAGE_MAP_TABLE);
  for (Index = 0; Index < Count; Index++) {
    if (Table[Index].DevType == BbsDeviceType) {
      *ImageId = Table[Index].ImageId;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetImageIdByDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN  BBS_TABLE                *BbsEntry OPTIONAL,
  OUT EFI_IMAGE_ID             *ImageId
  )
{
  UINT32                       BootDevType;
  EFI_STATUS                   Status;

  BootDevType = GetBootDevTypeFromDevicePath (DevicePath);
  if (BootDevType == DEV_TYPE_UNSUPPORT)  {
    return EFI_NOT_FOUND;
  }

  Status = EFI_SUCCESS;

  switch (BootDevType) {

  case LEGACY_DEV_BBS:
    ASSERT (BbsEntry != NULL);
    if (BbsEntry == NULL) {
      Status = EFI_NOT_FOUND;
      break;
    }

    if (IsUsbDevice (BbsEntry)) {
      Status = GetImageIdByBbsDeviceType (BBS_USB, ImageId);
    } else {
      Status = GetImageIdByBbsDeviceType (BbsEntry->DeviceType, ImageId);
    }
    break;

  case EFI_DEV_MEDIA_HD:
    *ImageId = IMAGE_DEV_EFI_OS;
    break;

  case EFI_DEV_MESSAGE_ATAPI:
  case EFI_DEV_MESSAGE_SATA:
    *ImageId = IMAGE_DEV_EFI_HARDDISK;
    break;

  case EFI_DEV_MESSAGE_SCSI:
    *ImageId = IMAGE_DEV_EFI_SCSI;
    break;

  case EFI_DEV_MESSAGE_USB_DEVICE:
    *ImageId = IMAGE_DEV_EFI_USB;
    break;

  case EFI_DEV_MESSAGE_MAC:
    *ImageId = IMAGE_DEV_EFI_NETWORK;
    break;

  case EFI_DEV_MEDIA_CDROM:
    *ImageId = IMAGE_DEV_EFI_CDROM;
    break;

  case EFI_DEV_ACPI_FLOPPY:
    *ImageId = IMAGE_DEV_EFI_FLOPPY;
    break;

  case EFI_DEV_MEDIA_FV_FILEPATH:
    *ImageId = IMAGE_DEV_EFI_SHELL;
    break;

  case EFI_DEV_MESSAGE_MISC:
  default:
    *ImageId = IMAGE_DEV_EFI_MISC;
    break;
  }

  return Status;
}

/**
 Get index value of mBbsTypeTable[] array by boot option

 @retval EFI_SUCCESS              Get index value of mBbsTypeTable[] array successfully
 @retval EFI_NOT_FOUND            Can not find the index value
**/
EFI_STATUS
GetBbsTypeTableIndex (
  IN  UINT16              BootOption,
  OUT UINTN               *TableIndex
  )
{
  UINTN                                      Index;
  UINTN                                      DevCount;
  UINTN                                      DevIndex;
  STRING_PTR                                 *StringPtr;


  for (Index = 0; Index < BBS_TYPE_TABLE_NUM; Index++) {
    if (mBbsTypeTable[Index].DeviceTypeCount == 0) {
      continue;
    }

    StringPtr = (STRING_PTR *) mBbsTypeTable[Index].StringPtr;
    DevCount  = mBbsTypeTable[Index].DeviceTypeCount;

    for (DevIndex = 0; DevIndex < DevCount; DevIndex++) {
      if (StringPtr[DevIndex].BootOrderIndex == BootOption) {
        break;
      }
    }

    if (DevIndex < DevCount) {
      break;
    }
  }

  if (Index == BBS_TYPE_TABLE_NUM) {
    return EFI_NOT_FOUND;
  }

  *TableIndex = Index;

  return EFI_SUCCESS;
}


/**
 Convert boot option to priority value

 @param[in]  BootOption             Boot option value
 @param[out] Priority               Pointer to priority value

 @retval EFI_SUCCESS                Convert boot option to priority value successfully
 @retval Other                      Locate setup utility private data fail
**/
EFI_STATUS
ConvertBootOptionToPriority (
  IN  UINT16                            BootOption,
  OUT UINT16                            *Priority
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  UINT16                                BootDevNum;
  UINT16                                Index;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo    = SuBrowser->SUCInfo;
  BootDevNum = SUCInfo->AdvBootDeviceNum;

  for (Index = 0; Index < BootDevNum; Index++) {
    if (SUCInfo->BootPriority[Index] == BootOption) {
      break;
    }
  }
  ASSERT(Index < BootDevNum);

  *Priority = Index + 1;

  return EFI_SUCCESS;
}

/**
 Convert priority value to boot option

 @param[in]  Priority               Pointer to priority value
 @param[out] BootOption             Boot option value

 @retval EFI_SUCCESS                Convert priority value to boot option successfully
 @retval Other                      Locate setup utility private data fail
**/
EFI_STATUS
ConvertPriorityToBootOption (
  IN  UINT16                            Priority,
  OUT UINT16                            *BootOption
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  UINT16                                BootOrderIndex;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;
  BootOrderIndex = Priority - 1;

  if (BootOrderIndex >= SUCInfo->AdvBootDeviceNum) {
    return EFI_INVALID_PARAMETER;
  }

  *BootOption = SUCInfo->BootPriority[BootOrderIndex];

  return EFI_SUCCESS;
}

/**
 Based on BootOrder, update device order of each legacy boot device type in mBbsTypeTable[]

 @retval EFI_SUCCESS              Update device order of legacy boot device type in mBbsTypeTable[] successfully
 @retval EFI_OUT_OF_RESOURCES     Allocate pool fail
 @retval EFI_ABORTED              Allocate pool fail
 @retval Other                    Locate setup utility private data fail
**/
EFI_STATUS
UpdateBbsTypeTable (
  VOID
  )
{
  EFI_STATUS                                 Status;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;
  UINT16                                     *BootOrder;
  UINT16                                     BootDevNum;
  STRING_PTR                                 *NewStringPtr;
  STRING_PTR                                 *StringPtr;
  UINT16                                     *LegacyBootDevType;
  UINT16                                     LegacyBootDevTypeCount;
  UINT16                                     TypeIndex;
  UINT16                                     TableIndex;
  UINT16                                     Index;
  UINT16                                     DevIndex;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status                 = EFI_SUCCESS;
  SUCInfo                = SuBrowser->SUCInfo;
  BootOrder              = SUCInfo->BootOrder;
  BootDevNum             = SUCInfo->AdvBootDeviceNum;
  LegacyBootDevType      = SUCInfo->LegacyBootDevType;
  LegacyBootDevTypeCount = SUCInfo->LegacyBootDevTypeCount;

  NewStringPtr = AllocatePool (BootDevNum * sizeof(STRING_PTR));
  if (NewStringPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Based on device boot priority by BootOrder, update device list in mBbsTypeTable[].StringPtr.
  //
  for (TypeIndex = 0; TypeIndex < LegacyBootDevTypeCount; TypeIndex++) {
    TableIndex = LegacyBootDevType[TypeIndex];
    if (mBbsTypeTable[TableIndex].DeviceTypeCount == 0) {
      continue;
    }

    mBbsTypeTable[TableIndex].CurrentIndex = 0;
    SetMem (NewStringPtr, (BootDevNum * sizeof(STRING_PTR)), 0xFF);
    StringPtr = (STRING_PTR *) mBbsTypeTable[TableIndex].StringPtr;

    for (Index = 0; Index < BootDevNum; Index++) {
      if (IsEfiDevice (BootOrder[Index])) {
        continue;
      }

      for (DevIndex = 0; DevIndex < mBbsTypeTable[TableIndex].DeviceTypeCount; DevIndex++) {
        if (StringPtr[DevIndex].BootOrderIndex == BootOrder[Index]) {
          CopyMem (
            &NewStringPtr[mBbsTypeTable[TableIndex].CurrentIndex],
            &StringPtr[DevIndex],
            sizeof(STRING_PTR)
            );
          mBbsTypeTable[TableIndex].CurrentIndex++;
          break;
        }
      }
    }

    CopyMem (
      StringPtr,
      NewStringPtr,
      sizeof(STRING_PTR) * mBbsTypeTable[TableIndex].DeviceTypeCount
      );
  }

  FreePool (NewStringPtr);

  return EFI_SUCCESS;
}

/**
 Update legacy boot device type order in setup configuration

 @retval EFI_SUCCESS              Update legacy boot device type order successfully
 @retval EFI_OUT_OF_RESOURCES     Allocate pool fail
 @retval Other                    Locate setup utility private data fail
**/
EFI_STATUS
UpdateLegacyBootDevTypeOrder (
  VOID
  )
{
  EFI_STATUS                                 Status;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;
  UINT16                                     *BootOrder;
  UINT16                                     BootDevNum;
  UINTN                                      BootOrderIndex;
  UINT8                                      *TypeOrder;
  UINT8                                      *OldTypeOrder;
  UINT8                                      *NewTypeOrder;
  UINT8                                      *PhysicalDevTypeOrder;
  UINTN                                      PhysicalDevTypeCount;
  UINTN                                      TypeOrderSize;
  UINTN                                      TypeOrderNum;
  UINTN                                      Count;
  UINTN                                      Start;
  UINTN                                      Index;
  UINTN                                      TypeIndex;
  UINTN                                      TableIndex;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo       = SuBrowser->SUCInfo;
  BootOrder     = SUCInfo->BootOrder;
  BootDevNum    = SUCInfo->AdvBootDeviceNum;
  TypeOrder     = ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->BootTypeOrder;
  TypeOrderSize = sizeof (((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->BootTypeOrder);

  TypeOrderNum  = 0;
  while (TypeOrder[TypeOrderNum] != 0 && TypeOrderNum < TypeOrderSize) {
    TypeOrderNum++;
  }

  OldTypeOrder = AllocateCopyPool (TypeOrderSize, ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->BootTypeOrder);
  if (OldTypeOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  NewTypeOrder = AllocatePool (TypeOrderSize);
  if (NewTypeOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  PhysicalDevTypeOrder = AllocatePool (TypeOrderSize);
  if (PhysicalDevTypeOrder == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Get device type order of physical devices.
  //
  PhysicalDevTypeCount = 0;
  for (BootOrderIndex = 0; BootOrderIndex < BootDevNum; BootOrderIndex++) {
    Status = GetBbsTypeTableIndex (BootOrder[BootOrderIndex], &TableIndex);
    if (EFI_ERROR(Status)) {
      continue;
    }

    for (Index = 0; Index < PhysicalDevTypeCount; Index++) {
      if (PhysicalDevTypeOrder[Index] == mBbsTypeTable[TableIndex].DeviceType) {
        break;
      }
    }

    if (Index == PhysicalDevTypeCount) {
      PhysicalDevTypeOrder[PhysicalDevTypeCount++] = mBbsTypeTable[TableIndex].DeviceType;
    }
  }

  //
  // Sync the device type order of physical devices into device type order
  //
  Start = 0;
  Count = 0;
  for (TypeIndex = 0; TypeIndex < TypeOrderNum; TypeIndex++) {
    for (Index = 0; Index < Count; Index++) {
      if (NewTypeOrder[Index] == OldTypeOrder[TypeIndex]) {
        break;
      }
    }
    if (Index < Count) {
      continue;
    }

    for (Index = Start; Index < PhysicalDevTypeCount; Index++) {
      if (PhysicalDevTypeOrder[Index] == OldTypeOrder[TypeIndex]) {
        CopyMem (
          &NewTypeOrder[Count],
          &PhysicalDevTypeOrder[Start],
          Index - Start + 1
          );
        Count += (Index - Start + 1);
        Start += (Index - Start + 1);
        break;
      }
    }

    if (Index == PhysicalDevTypeCount) {
      NewTypeOrder[Count++] = OldTypeOrder[TypeIndex];
    }
  }
  ASSERT(Count == TypeOrderNum);

  CopyMem (
    ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->BootTypeOrder,
    NewTypeOrder,
    TypeOrderNum
    );

  FreePool (OldTypeOrder);
  FreePool (NewTypeOrder);
  FreePool (PhysicalDevTypeOrder);

  return EFI_SUCCESS;
}

/**
 Update device order for each legacy boot device type in boot configuration

 @retval EFI_SUCCESS              Update device order successfully
 @retval Other                    Locate setup utility private data fail
**/
EFI_STATUS
UpdateLegacyBootDevTypeDevOrder (
  VOID
  )
{
  EFI_STATUS                                 Status;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;
  STRING_PTR                                 *StringPtr;
  UINT16                                     *LegacyBootDevType;
  UINT16                                     LegacyBootDevTypeCount;
  UINT16                                     TypeIndex;
  UINT16                                     TableIndex;
  UINT16                                     DevIndex;
  UINT16                                     Priority;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo                = SuBrowser->SUCInfo;
  LegacyBootDevType      = SUCInfo->LegacyBootDevType;
  LegacyBootDevTypeCount = SUCInfo->LegacyBootDevTypeCount;

  //
  // Update device order of legacy boot device type from mBbsTypeTable[]
  //
  for (TypeIndex = 0; TypeIndex < LegacyBootDevTypeCount; TypeIndex++) {
    TableIndex = LegacyBootDevType[TypeIndex];
    if (mBbsTypeTable[TableIndex].DeviceTypeCount == 0) {
      continue;
    }

    StringPtr  = (STRING_PTR *) mBbsTypeTable[TableIndex].StringPtr;

    for (DevIndex = 0; DevIndex < mBbsTypeTable[TableIndex].DeviceTypeCount; DevIndex++) {
      Status = ConvertBootOptionToPriority (StringPtr[DevIndex].BootOrderIndex, &Priority);
      if (EFI_ERROR(Status)) {
        ASSERT(FALSE);
        continue;
      }

      mBootConfig.LegacyTypeDevOrder[TypeIndex * MAX_BOOT_ORDER_NUMBER + DevIndex] = Priority;
    }
  }

  return Status;
}

/**
 Update legacy boot device order in boot configuration

 @retval EFI_SUCCESS              Update legacy boot devices order successfully
 @retval Other                    Locate setup utility private data fail
**/
EFI_STATUS
UpdateLegacyAdvBootDevOrder (
  VOID
  )
{
  EFI_STATUS                                 Status;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;
  UINT16                                     *BootOrder;
  UINT16                                     BootDevNum;
  UINT16                                     Count;
  UINT16                                     Index;
  UINT16                                     Priority;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status     = EFI_SUCCESS;
  SUCInfo    = SuBrowser->SUCInfo;
  BootOrder  = SUCInfo->BootOrder;
  BootDevNum = SUCInfo->AdvBootDeviceNum;
  Count      = 0;

  for (Index = 0; Index < BootDevNum; Index++) {
    if (IsEfiDevice (BootOrder[Index])) {
      continue;
    }

    Status = ConvertBootOptionToPriority (BootOrder[Index], &Priority);
    if (EFI_ERROR(Status)) {
      ASSERT(FALSE);
      continue;
    }

    mBootConfig.LegacyAdvBootDevOrder[Count++] = Priority;
  }

  return EFI_SUCCESS;
}

/**
 Update legacy boot option type order label

 @param[in] HiiHandle               HII hanlde for HII get string function
 @param[in] BootTypeOrderString     Pointer to boot type order information

 @retval EFI_SUCCESS                Update legacy boot option type order label successfully
 @retval Other                      Locate setup utility private data fail
**/
EFI_STATUS
UpdateLegacyBootDevTypeOrderLabel (
  IN EFI_HII_HANDLE               HiiHandle,
  IN STRING_PTR                   *BootTypeOrderString
  )
{
  EFI_STATUS                            Status;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;
  UINT16                                LegacyBootTypeOrderCount;
  UINTN                                 Index;
  UINT8                                 Count;
  STRING_REF                            Token;
  VOID                                  *BootTypeOrderOpCodeHandle;
  VOID                                  *OptionsOpCodeHandle;
  KERNEL_CONFIGURATION                  KernelConfig;
  UINT16                                Offset;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  VOID                                  *ImageOpCodeHandle;
  EFI_IMAGE_ID                          ImageId;


  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SUCInfo = SuBrowser->SUCInfo;
  LegacyBootTypeOrderCount = SUCInfo->LegacyBootDevTypeCount;

  BootTypeOrderOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (BootTypeOrderOpCodeHandle != NULL);

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (BootTypeOrderOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = BOOT_ORDER_LABEL;

  Count = 0;
  for (Index = 0; Index < (UINT16) LegacyBootTypeOrderCount; Index++) {
    if (BootTypeOrderString[Index].pString == NULL) {
      continue;
    }

    Token = HiiSetString (HiiHandle, 0, BootTypeOrderString[Index].pString, NULL);

    Status = GetImageIdByDevicePath (BootTypeOrderString[Index].DevicePath, BootTypeOrderString[Index].BbsEntry, &ImageId);
    if (Status == EFI_SUCCESS) {
      ImageOpCodeHandle = HiiAllocateOpCodeHandle ();
      HiiCreateImageOpCode (ImageOpCodeHandle, ImageId);

      HiiCreateOneOfOptionOpCodeWithExtOp (
        OptionsOpCodeHandle,
        Token,
        0,
        EFI_IFR_NUMERIC_SIZE_1,
        BootTypeOrderString[Index].BootOrderIndex,
        ImageOpCodeHandle
        );
    } else {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        Token,
        0,
        EFI_IFR_NUMERIC_SIZE_1,
        BootTypeOrderString[Index].BootOrderIndex
        );
    }
    Count++;
  }

  Offset = (UINT16) (UINTN) &KernelConfig.BootTypeOrder[0] - (UINT16) (UINTN) &KernelConfig;
  HiiCreateOrderedListOpCode (
     BootTypeOrderOpCodeHandle,                 // Container for dynamic created opcodes
     KEY_BOOT_TYPE_ORDER_BASE,                  // Question ID
     CONFIGURATION_VARSTORE_ID,                 // VarStore ID
     Offset,                                    // Offset in Buffer Storage
     STRING_TOKEN (STR_BOOT_TYPE_ORDER_OPTIONS),// Question prompt text
     STRING_TOKEN (STR_BOOT_TYPE_ORDER_OPTIONS),// Question help text
     EFI_IFR_FLAG_CALLBACK,                     // Question flag
     0,                                         // Ordered list flag, e.g. EFI_IFR_UNIQUE_SET
     EFI_IFR_NUMERIC_SIZE_1,                    // Data type of Question value
     Count,                                     // Maximum container
     OptionsOpCodeHandle,                       // Option Opcode list
     NULL                                       // Default Opcode is NULl
     );

  HiiUpdateForm (
    HiiHandle,
    NULL,
    BOOT_DEVICE_LEG_NOR_BOOT_ID,
    BootTypeOrderOpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (BootTypeOrderOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);

  return EFI_SUCCESS;
}

/**
 Update device list in order list opcode on specific label

 @param[in] HiiHandle               HII hanlde for HII get string function
 @param[in] DevListStringPtr        Pointer to device order information
 @param[in] DevCount                Device number
 @param[in] QuestionId              Question ID for order list opcode
 @param[in] Label                   Label
 @param[in] FormId                  Form ID of tge label
 @param[in] PromptStrToken          Prompt string token of the label
 @param[in] HelpStrToken            Help string token of the label
 @param[in] Offset                  Offset of the varstore

 @retval EFI_SUCCESS                Update device list in order list opcode successfully
 @retval Other                      Locate setup utility private data fail
**/
EFI_STATUS
UpdateDevListLabel (
  IN EFI_HII_HANDLE                     HiiHandle,
  IN STRING_PTR                         *DevListStringPtr,
  IN UINTN                              DevCount,
  IN UINT16                             QuestionId,
  IN UINT16                             Label,
  IN EFI_FORM_ID                        FormId,
  IN STRING_REF                         PromptStrToken,
  IN STRING_REF                         HelpStrToken,
  IN UINT16                             Offset
  )
{
  EFI_STATUS                            Status;
  VOID                                  *StartOpCodeHandle;
  VOID                                  *OptionsOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  UINTN                                 Index;
  STRING_REF                            StrToken;
  UINT16                                Priority;
  VOID                                  *ImageOpCodeHandle;
  EFI_IMAGE_ID                          ImageId;

  if (DevListStringPtr == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  if (StartOpCodeHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);
  if (OptionsOpCodeHandle == NULL) {
    HiiFreeOpCodeHandle (StartOpCodeHandle);
    return EFI_OUT_OF_RESOURCES;
  }
  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = Label;

  for (Index = 0; Index < DevCount; Index++) {
    Status = ConvertBootOptionToPriority (DevListStringPtr[Index].BootOrderIndex, &Priority);
    if (EFI_ERROR(Status)) {
      ASSERT(FALSE);
      continue;
    }

    StrToken = HiiSetString (HiiHandle, 0, DevListStringPtr[Index].pString, NULL);

    Status = GetImageIdByDevicePath (DevListStringPtr[Index].DevicePath, DevListStringPtr[Index].BbsEntry, &ImageId);
    if (Status == EFI_SUCCESS) {
      ImageOpCodeHandle = HiiAllocateOpCodeHandle ();
      HiiCreateImageOpCode (ImageOpCodeHandle, ImageId);

      HiiCreateOneOfOptionOpCodeWithExtOp (
        OptionsOpCodeHandle,
        StrToken,
        0,
        EFI_IFR_NUMERIC_SIZE_2,
        Priority,
        ImageOpCodeHandle
        );
    } else {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        StrToken,
        0,
        EFI_IFR_NUMERIC_SIZE_2,
        Priority
        );
    }
  }

  HiiCreateOrderedListOpCode (
    StartOpCodeHandle,                         // Container for dynamic created opcodes
    QuestionId,                                // Question ID
    BOOT_VARSTORE_ID,                          // VarStore ID
    Offset,                                    // Offset in Buffer Storage
    PromptStrToken,                            // Question prompt text
    HelpStrToken,                              // Question help text
    EFI_IFR_FLAG_CALLBACK,                     // Question flag
    0,                                         // Ordered list flag, e.g. EFI_IFR_UNIQUE_SET
    EFI_IFR_NUMERIC_SIZE_2,                    // Data type of Question value
    (UINT8) DevCount,                          // Maximum container
    OptionsOpCodeHandle,                       // Option Opcode list
    NULL                                       // Default Opcode is NULl
    );

  HiiUpdateForm (
    HiiHandle,
    NULL,
    FormId,
    StartOpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);

  return EFI_SUCCESS;
}

/**
 Sync order list value into BootOrder

 @param[in] QuestionId                Question ID for current order list
 @param[in] Priority                  Order list value which describe the boot priority

 @retval EFI_SUCCESS                  Sync legacy boot device order successful
 @retval EFI_ABORTED                  Fail to convert boot priority to boot option
 @retval Other                        Sync legacy boot device order successful
**/
EFI_STATUS
SyncOrderListValueToBootOrder (
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT16                                 *Priority
  )
{
  EFI_STATUS                                 Status;
  SETUP_UTILITY_BROWSER_DATA                 *SuBrowser;
  SETUP_UTILITY_CONFIGURATION                *SUCInfo;
  BOOLEAN                                    IsLegacyFirst;
  UINTN                                      Index;
  UINTN                                      Base;
  UINTN                                      TableIndex;
  UINTN                                      DevIndex;
  UINT16                                     *LegacyBootDevType;
  UINT16                                     LegacyBootDevTypeCount;
  UINT16                                     TypeIndex;
  UINTN                                      BootOrderIndex;
  UINT16                                     *BootOrder;
  UINT16                                     BootDevNum;
  UINT16                                     BootOption;
  UINT16                                     LegacyBootDevNum;
  UINT16                                     EfiBootDevNum;
  STRING_PTR                                 *StringPtr;
  UINT8                                      *BootTypeOrder;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status                 = EFI_SUCCESS;
  SUCInfo                = SuBrowser->SUCInfo;
  BootOrder              = SUCInfo->BootOrder;
  BootDevNum             = SUCInfo->AdvBootDeviceNum;
  EfiBootDevNum          = SUCInfo->EfiBootDeviceNum;
  LegacyBootDevNum       = SUCInfo->LegacyBootDeviceNum;
  LegacyBootDevType      = SUCInfo->LegacyBootDevType;
  LegacyBootDevTypeCount = SUCInfo->LegacyBootDevTypeCount;
  IsLegacyFirst          = ((KERNEL_CONFIGURATION *) SuBrowser->SCBuffer)->BootNormalPriority;
  StringPtr              = NULL;
  TableIndex             = 0;

  switch (QuestionId) {

  case KEY_EFI_BOOT_DEVICE_BASE:
    //
    // Update EFI boot device order into BootOrder
    //
    Base = IsLegacyFirst ? LegacyBootDevNum : 0;

    for (DevIndex = 0; DevIndex < EfiBootDevNum; DevIndex++, Priority++) {
      Status = ConvertPriorityToBootOption (*Priority, &BootOption);
      if (EFI_ERROR(Status)) {
        return EFI_ABORTED;
      }

      SUCInfo->BootOrder[Base + DevIndex] = BootOption;
    }
    break;

  case KEY_ADV_LEGACY_BOOT_BASE:
    //
    // Update legacy boot device order into BootOrder
    //
    Base = IsLegacyFirst ? 0 : EfiBootDevNum;

    for (DevIndex = 0; DevIndex < LegacyBootDevNum; DevIndex++, Priority++) {
      Status = ConvertPriorityToBootOption (*Priority, &BootOption);
      if (EFI_ERROR(Status)) {
        return EFI_ABORTED;
      }

      SUCInfo->BootOrder[Base + DevIndex] = BootOption;
    }

    UpdateBbsTypeTable ();

    UpdateLegacyBootDevTypeOrder ();
    UpdateLegacyBootDevTypeDevOrder ();
    break;

  case KEY_BOOT_TYPE_ORDER_BASE:
    //
    // Update devices of each type into BootOrder by type order
    //
    BootTypeOrder = (UINT8 *) Priority;
    Base          = IsLegacyFirst ? 0 : EfiBootDevNum;

    for (Index = 0; Index < LegacyBootDevTypeCount; Index++) {
      for (TypeIndex = 0; TypeIndex < LegacyBootDevTypeCount; TypeIndex++) {
        TableIndex = LegacyBootDevType[TypeIndex];

        if (mBbsTypeTable[TableIndex].DeviceType == BootTypeOrder[Index]) {
          break;
        }
      }
      ASSERT(TypeIndex < LegacyBootDevTypeCount);

      StringPtr = (STRING_PTR *) mBbsTypeTable[TableIndex].StringPtr;

      for (DevIndex = 0; DevIndex < mBbsTypeTable[TableIndex].DeviceTypeCount; DevIndex++) {
        BootOrder[Base++] = StringPtr[DevIndex].BootOrderIndex;
      }
    }

    UpdateLegacyAdvBootDevOrder ();
    break;

  case KEY_FDD_BOOT_DEVICE_BASE:
  case KEY_HDD_BOOT_DEVICE_BASE:
  case KEY_CD_BOOT_DEVICE_BASE:
  case KEY_OTHER_BOOT_DEVICE_BASE:
  case KEY_PCMCIA_BOOT_DEVICE_BASE:
  case KEY_USB_BOOT_DEVICE_BASE:
  case KEY_EMBED_NETWORK_BASE:
  case KEY_BEV_BOOT_DEVICE_BASE:
    //
    // Update device order of current device type into BootOrder
    // 1. Get current device type
    // 2. Get start BootOrder index of this device type
    // 3. Update device order in BootOrder
    //
    for (TableIndex = 0; TableIndex < BBS_TYPE_TABLE_NUM; TableIndex++) {
      if (mBbsTypeTable[TableIndex].KeyBootDeviceBase == QuestionId) {
        StringPtr = (STRING_PTR *) mBbsTypeTable[TableIndex].StringPtr;
        break;
      }
    }
    if (StringPtr == NULL) {
      break;
    }

    for (BootOrderIndex = 0; BootOrderIndex < BootDevNum; BootOrderIndex++) {
      for (DevIndex = 0; DevIndex < mBbsTypeTable[TableIndex].DeviceTypeCount; DevIndex++) {
        if (StringPtr[DevIndex].BootOrderIndex == BootOrder[BootOrderIndex]) {
          break;
        }
      }

      if (DevIndex < mBbsTypeTable[TableIndex].DeviceTypeCount) {
        break;
      }
    }
    ASSERT(BootOrderIndex < BootDevNum);

    for (DevIndex = 0; DevIndex < mBbsTypeTable[TableIndex].DeviceTypeCount; DevIndex++, Priority++) {
      Status = ConvertPriorityToBootOption (*Priority, &BootOption);
      if (EFI_ERROR(Status)) {
        return EFI_ABORTED;
      }

      BootOrder[BootOrderIndex++] = BootOption;
    }

    UpdateBbsTypeTable ();
    UpdateLegacyAdvBootDevOrder ();
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  return Status;
}

/**
  Update all of the boot devices in platform to boot page

  @param  BootHiiHandle          Hii hanlde for Boot page
  @param  SetupVariable          Pointer to SYSTEM_CONFIGURATION instance

  @retval EFI_SUCCESS            Update boot menu successful
  @retval Other                  Some error occured in update boot menu

**/
EFI_STATUS
UpdateBootOrderToOrderedListOpcode (
  IN     EFI_HII_HANDLE                 BootHiiHandle,
  IN OUT KERNEL_CONFIGURATION           *KernelConfig
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_HII_HANDLE                        HiiHandle;
  UINT16                                LegacyBootTypeOrderCount;
  UINT16                                LegacyBootDeviceNum;
  UINT16                                EfiBootDeviceNum;
  STRING_PTR                            *UpdateAdvBootString;
  STRING_PTR                            *UpdateLegacyAdvBootString;
  STRING_PTR                            *UpdateEfiBootString;
  STRING_PTR                            *UpdateBootTypeOrderString;
  UINT16                                *TempBootType;
  SETUP_UTILITY_CONFIGURATION           *SUCInfo;
  SETUP_UTILITY_BROWSER_DATA            *SuBrowser;

  Status = GetSetupUtilityBrowserData (&SuBrowser);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Initial the flag of showing from.
  //
  KernelConfig->NoAdvBootDev    = 0;
  KernelConfig->NoBootTypeOrder = 0;
  KernelConfig->NoLegacyBootDev = 0;
  KernelConfig->NoEfiBootDev    = 0;

  for(Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
    mBootConfig.NoBootDevs[Index] = 0;
  }

  //
  // if there is no BootOption, skip to build from.
  //
  if (SuBrowser->SUCInfo->AdvBootDeviceNum == 0) {
    return EFI_SUCCESS;
  }

  HiiHandle = BootHiiHandle;
  SUCInfo   = SuBrowser->SUCInfo;

  UpdateAdvBootString       = NULL;
  UpdateLegacyAdvBootString = NULL;
  UpdateEfiBootString       = NULL;
  UpdateBootTypeOrderString = NULL;

  Status = GetBbsName (
             HiiHandle,
             KernelConfig,
             &UpdateBootTypeOrderString,
             &UpdateAdvBootString,
             &UpdateLegacyAdvBootString,
             &UpdateEfiBootString
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  LegacyBootTypeOrderCount = SUCInfo->LegacyBootDevTypeCount;
  TempBootType             = SUCInfo->LegacyBootDevType;
  LegacyBootDeviceNum      = SUCInfo->LegacyBootDeviceNum;
  EfiBootDeviceNum         = SUCInfo->EfiBootDeviceNum;


  if (LegacyBootTypeOrderCount != 0) {
    KernelConfig->NoBootTypeOrder = 1;
    UpdateLegacyBootDevTypeOrderLabel (HiiHandle, UpdateBootTypeOrderString);
  }

  if (LegacyBootDeviceNum != 0) {
    KernelConfig->NoLegacyBootDev = 1;
    UpdateDevListLabel (
      HiiHandle,
      UpdateLegacyAdvBootString,
      SUCInfo->LegacyBootDeviceNum,
      KEY_ADV_LEGACY_BOOT_BASE,
      BOOT_LEGACY_ADV_BOOT_LABEL,
      BOOT_OPTION_FORM_ID,
      STRING_TOKEN (STR_LEGACY),
      STRING_TOKEN (STR_LEGACY),
      (UINT16) (UINTN) &mBootConfig.LegacyAdvBootDevOrder[0] - (UINT16) (UINTN) &mBootConfig
      );
  }

  //
  // update boot device form by type
  //
  for (Index = 0; Index < LegacyBootTypeOrderCount; Index++) {
    if (mBbsTypeTable[TempBootType[Index]].DeviceTypeCount != 0) {
      mBootConfig.NoBootDevs[TempBootType[Index]] = 1;
      UpdateDevListLabel (
        HiiHandle,
        (STRING_PTR *) mBbsTypeTable[TempBootType[Index]].StringPtr,
        mBbsTypeTable[TempBootType[Index]].DeviceTypeCount,
        mBbsTypeTable[TempBootType[Index]].KeyBootDeviceBase,
        mBbsTypeTable[TempBootType[Index]].BootDevicesLabel,
        mBbsTypeTable[TempBootType[Index]].FormId,
        mBbsTypeTable[TempBootType[Index]].StrToken,
        mBbsTypeTable[TempBootType[Index]].StrToken,
        (UINT16) (UINTN) &mBootConfig.LegacyTypeDevOrder[Index * MAX_BOOT_ORDER_NUMBER] - (UINT16) (UINTN) &mBootConfig
        );
    }
  }

  if (EfiBootDeviceNum != 0) {
    KernelConfig->NoEfiBootDev = 1;
    BootOptionStrAppendDeviceName (EfiBootDeviceNum, UpdateEfiBootString);

    UpdateDevListLabel (
      HiiHandle,
      UpdateEfiBootString,
      SUCInfo->EfiBootDeviceNum,
      KEY_EFI_BOOT_DEVICE_BASE,
      EFI_BOOT_DEVICE_LABEL,
      BOOT_DEVICE_EFI_FORM_ID,
      STRING_TOKEN (STR_EFI),
      STRING_TOKEN (STR_EFI),
      (UINT16) (UINTN) &mBootConfig.EfiBootDevOrder[0] - (UINT16) (UINTN) &mBootConfig
      );
  }


  if (UpdateBootTypeOrderString != NULL) {
    for (Index = 0; Index < MAX_BOOT_ORDER_NUMBER; Index++) {
      if (UpdateBootTypeOrderString[Index].pString != NULL) {
        gBS->FreePool (UpdateBootTypeOrderString[Index].pString);
      }
    }
    gBS->FreePool (UpdateBootTypeOrderString);
  }

  if (UpdateAdvBootString != NULL) {
    for (Index = 0; Index < SuBrowser->SUCInfo->AdvBootDeviceNum; Index++) {
      if (UpdateAdvBootString[Index].BbsEntry != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].BbsEntry);
      }
      if (UpdateAdvBootString[Index].pString != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].pString);
      }
      if (UpdateAdvBootString[Index].DevicePath != NULL) {
        gBS->FreePool (UpdateAdvBootString[Index].DevicePath);
      }
    }
    gBS->FreePool (UpdateAdvBootString);
  }

  if (UpdateLegacyAdvBootString != NULL) {
    for (Index = 0; Index < LegacyBootDeviceNum; Index++) {
      if (UpdateLegacyAdvBootString[Index].pString != NULL) {
        gBS->FreePool (UpdateLegacyAdvBootString[Index].pString);
      }
    }
    gBS->FreePool (UpdateLegacyAdvBootString);
  }

  if (UpdateEfiBootString != NULL) {
    for (Index = 0; Index < EfiBootDeviceNum; Index++) {
      if (UpdateEfiBootString[Index].pString != NULL) {
        gBS->FreePool (UpdateEfiBootString[Index].pString);
      }
    }
    gBS->FreePool (UpdateEfiBootString);
  }

  return EFI_SUCCESS;
}

