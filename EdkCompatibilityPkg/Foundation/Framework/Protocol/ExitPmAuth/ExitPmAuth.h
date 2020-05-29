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

Copyright (c)  2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  ExitPmAuth.h

Abstract:


--*/

#ifndef _EXIT_PM_AUTH_PROTOCOL_H_
#define _EXIT_PM_AUTH_PROTOCOL_H_

#include "Tiano.h"

#define EXIT_PM_AUTH_PROTOCOL_GUID \
  { \
    0xd088a413, 0xa70, 0x4217, 0xba, 0x55, 0x9a, 0x3c, 0xb6, 0x5c, 0x41, 0xb3 \
  }

extern EFI_GUID gExitPmAuthProtocolGuid;

#endif
