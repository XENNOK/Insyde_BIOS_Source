/** @file
  Provide OEM to update the Public Key ROM and SLP.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

/**
  This OemService provides OEM to update the Public Key ROM and SLP (System-Locked Preinstallation) Marker ROM. 
  The update info will return to ACPI SLIC table. The service is following the SLP specification 2.0. 

  @param  *PublicKey            If OEM updates the Public Key ROM, this parameter will return the address of ROM.
  @param  *UpdatedPublicKey     If OEM updates the Public Key ROM, this parameter will return true.
  @param  *SlpMarker            If OEM updates the SLP Marker ROM, this parameter will return the address of ROM.
  @param  *UpdatedMarker   	    If OEM updates the SLP Marker ROM, this parameter will return true.

  @retval EFI_SUCCESS           Get Slp2.0 information success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetSlp20PubkeyAndMarkerRomThunk (
  IN  OEM_SERVICES_PROTOCOL                *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;

  EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE     *PublicKey        = NULL;
  BOOLEAN                               *UpdatedPublickey = NULL;
  EFI_ACPI_SLP_MARKER_STRUCTURE         *SlpMarker        = NULL;
  BOOLEAN                               *UpdatedMarker    = NULL;
  EFI_STATUS                            Status;
  

  VA_START (Marker, NumOfArgs);

  PublicKey        = VA_ARG (Marker, EFI_ACPI_OEM_PUBLIC_KEY_STRUCTURE*);
  UpdatedPublickey = VA_ARG (Marker, BOOLEAN*);
  SlpMarker        = VA_ARG (Marker, EFI_ACPI_SLP_MARKER_STRUCTURE*);
  UpdatedMarker    = VA_ARG (Marker, BOOLEAN*);
  
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcGetSlp20PubkeyAndMarkerRom (
             PublicKey,
             UpdatedPublickey,
             SlpMarker,
             UpdatedMarker
             );

  return Status;
}
