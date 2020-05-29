/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SStateBootMode.h

Abstract:

  S-State Boot Mode PPI as defined in PEI CIS

--*/

#ifndef _PEI_S_STATE_BOOT_MODE_PPI_H
#define _PEI_S_STATE_BOOT_MODE_PPI_H

#define PEI_S_STATE_BOOT_MODE_PEIM_PPI \
  { \
    0x9c44db3d, 0xf434, 0x4b91, 0x8e, 0xb3, 0x30, 0x38, 0xb1, 0x84, 0xab, 0xfc \
  }

extern EFI_GUID gPeiSStateBootModePpiGuid;

#endif
