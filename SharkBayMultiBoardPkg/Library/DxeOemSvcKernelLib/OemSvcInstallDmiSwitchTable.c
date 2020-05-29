/** @file
  Provide OEM to define the SMBIOS policy.

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

#include <FrameworkDxe.h>
#include <Guid/DataHubRecords.h>
#include <Protocol/DataHubRecordPolicy.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeOemSvcKernelLib.h>

MISC_SUBCLASS              MiscSubclassDefault[] = {
//{ RecordNumber, MiscSubclassEnables, Instance }

  // Misc. BIOS Vendor - SMBIOS Type 0
  { EFI_MISC_BIOS_VENDOR_RECORD_NUMBER,             1, 1},
  // Misc. System Manufacturer - SMBIOS Type 1
  { EFI_MISC_SYSTEM_MANUFACTURER_RECORD_NUMBER,     1, 1},
  // Misc. Base Board Manufacturer - SMBIOS Type 2 --- dependency SMBIOS (Type 3)
  { EFI_MISC_BASE_BOARD_MANUFACTURER_RECORD_NUMBER, 1, 1},
  // Misc. System/Chassis Enclosure - SMBIOS Type 3
  { EFI_MISC_CHASSIS_MANUFACTURER_RECORD_NUMBER,    1, 1},
  // Misc. Port Connector Information - SMBIOS Type 8
  { EFI_MISC_PORT_INTERNAL_CONNECTOR_DESIGNATOR_RECORD_NUMBER, 1, 1},
  // Misc. System Slots - SMBIOS Type 9
  { EFI_MISC_SYSTEM_SLOT_DESIGNATION_RECORD_NUMBER, 1, 1},
  // Misc. System Slots - SMBIOS Type 9
  { EFI_MISC_SYSTEM_SLOT_DESIGNATION_RECORD_NUMBER, 1, 2},
  // Misc. System Slots - SMBIOS Type 9
  { EFI_MISC_SYSTEM_SLOT_DESIGNATION_RECORD_NUMBER, 1, 3},
  // Misc. System Slots - SMBIOS Type 9
  { EFI_MISC_SYSTEM_SLOT_DESIGNATION_RECORD_NUMBER, 1, 4},
  // Misc. System Slots - SMBIOS Type 9
  { EFI_MISC_SYSTEM_SLOT_DESIGNATION_RECORD_NUMBER, 1, 5},
  // Misc. System Slots - SMBIOS Type 9
  { EFI_MISC_SYSTEM_SLOT_DESIGNATION_RECORD_NUMBER, 1, 6},
  // Misc. Onboard Device - SMBIOS Type 10
  { EFI_MISC_ONBOARD_DEVICE_RECORD_NUMBER,          1, 1},
  // Misc. BIOS Language Information - SMBIOS Type 11
  { EFI_MISC_OEM_STRING_RECORD_NUMBER,              1, 1},
  // Misc. System Options - SMBIOS Type 12
  { EFI_MISC_SYSTEM_OPTION_STRING_RECORD_NUMBER,    1, 1},
  // Misc. Number of Installable Languages - SMBIOS Type 13
  { EFI_MISC_NUMBER_OF_INSTALLABLE_LANGUAGES_RECORD_NUMBER, 1, 1},
  // Misc. System Language String
  { EFI_MISC_SYSTEM_LANGUAGE_STRING_RECORD_NUMBER,  1, 1},
  // Misc. Pointing Device Type - SMBIOS Type 21
  { EFI_MISC_POINTING_DEVICE_TYPE_RECORD_NUMBER    ,1, 1},
  // Misc. Reset Capabilities - SMBIOS Type 23
  { EFI_MISC_RESET_CAPABILITIES_RECORD_NUMBER,      1, 1},
  // Misc. Hardware Security - SMBIOS Type 24
  { EFI_MISC_HARDWARE_SECURITY_SETTINGS_DATA_RECORD_NUMBER, 1, 1},
  // Misc. BIS Entry Point - SMBIOS Type 31
  { EFI_MISC_BIS_ENTRY_POINT_RECORD_NUMBER,         1, 1},
  // Misc. Boot Information - SMBIOS Type 32
  { EFI_MISC_BOOT_INFORMATION_STATUS_RECORD_NUMBER, 1, 1},
  // IPMI Data Record - SMBIOS Type 38
  { EFI_MISC_IPMI_INTERFACE_TYPE_RECORD_NUMBER,     1, 1},
  // OEM Data Record - SMBIOS Type 0x80-0xFF
  { EFI_MISC_SMBIOS_STRUCT_ENCAP_RECORD_NUMBER,     1, 1},
  //Misc. Asf Type129
  { EFI_MISC_SMBIOS_STRUCT_ENCAP_RECORD_NUMBER,     1, 2},
  //Misc. Amt Type130
  { EFI_MISC_SMBIOS_STRUCT_ENCAP_RECORD_NUMBER,     1, 3},
  //Misc. Amt Type131
  { EFI_MISC_SMBIOS_STRUCT_ENCAP_RECORD_NUMBER,     1, 4},
  // System Event Log Record - SMBIOS Type 15
  { EFI_MISC_SYSTEM_EVENT_LOG_RECORD_NUMBER,        1, 1},
  // System Power supply Record - SMBIOS Type 39
  { EFI_MISC_SYSTEM_POWER_SUPPLY_RECORD_NUMBER,     1, 1},
  // Misc. Cooling Device - SMBIOS Type 27
  { EFI_MISC_COOLING_DEVICE_TEMP_LINK_RECORD_NUMBER,1, 1},
  //End of Table
  { 0xFF, 1, 1},
};


EFI_DATAHUB_RECORD_POLICY        DataHubRecordPolicy = {
//CACHE_SMBIOS_TYPE          CacheEnables
  {                                       //SMBIOS (Type 7) Cache Information
    1, //UINT16 CacheSizeRecordType               :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 MaximumSizeCacheRecordType        :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheSpeedRecordType              :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheSocketRecordType             :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheSramTypeRecordType           :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheInstalledSramTypeRecordType  :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheErrorTypeRecordType          :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheTypeRecordType               :1; // 0: Disabled; 1: Enabled*

    1, //UINT16 CacheAssociativityRecordType      :1; // 0: Disabled; 1: Enabled*
    1, //UINT16 CacheConfigRecordType             :1; // 0: Disabled; 1: Enabled*
    0, //Rsvd	  6
  },

//ProcessorEnables
  {                                   //SMBIOS (Type 4) Processor Information
    1, //UINT32 ProcessorCoreFrequencyRecordType        :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorFsbFrequencyRecordType         :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorVersionRecordType              :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorManufacturerRecordType         :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorSerialNumberRecordType         :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorIdRecordType                   :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorTypeRecordType                 :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorFamilyRecordType               :1; // 0: Disabled; 1: Enabled*

    1, //UINT32 ProcessorVoltageRecordType              :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorApicBaseAddressRecordType      :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorApicIdRecordType               :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorApicVersionNumberRecordType    :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 CpuUcodeRevisionDataRecordType          :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorStatusRecordType               :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorSocketTypeRecordType           :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorSocketNameRecordType           :1; // 0: Disabled; 1: Enabled*

    1, //UINT32 CacheAssociationRecordType              :1; // 0: Disabled; 1: Enabled* --- dependency SMBIOS (Type 7)
    1, //UINT32 ProcessorMaxCoreFrequencyRecordType     :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorAssetTagRecordType             :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorMaxFsbFrequencyRecordType      :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorPackageNumberRecordType        :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorCoreFrequencyListRecordType    :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorFsbFrequencyListRecordType     :1; // 0: Disabled; 1: Enabled*
    1, //UINT32 ProcessorHealthStatusRecordType         :1; // 0: Disabled; 1: Enabled*

    1, //UINT32 ProcessorPartNumberRecordType           :1; // 0: Disabled; 1: Enabled*
    0, //Rsvd 7
  },
//MemoryEnables
  {
    0, //Reserved_1
  //
  // Note: According to SMBIOS Spec. v2.7, the required SMBIOS includes Type 16, Type 17, and Type19.
  //       User can not disabled those types.
  //
       //SMBIOS (Type 16) Physical Memory Array
    1, //UINT16 MemoryArrayLocationRecordNumber         :1; // 0: Disabled; 1: Enabled*

       //SMBIOS (Type 17) Memory Device --- dependency SMBIOS (Type 16)
    1, //UINT16 MemoryArrayLinkRecordNumber             :1; // 0: Disabled; 1: Enabled*

       //SMBIOS (TYPE 19) Memory Array Mapped Address --- dependency SMBIOS (Type 16)
    1, //UINT16 MemoryArrayStartAddressRecordNumber     :1; // 0: Disabled; 1: Enabled*

       //SMBIOS (TYPE 20) Memory Device Mapped Address --- dependency SMBIOS (Type 17) and SMBIOS (TYPE 19)
    1, //UINT16 MemoryDeviceStartAddressRecordNumber    :1; // 0: Disabled; 1: Enabled*

    0, //Reserved_2                                     :1;
    0, //Reserved_3                                     :1;

       //SMBIOS (TYPE 18)
    1, //UINT16 Memory32bitErrorInformationRecordNumber :1; // 0: Disabled; 1: Enabled*

       //SMBIOS (TYPE 5)
    1, //UINT16 MemoryControllerInformationRecordNumber :1; // 0: Disabled; 1: Enabled*

       //SMBIOS (TYPE 6) Memory Module Information
    1, //UINT16 MemoryModuleInformationDataRecordNumber :1; // 0: Disabled; 1: Enabled*

    0, //Reserved_4                                     :6;
  },
//MiscSubclass
  {MiscSubclassDefault}
};

/**
  Provide OEM to define the SMBIOS policy. The MiscSubClass table in SMBIOS is constructed by the instance of SMBIOS Type.
  This policy decides which instance of Type (RecordNumber) will be added into the table.

  @param[out]  *MiscSubclassDefaultSize  Pointer to the data size of *mDataHubRecordPolicy
  @param[out]  *mDataHubRecordPolicy     Pointer to EFI_DATAHUB_RECORD_POLICY

  @retval      EFI_MEDIA_CHANGED         Get SMBIOS policy success.
  @retval      Others                    The error status based on OEM design.
**/
EFI_STATUS
OemSvcInstallDmiSwitchTable (
  OUT UINTN                          *MiscSubclassDefaultSize,
  OUT EFI_DATAHUB_RECORD_POLICY      **mDataHubRecordPolicy
  )
{
  UINTN                                 TempDataSize;

  //
  // Get whole table size
  //
  TempDataSize = sizeof(MiscSubclassDefault) +
                 sizeof(EFI_DATAHUB_RECORD_POLICY) -
                 sizeof (UINTN);

  *MiscSubclassDefaultSize = TempDataSize;

  //
  // Allocate pool, prepare copy
  //
  (*mDataHubRecordPolicy) = AllocateZeroPool (TempDataSize);

  //
  // Copy Cache, Memory, Processor Policy
  //
  TempDataSize = sizeof(EFI_DATAHUB_RECORD_POLICY) - sizeof(UINTN);
  CopyMem (
    (*mDataHubRecordPolicy),
    &DataHubRecordPolicy,
    TempDataSize
    );

  //
  // Copy Cache, Memory, Processor Policy
  //
  TempDataSize = sizeof(MiscSubclassDefault);
  CopyMem (
  &((*mDataHubRecordPolicy)->MiscSubclass),
  MiscSubclassDefault,
  TempDataSize
  );

  return EFI_MEDIA_CHANGED;
}
