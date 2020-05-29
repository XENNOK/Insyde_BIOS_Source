//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Module Name:

  AhciController.c

Abstract:

  AHCI Controller PPI GUID

--*/

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (AhciController)

EFI_GUID  gPeiAhciControllerPpiGuid = PEI_AHCI_CONTROLLER_PPI_GUID;

EFI_GUID_STRING (&gPeiAhciControllerPpiGuid, "AhciController", "AHCI Controller PPI");
