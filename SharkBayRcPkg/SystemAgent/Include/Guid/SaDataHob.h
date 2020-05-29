/** @file

  The GUID definition for SaDataHob

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SA_DATA_HOB_H_
#define _SA_DATA_HOB_H_

#define SA_DATA_HOB_GUID \
  { 0x4C10D934, 0x38E6, 0x45A4, { 0x9A, 0x24, 0x2A, 0x79, 0xB9, 0x3D, 0xCB, 0x7F }}

extern EFI_GUID gSaDataHobGuid;

#define GP_ENABLE   1
#define GP_DISABLE  0

#ifndef __PI_HOB_H__
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  //
  // Guid specific data goes here
  //
} EFI_HOB_GUID_TYPE;
#endif
#endif

///
/// DPR Directory Types
///
typedef enum {
  EnumDprDirectoryTxt   = 0,
  EnumDprDirectoryPfat
} DPR_DIRECTORY_ELEMENT;

#define DPR_DIRECTORY_TYPE_TXT     0x01
#define DPR_DIRECTORY_TYPE_PFAT    0x02
#define DPR_DIRECTORY_MAX          2

///
/// DPR directory entry definition
///
typedef struct {
  UINT8   Type;          ///< DPR Directory Type
  UINT8   Size;          ///< DPR Size in MB
  UINT32  PhysBase;      ///< Physical address - must be 4K aligned (bits 11..0 must be clear)
  UINT16  Reserved;      ///< Must be 0
} DPR_DIRECTORY_ENTRY;

///
/// PEG data definition
///
typedef struct {
  UINT32  EndPointVendorIdDeviceId[3];  ///< VID/DID for each PEG controller
  UINT8   BestPreset[16];               ///< Best preset value for each lane
  UINT8   PegGen3PresetSearch;          ///< Policy value from earlier boot
  UINT8   PegLinkFailMask;              ///< Mask of PEG controllers to ignore
} SA_PEG_DATA;

//[-start-140220-IB10920078-add]//
//
// Support and Unsupport define
//
typedef enum {
  Unsupport = 0,
  Support
} SUPPORT_SETTING;

//
// Active and Inactive define
//
typedef enum {
  Inactive = 0,
  Active
} ACTIVE_SETTING;

//
// PowerXpress settings
//
typedef enum {
  MuxedFixed   = 1,
  MuxlessFixed = 5,
  MuxlessDynamic,
  MuxlessFixedDynamic,
  FullDgpuPowerOffDynamic,
  FixedFullDgpuPowerOffDynamic = 13
} POWER_XPRESS_SETTING;

//[-end-140220-IB10920078-add]//
///
/// SgMode settings
///
typedef enum {
  SgModeDisabled = 0,
  SgModeMuxed,
  SgModeMuxless,
  SgModeDgpu,
  SgModeMax
} SG_MODE;

///
/// SA Info HOB
///
typedef struct _SG_INFO_HOB {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  UINT8             RevisionId;     ///< Revision ID
  SG_MODE           SgMode;
  BOOLEAN           SgGpioSupport;  ///< 1=Supported; 0=Not Supported
  UINT8             SgDgpuPwrOK;
  UINT8             SgDgpuHoldRst;
  UINT8             SgDgpuPwrEnable;
  UINT8             SgDgpuPrsnt;
} SG_INFO_HOB;

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;
  DPR_DIRECTORY_ENTRY   DprDirectory[DPR_DIRECTORY_MAX];
  BOOLEAN               PegDataValid;                      ///< TRUE, if PegData contains data from a prior boot
  SA_PEG_DATA           PegData;
  UINT16                SaIotrapSmiAddress;                ///< Store address for Iotrap SMI
  BOOLEAN               InitPcieAspmAfterOprom;            ///< Switch for PCIe ASPM initialization after Oprom or before
  SG_INFO_HOB           SgInfo;
  BOOLEAN               PegPlatformResetRequired;          ///< if PegPlatformResetRequired=1, platform code should generate a cold/power cycle reset after saving PEG config data into NVRAM
} SA_DATA_HOB;

#endif
