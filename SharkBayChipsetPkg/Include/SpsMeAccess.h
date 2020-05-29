/** @file

  Macros to simplify and abstract the interface to PCI configuration.

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

#ifndef _SPS_ME_ACCESS_H_
#define _SPS_ME_ACCESS_H_

#include <SpsMeChipset.h>
#include <Library/PciLib.h>

///
/// HECI PCI Access Macro
///
#define SpsHeciPciRead32(Register) PciRead32 (PCI_LIB_ADDRESS (SPS_ME_BUS, SPS_ME_DEVICE_NUMBER, SPS_HECI_FUNCTION_NUMBER, Register))

#define SpsHeciPciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define SpsHeciPciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define SpsHeciPciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define SpsHeciPciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define SpsHeciPciRead16(Register) PciRead16 (PCI_LIB_ADDRESS (SPS_ME_BUS, SPS_ME_DEVICE_NUMBER, SPS_HECI_FUNCTION_NUMBER, Register))

#define SpsHeciPciWrite16(Register, Data) \
  PciWrite16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define SpsHeciPciOr16(Register, Data) \
  PciOr16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define SpsHeciPciAnd16(Register, Data) \
  PciAnd16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define SpsHeciPciAndThenOr16(Register, AndData, OrData) \
  PciAndThenOr16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define SpsHeciPciRead8(Register)  PciRead8 (PCI_LIB_ADDRESS (SPS_ME_BUS, SPS_ME_DEVICE_NUMBER, SPS_HECI_FUNCTION_NUMBER, Register))

#define SpsHeciPciWrite8(Register, Data) \
  PciWrite8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define SpsHeciPciOr8(Register, Data) \
  PciOr8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define SpsHeciPciAnd8(Register, Data) \
  PciAnd8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define SpswHeciPciAndThenOr8(Register, AndData, OrData) \
  PciAndThenOr8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI_FUNCTION_NUMBER, \
  Register), \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

///
/// HECI2 PCI Access Macro
///
#define SpsHeci2PciRead32(Register)  PciRead32 (PCI_LIB_ADDRESS (SPS_ME_BUS, SPS_ME_DEVICE_NUMBER, SPS_HECI2_FUNCTION_NUMBER, Register))

#define SpsHeci2PciWrite32(Register, Data) \
  PciWrite32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define SpsHeci2PciOr32(Register, Data) \
  PciOr32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define SpsHeci2PciAnd32(Register, Data) \
  PciAnd32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) Data \
  )

#define SpsHeci2PciAndThenOr32(Register, AndData, OrData) \
  PciAndThenOr32 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT32) AndData, \
  (UINT32) OrData \
  )

#define SpsHeci2PciRead16(Register)  PciRead16 (PCI_LIB_ADDRESS (SPS_ME_BUS, SPS_ME_DEVICE_NUMBER, SPS_HECI2_FUNCTION_NUMBER, Register))

#define SpsHeci2PciWrite16(Register, Data) \
  PciWrite16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define SpsHeci2PciOr16(Register, Data) \
  PciOr16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define SpsHeci2PciAnd16(Register, Data) \
  PciAnd16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) Data \
  )

#define SpsHeci2PciAndThenOr16(Register, AndData, OrData) \
  PciAndThenOr16 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT16) AndData, \
  (UINT16) OrData \
  )

#define SpsHeci2PciRead8(Register) PciRead8 (PCI_LIB_ADDRESS (SPS_ME_BUS, SPS_ME_DEVICE_NUMBER, SPS_HECI2_FUNCTION_NUMBER, Register))

#define SpsHeci2PciWrite8(Register, Data) \
  PciWrite8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define SpsHeci2PciOr8(Register, Data) \
  PciOr8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define SpsHeci2PciAnd8(Register, Data) \
  PciAnd8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) Data \
  )

#define SpsHeci2PciAndThenOr8(Register, AndData, OrData) \
  PciAndThenOr8 ( \
  PCI_LIB_ADDRESS (SPS_ME_BUS, \
  SPS_ME_DEVICE_NUMBER, \
  SPS_HECI2_FUNCTION_NUMBER, \
  Register), \
  (UINT8) AndData, \
  (UINT8) OrData \
  )

#endif
