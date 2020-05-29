/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcStage2MemoryDiscoverCallback().
 The function PeiCsSvcStage2MemoryDiscoverCallback() use chipset services to install
 Firmware Volume Hob's once there is main memory.

***************************************************************************
* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/ChipsetCpuLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
//#include <Guid/MtrrDataInfo.h>
#include <Ppi/CpuIo.h>
#include <Ppi/PciCfg2.h>
#include <Ppi/Cache.h>
//[-start-121201-IB03780468-remove]//
//#include <Ppi/Wdtt/Wdtt.h>
//[-end-121201-IB03780468-remove]//
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/EmuPei.h>
#include <SaRegs.h>
#include <PchAccess.h>
#include <ChipsetInit.h>
#include <CpuRegs.h>
#include <SaAccess.h>
//[-start-130426-12360003-remove]//
//#include <BiosProtectRegion.h>
//[-end-130426-12360003-remove]//
#include <SecureFlash.h>

BOOLEAN  mInRecoveryPei = FALSE;

typedef struct {
  UINT32                 Base;
  UINT32                 Size;
} BIOS_PROTECT_REGION;

typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  Length;
} MEM_MAP;

VOID
BiosProtectPei (
  IN CONST EFI_PEI_SERVICES **PeiServices,
  IN EFI_PEI_CPU_IO_PPI     *CpuIo
  );

/**
 Get FlashInfo base and size

 @param             Index               Index of Variable MTRR pair
 @param             MemoryFlashBase     A pointer to Flash base
 @param             MemoryFlashSize     A pointer to Flash size

 @retval            None

**/
STATIC
VOID
ChipsetGetFlashInfo (
  UINT32                Index,
  UINT64                *MemoryFlashBase,
  UINT64                *MemoryFlashSize
  )
{
  *MemoryFlashBase = AsmReadMsr32 (CACHE_VARIABLE_MTRR_BASE + Index);
  *MemoryFlashBase &= ~0xf;
  *MemoryFlashSize = AsmReadMsr32 (CACHE_VARIABLE_MTRR_BASE + Index + 1);
  *MemoryFlashSize &= ~0x800;
  *MemoryFlashSize |= 0xffffffff00000000;
  *MemoryFlashSize = (~(*MemoryFlashSize)) + 1;
}
VOID
RanOutMtrrSetting (
  IN CONST EFI_PEI_SERVICES  **PeiServices,
  IN PEI_CACHE_PPI           *CachePpi,
  IN UINT64                  TotalMemorySize,
  IN UINT64                  MemoryLength
  )
{
  EFI_STATUS            Status;
  UINT32                MtrrIndex = 0;
  UINT64                MtrrBase = 0;
  UINT64                MtrrSize = 0;
  UINT64                TotalUcLength = 0;
  UINT64                MemoryLengthUc;
  UINT64                MaxMemoryLength;
  UINT64                TempMaxMemoryLength = 0;
  HOB_MTRR_RESTORE_INFO *MtrrRestoreInfoHob;
  UINT64                TempMemoryUcSize = 0;
  UINT64                MemoryUcSize = 0;
  UINT64                ValidMtrrAddressMask;

  //
  // Get maximum physical address size supported by the processor.
  //
  ValidMtrrAddressMask = GetValidMtrrAddress();

  Status = PeiServicesAllocatePool (
             sizeof (HOB_MTRR_RESTORE_INFO),
             (VOID **)&MtrrRestoreInfoHob
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AllocatePool Fail!!\n"));
    ASSERT_EFI_ERROR (Status);
  }

  //
  //  Clear the MTRR Info Hob buffer.
  //
  ZeroMem (MtrrRestoreInfoHob, sizeof (HOB_MTRR_RESTORE_INFO));

  MtrrRestoreInfoHob->MtrrData.TotalMemorySize = TotalMemorySize;

  MtrrSize = GetPowerOfTwo64 (TotalMemorySize);

  if ((TotalMemorySize & ~MtrrSize) != 0){
    MtrrSize = GetNextPowerOfTwo64 (TotalMemorySize);
  }

  MtrrRestoreInfoHob->MtrrData.MtrrBase[MtrrIndex] = MtrrBase | EfiCacheTypeWriteBack;
  MtrrRestoreInfoHob->MtrrData.MtrrSize[MtrrIndex] = ((((~MtrrSize) + 1) & ValidMtrrAddressMask) | B_CACHE_MTRR_VALID);
  MtrrIndex++;

  //
  // Add a 32G cache for High memory when memory >= maximum physical address size.
  //
  if (HighBitSet64 (TotalMemorySize) >= HighBitSet64 (ValidMtrrAddressMask)) {
    MtrrBase = MEM_EQU_32GB;;
    MtrrRestoreInfoHob->MtrrData.MtrrBase[MtrrIndex] = MtrrBase | EfiCacheTypeWriteBack;
    MtrrRestoreInfoHob->MtrrData.MtrrSize[MtrrIndex] = ((((~MtrrSize) + 1) & ValidMtrrAddressMask) | B_CACHE_MTRR_VALID);
    MtrrIndex++;
  }

  //
  // Check if a UC region is present.
  //
  MaxMemoryLength = MemoryLength;
  MaxMemoryLength = GetPowerOfTwo64 (MemoryLength);

  if ((MemoryLength - MaxMemoryLength) != GetPowerOfTwo64 (MemoryLength - MaxMemoryLength)) {
    TempMaxMemoryLength = GetPowerOfTwo64 (LShiftU64 ((MemoryLength - MaxMemoryLength), 1));
  }
  MaxMemoryLength += TempMaxMemoryLength;
  MemoryLengthUc = MaxMemoryLength - MemoryLength;

  MaxMemoryLength = MEM_EQU_4GB;
  TempMaxMemoryLength = MaxMemoryLength;
  TotalUcLength = MaxMemoryLength - MemoryLength;

  //
  // Save below 4G Uc type Mtrr setting.
  //
  while (MaxMemoryLength != MemoryLength) {
    MemoryUcSize = TotalUcLength - TempMemoryUcSize;
    MemoryLengthUc = GetOnePowerOfTwo64 (MaxMemoryLength - MemoryLength);

    MtrrRestoreInfoHob->MtrrData.MtrrBase[MtrrIndex] = (TempMaxMemoryLength - MemoryUcSize) | EfiCacheTypeUncacheable;
    MtrrRestoreInfoHob->MtrrData.MtrrSize[MtrrIndex] = ((((~MemoryLengthUc) + 1)  & ValidMtrrAddressMask) | B_CACHE_MTRR_VALID);
    MtrrIndex ++;

    MaxMemoryLength -= MemoryLengthUc;
    TempMemoryUcSize += MemoryLengthUc;
  }

  BuildGuidDataHob (&gMtrrDataInfoGuid, MtrrRestoreInfoHob, sizeof (HOB_MTRR_RESTORE_INFO));

  return ;
}

/**
 Initializes variable and fixed MTRRs for memory cache.

 @param[in]         PeiServices         Describes the list of possible PEI Services.
 @param[in]         MemoryFlashBase
 @param[in]         MemoryFlashSize

 @retval            EFI_SUCCESS
*/
VOID
MtrrInitialization (
  IN  CONST EFI_PEI_SERVICES **PeiServices,
  IN  UINT64                 MemoryFlashBase,
  IN  UINT64                 MemoryFlashSize
  )
{
  EFI_STATUS                 Status;
  PEI_CACHE_PPI              *CachePpi;
  EFI_PEI_HOB_POINTERS       Hob;
  UINT64                     MemoryLength;
  UINT64                     MemoryLengthUc;
  UINT64                     MaxMemoryLength;
  UINT64                     RemapMemoryLength;
  UINT64                     RemapMemoryBase;
  UINT64                     TempMaxMemoryLength;
  UINT64                     TotalMemorySize = 0;
  UINT64                     RemainingLength = 0;
  EFI_STATUS                 UncacheStatus;
  UINT64                     TempMemoryLength;
  UINT64                     Value64;
  UINTN                      BitCount;

  UncacheStatus    = EFI_SUCCESS;
  TempMemoryLength = 0;
  Value64          = 0;
  BitCount         = 0;

  Status = PeiServicesLocatePpi (
             &gPeiCachePpiGuid,
             0,
             NULL,
             (VOID **)&CachePpi
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Set to 1 MB. Since 1MB cacheability will always be set
  // until override by CSM
  //
  MemoryLength  = 0x100000;
  RemapMemoryLength = 0;
  RemapMemoryBase = MEM_EQU_4GB;
  TempMaxMemoryLength = 0;

//[-start-120404-IB05300309-modify]//
  TotalMemorySize = (McDevFunPciCfg64 (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, R_SA_TOM) & B_SA_TOM_TOM_MASK);
//[-end-120404-IB05300309-modify]//
  //
  // Get memory address information from Hob
  //
  Status = PeiServicesGetHobList ((VOID **)&Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        //
        // Skip the memory region below 1MB
        //
        if ((Hob.ResourceDescriptor->PhysicalStart >= 0x100000) &&
            (Hob.ResourceDescriptor->PhysicalStart < MEM_EQU_4GB)) {
          MemoryLength += Hob.ResourceDescriptor->ResourceLength;
        } else if (Hob.ResourceDescriptor->PhysicalStart >= MEM_EQU_4GB) {
          if (Hob.ResourceDescriptor->PhysicalStart < RemapMemoryBase) {
            RemapMemoryBase = Hob.ResourceDescriptor->PhysicalStart;
          }
          RemapMemoryLength += Hob.ResourceDescriptor->ResourceLength;
        }
      }
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  //
  // Check if a UC region is present
  //
  MaxMemoryLength = MemoryLength;
  DEBUG ((EFI_D_INFO, "Memory Length = %x.\n", MemoryLength));
  DEBUG ((EFI_D_INFO, "Power Of Two Memory Length = %x.\n", MaxMemoryLength));

  MaxMemoryLength = GetPowerOfTwo64 (MemoryLength);
  TempMemoryLength = MemoryLength - MaxMemoryLength;
  for (BitCount = 0, Value64 = TempMemoryLength ; Value64 != 0 ; BitCount = BitCount + 1, Value64 = RShiftU64 (Value64, LowBitSet64 (Value64) + 1));
  if ((BitCount * 2) <= (UINTN)(HighBitSet64 (TempMemoryLength) - LowBitSet64 (TempMemoryLength) + 1 + 1 + 1)) {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Use mothod 1.\n"));

    Status = CachePpi->SetCache ( PeiServices, CachePpi, 0, MemoryLength, EfiCacheTypeWriteBack );
    ASSERT_EFI_ERROR (Status);

    MaxMemoryLength     = MemoryLength;
    TempMaxMemoryLength = 0;
  } else {
    DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Use mothod 2.\n"));

    TempMaxMemoryLength = GetPowerOfTwo64 (LShiftU64 (TempMemoryLength, 1));
    if ((MemoryLength - MaxMemoryLength) != GetPowerOfTwo64 (MemoryLength - MaxMemoryLength)) {
      if (MaxMemoryLength == TempMaxMemoryLength) {
          if (LShiftU64 (MaxMemoryLength, 1) < MEM_EQU_4GB) {
            CachePpi->SetCache (PeiServices, CachePpi, 0, LShiftU64 (MaxMemoryLength, 1), EfiCacheTypeWriteBack);
          }
      } else {
        CachePpi->SetCache(PeiServices, CachePpi, 0, (UINT64)MaxMemoryLength, EfiCacheTypeWriteBack);
        CachePpi->SetCache(PeiServices, CachePpi, (UINT64)MaxMemoryLength, (UINT64)TempMaxMemoryLength, EfiCacheTypeWriteBack);
      }
    } else {
      CachePpi->SetCache(PeiServices, CachePpi, 0, (UINT64)MaxMemoryLength, EfiCacheTypeWriteBack);
    }
  }

  MaxMemoryLength += TempMaxMemoryLength;
  MemoryLengthUc = MaxMemoryLength - MemoryLength;
  if (MaxMemoryLength == MEM_EQU_4GB) {
    CachePpi->SetCache (PeiServices, CachePpi, 0, MaxMemoryLength - MemoryLengthUc, EfiCacheTypeWriteBack);
  } else {
    CachePpi->SetCache (
                PeiServices,
                CachePpi,
                MaxMemoryLength - MemoryLengthUc,
                MemoryLengthUc,
                EfiCacheTypeUncacheable
                );
  }
  CachePpi->SetCache(
              PeiServices,
              CachePpi,
              MemoryFlashBase,
              MemoryFlashSize,
              EfiCacheTypeWriteProtected
              );

  //
  // Set memory was remap to 4G address above Cacheability
  //


  while(RemapMemoryLength > 0) {
    TempMaxMemoryLength = GetPowerOfTwo64 (RemapMemoryLength);
    if (RemapMemoryLength != TempMaxMemoryLength) {
      TempMaxMemoryLength = LShiftU64 (TempMaxMemoryLength, 1);
    }

    if(TempMaxMemoryLength > RemapMemoryBase) {
      //
      // cache length > base, violate the MTRR's rule (length must be <= base))
      //
      TempMaxMemoryLength = RemapMemoryBase;
    }
    Status = CachePpi->SetCache (
               PeiServices,
               CachePpi,
               RemapMemoryBase,
               TempMaxMemoryLength,
               EfiCacheTypeWriteBack
               );
    RemapMemoryBase += TempMaxMemoryLength;
    if(RemapMemoryLength < TempMaxMemoryLength) {
      //
      // cache set is more than required, set the un-required region as UC
      //
      RemainingLength = TempMaxMemoryLength - RemapMemoryLength;
      RemapMemoryBase -= RemainingLength;

      UncacheStatus = CachePpi->SetCache (
                        PeiServices,
                        CachePpi,
                        RemapMemoryBase,
                        RemainingLength,
                        EfiCacheTypeUncacheable
                        );

      RemapMemoryLength -= (TempMaxMemoryLength - RemainingLength);
    } else {
      RemapMemoryLength -= TempMaxMemoryLength;
    }
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Ran out of MTRR - Remapped WB.\n"));
      break;
    }
    if (EFI_ERROR (UncacheStatus)) {
      DEBUG ((EFI_D_INFO | EFI_D_ERROR, "Ran out of MTRR - Remapped UC.\n"));
      break;
    }
  }

  if (EFI_ERROR (Status)) {
    //
    // Re-program MTRR settings during DXE phase if MTRR registers ran out.
    //
    RanOutMtrrSetting (PeiServices, CachePpi, TotalMemorySize, MemoryLength);
  }

  CachePpi->SetCache (
              PeiServices,
              CachePpi,
              0xC0000,
              0x28000,
              EfiCacheTypeWriteProtected
              );

  CachePpi->SetCache (
              PeiServices,
              CachePpi,
              0xE8000,
              0x08000,
              EfiCacheTypeWriteCombining
              );

  CachePpi->SetCache (
              PeiServices,
              CachePpi,
              0xF0000,
              0x10000,
              EfiCacheTypeWriteProtected
              );

  CachePpi->SetCache (
              PeiServices,
              CachePpi,
              0x0,
              0xA0000,
              EfiCacheTypeWriteBack
              );

  CachePpi->ActivateCache(
              PeiServices,
              CachePpi
              );

  return;
}

/**
 Install Firmware Volume Hob's once there is main memory

 @param [in]        BootMode            A pointer to boot mode status

 @retval            EFI_SUCCESS         if the interface could be successfully installed
*/
EFI_STATUS
Stage2MemoryDiscoverCallback (
  IN EFI_BOOT_MODE      *BootMode
  )
{
  EFI_STATUS            Status;
  UINT8                 Index;
  PEI_CACHE_PPI         *CachePpi;
  EFI_PEI_CPU_IO_PPI    *CpuIo;
  EFI_PEI_PCI_CFG2_PPI  *PciCfg;
  UINT16                Pm1Cnt;
  UINT8                 MrcCompleteSignature = 0xFF;
  MEM_MAP               MmioMap[8];
  UINT64                MemoryFlashBase;
  UINT64                MemoryFlashSize;
//[-start-121206-IB10370029-add]//
  UINT64                MtrrSize;
  UINT64                MtrrBase;
//[-end-121206-IB10370029-add]//

//[-start-121201-IB03780468-remove]//
//  PEI_WDTT_PPI          *WdttPpi;
//[-end-121201-IB03780468-remove]//
  VOID                  *EmuPeiPpi;
  CONST EFI_PEI_SERVICES     **PeiServices;

  PeiServices = GetPeiServicesTablePointer ();

  MmioMap [0].BaseAddress = PcdGet64 (PcdPciExpressBaseAddress);

//[-start-130524-IB11410075-modify]//
//[-start-120731-IB10820094-modify]//
  if(PcdGet16(PcdPciExpressSize) == 256) {
    MmioMap[0].Length = 0x10000000;             // Reserved 256M for PCIe
  } else if (PcdGet16(PcdPciExpressSize) == 128) {
    MmioMap[0].Length = 0x08000000;             // Reserved 128M for PCIe
  } else if (PcdGet16(PcdPciExpressSize) == 64) {
    MmioMap[0].Length = 0x04000000;             // Reserved 64M for PCIe
  } else {
    DEBUG (( EFI_D_ERROR | EFI_D_INFO, "PcdPciExpressSize is not correct.\n"));
  }

  MmioMap [1].BaseAddress = PcdGet32 (PcdIoApicBaseAddress);
  MmioMap [1].Length      = 0x00001000;
//[-end-120731-IB10820094-modify]//
//[-end-130524-IB11410075-modify]//

  MmioMap [2].BaseAddress = 0xFEB00000;
  MmioMap [2].Length      = 0x00004000;
  MmioMap [3].BaseAddress = MCH_BASE_ADDRESS;
  MmioMap [3].Length      = 0x8000;
  MmioMap [4].BaseAddress = PcdGet32 (PcdDmiBaseAddress);
  MmioMap [4].Length      = 0x1000;
  MmioMap [5].BaseAddress = PcdGet32 (PcdEpBaseAddress);
  MmioMap [5].Length      = 0x1000;
  MmioMap [6].BaseAddress = PcdGet32 (PcdRcbaBaseAddress);
  MmioMap [6].Length      = 0x4000;
  MmioMap [7].BaseAddress = PcdGet32 (PcdCpuLocalApicBaseAddress);
  MmioMap [7].Length      = 0x1000;

  CpuIo  = (**PeiServices).CpuIo;
  PciCfg = (**PeiServices).PciCfg;

//[-start-120404-IB05300309-modify]//
  PciCfg->Write(
            PeiServices,
            PciCfg,
            EfiPeiPciCfgWidthUint8,
            EFI_PEI_PCI_CFG_ADDRESS (SA_MC_BUS, SA_MC_DEV, SA_MC_FUN, (R_SA_MC_SKPD_OFFSET + 3)),
            &MrcCompleteSignature
            );
//[-end-120404-IB05300309-modify]//
  //
  // Check if we are in recovery only PEI phase
  //
//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdUseFastCrisisRecovery)) {
    Status = PeiServicesLocatePpi (
               &gEmuPeiPpiGuid,
               0,
               NULL,
               (VOID **)&EmuPeiPpi
               );
    mInRecoveryPei = EFI_ERROR (Status) ? FALSE : TRUE;
  }
  if (!mInRecoveryPei) {
//[-end-120731-IB10820094-modify]//
    //
    //move before s3 resume for mps3 resume
    //
    Status = PeiServicesLocatePpi (
               &gPeiCachePpiGuid,
               0,
               NULL,
               (VOID **)&CachePpi
               );
    ASSERT_EFI_ERROR (Status);
    //
    //  read Flash base and size
    //
//[-start-121206-IB10370029-modify]//
    //ChipsetGetFlashInfo(2,&MemoryFlashBase,&MemoryFlashSize);
    MemoryFlashBase = FixedPcdGet32 (PcdFlashAreaBaseAddress);
    MemoryFlashSize = FixedPcdGet32 (PcdFlashAreaSize);

    if (GetPowerOfTwo64 (MemoryFlashSize) != MemoryFlashSize) {
      MtrrSize = GetNextPowerOfTwo64 (MemoryFlashSize);
      MtrrBase = MEM_EQU_4GB - MtrrSize;
    } else {
      MtrrBase = MemoryFlashBase;
      MtrrSize = MemoryFlashSize;
    }
//[-end-121206-IB10370029-modify]//
    //
    // Clear the CAR Settings
    //
    CachePpi->ResetCache (
                PeiServices,
                CachePpi
                );
    //
    // Move MTRR initialization from the end of PEI phase to here to reduce shadow time of dxeipl.
    // Initial variable MTRRs immediately after memory is available to improve peformance of memory services.
    //
//[-start-121206-IB10370029-modify]//
  MtrrInitialization (PeiServices, MtrrBase, MtrrSize);
//[-end-121206-IB10370029-modify]//

//[-start-120330-IB06460381-add]//
//[-start-120611-IB10820067-modify]//
//#ifdef GRANITE_CITY_SUPPORT
    if (FeaturePcdGet(PcdGraniteCitySupported)){
      if (*BootMode != BOOT_IN_RECOVERY_MODE) {
        //
        // This is normal boot, we need to lock start block and recovery block before enter boot block.
        //
        BiosProtectPei (PeiServices, CpuIo);
      }
    }
//#endif
//[-end-120611-IB10820067-modify]//
//[-end-120330-IB06460381-add]//


    if (*BootMode == BOOT_ON_S3_RESUME) {
      //
      // Clear SLP_TYP in PM1_CNT after MemorySizing in S3 Resume
      //
      CachePpi->SetCache(
                  (CONST EFI_PEI_SERVICES **)PeiServices,
                  CachePpi,
                  MemoryFlashBase,
                  MemoryFlashSize,
                  EfiCacheTypeWriteProtected
                  );
      CachePpi->ActivateCache(
                 (CONST EFI_PEI_SERVICES **)PeiServices,
                 CachePpi
                 );
      Pm1Cnt  = CpuIo->IoRead16 (PeiServices, CpuIo, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT);
      Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
      CpuIo->IoWrite16 (PeiServices, CpuIo, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT, Pm1Cnt);
      return EFI_SUCCESS;
    }

  }

  for (Index = 0; Index<sizeof (MmioMap)/ (sizeof (MEM_MAP)); Index++){
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length
      );

    BuildMemoryAllocationHob (
      MmioMap[Index].BaseAddress,
      MmioMap[Index].Length,
      EfiMemoryMappedIO
      );
  }

  //
  // Create a CPU hand-off information
  //
  BuildCpuHob (36, 16);

//[-start-121201-IB03780468-remove]//
//  if (FeaturePcdGet(PcdXtuSupported)) {
//    Status = PeiServicesLocatePpi (&gPeiWdttPpiGuid, 0, NULL, &WdttPpi);
//    //
//    // when system run to here, the XMP setting should be ok, so stop and clear watchdog status
//    //
//    WdttPpi->StopWDT (PeiServices, WdttPpi, XtuWatchdogFlagXmp);
//  }
//[-end-121201-IB03780468-remove]//
  return EFI_SUCCESS;
}
//[-start-120330-IB06460381-add]//
//[-start-120611-IB10820067-modify]//
//#ifdef GRANITE_CITY_SUPPORT
/**
 Lock start block and recovery block before enter boot block.

 @param [in]        PeiServices         General purpose services available to every PEIM.
 @param [in]        CpuIo               A pointer to CPU IO PPI

 @retval            VOID

*/
VOID
BiosProtectPei (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN EFI_PEI_CPU_IO_PPI       *CpuIo
  )
{
//[-start-130503-12360003-modify]//
  BIOS_PROTECT_REGION                BiosRegionTable[] = {
                                       {FixedPcdGet32 (PcdFlashFvBackupBase), FixedPcdGet32 (PcdFlashFvBackupSize)},
                                       {FixedPcdGet32 (PcdFlashFvRecoveryBase), FixedPcdGet32 (PcdFlashFvRecoverySize)}
                                     };
//[-end-130503-12360003-modify]//
  UINT32                             RegionIndex;
  UINT32                             ProtectData;
  UINT32                             Address;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *Variable;
  EFI_STATUS                         Status;
  UINTN                              Size;
  IMAGE_INFO                         ImageInfo;
//[-start-130503-12360003-add]//
  UINT8                              ProtectRegionNum;
//[-end-130503-12360003-add]//

  RegionIndex = 0;
  ProtectRegionNum = sizeof (BiosRegionTable) / sizeof (BIOS_PROTECT_REGION);

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  Size = sizeof (ImageInfo);
  Status = Variable->GetVariable (
                       Variable,
                       L"SecureFlashInfo",
                       &gSecureFlashInfoGuid,
                       NULL,
                       &Size,
                       &ImageInfo
                       );

  if (!(EFI_ERROR (Status)) && (ImageInfo.FlashMode)) {
    //
    // Update flash mode: Don't protect any region.
    //
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Update flash mode: System doesn't protect any region.\n"));
    return;
  }
//[-start-130503-12360003-modify]//
  if (BiosRegionTable != NULL && ProtectRegionNum != 0) {  
    for (RegionIndex = 0; RegionIndex < ProtectRegionNum; ++RegionIndex) {
      ProtectData = 0;
      //
      // Enable the write protect BIT. (PRx[31])
      //
      ProtectData |= BIT31;
      //
      // Set the Protected Range Base. (PRx[12:0])
      //
      ProtectData |= (BiosRegionTable[RegionIndex].Base & 0x00FFF000) >> 12;
      //
      // Set the Protected Range Limit. (PRx [28:16])
      //
      ProtectData |= ((BiosRegionTable[RegionIndex].Base + (BiosRegionTable[RegionIndex].Size - 1)) & 0x00FFF000) << 4;
//[-start-121102-IB05280008-modify]//
      Address = PcdGet32 (PcdRcbaBaseAddress) + R_PCH_SPI_PR0 + RegionIndex * 4;
//[-end-121102-IB05280008-modify]//
      CpuIo->MemWrite32 (PeiServices, CpuIo, Address, ProtectData);
    }
  }
//[-end-130503-12360003-modify]//
  DEBUG ((EFI_D_ERROR, "Exit BiosProtectPei\n"));

}
//#endif
//[-end-120611-IB10820067-modify]//
//[-end-120330-IB06460381-add]//
