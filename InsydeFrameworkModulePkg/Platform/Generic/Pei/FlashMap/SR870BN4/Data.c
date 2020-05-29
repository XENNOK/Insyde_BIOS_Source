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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Data.c

Abstract:

  The data mFlashAreaData for Flashmap.c

--*/

#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "flashlayout.h"

#include EFI_GUID_DEFINITION (FlashMapHob)
#include EFI_GUID_DEFINITION (SystemNvDataGuid)

EFI_FLASH_AREA_DATA           mFlashAreaData[] = {
  //
  // North FV1
  //
  {
    NORTH_FV1_BASE,
    NORTH_FV1_LENGTH,
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
    EFI_FLASH_AREA_RECOVERY_BIOS,
    0, 0, 0,
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  },
  //
  // North FV
  //
  {
    NORTH_FV2_BASE,
    NORTH_FV2_LENGTH,
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
    EFI_FLASH_AREA_RECOVERY_BIOS,
    0, 0, 0,
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  },
  //
  // South FV1 (Main FV)
  //
  {
    SOUTH_FV1_BASE,
    SOUTH_FV1_LENGTH,
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
    EFI_FLASH_AREA_MAIN_BIOS,
    0, 0, 0,
    { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
  },
  //
  // System Non-Volatile Storage FV
  //
  {
    NV_STORAGE_FV_BASE,
    NV_STORAGE_FV_LENGTH,
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
    EFI_FLASH_AREA_GUID_DEFINED,
    0, 0, 0,
    EFI_SYSTEM_NV_DATA_HOB_GUID
  },
  //
  // Storage area for MCA Logs
  //
  {
    MCA_STORAGE_FV_BASE,
    MCA_STORAGE_FV_LENGTH,
    EFI_FLASH_AREA_FV | EFI_FLASH_AREA_MEMMAPPED_FV,
    EFI_FLASH_AREA_GUID_DEFINED,
    0, 0, 0,
    EFI_SYSTEM_NV_DATA_HOB_GUID
  },
};

EFI_HOB_FLASH_MAP_ENTRY_TYPE  mFlashMapHobData[] = {
  //
  // NVSTORAGE.NV_VARIABLE_STORE Subregion
  //
  {
    EFI_HOB_TYPE_GUID_EXTENSION,
    sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE),
    0,
    EFI_FLASH_MAP_HOB_GUID,
    0,
    0,
    0,
    EFI_FLASH_AREA_EFI_VARIABLES,
    {
      0x00000000,
      0x0000,
      0x0000,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
    },
    1,
    {
      EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
      0,
      EFI_VARIABLE_STORE_BASE,
      EFI_VARIABLE_STORE_LENGTH,
      {
        0x00000000,
        0x0000,
        0x0000,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
      },
    },
  },
  //
  // NVSTORAGE.NV_FTW_WORKING Subregion
  //
  {
    EFI_HOB_TYPE_GUID_EXTENSION,
    sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE),
    0,
    EFI_FLASH_MAP_HOB_GUID,
    0,
    0,
    0,
    EFI_FLASH_AREA_FTW_STATE,
    {
      0x00000000,
      0x0000,
      0x0000,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
    },
    1,
    {
      EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
      0,
      EFI_FTW_WORKING_BASE,
      EFI_FTW_WORKING_LENGTH,
      {
        0x00000000,
        0x0000,
        0x0000,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
      },
    },
  },
  //
  // NVSTORAGE.NV_FTW_SPARE Subregion
  //
  {
    EFI_HOB_TYPE_GUID_EXTENSION,
    sizeof (EFI_HOB_FLASH_MAP_ENTRY_TYPE),
    0,
    EFI_FLASH_MAP_HOB_GUID,
    0,
    0,
    0,
    EFI_FLASH_AREA_FTW_BACKUP,
    {
      0x00000000,
      0x0000,
      0x0000,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00,
      0x00
    },
    1,
    {
      EFI_FLASH_AREA_SUBFV | EFI_FLASH_AREA_MEMMAPPED_FV,
      0,
      EFI_FTW_SPARE_BASE,
      EFI_FTW_SPARE_LENGTH,
      {
        0x00000000,
        0x0000,
        0x0000,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00
      },
    },
  },
};

//
// Get number of types
//
UINTN                         mNumOfFlashAreaData   = sizeof (mFlashAreaData) / sizeof (mFlashAreaData[0]);
UINTN                         mNumOfHobFlashMapData = sizeof (mFlashMapHobData) / sizeof (mFlashMapHobData[0]);
