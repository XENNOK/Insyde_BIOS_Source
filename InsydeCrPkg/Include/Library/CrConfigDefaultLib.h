/** @file
 CrConfig Default library 

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

#ifndef _CRCONFIG_DEFAULT_LIB_H_
#define _CRCONFIG_DEFAULT_LIB_H_

#include <IndustryStandard/Pci.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>
#include <Library/VariableLib.h>

#include <Protocol/TerminalEscCode.h>
#include <Protocol/CRPolicy.h>
#include <Protocol/PciIo.h>
#include <Protocol/SerialIo.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/DevicePath.h>

#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>

EFI_STATUS
CrConfigVarInit (void);


EFI_STATUS
ExtractCrConfigDefault (
  UINT8 *CrConfigDefault
  );


EFI_STATUS
CrExtractVfrDefault (
  UINT8 *NvData,
  UINT8 *VfrDefault
  );

#endif

