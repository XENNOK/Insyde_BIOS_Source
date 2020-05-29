/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "FbtsOemHook.h"
#include <Library/IoLib.h>
#include <PchRegs/PchRegsLpc.h>
//[-start-121214-IB10820195-remove]//
//#define PCH_GPIO_BASE_ADDRESS   0x0500
//[-end-121214-IB10820195-remove]//
FBTS_PLATFORM_ROM_MAP mOemRomMap[] = {
//
//EC SHARED FLASH SUPPORT
//
//#ifdef FLASH_REGION_FVEC_BASE
//  {FbtsRomMapEc,           FixedPcdGet32 (PcdFlashFvEcBase),              FixedPcdGet32 (PcdFlashFvEcSize)                    },
//#endif
  {FbtsRomMapDxe,          FixedPcdGet32 (PcdFlashFvMainBase),              FixedPcdGet32 (PcdFlashFvMainSize)                 },
  {FbtsRomMapNVRam,        FixedPcdGet32 (PcdFlashNvStorageVariableBase),   FixedPcdGet32 (PcdFlashNvStorageVariableSize)      },
  {FbtsRomMapCpuMicrocode, FixedPcdGet32 (PcdFlashNvStorageMicrocodeBase),  FixedPcdGet32 (PcdFlashNvStorageMicrocodeSize)},
//#ifdef  EVENT_LOG_SUPPORT
//  {FbtsRomMapGpnv,         FLASH_REGION_NV_COMMON_STORE_SUBREGION_GPNVFVHEADER_BASE,      FLASH_REGION_NV_COMMON_STORE_SUBREGION_GPNVFVHEADER_SIZE \
//                                                                                        + FLASH_REGION_NV_COMMON_STORE_SUBREGION_HANDLE0_SIZE \
//                                                                                        + FLASH_REGION_NV_COMMON_STORE_SUBREGION_HANDLE1_SIZE \
//                                                                                        + FLASH_REGION_NV_COMMON_STORE_SUBREGION_HANDLE2_SIZE \
//                                                                                        + FLASH_REGION_NV_COMMON_STORE_SUBREGION_HANDLE3_SIZE},
//#endif
  {FbtsRomMapFtwState,     FixedPcdGet32 (PcdFlashNvStorageFtwWorkingBase), FixedPcdGet32 (PcdFlashNvStorageFtwWorkingSize)},
  {FbtsRomMapFtwBackup,    FixedPcdGet32 (PcdFlashNvStorageFtwSpareBase),   FixedPcdGet32 (PcdFlashNvStorageFtwSpareSize)},
  {FbtsRomMapPei,          FixedPcdGet32 (PcdFlashFvRecoveryBase),          FixedPcdGet32 (PcdFlashFvRecoverySize)},
  {FbtsRomMapEos,          0,          0      }

};

UINTN FBTS_OEM_ROM_MAP_COUNT = (sizeof (mOemRomMap) / sizeof (mOemRomMap[0]));

//
// Add OEM private rom map table,
//
FBTS_PLATFORM_PRIVATE_ROM mOemPrivateRomMap[] = {
  {FixedPcdGet32 (PcdFlashNvStorageMsdmDataBase), FixedPcdGet32 (PcdFlashNvStorageMsdmDataSize)},
  {FbtsRomMapEos,  0}
};

/**
  Function to get AC status.

  @return UINT8      AC status.
**/
UINT8
GetACStatus (
  VOID
  )
{
  UINT8             ACStatus;
  //UINT32            AcPresent;
//[-start-130125-IB10820224-add]//
  EFI_STATUS        Status;
//[-end-130125-IB10820224-add]//
  //
  //	Note: There is no need to check AC/DC mode for Desktop platform. 
  //
  
  //
  // GPIO31 is ACPRESENT pin.
  // Just identify this pin to determine AC or DC mode.
  //
  //AcPresent = (UINT32)((IoRead32 (PcdGet16(PcdPchGpioBaseAddress) + R_PCH_GPIO_LVL) >> 31) & 0x01);

  //if (AcPresent == 1) {
  //  //
  //  // AC mode
  //  //
  //  ACStatus = 1;
  //} else {
  //  //
  //  // DC mode
  //  //
  //  ACStatus = 0;
  //}

  ACStatus = 1;
  
  //[-start-130125-IB10820224-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS10HookGetAcStatus (&ACStatus);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS10HookGetACStatus, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
//[-end-130125-IB10820224-add]//

  return  ACStatus;
}

/**
  Function to get battery life.

  @return UINT8      battery life status.
**/
UINT8
GetBattLife (
  VOID
  )
{
  UINT8             BattLife;
//[-start-130125-IB10820224-add]//
  EFI_STATUS        Status;
//[-end-130125-IB10820224-add]//

  BattLife = 100;
//[-start-130125-IB10820224-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS10HookGetBatterylife (&BattLife);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS10HookGetBatterylife, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
//[-end-130125-IB10820224-add]//
  return  BattLife;
}

/**
  According version or OEM condtion to get the flash permission.

  @param[in]  VersionPtr         A point Pointer to FBTS_TOOLS_VERSION_BUFFER.
  @param[out] Permission         A point to save flash permission.

  @retval EFI_SUCEESS            Get Flash permission successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
**/
EFI_STATUS
GetPermission (
  IN     FBTS_TOOLS_VERSION_BUFFER  *VersionPtr,
  OUT    UINT16                     *Permission
  )
{
  UINTN ATpVersion;
//[-start-130125-IB10820224-add]//
  EFI_STATUS        Status;
//[-end-130125-IB10820224-add]//

  if (VersionPtr == NULL || Permission == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Intialial permission to FBTS_PERMISSION_ALLOWED
  //
  *Permission = FBTS_PERMISSION_ALLOWED;
  //
  // Check version signature
  //
  if (VersionPtr->Signature != FBTS_VERSION_SIGNATURE) {
    *Permission = FBTS_PERMISSION_DENY;
  }

  if (PcdGetBool (PcdAntiTheftSupported)) {
    //
    // Since IHISI v1.8.5+ supported flashing AT-p capable BIOS,
    // if the BIOS is AT-p Capable, then we do not allow old version (v1.8.5-)
    // flash tool to flash BIOS. We have AtpVersion field from 1.8.5
    //
      ATpVersion = ((UINTN) VersionPtr->ATpVersion[0] << 16) +
                   ((UINTN) VersionPtr->ATpVersion[1] << 8) +
                    (UINTN) VersionPtr->ATpVersion[2];
    if (ATpVersion < 0x313835) {
      *Permission = FBTS_PERMISSION_DENY;
    }
  }
  // OEM can add other check condition in this area
  //
//[-start-130125-IB10820224-add]//
//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS10HookGetPermission (Permission);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib  OemSvcIhisiS10HookGetPermission, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
//[-end-130125-IB10820224-add]//
  return  EFI_SUCCESS;
}

/**
  Function to get battery life.

  @retval EFI_SUCCESS      Get OEM flash map successful.
  @retval EFI_UNSUPPORTED  FBTS_OEM_ROM_MAP_COUNT is 0 or module rom map buffer is full.
**/
EFI_STATUS
GetOemFlashMap (
  VOID
  )
{
  EFI_STATUS                    Status;
  UINTN                         Count;
//[-start-130125-IB10820224-modify]//
  UINTN                                 Media_mOemRomMapSize;
  FBTS_PLATFORM_ROM_MAP                 *Media_mOemRomMap;
  UINTN                                 Media_mOemPrivateRomMapSize;
  FBTS_PLATFORM_PRIVATE_ROM             *Media_mOemPrivateRomMap;

  Media_mOemRomMapSize = FBTS_OEM_ROM_MAP_COUNT;
  Media_mOemRomMap = mOemRomMap;
  Media_mOemPrivateRomMapSize = sizeof (mOemPrivateRomMap) / sizeof (FBTS_PLATFORM_PRIVATE_ROM);
  Media_mOemPrivateRomMap = mOemPrivateRomMap;

//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS12HookGetOemFlashMap (
             &Media_mOemRomMapSize,
             &Media_mOemRomMap,
             &Media_mOemPrivateRomMapSize,
             &Media_mOemPrivateRomMap
             );
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS12HookGetOemFlashMap, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return Status;
  }

  Status = EFI_UNSUPPORTED;

  if (Media_mOemRomMapSize == 0) {
    return Status;
  }

  for (Count = 0 ; Count < Media_mOemRomMapSize; Count++) {
    Status = FillPlatformRomMapBuffer (
               Media_mOemRomMap[Count].Type,
               Media_mOemRomMap[Count].Address,
               Media_mOemRomMap[Count].Length,
               (UINT8) Count
               );
  }

  for (Count = 0; Count < Media_mOemPrivateRomMapSize; Count++ ) {
    Status = FillPlatformPrivateRomMapBuffer (
               Media_mOemPrivateRomMap[Count].LinearAddress,
               Media_mOemPrivateRomMap[Count].Size,
               (UINT8) Count
               );
  }
//[-end-130125-IB10820224-modify]#
  return Status;
}

/**
  Function to do system shutdown.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
FbtsShutDown (
  VOID
  )
{
  UINT16                        AcpiBaseAddr;
  UINT32                        Buffer;

  AcpiBaseAddr = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBaseAddr + R_PCH_SMI_EN), 1, &Buffer);
  Buffer = Buffer & ~B_PCH_SMI_EN_ON_SLP_EN;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBaseAddr + R_PCH_SMI_EN), 1, &Buffer);

  gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBaseAddr + R_PCH_ACPI_PM1_CNT), 1, &Buffer);
  Buffer = Buffer & ~(B_PCH_ACPI_PM1_CNT_SLP_EN | B_PCH_ACPI_PM1_CNT_SLP_TYP);
  Buffer |= V_PCH_ACPI_PM1_CNT_S5;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBaseAddr + R_PCH_ACPI_PM1_CNT), 1, &Buffer);
  Buffer |= B_PCH_ACPI_PM1_CNT_SLP_EN;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (AcpiBaseAddr + R_PCH_ACPI_PM1_CNT), 1, &Buffer);

  return EFI_SUCCESS;
}

/**
  Function to reboot system.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
FbtsReboot (
  IN  BOOLEAN                      IssueGlobalReset
  )
{
  UINT8                         Buffer;

  if (IssueGlobalReset) {
    PchLpcPciCfg32Or ( R_PCH_LPC_PMIR, B_PCH_LPC_PMIR_CF9GR );
  }
  //
  //PCI reset command
  //CpuIoWrite8(0xcf9,0x6);
  //

  Buffer = 6;
  gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT8, 0xcf9, 1, &Buffer);

  return EFI_SUCCESS;
}

/**
  Function to do AP check.

  @return UINT8      AP check status.
**/
UINT8
FbtsApCheck (
  VOID
  )
{
//[-start-130125-IB10820224-modify]//
  EFI_STATUS        Status;
  UINT8             ApStatus;
  //
  //Example: Platform model name and version must be checked
  //         by AP, AP allow same version file.
  //return MODEL_NAME_CHECK | MODEL_VERSION_CHECK | ALLOW_SAME_VERSION_FILE;
  //
  ApStatus = AP_DO_NOTHING;

//[-start-130524-IB05160451-modify]//
  Status = OemSvcIhisiS11HookFbtsApCheck (&ApStatus);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "SmmOemSvcChipsetLib OemSvcIhisiS11HookFbtsApCheck, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//

  return ApStatus;
//[-end-130125-IB10820224-modify]//
}

