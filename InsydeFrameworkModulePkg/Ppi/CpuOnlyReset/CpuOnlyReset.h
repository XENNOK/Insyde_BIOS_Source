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

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  CpuOnlyReset.h

Abstract:

  Provides a common interface to perform a CPU Only reset in PEI.

--*/

#ifndef _PEI_CPU_ONLY_RESET_H_
#define _PEI_CPU_ONLY_RESET_H_

#define PEI_CPU_ONLY_RESET_PPI_GUID  \
  {0xf24d2f3b, 0x676e, 0x421e, 0x90, 0x6f, 0xf0, 0x5d, 0xf1, 0x46, 0xbe, 0x1a}

//
// Base function
//
typedef
EFI_STATUS
(EFIAPI *PEI_CPU_ONLY_RESET) (
  IN  EFI_PEI_SERVICES    **PeiServices
  );

typedef struct {
  PEI_CPU_ONLY_RESET  CpuOnlyReset;
} PEI_CPU_ONLY_RESET_PPI;

extern EFI_GUID   gPeiCpuOnlyResetPpiGuid;

#endif
