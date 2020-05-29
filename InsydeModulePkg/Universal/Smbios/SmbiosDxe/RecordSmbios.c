/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Protocol/PiPcd.h>
#include <Protocol/Smbios.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Guid/DebugMask.h>

#include "SmbiosDxe.h"

#define MAX_BUFFER_LENGTH                  SMBIOS_TABLE_MAX_LENGTH
#define MULTI_RECORDS_TOKEN_OFFSET         8
#define FixedPcdGetPtrSize(TokenName)      (UINTN)_PCD_PATCHABLE_##TokenName##_SIZE

STATIC UINT8                   *DmiStart   = NULL;
STATIC UINT8                   *DmiEnd     = NULL;
STATIC UINT8                   *VarStart   = NULL;
STATIC UINT8                   *VarEnd     = NULL;
STATIC DMI_UPDATABLE_STRING    *PnpTable   = NULL;
STATIC UINTN                    TableCount = 0;
STATIC EFI_PCD_PROTOCOL        *Pcd        = NULL;

extern SMBIOS_INSTANCE mPrivateData;
extern BOOLEAN AlreadyExist[256];
extern BOOLEAN EndOfSmbiosScm;

typedef struct _SMBIOS_DATA_INFO {
  UINTN        Length;
  CHAR8       *Data;
} SMBIOS_DATA_INFO;

typedef struct {
  UINT8        Type;
  UINT8        OffsetCount;
  UINT8        Offset[1];
} STRING_OFFSET_TABLE;


EFI_STATUS
EFIAPI
SmbiosAdd (
  IN CONST EFI_SMBIOS_PROTOCOL  *This,
  IN EFI_HANDLE                 ProducerHandle, OPTIONAL
  IN OUT EFI_SMBIOS_HANDLE      *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  );

/**

  Count number of strings in a string array.

  @param[IN]  StringArray    A string contain a string array.
                             Each element must end by ';'

  @retval     Integer        The number of strings. If StringArray is NULL will return zero.

**/
STATIC
UINTN
ParseStringArray (
  IN   CHAR8 *StringArray
  )
{
  CHAR8   *Ptr;
  UINTN    StrCount;

  if (StringArray == NULL) {
    return 0;
  }

  StrCount = 0;
  Ptr = StringArray;
  while(*Ptr != '\0') {
    if (*Ptr == ';') {
      ++StrCount;
    }
    ++Ptr;
  }

  return StrCount;
}

/**

  Retrieve string from a string array.

  @param[IN]   PcdString      A string contain a string array.
                              Each element must end by ';'.
  @param[IN]   Index          Which string want to retrieve.
  @param[OUT]  StrInfo        The information of retrieve string.

  @retval      EFI_SUCCESS              Get string from string array successfully.
  @retval      EFI_INVALID_PARAMETER    The index big then element number of string array.

**/
STATIC
EFI_STATUS
GetStringFromPcd (
  IN  CHAR8            *PcdString,
  IN  UINTN             Index,
  OUT SMBIOS_DATA_INFO *DataInfo
  )
{
  CHAR8   *Ptr;

  if (Index > ParseStringArray(PcdString)) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr = PcdString;
  while (Index > 0) {
    while (*Ptr++ != ';');
    --Index;
  }

  DataInfo->Data   = Ptr;
  DataInfo->Length = 0;

   while(*Ptr != ';' && *Ptr != '\0') {
    ++DataInfo->Length;
    ++Ptr;
  }

  return EFI_SUCCESS;
}

/**

  Retrieve data from DMI region or PNP variable.

  @param[IN]   TYPE           SMBIOS type number.
  @param[IN]   Offset         The offset of field in SMBIOS structure.
  @param[OUT]  Info           Return data length and data point.

  @retval          none

**/
STATIC
VOID
GetReplaceDataFromDmi (
  IN   UINT8             Type,
  IN   UINT8             Offset,
  OUT  SMBIOS_DATA_INFO *Info
  )
{
  DMI_STRING_STRUCTURE   *DmiEntry;

  if (DmiStart == NULL && VarStart == NULL) {
    return;
  }

  if (DmiStart != NULL) {
    DmiEntry = (DMI_STRING_STRUCTURE *)DmiStart;
    while ((UINT8 *)DmiEntry < DmiEnd && DmiEntry->Type != 0xFF && DmiEntry->Length != 0) {
      if (DmiEntry->Valid == 0xFF && DmiEntry->Type == Type && DmiEntry->Offset == Offset) {
        Info->Data   = (CHAR8 *)DmiEntry->String;
        Info->Length = DmiEntry->Length - sizeof(DMI_STRING_STRUCTURE) + sizeof(UINT8);
        break;
      }

      DmiEntry = (DMI_STRING_STRUCTURE *)((CHAR8 *)DmiEntry + DmiEntry->Length);
    }
  }


  if (VarStart != NULL) {
    DmiEntry = (DMI_STRING_STRUCTURE *)VarStart;
    while ((UINT8 *)DmiEntry < VarEnd && DmiEntry->Type != 0xFF && DmiEntry->Length != 0) {
      if (DmiEntry->Valid == 0xFF && DmiEntry->Type == Type && DmiEntry->Offset == Offset) {
        Info->Data   = (CHAR8 *)DmiEntry->String;
        Info->Length = DmiEntry->Length - sizeof(DMI_STRING_STRUCTURE) + sizeof(UINT8);
        break;
      }

      DmiEntry = (DMI_STRING_STRUCTURE *)((CHAR8 *)DmiEntry + DmiEntry->Length);
    }
  }

  return;
}

/**

  Get PNP information from DMI region and PNP variable.
  The information will be store to global variable.

  @param    VOID

  @retval   VOID

**/
STATIC
VOID
GetPnpRegion (
  VOID
  )
{
  UINTN                   VarSize;
  EFI_STATUS              Status;

  DmiStart = (UINT8*)(UINTN)(PcdGet32 (PcdFlashNvStorageDmiBase));
  if (*(UINT32*)DmiStart != DMI_UPDATE_STRING_SIGNATURE) {
    DEBUG((EFI_D_ERROR, "DMI region signature error!!\n"));
    DmiStart = NULL;
  } else {
    DmiEnd = DmiStart + PcdGet32 (PcdFlashNvStorageDmiSize);
    DmiStart += sizeof (DMI_UPDATE_STRING_SIGNATURE);
  }


  Status = GetVariable2 (L"PnpRuntime", &gEfiGenericVariableGuid, (VOID **)&VarStart, &VarSize);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Get Variable PnpRuntime Fail - %r!!\n", Status));

    if (VarStart != NULL) {
      FreePool(VarStart);
      VarStart = NULL;
    }
  } else if (VarStart != NULL) {
    if (*(UINT32 *)VarStart != DMI_UPDATE_STRING_SIGNATURE) {
      DEBUG((EFI_D_ERROR, "Variable PnpRuntime signature error!!\n"));
      FreePool(VarStart);
      VarStart = NULL;
    } else {
      VarEnd = VarStart + VarSize;
      VarStart += sizeof (DMI_UPDATE_STRING_SIGNATURE);
    }
  }

  return;
}

/**

  Dump record data for debug

  @param[IN]   Record           SMBIOS record data.

  @retval      VOID

**/
STATIC
VOID
DumpRecordData (
  IN EFI_SMBIOS_TABLE_HEADER *Record
)
{
DEBUG_CODE_BEGIN();

  UINTN  Index;
  UINT8 *Ptr;

  Ptr = (UINT8 *)Record;

  DEBUG((EFI_D_INFO, "Type   : %03d\n", Record->Type));
  DEBUG((EFI_D_INFO, "Length : 0x%x\n", Record->Length));
  DEBUG((EFI_D_INFO, "Handle : 0x%x\n", Record->Handle));

  DEBUG((EFI_D_INFO, "Record Data :\n"));
  for (Index = 0; Index < Record->Length; ++Index) {
    if ((Index & 0x0F) == 0) {
      DEBUG((EFI_D_INFO, "  0x%04x : ", Index));
    }

    DEBUG((EFI_D_INFO, "%02x ", Ptr[Index]));

    if ((Index & 0x0F) == 0x0F) {
      DEBUG((EFI_D_INFO, "\n"));
    }
  }
  DEBUG((EFI_D_INFO, "\n"));

  Ptr += Record->Length;

  DEBUG((EFI_D_INFO, "String :\n"));
  if (*Ptr == 0 && *(Ptr + 1) == 0) {
    DEBUG((EFI_D_INFO, "  No String !!\n"));
  } else {
    Index = 1;
    while (*Ptr != 0 && *(Ptr + 1) != 0) {
      DEBUG((EFI_D_INFO, "  %02d : %a\n", Index++, Ptr));
      Ptr += AsciiStrLen ((CHAR8 *)Ptr) + 1;
    }
    DEBUG((EFI_D_INFO, "\n"));
  }

DEBUG_CODE_END();

  return;
}


/**

  record SMBIOS to system.

  @param[IN]   TokenOffset        PCD token offset.

  @retval      VOID

**/
STATIC
VOID
RecordEachType (
  IN UINTN     TokenOffset
  )
{
  EFI_SMBIOS_TABLE_HEADER *Hdr;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  UINT8                   *Buffer;
  UINT8                   *BufEnd;
  UINT8                   *BufPtr;
  UINTN                    StrCount;
  UINTN                    StrIndex;
  UINTN                    PnpIndex;
  EFI_STATUS               Status;
  UINT8                   *RecordData;
  CHAR8                   *RecordStrings;
  UINTN                    RecordStrLen;
  UINTN                    RecordLength;
  SMBIOS_DATA_INFO         DataInfo;
  UINT8                    StrNumber;
  STRING_OFFSET_TABLE     *OffsetTable;
  STRING_OFFSET_TABLE     *OffsetTableEnd;
  BOOLEAN                  DataUpdate;

  RecordData    = Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Record) + TokenOffset);
  RecordLength  = Pcd->GetSize(&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Record) + TokenOffset);
  if (RecordLength == 1 && RecordData[0] == 0) {
    //
    //  The record is empty.
    //  Do nothing, Just return.
    //
    return;
  }

  Buffer = (UINT8 *)AllocateZeroPool (MAX_BUFFER_LENGTH);
  if (Buffer == NULL) {
    DEBUG((EFI_D_ERROR, "Allocate memory for SMBIOS buffer FAIL!!\n"));
    return;
  }

  CopyMem (Buffer, RecordData, RecordLength);

  Hdr = (EFI_SMBIOS_TABLE_HEADER *)Buffer;
  Hdr->Length = (UINT8)RecordLength;
  Hdr->Handle = 0;

  //
  // Format of OffsetTable
  //
  // Type  Number String Offset_1 offset_2 .....
  // ====  ============= ======== ======== .....
  // 0x00, 0x03          0x04,    0x05,    0x08
  // 0x01, 0x06          0x04,    0x05,    0x06, 0x07, 0x19, 0x1A
  // ....................
  //
  OffsetTable    = (STRING_OFFSET_TABLE *)Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken(PcdSmbiosStringFieldOffset));
  OffsetTableEnd = (STRING_OFFSET_TABLE *)((CHAR8 *)OffsetTable + Pcd->GetSize(&gSmbiosTokenSpaceGuid, PcdToken(PcdSmbiosStringFieldOffset)));

  while (OffsetTable < OffsetTableEnd) {
    if (OffsetTable->Type == Hdr->Type) {
      break;
    }

    OffsetTable = (STRING_OFFSET_TABLE *)((CHAR8 *)OffsetTable + sizeof(STRING_OFFSET_TABLE) + OffsetTable->OffsetCount - sizeof(UINT8));
  }

  if (OffsetTable >= OffsetTableEnd) {
    OffsetTable = NULL;
  }


  if (PnpTable != NULL && (DmiStart != NULL || VarStart != NULL)) {
    for (PnpIndex = 0; PnpIndex < TableCount; ++PnpIndex) {
      if (PnpTable[PnpIndex].Type == Hdr->Type) {
        DataUpdate = TRUE;
        if (OffsetTable != NULL) {
          for (StrIndex = 0; StrIndex < OffsetTable->OffsetCount; ++StrIndex) {
            if (PnpTable[PnpIndex].FixedOffset == OffsetTable->Offset[StrIndex]) {
              DataUpdate = FALSE;
              break;
            }
          }
        }

        if (DataUpdate) {
          ZeroMem(&DataInfo, sizeof(SMBIOS_DATA_INFO));
          GetReplaceDataFromDmi(Hdr->Type, PnpTable[PnpIndex].FixedOffset, &DataInfo);

          if (DataInfo.Length > 0) {
            CopyMem(Buffer + PnpTable[PnpIndex].FixedOffset, DataInfo.Data, DataInfo.Length);
          }
        }
      }
    }
  }

  BufPtr = Buffer + RecordLength;
  BufEnd = Buffer + MAX_BUFFER_LENGTH;


  RecordStrings = Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Strings) + TokenOffset);
  RecordStrLen  = Pcd->GetSize(&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Strings) + TokenOffset);

  if (RecordStrings[0] != '\0') {
    StrNumber = 1;
    StrCount  = ParseStringArray(RecordStrings);

    for (StrIndex = 0; StrIndex < StrCount; ++StrIndex) {
      ZeroMem(&DataInfo, sizeof(SMBIOS_DATA_INFO));

      Status = GetStringFromPcd(RecordStrings, StrIndex, &DataInfo);
      if (EFI_ERROR(Status)) {
        break;
      }

      if (OffsetTable != NULL) {
        Status = OemSvcDxeGetSmbiosReplaceString(Hdr->Type, OffsetTable->Offset[StrIndex], &DataInfo.Length, &DataInfo.Data);
        if (Status == EFI_UNSUPPORTED && PnpTable != NULL && (DmiStart != NULL || VarStart != NULL)) {
          for (PnpIndex = 0; PnpIndex < TableCount; ++PnpIndex) {
            if (PnpTable[PnpIndex].Type == Hdr->Type && PnpTable[PnpIndex].FixedOffset == OffsetTable->Offset[StrIndex]) {
              GetReplaceDataFromDmi(Hdr->Type, OffsetTable->Offset[StrIndex], &DataInfo);
              break;
            }
          }
        }
      }

      if (DataInfo.Length > 0) {
        if (BufPtr + DataInfo.Length + 2 <= BufEnd) {
          if (OffsetTable != NULL) {
            Buffer[OffsetTable->Offset[StrIndex]] = StrNumber++;
          }

          CopyMem (BufPtr, DataInfo.Data, DataInfo.Length);
          BufPtr += DataInfo.Length + 1;
        } else {
          DEBUG((EFI_D_ERROR, "The length of this structure table(Type %d) is too big.\n", Hdr->Type));
          DEBUG((EFI_D_ERROR, "So this table(Type %d) will not add to system.\n", Hdr->Type));
          FreePool(Buffer);
          return;
        }
      } else if (OffsetTable != NULL) {
        Buffer[OffsetTable->Offset[StrIndex]] = 0;
      }
    }
  }

  //
  //  Transfer record buffer to OEM service, So OEM have chance to midify and verify record data
  //  before add reocrd to the system.
  //
  Status = OemSvcDxeUpdateSmbiosRecord((EFI_SMBIOS_TABLE_HEADER *)Buffer);
  DEBUG((EFI_D_INFO, "The return status is %r from OEM service update SMBIOS type %d.\n", Status, Hdr->Type));

  //
  //  if return EFI_SUCCESS mean OEM service don't want to add this SMBIOS record to system
  //
  if (Status != EFI_SUCCESS) {
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = SmbiosAdd(&mPrivateData.Smbios, NULL, &SmbiosHandle, Hdr);

DEBUG_CODE_BEGIN();
    AlreadyExist[Hdr->Type] = TRUE;

    DEBUG((EFI_D_INFO, "The return status is %r for store SMBIOS type %d.\n", Status, Hdr->Type));
    DumpRecordData (Hdr);
DEBUG_CODE_END();
  }

  FreePool(Buffer);

  return;
}

/**

  Get type0's string from string PCD or DMI region.

  @param[in]   FieldOffset   the offset of field in the Type0 structure
  @param[in]   StrIndex      The index of string in PcdType000Strings
  @param[in]   BufPtr        Point where to copy new string
  @param[in]   MaxBuf        The buffer's last point


  @retval      EFI_SUCCESS            Find the string and copy to BufPtr successfully
               EFI_BUFFER_TOO_SMALL   The buffer to small

**/
STATIC
EFI_STATUS
GetStringForType0 (
  IN UINT8                    FieldOffset,
  IN UINT8                    StrIndex,
  IN UINT8                   *BufPtr,
  IN UINT8                   *BufEnd
)
{
  EFI_STATUS             Status;
  UINTN                  Index;
  CHAR8                 *RecordStr;
  SMBIOS_DATA_INFO       DataInfo;

  ZeroMem(&DataInfo, sizeof(SMBIOS_DATA_INFO));

  RecordStr = Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Strings));

  Status = GetStringFromPcd(RecordStr, StrIndex, &DataInfo);
  if (EFI_ERROR(Status)) {
     return Status;
  }

  Status = OemSvcDxeGetSmbiosReplaceString((EFI_SMBIOS_TYPE)EFI_SMBIOS_TYPE_BIOS_INFORMATION, FieldOffset, &DataInfo.Length, &DataInfo.Data);
  if (Status == EFI_UNSUPPORTED && PnpTable != NULL && (DmiStart != NULL || VarStart != NULL)) {
    for (Index = 0; Index < TableCount; ++Index) {
      if (PnpTable[Index].Type == EFI_SMBIOS_TYPE_BIOS_INFORMATION &&
          PnpTable[Index].FixedOffset == FieldOffset) {
        GetReplaceDataFromDmi (EFI_SMBIOS_TYPE_BIOS_INFORMATION, FieldOffset, &DataInfo);
        break;
      }
    }
  }

  if (DataInfo.Length > 0) {
    if (BufPtr + DataInfo.Length + 2 <= BufEnd) {
      CopyMem (BufPtr, DataInfo.Data, DataInfo.Length);
      BufPtr += DataInfo.Length + 1;
    } else {
      DEBUG((EFI_D_ERROR, "The length of this structure table(Type 0) is too big.\n"));
      DEBUG((EFI_D_ERROR, "So this table(Type 0) will not to be add to system.\n"));

      return EFI_BUFFER_TOO_SMALL;
    }
  }

  return EFI_SUCCESS;
}

/**

  record SMBIOS Type 0 to system.

  @param       VOID

  @retval      VOID

**/
STATIC
VOID
RecordType0 (
  VOID
  )
{
  SMBIOS_TABLE_TYPE0      *Type0;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  UINT8                   *Buffer;
  UINT8                   *BufEnd;
  UINT8                   *BufPtr;
  UINT8                   *Ptr;
  UINT16                  *VersionStr;
  UINT8                    Index;
  UINT8                   *RecordData;
  UINTN                    RecordLength;
  EFI_STATUS               Status;

  RecordData    = Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Record));
  RecordLength  = Pcd->GetSize(&gSmbiosTokenSpaceGuid, PcdToken(PcdType000Record));

  if (RecordLength == 1 && RecordData[0] == 0) {
    //
    //  The record is empty.
    //  Do nothing, Just return.
    //
    return;
  }

  Buffer = (UINT8 *)AllocateZeroPool (MAX_BUFFER_LENGTH);
  if (Buffer == NULL) {
    DEBUG((EFI_D_ERROR, "Allocate memory for SMBIOS buffer FAIL!!\n"));
    return;
  }

  CopyMem (Buffer, RecordData, RecordLength);

  Type0 = (SMBIOS_TABLE_TYPE0 *)Buffer;
  Type0->Hdr.Type   = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Type0->Hdr.Length = (UINT8)RecordLength;
  Type0->Hdr.Handle = 0;

  if (Type0->BiosSize == 0xFF) {
    Type0->BiosSize = (UINT8)((PcdGet32(PcdFlashAreaSize) >> 16) - 1);
  }

  BufPtr = Buffer + RecordLength;
  BufEnd = Buffer + MAX_BUFFER_LENGTH;

  Index = 1;
  if (Type0->Vendor == 0xFF) {
    UnicodeStrToAsciiStr ((CHAR16 *)PcdGetPtr(PcdFirmwareVendor), BufPtr);
  } else {
    Status = GetStringForType0(OFFSET_OF(SMBIOS_TABLE_TYPE0, Vendor), (UINT8)Type0->Vendor - 1, BufPtr, BufEnd);
    if (EFI_ERROR(Status)) {
      return;
    }
  }

  if (AsciiStrLen(BufPtr) > 0) {
    Type0->Vendor = (SMBIOS_TABLE_STRING)Index++;
    BufPtr += AsciiStrLen(BufPtr) + 1;
  } else {
    Type0->Vendor = 0;
  }

  if (Type0->BiosVersion == 0xFF) {
    //
    // BIOS Version UniString format is 'board_name.AB.CD.EF' or 'board_name.A.BC.DE'
    //
    VersionStr = (CHAR16 *)PcdGetPtr(PcdFirmwareVersionString);
    while (*VersionStr++ != L'.');

    UnicodeStrToAsciiStr (VersionStr, BufPtr);
  } else {
    Status = GetStringForType0(OFFSET_OF(SMBIOS_TABLE_TYPE0, BiosVersion), (UINT8)Type0->BiosVersion - 1, BufPtr, BufEnd);
    if (EFI_ERROR(Status)) {
      return;
    }
  }

  if (AsciiStrLen(BufPtr) > 0) {
    if (Type0->SystemBiosMajorRelease == 0xFF) {
      Ptr = BufPtr;
      Type0->SystemBiosMajorRelease = 0;
      while (*Ptr != '.') {
        Type0->SystemBiosMajorRelease *= 10;
        Type0->SystemBiosMajorRelease += (UINT8)(*Ptr - '0');
        ++Ptr;
      }
    }

    if (Type0->SystemBiosMinorRelease == 0xFF) {
      Ptr = BufPtr;
      while (*Ptr++ != '.');

      Type0->SystemBiosMinorRelease = 0;
      while (*Ptr != '.') {
        Type0->SystemBiosMinorRelease *= 10;
        Type0->SystemBiosMinorRelease += (UINT8)(*Ptr - '0');
        ++Ptr;
      }
    }

    Type0->BiosVersion = (SMBIOS_TABLE_STRING)Index++;
    BufPtr += AsciiStrLen(BufPtr) + 1;
  } else {
    Type0->BiosVersion = 0;
    Type0->SystemBiosMajorRelease = 0;
    Type0->SystemBiosMinorRelease = 0;
  }

  if (Type0->BiosReleaseDate == 0xFF) {
    UnicodeStrToAsciiStr ((CHAR16 *)PcdGetPtr(PcdFirmwareReleaseDateString), BufPtr);
  } else {
    Status = GetStringForType0(OFFSET_OF(SMBIOS_TABLE_TYPE0, BiosReleaseDate), (UINT8)Type0->BiosReleaseDate - 1, BufPtr, BufEnd);
    if (EFI_ERROR(Status)) {
      return;
    }
  }

  if (AsciiStrLen(BufPtr) > 0) {
    Type0->BiosReleaseDate = (SMBIOS_TABLE_STRING)Index++;
    BufPtr += AsciiStrLen(BufPtr) + 1;
  } else {
    Type0->BiosReleaseDate = 0;
  }

  //
  //  Transfer record buffer to OEM service, So OEM have chance to midify and verify record data
  //  before add reocrd to the system.
  //
  Status = OemSvcDxeUpdateSmbiosRecord((EFI_SMBIOS_TABLE_HEADER *)Buffer);
  DEBUG((EFI_D_INFO, "The return status is %r from OEM service update SMBIOS type %d.\n", Status, Type0->Hdr.Type));

  //
  //  if return EFI_SUCCESS from OEM service mean OEM don't want to add this SMBIOS record to system
  //
  if (Status != EFI_SUCCESS) {
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = SmbiosAdd(&mPrivateData.Smbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *)Buffer);

DEBUG_CODE_BEGIN();
    AlreadyExist[EFI_SMBIOS_TYPE_BIOS_INFORMATION] = TRUE;

    DEBUG((EFI_D_INFO, "The return status is %r for store SMBIOS type 0.\n", Status));
    DumpRecordData ((EFI_SMBIOS_TABLE_HEADER *)Buffer);
DEBUG_CODE_END();
  }

  FreePool(Buffer);

  return;
}

/**

  Depend on PCD create SMBIOS structures.

  @param       VOID

  @retval      VOID

**/
VOID
RecordSmbios(
  VOID
  )
{
  UINTN       Token;
  UINTN       TokenOffset;
  UINTN       Index;
  EFI_STATUS  Status;
  BOOLEAN     MultiRecords;
  UINT8      *MultiRecordsType ;
  UINTN       MultiRecordsTypeSize;
  UINT8       MaxMultiRecords;
  BOOLEAN     NeedFreePnpTable;

  Status = gBS->LocateProtocol (&gEfiPcdProtocolGuid, NULL, (VOID **) &Pcd);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Can not locate PCD protocol (%r)!!\n", Status));
    return;
  }

  //
  //  We don't need error checking for function GetPnpRegion().
  //  Because the DmiStart and/or VarStart will be NULL if can't find the PNP region and/or PNP variable.
  //
  GetPnpRegion();

  NeedFreePnpTable = FALSE;
  Status = OemSvcDxeInstallPnpStringTable (&TableCount, &PnpTable);
  if (Status == EFI_MEDIA_CHANGED) {
    NeedFreePnpTable = TRUE;
  } else {
    if (Status == EFI_UNSUPPORTED) {
      PnpTable   = PcdGetPtr(PcdSmbiosUpdatableStringTable);
      TableCount = FixedPcdGetPtrSize(PcdSmbiosUpdatableStringTable) / sizeof(DMI_UPDATABLE_STRING);
    } else if (PnpTable != NULL) {
      FreePool(PnpTable);  // BUGBUG : Do we real need to free memory in here??
      PnpTable = NULL;
    }
  }

  MultiRecordsType     = Pcd->GetPtr (&gSmbiosTokenSpaceGuid, PcdToken(PcdSmbiosMultiRecordsType));
  MultiRecordsTypeSize = Pcd->GetSize(&gSmbiosTokenSpaceGuid, PcdToken(PcdSmbiosMultiRecordsType));
  MaxMultiRecords      = Pcd->Get8   (&gSmbiosTokenSpaceGuid, PcdToken(PcdSmbiosMaxMultiRecords));

  //
  // Because Type 0 will to get some information from existing PCD.
  // So need a special function to process it.
  //
  if (Pcd->GetBool(&gSmbiosTokenSpaceGuid, PcdToken(PcdActiveSmbiosType000))) {
    RecordType0();
  }

  for (Token = PcdToken(PcdActiveSmbiosType001); Token <= PcdToken(PcdActiveSmbiosType255); ++Token) {
    if (Pcd->GetBool(&gSmbiosTokenSpaceGuid, Token)) {
      TokenOffset = Token - PcdToken(PcdActiveSmbiosType000);

      //
      //  The TokenOffset is equal to SMBIOS type number in this step.
      //
      MultiRecords = FALSE;
      for (Index = 0; Index < MultiRecordsTypeSize; ++Index) {
        if (TokenOffset == MultiRecordsType[Index]) {
          MultiRecords = TRUE;
          break;
        }
      }

      DEBUG((EFI_D_INFO, "Process SMBIOS Type %d\n", TokenOffset));

      if (MultiRecords) {
        for (Index = 0; Index < MaxMultiRecords; ++Index) {
          DEBUG((EFI_D_INFO, "Process SMBIOS Type %d  Sub-Record %d\n", TokenOffset, Index));
          RecordEachType(TokenOffset + (Index << MULTI_RECORDS_TOKEN_OFFSET));
        }
      } else {
        RecordEachType(TokenOffset);
      }
    }
  }

DEBUG_CODE_BEGIN();
  EndOfSmbiosScm = TRUE;
DEBUG_CODE_END();

  if (VarStart != NULL) {
    FreePool(VarStart);
  }

  if (NeedFreePnpTable) {
    FreePool(PnpTable);
  }

  return;
}
