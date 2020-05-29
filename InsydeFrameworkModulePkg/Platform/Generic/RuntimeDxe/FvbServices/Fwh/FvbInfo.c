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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  FvbInfo.c

Abstract:

  Defines data structure that is the volume header found.These data is intent
  to decouple FVB driver with FV header.

--*/

#include "Tiano.h"
#include "FlashLayout.h"
#include "EfiRuntimeLib.h"

#include EFI_GUID_DEFINITION (SystemNvDataGuid)
#include EFI_GUID_DEFINITION (FirmwareFileSystem)
#include EFI_PROTOCOL_DEFINITION (FirmwareVolumeBlock)

#define FVB_MEDIA_BLOCK_SIZE        FIRMWARE_BLOCK_SIZE
#define FV_RECOVERY_BASE_ADDRESS    FLASH_REGION_FV_RECOVERY_BASE
#define RECOVERY_BOIS_BLOCK_NUM     (FLASH_REGION_FV_RECOVERY_SIZE / FVB_MEDIA_BLOCK_SIZE)
#define FV_MAIN_BASE_ADDRESS        FLASH_REGION_FVMAIN_BASE
#define MAIN_BOIS_BLOCK_NUM         (FLASH_REGION_FVMAIN_SIZE / FVB_MEDIA_BLOCK_SIZE)
#define SYSTEM_NV_BLOCK_NUM         2
#define NV_STORAGE_BASE_ADDRESS     FLASH_REGION_NVSTORAGE_SUBREGION_NV_VARIABLE_STORE_BASE

typedef struct {
  EFI_PHYSICAL_ADDRESS        BaseAddress;
  EFI_FIRMWARE_VOLUME_HEADER  FvbInfo;
  //
  //EFI_FV_BLOCK_MAP_ENTRY    ExtraBlockMap[n];//n=0
  //
  EFI_FV_BLOCK_MAP_ENTRY      End[1];
} EFI_FVB_MEDIA_INFO;


EFI_FVB_MEDIA_INFO mPlatformFvbMediaInfo[] = {
  //
  // Recovery BOIS FVB
  //
  {
    FV_RECOVERY_BASE_ADDRESS,
    {
      {0,}, //ZeroVector[16]
      EFI_FIRMWARE_FILE_SYSTEM_GUID,
      FVB_MEDIA_BLOCK_SIZE * RECOVERY_BOIS_BLOCK_NUM,
      EFI_FVH_SIGNATURE,
      EFI_FVB_READ_ENABLED_CAP | EFI_FVB_READ_STATUS | EFI_FVB_WRITE_ENABLED_CAP | EFI_FVB_WRITE_STATUS,
      sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY),
      0,    //CheckSum
      {0,}, //Reserved[3]
      1,    //Revision
      {
        RECOVERY_BOIS_BLOCK_NUM,
        FVB_MEDIA_BLOCK_SIZE,
      }
    },
    {
      0,
      0
    }
  },
  //
  // Main BIOS FVB
  //
  {
    FV_MAIN_BASE_ADDRESS,
    {
      {0,}, //ZeroVector[16]
      EFI_FIRMWARE_FILE_SYSTEM_GUID,
      FVB_MEDIA_BLOCK_SIZE * MAIN_BOIS_BLOCK_NUM,
      EFI_FVH_SIGNATURE,
      EFI_FVB_READ_ENABLED_CAP | EFI_FVB_READ_STATUS | EFI_FVB_WRITE_ENABLED_CAP | EFI_FVB_WRITE_STATUS,
      sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY),
      0,    //CheckSum
      {0,}, //Reserved[3]
      1,    //Revision
      {
        MAIN_BOIS_BLOCK_NUM,
        FVB_MEDIA_BLOCK_SIZE,
      }
    },
    {
      0,
      0
    }
  },
#if FLASH_REGION_NVSTORAGE_SIZE == ((FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_SPARE_SIZE) * 2)
  //
  // Systen NvStorage FVB
  //
  {
    NV_STORAGE_BASE_ADDRESS,
    {
      {0,}, //ZeroVector[16]
      EFI_SYSTEM_NV_DATA_HOB_GUID,
      FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_SPARE_SIZE * SYSTEM_NV_BLOCK_NUM,
      EFI_FVH_SIGNATURE,
      EFI_FVB_READ_ENABLED_CAP | EFI_FVB_READ_STATUS | EFI_FVB_WRITE_ENABLED_CAP | EFI_FVB_WRITE_STATUS,
      sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY),
      0,    //CheckSum
      {0,}, //Reserved[3]
      1,    //Revision
      {
        SYSTEM_NV_BLOCK_NUM,
        FLASH_REGION_NVSTORAGE_SUBREGION_NV_FTW_SPARE_SIZE,
      }
    },
    {
      0,
      0
    }
  }
#else
  //
  // Systen NvStorage FVB
  //
  {
    NV_STORAGE_BASE_ADDRESS,
    {
      {0,}, //ZeroVector[16]
      EFI_SYSTEM_NV_DATA_HOB_GUID,
      FLASH_REGION_NVSTORAGE_SIZE,
      EFI_FVH_SIGNATURE,
      EFI_FVB_READ_ENABLED_CAP | EFI_FVB_READ_STATUS | EFI_FVB_WRITE_ENABLED_CAP | EFI_FVB_WRITE_STATUS,
      sizeof (EFI_FIRMWARE_VOLUME_HEADER) + sizeof (EFI_FV_BLOCK_MAP_ENTRY),
      0,    //CheckSum
      {0,}, //Reserved[3]
      1,    //Revision
      {
        FLASH_REGION_NVSTORAGE_SIZE / FVB_MEDIA_BLOCK_SIZE,
        FVB_MEDIA_BLOCK_SIZE,
      }
    },
    {
      0,
      0
    }
  }
#endif
};



UINT16
CalculateChecksum16 (
  IN UINT16       *Buffer,
  IN UINTN        Size
  )
/*++

Description:

  This function calculates the value needed for a valid UINT16 checksum

Input:

  Buffer      Pointer to buffer containing byte data of component.
  Size        Size of the buffer in UINT16 unit

Return:

  The 16 bit checksum value needed.

--*/
{
  UINTN   Index;
  UINT16  Sum;

  Sum = 0;
  //
  // Perform the word sum for buffer
  //
  for (Index = 0; Index < Size; Index++) {
    Sum = (UINT16) (Sum + Buffer[Index]);
  }

  return (UINT16) (0x10000 - Sum);
}


EFI_STATUS
GetFvbInfo (
  IN  EFI_PHYSICAL_ADDRESS         FvBaseAddress,
  OUT EFI_FIRMWARE_VOLUME_HEADER   **FvbInfo
  )
{
  UINTN   Index;

  for (Index=0; Index < sizeof (mPlatformFvbMediaInfo)/sizeof (EFI_FVB_MEDIA_INFO); Index += 1) {
    if (mPlatformFvbMediaInfo[Index].BaseAddress == FvBaseAddress) {
      *FvbInfo =  &mPlatformFvbMediaInfo[Index].FvbInfo;

      DEBUG ((EFI_D_ERROR, "\nBaseAddr: 0x%lx \n", FvBaseAddress));
      DEBUG ((EFI_D_ERROR, "FvLength: 0x%lx \n", (*FvbInfo)->FvLength));
      DEBUG ((EFI_D_ERROR, "HeaderLength: 0x%x \n", (*FvbInfo)->HeaderLength));
      DEBUG ((EFI_D_ERROR, "FvBlockMap[0].NumBlocks: 0x%x \n", (*FvbInfo)->FvBlockMap[0].NumBlocks));
      DEBUG ((EFI_D_ERROR, "FvBlockMap[0].BlockLength: 0x%x \n", (*FvbInfo)->FvBlockMap[0].BlockLength));
      DEBUG ((EFI_D_ERROR, "FvBlockMap[1].NumBlocks: 0x%x \n",   (*FvbInfo)->FvBlockMap[1].NumBlocks));
      DEBUG ((EFI_D_ERROR, "FvBlockMap[1].BlockLength: 0x%x \n\n", (*FvbInfo)->FvBlockMap[1].BlockLength));

      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


VOID
RecalculateFvHeaderChecksum (
  VOID
  )
/*++

Description:

  This function calculates the Fv header's checksum

Input:

  None

Return:

  None

--*/
{
  UINTN                       Index;
  EFI_FIRMWARE_VOLUME_HEADER  *FvHeader;

  for (Index = 0; Index < sizeof (mPlatformFvbMediaInfo) / sizeof (EFI_FVB_MEDIA_INFO); Index++) {
    FvHeader            = &mPlatformFvbMediaInfo[Index].FvbInfo;
    FvHeader->Checksum  = CalculateChecksum16 ((UINT16 *) FvHeader, FvHeader->HeaderLength / sizeof (UINT16));
  }

  return ;
}
