/** @file
  Industry Standard Definitions of SMBIOS Table Specification v2.8.0

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef __SMBIOS_STANDARD_280_H__
#define __SMBIOS_STANDARD_280_H__

#include "../../../Include/IndustryStandard/SmBios.h"

#pragma pack(1)
///
/// Processor Information - Processor Family.
///
typedef enum {
  ProcessorFamilyAmdFxSeries            = 0x3F,
  ProcessorFamilyAmdASeries             = 0x48,
  ProcessorFamilyAmdZSeries             = 0x4A,
  ProcessorFamilyAmdRSeries             = 0x4B,
  ProcessorFamilyAmdOpteron4300Series   = 0x4C,
  ProcessorFamilyAmdOpteron6300Series   = 0x4D,
  ProcessorFamilyAmdOpteron3300Series   = 0x4E,
  ProcessorFamilyAmdFireProSeries       = 0x4F,
  ProcessorFamilyAmdOpteron3000Series   = 0xE4,
  ProcessorFamilyAmdSempronII           = 0xE5,
} PROCESSOR_FAMILY_DATA_280;

///
/// Processor Information - Processor Upgrade.
///
typedef enum {
  ProcessorUpgradeSocketLGA1155 = 0x24,
  ProcessorUpgradeSocketLGA2011_3 = 0x2B,
  ProcessorUpgradeSocketLGA1356_3 = 0x2C
} PROCESSOR_UPGRADE_280;

typedef struct {
  UINT16    Reserved        :1;
  UINT16    Other           :1;
  UINT16    Unknown         :1;
  UINT16    FastPaged       :1;
  UINT16    StaticColumn    :1;
  UINT16    PseudoStatic    :1;
  UINT16    Rambus          :1;
  UINT16    Synchronous     :1;
  UINT16    Cmos            :1;
  UINT16    Edo             :1;
  UINT16    WindowDram      :1;
  UINT16    CacheDram       :1;
  UINT16    Nonvolatile     :1;
  UINT16    Registered      :1;
  UINT16    Unbuffered      :1;
  UINT16    LrDimm          :1;
} MEMORY_DEVICE_TYPE_DETAIL_280;

///
/// Memory Device (Type 17).
///
/// This structure describes a single memory device that is part of 
/// a larger Physical Memory Array (Type 16).
/// Note:  If a system includes memory-device sockets, the SMBIOS implementation 
/// includes a Memory Device structure instance for each slot, whether or not the 
/// socket is currently populated.
///
typedef struct {
  SMBIOS_STRUCTURE          Hdr;
  UINT16                    MemoryArrayHandle;
  UINT16                    MemoryErrorInformationHandle;
  UINT16                    TotalWidth;
  UINT16                    DataWidth;
  UINT16                    Size;
  UINT8                     FormFactor;                     ///< The enumeration value from MEMORY_FORM_FACTOR.
  UINT8                     DeviceSet;
  SMBIOS_TABLE_STRING       DeviceLocator;
  SMBIOS_TABLE_STRING       BankLocator;
  UINT8                     MemoryType;                     ///< The enumeration value from MEMORY_DEVICE_TYPE.
  MEMORY_DEVICE_TYPE_DETAIL_280 TypeDetail;
  UINT16                    Speed;
  SMBIOS_TABLE_STRING       Manufacturer;
  SMBIOS_TABLE_STRING       SerialNumber;
  SMBIOS_TABLE_STRING       AssetTag;
  SMBIOS_TABLE_STRING       PartNumber;
  //
  // Add for smbios 2.6
  //  
  UINT8                     Attributes;
  //
  // Add for smbios 2.7
  //
  UINT32                    ExtendedSize;
  UINT16                    ConfiguredMemoryClockSpeed;
  //
  // Add for smbios 2.8
  //
  UINT16                    MinimumVoltage;
  UINT16                    MaximumVoltage;
  UINT16                    ConfiguredVoltage;
} SMBIOS_TABLE_TYPE17_280;

///
/// Union of all the possible SMBIOS record types.
///
typedef union {
  SMBIOS_STRUCTURE      *Hdr;
  SMBIOS_TABLE_TYPE0    *Type0;
  SMBIOS_TABLE_TYPE1    *Type1;
  SMBIOS_TABLE_TYPE2    *Type2;
  SMBIOS_TABLE_TYPE3    *Type3;
  SMBIOS_TABLE_TYPE4    *Type4;
  SMBIOS_TABLE_TYPE5    *Type5;
  SMBIOS_TABLE_TYPE6    *Type6;
  SMBIOS_TABLE_TYPE7    *Type7;
  SMBIOS_TABLE_TYPE8    *Type8;
  SMBIOS_TABLE_TYPE9    *Type9;
  SMBIOS_TABLE_TYPE10   *Type10;
  SMBIOS_TABLE_TYPE11   *Type11;
  SMBIOS_TABLE_TYPE12   *Type12;
  SMBIOS_TABLE_TYPE13   *Type13;
  SMBIOS_TABLE_TYPE14   *Type14;
  SMBIOS_TABLE_TYPE15   *Type15;
  SMBIOS_TABLE_TYPE16   *Type16;
  SMBIOS_TABLE_TYPE17_280 *Type17;
  SMBIOS_TABLE_TYPE18   *Type18;
  SMBIOS_TABLE_TYPE19   *Type19;
  SMBIOS_TABLE_TYPE20   *Type20;
  SMBIOS_TABLE_TYPE21   *Type21;
  SMBIOS_TABLE_TYPE22   *Type22;
  SMBIOS_TABLE_TYPE23   *Type23;
  SMBIOS_TABLE_TYPE24   *Type24;
  SMBIOS_TABLE_TYPE25   *Type25;
  SMBIOS_TABLE_TYPE26   *Type26;
  SMBIOS_TABLE_TYPE27   *Type27;
  SMBIOS_TABLE_TYPE28   *Type28;
  SMBIOS_TABLE_TYPE29   *Type29;
  SMBIOS_TABLE_TYPE30   *Type30;
  SMBIOS_TABLE_TYPE31   *Type31;
  SMBIOS_TABLE_TYPE32   *Type32;
  SMBIOS_TABLE_TYPE33   *Type33;
  SMBIOS_TABLE_TYPE34   *Type34;
  SMBIOS_TABLE_TYPE35   *Type35;
  SMBIOS_TABLE_TYPE36   *Type36;
  SMBIOS_TABLE_TYPE37   *Type37;
  SMBIOS_TABLE_TYPE38   *Type38;
  SMBIOS_TABLE_TYPE39   *Type39;
  SMBIOS_TABLE_TYPE40   *Type40;
  SMBIOS_TABLE_TYPE41   *Type41;
  SMBIOS_TABLE_TYPE42   *Type42;
  SMBIOS_TABLE_TYPE126  *Type126;
  SMBIOS_TABLE_TYPE127  *Type127;
  UINT8                 *Raw;
} SMBIOS_STRUCTURE_POINTER_280;

#pragma pack()

#endif
