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

  AtaPolicy.h

Abstract:

  ATA Policy PPI as defined in EFI 2.0

--*/

#ifndef _PEI_ATA_POLICY_PPI_H
#define _PEI_ATA_POLICY_PPI_H

#define PEI_ATA_POLICY_PPI_GUID \
  { \
    0x1b8ddea4, 0xdeb2, 0x4152, 0x91, 0xc2, 0xb7, 0x3c, 0xb1, 0x6c, 0xe4, 0x64 \
  }

EFI_FORWARD_DECLARATION (PEI_ATA_POLICY_PPI);

typedef struct _PEI_ATA_POLICY_PPI {
  UINTN BaseAddress;
} PEI_ATA_POLICY_PPI;

extern EFI_GUID gPeiAtaPolicyPpiGuid;

#endif
