/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchAccess.h

@brief
  Macros that simplify accessing PCH devices's PCI registers.

  ** NOTE ** these macros assume the PCH device is on BUS 0

**/
#ifndef _PCH_ACCESS_H_
#define _PCH_ACCESS_H_

#include "PchRegs.h"

#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND 1
#endif
#ifndef STALL_ONE_SECOND
#define STALL_ONE_SECOND 1000000
#endif

///
/// Memory Mapped PCI Access macros
///
///
/// PCI Device MM Base
///
#ifndef MmPciAddress
#define MmPciAddress(Segment, Bus, Device, Function, Register) \
  ((UINTN) PcdGet64 (PcdPciExpressBaseAddress) + \
   (UINTN) (Bus << 20) + \
   (UINTN) (Device << 15) + \
   (UINTN) (Function << 12) + \
   (UINTN) (Register) \
  )
#endif
///
/// Pch Controller PCI access macros
///
#define PCH_RCRB_BASE ( \
  MmioRead32 (MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  R_PCH_LPC_RCBA)) &~BIT0 \
  )

//
// Device 0x1b, Function 0
//
#define PchAzaliaPciCfg32(Register) \
  MmioRead32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register) \
  )

#define PchAzaliaPciCfg32Or(Register, OrData) \
  MmioOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  OrData \
  )

#define PchAzaliaPciCfg32And(Register, AndData) \
  MmioAnd32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  AndData \
  )

#define PchAzaliaPciCfg32AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  OrData \
  )

#define PchAzaliaPciCfg16(Register) \
  MmioRead16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register) \
  )

#define PchAzaliaPciCfg16Or(Register, OrData) \
  MmioOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  OrData \
  )

#define PchAzaliaPciCfg16And(Register, AndData) \
  MmioAnd16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  AndData \
  )

#define PchAzaliaPciCfg16AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  AndData, \
  OrData \
  )

#define PchAzaliaPciCfg8(Register)  MmioRead8 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_AZALIA, 0, Register))

#define PchAzaliaPciCfg8Or(Register, OrData) \
  MmioOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  OrData \
  )

#define PchAzaliaPciCfg8And(Register, AndData) \
  MmioAnd8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  AndData \
  )

#define PchAzaliaPciCfg8AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_AZALIA, \
  0, \
  Register), \
  AndData, \
  OrData \
  )

//
// Device 0x1f, Function 0
//
#define PchLpcPciCfg32(Register)  MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, Register))

#define PchLpcPciCfg32Or(Register, OrData) \
  MmioOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  OrData \
  )

#define PchLpcPciCfg32And(Register, AndData) \
  MmioAnd32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  AndData \
  )

#define PchLpcPciCfg32AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  AndData, \
  OrData \
  )

#define PchLpcPciCfg16(Register)  MmioRead16 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, Register))

#define PchLpcPciCfg16Or(Register, OrData) \
  MmioOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  OrData \
  )

#define PchLpcPciCfg16And(Register, AndData) \
  MmioAnd16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  AndData \
  )

#define PchLpcPciCfg16AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  AndData, \
  OrData \
  )

#define PchLpcPciCfg8(Register) MmioRead8 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_LPC, 0, Register))

#define PchLpcPciCfg8Or(Register, OrData) \
  MmioOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  OrData \
  )

#define PchLpcPciCfg8And(Register, AndData) \
  MmioAnd8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  AndData \
  )

#define PchLpcPciCfg8AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_LPC, \
  0, \
  Register), \
  AndData, \
  OrData \
  )

//
// SATA 1 device 0x1f, Function 2
//
#define PchSataPciCfg32(Register) MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, 2, Register))

#define PchSataPciCfg32Or(Register, OrData) \
  MmioOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  OrData \
  )

#define PchSataPciCfg32And(Register, AndData) \
  MmioAnd32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  AndData \
  )

#define PchSataPciCfg32AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  AndData, \
  OrData \
  )

#define PchSataPciCfg16(Register) MmioRead16 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, 2, Register))

#define PchSataPciCfg16Or(Register, OrData) \
  MmioOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  OrData \
  )

#define PchSataPciCfg16And(Register, AndData) \
  MmioAnd16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  AndData \
  )

#define PchSataPciCfg16AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  AndData, \
  OrData \
  )

#define PchSataPciCfg8(Register)  MmioRead8 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA, 2, Register))

#define PchSataPciCfg8Or(Register, OrData) \
  MmioOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  OrData \
  )

#define PchSataPciCfg8And(Register, AndData) \
  MmioAnd8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  AndData \
  )

#define PchSataPciCfg8AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA, \
  2, \
  Register), \
  AndData, \
  OrData \
  )

//
// SATA 2 device 0x1f, Function 5
//
#define PchSata2PciCfg32(Register)  MmioRead32 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA2, 5, Register))

#define PchSata2PciCfg32Or(Register, OrData) \
  MmioOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  OrData \
  )

#define PchSata2PciCfg32And(Register, AndData) \
  MmioAnd32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  AndData \
  )

#define PchSata2PciCfg32AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr32 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  AndData, \
  OrData \
  )

#define PchSata2PciCfg16(Register)  MmioRead16 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA2, 5, Register))

#define PchSata2PciCfg16Or(Register, OrData) \
  MmioOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  OrData \
  )

#define PchSata2PciCfg16And(Register, AndData) \
  MmioAnd16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  AndData \
  )

#define PchSata2PciCfg16AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr16 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  AndData, \
  OrData \
  )

#define PchSata2PciCfg8(Register) MmioRead8 (MmPciAddress (DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_SATA2, 5, Register))

#define PchSata2PciCfg8Or(Register, OrData) \
  MmioOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  OrData \
  )

#define PchSata2PciCfg8And(Register, AndData) \
  MmioAnd8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  AndData \
  )

#define PchSata2PciCfg8AndThenOr(Register, AndData, OrData) \
  MmioAndThenOr8 ( \
  MmPciAddress (0, \
  DEFAULT_PCI_BUS_NUMBER_PCH, \
  PCI_DEVICE_NUMBER_PCH_SATA2, \
  5, \
  Register), \
  AndData, \
  OrData \
  )

//
// Root Complex Register Block
//
#define PchMmRcrb32(Register)                           MmioRead32 (PCH_RCRB_BASE + Register)

#define PchMmRcrb32Or(Register, OrData)                 MmioOr32 (PCH_RCRB_BASE + Register, OrData)

#define PchMmRcrb32And(Register, AndData)               MmioAnd32 (PCH_RCRB_BASE + Register, AndData)

#define PchMmRcrb32AndThenOr(Register, AndData, OrData) MmioAndThenOr32 (PCH_RCRB_BASE + Register, AndData, OrData)

#define PchMmRcrb16(Register)                           MmioRead16 (PCH_RCRB_BASE + Register)

#define PchMmRcrb16Or(Register, OrData)                 MmioOr16 (PCH_RCRB_BASE + Register, OrData)

#define PchMmRcrb16And(Register, AndData)               MmioAnd16 (PCH_RCRB_BASE + Register, AndData)

#define PchMmRcrb16AndThenOr(Register, AndData, OrData) MmioAndThenOr16 (PCH_RCRB_BASE + Register, AndData, OrData)

#define PchMmRcrb8(Register)                            MmioRead8 (PCH_RCRB_BASE + Register)

#define PchMmRcrb8Or(Register, OrData)                  MmioOr8 (PCH_RCRB_BASE + Register, OrData)

#define PchMmRcrb8And(Register, AndData)                MmioAnd8 (PCH_RCRB_BASE + Register, AndData)

#define PchMmRcrb8AndThenOr(Register, AndData, OrData)  MmioAndThenOr8 (PCH_RCRB_BASE + Register, AndData, OrData)

#endif
