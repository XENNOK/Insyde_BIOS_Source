/** @file
 Header file for Setup utility application.

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

#ifndef _SETUP_UTILITY_APPLICATION_H_
#define _SETUP_UTILITY_APPLICATION_H_

#include <Uefi.h>

#include <Protocol/HiiDatabase.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/SetupUtilityApplication.h>
#include <Protocol/SetupUtility.h>

#include <Guid/MdeModuleHii.h>

#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DxeOemSvcKernelLib.h>
#include <Library/VariableLib.h>

#include <KernelSetupConfig.h>

#endif

