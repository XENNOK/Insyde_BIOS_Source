/** @file
 DXE IPMI Package library implement code - Update NVS Variable.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Library/DxeIpmiPackageLib.h>

#include <Library/UefiBootServicesTableLib.h>

#include <Protocol/GlobalNvsArea.h>


/**
 This package function can set variable in GlobalNVS to store BMC status for ACPI method.
 Platform implement this function to set correct variable.

 @param[in]         Ipmi                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval EFI_SUCCESS                    Set variable in GlobalNVS to store BMC status success.
 @return EFI_ERROR (Status)             Locate gEfiGlobalNvsAreaProtocolGuid Protocol error.
*/
EFI_STATUS
IpmiUpdateNvsVariable (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *Ipmi
  )
{
  EFI_STATUS                            Status;
  EFI_GLOBAL_NVS_AREA                   *GlobalNvsArea;
  EFI_GLOBAL_NVS_AREA_PROTOCOL          *GlobalNvsAreaProtocol;

  //
  // Locate EFI_GLOBAL_NVS_AREA Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEfiGlobalNvsAreaProtocolGuid,
                  NULL,
                  (VOID **)&GlobalNvsAreaProtocol
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  GlobalNvsArea = GlobalNvsAreaProtocol->Area;

  if (Ipmi == NULL) {
    GlobalNvsArea->BmcStatus = BMC_STATUS_UNKNOWN;
  } else if (Ipmi != NULL) {
    GlobalNvsArea->BmcStatus = BMC_STATUS_OK;
    GlobalNvsArea->SpecificationRevision = Ipmi->GetIpmiVersion (Ipmi);
    GlobalNvsArea->InterfaceType = Ipmi->GetIpmiInterfaceType (Ipmi);
    GlobalNvsArea->BaseAddress = Ipmi->GetIpmiBaseAddress (Ipmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX);
    GlobalNvsArea->RegisterOffset = (UINT8)Ipmi->GetIpmiBaseAddressOffset (Ipmi, H2O_IPMI_OS_BASE_ADDRESS_INDEX);
  }

  return EFI_SUCCESS;

}

