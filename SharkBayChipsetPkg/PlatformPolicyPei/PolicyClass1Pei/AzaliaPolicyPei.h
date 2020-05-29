/** @file
  
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

#ifndef _AZALIA_POLICY_PEI_H_
#define _AZALIA_POLICY_PEI_H_

#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiOemSvcChipsetLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/AzaliaPolicy.h>
#include <ChipsetSetupConfig.h>

VOID
DumpAzaliaPolicyPEI (
  IN      CONST EFI_PEI_SERVICES        **PeiServices,
  IN      AZALIA_POLICY                 *SAPlatformPolicyPpi
  );

#endif
