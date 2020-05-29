/**
This file contains an 'Intel Peripheral Driver' and uniquely
identified as "Intel Reference Module" and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor.  This file may
be modified by the user, subject to additional terms of the
license agreement

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MrcOemAddrDecode.c

@brief:
  Wrapper file for AddrDecode files.
**/

//
// Include files
//
#include "MrcApi.h"
#include "MrcGlobal.h"
#include "MrcOemAddrDecode.h"
#include "MrcOemIo.h"
#include "MrcOemMmio.h"
#include "McAddress.h"
#include "MrcHswMcAddrDecode.h"

/**
@brief
  Address decode function
  Converts system address to DRAM address

  @param[in]      SystemAddress  - The 39-bit system address to convert.
  @param[out]     DramAddress    - The dram address struct that the system address decodes to.

  @retval Returns 1 if successful, 0 otherwise.

**/
unsigned char
MrcMcAddressDecode (
  IN     unsigned long long SystemAddress,
  OUT    ADDRESS_DECODE     *DramAddress
  )
{
  unsigned char                        Status;
  U32                                  MchBarBaseAddress;
  U32                                  PciEBaseAddress;
  U32                                  Offset;
  MRC_PCI_000_TOLUD_STRUCT             Tolud;
  MRC_PCI_000_REMAPBASE_STRUCT         RemapBase;
  MRC_PCI_000_REMAPLIMIT_STRUCT        RemapLimit;
  MCDECS_CR_CHANNEL_HASH_MCMAIN_STRUCT ChannelHash;
  MCDECS_CR_MAD_ZR_MCMAIN_STRUCT       MadZr;
  MCDECS_CR_MAD_CHNL_MCMAIN_STRUCT     MadChnl;
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT DimmCh0McMain;
  MCDECS_CR_MAD_DIMM_CH1_MCMAIN_STRUCT DimmCh1McMain;
  U16                                  TempChannel;
  U16                                  TempDimm;
  U16                                  TempRank;
  U16                                  TempBank;
  U16                                  TempRow;
  U16                                  TempColumn;
  BOOL                                 IsTcm;

  Status = 0;

  //
  // Check that MCHBAR is programmed
  //
  MrcOemOutPort32 (MrcOemPciIndex (), MrcOemGetPciDeviceAddress (0, 0, 0, MRC_PCI_000_MCHBAR_REG));
  MchBarBaseAddress = MrcOemInPort32 (MrcOemPciData ());

  if ((MchBarBaseAddress & MRC_BIT0) == MRC_BIT0) {
    MchBarBaseAddress &= (~MRC_BIT0);
  } else {
    return Status;
  }

  //
  // Check that PCIEXBAR is programmed
  //
  MrcOemOutPort32 (MrcOemPciIndex(), MrcOemGetPciDeviceAddress (0, 0, 0, MRC_PCI_000_PCIEXBAR_REG));
  PciEBaseAddress = MrcOemInPort32 (MrcOemPciData ());

  if ((PciEBaseAddress & MRC_BIT0) == MRC_BIT0) {
    PciEBaseAddress &= ~(MRC_BIT2 | MRC_BIT1 | MRC_BIT0);
  } else {
    return Status;
  }

  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOLUD_REG);
  MrcOemMmioRead (Offset, &Tolud.Data, PciEBaseAddress);
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPBASE_REG);
  MrcOemMmioRead64 (Offset, &RemapBase.Data, PciEBaseAddress);
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPLIMIT_REG);
  MrcOemMmioRead64 (Offset, &RemapLimit.Data, PciEBaseAddress);
  MrcOemMmioRead (MCDECS_CR_CHANNEL_HASH_MCMAIN_REG, &ChannelHash.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_ZR_MCMAIN_REG, &MadZr.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_CHNL_MCMAIN_REG, &MadChnl.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG, &DimmCh0McMain.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG, &DimmCh1McMain.Data, MchBarBaseAddress);
  IsTcm = FALSE;

  Status = MrcHswDecode (
             SystemAddress,
             &IsTcm,
             Tolud.Data,
             RemapBase.Data,
             RemapLimit.Data,
             ChannelHash.Data,
             MadZr.Data,
             MadChnl.Data,
             DimmCh0McMain.Data,
             DimmCh1McMain.Data,
             0,
             &TempChannel,
             &TempDimm,
             &TempRank,
             &TempBank,
             &TempRow,
             &TempColumn
             );
  DramAddress->ChannelNumber = (U8) TempChannel;
  DramAddress->DIMMNumber    = (U8) TempDimm;
  DramAddress->Rank          = (U8) TempRank;
  DramAddress->Bank          = (U8) TempBank;
  DramAddress->RAS           = (U16) TempRow;
  DramAddress->CAS           = (U16) TempColumn;

  return Status;
}

/**
@brief
  Address encode function (reverse address decode)
  DRAM address to system address conversion

  @param[in]      DramAddress    - The dram address that is converted.
  @param[out]     SystemAddress  - The 39-bit system address to convert to.

  @retval Returns 1 if successful, 0 otherwise.

**/
unsigned char
MrcMcAddressEncode (
  IN     ADDRESS_DECODE     *DramAddress,
  OUT    unsigned long long *SystemAddress
  )
{
  unsigned char                        Status;
  U32                                  MchBarBaseAddress;
  U32                                  PciEBaseAddress;
  U32                                  Offset;
  MRC_PCI_000_TOLUD_STRUCT             Tolud;
  MRC_PCI_000_REMAPBASE_STRUCT         RemapBase;
  MRC_PCI_000_REMAPLIMIT_STRUCT        RemapLimit;
  MCDECS_CR_CHANNEL_HASH_MCMAIN_STRUCT ChannelHash;
  MCDECS_CR_MAD_ZR_MCMAIN_STRUCT       MadZr;
  MCDECS_CR_MAD_CHNL_MCMAIN_STRUCT     MadChnl;
  MCDECS_CR_MAD_DIMM_CH0_MCMAIN_STRUCT DimmCh0McMain;
  MCDECS_CR_MAD_DIMM_CH1_MCMAIN_STRUCT DimmCh1McMain;
  BOOL                                 IsTcm;

  Status = 0;

  //
  // Check that MCHBAR is programmed
  //
  MrcOemOutPort32 (MrcOemPciIndex (), MrcOemGetPciDeviceAddress (0, 0, 0, MRC_PCI_000_MCHBAR_REG));
  MchBarBaseAddress = MrcOemInPort32 (MrcOemPciData ());

  if ((MchBarBaseAddress & MRC_BIT0) == MRC_BIT0) {
    MchBarBaseAddress &= (~MRC_BIT0);
  } else {
    return Status;
  }

  //
  // Check that PCIEXBAR is programmed
  //
  MrcOemOutPort32 (MrcOemPciIndex(), MrcOemGetPciDeviceAddress (0, 0, 0, MRC_PCI_000_PCIEXBAR_REG));
  PciEBaseAddress = MrcOemInPort32 (MrcOemPciData ());

  if ((PciEBaseAddress & MRC_BIT0) == MRC_BIT0) {
    PciEBaseAddress &= ~(MRC_BIT2 | MRC_BIT1 | MRC_BIT0);
  } else {
    return Status;
  }

  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_TOLUD_REG);
  MrcOemMmioRead (Offset, &Tolud.Data, PciEBaseAddress);
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPBASE_REG);
  MrcOemMmioRead64 (Offset, &RemapBase.Data, PciEBaseAddress);
  Offset = MrcOemGetPcieDeviceAddress (0, 0, 0, MRC_PCI_000_REMAPLIMIT_REG);
  MrcOemMmioRead64 (Offset, &RemapLimit.Data, PciEBaseAddress);
  MrcOemMmioRead (MCDECS_CR_CHANNEL_HASH_MCMAIN_REG, &ChannelHash.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_ZR_MCMAIN_REG, &MadZr.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_CHNL_MCMAIN_REG, &MadChnl.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_DIMM_CH0_MCMAIN_REG, &DimmCh0McMain.Data, MchBarBaseAddress);
  MrcOemMmioRead (MCDECS_CR_MAD_DIMM_CH1_MCMAIN_REG, &DimmCh1McMain.Data, MchBarBaseAddress);
  IsTcm = FALSE;

  Status = MrcHswEncode (
             (U16) DramAddress->ChannelNumber,
             (U16) DramAddress->DIMMNumber,
             (U16) DramAddress->Rank,
             (U16) DramAddress->Bank,
             (U16) DramAddress->RAS,
             (U16) DramAddress->CAS,
             Tolud.Data,
             RemapBase.Data,
             RemapLimit.Data,
             ChannelHash.Data,
             MadZr.Data,
             MadChnl.Data,
             DimmCh0McMain.Data,
             DimmCh1McMain.Data,
             0,
             SystemAddress,
             &IsTcm
             );

  return Status;
}
