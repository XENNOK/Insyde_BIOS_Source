/** @file
  This is part of the implementation of an Intel Graphics drivers OpRegion /
  Software SCI interface between system BIOS, ASL code, and Graphics drivers.

  Supporting Specifiction: OpRegion / Software SCI SPEC 0.70

  Acronyms:
    IGD:        Internal Graphics Device
    NVS:        ACPI Non Volatile Storage
    OpRegion:   ACPI Operational Region
    VBT:        Video BIOS Table (OEM customizable data)

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _IGD_OPREGION_H_
#define _IGD_OPREGION_H_

///
/// Statements that include other header files.
///
//#include "EdkIIGlueDxe.h"
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <EfiScriptLib.h>
#include <PchAccess.h>
#include <PchPlatformLib.h>
#include <SaAccess.h>
#include <IndustryStandard/Pci22.h>

//#include "EfiMgmtModeRuntimeLib.h"
//#include "EfiApi.h"

///
/// Driver Consumed Protocol Prototypes
///
#include <FrameworkDxe.h>
#include <Guid/DataHubRecords.h>
#include <FrameworkDxe.h>
#include <Protocol/SaPlatformPolicy.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>

#include <Protocol/CpuIo.h>
#include <Protocol/DataHub.h>
#include <Protocol/SaGlobalNvsArea.h>
#include <Protocol/LegacyBios.h>
#include <Library/IoLib.h>

#include <ExitPmAuth.h>
//[-start-120419-IB06150216-add]//
#include <Protocol/PlatformGopPolicy.h>
//[-end-120419-IB06150216-add]//

///
/// Driver Produced Protocol Prototypes
///
#include <Protocol/IgdOpRegion.h>

///
///
/// OpRegion (Miscellaneous) #defines.
///
/// OpRegion Header #defines.
///
#define HEADER_SIGNATURE            "IntelGraphicsMem"
#define HEADER_SIZE                 0x2000
#define HEADER_OPREGION_VER         0x0200
#define HEADER_OPREGION_REV         0x00
#define HEADER_MBOX_SUPPORT         (HD_MBOX5 + HD_MBOX4 + HD_MBOX3 + HD_MBOX2 + HD_MBOX1)
#define HEADER_MBOX_SUPPORT_MOBILE  HEADER_MBOX_SUPPORT
#define HEADER_MBOX_SUPPORT_DESKTOP HEADER_MBOX_SUPPORT
#define HD_MBOX1                    BIT0
#define HD_MBOX2                    BIT1
#define HD_MBOX3                    BIT2
#define HD_MBOX4                    BIT3
#define HD_MBOX5                    BIT4
#define SVER_SIZE                   32

///
/// OpRegion Mailbox 1 EQUates.
///
/// OpRegion Mailbox 3 EQUates.
///
#define ALS_ENABLE            BIT0
#define BLC_ENABLE            BIT1
#define BACKLIGHT_BRIGHTNESS  0xFF
#define FIELD_VALID_BIT       BIT31
#define WORD_FIELD_VALID_BIT  BIT15
#define PFIT_ENABLE           BIT2
#define PFIT_OPRN_AUTO        0x00000000
#define PFIT_OPRN_SCALING     0x00000007
#define PFIT_OPRN_OFF         0x00000000
#define PFIT_SETUP_AUTO       0
#define PFIT_SETUP_SCALING    1
#define PFIT_SETUP_OFF        2
#define INIT_BRIGHT_LEVEL     0x64
#define PFIT_STRETCH          6

///
/// GMCH PCI register access #defines.
///
#define IgdMmPci32(Register)            MmPci32 (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, Register)
#define IgdMmPci16Or(Register, OrData)  MmPci16Or (0, SA_IGD_BUS, SA_IGD_DEV, SA_IGD_FUN_0, Register, OrData)
#define IgdMmPci16AndThenOr(Register, AndData, OrData) \
  MmPci16AndThenOr ( \
  0, \
  SA_IGD_BUS, \
  SA_IGD_DEV, \
  SA_IGD_FUN_0, \
  Register, \
  AndData, \
  OrData \
  )

///
/// Video BIOS / VBT #defines
///
#define OPTION_ROM_SIGNATURE    0xAA55
#define VBIOS_LOCATION_PRIMARY  0xC0000

#define VBT_SIGNATURE           SIGNATURE_32 ('$', 'V', 'B', 'T')
///
/// Typedef stuctures
///
#pragma pack(1)
typedef struct {
  UINT16  Signature;  /// 0xAA55
  UINT8   Size512;
  UINT8   Reserved[21];
  UINT16  PcirOffset;
  UINT16  VbtOffset;
} INTEL_VBIOS_OPTION_ROM_HEADER;
#pragma pack()

#pragma pack(1)
typedef struct {
  UINT32  Signature;  /// "PCIR"
  UINT16  VendorId;   /// 0x8086
  UINT16  DeviceId;
  UINT16  Reserved0;
  UINT16  Length;
  UINT8   Revision;
  UINT8   ClassCode[3];
  UINT16  ImageLength;
  UINT16  CodeRevision;
  UINT8   CodeType;
  UINT8   Indicator;
  UINT16  Reserved1;
} INTEL_VBIOS_PCIR_STRUCTURE;
#pragma pack()

#pragma pack(1)
typedef struct {
  UINT8   HeaderSignature[20];
  UINT16  HeaderVersion;
  UINT16  HeaderSize;
  UINT16  HeaderVbtSize;
  UINT8   HeaderVbtCheckSum;
  UINT8   HeaderReserved;
  UINT32  HeaderOffsetVbtDataBlock;
  UINT32  HeaderOffsetAim1;
  UINT32  HeaderOffsetAim2;
  UINT32  HeaderOffsetAim3;
  UINT32  HeaderOffsetAim4;
  UINT8   DataHeaderSignature[16];
  UINT16  DataHeaderVersion;
  UINT16  DataHeaderSize;
  UINT16  DataHeaderDataBlockSize;
  UINT8   CoreBlockId;
  UINT16  CoreBlockSize;
  UINT16  CoreBlockBiosSize;
  UINT8   CoreBlockBiosType;
  UINT8   CoreBlockReleaseStatus;
  UINT8   CoreBlockHWSupported;
  UINT8   CoreBlockIntegratedHW;
  UINT8   CoreBlockBiosBuild[4];
  UINT8   CoreBlockBiosSignOn[155];
} VBIOS_VBT_STRUCTURE;
#pragma pack()
///
/// Driver Private Function definitions
///
EFI_STATUS
GetSVER (
  OUT UINT8 *SVER
  )
/**
  Set the SVER (system BIOS ID) string with the system BIOS build number.

  @param[in] SVER  String to populate with system BIOS build number.

  @retval EFI_SUCCESS     The SVER string is populated.
  @exception EFI_UNSUPPORTED The SVER string is not populated.
**/
;

EFI_STATUS
GetStringFromToken (
  IN      EFI_GUID                  *ProducerGuid,
  IN      STRING_REF                Token,
  OUT     CHAR16                    **String
  )
/**
  Acquire the string associated with the ProducerGuid and return it.

  @param[in] ProducerGuid - The Guid to search the HII database for
  @param[in] Token - The token value of the string to extract
  @param[in] String - The string that is extracted

  @retval EFI_SUCCESS       The function completed successfully
  @retval EFI_NOT_FOUND     The requested string was not found
**/
;

EFI_STATUS
IgdOpRegionInit (
  void
  )
/**
  Graphics OpRegion / Software SCI driver installation function.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The driver installed without error.
  @retval EFI_ABORTED     - The driver encountered an error and could not complete
                            installation of the ACPI tables.
**/
;

EFI_STATUS
ExtractDataFromHiiHandle (
  IN      EFI_HII_HANDLE      HiiHandle,
  IN OUT  UINT16              *ImageLength,
  OUT     UINT8               *DefaultImage,
  OUT     EFI_GUID            *Guid
  )
/**
  Extract information pertaining to the HiiHandle

  @param[in] HiiHandle       - Hii handle
  @param[in] ImageLength     - For input, length of DefaultImage;
                    For output, length of actually required
  @param[in] DefaultImage    - Image buffer prepared by caller
  @param[in] Guid            - Guid information about the form

  @retval EFI_OUT_OF_RESOURCES    - No enough buffer to allocate
  @retval EFI_BUFFER_TOO_SMALL    - DefualtImage has no enough ImageLength
  @retval EFI_SUCCESS             - Successfully extract data from Hii database.
**/
;

EFI_STATUS
GetVBiosVbtExitPmAuth (
  VOID
  )
/**
  Get Intel video BIOS VBT information (i.e. Pointer to VBT and VBT size).
  The VBT (Video BIOS Table) is a block of customizable data that is built
  within the video BIOS and edited by customers.

  @retval EFI_SUCCESS            - Video BIOS VBT information returned.
  @exception EFI_UNSUPPORTED     - Could not find VBT information (*VBiosVbtPtr = NULL).
**/
;

EFI_STATUS
UpdateIgdOpRegionExitPmAuth (
  VOID
  )
/**
  Update Graphics OpRegion after PCI enumeration.

  @param[in] void         - None
  @retval EFI_SUCCESS     - The function completed successfully.
**/
;
#endif
