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
  SaAccess.h

@brief
  Macros to simplify and abstract the interface to PCI configuration.

**/
#ifndef _SAACCESS_H_
#define _SAACCESS_H_

#include "SaRegs.h"
#include "SaCommonDefinitions.h"

///
/// SystemAgent Base Address definition
///
#ifndef MCH_BASE_ADDRESS
#define MCH_BASE_ADDRESS  0xfed10000
#endif
#ifndef STALL_ONE_MICRO_SECOND
#define STALL_ONE_MICRO_SECOND  1
#endif
#ifndef STALL_ONE_MILLI_SECOND
#define STALL_ONE_MILLI_SECOND  1000
#endif

//
// SA DMI configuration
//
#define SA_DMI_MAX_LANE    0x04
#define SA_DMI_MAX_BUNDLE  0x02

//
// SA PCI Express* Port configuration
//
#define SA_PEG_BUS_NUM     0x00
#define SA_PEG_DEV_NUM     0x01
#define SA_PEG10_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG10_FUN_NUM   0x00
#define SA_PEG11_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG11_FUN_NUM   0x01
#define SA_PEG12_DEV_NUM   SA_PEG_DEV_NUM
#define SA_PEG12_FUN_NUM   0x02
#define SA_PEG_MAX_FUN     0x03
#define SA_PEG_MAX_LANE    0x10
#define SA_PEG_MAX_BUNDLE  0x08

#define SA_SWING_HALF     0x1
#define SA_SWING_FULL     0x2

///
/// PCI Express* Port configuration Hardware Strapping value
///
#define SA_PEG_x8_x4_x4  0x00
#define SA_PEG_x8_x8_x0  0x02
#define SA_PEG_x16_x0_x0 0x03

///
/// System Agent PCI access macros
///
///
/// Device #, Function #
///
#define McDevFunPciCfg64(Bus, Dev, Func, Register)              MmPci64 (0, Bus, Dev, Func, Register)
#define McDevFunPciCfg64Or(Bus, Dev, Func, Register, OrData)    MmPci64Or (0, Bus, Dev, Func, Register, OrData)
#define McDevFunPciCfg64And(Bus, Dev, Func, Register, AndData)  MmPci64And (0, Bus, Dev, Func, Register, AndData)
#define McDevFunPciCfg64AndThenOr(Bus, Dev, Func, Register, AndData, OrData) \
  MmPci64AndThenOr (0, \
                    Bus, \
                    Dev, \
                    Func, \
                    Register, \
                    AndData, \
                    OrData \
      )

#define McDevFunPciCfg32(Bus, Dev, Func, Register)              MmPci32 (0, Bus, Dev, Func, Register)
#define McDevFunPciCfg32Or(Bus, Dev, Func, Register, OrData)    MmPci32Or (0, Bus, Dev, Func, Register, OrData)
#define McDevFunPciCfg32And(Bus, Dev, Func, Register, AndData)  MmPci32And (0, Bus, Dev, Func, Register, AndData)
#define McDevFunPciCfg32AndThenOr(Bus, Dev, Func, Register, AndData, OrData) \
  MmPci32AndThenOr (0, \
                    Bus, \
                    Dev, \
                    Func, \
                    Register, \
                    AndData, \
                    OrData \
      )

#define McDevFunPciCfg16(Bus, Dev, Func, Register)              MmPci16 (0, Bus, Dev, Func, Register)
#define McDevFunPciCfg16Or(Bus, Dev, Func, Register, OrData)    MmPci16Or (0, Bus, Dev, Func, Register, OrData)
#define McDevFunPciCfg16And(Bus, Dev, Func, Register, AndData)  MmPci16And (0, Bus, Dev, Func, Register, AndData)
#define McDevFunPciCfg16AndThenOr(Bus, Dev, Func, Register, AndData, OrData) \
  MmPci16AndThenOr (0, \
                    Bus, \
                    Dev, \
                    Func, \
                    Register, \
                    AndData, \
                    OrData \
      )

#define McDevFunPciCfg8(Bus, Dev, Func, Register)             MmPci8 (0, Bus, Dev, Func, Register)
#define McDevFunPciCfg8Or(Bus, Dev, Func, Register, OrData)   MmPci8Or (0, Bus, Dev, Func, Register, OrData)
#define McDevFunPciCfg8And(Bus, Dev, Func, Register, AndData) MmPci8And (0, Bus, Dev, Func, Register, AndData)
#define McDevFunPciCfg8AndThenOr(Bus, Dev, Func, Register, AndData, OrData) \
  MmPci8AndThenOr (0, \
                   Bus, \
                   Dev, \
                   Func, \
                   Register, \
                   AndData, \
                   OrData \
      )

///
/// Device 0, Function 0
///
#define McD0PciCfg64(Register)                            MmPci64 (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg64Or(Register, OrData)                  MmPci64Or (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg64And(Register, AndData)                MmPci64And (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg64AndThenOr(Register, AndData, OrData)  MmPci64AndThenOr (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

#define McD0PciCfg32(Register)                            MmPci32 (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg32Or(Register, OrData)                  MmPci32Or (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg32And(Register, AndData)                MmPci32And (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

#define McD0PciCfg16(Register)                            MmPci16 (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg16Or(Register, OrData)                  MmPci16Or (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg16And(Register, AndData)                MmPci16And (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg16AndThenOr(Register, AndData, OrData)  MmPci16AndThenOr (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

#define McD0PciCfg8(Register)                             MmPci8 (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg8Or(Register, OrData)                   MmPci8Or (0, SA_MC_BUS, 0, 0, Register, OrData)
#define McD0PciCfg8And(Register, AndData)                 MmPci8And (0, SA_MC_BUS, 0, 0, Register, AndData)
#define McD0PciCfg8AndThenOr(Register, AndData, OrData)   MmPci8AndThenOr (0, SA_MC_BUS, 0, 0, Register, AndData, OrData)

///
/// Device 1, Function 0
///
#define McD1PciCfg64(Register)                            MmPci64 (0, SA_MC_BUS, 1, 0, Register)
#define McD1PciCfg64Or(Register, OrData)                  MmPci64Or (0, SA_MC_BUS, 1, 0, Register, OrData)
#define McD1PciCfg64And(Register, AndData)                MmPci64And (0, SA_MC_BUS, 1, 0, Register, AndData)
#define McD1PciCfg64AndThenOr(Register, AndData, OrData)  MmPci64AndThenOr (0, SA_MC_BUS, 1, 0, Register, AndData, OrData)

#define McD1PciCfg32(Register)                            MmPci32 (0, SA_MC_BUS, 1, 0, Register)
#define McD1PciCfg32Or(Register, OrData)                  MmPci32Or (0, SA_MC_BUS, 1, 0, Register, OrData)
#define McD1PciCfg32And(Register, AndData)                MmPci32And (0, SA_MC_BUS, 1, 0, Register, AndData)
#define McD1PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, SA_MC_BUS, 1, 0, Register, AndData, OrData)

#define McD1PciCfg16(Register)                            MmPci16 (0, SA_MC_BUS, 1, 0, Register)
#define McD1PciCfg16Or(Register, OrData)                  MmPci16Or (0, SA_MC_BUS, 1, 0, Register, OrData)
#define McD1PciCfg16And(Register, AndData)                MmPci16And (0, SA_MC_BUS, 1, 0, Register, AndData)
#define McD1PciCfg16AndThenOr(Register, AndData, OrData)  MmPci16AndThenOr (0, SA_MC_BUS, 1, 0, Register, AndData, OrData)

#define McD1PciCfg8(Register)                             MmPci8 (0, SA_MC_BUS, 1, 0, Register)
#define McD1PciCfg8Or(Register, OrData)                   MmPci8Or (0, SA_MC_BUS, 1, 0, Register, OrData)
#define McD1PciCfg8And(Register, AndData)                 MmPci8And (0, SA_MC_BUS, 1, 0, Register, AndData)
#define McD1PciCfg8AndThenOr(Register, AndData, OrData)   MmPci8AndThenOr (0, SA_MC_BUS, 1, 0, Register, AndData, OrData)

///
/// Device 1, Function 1
///
#define McD1F1PciCfg64(Register)                            MmPci64 (0, SA_MC_BUS, 1, 1, Register)
#define McD1F1PciCfg64Or(Register, OrData)                  MmPci64Or (0, SA_MC_BUS, 1, 1, Register, OrData)
#define McD1F1PciCfg64And(Register, AndData)                MmPci64And (0, SA_MC_BUS, 1, 1, Register, AndData)
#define McD1F1PciCfg64AndThenOr(Register, AndData, OrData)  MmPci64AndThenOr (0, SA_MC_BUS, 1, 1, Register, AndData, OrData)

#define McD1F1PciCfg32(Register)                            MmPci32 (0, SA_MC_BUS, 1, 1, Register)
#define McD1F1PciCfg32Or(Register, OrData)                  MmPci32Or (0, SA_MC_BUS, 1, 1, Register, OrData)
#define McD1F1PciCfg32And(Register, AndData)                MmPci32And (0, SA_MC_BUS, 1, 1, Register, AndData)
#define McD1F1PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, SA_MC_BUS, 1, 1, Register, AndData, OrData)

#define McD1F1PciCfg16(Register)                            MmPci16 (0, SA_MC_BUS, 1, 1, Register)
#define McD1F1PciCfg16Or(Register, OrData)                  MmPci16Or (0, SA_MC_BUS, 1, 1, Register, OrData)
#define McD1F1PciCfg16And(Register, AndData)                MmPci16And (0, SA_MC_BUS, 1, 1, Register, AndData)
#define McD1F1PciCfg16AndThenOr(Register, AndData, OrData)  MmPci16AndThenOr (0, SA_MC_BUS, 1, 1, Register, AndData, OrData)

#define McD1F1PciCfg8(Register)                             MmPci8 (0, SA_MC_BUS, 1, 1, Register)
#define McD1F1PciCfg8Or(Register, OrData)                   MmPci8Or (0, SA_MC_BUS, 1, 1, Register, OrData)
#define McD1F1PciCfg8And(Register, AndData)                 MmPci8And (0, SA_MC_BUS, 1, 1, Register, AndData)
#define McD1F1PciCfg8AndThenOr(Register, AndData, OrData)   MmPci8AndThenOr (0, SA_MC_BUS, 1, 1, Register, AndData, OrData)

///
/// Device 1, Function 2
///
#define McD1F2PciCfg64(Register)                            MmPci64 (0, SA_MC_BUS, 1, 2, Register)
#define McD1F2PciCfg64Or(Register, OrData)                  MmPci64Or (0, SA_MC_BUS, 1, 2, Register, OrData)
#define McD1F2PciCfg64And(Register, AndData)                MmPci64And (0, SA_MC_BUS, 1, 2, Register, AndData)
#define McD1F2PciCfg64AndThenOr(Register, AndData, OrData)  MmPci64AndThenOr (0, SA_MC_BUS, 1, 2, Register, AndData, OrData)

#define McD1F2PciCfg32(Register)                            MmPci32 (0, SA_MC_BUS, 1, 2, Register)
#define McD1F2PciCfg32Or(Register, OrData)                  MmPci32Or (0, SA_MC_BUS, 1, 2, Register, OrData)
#define McD1F2PciCfg32And(Register, AndData)                MmPci32And (0, SA_MC_BUS, 1, 2, Register, AndData)
#define McD1F2PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, SA_MC_BUS, 1, 2, Register, AndData, OrData)

#define McD1F2PciCfg16(Register)                            MmPci16 (0, SA_MC_BUS, 1, 2, Register)
#define McD1F2PciCfg16Or(Register, OrData)                  MmPci16Or (0, SA_MC_BUS, 1, 2, Register, OrData)
#define McD1F2PciCfg16And(Register, AndData)                MmPci16And (0, SA_MC_BUS, 1, 2, Register, AndData)
#define McD1F2PciCfg16AndThenOr(Register, AndData, OrData)  MmPci16AndThenOr (0, SA_MC_BUS, 1, 2, Register, AndData, OrData)

#define McD1F2PciCfg8(Register)                             MmPci8 (0, SA_MC_BUS, 1, 2, Register)
#define McD1F2PciCfg8Or(Register, OrData)                   MmPci8Or (0, SA_MC_BUS, 1, 2, Register, OrData)
#define McD1F2PciCfg8And(Register, AndData)                 MmPci8And (0, SA_MC_BUS, 1, 2, Register, AndData)
#define McD1F2PciCfg8AndThenOr(Register, AndData, OrData)   MmPci8AndThenOr (0, SA_MC_BUS, 1, 2, Register, AndData, OrData)

///
/// Device 2, Function 0
///
#define McD2PciCfg64(Register)                            MmPci64 (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg64Or(Register, OrData)                  MmPci64Or (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg64And(Register, AndData)                MmPci64And (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg64AndThenOr(Register, AndData, OrData)  MmPci64AndThenOr (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

#define McD2PciCfg32(Register)                            MmPci32 (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg32Or(Register, OrData)                  MmPci32Or (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg32And(Register, AndData)                MmPci32And (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

#define McD2PciCfg16(Register)                            MmPci16 (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg16Or(Register, OrData)                  MmPci16Or (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg16And(Register, AndData)                MmPci16And (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg16AndThenOr(Register, AndData, OrData)  MmPci16AndThenOr (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

#define McD2PciCfg8(Register)                             MmPci8 (0, SA_MC_BUS, 2, 0, Register)
#define McD2PciCfg8Or(Register, OrData)                   MmPci8Or (0, SA_MC_BUS, 2, 0, Register, OrData)
#define McD2PciCfg8And(Register, AndData)                 MmPci8And (0, SA_MC_BUS, 2, 0, Register, AndData)
#define McD2PciCfg8AndThenOr(Register, AndData, OrData)   MmPci8AndThenOr (0, SA_MC_BUS, 2, 0, Register, AndData, OrData)

///
/// Device 22, Function 0
///
#define PchD22PciCfg32(Register)                            MmPci32 (0, 0, 22, 0, Register)
#define PchD22PciCfg32Or(Register, OrData)                  MmPci32Or (0, 0, 22, 0, Register, OrData)
#define PchD22PciCfg32And(Register, AndData)                MmPci32And (0, 0, 22, 0, Register, AndData)
#define PchD22PciCfg32AndThenOr(Register, AndData, OrData)  MmPci32AndThenOr (0, 0, 22, 0, Register, AndData, OrData)

///
/// Memory Controller Hub Memory Mapped IO register access
///
#define MCH_REGION_BASE                 (McD0PciCfg64 (R_SA_MCHBAR) &~BIT0)
#define McMmioAddress(Register)         ((UINTN) MCH_REGION_BASE + (UINTN) (Register))

#define McMmio32Ptr(Register)           ((volatile UINT32 *) McMmioAddress (Register))
#define McMmio64Ptr(Register)           ((volatile UINT64 *) McMmioAddress (Register))

#define McMmio64(Register)              *McMmio64Ptr (Register)
#define McMmio64Or(Register, OrData)    (McMmio64 (Register) |= (UINT64) (OrData))
#define McMmio64And(Register, AndData)  (McMmio64 (Register) &= (UINT64) (AndData))
#define McMmio64AndThenOr(Register, AndData, OrData) \
    (McMmio64 (Register) = (McMmio64 (Register) & (UINT64) (AndData)) | (UINT64) (OrData))

#define McMmio32(Register)              *McMmio32Ptr (Register)
#define McMmio32Or(Register, OrData)    (McMmio32 (Register) |= (UINT32) (OrData))
#define McMmio32And(Register, AndData)  (McMmio32 (Register) &= (UINT32) (AndData))
#define McMmio32AndThenOr(Register, AndData, OrData) \
    (McMmio32 (Register) = (McMmio32 (Register) & (UINT32) (AndData)) | (UINT32) (OrData))

#define McMmio16Ptr(Register)           ((volatile UINT16 *) McMmioAddress (Register))
#define McMmio16(Register)              *McMmio16Ptr (Register)
#define McMmio16Or(Register, OrData)    (McMmio16 (Register) |= (UINT16) (OrData))
#define McMmio16And(Register, AndData)  (McMmio16 (Register) &= (UINT16) (AndData))
#define McMmio16AndThenOr(Register, AndData, OrData) \
    (McMmio16 (Register) = (McMmio16 (Register) & (UINT16) (AndData)) | (UINT16) (OrData))

#define McMmio8Ptr(Register)          ((volatile UINT8 *) McMmioAddress (Register))
#define McMmio8(Register)             *McMmio8Ptr (Register)
#define McMmio8Or(Register, OrData)   (McMmio8 (Register) |= (UINT8) (OrData))
#define McMmio8And(Register, AndData) (McMmio8 (Register) &= (UINT8) (AndData))
#define McMmio8AndThenOr(Register, AndData, OrData) \
    (McMmio8 (Register) = (McMmio8 (Register) & (UINT8) (AndData)) | (UINT8) (OrData))

#define MmioR32(Address)        (*(volatile UINT32 *) (UINTN) (Address))
#define MmioW32(Address, Value) (*(volatile UINT32 *) (UINTN) (Address) = (Value))

#endif
