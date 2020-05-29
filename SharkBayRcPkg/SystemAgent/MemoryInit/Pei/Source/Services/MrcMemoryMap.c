/** @file
  The functions in this file initializes the physical memory map.

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
  license agreement.

*/

//
// Include files
//
#include <MrcMemoryMap.h>
#include <PttHciRegs.h>

/**
@brief
  After BIOS determines the total physical memory size.
  Determines TOM which is defined by the total physical memory size.
  Determines TOM minus the ME memory size. The ME memory size is calculated from MESEG_BASE and MESEG_MASK.
  Determines MMIO allocation, which is system configuration dependent.

  Determines TOLUD which is the minimum value by comparing between "4GB minus MMIO size" and "TOM minus ME size".
  Determines Graphics Stolen Base, BDSM by subtracting the graphics data stolen memory size from TOLUD.
  Graphics Data Stolen Memory size is given by GMS field in GGC register.  It must be define before this stage.
  Determines Graphics GTT Stolen Base, BGSM by subtracting the GTT graphics stolen memory size from BDSM.
  GTT Stolen Memory size is given by GGMS field in GGC register.  It must be define before this stage.
  Determines TSEG Base, TSEGMB by subtracting TSEG size from BGSM.
  TSEG should be defined.
  Remove the memory hole caused by aligning TSEG to a 8MB boundary.
  Determine whether Memory Reclaim is available.  If "TOM minus ME Stolem Memory Size" is greater than the value of TOLUD, then memory reclaim is available to enable.
  Determine REMAPBASE if reclaim is enabled.  This is the maximum value by comparing between 4GB and "TOM minus ME size".
  Determine REMAPLIMIT () if reclaim is enabled.  This is the value of REMAPBASE plus "the difference between the value in TOLUD register and the lower of either 4GB or 'TOM minus ME Stolen memory size", and then minus 1 boundary.
  Determine TOUUD. TOUUD indicates the address one byte above the maximum DRAM.  If relcaim is disabled, this value is calculated by "TOM minus ME stolen size".  Otherwise, this value is set to REMAPLIMIT plus 1MB.

  @param[in, out] MrcData - Include all MRC global data. include also the memory map data.

  @retval MrcStatus -  if the reset is succeded.
**/
MrcStatus
MrcSetMemoryMap (
  IN OUT MrcParameters *const MrcData
  )
{
  const MrcDebug          *Debug;
  const MrcInput          *Inputs;
  MrcOutput               *Outputs;
  MrcMemoryMap            *MemoryMap;
  MRC_PCI_000_GGC_STRUCT  Ggc;
  U32                     Offset;
  U32                     TsegBaseOrg;
  U32                     TsegBaseDelta;
  U32                     GdxcTop;
  U32                     FtpmTop;
  U32                     MmioSize;
#ifdef PTT_FLAG
  U32                     PttSts;
#endif

  Inputs    = &MrcData->SysIn.Inputs;
  Debug     = &Inputs->Debug;
  Outputs   = &MrcData->SysOut.Outputs;
  MemoryMap = &Outputs->MemoryMapData;

  //
  // Find the total memory size
  //
  MrcTotalMemory (MrcData);
  
  //
  // Set TOM register
  //
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TOM (Total physical memory size) = %u MB\n", MemoryMap->TotalPhysicalMemorySize);

  //
  // Find the TOM minus ME size only for internal calculations
  //
  MemoryMap->TomMinusMe = MemoryMap->TotalPhysicalMemorySize - Inputs->MeStolenSize;

  MmioSize = Inputs->MmioSize;
  if (Inputs->MemoryTrace) {
    if (MemoryMap->TotalPhysicalMemorySize <= MEM_4GB) {
      MmioSize = MAX (MmioSize, MEM_4GB - MemoryMap->TotalPhysicalMemorySize / 2);
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Adjusted MmioSize = %Xh\n", MmioSize);
    }
  }
  
  //
  // Find and set TOLUD.
  // TOLUD which is the minimum value by comparing between "4GB minus MMIO size" and "TOM minus ME size"
  //
  MemoryMap->ToludBase = MIN (MemoryMap->TomMinusMe, MEM_4GB - MmioSize);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TOLUD base = %Xh\n", MemoryMap->ToludBase);

  //
  // Find and set BDSM Graphics Stolen Base.
  // Graphics Stolen Base, BDSM by subtracting the graphics data stolen memory size from TOLUD.
  //
  MemoryMap->BdsmBase = MemoryMap->ToludBase - Outputs->GraphicsStolenSize;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BDSM base = %Xh\n", MemoryMap->BdsmBase);

  //
  // Graphics GTT Stolen Base
  // Graphics GTT Stolen Base, BGSM by subtracting the GTT graphics stolen memory size from BDSM.
  //
  MemoryMap->GttBase = MemoryMap->BdsmBase - Outputs->GraphicsGttSize;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GTT base = %Xh\n", MemoryMap->GttBase);

  //
  // Graphics size register init.
  //
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_GGC_REG);
  MrcOemMmioRead (Offset, &Ggc.Data, Inputs->PciEBaseAddress);
  Ggc.Bits.Vamen  = (Inputs->GfxIsVersatileAcceleration == TRUE) ? 1 : 0;
  Ggc.Bits.Ggms   = MIN (GGC_GGMS_MAX, Outputs->GraphicsGttSize);
  //
  // GMS limitation is 5 bits
  //
  if (Outputs->GraphicsStolenSize == 1024) {
    Ggc.Bits.Gms = 17;
  } else {
    Ggc.Bits.Gms = MIN (GGC_GMS_MAX, (Outputs->GraphicsStolenSize / 32));
  }

  MemoryMap->GraphicsControlRegister = Ggc.Data;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GGC value = %Xh\n", MemoryMap->GraphicsControlRegister);

  //
  // TSEG Base
  // TSEGMB by subtracting TSEG size from BGSM.
  //
  MemoryMap->TsegBase = MemoryMap->GttBase - Inputs->TsegSize;
  TsegBaseOrg         = MemoryMap->TsegBase;

  //
  // Dpr size to program DPR register in update MemoryMap
  //
  MemoryMap->DprSize = Inputs->DprSize;

  //
  // SMRR must be aligned at 8MB boundary.
  // according to this TSEG base need to be also aligned to 8MB boundary.
  // Round it down to the nearest 8MB boundary.
  //
  MemoryMap->TsegBase &= ~(Inputs->TsegSize - 1);
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TSEG base = %Xh\n", MemoryMap->TsegBase);

  //
  // Remove the hole between top of aligned TSEG and GTT Base:
  //  1. Calculate Delta = TsegMB - aligned_TsegMB
  //  2. Walk backwards and adjust BGSM_new = BGSM - DELTA, TOLUD_new = TOLUD - DELTA
  //
  TsegBaseDelta = TsegBaseOrg - MemoryMap->TsegBase;
  if (TsegBaseDelta != 0) {
    MemoryMap->GttBase   = MemoryMap->GttBase - TsegBaseDelta;
    MemoryMap->BdsmBase  = MemoryMap->BdsmBase - TsegBaseDelta;
    MemoryMap->ToludBase = MemoryMap->ToludBase - TsegBaseDelta;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GTT base = %Xh\n", MemoryMap->GttBase);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "BDSM base = %Xh\n", MemoryMap->BdsmBase);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TOLUD base = %Xh\n", MemoryMap->ToludBase);
  }
  //
  // test if Reclaim is available
  // If "TOM minus ME Stolem Memory Size" is greater than the value of TOLUD, then memory reclaim is available to enable
  //
  if (Inputs->RemapEnable && (MemoryMap->TomMinusMe > MemoryMap->ToludBase)) {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Reclaim Enable\n");
    MemoryMap->ReclaimEnable = TRUE;
    //
    // Remap Base
    // This is the maximum value by comparing between 4GB and "TOM minus ME size".
    //
    MemoryMap->RemapBase = MAX (MEM_4GB, MemoryMap->TomMinusMe);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Remap Base %Xh\n", MemoryMap->RemapBase);
    //
    // Remap Limit
    // This is the value of REMAPBASE plus "the difference between the value in TOLUD register and the lower of either 4GB or 'TOM minus ME Stolen memory size", and then minus 1MB boundary.
    //
    MemoryMap->RemapLimit = MemoryMap->RemapBase + (MIN (MEM_4GB, MemoryMap->TomMinusMe) - MemoryMap->ToludBase);

    MemoryMap->TouudBase = MemoryMap->RemapLimit;

    if (!((Inputs->CpuModel == cmHSW) && (Inputs->CpuStepping == csHswA0))) {
      MemoryMap->RemapLimit -= 0x1;
    }

    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Remap Limit %Xh\n", MemoryMap->RemapLimit);
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TOUUD Base %Xh\n", MemoryMap->TouudBase);
  } else {
    MemoryMap->ReclaimEnable = FALSE;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Reclaim disable \n");
    //
    // TOUUD Base
    // If relcaim is disabled, this value is calculated by "TOM minus ME stolen size".
    //
    MemoryMap->TouudBase = MemoryMap->TomMinusMe;
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "TOUUD Base %Xh\n", MemoryMap->TouudBase);
  }
  //
  // GDXC must be aligned to 8MB boundary. But PSMI must be 16MB alligned
  // GdxcBase by subtracting Gdxc from BGSM.
  // @todo: GDXC is below DPRBASE if TXT is enabled, which is below TSEG. Maybe it will be required to be placed anywhere below TOLUD.
  //
  if (Outputs->Gdxc.GdxcEnable) {
    if (Inputs->MemoryTrace) {
      //
      // Put GDXC at the top of the second channel
      //
      if (MemoryMap->TotalPhysicalMemorySize <= MEM_4GB) {
        GdxcTop = MemoryMap->TouudBase;
      } else {
        GdxcTop = MemoryMap->TomMinusMe;
      }
    } else {
      //
      // Put GDXC below DPR stolen region.
      //
      GdxcTop = MemoryMap->TsegBase - Inputs->DprSize;
    }
    //
    // @todo For C-step we can remove the "minus 1MB" W/A
    //
    MemoryMap->GdxcMotSize = Outputs->Gdxc.GdxcMotSize << (23 - 20); // In MB
    MemoryMap->GdxcMotBase = GdxcTop - MemoryMap->GdxcMotSize - 1; // Minus 1 MB - WA for MOT overflow.
    MemoryMap->GdxcMotBase &= ~(MRC_BIT4 - 1);  // Round down to 16MB boundary
    
    MemoryMap->GdxcIotSize = Outputs->Gdxc.GdxcIotSize << (23 - 20); // In MB
    MemoryMap->GdxcIotBase = MemoryMap->GdxcMotBase - MemoryMap->GdxcIotSize;
    MemoryMap->GdxcIotBase -= 16; // 16MB for PSMI
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "GDXC MOT base %Xh, size %d (%Xh) MB\n",
      MemoryMap->GdxcMotBase,
      MemoryMap->GdxcMotSize,
      MemoryMap->GdxcMotSize
      );
    MRC_DEBUG_MSG (
      Debug,
      MSG_LEVEL_NOTE,
      "GDXC IOT base %Xh, size %d (%Xh) MB\n",
      MemoryMap->GdxcIotBase,
      MemoryMap->GdxcIotSize,
      MemoryMap->GdxcIotSize
      );
    if (Inputs->MemoryTrace) {
      //
      // Put fTPM below DPR
      //
      FtpmTop = MemoryMap->TsegBase - Inputs->DprSize;
    } else {
      //
      // Put fTPM below GDXC.
      //
      FtpmTop = MemoryMap->GdxcIotBase;
    }
  } else {
    MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "GDXC DISABLED\n");
    FtpmTop = MemoryMap->TsegBase - Inputs->DprSize;
  }
  
#ifdef PTT_FLAG
  if (Inputs->CpuModel == cmHSW_ULT) {
    MrcOemMmioRead (R_PTT_HCI_STS, (U32 *) &PttSts, R_PTT_HCI_BASE_ADDRESS);
    if ((PttSts & B_PTT_HCI_STS_ENABLED) == B_PTT_HCI_STS_ENABLED) {
      //
      // fTPM Stolen size is 4KB
      //
      MemoryMap->FtpmStolenBase = (FtpmTop << 20) - 0x1000;
      MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "Ftpm Stolen base = %Xh\n", MemoryMap->FtpmStolenBase);
    }
  }
#endif // PTT_FLAG

  MemoryMap->MeStolenBase = MemoryMap->TomMinusMe;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ME stolen base %Xh\n", MemoryMap->MeStolenBase);

  MemoryMap->MeStolenSize = Inputs->MeStolenSize;
  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "ME stolen size %Xh\n", MemoryMap->MeStolenSize);

  UpdateMemoryMapRegisters (Inputs->PciEBaseAddress, Inputs->GdxcBaseAddress, MemoryMap);
  return mrcSuccess;
}

/**
@brief
  This function find the total memory in the system.
  and write it to TotalPhysicalMemorySize in MrcData structure.

  @param[in, out] MrcData - Include all MRC global data.

  @retval Nothing
**/
void
MrcTotalMemory (
  IN OUT MrcParameters *const MrcData
  )
{
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcDimmOut        *DimmOut;
  U8                Controller;
  U8                Channel;
  U8                Dimm;

  Outputs = &MrcData->SysOut.Outputs;
  Outputs->MemoryMapData.TotalPhysicalMemorySize = 0;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerOut = &Outputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelOut           = &ControllerOut->Channel[Channel];
      ChannelOut->Capacity = 0;
      if (MrcChannelExist (Outputs, Channel)) {
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          DimmOut = &ChannelOut->Dimm[Dimm];
          if (DimmOut->Status == DIMM_PRESENT) {
            ChannelOut->Capacity += DimmOut->DimmCapacity;
          }
        }

        ChannelOut->Capacity = MIN (ChannelOut->Capacity, Outputs->MrcTotalChannelLimit);
        Outputs->MemoryMapData.TotalPhysicalMemorySize += ChannelOut->Capacity;
      }
    }
  }

  return;
}

/**
@brief
  this function write to the memory init registers.

  @param[in] PciEBaseAddress - Address of the PCI Express BAR
  @param[in] GdxcBaseAddress - Address of the GDXC BAR
  @param[in] MemoryMap       - Include all the memory map definitions

  @retval Nothing
**/
void
UpdateMemoryMapRegisters (
  IN const U32                 PciEBaseAddress,
  IN const U32                 GdxcBaseAddress,
  IN const MrcMemoryMap *const MemoryMap
  )
{
  MRC_PCI_000_TOM_STRUCT        Tom;
  MRC_PCI_000_TOLUD_STRUCT      Tolud;
  MRC_PCI_000_TOUUD_STRUCT      Touud;
  MRC_PCI_000_REMAPBASE_STRUCT  RemapBase;
  MRC_PCI_000_REMAPLIMIT_STRUCT RemapLimit;
  MRC_PCI_000_TSEGMB_STRUCT     Tsegmb;
  MRC_PCI_000_BDSM_STRUCT       Bdsm;
  MRC_PCI_000_BGSM_STRUCT       Bgsm;
  MRC_PCI_000_MESEG_BASE_STRUCT MeSegBase;
  MRC_PCI_000_MESEG_MASK_STRUCT MeSegMask;
  MRC_PCI_000_DPR_STRUCT        Dpr;
  U32                           Offset;

  //
  // Write TOM register
  //
  Tom.Data                   = 0;
  Tom.Data32.Low.Bits.Value  = MemoryMap->TotalPhysicalMemorySize;
  Tom.Data32.High.Bits.Value = MemoryMap->TotalPhysicalMemorySize >> (32 - TOM_TOM_OFF);
  Offset                     = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOM_REG);
  MrcOemMmioWrite (Offset, Tom.Data32.Low.Data, PciEBaseAddress);
  MrcOemMmioWrite (Offset + 4, Tom.Data32.High.Data, PciEBaseAddress);

  //
  // Write TOLUD register
  //
  Tolud.Data       = 0;
  Tolud.Bits.Value = MemoryMap->ToludBase;
  Offset           = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOLUD_REG);
  MrcOemMmioWrite (Offset, Tolud.Data, PciEBaseAddress);

  //
  // Write TOUUD register
  //
  Touud.Data                   = 0;
  Touud.Data32.Low.Bits.Value  = MemoryMap->TouudBase;
  Touud.Data32.High.Bits.Value = MemoryMap->TouudBase >> (32 - TOUUD_TOUUD_OFF);
  Offset                       = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOUUD_REG);
  MrcOemMmioWrite (Offset, Touud.Data32.Low.Data, PciEBaseAddress);
  MrcOemMmioWrite (Offset + 4, Touud.Data32.High.Data, PciEBaseAddress);

  if (MemoryMap->ReclaimEnable) {
    //
    // Write REMAPBASE register.
    //
    RemapBase.Data                   = 0;
    RemapBase.Data32.Low.Bits.Value  = MemoryMap->RemapBase;
    RemapBase.Data32.High.Bits.Value = MemoryMap->RemapBase >> (32 - REMAPBASE_REMAPBASE_OFF);
    Offset                           = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPBASE_REG);
    MrcOemMmioWrite (Offset, RemapBase.Data32.Low.Data, PciEBaseAddress);
    MrcOemMmioWrite (Offset + 4, RemapBase.Data32.High.Data, PciEBaseAddress);

    //
    // Write REMAPLIMIT register.
    //
    RemapLimit.Data                   = 0;
    RemapLimit.Data32.Low.Bits.Value  = MemoryMap->RemapLimit;
    RemapLimit.Data32.High.Bits.Value = MemoryMap->RemapLimit >> (32 - REMAPLIMIT_REMAPLMT_OFF);
    Offset                            = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPLIMIT_REG);
    MrcOemMmioWrite (Offset, RemapLimit.Data32.Low.Data, PciEBaseAddress);
    MrcOemMmioWrite (Offset + 4, RemapLimit.Data32.High.Data, PciEBaseAddress);
  }
  //
  // Write TSEGMB register
  //
  Offset            = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TSEGMB_REG);
  Tsegmb.Data       = 0;
  Tsegmb.Bits.Value = MemoryMap->TsegBase;
  MrcOemMmioWrite (Offset, Tsegmb.Data, PciEBaseAddress);

  //
  // Program DPR Register with DPR size & DMA Protection Enabled
  //
  if(MemoryMap->DprSize != 0){
    Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_DPR_REG);
    MrcOemMmioRead (Offset, &Dpr.Data, PciEBaseAddress);
    Dpr.Bits.Dprsize = MemoryMap->DprSize;
    Dpr.Bits.Epm     = 1;
    MrcOemMmioWrite (Offset, Dpr.Data, PciEBaseAddress);
  }
  //
  // Write BDSM register
  //
  Offset          = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_BDSM_REG);
  Bdsm.Data       = 0;
  Bdsm.Bits.Value = MemoryMap->BdsmBase;
  MrcOemMmioWrite (Offset, Bdsm.Data, PciEBaseAddress);

  //
  // Write BGSM register
  //
  Offset          = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_BGSM_REG);
  Bgsm.Data       = 0;
  Bgsm.Bits.Value = MemoryMap->GttBase;
  MrcOemMmioWrite (Offset, Bgsm.Data, PciEBaseAddress);

  //
  // Enable ME Stolen Memory if the size is not zero
  //
  if (MemoryMap->MeStolenSize != 0) {
    //
    // Write MESEG_MASK register. Must be written before MESEG_BASE.
    //
    MeSegMask.Data                   = 0;
    MeSegMask.Data32.Low.Bits.Enable = 1;
    Offset                           = 0x80000 - MemoryMap->MeStolenSize;
    MeSegMask.Data32.Low.Bits.Value  = Offset;
    MeSegMask.Data32.High.Bits.Value = Offset >> (32 - MESEG_MASK_MEMASK_OFF);
    Offset                           = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_MESEG_MASK_REG);
    MrcOemMmioWrite (Offset, MeSegMask.Data32.Low.Data, PciEBaseAddress);
    MrcOemMmioWrite (Offset + 4, MeSegMask.Data32.High.Data, PciEBaseAddress);

    //
    // Write MESEG_BASE register
    //
    MeSegBase.Data                   = 0;
    MeSegBase.Data32.Low.Bits.Value  = MemoryMap->MeStolenBase;
    MeSegBase.Data32.High.Bits.Value = MemoryMap->MeStolenBase >> (32 - MESEG_BASE_MEBASE_OFF);
    Offset                           = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_MESEG_BASE_REG);
    MrcOemMmioWrite (Offset, MeSegBase.Data32.Low.Data, PciEBaseAddress);
    MrcOemMmioWrite (Offset + 4, MeSegBase.Data32.High.Data, PciEBaseAddress);
  }
  //
  // Write graphics control register
  //
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_GGC_REG);
  MrcOemMmioWrite (Offset, MemoryMap->GraphicsControlRegister, PciEBaseAddress);

  //
  // Program GDXC Registers
  // 1st MOT: 0x10, 0x14 and 0x18 (Address Low, Address High and Region)
  // 2nd IOT: 0x20, 0x24 and 0x28 (Address Low, Address High and Region)
  //
  MrcOemMmioWrite (
    MPCOHTRK_CR_GDXC_MOT_ADDRESS_LO_REG,
    MemoryMap->GdxcMotBase << 14,   // (GdxcMotBase << 20) >> 6, Current Pointer in cache line units
    GdxcBaseAddress
    );
  MrcOemMmioWrite (
    MPCOHTRK_CR_GDXC_MOT_ADDRESS_HI_REG,
    (MemoryMap->GdxcMotBase & MRC_BIT18) >> 18, // Bit [18] will be bit [32], so it goes to MOT_ADDRESS_HI.MEM_PTR
    GdxcBaseAddress
    );
  MrcOemMmioWrite (
    MPCOHTRK_CR_GDXC_MOT_REGION_REG,
    MemoryMap->GdxcMotBase >> 3 |  // (GdxcMotBase << 20) >> 23, MOT_REGION.START_ADDRESS is bits [38:23]
    (((MemoryMap->GdxcMotBase + MemoryMap->GdxcMotSize) >> 3) << 16),
    GdxcBaseAddress
    );
    
  MrcOemMmioWrite (
    MPCOHTRK_CR_GDXC_OCLA_ADDRESS_LO_REG,
    MemoryMap->GdxcIotBase << 14,   // (GdxcIotBase << 20) >> 6, Current Pointer in cache line units
    GdxcBaseAddress
    );
  MrcOemMmioWrite (
    MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_REG,
    MPCOHTRK_CR_GDXC_OCLA_ADDRESS_HI_LOCK_MSK | (MemoryMap->GdxcIotBase & MRC_BIT18) >> 18, // Bit [18] will be bit [32], goes to IOT_ADDRESS_HI.MEM_PTR
    GdxcBaseAddress
    );
  MrcOemMmioWrite (
    MPCOHTRK_CR_GDXC_OCLA_REGION_REG,
    MemoryMap->GdxcIotBase >> 3 |  // (GdxcIotBase << 20) >> 23, OCLA_REGION.START_ADDRESS is bits [38:23]
    (((MemoryMap->GdxcIotBase + MemoryMap->GdxcIotSize) >> 3) << 16),
    GdxcBaseAddress
    );
  return;
}
