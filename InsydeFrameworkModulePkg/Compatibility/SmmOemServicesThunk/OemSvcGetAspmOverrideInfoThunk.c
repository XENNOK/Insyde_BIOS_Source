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

#include "SmmOemServicesThunk.h"
#include <Library/SmmOemSvcKernelLib.h>

/**
  This OemService provides OEM to get the PCI-e ASPM (Active State Power Management) table 
  that describes the status (L0, L1, etc¡K) of PCI-e ASPM devices.
  
  @param  *PciAspmDevs          Point to PCIE_ASPM_DEV_INFO table.

  @retval EFI_SUCCESS           Always returns success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcGetAspmOverrideInfoThunk (
  IN  SMM_OEM_SERVICES_PROTOCOL          *This,
  IN  UINTN                              NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;  
  PCIE_ASPM_DEV_INFO                    **PcieAspmDevs;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  PcieAspmDevs = VA_ARG (Marker, PCIE_ASPM_DEV_INFO **);
  VA_END (Marker);

  //
  // make a call to SmmOemSvcKernelLib internally
  //
  Status = OemSvcGetAspmOverrideInfo (
             PcieAspmDevs
             );

  return Status;
}
