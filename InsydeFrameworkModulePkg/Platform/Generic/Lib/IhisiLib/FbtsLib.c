//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   FbtsLib.c
//;

#include "IhisiLib.h"
#include "Kernel.h"
#include "EfiVariable.h"
#include EFI_PROTOCOL_DEFINITION (SmmVariable)
#include EFI_PROTOCOL_DEFINITION (SmmOemServices)

#define DEFAULT_VARIABLE_NAME_SIZE    0x50

PRESERVED_VARIABLE_TABLE      *mPreservedTableList = NULL;
BOOLEAN                       mIsKeepVariableInList;
EFI_SMM_VARIABLE_PROTOCOL     *mSmmVariable = NULL;
EFI_GUID                      mDefaultPreservedVendorGuid = { 0x77fa9abd, 0x0359, 0x4d32, 0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b };
#ifdef SECURE_FLASH_SUPPORT
extern BOOLEAN mInPOST;
#endif

EFI_STATUS
FbtsLibGetPlatformInfo (
  OUT    UINT32                             *IhisiStatus
  )
/*++

Routine Description:

  Get platform information.(AH=11h)

Arguments:

  IhisiStatus          - Return IHISI status

Returns:

  EFI_SUCCESS - FBTS get platform information success.
  Others      - FBTS get platform information failed.

--*/
{
  FBTS_PLATFORM_INFO_BUFFER                 *PlatformInfoPtr;
  UINTN                                     StrSize;
  EFI_STATUS                                Status;

  *IhisiStatus = FbtsUnknowPlatformInfo;
  //
  // DS:EDI - Pointer to platform information structure as below.
  //
  //  Offset | Size | Item      | Description
  // --------|------|-----------|--------------
  //   00h   | 40h  | Model Name| Unicode string, end with ¡¥00h¡¦.
  //
  PlatformInfoPtr = (FBTS_PLATFORM_INFO_BUFFER *)(UINTN)(SmmCpuSaveLocalState->EDI);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) PlatformInfoPtr, sizeof (FBTS_PLATFORM_INFO_BUFFER))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  EfiZeroMem (PlatformInfoPtr, sizeof (FBTS_PLATFORM_INFO_BUFFER));

  StrSize = MODEL_VERSION_SIZE;
  Status  = GetBvdtInfo ((BVDT_TYPE) BvdtBiosVer, &StrSize, PlatformInfoPtr->ModelVersion);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  StrSize = MODEL_NAME_SIZE;
  Status  = GetBvdtInfo ((BVDT_TYPE) BvdtProductName, &StrSize, PlatformInfoPtr->ModelName);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *IhisiStatus = FbtsLibStatusTranslation (Status);

  return Status;
}

EFI_STATUS
FbtsLibGetPlatformRomMap (
  IN     FBTS_PLATFORM_ROM_MAP_BUFFER       *RomMapBufferPtr,
  IN     FBTS_PLATFORM_PRIVATE_ROM_BUFFER   *PrivateRomMapBufferPtr,
  OUT    UINT32                             *IhisiStatus
  )
/*++

Routine Description:

  Get Platform ROM map protection.(AH=12h)

Arguments:

  RomMapBufferPtr        - Pointer to returned data buffer (ROM map).
  PrivateRomMapBufferPtr - Pointer to returned data buffer (Private map).
  IhisiStatus            - Return IHISI status.

Returns:

  EFI_SUCCESS - FBTS get platform ROM map success.
  Others      - FBTS get platform ROM map failed.

--*/
{
  FBTS_PLATFORM_ROM_MAP_BUFFER              *InputRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER          *InputPrivateRomMapBuffer;
  EFI_STATUS                                Status;
  //
  // Get ROM map protection structure.
  //
  InputRomMapBuffer = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN)(SmmCpuSaveLocalState->EDI);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) InputRomMapBuffer, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  //
  // Get platform¡¦s private map structure.
  //
  InputPrivateRomMapBuffer = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN)(SmmCpuSaveLocalState->ESI);
   if (BufferOverlapSmram ((VOID *) InputPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  EfiCopyMem (InputRomMapBuffer, RomMapBufferPtr, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER));
  EfiCopyMem (InputPrivateRomMapBuffer, PrivateRomMapBufferPtr, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));

  *IhisiStatus = IhisiSuccess;

  return EFI_SUCCESS;
}

EFI_STATUS
FbtsLibGetFlashPartInfo (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  IN     EFI_SMM_SYSTEM_TABLE               *mSmst,
  OUT    UINT32                             *IhisiStatus
  )
/*++

Routine Description:

  Flash part information.(AH=13h)

Arguments:

  SmmFwBlockService    - The EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  mSmst                - The EFI_SMM_SYSTEM_TABLE instance.
  IhisiStatus          - Return IHISI status

Returns:

  EFI_SUCCESS - FBTS get flash part information success.
  Others      - FBTS get flash part information failed.

--*/
{
  FLASH_DEVICE                              *Buffer;
  EFI_STATUS                                Status;
  FBTS_FLASH_DEVICE                         FlashDevice;
  UINT16                                    BlockMap[3];
  UINT8                                     SpiFlashNumber;
  UINT8                                     *FlashInfo;
  UINTN                                     FlashInfoSize;
  UINT8                                     *FlashBlockMap;

  *IhisiStatus = FbtsUnknowFlashPartInfo;
  //
  // CL = 00h => default, no choice from AP.
  //      01h => AP need flash SPI flash part.
  //      02h => AP need flash non-SPI flash part (LPC, FWH).
  //
  mSmmChipsetLibServices->SelectFlashInterface (SmmCpuSaveLocalState->ECX);

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FLASH_DEVICE),
                    &Buffer
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmFwBlockService->DetectDevice (
                                SmmFwBlockService,
                                (UINT8*)Buffer
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FlashDevice.Id = (UINT32)Buffer->DeviceInfo.Id;
  FlashDevice.Size = Buffer->DeviceInfo.Size;
  FlashDevice.SpecifiedSize = 0;
  EfiAsciiStrCpy (FlashDevice.VendorName, Buffer->DeviceInfo.VendorName);
  EfiAsciiStrCpy (FlashDevice.DeviceName, Buffer->DeviceInfo.DeviceName);
  EfiCopyMem ((VOID *)BlockMap, &(Buffer->DeviceInfo.BlockMap), sizeof (FD_BLOCK_MAP));
  if (FlashDevice.Size == 0xFF) {
    //
    // The BlockSize unit is 256(0x100) byte.
    //
    FlashDevice.SpecifiedSize = (Buffer->DeviceInfo.BlockMap.Mutiple * Buffer->DeviceInfo.BlockMap.BlockSize) * 0x100;
  } else {
    Status = SmmFwBlockService->GetSpiFlashNumber (
                                  SmmFwBlockService,
                                  &SpiFlashNumber
                                  );

    if (!EFI_ERROR (Status)) {
      FlashDevice.Size += SpiFlashNumber - 1;
      BlockMap[1] *= 1 << (SpiFlashNumber - 1);
    }
  }

  //
  // DS:EDI - Pointer to flash part information structure.
  // DS:ESI - Pointer to flash part block map structure.
  //
  FlashInfo     = (UINT8 *) (UINTN)(SmmCpuSaveLocalState->EDI);
  FlashInfoSize = sizeof(FBTS_FLASH_DEVICE) - sizeof (FlashDevice.BlockMap);
  //
  // Check output address and whole output buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) FlashInfo , FlashInfoSize)) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  EfiCopyMem (
    (VOID *) FlashInfo,
    &FlashDevice,
    FlashInfoSize
    );
  FlashBlockMap     = (UINT8 *) (UINTN) SmmCpuSaveLocalState->ESI;
  //
  // Check output address and whole output buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) FlashBlockMap, sizeof (FD_BLOCK_MAP))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  EfiCopyMem ((VOID *) FlashBlockMap, BlockMap, sizeof (FD_BLOCK_MAP));

  *IhisiStatus = IhisiSuccess;

  return EFI_SUCCESS;
}

EFI_STATUS
FbtsLibRead (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  OUT    UINT32                             *IhisiStatus
  )
/*++

Routine Description:

  FBTS Read.(AH=14h)

Arguments:

  SmmFwBlockService    - The EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  IhisiStatus          - Return IHISI status

Returns:

  EFI_SUCCESS - FBTS read success.
  Others      - FBTS read failed.

--*/
{
  EFI_STATUS                                Status;
  UINT8                                     *DataBuffer;
  UINTN                                     Size;
  UINTN                                     Address;
  UINTN                                     Offset;

  //
  // ECX    - Size to read.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to read.
  //
  DataBuffer = (UINT8 *)(UINTN)(SmmCpuSaveLocalState->ESI);
  Size       = (UINTN)(SmmCpuSaveLocalState->ECX);
  Address    = (UINTN)(SmmCpuSaveLocalState->EDI);
  //
  // Check output address and whole output buffer isn't located in SM RAM.
  // Check input SPI address isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) DataBuffer, Size) || BufferOverlapSmram ((VOID *) Address, Size)) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  Offset     = 0;

  Status = SmmFwBlockService->Read (
                                SmmFwBlockService,
                                Address,
                                Offset,
                                &Size,
                                DataBuffer
                                );
  if (EFI_ERROR (Status)) {
    *IhisiStatus = FbtsReadFail;
  } else {
    *IhisiStatus = IhisiSuccess;
  }

  return Status;
}

EFI_STATUS
FbtsLibWrite (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  OUT    UINT32                             *IhisiStatus
  )
/*++

Routine Description:

  FBTS write.(AH=15h)

Arguments:

  SmmFwBlockService    - The EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  IhisiStatus          - Return IHISI status

Returns:

  EFI_SUCCESS - FBTS write success.
  Others      - FBTS write failed.

--*/
{
  EFI_STATUS                                Status;
  UINT8                                     *WriteDataBuffer;
  UINTN                                     WriteSize;
  UINTN                                     RomBaseAddress;
  UINT8                                     EraseCount;
  UINT8                                     WriteCount;

  // ECX    - Size to read.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to write.

  WriteDataBuffer = (UINT8 *)(UINTN)(SmmCpuSaveLocalState->ESI);
  WriteSize       = (UINTN)(SmmCpuSaveLocalState->ECX);
  RomBaseAddress  = (UINTN)(SmmCpuSaveLocalState->EDI);
  if (BufferOverlapSmram ((VOID *) WriteDataBuffer, WriteSize) || BufferOverlapSmram ((VOID *) RomBaseAddress, WriteSize)) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  EraseCount      = 0;
  WriteCount      = 0;

#ifdef SECURE_FLASH_SUPPORT
if (!mInPOST) {
  *IhisiStatus = FbtsWriteFail;
  return EFI_UNSUPPORTED;
}
#endif

  do {
    Status = SmmFwBlockService->EraseBlocks (
                                  SmmFwBlockService,
                                  RomBaseAddress,
                                  (UINTN *) &WriteSize
                                  );
    if (!EFI_ERROR (Status)) {
      EraseCount = 0;
      Status = SmmFwBlockService->Write (
                                    SmmFwBlockService,
                                    RomBaseAddress,
                                    (UINTN *) &WriteSize,
                                    WriteDataBuffer
                                    );
      if (!EFI_ERROR (Status)) {
        *IhisiStatus = IhisiSuccess;

        return Status;
      } else {
        *IhisiStatus = FbtsWriteFail;
        WriteCount++;
      }
    } else {
      *IhisiStatus = FbtsEraseFail;
      EraseCount++;
    }
  } while ((EraseCount < 100) && (WriteCount < 100));

  return Status;
}


UINT32
FbtsLibStatusTranslation (
  IN EFI_STATUS                             Status
  )
/*++
Routine Description:

  EFI status translate to IHISI status.

Arguments:

  Status: EFI_STATUS

Returns:

  IHISI status

--*/
{
  switch (Status) {

  case EFI_SUCCESS:
    return (UINT32)IhisiSuccess;
    break;

  case EFI_BUFFER_TOO_SMALL:
    return (UINT32)IhisiObLenTooSmall;
    break;

  case EFI_UNSUPPORTED:
    return (UINT32)IhisiFunNotSupported;
    break;

  default:
    return (UINT32)FbtsPermissionDeny;
    break;
  }
}

EFI_STATUS
RelocateNextVariableName (
  IN OUT UINTN                  *VariableSize,
  IN OUT CHAR16                 **VariableName,
  IN OUT EFI_GUID               *VendorGuid,
  IN OUT UINTN                  *MaxVariableNameSize
  )
/*++
Routine Description:

  Use variable service to get the next variable. If the variable name size is not enough, re-allocate memory for it.
  return the memory buffer size to "MaxVariableNameSize".

Arguments:

  VariableSize              The variable size.
  VariableName              The varialbe name.
  VendorGuid                The variable GUID.
  MaxVariableNameSize       The max variable name size. Will allocate memory according to this size.

Returns:

  EFI_INVALID_PARAMETER         Invalid parameters
  EFI_OUT_OF_RESOURCES          Not enough memory
  EFI_SUCCESS                   Successfully

--*/
{
  UINTN           NextVariableSize;
  CHAR16          *NextVariableName;
  EFI_GUID        NextVendorGuid;
  EFI_STATUS      Status;
  CHAR16          *NeWVariableName;

  NeWVariableName = NULL;

  if (VariableSize == NULL || VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NextVariableName = *VariableName;
  EfiCopyMem (&NextVendorGuid, VendorGuid, sizeof (EFI_GUID));
  if (*VariableName == NULL) {
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, *MaxVariableNameSize, &NextVariableName);
    if (NextVariableName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    EfiSetMem (NextVariableName, *MaxVariableNameSize, 0);
  }

  NextVariableSize = *MaxVariableNameSize;
  Status = mSmmVariable->GetNextVariableName (&NextVariableSize, NextVariableName, &NextVendorGuid);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, NextVariableSize, &NeWVariableName);
    if (NeWVariableName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    EfiCopyMem (NeWVariableName, NextVariableName, *MaxVariableNameSize);
    Status = mSmst->SmmFreePool (NextVariableName);
    NextVariableName = NeWVariableName;

    *MaxVariableNameSize = NextVariableSize;
    Status = mSmmVariable->GetNextVariableName (&NextVariableSize, NextVariableName, &NextVendorGuid);
  }

  if (!EFI_ERROR (Status)) {
    *VariableSize = NextVariableSize;
    *VariableName = NextVariableName;
    EfiCopyMem (VendorGuid, &NextVendorGuid, sizeof (EFI_GUID));
  }

  return Status;
}

BOOLEAN
CheckVariableDelete (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   VendorGuid,
  IN PRESERVED_VARIABLE_TABLE   *VariablePreservedTablePtr,
  IN BOOLEAN                    IsKeepVariableInList
  )
/*++
Routine Description:

  Check if this variable should be deleted. Search from the preserved list.

Arguments:

  VariableName               The variable name.
  VendorGuid                 The varialbe GUID.
  VariablePreservedTablePtr  The preserved table.
  IsKeepVariableInList       This flag determines the property of the preserved table.

Returns:

  TRUE   This variable should be delteted
  FALSE  Keep this variable

--*/
{
  UINTN                         Index;
  BOOLEAN                       IsVariableFound;


  //
  // According to logo requirement, should preserve all UEFI variables with VendorGuid
  // {77fa9abd-0359-4d32-bd60-28f4e78f784b}
  //
  if (EfiCompareGuid (&VendorGuid, &mDefaultPreservedVendorGuid)) {
    return FALSE;
  }
  //
  // Check if the variable is in the preserved list or not.
  //
  Index = 0;
  IsVariableFound = FALSE;
  while (VariablePreservedTablePtr[Index].VariableName != NULL) {
    if ((EfiCompareGuid (&VendorGuid, &VariablePreservedTablePtr[Index].VendorGuid)) &&
        (EfiStrCmp (VariableName, VariablePreservedTablePtr[Index].VariableName) == 0)) {
      IsVariableFound = TRUE;
      break;
    } else {
      Index++;
    }
  }

  //
  //  IsKeepVariableInList | IsVariableFound | result
  // ------------------------------------------------
  //         TRUE          |      TRUE       | Keep
  //         TRUE          |      FALSE      | Delete
  //         FALSE         |      TRUE       | Delete
  //         FALSE         |      FALSE      | Keep
  //
  if (IsKeepVariableInList != IsVariableFound) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
PurifyVariable (
  VOID
  )
/*++
Routine Description:

  Purify the variables according to the table from OemService COMMON_VARIABLE_PRESERVED_TABLE.

Arguments:

  None

Returns:

  EFI_NOT_FOUND         Cannot locate gEfiSmmVariableProtocolGuid protocol
  EFI_OUT_OF_RESOURCES  Not enough memory
  EFI_SUCCESS           Purify variables if needed

--*/
{
  EFI_STATUS                    Status;
  EFI_STATUS                    VariableStatus;
  UINTN                         VariableNameSize;
  UINTN                         MaxVariableNameSize;
  CHAR16                        *VariableName;
  CHAR16                        *NextVariableName;
  CHAR16                        *NeWVariableName;
  UINTN                         NextVariableNameSize;
  EFI_GUID                      VendorGuid;
  EFI_GUID                      NextVendorGuid;

  //
  // Get the preserved variable table.
  //
  if (FbtsLibGetPreservedTable () != EFI_SUCCESS) {
    return EFI_NOT_FOUND;
  }

  if (mPreservedTableList == NULL) {
    //
    // This OemService does not exist, so do nothing.
    //
    return EFI_SUCCESS;
  }

  if ((!mIsKeepVariableInList) && (mPreservedTableList->VariableName == NULL)) {
    //
    // Clear an empty table, so do nothing.
    //
    return EFI_SUCCESS;
  }

  Status = mSmmRT->LocateProtocol (
                     &gEfiSmmVariableProtocolGuid,
                     NULL,
                     &mSmmVariable
                     );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Initialize the memory of variable name.
  //
  VariableName = NULL;
  NextVariableName = NULL;
  MaxVariableNameSize = DEFAULT_VARIABLE_NAME_SIZE;
  VariableStatus = RelocateNextVariableName (&VariableNameSize, &VariableName, &VendorGuid, &MaxVariableNameSize);

  while (!EFI_ERROR (VariableStatus)) {
    VariableStatus = RelocateNextVariableName (&NextVariableNameSize, &NextVariableName, &NextVendorGuid, &MaxVariableNameSize);
    if (VariableStatus == EFI_OUT_OF_RESOURCES) {
      Status = EFI_OUT_OF_RESOURCES;
      goto Done;
    }

    if (CheckVariableDelete (VariableName, VendorGuid, mPreservedTableList, mIsKeepVariableInList)) {
      Status = mSmmVariable->SetVariable (VariableName, &VendorGuid, 0, 0, NULL);
    }

    if (VariableNameSize < MaxVariableNameSize) {
      Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, MaxVariableNameSize, &NeWVariableName);
      if (EFI_ERROR (Status)) {
        Status = EFI_OUT_OF_RESOURCES;
        goto Done;
      }
      EfiCopyMem (NeWVariableName, VariableName, VariableNameSize);
      Status = mSmst->SmmFreePool (VariableName);
      VariableName = NeWVariableName;

      VariableNameSize = MaxVariableNameSize;
    }
    EfiCopyMem (VariableName, NextVariableName, NextVariableNameSize);
    EfiCopyMem (&VendorGuid, &NextVendorGuid, sizeof (EFI_GUID));
  }
  Status = EFI_SUCCESS;

Done:

  if (VariableName != NULL) {
    mSmst->SmmFreePool (VariableName);
  }

  if (NextVariableName != NULL) {
    mSmst->SmmFreePool (NextVariableName);
  }

  return Status;
}

EFI_STATUS
FbtsLibComplete (
  VOID
  )
{
  EFI_STATUS   Status;

  Status = PurifyVariable ();

  return Status;
}

EFI_STATUS
FbtsLibGetPreservedTable (
  VOID
  )
{
  SMM_OEM_SERVICES_PROTOCOL         *OemServices;
  EFI_STATUS                        Status;

  Status = mSmmRT->LocateProtocol (&gSmmOemServicesProtocolGuid, NULL, &OemServices);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }


  OemServices->Funcs[COMMON_VARIABLE_PRESERVED_TABLE] (
                 OemServices,
                 COMMON_VARIABLE_PRESERVED_TABLE_ARG_COUNT,
                 &mPreservedTableList,
                 &mIsKeepVariableInList
                 );

  return EFI_SUCCESS;
}

