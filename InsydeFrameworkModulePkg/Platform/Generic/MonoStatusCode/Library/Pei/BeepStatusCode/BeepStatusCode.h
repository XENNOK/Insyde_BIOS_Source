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

  BeepStatusCode.h

Abstract:

  Lib to provide status code reporting via speaker.

--*/

#ifndef _PEI_BEEP_STATUS_CODE_H_
#define _PEI_BEEP_STATUS_CODE_H_

//
// Statements that include other files
//
#include "Tiano.h"
#include "Pei.h"
#include "SimpleCpuIoLib.h"
#include "EfiCommonLib.h"

//
// Ppi Consumed if present
//
#include EFI_PPI_CONSUMER (Speaker)

EFI_STATUS
PlatformBeepCode (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN  PEI_SPEAKER_IF_PPI      *Speaker,
  IN  EFI_STATUS_CODE_VALUE   Value
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  PeiServices - GC_TODO: add argument description
  Speaker     - GC_TODO: add argument description
  Value       - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
