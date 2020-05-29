/** @file
  This OemService provides OEM to get the PCI-e ASPM (Active State Power Management) table 
  that describes the status (L0, L1, etc¡K) of PCI-e ASPM devices.

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

#include <Library/SmmOemSvcKernelLib.h>

/**
  This OemService provides OEM to get the PCI-e ASPM (Active State Power Management) table 
  that describes the status (L0, L1, etc¡K) of PCI-e ASPM devices.
  
  @param[out]  *PciAspmDevs          Point to PCIE_ASPM_DEV_INFO table.

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           The service is customized in the project.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Depends on customization.
**/
EFI_STATUS
OemSvcGetAspmOverrideInfo (
  OUT PCIE_ASPM_DEV_INFO                    **PcieAspmDevs
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
