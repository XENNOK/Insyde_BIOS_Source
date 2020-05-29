/** @file
  Provides the driver IsaAcpiDriver.inf to get the resource of multiple SuperIO controllers.

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
  This OemService provides the driver IsaAcpiDriver.inf to get the resource of multiple SuperIO controllers. 
  The driver supports initialize and resource assignation on single or multiple super I/O chips on the platform. 
  OEM modifies the ISA controllers table based on layout of platform. The detail refers to the document "MultiSuperIO".  

  @param  *MSIOTableCount       Point to the instances of the ISA controllers table.
  @param  **IsaAcpiDeviceList   Point to the ISA controllers table. 
  
  @retval EFI_SUCCESS           Get MultiSuperIo info success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcInstallMultiSuperIoTableThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{ 
  return EFI_UNSUPPORTED;
}
