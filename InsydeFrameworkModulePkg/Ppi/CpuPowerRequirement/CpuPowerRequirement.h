//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c) 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CpuPowerRequirement.h

Abstract:

  Provides a common interface to judge power requirement in PEI.

--*/

#ifndef _PEI_CPU_POWER_REQUIREMENT_H_
#define _PEI_CPU_POWER_REQUIREMENT_H_

#define PEI_CPU_POWER_REQUIREMENT_PPI_GUID  \
  {0x4a129e3d, 0x8cc8, 0x443e, 0xbf, 0xa2, 0x6f, 0xf4, 0xfa, 0x80, 0x69, 0x4a}

//
// Base function
//
typedef
BOOLEAN
(EFIAPI *PEI_FREQUENCY_SELECTION_CPU_PPI_POWER_REQUIREMENT) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  BOOLEAN                           PowerRequirementMeet
  );

typedef struct {
  PEI_FREQUENCY_SELECTION_CPU_PPI_POWER_REQUIREMENT CpuPowerRequirement;
} PEI_CPU_POWER_REQUIREMENT_PPI;

extern EFI_GUID   gPeiCpuPowerRequirementPpiGuid;

#endif
