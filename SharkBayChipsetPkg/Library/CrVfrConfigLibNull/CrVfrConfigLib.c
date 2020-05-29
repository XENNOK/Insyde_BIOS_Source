/** @file
  This Library will install CRVfrConfigLib for reference.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

//#include "CrVfrConfigLib.h"

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

EFI_STATUS
CrSerialDevOptionInit (
  IN EFI_HII_HANDLE               HiiHandle
  )
{  
  return EFI_UNSUPPORTED;
}

EFI_STATUS
CRScuAdvanceCallback (
  IN  EFI_QUESTION_ID             QuestionId
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
CRScuHotKeyCallback (
  IN  EFI_QUESTION_ID             QuestionId,
  IN  EFI_INPUT_KEY               Key
  ) 
{
  return EFI_UNSUPPORTED;
}


EFI_STATUS
CRBdsScuInit (
  EFI_CR_POLICY_PROTOCOL    *CRPolicy
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
CRConfigExtractCallBack (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
CRConfigRouteCallBack (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Configuration,
  OUT      EFI_STRING                       *Progress
  )
{
  return EFI_UNSUPPORTED;
}

