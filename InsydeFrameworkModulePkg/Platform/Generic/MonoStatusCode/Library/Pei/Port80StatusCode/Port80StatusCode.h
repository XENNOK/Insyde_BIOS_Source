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
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
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

  Port80StatusCode.h

Abstract:

  Lib to provide status code reporting via port 80.

--*/

#ifndef _PEI_PORT_80_STATUS_CODE_H_
#define _PEI_PORT_80_STATUS_CODE_H_

//
// Statements that include other files
//
#include "Tiano.h"
#include "Pei.h"
#include "SimpleCpuIoLib.h"
#include "EfiCommonLib.h"
#include "PostCode.h"

extern BOOLEAN
EFIAPI
StatusCodeToPostCode (
  IN  EFI_STATUS_CODE_TYPE    CodeType,
  IN  EFI_STATUS_CODE_VALUE   Value,
  OUT UINT8                   *PostCode
  );

#endif
