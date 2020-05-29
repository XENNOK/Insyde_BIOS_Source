/** @file
  This function offers an interface to Hook IHISI Sub function AH=12h,function "GetOemFlashMap"
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/SmmOemSvcChipsetLib.h>

#if 0 // Sample Implementation

FBTS_PLATFORM_ROM_MAP mmOemRomMap[] = {
//  {FbtsRomMapDxe,          FixedPcdGet32 (PcdFlashFvMainBase),              FixedPcdGet32 (PcdFlashFvMainSize)                 },
//  {FbtsRomMapNVRam,        FixedPcdGet32 (PcdFlashNvStorageVariableBase),   FixedPcdGet32 (PcdFlashNvStorageVariableSize)      },
//  {FbtsRomMapCpuMicrocode, FixedPcdGet32 (PcdFlashNvStorageMicrocodeBase),  FixedPcdGet32 (PcdFlashNvStorageMicrocodeSize)},
//  {FbtsRomMapFtwState,     FixedPcdGet32 (PcdFlashNvStorageFtwWorkingBase), FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize)},
//  {FbtsRomMapFtwBackup,    FixedPcdGet32 (PcdFlashNvStorageFtwSpareBase),   FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize)},
//  {FbtsRomMapPei,          FixedPcdGet32 (PcdFlashFvRecoveryBase),          FixedPcdGet32 (PcdFlashFvRecoverySize)},
  {FbtsRomMapEos,          0,          0      },
};

//
// Add OEM private rom map table,
//
FBTS_PLATFORM_PRIVATE_ROM mmOemPrivateRomMap[] = {
//  {FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase), FixedPcdGet32 (PcdFlashNvStorageMsdmDataSize)},
  {FbtsRomMapEos,  0},
};

#endif

/**
 This function offers an interface to Hook IHISI Sub function AH=12h,function "GetOemFlashMap"

 @param[in out]     *Media_mOemRomMapSize         On entry, the size of FBTS_PLATFORM_ROM_MAP matrix.
                                                  On exit , the size of updated FBTS_PLATFORM_ROM_MAP matrix.
 @param[in out]     **Media_mOemRomMap            On entry, points to FBTS_PLATFORM_ROM_MAP matrix.
                                                  On exit , points to updated FBTS_PLATFORM_ROM_MAP matrix.
 @param[in out]     *Media_mOemPrivateRomMapSize  On entry, the size of FBTS_PLATFORM_PRIVATE_ROM matrix.
                                                  On exit , the size of updated FBTS_PLATFORM_PRIVATE_ROM matrix.
 @param[in out]     **Media_mOemPrivateRomMap     On entry, points to FBTS_PLATFORM_PRIVATE_ROM matrix.
                                                  On exit , points to updated FBTS_PLATFORM_PRIVATE_ROM matrix.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS12HookGetOemFlashMap (
  IN OUT UINTN                          *Media_mOemRomMapSize,
  IN OUT FBTS_PLATFORM_ROM_MAP          **Media_mOemRomMap,
  IN OUT UINTN                          *Media_mOemPrivateRomMapSize,
  IN OUT FBTS_PLATFORM_PRIVATE_ROM      **Media_mOemPrivateRomMap
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
  *Media_mOemRomMapSize = sizeof (mmOemRomMap) / sizeof (mmOemRomMap[0]);
  (*Media_mOemRomMap) = mmOemRomMap;
  *Media_mOemPrivateRomMapSize = sizeof (mmOemPrivateRomMap) / sizeof (mmOemPrivateRomMap[0]);
  (*Media_mOemPrivateRomMap) = mmOemPrivateRomMap;
#endif
  return EFI_UNSUPPORTED;
}


