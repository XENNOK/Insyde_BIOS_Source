/** @file
  Processor Cache data records. 

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
#include "CpuAccess.h"
#include "CacheData.h"

#include "CpuInitDxeStrDefs.h"
#include <IndustryStandard/SmBios.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/ProcessorProducer.h>

extern EFI_DATA_HUB_PROTOCOL      *mDataHub;
extern EFI_SUBCLASS_TYPE1_HEADER  mCpuDataRecordHeader;

EFI_CACHE_CONVERTER               mCacheConverter[] = {
  {
    1,
    0x09,
    32,
    CacheAssociativity4Way,
    CacheTypeInstruction
  },
  {
    1,
    0x0D,
    16,
    CacheAssociativity4Way,
    CacheTypeData
  },
  {
    2,
    0x21,
    256,
    CacheAssociativity8Way,
    CacheTypeUnified
  },
  {
    1,
    0x2C,
    32,
    CacheAssociativity8Way,
    CacheTypeData
  },
  {
    2,
    0x40,
    0,
    CacheAssociativityUnknown,
    CacheTypeUnknown
  },
  {
    3,
    0xD0,
    512,
    CacheAssociativity4Way,
    CacheTypeUnified
  },
  {
    3,
    0xD1,
    1024,
    CacheAssociativity4Way,
    CacheTypeUnified
  },
  {
    3,
    0xD2,
    2048,
    CacheAssociativity4Way,
    CacheTypeUnified
  },
  {
    3,
    0xD3,
    4096,
    CacheAssociativity4Way,
    CacheTypeUnified
  },
  {
    3,
    0xD4,
    8192,
    CacheAssociativity4Way,
    CacheTypeUnified
  },
  {
    3,
    0xD6,
    1024,
    CacheAssociativity8Way,
    CacheTypeUnified
  },
  {
    3,
    0xD7,
    2048,
    CacheAssociativity8Way,
    CacheTypeUnified
  },
  {
    3,
    0xD8,
    4096,
    CacheAssociativity8Way,
    CacheTypeUnified
  },
  {
    3,
    0xD9,
    8192,
    CacheAssociativity8Way,
    CacheTypeUnified
  },
  {
    3,
    0xDA,
    12288,
    CacheAssociativity8Way,
    CacheTypeUnified
  },
  {
    3,
    0xDC,
    1536,
    CacheAssociativity12Way,
    CacheTypeUnified
  },
  {
    3,
    0xDD,
    3072,
    CacheAssociativity12Way,
    CacheTypeUnified
  },
  {
    3,
    0xDE,
    6144,
    CacheAssociativity12Way,
    CacheTypeUnified
  },
  {
    3,
    0xDF,
    12288,
    CacheAssociativity12Way,
    CacheTypeUnified
  },
  {
    3,
    0xE0,
    18432,
    CacheAssociativity12Way,
    CacheTypeUnified
  },
  {
    3,
    0xE2,
    2048,
    CacheAssociativity16Way,
    CacheTypeUnified
  },
  {
    3,
    0xE3,
    4096,
    CacheAssociativity16Way,
    CacheTypeUnified
  },
  {
    3,
    0xE4,
    8192,
    CacheAssociativity16Way,
    CacheTypeUnified
  },
  {
    3,
    0xE5,
    16384,
    CacheAssociativity16Way,
    CacheTypeUnified
  },
  {
    3,
    0xE6,
    24576,
    CacheAssociativity16Way,
    CacheTypeUnified
  },
  {
    3,
    0xEA,
    12288,
    CacheAssociativity24Way,
    CacheTypeUnified
  },
  {
    3,
    0xEB,
    18432,
    CacheAssociativity24Way,
    CacheTypeUnified
  },
  {
    3,
    0xEC,
    24567,
    CacheAssociativity24Way,
    CacheTypeUnified
  },
  {
    0,
    0xFF,
    0,
    0,
    0
  }
};

///
/// Convert Cache Type Field to SMBIOS format
///
#define SMBIOS_CACHE_TYPE_MAX 5
UINT8 SmbiosCacheTypeFieldConverter[SMBIOS_CACHE_TYPE_MAX] = {
  EfiCacheTypeUnknown, 
  EfiCacheTypeData, 
  EfiCacheTypeInstruction, 
  EfiCacheTypeUnified,
  EfiCacheTypeOther
};

UINT8                             mCacheInstance[EFI_CACHE_LMAX] = { 0, 0, 0, 0 };

EFI_SUBCLASS_TYPE1_HEADER         mCacheDataRecordHeader = {
  EFI_CACHE_SUBCLASS_VERSION,         /// Version
  sizeof (EFI_SUBCLASS_TYPE1_HEADER), /// Header Size
  0,                                  /// Instance, Initialize later
  0,                                  /// SubInstance, Initialize later to Cache Level
  0                                   /// RecordType, Initialize later
};

/**

  This function gets called with the processor number and will log all cache data to data hub
  pertaining to this processor.

  @param[in] CpuNumber - Processor Number
  @param[in] CacheInformation - Cache information get from cpuid instruction

  @retval EFI_OUT_OF_RESOURCES - Failed to allocate required POOL for record buffer.
  @retval EFI_SUCCESS - successful to update cache data

**/
EFI_STATUS
InitializeCacheData (
  IN  UINTN                            CpuNumber,
  IN  EFI_CPUID_REGISTER               *CacheInformation
  )
{
  EFI_STATUS                    Status;
  UINT32                        HeaderSize;
  UINT32                        TotalSize;
  CPU_CACHE_DATA_RECORD_BUFFER  RecordBuffer;
  UINT8                         Index1;
  UINT8                         CacheLevel;
  UINT8                         LxCacheType;

  UINT32                        Ways;
  UINT32                        Partitions;
  UINT32                        LineSets;
  UINT32                        Sets;
  UINT32                        LxCacheSize;
  EFI_CPUID_REGISTER            CpuidRegisters;
//[-start-110805-IB03600420-add]//
  STRING_REF                    CacheDesignation[] = {
    STRING_TOKEN(STR_L1_CACHE),     STRING_TOKEN(STR_L2_CACHE),
    STRING_TOKEN(STR_L3_CACHE),     STRING_TOKEN(STR_L4_CACHE)
  };  
//[-end-110805-IB03600420-add]//

  ///
  /// Only log CPU socket level information.
  ///
  if (CpuNumber != 0) {
    return EFI_SUCCESS;
  }

  mCacheDataRecordHeader.Instance = (UINT16) (CpuNumber + 1);

  HeaderSize                      = sizeof (EFI_SUBCLASS_TYPE1_HEADER);
  RecordBuffer.Raw                = AllocatePool (HeaderSize + CPU_CACHE_DATA_MAXIMUM_LENGTH);
  if (RecordBuffer.Raw == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index1 = 0;; Index1++) {
    AsmCpuidEx (
      CPUID_CACHE_PARAMS,
      Index1,
      &CpuidRegisters.RegEax,
      &CpuidRegisters.RegEbx,
      &CpuidRegisters.RegEcx,
      &CpuidRegisters.RegEdx
      );
    if ((CpuidRegisters.RegEax & V_CPUID_CACHE_TYPE_MASK) == 0) {
      break;
    }

    Ways        = ((CpuidRegisters.RegEbx >> B_CPUID_CACHE_PARAMS_WAYS_SHIFT) & 0x3FF) + 1;
    Partitions  = ((CpuidRegisters.RegEbx >> B_CPUID_CACHE_PARAMS_PARTITIONS_SHIFT) & 0x3FF) + 1;
    LineSets    = (CpuidRegisters.RegEbx & 0xFFF) + 1;
    Sets        = CpuidRegisters.RegEcx + 1;

    CacheLevel  = (UINT8) (CpuidRegisters.RegEax & V_CPUID_CACHE_LEVEL_MASK) >> B_CPUID_CACHE_LEVEL_SHIFT;
    LxCacheSize = (Ways * Partitions * LineSets * Sets) / 1024;
    LxCacheType = (UINT8) (CpuidRegisters.RegEax & V_CPUID_CACHE_TYPE_MASK);

    CopyMem (RecordBuffer.Raw, &mCacheDataRecordHeader, HeaderSize);

    mCacheInstance[CacheLevel - 1]++;
    RecordBuffer.DataRecord->DataRecordHeader.Instance    = mCacheInstance[CacheLevel - 1];
    RecordBuffer.DataRecord->DataRecordHeader.SubInstance = CacheLevel;

    ///
    ///  Record Type 1
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType        = CacheSizeRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_SIZE_DATA);
    RecordBuffer.DataRecord->VariableRecord.CacheSize.Value = (UINT16) LxCacheSize;
    RecordBuffer.DataRecord->VariableRecord.CacheSize.Exponent = 10;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 2
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType              = MaximumSizeCacheRecordType;
    TotalSize = HeaderSize + sizeof (EFI_MAXIMUM_CACHE_SIZE_DATA);
    RecordBuffer.DataRecord->VariableRecord.MaximumCacheSize.Value = (UINT16) LxCacheSize;
    RecordBuffer.DataRecord->VariableRecord.MaximumCacheSize.Exponent = 10;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 3
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType        = CacheSpeedRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_SPEED_DATA);
    RecordBuffer.DataRecord->VariableRecord.CacheSpeed.Exponent = 0;
    RecordBuffer.DataRecord->VariableRecord.CacheSpeed.Value = 0;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 4
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType  = CacheSocketRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_SOCKET_DATA);
//[-start-110805-IB03600420-modify]//
    RecordBuffer.DataRecord->VariableRecord.CacheSocket =  CacheDesignation[CacheLevel-1];
//[-end-110805-IB03600420-modify]//
    ///
    /// CacheDesignation[Index1];
    ///
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 5
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType  = CacheSramTypeRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_SRAM_TYPE_DATA);
    ZeroMem (
      &RecordBuffer.DataRecord->VariableRecord.CacheSramType,
      sizeof (EFI_CACHE_SRAM_TYPE_DATA)
      );
    RecordBuffer.DataRecord->VariableRecord.CacheSramType.Synchronous = TRUE;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 6, since record same as Type 5
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType  = CacheInstalledSramTypeRecordType;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 7
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType    = CacheErrorTypeRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_ERROR_TYPE_DATA);
    RecordBuffer.DataRecord->VariableRecord.CacheErrorType = EfiCacheErrorSingleBit;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 8
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType  = CacheTypeRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_TYPE_DATA);
    ///
    /// If cache type is larger or equal than 5, this is undefined type so mark it as "Other" Cache type.
    ///
    if (LxCacheType >= SMBIOS_CACHE_TYPE_MAX) {
      LxCacheType = SMBIOS_CACHE_TYPE_MAX - 1;
    }
    RecordBuffer.DataRecord->VariableRecord.CacheType = SmbiosCacheTypeFieldConverter[LxCacheType];
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 9
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType        = CacheAssociativityRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_ASSOCIATIVITY_DATA);
    ///
    /// Convert Associativity Ways to SMBIOS format
    ///
    switch (Ways) {
      case 2:
      	Ways = CacheAssociativity2Way;
      	break;
      case 4:
      	Ways = CacheAssociativity4Way;
      	break;
      case 8:
      	Ways = CacheAssociativity8Way;
      	break;
      case 12:
      	Ways = CacheAssociativity12Way;
      	break;
      case 16:
      	Ways = CacheAssociativity16Way;
      	break;
      case 24:
      	Ways = CacheAssociativity24Way;
      	break;
      case 32:
      	Ways = CacheAssociativity32Way;
      	break;
      case 48:
      	Ways = CacheAssociativity48Way;
      	break;
      case 64:
      	Ways = CacheAssociativity64Way;
      	break;
      default:
        Ways = CacheAssociativityOther;
        break;
    }
    RecordBuffer.DataRecord->VariableRecord.CacheAssociativity = Ways;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    ///  Record Type 10
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType                = CacheConfigRecordType;
    TotalSize = HeaderSize + sizeof (EFI_CACHE_CONFIGURATION_DATA);
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.Level = CacheLevel;
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.Socketed = EFI_CACHE_NOT_SOCKETED;
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.Reserved2 = 0;
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.Location = EfiCacheInternal;
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.Enable = EFI_CACHE_ENABLED;
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.OperationalMode = EfiCacheWriteBack;
    RecordBuffer.DataRecord->VariableRecord.CacheConfig.Reserved1 = 0;
    Status = LogCacheData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Cache Association. Processor Record Type 17
    ///
    TotalSize = HeaderSize + sizeof (EFI_CACHE_ASSOCIATION_DATA);
    RecordBuffer.DataRecord->VariableRecord.CacheAssociation.ProducerName = gProcessorProducerGuid;
    ///
    /// RecordBuffer.DataRecord->VariableRecord.CacheAssociation.ProducerInstance = (UINT16)Instance;
    ///
    RecordBuffer.DataRecord->VariableRecord.CacheAssociation.Instance = RecordBuffer.DataRecord->DataRecordHeader.Instance;
    RecordBuffer.DataRecord->VariableRecord.CacheAssociation.SubInstance = RecordBuffer.DataRecord->DataRecordHeader.SubInstance;
    CopyMem (RecordBuffer.Raw, &mCpuDataRecordHeader, HeaderSize);
    RecordBuffer.DataRecord->DataRecordHeader.RecordType  = CacheAssociationRecordType;
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);
  }

  FreePool (RecordBuffer.Raw);
  return EFI_SUCCESS;
}

/**
  Log cache data into data hub

  @param[in] DataHub - Pointer to the DataHub protocol that will be updated
  @param[in] Buffer  - Data buffer which will be updated into DataHub
  @param[in] Size    - The size of data buffer

  @retval EFI_STATUS - status code for logging data into dat hub

**/
EFI_STATUS
LogCacheData (
  EFI_DATA_HUB_PROTOCOL      *DataHub,
  UINT8                      *Buffer,
  UINT32                     Size
  )
{
  EFI_STATUS  Status;

  Status = DataHub->LogData (
                      DataHub,
                      &gEfiCacheSubClassGuid,
                      &gProcessorProducerGuid,
                      EFI_DATA_RECORD_CLASS_DATA,
                      Buffer,
                      Size
                      );
  return Status;

}
