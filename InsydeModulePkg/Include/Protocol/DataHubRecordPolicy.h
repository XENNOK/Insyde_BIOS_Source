/** @file
  Protocol used for SMBIOS Policy definition.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DATAHUB_RECORD_POLICY_H_
#define _DATAHUB_RECORD_POLICY_H_

#define EFI_DATAHUB_RECORD_POLICY_PROTOCOL_GUID \
         { 0x41a3ee4e, 0x6d57, 0x418b, 0x8f, 0x8e, 0xc3, 0x66, 0xa5, 0xb7, 0xc, 0x4b }

typedef struct _EFI_DATAHUB_RECORD_POLICY EFI_DATAHUB_RECORD_POLICY;

#pragma pack (1)

typedef struct _CACHE_SMBIOS_TYPE {     //SMBIOS (Type 7) Cache Information
  UINT16 CacheSizeRecordType               :1; // 0: Disabled; 1: Enabled*
  UINT16 MaximumSizeCacheRecordType        :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheSpeedRecordType              :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheSocketRecordType             :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheSramTypeRecordType           :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheInstalledSramTypeRecordType  :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheErrorTypeRecordType          :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheTypeRecordType               :1; // 0: Disabled; 1: Enabled*

  UINT16 CacheAssociativityRecordType      :1; // 0: Disabled; 1: Enabled*
  UINT16 CacheConfigRecordType             :1; // 0: Disabled; 1: Enabled*
  UINT16 Rsvd                              :6;
} CACHE_SMBIOS_TYPE;

typedef struct _PROCESSOR_SMBIOS_TYPE {  //SMBIOS (Type 4) Processor Information
  UINT32 ProcessorCoreFrequencyRecordType        :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorFsbFrequencyRecordType         :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorVersionRecordType              :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorManufacturerRecordType         :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorSerialNumberRecordType         :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorIdRecordType                   :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorTypeRecordType                 :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorFamilyRecordType               :1; // 0: Disabled; 1: Enabled*

  UINT32 ProcessorVoltageRecordType              :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorApicBaseAddressRecordType      :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorApicIdRecordType               :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorApicVersionNumberRecordType    :1; // 0: Disabled; 1: Enabled*
  UINT32 CpuUcodeRevisionDataRecordType          :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorStatusRecordType               :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorSocketTypeRecordType           :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorSocketNameRecordType           :1; // 0: Disabled; 1: Enabled*

  UINT32 CacheAssociationRecordType              :1; // 0: Disabled; 1: Enabled* --- dependency SMBIOS (Type 7)
  UINT32 ProcessorMaxCoreFrequencyRecordType     :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorAssetTagRecordType             :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorMaxFsbFrequencyRecordType      :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorPackageNumberRecordType        :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorCoreFrequencyListRecordType    :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorFsbFrequencyListRecordType     :1; // 0: Disabled; 1: Enabled*
  UINT32 ProcessorHealthStatusRecordType         :1; // 0: Disabled; 1: Enabled*

  UINT32 ProcessorPartNumberRecordType           :1; // 0: Disabled; 1: Enabled*
  UINT32 Rsvd                                    :7;
} PROCESSOR_SMBIOS_TYPE;

typedef struct _MEMORY_RECORD_NUMBER {
  UINT16 Reserved_1                              :1;

  //
  //SMBIOS (Type 16) Physical Memory Array
  //
  UINT16 MemoryArrayLocationRecordNumber         :1; // 0: Disabled; 1: Enabled*

  //
  //SMBIOS (Type 17) Memory Device --- dependency SMBIOS (Type 16)
  //
  UINT16 MemoryArrayLinkRecordNumber             :1; // 0: Disabled; 1: Enabled*

  //
  //SMBIOS (TYPE 19) Memory Array Mapped Address --- dependency SMBIOS (Type 16)
  //
  UINT16 MemoryArrayStartAddressRecordNumber     :1; // 0: Disabled; 1: Enabled*

  //
  //SMBIOS (TYPE 20) Memory Device Mapped Address --- dependency SMBIOS (Type 17) and SMBIOS (TYPE 19)
  //
  UINT16 MemoryDeviceStartAddressRecordNumber    :1; // 0: Disabled; 1: Enabled*

  UINT16 Reserved_2                              :1;
  UINT16 Reserved_3                              :1;

  //
  //SMBIOS (TYPE 18) 32-Bit Memory Error Information
  //
  UINT16 Memory32bitErrorInformationRecordNumber :1; // 0: Disabled; 1: Enabled*

  //
  //SMBIOS (TYPE 5) Memory Controller Information
  //
  UINT16 MemoryControllerInformationRecordNumber :1; // 0: Disabled; 1: Enabled*

  //
  //SMBIOS (TYPE 6) Memory Module Information
  //
  UINT16 MemoryModuleInformationDataRecordNumber :1; // 0: Disabled; 1: Enabled*

  //
  //Reserved
  //
  UINT16 Reserved_4                              :6;

} MEMORY_RECORD_NUMBER;

typedef struct {

 UINT16                          RecordNumber;
 UINT8                           MiscSubclassEnables;
 UINT16                          Instance;

} MISC_SUBCLASS;

struct _EFI_DATAHUB_RECORD_POLICY {
  CACHE_SMBIOS_TYPE          CacheEnables;
  PROCESSOR_SMBIOS_TYPE      ProcessorEnables;
  MEMORY_RECORD_NUMBER       MemoryEnables;
  MISC_SUBCLASS              *MiscSubclass;
};

#pragma pack ()

extern EFI_GUID gEfiDataHubRecordPolicyGuid;

#endif
