/** @file
  FbtsLib Library Instance implementation

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

#include <Library/IhisiLib.h>
#include <Library/SmmOemSvcKernelLib.h>
#include <Library/MemoryAllocationLib.h>


#define DEFAULT_VARIABLE_NAME_SIZE    0x50

extern BOOLEAN mInPOST;
EFI_GUID       mDefaultPreservedVendorGuid = { 0x77fa9abd, 0x0359, 0x4d32, 0xbd, 0x60, 0x28, 0xf4, 0xe7, 0x8f, 0x78, 0x4b };
/**
  Get platform information.(AH=11h)

  @param[out] IhisiStatus    Buffer to save secure boot settings.

  @retval EFI_SUCCESS        FBTS get platform information success.
  @return Others             FBTS get platform information failed.
**/
EFI_STATUS
FbtsLibGetPlatformInfo (
  OUT    UINT32                             *IhisiStatus
  )
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
  PlatformInfoPtr = (FBTS_PLATFORM_INFO_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) PlatformInfoPtr, sizeof (FBTS_PLATFORM_INFO_BUFFER))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  ZeroMem (PlatformInfoPtr, sizeof (FBTS_PLATFORM_INFO_BUFFER));

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

/**
  Get Platform ROM map protection.(AH=12h)

  @param[in]  RomMapBufferPtr         Pointer to returned data buffer (ROM map).
  @param[in]  PrivateRomMapBufferPtr  Pointer to returned data buffer (Private map).
  @param[out] IhisiStatus             Return IHISI status.

  @retval EFI_SUCCESS                 FBTS get platform ROM map success.
  @return Others                      FBTS get platform ROM map failed.
**/
EFI_STATUS
FbtsLibGetPlatformRomMap (
  IN     FBTS_PLATFORM_ROM_MAP_BUFFER       *RomMapBufferPtr,
  IN     FBTS_PLATFORM_PRIVATE_ROM_BUFFER   *PrivateRomMapBufferPtr,
  OUT    UINT32                             *IhisiStatus
  )
{
  FBTS_PLATFORM_ROM_MAP_BUFFER              *InputRomMapBuffer;
  FBTS_PLATFORM_PRIVATE_ROM_BUFFER          *InputPrivateRomMapBuffer;
  EFI_STATUS                                Status;

  //
  // Get ROM map protection structure.
  //
  InputRomMapBuffer = (FBTS_PLATFORM_ROM_MAP_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
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
  InputPrivateRomMapBuffer = (FBTS_PLATFORM_PRIVATE_ROM_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) InputPrivateRomMapBuffer, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  CopyMem (InputRomMapBuffer, RomMapBufferPtr, sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER));
  CopyMem (InputPrivateRomMapBuffer, PrivateRomMapBufferPtr, sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER));

  *IhisiStatus = IhisiSuccess;

  return EFI_SUCCESS;
}

/**
  Get default BIOS ROM map

  @param[out] BiosRomMap              Pointer to the returned (FBTS_INTERNAL_BIOS_ROM_MAP *) data
  @param[out] NumberOfRegions         The total number of regions in BiosRomMap

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
GetDefaultBiosRomMap (
  OUT FBTS_INTERNAL_BIOS_ROM_MAP    **BiosRomMap,
  OUT UINTN                         *NumberOfRegions
  )
{
  UINTN  Index;
  extern FBTS_INTERNAL_BIOS_ROM_MAP mDefaultBiosRomMap[];
  UINT32 NvStorageRegionSize;

  if ((BiosRomMap == NULL) || (NumberOfRegions == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

#define ADD_ROM_MAP_ENTRY(RegionType, RegionAddress, RegionSize, RegionAttr)        \
            mDefaultBiosRomMap[Index].Type = (UINT8)RegionType;                     \
            mDefaultBiosRomMap[Index].Address = RegionAddress;                      \
            mDefaultBiosRomMap[Index].Size = RegionSize;                            \
            mDefaultBiosRomMap[Index++].Attribute = RegionAttr;                     \
            Index = __COUNTER__;

#define ADD_ROM_MAP_ENTRY_FROM_PCD(RegionType, AddressPcd, SizePcd, RegionAttr)                 \
          if (PcdGet32(SizePcd) > 0) {                                                          \
            ADD_ROM_MAP_ENTRY(RegionType, PcdGet32(AddressPcd), PcdGet32(SizePcd), RegionAttr); \
          }

  Index = 0;
  ADD_ROM_MAP_ENTRY_FROM_PCD (FbtsRomMapDxe,          PcdFlashFvMainBase,               PcdFlashFvMainSize,               0);
  ADD_ROM_MAP_ENTRY_FROM_PCD (FbtsRomMapCpuMicrocode, PcdFlashNvStorageMicrocodeBase,   PcdFlashNvStorageMicrocodeSize,   0);
  ADD_ROM_MAP_ENTRY_FROM_PCD (FbtsRomMapDmiFru,       PcdFlashNvStorageDmiBase,         PcdFlashNvStorageDmiSize,         0);
  ADD_ROM_MAP_ENTRY_FROM_PCD (FbtsRomMapOemData,      PcdFlashNvStorageMsdmDataBase,    PcdFlashNvStorageMsdmDataSize,    0);
  ADD_ROM_MAP_ENTRY_FROM_PCD (FbtsRomMapOemData,      PcdFlashNvStorageBvdtBase,        PcdFlashNvStorageBvdtSize,        0);
  ADD_ROM_MAP_ENTRY_FROM_PCD (FbtsRomMapPei,          PcdFlashFvRecoveryBase,           PcdFlashFvRecoverySize,           0);

  NvStorageRegionSize = PcdGet32 (PcdFlashNvStorageVariableSize) + PcdGet32 (PcdFlashNvStorageFtwWorkingSize) +
                        PcdGet32 (PcdFlashNvStorageFtwSpareSize) + PcdGet32 (PcdFlashNvStorageFactoryCopySize);
  if (NvStorageRegionSize > 0) {
    ADD_ROM_MAP_ENTRY (FbtsRomMapNvStorage, PcdGet32 (PcdFlashNvStorageVariableBase), NvStorageRegionSize, 0);
  }

  //
  // End of ROM map
  //
  ADD_ROM_MAP_ENTRY (FbtsRomMapEos, 0, 0, 0);

  *BiosRomMap = (FBTS_INTERNAL_BIOS_ROM_MAP *)mDefaultBiosRomMap;
  *NumberOfRegions = Index;

  return EFI_SUCCESS;
}
//
// the mDefaultBiosRomMap is declared after GetDefaultRomMap() on purpose to
// ensure the array size of mDefaultBiosRomMap is enough, DO NOT move it to the top
//
FBTS_INTERNAL_BIOS_ROM_MAP mDefaultBiosRomMap[__COUNTER__];


/**
  Get whole BIOS ROM map.(AH=1Eh)

  @param[out] IhisiStatus             Return IHISI status.

  @retval EFI_SUCCESS                 FBTS get BIOS ROM map success.
  @return Others                      FBTS get BIOS ROM map failed.
**/
EFI_STATUS
FbtsLibGetWholeBiosRomMap (
  OUT    UINT32                             *IhisiStatus
  )
{
  FBTS_INTERNAL_BIOS_ROM_MAP                *BiosRomMap;
  UINTN                                     NumberOfRegions;
  EFI_STATUS                                Status;
  UINTN                                     RomMapSize;
  UINT8                                     *RomMapPtr;

  if (IhisiStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *IhisiStatus = FbtsUnknowPlatformRomMap;


  GetDefaultBiosRomMap ((FBTS_INTERNAL_BIOS_ROM_MAP **)&BiosRomMap, &NumberOfRegions);
  Status = OemSvcIhisiGetWholeBiosRomMap ((VOID **)&BiosRomMap, &NumberOfRegions);

  if (Status == EFI_SUCCESS) {
    *IhisiStatus = IhisiSuccess;
    return EFI_SUCCESS;
  }
  if (BiosRomMap[NumberOfRegions - 1].Type != FbtsRomMapEos) {
    return EFI_ABORTED;
  }

  RomMapPtr  = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  RomMapSize = NumberOfRegions * sizeof (FBTS_INTERNAL_BIOS_ROM_MAP);
  //
  // Check input address and whole input buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) RomMapPtr, RomMapSize)) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  //
  // Get ROM map protection structure.
  //
  CopyMem (
    (VOID *) RomMapPtr,
    (VOID *) BiosRomMap,
    RomMapSize
    );

  *IhisiStatus = IhisiSuccess;

  return EFI_SUCCESS;
}


/**
  Flash part information.(AH=13h)

  @param[in]  SmmFwBlockService  The EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] IhisiStatus        Return IHISI status

  @retval EFI_SUCCESS            FBTS get flash part information success.
  @return Others                 FBTS get flash part information failed.
**/
EFI_STATUS
FbtsLibGetFlashPartInfo (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  OUT    UINT32                             *IhisiStatus
  )
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
//  mSmmChipsetLibServices->SelectFlashInterface (IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX));

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FLASH_DEVICE),
                    (VOID **)&Buffer
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
  AsciiStrCpy (FlashDevice.VendorName, Buffer->DeviceInfo.VendorName);
  AsciiStrCpy (FlashDevice.DeviceName, Buffer->DeviceInfo.DeviceName);
  CopyMem ((VOID *) BlockMap, &(Buffer->DeviceInfo.BlockMap), sizeof (FD_BLOCK_MAP));
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
  FlashInfo     = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  FlashInfoSize = sizeof(FBTS_FLASH_DEVICE) - sizeof (FlashDevice.BlockMap);
  //
  // Check output address and whole output buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) FlashInfo , FlashInfoSize)) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  CopyMem (
    (VOID *) FlashInfo,
    &FlashDevice,
    FlashInfoSize
    );

  FlashBlockMap     = (UINT8 *) (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  //
  // Check output address and whole output buffer isn't located in SM RAM.
  //
  if (BufferOverlapSmram ((VOID *) FlashBlockMap, sizeof (FD_BLOCK_MAP))) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  CopyMem ((VOID *) FlashBlockMap, BlockMap, sizeof (FD_BLOCK_MAP));

  *IhisiStatus = IhisiSuccess;

  return EFI_SUCCESS;
}

/**
  FBTS Read.(AH=14h)

  @param[in]  SmmFwBlockService  The EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] IhisiStatus        Return IHISI status

  @retval EFI_SUCCESS            FBTS read success.
  @return Others                 FBTS read failed.
**/
EFI_STATUS
FbtsLibRead (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  OUT    UINT32                             *IhisiStatus
  )
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
  DataBuffer = (UINT8 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  Size       = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Address    = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
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

/**
  FBTS write.(AH=15h)

  @param[in]  SmmFwBlockService  The EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] IhisiStatus        Return IHISI status.

  @retval EFI_SUCCESS            FBTS write success.
  @return Others                 FBTS write failed.
**/
EFI_STATUS
FbtsLibWrite (
  IN     EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlockService,
  OUT    UINT32                             *IhisiStatus
  )
{
  EFI_STATUS                                Status;
  UINT8                                     *WriteDataBuffer;
  UINTN                                     WriteSize;
  UINTN                                     RomBaseAddress;
  UINT8                                     EraseCount;
  UINT8                                     WriteCount;
  UINTN                                     UnsignedRegionBase;
  BOOLEAN                                   InUnsignedRegion;

  //
  // ECX    - Size to write.
  // DS:ESI - Pointer to returned data buffer. Size in ECX.
  // EDI    - Target linear address to write.
  //
  WriteDataBuffer = (UINT8 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  WriteSize       = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  RomBaseAddress  = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  if (BufferOverlapSmram ((VOID *) WriteDataBuffer, WriteSize) || BufferOverlapSmram ((VOID *) RomBaseAddress, WriteSize)) {
    Status = EFI_UNSUPPORTED;
    *IhisiStatus = FbtsLibStatusTranslation (Status);
    return Status;
  }
  InUnsignedRegion = FALSE;

  if (PcdGetBool (PcdSecureFlashSupported)) {
    Status = SmmFwBlockService->ConvertToSpiAddress(
                                   SmmFwBlockService,
                                   (UINTN) PcdGet32 (PcdFlashUnsignedFvRegionBase),
                                   &UnsignedRegionBase
                                   );
    if (!EFI_ERROR (Status)) {
      //
      // Check Write address is in the Unsigned Region or not
      //
      if ((RomBaseAddress >= UnsignedRegionBase) && \
          ((RomBaseAddress + WriteSize) <= (UnsignedRegionBase + PcdGet32 (PcdEndOfFlashUnsignedFvRegionTotalSize)))) {
        InUnsignedRegion = TRUE;
      }
    }

    if (!mInPOST && !InUnsignedRegion) {
      *IhisiStatus = FbtsWriteFail;
      return EFI_UNSUPPORTED;
    }
  }


  EraseCount      = 0;
  WriteCount      = 0;

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

/**
  EFI status translate to IHISI status.

  @param[in] Status  EFI_STATUS

  @return UINT32     IHISI status
**/
UINT32
FbtsLibStatusTranslation (
  IN EFI_STATUS                             Status
  )
{
  UINT32        IhisiStatus;

  switch (Status) {

  case EFI_SUCCESS:
    IhisiStatus = IhisiSuccess;
    break;

  case EFI_BUFFER_TOO_SMALL:
    IhisiStatus = IhisiObLenTooSmall;
    break;

  case EFI_UNSUPPORTED:
    IhisiStatus = IhisiFunNotSupported;
    break;

  default:
    IhisiStatus = FbtsPermissionDeny;
    break;
  }

  return IhisiStatus;
}

BOOLEAN
IsZeroGuid (
  IN EFI_GUID                   *Guid
  )
{
  UINT8 *TempPtr;
  UINTN Index;

  TempPtr = (UINT8 *) Guid;

  for (Index = 0; Index < sizeof (EFI_GUID); Index++) {
    if (TempPtr[Index] != 0) {
      return FALSE;
    }
  }

  return TRUE;
}

UINTN
GetNumberOfVariable (
  IN UINT8                      *TablePtr
  )
{
  UINTN NumberOfVariable;


  NumberOfVariable = 0;
  while (!IsZeroGuid ((EFI_GUID *) TablePtr)) {
    TablePtr += sizeof (EFI_GUID);
    NumberOfVariable++;
    TablePtr += StrSize ((CHAR16 *) TablePtr);
  }

  //
  // Add one for the end of data
  //
  NumberOfVariable++;

  return NumberOfVariable;
}

/**
  Get the default preserved variable table from PCD.

  @param[out]  TablePtr  The pointer to the default table
**/
VOID
GetDefaultTable (
  OUT PRESERVED_VARIABLE_TABLE   **TablePtr
  )
{
  PRESERVED_VARIABLE_TABLE *TempTablePtr;
  UINTN                    NumberOfVariable;
  UINT8                    *StringPtr;
  UINTN                    Index;


  TempTablePtr = NULL;
  *TablePtr = (PRESERVED_VARIABLE_TABLE *) PcdGetPtr (PcdDefaultPreservedVariableList);

  if (!IsZeroGuid ((EFI_GUID *) *TablePtr)) {
    NumberOfVariable = GetNumberOfVariable ((UINT8 *) *TablePtr);
    gSmst->SmmAllocatePool (
             EfiRuntimeServicesData,
             NumberOfVariable * sizeof (PRESERVED_VARIABLE_TABLE),
             (VOID **)&TempTablePtr
             );

    StringPtr = (UINT8 *) *TablePtr;
    for (Index = 0; (!IsZeroGuid ((EFI_GUID *) StringPtr)); Index++) {
      CopyMem (&TempTablePtr[Index].VendorGuid, StringPtr, sizeof (EFI_GUID));

      StringPtr += sizeof (EFI_GUID);
      TempTablePtr[Index].VariableName = (CHAR16 *) StringPtr;
      //
      // Go to the next variable.
      //
      StringPtr += StrSize (TempTablePtr[Index].VariableName);
    }
  }

  *TablePtr = TempTablePtr;
}

BOOLEAN
IsVariableServiceSupported (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         VariableDataSize;

  Status = CommonGetVariable (NULL, NULL, &VariableDataSize, NULL);

  return Status == EFI_UNSUPPORTED ? FALSE : TRUE;
}

/**
  Check if this variable should be deleted. Search from the preserved list.

  @param[in]  VariableName               The variable name.
  @param[in]  VendorGuid                 The varialbe GUID.
  @param[in]  VariablePreservedTablePtr  The preserved table.
  @param[in]  IsKeepVariableInList       This flag determines the property of the preserved table.

  @retval TRUE                           This variable should be delteted
  @retval FALSE                          Keep this variable
**/
BOOLEAN
CheckVariableDelete (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid,
  IN PRESERVED_VARIABLE_TABLE   *VariablePreservedTablePtr,
  IN BOOLEAN                    IsKeepVariableInList
  )
{
  UINTN                         Index;
  BOOLEAN                       IsVariableFound;

  //
  // According to logo requirement, should preserve all UEFI variables with VendorGuid
  // {77fa9abd-0359-4d32-bd60-28f4e78f784b}
  //
  if (CompareGuid (VendorGuid, &mDefaultPreservedVendorGuid)) {
    return FALSE;
  }
  //
  // Check if the variable is in the preserved list or not.
  //
  Index = 0;
  IsVariableFound = FALSE;
  while (VariablePreservedTablePtr[Index].VariableName != NULL) {
    if ((CompareGuid (VendorGuid, &VariablePreservedTablePtr[Index].VendorGuid)) &&
        (StrCmp (VariableName, VariablePreservedTablePtr[Index].VariableName) == 0)) {
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

/**
  Use variable service to get the next variable. If the variable name size is not enough, re-allocate memory for it.
  return the memory buffer size to "MaxVariableNameSize".

  @param[in, out]  VariableSize         The variable size.
  @param[in, out]  VariableName         The varialbe name.
  @param[in, out]  VendorGuid           The variable GUID.
  @param[in, out]  MaxVariableNameSize  The max variable name size. Will allocate memory according to this size.

  @retval EFI_INVALID_PARAMETER         Invalid parameters
  @retval EFI_OUT_OF_RESOURCES          Not enough memory
  @retval EFI_SUCCESS                   Successfully
**/
EFI_STATUS
RelocateNextVariableName (
  IN OUT UINTN                  *VariableSize,
  IN OUT CHAR16                 **VariableName,
  IN OUT EFI_GUID               *VendorGuid,
  IN OUT UINTN                  *MaxVariableNameSize
  )
{
  UINTN           NextVariableSize;
  CHAR16          *NextVariableName;
  EFI_GUID        NextVendorGuid;
  EFI_STATUS      Status;

  if (VariableSize == NULL || VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NextVariableName = *VariableName;
  CopyGuid (&NextVendorGuid, VendorGuid);
  if (*VariableName == NULL) {
    NextVariableName = AllocateZeroPool (*MaxVariableNameSize);
    if (NextVariableName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  NextVariableSize = *MaxVariableNameSize;
  Status = CommonGetNextVariableName (&NextVariableSize, NextVariableName, &NextVendorGuid);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    NextVariableName = ReallocatePool (*MaxVariableNameSize, NextVariableSize, NextVariableName);
    if (NextVariableName == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    *MaxVariableNameSize = NextVariableSize;
    Status = CommonGetNextVariableName (&NextVariableSize, NextVariableName, &NextVendorGuid);
  }

  if (!EFI_ERROR (Status)) {
    *VariableSize = NextVariableSize;
    *VariableName = NextVariableName;
    CopyGuid (VendorGuid, &NextVendorGuid);
  }

  return Status;
}

/**
 Purify the variables if needed. If there is NO OemService "OemSvcVariablePreservedTable",
 do nothing in this function.

 @retval EFI_SUCCESS            Success
 @retval EFI_UNSUPPORTED        there is no SmmVariable service
 @retval EFI_BUFFER_TOO_SMALL   not enough memory

**/
EFI_STATUS
PurifyVariable (
  VOID
  )
{
  EFI_STATUS                    Status;
  CHAR16                        *VariableName;
  UINTN                         VariableNameSize;
  EFI_GUID                      VendorGuid;
  CHAR16                        *NextVariableName;
  UINTN                         NextVariableNameSize;
  EFI_GUID                      NextVendorGuid;
  UINTN                         MaxVariableNameSize;
  BOOLEAN                       IsKeepVariableInList;
  PRESERVED_VARIABLE_TABLE      *VariablePreservedTablePtr;


  if (!IsVariableServiceSupported ()) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get the default preserved variable table from PCD.
  //
  GetDefaultTable (&VariablePreservedTablePtr);
  IsKeepVariableInList = FeaturePcdGet (PcdKeepVariableInList);
  //
  // Get variable preserved table from OemServices
  //
  Status = OemSvcVariablePreservedTable (
             &VariablePreservedTablePtr,
             &IsKeepVariableInList
             );
  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  if (VariablePreservedTablePtr == NULL) {
    //
    // This OemService does not exist, so do nothing.
    //
    return EFI_SUCCESS;
  }

  if ((!IsKeepVariableInList) && (VariablePreservedTablePtr->VariableName == NULL)) {
    //
    // Clear an empty table, so do nothing.
    //
    return EFI_SUCCESS;
  }

  VariableName = NULL;
  NextVariableName = NULL;
  MaxVariableNameSize = DEFAULT_VARIABLE_NAME_SIZE;
  Status = RelocateNextVariableName (&VariableNameSize, &VariableName, &VendorGuid, &MaxVariableNameSize);

  while (!EFI_ERROR (Status)) {
    Status = RelocateNextVariableName (&NextVariableNameSize, &NextVariableName, &NextVendorGuid, &MaxVariableNameSize);
    if (Status == EFI_OUT_OF_RESOURCES) {
      goto Done;
    }

    if (CheckVariableDelete (VariableName, &VendorGuid, VariablePreservedTablePtr, IsKeepVariableInList)) {
      CommonSetVariable (VariableName, &VendorGuid, 0, 0, NULL);
    }

    if (VariableNameSize < MaxVariableNameSize) {
      VariableName = ReallocatePool (VariableNameSize, MaxVariableNameSize, VariableName);
      VariableNameSize = MaxVariableNameSize;
    }
    CopyMem (VariableName, NextVariableName, NextVariableNameSize);
    CopyGuid (&VendorGuid, &NextVendorGuid);
  }
  Status = EFI_SUCCESS;

Done:

  if (VariableName != NULL) {
    gSmst->SmmFreePool (VariableName);
  }

  if (NextVariableName != NULL) {
    gSmst->SmmFreePool (NextVariableName);
  }

  return Status;
}

/**
  FBTS Complete.(AH=16h)

  @retval EFI_SUCCESS            FBTS Complete success.
  @return Others                 FBTS Complete failed.
**/
EFI_STATUS
FbtsLibComplete (
  VOID
  )
{
  EFI_STATUS   Status;

  //
  // After flash process, clear variables by using OemService.
  // Because Flash utility will skip NV region when Secure Boot support.
  //
  Status = PurifyVariable ();

  return Status;
}

