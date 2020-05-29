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

  Metronome.h

Abstract:

  Metronome PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide delay services.

--*/

#ifndef _PEI_METRONOME_PPI_H_
#define _PEI_METRONOME_PPI_H_

#define PEI_METRONOME_PPI_GUID \
  { \
    0x24cb298c, 0x578a, 0x4493, 0xaf, 0x2b, 0x5d, 0x8b, 0x44, 0xc9, 0x90, 0x5a \
  }

EFI_FORWARD_DECLARATION (PEI_METRONOME_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_METRONOME_WAIT_FOR_NEXT_TICK) (
  IN EFI_PEI_SERVICES            **PeiServices,
  IN PEI_METRONOME_PPI           * This,
  IN UINTN                       TickNumber
  );

typedef struct _PEI_METRONOME_PPI {
  UINT32                            TickPeriod;
  PEI_METRONOME_WAIT_FOR_NEXT_TICK  WaitForNextTick;
} PEI_METRONOME_PPI;

extern EFI_GUID gPeiMetronomePpiGuid;

#endif
