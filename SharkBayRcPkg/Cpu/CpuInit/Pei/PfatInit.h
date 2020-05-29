/** @file

  Describes the functions visible to the rest of the PfatInit.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
    This software and associated documentation (if any) is furnished
    under a license and may only be used or copied in accordance
    with the terms of the license. Except as permitted by such
    license, no part of this software or documentation may be
    reproduced, stored in a retrieval system, or transmitted in any
    form or by any means without the express written consent of
    Intel Corporation.

*/

#ifndef _PFAT_INIT_H_
#define _PFAT_INIT_H_

#include <PfatDefinitions.h>
#include <Ppi/CpuPlatformPolicy.h>

///
/// Function Prototypes
///
/**
  Execute Early-Post initialization of PFAT specific MSRs

  @param[in] PeiServices          - Indirect reference to the PEI Services Table.
  @param[in] CpuPlatformPolicyPpi - Platform Policy PPI
**/
VOID
PfatInit (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN PEI_CPU_PLATFORM_POLICY_PPI *CpuPlatformPolicyPpi
  );

#endif
