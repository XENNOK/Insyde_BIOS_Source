//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  LibGlobals.c

Abstract:

  Globals used in EFI SMM Driver Lib. They are initialized in EfiSmmDriverLib.c.
  Each seperatly linked module has it's own copy of these globals.

  gBS       - Boot Services table pointer
  gRT       - Runt Time services table pointer
  gST       - System Table pointer

  gErrorLevel     - Debug error level.

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmStatusCode)

EFI_BOOT_SERVICES             *gBS;
EFI_RUNTIME_SERVICES          *gRT;
EFI_SYSTEM_TABLE              *gST;
EFI_SMM_BASE_PROTOCOL         *gSMM = NULL;
EFI_SMM_STATUS_CODE_PROTOCOL  *mSmmDebug = NULL;
UINTN                         gErrorLevel = EFI_DBUG_MASK | EFI_D_LOAD;
