/** @file



;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_POLICY_PEI_H_
#define _SPS_POLICY_PEI_H_

#include <Library/PeimEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>

#include <Guid/SpsPolicyHob.h>
#include <Ppi/ReadOnlyVariable2.h>

#include <Ppi/SpsPolicy.h>

#include <SetupConfig.h>

#include <PchRegs/PchRegsLpc.h>
#include <Library/PchPlatformLib/PchPlatformLibrary.h>

#endif
