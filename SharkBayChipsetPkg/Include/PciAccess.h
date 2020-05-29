/** @file
  Provide memory map pci access macro.

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

#ifndef _PciAccess_h_INCLUDED_
#define _PciAccess_h_INCLUDED_

#define PCI_EXPRESS_BASE          (UINTN)PcdGet64(PcdPciExpressBaseAddress)

#define PCI_EXPRESS_BASE_ADDRESS  ((VOID *)(UINTN)PCI_EXPRESS_BASE)

#ifndef MmPciAddress
#define MmPciAddress(Segment, Bus, Device, Function, Register) \
    ( \
      (UINTN) PCI_EXPRESS_BASE_ADDRESS + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) (Function << 12) + \
        (UINTN) (Register) \
    )
#endif

//
// UINT32
//
#define MmPci32Ptr(Segment, Bus, Device, Function, Register) \
    ((volatile UINT32 *) MmPciAddress (Segment, Bus, Device, Function, Register))

#define MmPci32(Segment, Bus, Device, Function, Register) *MmPci32Ptr (Segment, Bus, Device, Function, Register)

#define MmPci32Or(Segment, Bus, Device, Function, Register, OrData) \
  MmPci32 (Segment, Bus, Device, Function, Register) = \
    (UINT32) (MmPci32 (Segment, Bus, Device, Function, Register) | (UINT32) (OrData))

#define MmPci32And(Segment, Bus, Device, Function, Register, AndData) \
  MmPci32 (Segment, \
           Bus, \
           Device, \
           Function, \
           Register \
      ) = (UINT32) (MmPci32 (Segment, Bus, Device, Function, Register) & (UINT32) (AndData))

#define MmPci32AndThenOr(Segment, Bus, Device, Function, Register, AndData, OrData) \
  MmPci32 (Segment, \
           Bus, \
           Device, \
           Function, \
           Register \
      ) = (UINT32) ((MmPci32 (Segment, Bus, Device, Function, Register) & (UINT32) (AndData)) | (UINT32) (OrData))

//
// UINT16
//
#define MmPci16Ptr(Segment, Bus, Device, Function, Register) \
    ((volatile UINT16 *) MmPciAddress (Segment, Bus, Device, Function, Register))

#define MmPci16(Segment, Bus, Device, Function, Register) *MmPci16Ptr (Segment, Bus, Device, Function, Register)

#define MmPci16Or(Segment, Bus, Device, Function, Register, OrData) \
  MmPci16 (Segment, Bus, Device, Function, Register) = \
    (UINT16) (MmPci16 (Segment, Bus, Device, Function, Register) | (UINT16) (OrData))

#define MmPci16And(Segment, Bus, Device, Function, Register, AndData) \
  MmPci16 (Segment, \
           Bus, \
           Device, \
           Function, \
           Register \
      ) = (UINT16) (MmPci16 (Segment, Bus, Device, Function, Register) & (UINT16) (AndData))

#define MmPci16AndThenOr(Segment, Bus, Device, Function, Register, AndData, OrData) \
  MmPci16 (Segment, \
           Bus, \
           Device, \
           Function, \
           Register \
      ) = (UINT16) ((MmPci16 (Segment, Bus, Device, Function, Register) & (UINT16) (AndData)) | (UINT16) (OrData))

//
// UINT8
//
#define MmPci8Ptr(Segment, Bus, Device, Function, Register) \
    ((volatile UINT8 *) MmPciAddress (Segment, Bus, Device, Function, Register))

#define MmPci8(Segment, Bus, Device, Function, Register)  *MmPci8Ptr (Segment, Bus, Device, Function, Register)

#define MmPci8Or(Segment, Bus, Device, Function, Register, OrData) \
  MmPci8 (Segment, Bus, Device, Function, Register) = \
    (UINT8) (MmPci8 (Segment, Bus, Device, Function, Register) | (UINT8) (OrData))

#define MmPci8And(Segment, Bus, Device, Function, Register, AndData) \
  MmPci8 (Segment, Bus, Device, Function, Register) = \
    (UINT8) (MmPci8 (Segment, Bus, Device, Function, Register) & (UINT8) (AndData))

#define MmPci8AndThenOr(Segment, Bus, Device, Function, Register, AndData, OrData) \
  MmPci8 (Segment, \
          Bus, \
          Device, \
          Function, \
          Register \
      ) = (UINT8) ((MmPci8 (Segment, Bus, Device, Function, Register) & (UINT8) (AndData)) | (UINT8) (OrData))

#endif
