//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  ChipsetLibServices.h

Abstract:

  ChipsetLib Services PPI


--*/

#ifndef _CHIPSET_LIB_SERVICES_PPI_H_
#define _CHIPSET_LIB_SERVICES_PPI_H_

#include "BootMode.h"
#include "SetupConfig.h"
#include "OemClkGen.h"

#define CHIPSET_LIB_SERVICES_PPI_GUID \
  { \
    0x733C2319, 0xDBD5, 0x4E77, 0xBF, 0x54, 0xD6, 0x75, 0xD8, 0x66, 0x6C, 0x08 \
  }

EFI_FORWARD_DECLARATION (CHIPSET_LIB_SERVICES_PPI);

#define CHIPSET_LIB_SERVICES_PPI_VERSION  1

typedef
EFI_STATUS 
(EFIAPI *ON_START_OF_TCG_PEI) (
  IN      VOID                            **PeiServices,
  IN OUT  BOOLEAN                         *NoNeedToInitTpm
);
  
typedef 
EFI_STATUS
(EFIAPI *INIT_FLASH_MODE) (
  IN      UINT8                           *FlashMode
);
/*++

Routine Description:

  Initialize Flash mode pointer for libary to use

Arguments:

  FlashMode       - FlashMode pointer

Returns:

  EFI_SUCCESS

--*/

typedef
EFI_STATUS
(EFIAPI *ENABLE_FVB_WRITES) (
  IN      BOOLEAN                         EnableWrites
);
/*++

Routine Description:

  Platform specific function to enable flash / hardware

Arguments:

  EnableWrites       - Boolean to enable/disable flash

Returns:
  EFI_SUCCESS

--*/

typedef
VOID
(EFIAPI *RESET_SYSTEM) (
  IN      EFI_RESET_TYPE                  ResetType,
  IN      EFI_STATUS                      ResetStatus,
  IN      UINTN                           DataSize,
  IN      CHAR16                          *ResetData OPTIONAL
);
/*++

Routine Description:

  Reset the system.

Arguments:
  
    ResetType - warm or cold
    ResetStatus - possible cause of reset
    DataSize - Size of ResetData in bytes
    ResetData - Optional Unicode string
    For details, see efiapi.h

Returns:
  Does not return if the reset takes place.
  EFI_INVALID_PARAMETER   If ResetType is invalid.

--*/

typedef
VOID
(EFIAPI *PROGRAM_GPIO) (
  VOID
);

typedef
EFI_STATUS
(EFIAPI *READ_NBVID) (
  IN      EFI_PEI_SERVICES                **PeiServices,
  OUT     UINT16                          *VenderID
);

typedef
EFI_STATUS
(EFIAPI *GET_VGA_TYPE_INFO) (
  IN      EFI_PEI_SERVICES                **PeiServices,
  OUT     CLOCK_GEN_VGA_TYPE              *VgaType
);


typedef
EFI_STATUS
(EFIAPI *LEGACY_REGION_ACCESS_CTRL) (
  IN      UINTN                           Start,
  IN      UINTN                           Length,
  IN      UINTN                           Mode  
);
/*++

Routine Description:
  LegacyRegionAccessCtrl

Arguments:
  Start               Start of the region to lock or unlock.
  Length              Length of the region.
  Mode                If  LEGACY_REGION_ACCESS_LOCK, then LegacyRegionAccessCtrl ()
                      lock the specific legacy region. 
                      
                      If  LEGACY_REGION_ACCESS_UNLOCK, then LegacyRegionAccessCtrl ()
                      unlock the specific legacy region.
                      
                      Other value LegacyRegionAccessCtrl () will return EFI_INVALID_PARAMETER .

Returns:
  EFI_INVALID_PARAMETER        Incorrect input parameter.
  EFI_UNSUPPORTED              The specific region is unsupported.
  EFI_SUCCESS                  The specific legacy region lock or unlock successfully.
  
--*/


//
// Ppi data structure
//    Version & Size are for compatibility.
//    After Caller get a ChipsetLibServicesPpi, it should check its'
//         1. Version is same.  It should equal to 
//            CHIPSET_LIB_SERVICES_PPI_VERSION. If the versions are same that
//            means the functions in the Ppis are same in meaning and arguments.
//         2. Size is equal or larger. It equals
//            sizeof(CHIPSET_LIB_SERVICES_PPI). Larger or equal size means the
//            Ppi has at least caller necessary functions.
//    If one of these are not true, it is not compatible. 
//
typedef struct _CHIPSET_LIB_SERVICES_PPI {
  UINTN                               Version;
  UINTN                               Size;
  ON_START_OF_TCG_PEI                 OnStartOfTcgPei;
  INIT_FLASH_MODE                     InitFlashMode;
  ENABLE_FVB_WRITES                   EnableFvbWrites;
  RESET_SYSTEM                        ResetSystem;
  PROGRAM_GPIO                        ProgramGpio;
  READ_NBVID                          ReadNBVID;
  GET_VGA_TYPE_INFO                   GetVgaTypeInfo;  
  LEGACY_REGION_ACCESS_CTRL           LegacyRegionAccessCtrl;
} CHIPSET_LIB_SERVICES_PPI;

extern EFI_GUID gChipsetLibServicesPpiGuid;

#endif
