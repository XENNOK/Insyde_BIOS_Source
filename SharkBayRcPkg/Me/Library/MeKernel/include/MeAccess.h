/** @file

  Macros to simplify and abstract the interface to PCI configuration.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _ME_ACCESS_H_
#define _ME_ACCESS_H_

#include "MeChipset.h"
#include "PchAccess.h"
#include <Library/PciLib.h>
///
/// HECI PCI Access Macro
///
#define HeciPciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#define HeciPciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define HeciPciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define HeciPciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define HeciPciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define HeciPciRead16(Register) PciRead16 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#define HeciPciWrite16(Register, Data) \
  PciWrite16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define HeciPciOr16(Register, Data) \
  PciOr16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define HeciPciAnd16(Register, Data) \
  PciAnd16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define HeciPciAndThenOr16(Register, AndData, OrData) \
  PciAndThenOr16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define HeciPciRead8(Register)  PciRead8 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, Register))

#define HeciPciWrite8(Register, Data) \
  PciWrite8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define HeciPciOr8(Register, Data) \
  PciOr8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define HeciPciAnd8(Register, Data) \
  PciAnd8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define HeciPciAndThenOr8(Register, AndData, OrData) \
  PciAndThenOr8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

///
/// HECI2 PCI Access Macro
///
#define Heci2PciRead32(Register)  PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, Register))

#define Heci2PciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci2PciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci2PciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define Heci2PciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define Heci2PciRead16(Register)  PciRead16 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, Register))

#define Heci2PciWrite16(Register, Data) \
  PciWrite16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define Heci2PciOr16(Register, Data) \
  PciOr16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define Heci2PciAnd16(Register, Data) \
  PciAnd16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define Heci2PciAndThenOr16(Register, AndData, OrData) \
  PciAndThenOr16 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define Heci2PciRead8(Register) PciRead8 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI2_FUNCTION_NUMBER, Register))

#define Heci2PciWrite8(Register, Data) \
  PciWrite8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define Heci2PciOr8(Register, Data) \
  PciOr8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define Heci2PciAnd8(Register, Data) \
  PciAnd8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define Heci2PciAndThenOr8(Register, AndData, OrData) \
  PciAndThenOr8 ( \
  PCI_LIB_ADDRESS (ME_BUS, \
  ME_DEVICE_NUMBER, \
  HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

#ifndef MmPciAddress
#define MmPciAddress(Segment, Bus, Device, Function, Register) \
    ( \
      (UINTN) (PciRead32 (PCI_LIB_ADDRESS (0,0,0,0x60)) & 0xFC000000) + \
      (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) \
      (Function << 12) + (UINTN) (Register) \
    )
#endif
///
/// ME Macro Setting
///
#define McD0PciCfg64(Register)              MmPci64 (0, SA_MC_BUS, 0, 0, Register)
#define McD0PciCfg32And(Register, AndData)  PciAnd32 (PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, Register), (UINT32) AndData)
#define McD0PciCfg16And(Register, AndData)  PciAnd16 (PCI_LIB_ADDRESS (SA_MC_BUS, 0, 0, Register), (UINT16) AndData)

///
///  MMIO access macros
///
#define MmioAddress(BaseAddr, Register) ((UINTN) BaseAddr + (UINTN) (Register))

///
/// 16 bit MMIO access
///
#define Mmio16Ptr(BaseAddr, Register) ((volatile UINT16 *) MmioAddress (BaseAddr, Register))

#define Mmio16(BaseAddr, Register)    *Mmio16Ptr (BaseAddr, Register)

#define Mmio16Or(BaseAddr, Register, OrData) \
  Mmio16 (BaseAddr, Register) = (UINT16) \
    (Mmio16 (BaseAddr, Register) | (UINT16) (OrData))

#define Mmio16And(BaseAddr, Register, AndData) \
  Mmio16 (BaseAddr, Register) = (UINT16) \
    (Mmio16 (BaseAddr, Register) & (UINT16) (AndData))

#define Mmio16AndThenOr(BaseAddr, Register, AndData, OrData) \
  Mmio16 (BaseAddr, Register) = (UINT16) \
    ((Mmio16 (BaseAddr, Register) & (UINT16) (AndData)) | (UINT16) (OrData))

///
/// 32 bit MMIO access
///
#define Mmio32Ptr(BaseAddr, Register) ((volatile UINT32 *) MmioAddress (BaseAddr, Register))

#define Mmio32(BaseAddr, Register)    *Mmio32Ptr (BaseAddr, Register)

#define Mmio32Or(BaseAddr, Register, OrData) \
  Mmio32 (BaseAddr, Register) = (UINT32) \
    (Mmio32 (BaseAddr, Register) | (UINT32) (OrData))

#define Mmio32And(BaseAddr, Register, AndData) \
  Mmio32 (BaseAddr, Register) = (UINT32) \
    (Mmio32 (BaseAddr, Register) & (UINT32) (AndData))

#define Mmio32AndThenOr(BaseAddr, Register, AndData, OrData) \
  Mmio32 (BaseAddr, Register) = (UINT32) \
    ((Mmio32 (BaseAddr, Register) & (UINT32) (AndData)) | (UINT32) (OrData))

///
/// Memory Controller PCI access macros
///
#define MCH_REGION_BASE               (McD0PciCfg64 (MC_MCHBAR_OFFSET) &~BIT0)
#define McMmioAddress(Register)       ((UINTN) MCH_REGION_BASE + (UINTN) (Register))

#define McMmio8Ptr(Register)          ((volatile UINT8 *) McMmioAddress (Register))
#define McMmio8(Register)             *McMmio8Ptr (Register)
#define McMmio8Or(Register, OrData)   (McMmio8 (Register) |= (UINT8) (OrData))
#define McMmio8And(Register, AndData) (McMmio8 (Register) &= (UINT8) (AndData))
#define McMmio8AndThenOr(Register, AndData, OrData) \
    (McMmio8 (Register) = (McMmio8 (Register) & (UINT8) (AndData)) | (UINT8) (OrData))

#endif
