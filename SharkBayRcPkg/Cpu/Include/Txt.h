/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Txt.h

@brief
  This file contains definitions required to use the TXT BIOS
  Authenticated Code Module Library.
**/
#ifndef _TXT_BIOS_ACM_LIB_H_
#define _TXT_BIOS_ACM_LIB_H_

#define TPM_TIME_OUT  750

#define TXT_RLP_INIT  1

///
/// MMIO definition
///
#ifndef MmPciAddress
#define MmPciAddress(Segment, Bus, Device, Function, Register) \
        ((UINTN) (PciRead32 (PCI_LIB_ADDRESS (0,0,0,0x60)) & 0xFC000000) + \
         (UINTN) (Bus << 20) + \
         (UINTN) (Device << 15) + \
         (UINTN) (Function << 12) + \
         (UINTN) (Register) \
  )
#endif
//
// TxT BIOS Spec version
//
#define TXT_BIOS_SPEC_VER_MAJOR     2
#define TXT_BIOS_SPEC_VER_MINOR     1
#define TXT_BIOS_SPEC_VER_REVISION  0

//
// The following are values that are placed in the esi register when
// calling the BIOS ACM.  These constants are used as parameters to the
// TxtSetupandLaunchBiosAcm function.
//
#define TXT_LAUNCH_SCLEAN         0x00
#define TXT_RESET_EST_BIT         0x01
#define TXT_RESET_AUX             0x02
#define TXT_LAUNCH_SCHECK         0x04

#define TPM_STATUS_REG_ADDRESS    0xFED40000
#define TXT_PUBLIC_BASE           0xFED30000
#define TXT_PRIVATE_BASE          0xFED20000
#define TXT_CONFIG_SPACE_LENGTH   0x60000

//[-start-110216-IB06690094-add]//
#define TXT_VENDOR_ID_REG_OFF     0x0110
#define TXT_DEVICE_ID_REG_OFF     0x0112
//[-end-110216-IB06690094-add]//
#define TXT_ERROR_STATUS_REG_OFF  0x8
#define TXT_SINIT_BASE_REG_OFF    0x270
#define TXT_SINIT_SIZE_REG_OFF    0x278
#define TXT_SINIT_SIZE_REG_OFF2   0x27C
#define TXT_SVMM_JOIN_REG_OFF     0x290
#define TXT_SVMM_JOIN_REG_OFF2    0x294
#define TXT_HEAP_BASE_REG_OFF     0x300
#define TXT_HEAP_SIZE_REG_OFF     0x308
#define TXT_SCRATCHPAD          0x320
#define TXT_SCRATCHPAD2         0x324
#define TXT_SCRATCHPAD3         0x328
#define TXT_SCRATCHPAD4         0x32C
#define TXT_DPR_SIZE_REG_OFF    0x330

#define TXT_E2STS_REG_OFF       0x8F0
#define TXT_BLOCK_MEM_STS       BIT2
#define TXT_SECRETS_STS         BIT1
#define TXT_SLP_ENTRY_ERROR_STS BIT0

#define MCU_BASE_ADDR           TXT_SINIT_SIZE_REG_OFF
#define BIOACM_ADDR             TXT_SINIT_SIZE_REG_OFF2
#define APINIT_ADDR             TXT_SVMM_JOIN_REG_OFF
#define SEMAPHORE               TXT_SVMM_JOIN_REG_OFF2

///
/// GUIDs used by TXT drivers
///
#define PEI_BIOS_ACM_FILE_GUID \
  { \
    0x2D27C618, 0x7DCD, 0x41F5, 0xBB, 0x10, 0x21, 0x16, 0x6B, 0xE7, 0xE1, 0x43 \
  }
//[-start-110215-IB06690094-add]//
#define PEI_SINIT_ACM_FILE_GUID \
  { \
    0xcba7d52d, 0x5386, 0x495c, 0xaa, 0xc4, 0xdb, 0x32, 0x41, 0xfe, 0xd7, 0x66 \
  }
//[-end-110215-IB06690094-add]//

#define PEI_AP_STARTUP_FILE_GUID \
  { \
    0xD1E59F50, 0xE8C3, 0x4545, 0xBF, 0x61, 0x11, 0xF0, 0x02, 0x23, 0x3C, 0x97 \
  }

#define CPU_MICROCODE_FILE_GUID \
  { \
    0x17088572, 0x377F, 0x44ef, 0x8F, 0x4E, 0xB0, 0x9F, 0xFF, 0x46, 0xA0, 0x70 \
  }

//[-start-130124-IB10930014-add]//
extern EFI_GUID gTxtBiosAcmPeiFileGuid;
extern EFI_GUID gTxtSinitAcmPeiFileGuid;
//[-end-130124-IB10930014-add]//
#pragma pack(push, 1)
///
/// BIOS OS Data region definitions
///
#define BIOS_OS_DATAREGION_VERSION  4

typedef struct {
  UINT32 Type;
  UINT32 Size;
} HEAP_EXT_DATA_ELEMENT;

///
/// BIOS spec version element
///
#define HEAP_EXTDATA_TYPE_BIOS_SPEC_VER 1

typedef struct {
  HEAP_EXT_DATA_ELEMENT Header;
  UINT16                SpecVerMajor;
  UINT16                SpecVerMinor;
  UINT16                SpecVerRevision;
} HEAP_BIOS_SPEC_VER_ELEMENT;

///
/// BIOS ACM element
///
#define HEAP_EXTDATA_TYPE_BIOSACM 2
typedef struct {
  HEAP_EXT_DATA_ELEMENT Header;
  UINT32                NumAcms;
} HEAP_BIOSACM_ELEMENT;

///
/// END type
///
#define HEAP_EXTDATA_TYPE_END 0

typedef struct {
  UINT32                Version;
  UINT32                BiosSinitSize;
  EFI_PHYSICAL_ADDRESS  LcpPdBase;
  UINT64                LcpPdSize;
  UINT32                NumOfLogicalProcessors;
  UINT64                Flags;
  HEAP_EXT_DATA_ELEMENT ExtData;
} BIOS_OS_DATA_REGION;

#pragma pack(pop)
#endif
