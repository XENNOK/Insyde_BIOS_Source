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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmControl.h

Abstract:

  Stall PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide SmmControl services.

--*/

#ifndef _PEI_SMM_CONTROL_PPI_H_
#define _PEI_SMM_CONTROL_PPI_H_

#define PEI_SMM_CONTROL_PPI_GUID \
  { \
    0x61c68702, 0x4d7e, 0x4f43, 0x8d, 0xef, 0xa7, 0x43, 0x5, 0xce, 0x74, 0xc5 \
  }

EFI_FORWARD_DECLARATION (PEI_SMM_CONTROL_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_SMM_ACTIVATE) (
  IN EFI_PEI_SERVICES                                **PeiServices,
  IN PEI_SMM_CONTROL_PPI                             * This,
  IN OUT INT8                                        *ArgumentBuffer OPTIONAL,
  IN OUT UINTN                                       *ArgumentBufferSize OPTIONAL,
  IN BOOLEAN                                         Periodic OPTIONAL,
  IN UINTN                                           ActivationInterval OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *PEI_SMM_DEACTIVATE) (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN PEI_SMM_CONTROL_PPI                   * This,
  IN BOOLEAN                               Periodic OPTIONAL
  );

typedef struct _PEI_SMM_CONTROL_PPI {
  PEI_SMM_ACTIVATE    Trigger;
  PEI_SMM_DEACTIVATE  Clear;
} PEI_SMM_CONTROL_PPI;

extern EFI_GUID gPeiSmmControlPpiGuid;

#endif
