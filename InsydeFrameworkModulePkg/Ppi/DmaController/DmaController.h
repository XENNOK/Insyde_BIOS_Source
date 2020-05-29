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

  DmaController.h

Abstract:

  DMA Controller PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide DMA services.

--*/

#ifndef _PEI_DMA_CONTROLLER_PPI_H_
#define _PEI_STALL_PPI_H_

#define PEI_DMA_CONTROLLER_PPI_GUID \
  { \
    0x84b107c9, 0xaa8e, 0x4353, 0x86, 0x18, 0xaf, 0x86, 0x30, 0xa8, 0x0f, 0xf8 \
  }

EFI_FORWARD_DECLARATION (PEI_DMA_CONTROLLER_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_INIT_DMA) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_DMA_CONTROLLER_PPI         * This
  );

typedef struct _PEI_DMA_CONTROLLER_PPI {
  PEI_INIT_DMA  InitializeDma;
} PEI_DMA_CONTROLLER_PPI;

extern EFI_GUID gPeiDmaControllerPpiGuid;

#endif
