/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcModifyMemoryRange().
 The function PeiCsSvcModifyMemoryRange() use chipset services to modify
 memory range setting.

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/PlatformMemoryRange.h>
#include <SaAccess.h>

//[-start-121220-IB04770255-add]//
//
// BUGBUG : This should be defined in SaRegs.h
//
#define V_SA_GGC_GMS_1024MB        0x11
//[-end-121220-IB04770255-add]//

/**
 To modify memory range setting.


 @param[in, out]    OptionRomMask       A pointer to PEI_MEMORY_RANGE_OPTION_ROM
 @param[in, out]    SmramMask           A pointer to PEI_MEMORY_RANGE_SMRAM
 @param[in, out]    GraphicsMemoryMask  A pointer to PEI_MEMORY_RANGE_GRAPHICS_MEMORY
 @param[in, out]    PciMemoryMask       A pointer to PEI_MEMORY_RANGE_PCI_MEMORY

 @return            EFI_SUCCESS         Always return successfully
*/
EFI_STATUS
ModifyMemoryRange (
  IN OUT  PEI_MEMORY_RANGE_OPTION_ROM           *OptionRomMask,
  IN OUT  PEI_MEMORY_RANGE_SMRAM                *SmramMask,
  IN OUT  PEI_MEMORY_RANGE_GRAPHICS_MEMORY      *GraphicsMemoryMask,
  IN OUT  PEI_MEMORY_RANGE_PCI_MEMORY           *PciMemoryMask
  )
{
  UINT16                    GraphicsControlRegister = 0;
  UINT16                    DeviceEnableRegister    = 0;
  CONST EFI_PEI_SERVICES  **PeiServices;
//[-start-121220-IB04770255-add]//
  UINT32  GraphicsMemorySize = 0;
//[-end-121220-IB04770255-add]//

  PeiServices = GetPeiServicesTablePointer ();

  //
  // Choose one or none of the following:
  //   PEI_MR_SMRAM_ABSEG_128K_NOCACHE
  //   PEI_MR_SMRAM_HSEG_128K_CACHE
  //   PEI_MR_SMRAM_HSEG_128K_NOCACHE
  //
  *SmramMask |= PEI_MR_SMRAM_ABSEG_128K_NOCACHE;
  
  //
  // Choose one or none of the following:
  //   PEI_MR_SMRAM_TSEG_512K_CACHE
  //   PEI_MR_SMRAM_TSEG_512K_NOCACHE
  //   PEI_MR_SMRAM_TSEG_1024K_CACHE
  //   PEI_MR_SMRAM_TSEG_1024K_NOCACHE
  //
  *SmramMask |= PEI_MR_SMRAM_TSEG_8192K_CACHE;
  
  //
  // Choose regions to reserve for Graphics Memory use
  // Choose one or none of the following:
  //   PEI_MR_GRAPHICS_MEMORY_1M_NOCACHE    0x00000001
  //   PEI_MR_GRAPHICS_MEMORY_4M_NOCACHE    0x00000004
  //   PEI_MR_GRAPHICS_MEMORY_8M_NOCACHE    0x00000008
  //   PEI_MR_GRAPHICS_MEMORY_16M_NOCACHE   0x00000010
  //   PEI_MR_GRAPHICS_MEMORY_32M_NOCACHE   0x00000020
  //

//[-start-120404-IB05300309-modify]//
  GraphicsControlRegister = McD0PciCfg16 (R_SA_GGC);
  DeviceEnableRegister = McD0PciCfg16 (R_SA_DEVEN);

  if ((DeviceEnableRegister & B_SA_DEVEN_D2EN_MASK) == B_SA_DEVEN_D2EN_MASK) {
    if ((GraphicsControlRegister & B_SA_GGC_IVD_MASK) != 0) {
      McD0PciCfg16AndThenOr (R_SA_GGC, ~(B_SA_GGC_GMS_MASK), V_SA_GGC_GMS_64MB << N_SA_GGC_GMS_OFFSET);
    }
    switch ((GraphicsControlRegister & B_SA_GGC_GMS_MASK) >> 3) {

//[-start-121220-IB04770255-modify]//
    case V_SA_GGC_GMS_1024MB:
      GraphicsMemorySize = 1024;
      break;

    case V_SA_GGC_GMS_512MB:
      GraphicsMemorySize = 512;
      break;

    case V_SA_GGC_GMS_480MB:
      GraphicsMemorySize = 480;
      break;

    case V_SA_GGC_GMS_448MB:
      GraphicsMemorySize = 448;
      break;

    case V_SA_GGC_GMS_416MB:
      GraphicsMemorySize = 416;
      break;

    case V_SA_GGC_GMS_384MB:
      GraphicsMemorySize = 384;
      break;

    case V_SA_GGC_GMS_352MB:
      GraphicsMemorySize = 352;
      break;

    case V_SA_GGC_GMS_320MB:
      GraphicsMemorySize = 320;
      break;

    case V_SA_GGC_GMS_288MB:
      GraphicsMemorySize = 288;
      break;

    case V_SA_GGC_GMS_256MB:
      GraphicsMemorySize = 256;
      break;

    case V_SA_GGC_GMS_224MB:
      GraphicsMemorySize = 224;
      break;

    case V_SA_GGC_GMS_192MB:
      GraphicsMemorySize = 192;
      break;

    case V_SA_GGC_GMS_160MB:
      GraphicsMemorySize = 160;
      break;

    case V_SA_GGC_GMS_128MB:
      GraphicsMemorySize = 128;
      break;

    case V_SA_GGC_GMS_96MB:
      GraphicsMemorySize = 96;
      break;

    case V_SA_GGC_GMS_64MB:
      GraphicsMemorySize = 64;
      break;

    case V_SA_GGC_GMS_32MB:
      GraphicsMemorySize = 32;
      break;

    case V_SA_GGC_GMS_DIS:
    default:
      GraphicsMemorySize = 0;
    }

    switch ((GraphicsControlRegister & B_SA_GGC_GGMS_MASK) >> 8) {

    case V_SA_GGC_GGMS_DIS:
      break;

    case V_SA_GGC_GGMS_2MB:
      GraphicsMemorySize = GraphicsMemorySize + 2;
      break;

    case V_SA_GGC_GGMS_1MB:
    default:
      GraphicsMemorySize = GraphicsMemorySize + 1;
      break;
    }

  } else {
    GraphicsMemorySize = 0;
  }
//[-end-120404-IB05300309-modify]//
//[-end-121220-IB04770255-modify]//
//[-start-121220-IB04770255-add]//
  //
  // The granularity is 512 KB.
  //
  *GraphicsMemoryMask = GraphicsMemorySize * 1024 / 512;
//[-end-121220-IB04770255-add]//
  return EFI_SUCCESS;
}