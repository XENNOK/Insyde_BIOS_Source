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

  WarmResetDetected.h

Abstract:

  Dummy PPI to signal that a warm reset was detected.

--*/

#ifndef _PEI_WARM_RESET_DETECTED_H_
#define _PEI_WARM_RESET_DETECTED_H_

#define PEI_WARM_RESET_DETECTED_GUID \
  { \
    0xf860c9b5, 0x41cb, 0x4e25, 0x84, 0x9a, 0x31, 0x50, 0x75, 0xa0, 0xe8, 0xf2 \
  }

extern EFI_GUID gPeiWarmResetDetectedGuid;

#endif
