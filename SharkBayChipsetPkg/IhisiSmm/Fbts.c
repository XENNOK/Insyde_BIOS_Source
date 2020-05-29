/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Fbts.h"
//[-start-130813-IB06720232-add]//
#include <Library/BaseOemSvcKernelLib.h>
//[-end-130813-IB06720232-add]//

//[-start-130220-IB10310017-add]//
#include <Library/SpsBiosLib.h>
#include <Protocol/SpsMeUpgrade.h>
//[-end-130220-IB10310017-add]//
//[-start-130808-IB10300050-add]//
#include <Guid/CheckFlashAccess.h>
//[-start-140103-IB05160535-remove]//
//#include <Library/UefiRuntimeServicesTableLib.h>
//[-end-140103-IB05160535-remove]//
//[-end-130808-IB10300050-add]//
#include <Library/VariableLib.h>

//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-add]//
extern UINT32                           mIhisiFlash;
//[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-modify]//
//[-start-140505-IB10310055-add]//
UINT8                                   *mNewImageBuffer      = NULL;
UINTN                                   mNewImageBufferSize   = 0;
UINTN                                   mNewImageOffset       = 0;
UINTN                                   mNewImageRemainCount  = 0;
//[-end-140505-IB10310055-add]//

EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL       *mSmmFwBlockService;
FBTS_PLATFORM_ROM_MAP_BUFFER            mRomMapBuffer;
BOOLEAN                                 mFlashME;
FBTS_PLATFORM_PRIVATE_ROM_BUFFER        mPrivateRomMapBuffer;
UINT8                                   mA0A1MEdelta;
//[-start-130717-IB05400431-modify]//
FLASH_BASE_MAP_TABLE                    mFlashBaseMapTable[] = {{FLASH_SIZE_1024K, ROM_1M_BASE },
                                                                {FLASH_SIZE_2048K, ROM_2M_BASE },
                                                                {FLASH_SIZE_4096K, ROM_4M_BASE },
                                                                {FLASH_SIZE_8192K, ROM_8M_BASE },
                                                                {FLASH_SIZE_16384K, ROM_16M_BASE },
                                                                {FLASH_SIZE_32768K, ROM_32M_BASE }
                                                               };
//[-end-130717-IB05400431-modify]//

FLASH_MAP_MAPPING_TABLE mFlashMapTypeMappingTable[] = {
  {FbtsRomMapPei,           EFI_FLASH_AREA_RECOVERY_BIOS},
  {FbtsRomMapCpuMicrocode,  EFI_FLASH_AREA_CPU_MICROCODE},
  {FbtsRomMapNVRam,         EFI_FLASH_AREA_EFI_VARIABLES},
  {FbtsRomMapDxe,           EFI_FLASH_AREA_MAIN_BIOS},
  {FbtsRomMapEc,            EFI_FLASH_AREA_FV_EC},
  {FbtsRomMapNvStorage,     EFI_FLASH_AREA_GUID_DEFINED},
  {FbtsRomMapFtwBackup,     EFI_FLASH_AREA_FTW_BACKUP},
  {FbtsRomMapFtwState,      EFI_FLASH_AREA_FTW_STATE},
  {FbtsRomMapSmbiosLog,     EFI_FLASH_AREA_SMBIOS_LOG},
  {FbtsRomMapOemData,       EFI_FLASH_AREA_OEM_BINARY},
  {FbtsRomMapGpnv,          EFI_FLASH_AREA_GPNV},
  {FbtsRomMapDmiFru,        EFI_FLASH_AREA_DMI_FRU},
  {FbtsRomMapPalB,          EFI_FLASH_AREA_PAL_B},
  {FbtsRomMapMcaLog,        EFI_FLASH_AREA_MCA_LOG},
  {FbtsRomMapPassword,      EFI_FLASH_AREA_RESERVED_03},
  {FbtsRomMapOemNvs,        EFI_FLASH_AREA_RESERVED_04},
  {FbtsRomMapReserved07,    EFI_FLASH_AREA_RESERVED_07},
  {FbtsRomMapReserved08,    EFI_FLASH_AREA_RESERVED_08},
  {FbtsRomMapReserved0A,    EFI_FLASH_AREA_RESERVED_0A},
  {FbtsRomMapUnused,        EFI_FLASH_AREA_UNUSED},
  {FbtsRomMapUndefined,     EFI_FLASH_AREA_GUID_DEFINED},
  {FbtsRomMapFactoryCopy,   EFI_FLASH_AREA_RESERVED_09}, // factory default
  {FbtsRomMapEos,           FbtsRomMapEos}               //End of table
};

//[-start-130409-IB10310017-add]//
EFI_STATUS
SpsGetPlatformRomMapHook (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINT8                                Entry;
  UINTN                                Index;
  UINTN                                NumberOfProtectRegion;
  SPS_ME_PROTECT_ROM_MAP               *ProtectRomMap;

  Entry                 = 0;
  Index                 = 0;
  NumberOfProtectRegion = 0;
  ProtectRomMap         = NULL;

  ///
  /// Get ME protect ROM map
  ///
  Status = SpsGetProtectRomMap ((VOID **)&ProtectRomMap, &NumberOfProtectRegion);
  if (EFI_ERROR (Status) || (ProtectRomMap == NULL) || (NumberOfProtectRegion == 0)) {
    return Status;
  }

  ///
  /// Update protect ROM map
  ///
  for (Entry = 0;
       Entry < (sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER) / sizeof (FBTS_PLATFORM_ROM_MAP));
       Entry ++) {
    if (mRomMapBuffer.PlatFormRomMap[Entry].Type == (UINT8) FbtsRomMapEos) {
      break;
    }
  }

  for (Index = 0; Index < NumberOfProtectRegion; Index ++) {
    if ((ProtectRomMap[Index].Attribute == SPS_ME_PROTECT_ROM_OVERRIDABLE) &&
        (Entry < (sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER) / sizeof (FBTS_PLATFORM_ROM_MAP)))
        ) {
      mRomMapBuffer.PlatFormRomMap[Entry].Type    = (UINT8) FbtsRomMapUndefined;
      mRomMapBuffer.PlatFormRomMap[Entry].Address = (UINT32) ProtectRomMap[Index].ProtectBase;
      mRomMapBuffer.PlatFormRomMap[Entry].Length  = (UINT32) (ProtectRomMap[Index].ProtectLimit - ProtectRomMap[Index].ProtectBase + 1);
      Entry ++;
    }
  }
  ASSERT (Entry < (sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER) / sizeof (FBTS_PLATFORM_ROM_MAP)));
  FillPlatformRomMapBuffer ((UINT8) FbtsRomMapEos, 0, 0, Entry);

  ///
  /// Update private ROM map
  ///
  for (Entry = 0;
      Entry < (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER) / sizeof (FBTS_PLATFORM_PRIVATE_ROM));
      Entry ++) {
    if (mPrivateRomMapBuffer.PlatFormRomMap[Entry].LinearAddress == (UINT32) FbtsRomMapEos) {
      break;
    }
  }

  for (Index = 0; Index < NumberOfProtectRegion; Index ++) {
    if ((ProtectRomMap[Index].Attribute == SPS_ME_PROTECT_ROM_NON_OVERRIDABLE) &&
        (Entry < (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER) / sizeof (FBTS_PLATFORM_PRIVATE_ROM)))
        ) {
      mPrivateRomMapBuffer.PlatFormRomMap[Entry].LinearAddress  = (UINT32) ProtectRomMap[Index].ProtectBase;
      mPrivateRomMapBuffer.PlatFormRomMap[Entry].Size           = (UINT32) (ProtectRomMap[Index].ProtectLimit - ProtectRomMap[Index].ProtectBase + 1);
      Entry ++;
    }
  }
  ASSERT (Entry < (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER) / sizeof (FBTS_PLATFORM_PRIVATE_ROM)));

  FillPlatformPrivateRomMapBuffer ((UINT32) FbtsRomMapEos, 0, Entry);

  if (ProtectRomMap != NULL) {
    gSmst->SmmFreePool (ProtectRomMap);
    ProtectRomMap = NULL;
  }

  return EFI_SUCCESS;
}
//[-end-130409-IB10310017-add]//

/**
  Check the flash region whether is used.

  @param[in] FlashRegion  Flash Region x Register (x = 0 - 3).

  @retval TRUE            This  region is used
  @retval FALSE           This  region is not used
**/
STATIC
BOOLEAN
CheckFlashRegionIsValid (
  IN UINT32   FlashRegion
  )
{
  BOOLEAN         Flag = TRUE;

  //
  // the Region Base must be programmed to 0x1FFFh and the Region Limit
  // to 0x0000h to disable the region.
  //
  // Now, the tool programmed 0x0fff to base and 0x000 to limit to indicate
  // this is region is not used.
  //

  //
  //The limit block address must be greater than or equal to the base block address
  //
  if ((FlashRegion & 0x1fff) > (FlashRegion >> 16 & 0x1fff)) {
    Flag = FALSE;
  }

  return Flag;
}

/**
  Get corresponding IHISI area type by flash area type accordingly.

  @param[in] FlashAreaType  Flash area type.

  @return UINT8      Corresponding IHISI area type.
**/
STATIC
UINT8
GetMappingType (
  UINT8  FlashAreaType
  )
{
  UINTN Index = 0;

  while (mFlashMapTypeMappingTable[Index].FlashAreaType != FbtsRomMapEos) {
    if (FlashAreaType == mFlashMapTypeMappingTable[Index].FlashAreaType) {
      return mFlashMapTypeMappingTable[Index].IhisiAreaType;
    } else {
      Index++;
    }
  }

  return FbtsRomMapUndefined;
}

/**
  AH=1Eh, Get whole BIOS ROM map (Internal BIOS map)

  @retval EFI_SUCCESS        Get whole BIOS ROM map successful.
**/
EFI_STATUS
FbtsGetWholeBiosRomMap (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT32                                IhisiStatus;

  IhisiStatus = FbtsUnknowPlatformRomMap;
  Status = FbtsLibGetWholeBiosRomMap (&IhisiStatus);

  if (EFI_ERROR(Status)) {
    IhisiLibErrorCodeHandler (IhisiStatus);
    return Status;
  }

  IhisiLibErrorCodeHandler (EFI_SUCCESS);
  return EFI_SUCCESS;
}

/**
  AH=10h, Get FBTS supported version and FBTS permission.

  @retval EFI_SUCCESS        Get whole BIOS ROM map successful.
**/
EFI_STATUS
FbtsGetSupportVersion (
  VOID
  )
{
  UINT16                          Permission;
  FBTS_PLATFORM_STATUS_BUFFER     *PlatformInfoPtr;
  FBTS_TOOLS_VERSION_BUFFER       *VersionPtr;
  UINT32                           Eax;
  UINT32                           Ecx;
  UINT32                           Edx;

  VersionPtr = (FBTS_TOOLS_VERSION_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  Permission = FBTS_PERMISSION_DENY;
  GetPermission (VersionPtr, &Permission);
  //
  //Get AC Status and BattLife.
  //Note: Define in Oemhook
  //
  PlatformInfoPtr = (FBTS_PLATFORM_STATUS_BUFFER *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  ZeroMem (PlatformInfoPtr, sizeof(FBTS_PLATFORM_STATUS_BUFFER));
  PlatformInfoPtr->AcStatus = GetACStatus ();
  PlatformInfoPtr->Battery  = GetBattLife ();
  PlatformInfoPtr->Bound    = 20;
  PlatformInfoPtr->Customer = FBTS_VENDER_ID;

  Eax = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Ecx = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Edx = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDX);

  Eax = (UINT32)((Eax & 0xffff0000) | Permission);
  Ecx = (UINT32)((Ecx & 0xffff0000) | FBTS_VERSION);
  Edx = (UINT32)((Edx & 0x00000000) | (UINT8)(gSmst->NumberOfCpus));
  Edx = (UINT32)(Edx << 8);
  Edx = (UINT32)(Edx | (UINT8) IhisiLibGetCpuIndex ());

  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx);
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDX, Edx);

  return EFI_SUCCESS;
}

/**
  AH=11h, Get platform information.

  @retval EFI_SUCCESS        Get platform information successful.
  @return Other              Get platform information failed.
**/
EFI_STATUS
FbtsGetPlatformInfo (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINT8                                 ApCheck;
  UINT32                                IhisiStatus;
  UINT32                                Eax;

  Status = FbtsLibGetPlatformInfo (&IhisiStatus);
  IhisiLibErrorCodeHandler (IhisiStatus);

  //
  //Ap check name
  //
  ApCheck = FbtsApCheck();
  Eax = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX);
  Eax = (UINT32) ((Eax & 0xffff00ff) | (ApCheck<<8));
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RAX, Eax);

  return Status;
}

/**
  AH=12h, Get Platform ROM map protection.

  @retval EFI_SUCCESS        Get Platform ROM map protection successful.
**/
EFI_STATUS
FbtsGetPlatformRomMap (
  VOID
  )
{
  UINT32                                IhisiStatus;


  FbtsLibGetPlatformRomMap (&mRomMapBuffer, &mPrivateRomMapBuffer, &IhisiStatus);

  IhisiLibErrorCodeHandler (IhisiStatus);

  return EFI_SUCCESS;
}

/**
   AH=13h, Get Flash part information.

  @retval EFI_SUCCESS        Get Flash part information successful.
  @return Other              Get Flash part information failed.
**/
EFI_STATUS
FbtsGetFlashPartInfo (
  VOID
  )
{
  EFI_STATUS                     Status;
  UINT32                         IhisiStatus;

  Status = FbtsLibGetFlashPartInfo (mSmmFwBlockService, &IhisiStatus);
  IhisiLibErrorCodeHandler (IhisiStatus);

  return Status;
}

/**
   AH=14h, FBTS Read.

  @retval EFI_SUCCESS        Read data from flash part successful.
  @return Other              Read data from flash part failed.
**/
EFI_STATUS
FbtsRead (
  VOID
  )
{
  EFI_STATUS                     Status;
  UINT32                         IhisiStatus;

//[-start-121002-IB05300338-add]//
  if (mGlobalNvsArea->PfatMemAddress != 0x00) {
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }
//[-end-121002-IB05300338-add]//

  if (*(mSmmFwBlockService->FlashMode) == SMM_FW_DEFAULT_MODE) {
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
//[-start-130809-IB06720232-modify]//
      OemSvcEcIdle (TRUE);
//[-end-130809-IB06720232-modify]//
    }
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
  }

//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-modify]//
  mIhisiFlash = CHECK_FLASH_ACCESS_DO_NOT_CONVERT;
  Status = FbtsLibRead (mSmmFwBlockService, &IhisiStatus);
  mIhisiFlash = CHECK_FLASH_ACCESS_CONVERT;
//[-end-130808-IB10300050-modify]//
//[-end-140103-IB05160535-modify]//
//[-start-130415-IB10310017-add]//
  if (FeaturePcdGet (PcdSpsMeSupported)) {
    Status = SpsUpgradeFbtsReadHook (Status, &IhisiStatus);
  }
//[-end-130415-IB10310017-add]//
  IhisiLibErrorCodeHandler (IhisiStatus);

  return Status;
}

/**
  AH=15h, FBTS Write.

  @retval EFI_SUCCESS        Write data to flash part successful.
  @return Other              Write data to flash part failed.
**/
EFI_STATUS
FbtsWrite (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       WriteSize;
  UINTN                       RomBaseAddress;
  FLASH_DEVICE                *Buffer;
  FLASH_REGION                *FlashRegionPtr;
  UINTN                       ROMBase;
  UINTN                       ROMOffset;
  UINT8                       *OutputDataBuffer;
  UINT8                       Index;
  UINT8                       SpiFlashNumber;
  UINT8                       WholeROMSizeIndex;
  UINT32                      IhisiStatus;
  UINT16                      PmBase;
  UINT32                      SmiEnSave;
  UINT32                      Data32;

  WriteSize         = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
//[-start-130131-IB10820226-modify]//
  RomBaseAddress    = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
//[-end-130131-IB10820226-modify]//

  OutputDataBuffer  = NULL;
  ROMBase           = 0;
  ROMOffset         = 0;
  SpiFlashNumber    = 0;
  WholeROMSizeIndex = 0;
  PmBase            = 0;
  SmiEnSave         = 0;
  Data32            = 0;

//[-start-121002-IB05300338-add]//
  if (mGlobalNvsArea->PfatMemAddress != 0x00) {
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }
//[-end-121002-IB05300338-add]//

  if (*(mSmmFwBlockService->FlashMode) == SMM_FW_DEFAULT_MODE) {
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
//[-start-130809-IB06720232-modify]//
      OemSvcEcIdle (TRUE);
//[-end-130809-IB06720232-modify]//
    }
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
  }
  if ( !mFlashME ) {
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (FLASH_DEVICE),
                      (VOID **)&Buffer
                      );
    if (!EFI_ERROR ( Status ) ) {
      Status = mSmmFwBlockService->DetectDevice (
                                     mSmmFwBlockService,
                                     (UINT8 *) Buffer
                                     );
    }
    if (!EFI_ERROR (Status )) {
      Status = mSmmFwBlockService->GetSpiFlashNumber (
                                     mSmmFwBlockService,
                                     (UINT8 *)&SpiFlashNumber
                                     );
    }
    if ( EFI_ERROR ( Status ) ) {
      DEBUG ((EFI_D_ERROR, "Get ROM Information Failure!\n"));
    }

    if (!EFI_ERROR (Status)) {
      if (Buffer->DeviceInfo.Size == 0xFF) {
        ROMBase = (0x100000000 - (Buffer->DeviceInfo.BlockMap.Mutiple * Buffer->DeviceInfo.BlockMap.BlockSize) * 0x100);
        ROMOffset = RomBaseAddress - ROMBase;
      } else {
        WholeROMSizeIndex = Buffer->DeviceInfo.Size;
        if ( SpiFlashNumber == 2 ) {
          WholeROMSizeIndex = WholeROMSizeIndex + 1;
        }

        for (Index = 0; Index < (sizeof (mFlashBaseMapTable) / sizeof (FLASH_BASE_MAP_TABLE)); Index = Index + 1) {
          if (mFlashBaseMapTable[Index].SizeIndex == WholeROMSizeIndex) {
            ROMBase   = mFlashBaseMapTable[Index].Base;
            ROMOffset = RomBaseAddress - ROMBase;
            break;
          }
        }
      }

      Status = gSmst->SmmAllocatePool (
                        EfiRuntimeServicesData,
                        (sizeof (FLASH_REGION) * TOTAL_FLASH_REGION),
                        (VOID **)&OutputDataBuffer
                        );
   
      if (!EFI_ERROR ( Status )) {
        Status = mSmmFwBlockService->GetFlashTable (
                                       mSmmFwBlockService,
                                       OutputDataBuffer
                                       );
        if (!EFI_ERROR (Status)) {
          FlashRegionPtr = (FLASH_REGION *) OutputDataBuffer;
          for (Index = DESC_REGION; Index < MAX_FLASH_REGION; Index = Index + 1, FlashRegionPtr++) {
            if (Index == ME_REGION) {
              if (!(((UINT32) (ROMOffset + WriteSize - 1) < FlashRegionPtr->Offset) ||
                  ((FlashRegionPtr->Offset + FlashRegionPtr->Size - 1) < (UINT32) (ROMOffset)))) {
                mFlashME = TRUE;
                break;
              }
            }
          }
        }
      }
      if (EFI_ERROR( Status )) {
        DEBUG (( EFI_D_ERROR, "Get ROM Map Information Failure!\n"));
      }
    }
  }

  if (mFlashME == TRUE) {
    PmBase = (UINT16) (PchLpcPciCfg32 (R_PCH_LPC_ACPI_BASE) & B_PCH_LPC_ACPI_BASE_BAR);
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &SmiEnSave);
    Data32 = SmiEnSave & ~B_PCH_SMI_EN_GBL_SMI;
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &Data32);
  }

//[-start-130220-IB10310017-add]//
  if (FeaturePcdGet (PcdSpsMeSupported)) {
//[-start-140311-IB10310052-modify]//
    ///
    /// Case : (Upgrade Mode : SPS_ME_UPGRADE_MODE_FULL_UPGRADE)
    /// 1. ME updated by Driver success. AP skip all ME region.
    ///    Status : EFI_WRITE_PROTECTED.
    ///    Reason : Upgrade Done, Ap Skip ME region please.
    ///    
    ///    Status : EFI_NOT_READY.
    ///    Reason : Upgrade is running and not complete, Call again please.
    ///
    /// 2. ME updated by Driver fail. Abort this progress. AP should stop.
    ///    Status : EFI_ABORTED.
    ///    Reason : Maybe new image is invlid.
    ///
    /// Case : (Upgrade Mode : SPS_ME_UPGRADE_MODE_ONLY_ENABLE_HMRFPO)
    /// Case : (Upgrade Mode : SPS_ME_UPGRADE_MODE_DEFAULT)
    /// 3. ME update by Ap. 
    ///    Status : EFI_SUCCESS.
    ///    Reason : Maybe new image information not enough to verify by driver.
    ///             So... BIOS trust the user...
    ///
    Status = SpsUpgradeProgress (RomBaseAddress, WriteSize);
    if (mFlashME && (Status == EFI_WRITE_PROTECTED)) {
      gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &SmiEnSave);
      mFlashME    = FALSE;
      IhisiStatus = FbtsSkipThisWriteBlock;
      IhisiLibErrorCodeHandler (IhisiStatus);
      DEBUG ((EFI_D_ERROR, "[SPS UPGRADE] INFO: IhisiStatus (%d)\n", IhisiStatus));
      return EFI_SUCCESS;
    }
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[SPS UPGRADE] INFO: SPS Upgrade Progress (%r)\n", Status));
    }
//[-end-140311-IB10310052-modify]//
  }
//[-end-130220-IB10310017-add]//
//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-modify]//
  mIhisiFlash = CHECK_FLASH_ACCESS_DO_NOT_CONVERT;
  Status = FbtsLibWrite (mSmmFwBlockService, &IhisiStatus);
  mIhisiFlash = CHECK_FLASH_ACCESS_CONVERT;
//[-end-130808-IB10300050-modify]//
//[-end-140103-IB05160535-modify]//

  if (mFlashME == TRUE) {
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &SmiEnSave);
  }

  if ((EFI_ERROR (Status)) && (mFlashME)) {
    mFlashME = FALSE;
    DEBUG ((EFI_D_ERROR, "Write ME Failure!\n"));
  }

  IhisiLibErrorCodeHandler (IhisiStatus);

  return Status;
}

/**
  AH=16h, This function uses to execute some specific action after the flash process is completed.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
Fbtscomplete (
  VOID
  )
{
  UINT8                       ApRequest;
  EFI_STATUS                  Status;
//[-start-130327-IB08340100-add]//
  FBTS_FLASH_COMPLETE_STATUS  *FlashCompleteStatus;
//[-end-130327-IB08340100-add]//


  ApRequest = (UINT8) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  *(mSmmFwBlockService->FlashMode) = SMM_FW_DEFAULT_MODE;

//[-start-130327-IB08340100-add]//
  FlashCompleteStatus = (FBTS_FLASH_COMPLETE_STATUS *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  if ((FlashCompleteStatus->Signature == FLASH_COMPLETE_STATUS_SIGNATURE) &&
      (FlashCompleteStatus->SturctureSize == sizeof (FBTS_FLASH_COMPLETE_STATUS))) {

    switch (FlashCompleteStatus->CompleteStatus) {

    case ApTerminated:
      break;

    case NormalFlash:
      Status = FbtsLibComplete ();
      break;

    case PartialFlash:
      break;

    default:
      break;
    }
  }
//[-end-130327-IB08340100-add]//

  if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
//[-start-130809-IB06720232-modify]//
    OemSvcEcIdle (TRUE);
//[-end-130809-IB06720232-modify]//
  }

  if (PcdGetBool (PcdMe5MbSupported) && !mFlashME) {
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, MEFlashReset, V_ME_NEED_BIOS_SYNC);
  }

//[-start-130220-IB10310017-add]//
  if (FeaturePcdGet (PcdSpsMeSupported)) {
    ///
    /// Notify the SPS ME upgrade driver and SEC module that SPS ME is upgreded
    ///
    Status = SpsUpgradeComplete ();
    if (EFI_ERROR (Status) && (Status != EFI_UNSUPPORTED)) {
      DEBUG ((EFI_D_INFO, "[SPS UPGRADE] INFO: SPS Upgrade Complete (%r)\n", Status));
    }
  }
//[-end-130220-IB10310017-add]//

  switch(ApRequest) {

  case 0x00: //Do not thing
    if (mFlashME) {
      WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, MEFlashReset, V_ME_GLOBAL_RESET);
      mFlashME = FALSE;
    }
    Status = EFI_SUCCESS;
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
    break;

  case 0x01: //Shut down
    //
    // Note: Shut down by Oem hook
    //
    if (mFlashME) {
      WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, MEFlashReset, V_ME_GLOBAL_RESET);
      mFlashME = FALSE;
    }
    Status = FbtsShutDown ();
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
    break;

  case 0x02: //Reboot
    //
    // Note: Reboot by Oem hook
    //
    //
    // if ME has been flashed, issue a global reset.
    //
    Status = FbtsReboot (mFlashME);
    mFlashME = FALSE;
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
    break;

  default:
    if (mFlashME) {
      WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, MEFlashReset, V_ME_GLOBAL_RESET);
      mFlashME = FALSE;
    }
    Status = EFI_SUCCESS;
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
    break;
  }
   return Status;
}

/**
  Passing information to flash program on whether
  if current BIOS (AT-p capable) can be flashed with
  other BIOS ROM file
  AT-p: (Anti-Theft PC Protection).

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
FbtsGetATpInformation (
  VOID
  )
{
  EFI_GUID                              mSystemConfigurationGuid  = SYSTEM_CONFIGURATION_GUID;
//[-start-130710-IB05160465-modify]//
  VOID                                  *SetupVariable;
//[-end-130710-IB05160465-modify]//
  UINTN                                 DataSize;
  UINT32                                Ecx_data = 0;
  BOOLEAN                               Enough_Time = FALSE;
  EFI_STATUS                            Status;

  if (!PcdGetBool (PcdAntiTheftSupported)) {
      Ecx_data &= ~(bit(7));
      //
      // Return ECX for tool to determin proper error message.
      // Please refer to IHISI spec.
      //
      IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx_data);
      IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (EFI_SUCCESS));
      return EFI_SUCCESS;
  }

//[-start-130709-IB05160465-modify]//
  DataSize = PcdGet32 (PcdSetupConfigSize);
  SetupVariable = AllocateZeroPool (DataSize);
//[-start-140625-IB05080432-add]//
  if (SetupVariable == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//

  Status = CommonGetVariable (
             L"Setup",
             &mSystemConfigurationGuid,
             &DataSize,
             SetupVariable
             );
  ASSERT_EFI_ERROR (Status);
//[-start-140625-IB05080432-add]//
  if (EFI_ERROR(Status)) {
    return Status;
  }
//[-end-140625-IB05080432-add]//

  //
  // AT4.0 is Enable
  //
  if (((CHIPSET_CONFIGURATION *)SetupVariable)->AtConfig == 1) {

    // Return value:
    // Bit[7:0] is defined as follow:
    // bit[7]: Current platform BIOS support AT4.0 (1:support, 0:not support)
    // bit[6~3]: Reserved
    // bit[2]: Dtimer value, timer should not expire during flashing BIOS (1:Enough time, 0:Not enough)
    // bit[1]: Platform is NOT under stolen mode, (1:NOT stolen, 0:Stolen)
    //         Warning: value = 1 is platform NOT stolen, people usually misunderstand.
    // bit[0]: Platform is enrolled (1:Enroll, 0:Not enroll)
    //

    //
    // Current BIOS is AT4.0 Capable
    //
    Ecx_data |= bit(7);

    //
    // Not enroll, Not stolen
    //
    if (((CHIPSET_CONFIGURATION *)SetupVariable)->AtState == TDT_STATE_INACTIVE) {
      Ecx_data |= bit(1);
    }else \
    //
    // Enroll, Not stolen
    // Prompt user to un-enroll
    //
    if (((CHIPSET_CONFIGURATION *)SetupVariable)->AtState == TDT_STATE_ACTIVE) {
      Ecx_data |= bit(0);
      Ecx_data |= bit(1);
    }else \
    //
    // Stolen, SHOULD NOT FLASH ~~!
    //
    if (((CHIPSET_CONFIGURATION *)SetupVariable)->AtState == TDT_STATE_LOCK_OR_STOLEN) {
      Ecx_data |= bit(0);
    }

    //
    // work around for caller to bypass this function.
    // For reason, please see ATpTimerCheck() function call discription.
    //
    Enough_Time = TRUE;

    if (Enough_Time) {
      Ecx_data |= bit(2);
    }


  } else {
    //
    // Current platform BIOS is not AT4.0 Capable
    //
    Ecx_data &= ~(bit(7));
  }

  //
  // Return ECX for tool to determin proper error message.
  // Please refer to IHISI spec.
  //
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, Ecx_data);
  IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (EFI_SUCCESS));

  FreePool (SetupVariable);
//[-end-130709-IB05160465-modify]//

  return EFI_SUCCESS;
}

/**
  Get platform and Rom file flash descriptor region.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
GetRomFileAndPlatformTable (
  VOID
  )
{
  UINTN           DataSize;
  UINT8           *OutputDataBuffer;
  UINT8           *InputDataBuffer;
  FLASH_REGION    *FlashRegionPtr;
  EFI_STATUS      Status;

  Status = EFI_UNSUPPORTED;
  DataSize = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  if (DataSize == 0) {
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));

    return Status;
  }
  OutputDataBuffer = (UINT8 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);

  //
  //we must get platform region table through read chipset register, because
  //1. we can override access permissions in the Flash Descriptor through BMWAG and BMRAG
  //2. Flash regions may be haven't read/write access.
  //
  Status = mSmmFwBlockService->GetFlashTable (
                                 mSmmFwBlockService,
                                 OutputDataBuffer
                                 );
//[-start-130409-IB10310017-add]//
  if (FeaturePcdGet (PcdSpsMeSupported)) {
    Status = SpsPlatformTableHook (OutputDataBuffer);
  }
//[-end-130409-IB10310017-add]//

  InputDataBuffer  = (UINT8 *)(UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);
  OutputDataBuffer += ROM_FILE_REGION_OFFSET;
  FlashRegionPtr = (FLASH_REGION *) OutputDataBuffer;


  if (*((UINT32 *) (InputDataBuffer + 0x10)) == FLASH_VALID_SIGNATURE) {
    mA0A1MEdelta = 0x10; // B0 stepping, ME has been shift 0x10 bytes
  }
  else{
    mA0A1MEdelta = 0x0; // else keep what it is
  }

  //
  //check Rom file is whether descriptor mode
  //

  if (*((UINT32 *)(InputDataBuffer + mA0A1MEdelta)) == FLASH_VALID_SIGNATURE) {
    Status = GetRomFileFlashTable (InputDataBuffer, FlashRegionPtr, TRUE);
  } else {
    Status = GetRomFileFlashTable (InputDataBuffer, FlashRegionPtr, FALSE);
  }

  IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (Status));
  return Status;
}

/**
  Skip module check allows and binary file transmissions.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
SkipMcCheckAndBinaryTrans (
  VOID
  )
{
  //
  // return IhisiSuccess to allow skip module check. If doesn't allow
  // skip module check, please return FbtsCannotSkipModuleCheck (0x27)
  //
  //
  IhisiLibErrorCodeHandler ((UINT32)IhisiSuccess);

  return EFI_SUCCESS;
}

/**
  Get Flash table from Rom file.
  if DescriptorMode is true, the FlashTable will be filled.
  if the descriptor is false,the FlashTable will be filled RegionTypeEos(0xff) directly.

  @param[in]  InputDataBuffer    the pointer to Rom file.
  @param[in]  DataBuffer         IN: the input buffer address.
                                 OUT:the flash region table from rom file.
  @param[in]  DescriptorMode     TRUE is indicate this Rom file is descriptor mode
                                 FALSE is indicate this Rom file is non-descriptor mode

  @retval EFI_SUCCESS            Successfully returns
**/
EFI_STATUS
GetRomFileFlashTable (
  IN       UINT8           *InputDataBuffer,
  IN OUT   FLASH_REGION    *DataBuffer,
  IN       BOOLEAN         DescriptorMode
  )
{
  UINT8           Index;
  UINT32          *FlashRegionPtr;
  UINT32          Frba;
  UINT32          *FlashMasterPtr;
  UINT32          Fmba;
  UINT32          FlashMap0Reg;
  UINT32          FlashMap1Reg;
  UINT32          ReadAccess;
  UINT32          WriteAccess;

  //
  //calulate Flash region base address
  //
  FlashMap0Reg = *((UINT32 *)(InputDataBuffer + FLASH_MAP_0_OFFSET + mA0A1MEdelta));
  Frba = (FlashMap0Reg & FLASH_REGION_BASE_MASK) >> 12;
  FlashRegionPtr = (UINT32 *)(InputDataBuffer + Frba);
  //
  //calulate Flash master base address
  //

  FlashMap1Reg = *((UINT32 *)(InputDataBuffer + FLASH_MAP_1_OFFSET + mA0A1MEdelta));

  Fmba = (FlashMap1Reg & FLASH_MASTER_BASE_MASK) << 4;
  FlashMasterPtr = (UINT32 *)(InputDataBuffer + Fmba);

  if (DescriptorMode) {
    for (Index = DESC_REGION; Index < MAX_FLASH_REGION; Index++,  FlashRegionPtr++) {
      if (CheckFlashRegionIsValid (*FlashRegionPtr)){

        DataBuffer->Type = Index;
        DataBuffer->Offset = (*FlashRegionPtr & 0x1fff) << 12;
        DataBuffer->Size = ((*FlashRegionPtr >> 16 & 0x1fff) - (*FlashRegionPtr & 0x1fff) + 1) << 12;
        //
        //Bios primary master always has access permissions to it's primary region
        //
        if (Index == BIOS_REGION) {
          DataBuffer->Access = ACCESS_AVAILABLE;
        } else {
          ReadAccess = (*FlashMasterPtr >> (FLASH_MASTER_1_READ_ACCESS_BIT + Index)) & ACCESS_AVAILABLE;
          WriteAccess = (*FlashMasterPtr >> (FLASH_MASTER_1_WRITE_ACCESS_BIT + Index)) & ACCESS_AVAILABLE;
          if (ReadAccess == ACCESS_AVAILABLE && WriteAccess == ACCESS_AVAILABLE) {
            DataBuffer->Access = ACCESS_AVAILABLE;
          } else {
            DataBuffer->Access = ACCESS_NOT_AVAILABLE;
          }
        }
        DataBuffer++;
      }
    }
  }
  DataBuffer->Type = REGION_TYPE_OF_EOS;

  return EFI_SUCCESS;
}

//[-start-140505-IB10310055-add]//
/**
  Initialize IHISI 50h relative services

  @retval EFI_SUCCESS        Initialize IHISI 50h services successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
PassImageInit (
  VOID
  )
{
  EFI_STATUS            Status;
  UINTN                 SpiSize;
  FLASH_DEVICE          *Buffer;
  UINT8                 SpiFlashNumber;

  SpiSize        = 0;
  SpiFlashNumber = 0;

  if (mSmmFwBlockService == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmFwBlockServiceProtocolGuid,
                      NULL,
                      (VOID **)&mSmmFwBlockService
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  Status = mSmmFwBlockService->GetSpiFlashNumber (mSmmFwBlockService, (UINT8 *)&SpiFlashNumber);
  if (EFI_ERROR (Status)) {
    //
    // Default Spi Flash number to 1.
    //
    SpiFlashNumber = 1;
  }

  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (FLASH_DEVICE),
                    &Buffer
                    );
  if (!EFI_ERROR (Status)) {
    Status = mSmmFwBlockService->DetectDevice (mSmmFwBlockService, (UINT8 *) Buffer);
    if (!EFI_ERROR (Status)) {
      if (Buffer->DeviceInfo.Size == 0xFF) {
        SpiSize = (Buffer->DeviceInfo.BlockMap.Mutiple * Buffer->DeviceInfo.BlockMap.BlockSize) * 0x100;
      } else {
        switch (Buffer->DeviceInfo.Size) {
          case FLASH_SIZE_128K:
            SpiSize = SIZE_128KB;
            break;
          case FLASH_SIZE_256K:
            SpiSize = SIZE_256KB;
            break;
          case FLASH_SIZE_512K:
            SpiSize = SIZE_512KB;
            break;
          case FLASH_SIZE_1024K:
            SpiSize = SIZE_1MB;
            break;
          case FLASH_SIZE_2048K:
            SpiSize = SIZE_2MB;
            break;
          case FLASH_SIZE_4096K:
            SpiSize = SIZE_4MB;
            break;
          case FLASH_SIZE_8192K:
            SpiSize = SIZE_8MB;
            break;
          case FLASH_SIZE_16384K:
            SpiSize = SIZE_16MB;
            break;
          default:
            SpiSize = SIZE_16MB;
            break;
        }
      }
    }
    Status = gSmst->SmmFreePool (Buffer);
    if (!EFI_ERROR (Status)) {
      Buffer = NULL;
    }
  }

  //
  // If Spi size is 0, set Spi size to 16 MB, make sure enough buffer.
  //
  if (SpiSize == 0) {
    SpiSize = SIZE_16MB;
  }
  if (SpiFlashNumber == 2) {
    SpiSize = SpiSize * 2;
  }

  //
  // Increase 1MB for future use.
  //
  SpiSize += SIZE_1MB;

  Status = gBS->AllocatePool (
                  EfiReservedMemoryType,
                  SpiSize,
                  &mNewImageBuffer
                  );
  if (!EFI_ERROR (Status)) {
    mNewImageBufferSize = SpiSize;
  } else {
    DEBUG ((EFI_D_ERROR, "Allocate memory => (%r)\n", Status));
    mNewImageBuffer = NULL;
  }

  return Status;
}
//[-end-140505-IB10310055-add]//

/**
  Initialize Fbts relative services

  @retval EFI_SUCCESS        Initialize Fbts services successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
FbtsInit (
  VOID
  )
{
  EFI_STATUS                  Status;
//[-start-140103-IB05160535-remove]//
////[-start-130808-IB10300050-add]//
//  UINTN                       VarBufferData;
//  UINTN                       VarBufferSize;
////[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-remove]//

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    (VOID **)&mSmmFwBlockService
                    );

  //
  //Get Oem Flash Map
  //Note: Define in Oem hook.
  //
  Status = GetOemFlashMap ();

  //
  //If Get Oem Flash Map fail.
  //Try to get Flash Map By Hob dynamically.
  //
  if (EFI_ERROR (Status)) {
    Status = GetFlashMapByHob ();
  }

//[-start-130409-IB10310017-add]//
  if (FeaturePcdGet (PcdSpsMeSupported)) {
    SpsGetPlatformRomMapHook ();
  }
//[-end-130409-IB10310017-add]//
//[-start-140505-IB10310055-add]//
  PassImageInit ();
//[-end-140505-IB10310055-add]//

//[-start-140103-IB05160535-remove]//
////[-start-130808-IB10300050-add]//
//  //
//  // Get mail box variable
//  //
//  VarBufferSize = sizeof (UINTN);
//  Status = gRT->GetVariable(
//                  CHECK_FLASH_ACCESS_VARIABLE,
//                  &gCheckFlashAccessGuid,
//                  NULL,
//                  &VarBufferSize,
//                  &VarBufferData
//                  );
//
//  if (!EFI_ERROR(Status)) {
//    //
//    // Mail box is allocated, use it
//    //
//    mIhisiFlashPtr = (UINT32 *)VarBufferData;
//
//  } else {
//    //
//    // Mail box is not allocated, create it
//    //
//    Status = gBS->AllocatePool(
//                    EfiReservedMemoryType,
//                    sizeof (UINT32),
//                    (VOID **)&mIhisiFlashPtr
//                    );
//
//    //
//    // Initialize it to convert address and set mail box address to variable
//    //
//    *mIhisiFlashPtr = CHECK_FLASH_ACCESS_CONVERT;
//    VarBufferData = (UINTN)mIhisiFlashPtr;
//	VarBufferSize = sizeof (UINTN);
//    Status = gRT->SetVariable(
//                    CHECK_FLASH_ACCESS_VARIABLE,
//                    &gCheckFlashAccessGuid,
//                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
//                    VarBufferSize,
//                    &VarBufferData
//                    );
//  }
////[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-remove]//

  return Status;
}

/**
  Get flash map from HOB.

  @retval EFI_SUCCESS        Get flash map from HOB successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
GetFlashMapByHob (
  VOID
  )
{
  EFI_FLASH_MAP_ENTRY_DATA      *FlashMapEntryData;
  VOID                          *HobList;
  EFI_STATUS                    Status;
  UINT8                         Index;

  HobList = GetHobList ();
  ASSERT (HobList != NULL);

  Index = 0;
  do {
    HobList = GetNextGuidHob (&gEfiFlashMapHobGuid, HobList);
    if (HobList == NULL) {
      //
      // Fill end of structure ROM map if all of flash map HOBs have been filled to ROM map buffer.
      //
      Status = FillPlatformRomMapBuffer ((UINT8)FbtsRomMapEos, 0, 0, Index);
      break;
    }

    FlashMapEntryData = (EFI_FLASH_MAP_ENTRY_DATA *) HobList;
    Status = FillPlatformRomMapBuffer (GetMappingType (
               FlashMapEntryData->AreaType),
               (UINT32) (FlashMapEntryData->Entries[0].Base),
               (UINT32) (FlashMapEntryData->Entries[0].Length),
               Index
               );
    Index++;
  } while (!EFI_ERROR (Status));

  return Status;
}

/**
 Fill platform protect ROM map information to module ROM map buffer.

 @retval            EFI_SUCCESS         Set platform protect ROM map information to module ROM map buffer successful.
 @retval            EFI_UNSUPPORTED     Module ROM map buffer is full.
*/
EFI_STATUS
FillPlatformRomMapBuffer (
  IN UINT8    Type,
  IN UINT32   Address,
  IN UINT32   Length,
  IN UINT8    Entry
  )
{
  EFI_STATUS    Status;
  UINTN         ConvertedAddress = 0;

//[-start-130110-IB10870066-modify]//
  if (Entry >= (sizeof (FBTS_PLATFORM_ROM_MAP_BUFFER) / sizeof (FBTS_PLATFORM_ROM_MAP))) {
   return EFI_UNSUPPORTED;
  }
//[-end-130110-IB10870066-modify]//

  if (Type != FbtsRomMapEos) {
    Status = mSmmFwBlockService->ConvertToSpiAddress(
                                   mSmmFwBlockService,
                                   (UINTN) Address,
                                   &ConvertedAddress
                                   );
    if (!EFI_ERROR (Status)) {
      Address = (UINT32) ConvertedAddress;
    }
  }

  mRomMapBuffer.PlatFormRomMap[Entry].Type    = Type;
  mRomMapBuffer.PlatFormRomMap[Entry].Address = Address;
  mRomMapBuffer.PlatFormRomMap[Entry].Length  = Length;

  return EFI_SUCCESS;
}

/**
 Fill platform private ROM map information to module ROM map buffer.

 @retval            EFI_SUCCESS         Set platform ROM map information to module ROM map buffer successful.
 @retval            EFI_UNSUPPORTED     Module ROM map buffer is full.
*/
EFI_STATUS
FillPlatformPrivateRomMapBuffer (
  IN UINT32   Address,
  IN UINT32   Length,
  IN UINT8    Entry
  )
{
  EFI_STATUS    Status;
  UINTN         ConvertedAddress;

//[-start-130110-IB10870066-modify]//
  if (Entry >= (sizeof (FBTS_PLATFORM_PRIVATE_ROM_BUFFER) / sizeof (FBTS_PLATFORM_PRIVATE_ROM))) {

   return EFI_UNSUPPORTED;
  }
//[-end-130110-IB10870066-modify]//
  if (Address != FbtsRomMapEos) {
    Status = mSmmFwBlockService->ConvertToSpiAddress(
                                   mSmmFwBlockService,
                                   (UINTN) Address,
                                   &ConvertedAddress
                                   );
    if (!EFI_ERROR (Status)) {
      Address = (UINT32) ConvertedAddress;
    }
  }
  mPrivateRomMapBuffer.PlatFormRomMap[Entry].LinearAddress = Address;
  mPrivateRomMapBuffer.PlatFormRomMap[Entry].Size = Length;

  return EFI_SUCCESS;
}

//[-start-140505-IB10310055-add]//
/**
  IHISI Function 50h.
  (Tool is master to pass image to BIOS via the interface.)

  @param None.

  @retval EFI_SUCCESS           Image is valid so far.
  @retval EFI_UNSUPPORTED       Image is valid and the last check is bypassed.
  @retval EFI_BAD_BUFFER_SIZE   Invalid data size.
  @retval Others                Image is invalid
*/
EFI_STATUS
FbtsPassImage (
  VOID
  )
{
  EFI_STATUS            Status;
  IHISI_STATUS_CODE     ReturnStatus;
  UINTN                 Address;
  UINTN                 Length;
  UINT32                VerificationFeature;

  Address             = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  Length              = IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  Status              = EFI_UNSUPPORTED;
  VerificationFeature = 0;

  //
  // When EDI (input register) is 0,
  // ECX (input register) represents Block Number of the image size.
  // (Block Number = Total Image size (bytes) / 64KB).
  //
  // When EDI (input register) is NOT 0,
  // it means starting to pass image data with max size 64KB for each transfer.
  // ECX (input register) represents Current Transferring Image data size (byte).
  // ( It should be less and equal to 0x10000 = 64KB )
  //
  if (Address == 0) {

    mNewImageRemainCount = Length;
    //
    // Check Buffer size is enough.
    //
    if ((Length * SIZE_64KB) > mNewImageBufferSize) {
      ReturnStatus = IhisiObLenTooSmall;
      IhisiLibErrorCodeHandler ( ReturnStatus );
      return Status;
    }
  } else {
    if (mNewImageBuffer == NULL) {
      ReturnStatus = IhisiInvalidParamFatalError;
      IhisiLibErrorCodeHandler ( ReturnStatus );
      return Status;
    }
    CopyMem ((mNewImageBuffer + mNewImageOffset), (UINT8 *)((UINTN)(Address)), Length);
    mNewImageOffset += Length;
    mNewImageRemainCount--;

    if (mNewImageRemainCount == 0) {
      //
      // Last Block Transfered
      //
      VerificationFeature = 0;

//[-start-140311-IB10310052-add]//
      //
      // Pass New image to driver.
      //
      if (FeaturePcdGet (PcdSpsMeSupported)) {
        //
        // Pass new image to SPS ME firmware Upgrade driver.
        //
        Status = SpsUpgradePassImage (mNewImageBuffer, mNewImageOffset);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "[SPS UPGRADE] INFO: Pass image to SPS upgrade driver => (%r)\n", Status));
        } else {
          //
          // Set SPS Upgrade mode to Full upgrade mode.
          //
          Status = SpsUpgradeSetMode (SPS_ME_UPGRADE_MODE_FULL_UPGRADE);
          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "[SPS UPGRADE] INFO: Set mode => (%r)\n", Status));
          }
        }
      }
//[-end-140311-IB10310052-add]//
    }
  }

  ReturnStatus = IhisiSuccess;
  Status       = EFI_SUCCESS;

  //
  // Set Verifiy reseult to ECX.
  //
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, (UINT32) VerificationFeature);

  IhisiLibErrorCodeHandler ( ReturnStatus );
  return Status;
}

/**
  IHISI Function 51h.
  Inform BIOS to write image data to SPI ROM.

  @retval EFI_SUCCESS        Write data to flash part successful.
  @return Other              Write data to flash part failed.
**/
EFI_STATUS
FbtsWriteImageToAddress (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       WriteSize;
  UINTN                       RomBaseAddress;
  FLASH_DEVICE                *Buffer;
  FLASH_REGION                *FlashRegionPtr;
  UINTN                       ROMBase;
  UINTN                       ROMOffset;
  UINT8                       *OutputDataBuffer;
  UINT8                       Index;
  UINT8                       SpiFlashNumber;
  UINT8                       WholeROMSizeIndex;
  UINT32                      IhisiStatus;
  UINT16                      PmBase;
  UINT32                      SmiEnSave;
  UINT32                      Data32;
  UINTN                       WriteOffset;
  UINT8                       *WriteDataBuffer;

  if (mNewImageBuffer == NULL) {
    IhisiLibErrorCodeHandler (IhisiObLenTooSmall);
    return EFI_OUT_OF_RESOURCES;
  }

  WriteSize         = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX);
  RomBaseAddress    = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RDI);
  WriteOffset       = (UINTN) IhisiLibGetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI);

  OutputDataBuffer  = NULL;
  ROMBase           = 0;
  ROMOffset         = 0;
  SpiFlashNumber    = 0;
  WholeROMSizeIndex = 0;
  PmBase            = 0;
  SmiEnSave         = 0;
  Data32            = 0;

  WriteDataBuffer = (UINT8 *) (UINTN)(mNewImageBuffer + WriteOffset);
  IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI, (UINT32)(UINTN) WriteDataBuffer);

//[-start-121002-IB05300338-add]//
  if (mGlobalNvsArea->PfatMemAddress != 0x00) {
    IhisiLibErrorCodeHandler (FbtsLibStatusTranslation (EFI_UNSUPPORTED));
    return EFI_UNSUPPORTED;
  }
//[-end-121002-IB05300338-add]//

  if (*(mSmmFwBlockService->FlashMode) == SMM_FW_DEFAULT_MODE) {
    if (PcdGetBool (PcdEcSharedFlashSupported) && !PcdGetBool (PcdEcIdlePerWriteBlockSupported)) {
//[-start-130809-IB06720232-modify]//
      OemSvcEcIdle (TRUE);
//[-end-130809-IB06720232-modify]//
    }
    *(mSmmFwBlockService->FlashMode) = SMM_FW_FLASH_MODE;
  }
  if ( !mFlashME ) {
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof (FLASH_DEVICE),
                      (VOID **)&Buffer
                      );
    if (!EFI_ERROR ( Status ) ) {
      Status = mSmmFwBlockService->DetectDevice (
                                     mSmmFwBlockService,
                                     (UINT8 *) Buffer
                                     );
    }
    if (!EFI_ERROR (Status )) {
      Status = mSmmFwBlockService->GetSpiFlashNumber (
                                     mSmmFwBlockService,
                                     (UINT8 *)&SpiFlashNumber
                                     );
    }
    if ( EFI_ERROR ( Status ) ) {
      DEBUG ((EFI_D_ERROR, "Get ROM Information Failure!\n"));
    }

    if (!EFI_ERROR (Status)) {
      if (Buffer->DeviceInfo.Size == 0xFF) {
        ROMBase = (0x100000000 - (Buffer->DeviceInfo.BlockMap.Mutiple * Buffer->DeviceInfo.BlockMap.BlockSize) * 0x100);
        ROMOffset = RomBaseAddress - ROMBase;
      } else {
        WholeROMSizeIndex = Buffer->DeviceInfo.Size;
        if ( SpiFlashNumber == 2 ) {
          WholeROMSizeIndex = WholeROMSizeIndex + 1;
        }

        for (Index = 0; Index < (sizeof (mFlashBaseMapTable) / sizeof (FLASH_BASE_MAP_TABLE)); Index = Index + 1) {
          if (mFlashBaseMapTable[Index].SizeIndex == WholeROMSizeIndex) {
            ROMBase   = mFlashBaseMapTable[Index].Base;
            ROMOffset = RomBaseAddress - ROMBase;
            break;
          }
        }
      }

      Status = gSmst->SmmAllocatePool (
                        EfiRuntimeServicesData,
                        (sizeof (FLASH_REGION) * TOTAL_FLASH_REGION),
                        (VOID **)&OutputDataBuffer
                        );

      if (!EFI_ERROR ( Status )) {
        Status = mSmmFwBlockService->GetFlashTable (
                                       mSmmFwBlockService,
                                       OutputDataBuffer
                                       );
        if (!EFI_ERROR (Status)) {
          FlashRegionPtr = (FLASH_REGION *) OutputDataBuffer;
          for (Index = DESC_REGION; Index < MAX_FLASH_REGION; Index = Index + 1, FlashRegionPtr++) {
            if (Index == ME_REGION) {
              if (!(((UINT32) (ROMOffset + WriteSize - 1) < FlashRegionPtr->Offset) ||
                  ((FlashRegionPtr->Offset + FlashRegionPtr->Size - 1) < (UINT32) (ROMOffset)))) {
                mFlashME = TRUE;
                break;
              }
            }
          }
        }
      }
      if (EFI_ERROR( Status )) {
        DEBUG (( EFI_D_ERROR, "Get ROM Map Information Failure!\n"));
      }
    }
  }

  if (mFlashME == TRUE) {
    PmBase = (UINT16) (PchLpcPciCfg32 (R_PCH_LPC_ACPI_BASE) & B_PCH_LPC_ACPI_BASE_BAR);
    gSmst->SmmIo.Io.Read (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &SmiEnSave);
    Data32 = SmiEnSave & ~B_PCH_SMI_EN_GBL_SMI;
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &Data32);
  }

//[-start-140311-IB10310052-add]//
  if (FeaturePcdGet (PcdSpsMeSupported)) {
    ///
    /// Case : (Upgrade Mode : SPS_ME_UPGRADE_MODE_FULL_UPGRADE)
    /// 1. ME updated by Driver success. AP skip all ME region.
    ///    Status : EFI_WRITE_PROTECTED.
    ///    Reason : Upgrade Done, Ap Skip ME region please.
    ///    
    ///    Status : EFI_NOT_READY.
    ///    Reason : Upgrade is running and not complete, Call again please.
    ///
    /// 2. ME updated by Driver fail. Abort this progress. AP should stop.
    ///    Status : EFI_ABORTED.
    ///    Reason : Maybe new image is invlid.
    ///
    /// Case : (Upgrade Mode : SPS_ME_UPGRADE_MODE_ONLY_ENABLE_HMRFPO)
    /// Case : (Upgrade Mode : SPS_ME_UPGRADE_MODE_DEFAULT)
    /// 3. ME update by Ap. 
    ///    Status : EFI_SUCCESS.
    ///    Reason : Maybe new image information not enough to verify by driver.
    ///             So... BIOS trust the user...
    ///
    Status = SpsUpgradeProgress (RomBaseAddress, WriteSize);
    if (Status == EFI_WRITE_PROTECTED) {
      IhisiStatus = FbtsSkipThisWriteBlock;
      IhisiLibErrorCodeHandler (IhisiStatus);
      return EFI_SUCCESS;
    }
    ///
    /// Set CX bit[0] to 1, AP will waiting BIOS ready.
    ///
    if (Status == EFI_NOT_READY) {
      IhisiStatus = IhisiSuccess;
      IhisiLibErrorCodeHandler (IhisiStatus);
      IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RCX, (UINT32)(0x01));
      return EFI_SUCCESS;
    }
    ///
    /// Set SI bit[0] to 1, AP will abort this progress.
    ///
    if (Status == EFI_ABORTED) {
      IhisiStatus = FbtsPermissionDeny;
      IhisiLibErrorCodeHandler (IhisiStatus);
      IhisiLibSetDwordRegister (EFI_SMM_SAVE_STATE_REGISTER_RSI, (UINT32)(0x01));
      return EFI_SUCCESS;
    }
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "[SPS UPGRADE] INFO: SPS Upgrade Progress (%r)\n", Status));
    }
  }
//[-end-140311-IB10310052-add]//

//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-modify]//
  mIhisiFlash = CHECK_FLASH_ACCESS_DO_NOT_CONVERT;
  Status = FbtsLibWrite (mSmmFwBlockService, &IhisiStatus);
  mIhisiFlash = CHECK_FLASH_ACCESS_CONVERT;
//[-end-130808-IB10300050-modify]//
//[-end-140103-IB05160535-modify]//

  if (mFlashME == TRUE) {
    gSmst->SmmIo.Io.Write (&gSmst->SmmIo, SMM_IO_UINT32, (PmBase + R_PCH_SMI_EN), 1, &SmiEnSave);
  }

  if ((EFI_ERROR (Status)) && (mFlashME)) {
    mFlashME = FALSE;
    DEBUG ((EFI_D_ERROR, "Write ME Failure!\n"));
  }

  IhisiLibErrorCodeHandler (IhisiStatus);

  return Status;
}
//[-end-140505-IB10310055-add]//

