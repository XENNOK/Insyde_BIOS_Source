/** @file
  Serial driver for standard UARTS on an ISA bus.

Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/



#include <Library/PrintLib.h>
#include <Library/VariableLib.h>
#include <IndustryStandard/Atapi.h>
#include "SnapLib.h"
#include "SnapConOut.h"

#include <Protocol/DiskInfo.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/BlockIo.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/GlobalVariable.h>

#define TYPE_IDENTIFY     0
#define TYPE_INQUIRY      1

#define MAX_LABEL_LENGHT    11

typedef struct _DISK_TYPE_INFO {
  DISK_TYPE   Type;
  EFI_GUID    *Guid;
} DISK_TYPE_INFO;

DISK_TYPE_INFO    gDiskType[] = {
  {DT_IDE,  &gEfiDiskInfoIdeInterfaceGuid},
  {DT_SCSI, &gEfiDiskInfoScsiInterfaceGuid},
  {DT_USB,  &gEfiDiskInfoUsbInterfaceGuid},
  {DT_AHCI, &gEfiDiskInfoAhciInterfaceGuid},
  {DT_UNKNOW, NULL}
};


static EFI_GRAPHICS_OUTPUT_PROTOCOL      *mActiveVgaGop = NULL;


/**
  Removes (trims) specified leading and trailing characters from a string.

  @param[in, out]  Str  Pointer to the null-terminated string to be trimmed. On return,
                        Str will hold the trimmed string.

  @param[in]      CharC Character will be trimmed from str.
**/
VOID
StrTrim (
  IN OUT CHAR16   *Str,
  IN     CHAR16   CharC
  )
{
  CHAR16  *Pointer1;
  CHAR16  *Pointer2;

  if (*Str == 0) {
    return;
  }

  //
  // Trim off the leading and trailing characters c
  //
  for (Pointer1 = Str; (*Pointer1 != 0) && (*Pointer1 == CharC); Pointer1++) {
    ;
  }

  Pointer2 = Str;
  if (Pointer2 == Pointer1) {
    while (*Pointer1 != 0) {
      Pointer2++;
      Pointer1++;
    }
  } else {
    while (*Pointer1 != 0) {
    *Pointer2 = *Pointer1;
    Pointer1++;
    Pointer2++;
    }
    *Pointer2 = 0;
  }


  for (Pointer1 = Str + StrLen(Str) - 1; Pointer1 >= Str && *Pointer1 == CharC; Pointer1--) {
    ;
  }
  if  (Pointer1 !=  Str + StrLen(Str) - 1) {
    *(Pointer1 + 1) = 0;
  }
}

/**

  Duplicates a string.

  @param  Src  Source string.

  @return The duplicated string.

**/
CHAR16 *
StrDuplicate (
  IN CONST CHAR16  *Src
  )
{
  return AllocateCopyPool (StrSize (Src), Src);
}


CHAR16 *
EfiStrDuplicate (
  IN CHAR16   *Src
  )
{
  CHAR16  *Dest;
  UINTN   Size;

  Size  = StrSize (Src);
  Dest  = AllocateZeroPool (Size);
  ASSERT (Dest != NULL);
  if (Dest) {
    CopyMem (Dest, Src, Size);
  }

  return Dest;
}

/**

  Calculate the size of device path

  @param  DevicePath            Pointer of EFI_DEVICE_PATH_PROTOCOL

  @retval Size of device path

**/
STATIC
UINTN
DevicePathSize (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Start;

  if (DevicePath == NULL) {
    return 0;
  }
  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!IsDevicePathEnd (DevicePath)) {
    DevicePath = NextDevicePathNode (DevicePath);
  }
  //
  // Compute the size and add back in the size of the end device path structure
  //
  return ((UINTN) DevicePath - (UINTN) Start) + sizeof (EFI_DEVICE_PATH_PROTOCOL);
}


VOID
TruncateLastNode (
  IN OUT EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *LastNode = NULL;
  EFI_DEVICE_PATH_PROTOCOL    *DevPath;

  DevPath = DevicePath;

  while (!IsDevicePathEnd(DevPath)) {
      LastNode = DevPath;
      DevPath = NextDevicePathNode (DevPath);
  }

  if (LastNode != NULL) {
    CopyMem( LastNode, DevPath, sizeof(EFI_DEVICE_PATH_PROTOCOL));
  }

  return;
}


EFI_STATUS
GetParentHandle (
  EFI_HANDLE      ChildHandle,
  EFI_HANDLE      *ParentHandle
  )
{
  EFI_STATUS                    Status = EFI_NOT_FOUND;
  EFI_HANDLE                    Handle;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *TmpDevPath;
  EFI_DEVICE_PATH_PROTOCOL      *OrgDevPath;

  Status = gBS->HandleProtocol (ChildHandle, &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);

  if (EFI_ERROR(Status)) {
    return Status;
  }

  OrgDevPath = DuplicateDevicePath (DevicePath);
  TmpDevPath = OrgDevPath;

  TruncateLastNode (TmpDevPath);
  gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &TmpDevPath, &Handle);

  if (IsDevicePathEnd(TmpDevPath)) {
    *ParentHandle = Handle;
    Status = EFI_SUCCESS;
  }

  gBS->FreePool (TmpDevPath);

  return Status;
}

VOID
ValueToString (
  UINTN     Value,
  UINTN     Digitals,
  CHAR16    *Buffer,
  UINTN     Flags
  )
{
  CHAR8   Str[30];
  UINTN   mod;
  CHAR8   *p1;
  CHAR16  *p2;
  CHAR16  StuffChar = 0;
  UINTN   count = 0;
  UINTN   Padding;

  p1  = Str;
  p2  = Buffer;

  if (Flags == VTS_RIGHT_ALIGN)
    StuffChar = L' ';

  if (Flags == VTS_LEAD_0)
    StuffChar = L'0';

  if (Value == 0) {

    // Stuff specify char to buffer
    if (StuffChar != 0) {
      Padding = Digitals - 1;

      while (Padding > 0) {
        *p2 = StuffChar;
        p2++;
        Padding--;
      }
    }

    *p2++ = '0';
    *p2 = 0;
    return ;
  }

  while (Value) {
    mod = Value % 10;
    *p1 = (CHAR8)(mod + '0');
    Value = Value / 10;
    p1++;
    count++;
  }

  if ((count < Digitals) && (StuffChar != 0)) {
    Padding = Digitals - count;
    while (Padding) {
      *p2 = StuffChar;
      p2++;
      Padding--;
    }
  }

  while (count > 0) {
    p1--;
    *p2 = *p1;
    p2++;

    count--;
  }

  *p2 = 0;
}

EFI_STATUS
GetDiskIdentifyData (
  EFI_DISK_INFO_PROTOCOL    *DiskInfo,
  UINT8                     **Buffer,
  UINTN                     *BufferSize
  )
{
  EFI_STATUS    Status;
  UINT8         *IdentifyData = NULL;
  UINT32        DataSize;

  DataSize = 0;
  Status = DiskInfo->Identify (DiskInfo, IdentifyData, &DataSize);

  // process buffer too small
  if (Status == EFI_BUFFER_TOO_SMALL) {

    IdentifyData = AllocatePool ( DataSize);
    if (IdentifyData == NULL)
      return EFI_OUT_OF_RESOURCES;

    Status = DiskInfo->Identify (DiskInfo, IdentifyData, &DataSize);
  }

  if (EFI_ERROR(Status)) {
    if (IdentifyData != NULL)
      gBS->FreePool (IdentifyData);

    return Status;
  }

  *Buffer = IdentifyData;
  *BufferSize = DataSize;

  return  Status;
}

EFI_STATUS
GetDiskInquiryData (
  EFI_DISK_INFO_PROTOCOL    *DiskInfo,
  UINT8                     **Buffer,
  UINTN                     *BufferSize
  )
{
  EFI_STATUS    Status;
  UINT8         *InquiryData = NULL;
  UINT32        DataSize;

  DataSize = 0;
  Status = DiskInfo->Inquiry (DiskInfo, InquiryData, &DataSize);

  // process buffer too small
  if (Status == EFI_BUFFER_TOO_SMALL) {
    InquiryData = AllocatePool ( DataSize);
    Status = DiskInfo->Inquiry (DiskInfo, InquiryData, &DataSize);
  }

  //
  if (EFI_ERROR(Status)) {
    if (InquiryData != NULL)
      gBS->FreePool (InquiryData);

    return Status;
  }

  *Buffer = InquiryData;
  *BufferSize = DataSize;

  return  Status;
}

VOID
UsbVendorIdToUnicodeStr (
  CHAR16      *Buffer,
  CHAR8       *VendorId
  )
{
  UINTN   Index;

  for (Index = 0; Index < 8; Index++)
    *Buffer++ = *VendorId++;

  *Buffer = 0;

  // Trim space char
  StrTrim(Buffer, L' ');

  return;
}

CHAR16 *
GetNameFromInquiry (
  DISK_TYPE     Type,
  UINT8         *Data
  )
{
  CHAR16                  Buffer[9];
  CHAR16                  *Str;
  ATAPI_INQUIRY_DATA      *InquiryData;
  USB_BOOT_INQUIRY_DATA   *UsbInquiry;

  Buffer [8] = 0;

  switch (Type) {
    case DT_IDE :
    case DT_AHCI :
    case DT_SCSI :
      InquiryData = (ATAPI_INQUIRY_DATA *) Data;
      UsbVendorIdToUnicodeStr( Buffer, InquiryData->vendor_info);
      Str  = StrDuplicate( Buffer);
      break;
    case DT_USB :
      UsbInquiry = (USB_BOOT_INQUIRY_DATA *) Data;
      UsbVendorIdToUnicodeStr (Buffer, UsbInquiry->VendorID);
      Str = StrDuplicate(Buffer);
      break;
    default :
      Str = NULL;
  }

  return Str;
}

VOID
IdentifyModelNameToUnicode (
  CHAR16      *Buffer,
  CHAR8       *ModelName
  )
{
  UINTN       Index;
  CHAR8       SwModelName[41];
  CHAR8       *Ptr;

  // swap ATA hi-lo byte
  for (Index=0; Index < 40; Index+=2) {
    SwModelName[Index] = ModelName[Index+1];
    SwModelName[Index+1] = ModelName[Index];
  }

  SwModelName[40] = 0;
  Ptr = SwModelName;

  while (*Ptr && *Ptr == ' ') Ptr++;

  for (Index = 0; Index <= 12; Index++) {

    if (*Ptr == 0)
      break;

    *Buffer++ = *Ptr++;
  }

  *Buffer = 0;
}


CHAR16 *
GetNameFromIdentify (
  DISK_TYPE     Type,
  UINT8         *Data
  )
{
  ATA5_IDENTIFY_DATA     *AtaIdentifyData;
  CHAR16                 *ModelName;
  CHAR16                 Buffer[13];

  switch (Type) {
    case DT_IDE :
    case DT_AHCI :
    case DT_SCSI :
      AtaIdentifyData = (ATA5_IDENTIFY_DATA *) Data;
      IdentifyModelNameToUnicode (Buffer, AtaIdentifyData->ModelName);
      StrTrim (Buffer, L' ');
      ModelName = StrDuplicate(Buffer);
      break;

    case DT_USB :
    default :
      ModelName = NULL;
  }

  return ModelName;
}

DISK_TYPE
GetDiskInfoType (
  EFI_HANDLE      Handle
  )
{
  EFI_STATUS                Status;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  UINTN                     Index;

  Status = gBS->HandleProtocol (Handle, &gEfiDiskInfoProtocolGuid, (VOID **)&DiskInfo);
  if (EFI_ERROR(Status))
    return DT_UNKNOW;

  for (Index=0; gDiskType[Index].Type != DT_UNKNOW; Index++) {
    if (CompareGuid(&DiskInfo->Interface, gDiskType[Index].Guid) == TRUE)
      break;
  }

  return gDiskType[Index].Type;
}

CHAR16 *
GetDiskInfoName (
  EFI_HANDLE      Handle
  )
{
  EFI_STATUS                Status;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  UINTN                     BufferSize = 512;
  UINT8                     *Buffer = NULL;
  DISK_TYPE                 DiskType;
  UINT8                     DataType;
  CHAR16                    *ProductName = NULL;

  DiskType = GetDiskInfoType (Handle);
  if (DiskType == DT_UNKNOW) {
    return NULL;
  }

  Status = gBS->HandleProtocol (Handle, &gEfiDiskInfoProtocolGuid, (VOID **)&DiskInfo);
  if (EFI_ERROR(Status))
    return NULL;

  DataType = TYPE_IDENTIFY;
  Status = GetDiskIdentifyData ( DiskInfo, &Buffer, &BufferSize);

  if (EFI_ERROR(Status)) {
    DataType = TYPE_INQUIRY;
    Status = GetDiskInquiryData (DiskInfo, &Buffer, &BufferSize);
    if (EFI_ERROR(Status)) {
      return NULL;
    }
  }

  switch (DataType) {
    case TYPE_IDENTIFY :
      ProductName = GetNameFromIdentify (DiskType, Buffer);
      break;

    case TYPE_INQUIRY :
      ProductName = GetNameFromInquiry (DiskType, Buffer);
      break;
  }

  if (Buffer != NULL)
    gBS->FreePool (Buffer);

  return ProductName;
}

CHAR16 *
GetVolumnName (
  EFI_HANDLE    FsHandle
  )
{
  EFI_STATUS                        Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *FileSystem;
  EFI_FILE                          *RootFs;
  UINTN                             Size;
  EFI_FILE_SYSTEM_INFO              *VolumeInfo;
  CHAR16                            LabelStr[MAX_LABEL_LENGHT+1] = {0};
  CHAR16                            *VolumeName = NULL;

  Status = gBS->HandleProtocol (FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystem);
  if (EFI_ERROR(Status))
    return NULL;

  Status = FileSystem->OpenVolume (FileSystem, &RootFs);
  if (EFI_ERROR(Status))
    return NULL;

  Size        = SIZE_OF_EFI_FILE_SYSTEM_INFO + 100;
  VolumeInfo  = (EFI_FILE_SYSTEM_INFO *) AllocatePool (Size);
  Status = RootFs->GetInfo (RootFs, &gEfiFileSystemInfoGuid, &Size, VolumeInfo);

  if (Status == EFI_SUCCESS) {
    if (VolumeInfo->VolumeLabel[0] != 0) {
      StrnCpy (LabelStr, VolumeInfo->VolumeLabel, MAX_LABEL_LENGHT);
      VolumeName = StrDuplicate(LabelStr);
    }
  }

  gBS->FreePool (VolumeInfo);

  return VolumeName;
}

VOID
ConvDititalByUnit (
  CHAR16    *Str
  )
{
  UINTN   Len;
  UINTN   Quotient;
  CHAR16  *Ptr;
  CHAR16  LastD;
  INT16   Pos;

  Len = StrLen(Str);

  Quotient = (Len-1) / 3;

  Ptr = Str;
  while (*Ptr != 0) Ptr++;

  if (Quotient != 0) {
    Pos = -((INT16)Quotient * 3);
    LastD = Ptr[Pos];
    if (LastD != L'0') {
      Ptr[Pos++] = L'.';
      Ptr[Pos++] = LastD;
      Ptr[Pos] = 0;
    }
    else {
      Ptr[Pos] = 0;
    }
  }

  switch (Quotient) {
    //Bytes
    case 0:
      StrCat (Str, L"Byte");
      break;

    // KB
    case 1:
      StrCat (Str, L" KB");
      break;

    // MB
    case 2:
      StrCat (Str, L" MB");
      break;

    // GB
    case 3 :
      StrCat (Str, L" GB");
      break;

    default :
      StrCat (Str, L" TB");
      break;
  }
}

CHAR16 *
GetVolumnSizeAsString (
  EFI_HANDLE    FsHandle
  )
{
  EFI_STATUS                        Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *FileSystem;
  EFI_FILE                          *RootFs;
  UINTN                             Size;
  EFI_FILE_SYSTEM_INFO              *VolumeInfo;
  CHAR16                            *VolumeSizeStr = NULL;
  CHAR16                            Buffer[32];

  Status = gBS->HandleProtocol (FsHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileSystem);
  if (EFI_ERROR(Status))
    return NULL;

  Status = FileSystem->OpenVolume (FileSystem, &RootFs);
  if (EFI_ERROR(Status))
    return NULL;

  Size        = SIZE_OF_EFI_FILE_SYSTEM_INFO + 100;
  VolumeInfo  = (EFI_FILE_SYSTEM_INFO *) AllocatePool (Size);
  Status = RootFs->GetInfo (RootFs, &gEfiFileSystemInfoGuid, &Size, VolumeInfo);
  if (Status == EFI_SUCCESS) {
    ValueToString (VolumeInfo->VolumeSize, 32, Buffer, VTS_LEFT_ALIGN);
    ConvDititalByUnit (Buffer);
    VolumeSizeStr = StrDuplicate( Buffer);
  }

  gBS->FreePool (VolumeInfo);

  return VolumeSizeStr;
}

BOOLEAN
IsRecordableDevice (
  EFI_HANDLE    Handle
  )
{
  EFI_BLOCK_IO_PROTOCOL          *BlkIo;
  EFI_STATUS                      Status;

  Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlkIo);
  if (EFI_ERROR(Status))
    return FALSE;

  if (BlkIo->Media->ReadOnly)
    return FALSE;

  //if (!BlkIo->Media->RemovableMedia)
  //  return FALSE;

  return TRUE;
}

/**
 Function compares a device path data structure to that of all the nodes of a
 second device path instance.

 @param [in]   Multi            A pointer to a multi-instance device path data structure.
 @param [in]   Single           A pointer to a single-instance device path data structure.

 @retval TRUE                   If the Single is contained within Multi
 @retval FALSE                  The Single is not match within Multi

**/
BOOLEAN
MatchDevicePaths (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN  EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePathInst;
  UINTN                     Size;

  if (!Multi || !Single) {
    return FALSE;
  }

  DevicePath      = Multi;
  DevicePathInst  = GetNextDevicePathInstance (&DevicePath, &Size);

  //
  // Search for the match of 'Single' in 'Multi'
  //
  while (DevicePathInst != NULL) {
    //
    // If the single device path is found in multiple device paths,
    // return success
    //
    if (CompareMem (Single, DevicePathInst, Size) == 0) {
      gBS->FreePool (DevicePathInst);
      return TRUE;
    }

    gBS->FreePool (DevicePathInst);
    DevicePathInst = GetNextDevicePathInstance (&DevicePath, &Size);
  }

  return FALSE;
}


EFI_STATUS
GetActiveVgaHandle (
  EFI_HANDLE      *VgaHandle
  )
{
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *ConOutDevPath;
  EFI_DEVICE_PATH_PROTOCOL      *DevPath;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         Index;
  static EFI_HANDLE             mActiveVgaHandle = NULL;

  // verify  parameter
  if (VgaHandle == NULL)
    return EFI_INVALID_PARAMETER;

  //3 If we have the VGA handle just return it.
  if (mActiveVgaHandle != NULL) {
    *VgaHandle = mActiveVgaHandle;
    return EFI_SUCCESS;
  }

  // Get ConOut device path from variable
  ConOutDevPath = CommonGetVariableData (L"ConOut", &gEfiGlobalVariableGuid);

  if (ConOutDevPath == NULL)
     return EFI_NOT_READY;


  // Get all GOP protocol handles
  Status = gBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiGraphicsOutputProtocolGuid,
                 NULL,
                 &HandleCount,
                 &HandleBuffer
                 );

  if (EFI_ERROR(Status))
    return EFI_NOT_READY;

  // compare Active VGA device path
  for (Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevPath);

    if (EFI_ERROR(Status))
      continue;

    if (MatchDevicePaths (ConOutDevPath, DevPath)) {
      mActiveVgaHandle = HandleBuffer[Index];
      break;
    }
  }

  if (mActiveVgaHandle == NULL)
    return EFI_UNSUPPORTED;

  // Set return value
  *VgaHandle = mActiveVgaHandle;

  gBS->FreePool (ConOutDevPath);
  gBS->FreePool (HandleBuffer);

  return EFI_SUCCESS;
}

EFI_STATUS
GetActiveVgaGop (
  EFI_GRAPHICS_OUTPUT_PROTOCOL    **Gop
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    VgaHandle;

  if (mActiveVgaGop == NULL) {

    Status = GetActiveVgaHandle(&VgaHandle);
    if (EFI_ERROR(Status))
      return EFI_NOT_READY;

    gBS->HandleProtocol (VgaHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **)&mActiveVgaGop);
  }

  *Gop = mActiveVgaGop;

  return EFI_SUCCESS;
}

VOID
GetKey (
  OUT EFI_INPUT_KEY      *Key
)
{

  Key->ScanCode    = 0;
  Key->UnicodeChar = 0;

  do {
    gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
  } while ((Key->ScanCode == 0) && (Key->UnicodeChar == 0));

}

EFI_STATUS
LocateDriverIndex (
  EFI_GUID        *ProtocolGuid
  )
{
  EFI_STATUS      Status;
  UINTN           HandleCount;
  EFI_HANDLE      *HandleBuffer;
  UINTN           HIndex;
  UINTN           PIndex;
  UINTN           ProtocolCount;
  EFI_GUID        **ProtocolBuffer;
  VOID            *Interface;

  Status = gBS->LocateProtocol (ProtocolGuid, NULL, (VOID **)&Interface);
  if (EFI_ERROR(Status))
    return Status;

  Status = gBS->LocateHandleBuffer (
                AllHandles,
                NULL,
                NULL,
                &HandleCount,
                &HandleBuffer
                );

  if (EFI_ERROR (Status)) {
    //Print(L"LibScanHandleDatabase ERROR!!!\n");
    //UnicodeSPrint(NULL,NULL,L"LibScanHandleDatabase ERROR!!!\n");
    return Status;
  }

  for (HIndex = HandleCount; HIndex > 0; HIndex--) {

    Status = gBS->ProtocolsPerHandle (HandleBuffer[HIndex-1], &ProtocolBuffer, &ProtocolCount);

    for (PIndex = 0; PIndex < ProtocolCount; PIndex++) {

      if(CompareGuid(ProtocolBuffer[PIndex], ProtocolGuid) == TRUE) {
        //Print (L"SnapScreen Image Handle Index : 0x%X\n", HIndex);
        //UnicodeSPrint (NULL,NULL,L"SnapScreen Image Handle Index : 0x%X\n", HIndex);
        return EFI_SUCCESS;
      }
    }
    gBS->FreePool (ProtocolBuffer);
  }

  gBS->FreePool (HandleBuffer);

  return EFI_NOT_FOUND;
}


