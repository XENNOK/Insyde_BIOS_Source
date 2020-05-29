/** @file
  Provides an opportunity for OEM to update the Public Key ROM and SLP.

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

#include <Library/DxeOemSvcKernelLib.h>

/**
  This OemService provides OEM to update the Public Key ROM and SLP (System-Locked Preinstallation) Marker ROM. 
  The update info will return to ACPI SLIC table. The service is following the SLP specification 2.0. 

  @param[in, out]  *PublicKey            If OEM updates the Public Key ROM, this parameter will return the address of ROM.
  @param[in, out]  *UpdatedPublicKey     If OEM updates the Public Key ROM, this parameter will return true.
  @param[in, out]  *SlpMarker            If OEM updates the SLP Marker ROM, this parameter will return the address of ROM.
  @param[in, out]  *UpdatedMarker   	 If OEM updates the SLP Marker ROM, this parameter will return true.

  @retval          EFI_UNSUPPORTED       Returns unsupported by default.
  @retval          EFI_SUCCESS           Get Slp2.0 information success.
  @retval          EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval          Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetSlp20PubkeyAndMarkerRom (
  IN OUT EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE     *PublicKey,
  IN OUT BOOLEAN                               *UpdatedPublickey,
  IN OUT EFI_ACPI_SLP_MARKER_STRUCTURE         *SlpMarker,
  IN OUT BOOLEAN                               *UpdatedMarker
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  
  return EFI_UNSUPPORTED;
}
