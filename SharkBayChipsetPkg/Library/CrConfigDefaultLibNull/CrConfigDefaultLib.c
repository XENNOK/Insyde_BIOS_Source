/** @file
  Use for CrConfig variable default setting

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <IndustryStandard/Pci.h>
#include <FrameworkDxe.h>
#include <CrSetupConfig.h>

#include <Guid/GlobalVariable.h>
#include <Guid/MdeModuleHii.h>

#include <Library/PrintLib.h>
#include <Library/CrVfrConfigLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/VariableLib.h>
#include <Library/CrBdsLib.h>
#include <Library/UefiHiiServicesLib.h>

#include <Protocol/PciIo.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/CRPolicy.h>
#include <Protocol/DevicePath.h>
#include <Protocol/UsbSerialControllerIo.h>


extern UINT8  AdvanceVfrCrConfigDefault0000[];


EFI_STATUS
CrExtractVfrDefault (
  UINT8 *NvData,
  UINT8 *VfrDefault
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
ExtractCrConfigDefault (
  UINT8 *CrConfigDefault
  )
{
  return EFI_UNSUPPORTED; 
}


EFI_STATUS
CrConfigVarInit (void)
{
  return EFI_UNSUPPORTED;
}


