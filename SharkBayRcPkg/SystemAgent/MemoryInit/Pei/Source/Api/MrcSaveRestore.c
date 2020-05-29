/** @file

  Power state and boot mode save and restore data functions.
    
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/
//
// Include files
//
#include <MrcTypes.h>
#include <MrcCommon.h>
#include <MrcGeneral.h>
#include <MrcGlobal.h>
#include <MrcOem.h>
#include <MrcSaveRestore.h>
#include <MrcSpdProcessing.h>

//
// ------- IMPORTANT NOTE --------
// MRC_MC_REGISTER_COUNT in Global.h should match this table.
// Update this define whenever you add/remove registers from this table.
//
// Total register count = 1872 + 624 = 2496
//
const SaveDataControl SaveDataArray[] = {
  {0x0000, 0x003C}, {0x0048, 0x004C}, {0x005C, 0x0078},  // 40h + 8 + 20h = 68h => 104 * 18 = 1872
  {0x0100, 0x013C}, {0x0148, 0x014C}, {0x015C, 0x0178},
  {0x0200, 0x023C}, {0x0248, 0x024C}, {0x025C, 0x0278},
  {0x0300, 0x033C}, {0x0348, 0x034C}, {0x035C, 0x0378},
  {0x0400, 0x043C}, {0x0448, 0x044C}, {0x045C, 0x0478},
  {0x0500, 0x053C}, {0x0548, 0x054C}, {0x055C, 0x0578},
  {0x0600, 0x063C}, {0x0648, 0x064C}, {0x065C, 0x0678},
  {0x0700, 0x073C}, {0x0748, 0x074C}, {0x075C, 0x0778},
  {0x0800, 0x083C}, {0x0848, 0x084C}, {0x085C, 0x0878},
  {0x0900, 0x093C}, {0x0948, 0x094C}, {0x095C, 0x0978},
  {0x0A00, 0x0A3C}, {0x0A48, 0x0A4C}, {0x0A5C, 0x0A78},
  {0x0B00, 0x0B3C}, {0x0B48, 0x0B4C}, {0x0B5C, 0x0B78},
  {0x0C00, 0x0C3C}, {0x0C48, 0x0C4C}, {0x0C5C, 0x0C78},
  {0x0D00, 0x0D3C}, {0x0D48, 0x0D4C}, {0x0D5C, 0x0D78},
  {0x0E00, 0x0E3C}, {0x0E48, 0x0E4C}, {0x0E5C, 0x0E78},
  {0x0F00, 0x0F3C}, {0x0F48, 0x0F4C}, {0x0F5C, 0x0F78},
  {0x1000, 0x103C}, {0x1048, 0x104C}, {0x105C, 0x1078},
  {0x1100, 0x113C}, {0x1148, 0x114C}, {0x115C, 0x1178},
  {0x1204, 0x1208},  //   8
  {0x1214, 0x121C},  //  12
  {0x1304, 0x1308},  //   8
  {0x1314, 0x131C},  //  12
  {0x1404, 0x140C},  //  12
  {0x1504, 0x150C},  //  12
  {0x1808, 0x1810},  //  12
  {0x1908, 0x1910},  //  12
  {0x1A04, 0x1A0C},  //  12
  {0x1B04, 0x1B0C},  //  12
  {0x1C14, 0x1C1C},  //  12
  {0x1D14, 0x1D1C},  //  12
  {0x2000, 0x2008},  //  12
  {0x3A14, 0x3A1C},  //  12
  {0x3A24, 0x3A24},  //   4
  {0x4000, 0x4014},  //  24
  {0x4024, 0x4028},  //   8
  {0x40D0, 0x40D0},  //   4
  {0x4220, 0x4224},  //   8
  {0x4294, 0x4294},  //   4
  {0x429C, 0x42A0},  //   8
  {0x42EC, 0x42FC},  //  20
  {0x438C, 0x4390},  //   8
  {0x4328, 0x4328},  //   4
  {0x4400, 0x4414},  //  24
  {0x4424, 0x4428},  //   8
  {0x44D0, 0x44D0},  //   4
  {0x4620, 0x4624},  //   8
  {0x4694, 0x4694},  //   4
  {0x469C, 0x46A0},  //   8
  {0x46EC, 0x46FC},  //  20
  {0x4728, 0x4728},  //   4
  {0x478C, 0x4790},  //   8
  {0x5884, 0x5888},  //   8
  {0x5890, 0x589C},  //  16
  {0x58A4, 0x58A4},  //   4
  {0x58D0, 0x58E4},  //  24
  {0x5880, 0x5880},  //   4
  {0x5000, 0x50DC},  // 224
  {0x59b8, 0x59b8}   //   4
};                   // = 624

/**
@brief
  This function verifies that neither CPU fuses or DIMMs have changed.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if fast boot is allowed, otherwise mrcColdBootRequired.
**/
MrcStatus
MrcFastBootPermitted (
  IN     MrcParameters *const MrcData
  )
{
  const MrcInput            *Inputs;
  const MrcDimmIn           *DimmIn;
  const U8                  *CrcStart;
  MrcSaveData               *Save;
  MrcDimmOut                *DimmSave;
  MRC_PCI_000_CAPID0_STRUCT Capid0Reg;
  U32                       CrcSize;
  U8                        Controller;
  U8                        Channel;
  U8                        Dimm;
  U16                       DimmCrc;
  U32                       Offset;

  CrcStart = NULL;
  CrcSize  = 0;
  Inputs   = &MrcData->SysIn.Inputs;
  Save     = &MrcData->SysSave.Save.Data;

  //
  // Obtain the capabilities of the memory controller and see if they have changed.
  //
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_CAPID0_REG);
  MrcOemMmioRead (Offset, &Capid0Reg.Data32.A.Data, Inputs->PciEBaseAddress);
  MrcOemMmioRead (Offset + 4, &Capid0Reg.Data32.B.Data, Inputs->PciEBaseAddress);
  if (Capid0Reg.Data != Save->McCapId.Data) {
    MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Capabilities have changed, cold boot required\n");
    MRC_DEBUG_MSG (
      &Inputs->Debug,
      MSG_LEVEL_NOTE,
      "  '%X_%X' --> '%X_%X'\n",
      Save->McCapId.Data32[1],
      Save->McCapId.Data32[0],
      Capid0Reg.Data32.B.Data,
      Capid0Reg.Data32.A.Data
      );
    return mrcColdBootRequired;
  }
  //
  // See if any of the DIMMs have changed.
  //
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmIn   = &Inputs->Controller[Controller].Channel[Channel].Dimm[Dimm];
        DimmSave = &Save->Controller[Controller].Channel[Channel].Dimm[Dimm];
        if (DimmIn->Status == DIMM_DISABLED) {
          DimmCrc = 0;
        } else {
          CrcStart = MrcSpdCrcArea (MrcData, Controller, Channel, Dimm, &CrcSize);
          GetDimmCrc ((const U8 *const) CrcStart, CrcSize, &DimmCrc);
        }

        if (DimmCrc != DimmSave->Crc) {
          MRC_DEBUG_MSG (
            &Inputs->Debug,
            MSG_LEVEL_NOTE,
            "Channel %u Dimm %u has changed, cold boot required\n",
            Channel,
            Dimm
            );
          MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "  DimmCrc %Xh, DimmSave->Crc %Xh\n", DimmCrc, DimmSave->Crc);
          return mrcColdBootRequired;
        }
      }
    }
  }
  //
  // Set RestoreMRs flag to use trained Opt Param Values for Power Savings.
  //
  MrcData->SysOut.Outputs.RestoreMRs = TRUE;

  return mrcSuccess;
}

/**
@brief
  This function saves any values that need to be used during non-cold boots.

  @param[in, out] MrcData - Include all the MRC global data.

  @retval mrcSuccess if the save occurred with no errors, otherwise returns an error code.
**/
MrcStatus
MrcSaveMCValues (
  IN OUT MrcParameters *const MrcData
  )
{
  const SaveDataControl *SaveIt;
  const MrcInput        *Inputs;
  const MrcControllerIn *ControllerIn;
  const MrcChannelIn    *ChannelIn;
  const MrcSpd          *SpdIn;
  MrcOutput             *Outputs;
  MrcSaveData           *SaveData;
  MrcSaveHeader         *SaveHeader;
  MrcControllerOut      *ControllerOut;
  MrcChannelOut         *ChannelOut;
  MrcContSave           *ControllerSave;
  MrcChannelSave        *ChannelSave;
  MrcSpdSave            *SpdSavePtr;
  U32                   *McRegister;
  U8                    *SpdBegin;
  MrcProfile            Profile;
  U32                   Offset;
  U32                   Index;
  U32                   Value;
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U8                    CopySize;

  //
  // Copy channel and DIMM information to the data area that will be saved.
  //
  Inputs      = &MrcData->SysIn.Inputs;
  Outputs     = &MrcData->SysOut.Outputs;
  SaveData    = &MrcData->SysSave.Save.Data;
  SaveHeader  = &MrcData->SysSave.Save.Header;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn                  = &Inputs->Controller[Controller];
    ControllerOut                 = &Outputs->Controller[Controller];
    ControllerSave                = &SaveData->Controller[Controller];
    ControllerSave->ChannelCount  = ControllerOut->ChannelCount;
    ControllerSave->Status        = ControllerOut->Status;

    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn   = &ControllerIn->Channel[Channel];
      ChannelOut  = &ControllerOut->Channel[Channel];
      ChannelSave                   = &ControllerSave->Channel[Channel];
      ChannelSave->DimmCount        = ChannelOut->DimmCount;
      ChannelSave->ValidRankBitMask = ChannelOut->ValidRankBitMask;
      ChannelSave->EccSupport       = ChannelOut->EccSupport;
      ChannelSave->Status           = ChannelOut->Status;
      for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
        MrcOemMemoryCpy ((U8 *) &ChannelSave->Timing[Profile], (U8 *) &ChannelOut->Timing[Profile], sizeof (MrcTiming));
      }
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        MrcOemMemoryCpy ((U8 *) &ChannelSave->Dimm[Dimm], (U8 *) &ChannelOut->Dimm[Dimm], sizeof (MrcDimmOut));
        SpdIn = &ChannelIn->Dimm[Dimm].Spd;
        SpdSavePtr = &ChannelSave->SpdSave[Dimm];
        {
          SpdSavePtr->SmbiosData.ModuleType           = SpdIn->Ddr3.General.ModuleType;
          SpdSavePtr->SmbiosData.ModuleMemoryBusWidth = SpdIn->Ddr3.General.ModuleMemoryBusWidth;
          SpdBegin = (U8 *) &SpdIn->Ddr3.ModuleId;
          CopySize = sizeof (SpdSavePtr->ManufacturingData.Ddr3Data);
        }
        //
        // Save just enough SPD information so it can be restored during non-cold boot.
        //
        MrcOemMemoryCpy ((U8 *) &SpdSavePtr->ManufacturingData, SpdBegin, CopySize);
      } // for Dimm
    } // for Channel
  } // for Controller

  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    SaveData->VddVoltage[Profile] = Outputs->VddVoltage[Profile];
  }

  //
  // Copy specified memory controller MMIO registers to the data area that will be saved.
  //
  McRegister = SaveData->McRegister;
  for (Index = 0; Index < (sizeof (SaveDataArray) / sizeof (SaveDataControl)); Index++) {
    SaveIt = &SaveDataArray[Index];
    for (Offset = SaveIt->StartMchbarOffset; Offset <= SaveIt->EndMchbarOffset; Offset += sizeof (U32)) {
      Value         = MrcReadCR (MrcData, Offset);
      *McRegister++ = Value;
    }
  }

//
// ------- IMPORTANT NOTE --------
// MeStolenSize should not be saved/restored.  There is no rule stating that ME FW cannot request a different
// amount of ME UMA space from one boot to the next.  Also, if ME FW is updated/changed, the UMA Size requested
// from the previous version should not be restored.
//

  MrcVersionGet (&SaveData->Version);
  SaveData->CpuModel         = Inputs->CpuModel;
  SaveData->CpuStepping      = Inputs->CpuStepping;
  SaveData->Frequency        = Outputs->Frequency;
  SaveData->MemoryClock      = Outputs->MemoryClock;
  SaveData->Ratio            = Outputs->Ratio;
  SaveData->RefClk           = Outputs->RefClk;
  SaveData->EccSupport       = Outputs->EccSupport;
  SaveData->DdrType          = Outputs->DdrType;
  SaveData->XmpProfileEnable = Outputs->XmpProfileEnable;

  SaveData->SaMemCfgCrc = MrcCalculateCrc32 ((U8 *) Inputs->SaMemCfgAddress, Inputs->SaMemCfgSize);
  SaveHeader->Crc       = MrcCalculateCrc32 ((U8 *) SaveData, sizeof (MrcSaveData));
  MRC_DEBUG_MSG (&Inputs->Debug, MSG_LEVEL_NOTE, "Saved data CRC = %xh\n", SaveHeader->Crc);

  return mrcSuccess;
}

/**
@brief
  This function copies the non-training information that needs to be restored
  from the 'save' data structure to the 'Output' data structure.

  @param[in, out] MrcData - include all the MRC global data.

  @retval mrcSuccess if the copy completed with no errors, otherwise returns an error code.
**/
MrcStatus
MrcRestoreNonTrainingValues (
  IN OUT MrcParameters *const MrcData
  )
{
  MrcInput          *Inputs;
  MrcControllerIn   *ControllerIn;
  MrcChannelIn      *ChannelIn;
  MrcSaveData       *SaveData;
  MrcContSave       *ControllerSave;
  MrcChannelSave    *ChannelSave;
  MrcDimmOut        *DimmSave;
  MrcOutput         *Outputs;
  MrcControllerOut  *ControllerOut;
  MrcChannelOut     *ChannelOut;
  MrcDimmOut        *DimmOut;
  MrcSpd            *SpdIn;
  MrcSpdSave        *SpdSavePtr;
  U8                *SpdBegin;
  MrcProfile        Profile;
  U8                Controller;
  U8                Channel;
  U8                Dimm;
  U8                CopySize;

  SaveData = &MrcData->SysSave.Save.Data;
  Outputs  = &MrcData->SysOut.Outputs;
  Inputs = &MrcData->SysIn.Inputs;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn                = &Inputs->Controller[Controller];
    ControllerSave              = &SaveData->Controller[Controller];
    ControllerOut               = &Outputs->Controller[Controller];
    ControllerOut->ChannelCount = ControllerSave->ChannelCount;
    ControllerOut->Status       = ControllerSave->Status;
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn                     = &ControllerIn->Channel[Channel];
      ChannelSave                   = &ControllerSave->Channel[Channel];
      ChannelOut                    = &ControllerOut->Channel[Channel];
      ChannelOut->DimmCount         = ChannelSave->DimmCount;
      ChannelOut->ValidRankBitMask  = ChannelSave->ValidRankBitMask;
      ChannelOut->EccSupport        = ChannelSave->EccSupport;
      ChannelOut->Status            = ChannelSave->Status;
      for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
        MrcOemMemoryCpy ((U8 *) &ChannelOut->Timing[Profile], (U8 *) &ChannelSave->Timing[Profile], sizeof (MrcTiming));
      }
      for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
        DimmSave = &ChannelSave->Dimm[Dimm];
        if (DimmSave->Status == DIMM_PRESENT || DimmSave->Status == DIMM_DISABLED) {
          DimmOut     = &ChannelOut->Dimm[Dimm];
          SpdIn       = &ChannelIn->Dimm[Dimm].Spd;
          SpdSavePtr  = &ChannelSave->SpdSave[Dimm];
          MrcOemMemoryCpy ((U8 *) DimmOut, (U8 *) DimmSave, sizeof (MrcDimmOut));
          {
            SpdIn->Ddr3.General.ModuleType           = SpdSavePtr->SmbiosData.ModuleType;
            SpdIn->Ddr3.General.ModuleMemoryBusWidth = SpdSavePtr->SmbiosData.ModuleMemoryBusWidth;
            SpdBegin = (U8 *) &SpdIn->Ddr3.ModuleId;
            CopySize = sizeof (SpdSavePtr->ManufacturingData.Ddr3Data);
          }
          //
          // Restore just enough SPD information so it can be passed out in the HOB.
          //
          MrcOemMemoryCpy (SpdBegin, (U8 *) &SpdSavePtr->ManufacturingData, CopySize);
        } // if
      } // for Dimm
    } // for Channel
  } // for Controller

  for (Profile = STD_PROFILE; Profile < MAX_PROFILE; Profile++) {
    Outputs->VddVoltage[Profile] = SaveData->VddVoltage[Profile];
  }

//
// ------- IMPORTANT NOTE --------
// MeStolenSize should not be saved/restored.  There is no rule stating that ME FW cannot request a different
// amount of ME UMA space from one boot to the next.  Also, if ME FW is updated/changed, the UMA Size requested
// from the previous version should not be restored.
//

  Inputs->CpuModel          = SaveData->CpuModel;
  Inputs->CpuStepping       = SaveData->CpuStepping;
  Outputs->Frequency        = SaveData->Frequency;
  Outputs->MemoryClock      = SaveData->MemoryClock;
  Outputs->Ratio            = SaveData->Ratio;
  Outputs->RefClk           = SaveData->RefClk;
  Outputs->EccSupport       = SaveData->EccSupport;
  Outputs->DdrType          = SaveData->DdrType;
  Outputs->XmpProfileEnable = SaveData->XmpProfileEnable;

  return mrcSuccess;
}

/**
@brief
  This function writes the previously determined training values back to the memory controller.

  @param[in] MrcData - Include all the MRC global data.

  @retval mrcSuccess if the memory controller write back completed with no errors, otherwise returns an error code.
**/
MrcStatus
MrcRestoreTrainingValues (
  IN     MrcParameters *const MrcData
  )
{
  const SaveDataControl *RestoreIt;
  U32                   *McRegister;
  U32                   Offset;
  U32                   Index;
  U32                   Value;

  McRegister = MrcData->SysSave.Save.Data.McRegister;
  for (Index = 0; Index < (sizeof (SaveDataArray) / sizeof (SaveDataControl)); Index++) {
    RestoreIt = &SaveDataArray[Index];
    for (Offset = RestoreIt->StartMchbarOffset; Offset <= RestoreIt->EndMchbarOffset; Offset += sizeof (U32)) {
      Value = *McRegister++;
      MrcWriteCR (MrcData, Offset, Value);
    }
  }

  return mrcSuccess;
}

/**
@brief
  Calculates a CRC-32 of the specified data buffer.

  @param[in] Data     - Pointer to the data buffer.
  @param[in] DataSize - Size of the data buffer, in bytes.

  @retval The CRC-32 value.
**/
U32
MrcCalculateCrc32 (
  IN     const U8          *const Data,
  IN     const U32         DataSize
  )
{
  U32 i;
  U32 j;
  U32 crc;
  U32 CrcTable[256];

  crc = (U32) (-1);

  //
  // Initialize the CRC base table.
  //
  for (i = 0; i < 256; i++) {
    CrcTable[i] = i;
    for (j = 8; j > 0; j--) {
      CrcTable[i] = (CrcTable[i] & 1) ? (CrcTable[i] >> 1) ^ 0xEDB88320 : CrcTable[i] >> 1;
    }
  }
  //
  // Calculate the CRC.
  //
  for (i = 0; i < DataSize; i++) {
    crc = (crc >> 8) ^ CrcTable[(U8) crc ^ (Data)[i]];
  }

  return ~crc;
}
