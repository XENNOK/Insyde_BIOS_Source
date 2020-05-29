/** @file
  Common Pci Lib will provide a function to get Pci capability ID offset.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>

/**
  Find the Offset to a given Capabilities ID

  CAPID list:

    0x01 = PCI Power Management Interface

    0x04 = Slot Identification

    0x05 = MSI Capability

    0x0D = SSID / SSVID Capability

    0x10 = PCI Express Capability


  @param[in]  Bus                PCI Bus Number
  @param[in]  Device             PCI Device Number
  @param[in]  Function           PCI Function Number
  @param[in]  CapId              CAPID to search for
  @param[in]  CapHeader          Offset of desired CAPID

  @retval EFI_SUCCESS        CAPID found
  @retval EFI_NOT_FOUND      CAPID not found
  @retval EFI_UNSUPPORTED    Capabilities List not supported
  
**/
EFI_STATUS
PciFindCapId (
  IN     UINT8    Bus,
  IN     UINT8    Device,
  IN     UINT8    Function,
  IN     UINT8    CapId,
  OUT    UINT8    *CapHeader
)
{
  UINT16    PciStatusRegister;
  UINT8     PciHeaderType;
  UINT8     CapPtrOffset;
  UINT8     CurrentCapId;

  //
  // Check Capabilities List bit
  //
  PciStatusRegister = PciExpressRead16(PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Function, PCI_PRIMARY_STATUS_OFFSET));

  if ( !( PciStatusRegister & EFI_PCI_STATUS_CAPABILITY ) ) {
    return EFI_UNSUPPORTED;
  }

  //
  // Check the header layout to determine the Offset of Capabilities Pointer Register
  //
  PciHeaderType = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Function, PCI_HEADER_TYPE_OFFSET));
 
  if ( ( PciHeaderType & HEADER_LAYOUT_CODE ) == HEADER_TYPE_CARDBUS_BRIDGE ) {
    //
    // If CardBus bridge, start at Offset 0x14
    //
    CapPtrOffset = EFI_PCI_CARDBUS_BRIDGE_CAPABILITY_PTR;
  } else {
    //
    // Otherwise, start at Offset 0x34
    //
    CapPtrOffset = PCI_CAPBILITY_POINTER_OFFSET;
  }

  *CapHeader = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Function, CapPtrOffset));

  if ( *CapHeader == 0xFF ) {
    return EFI_UNSUPPORTED;
  }

  while ( *CapHeader != 0x00 ) {
    //
    // Each capability must be DWORD aligned.
    //
    // The bottom two bits of all pointers ( including the initial pointer ) are reserved and must be implemented as 00b although software must mask them to allow for future uses of these bits.
    //
    *CapHeader = *CapHeader & ( ~( BIT1 | BIT0 ) );

    CurrentCapId = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Function, *CapHeader));
   
    if ( CurrentCapId == CapId ) {
      return EFI_SUCCESS;
    }

    *CapHeader = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS(Bus, Device, Function, ( *CapHeader + 0x01 )));
   
  }
  *CapHeader = 0;

  return EFI_NOT_FOUND;
}

