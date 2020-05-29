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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
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

    Smm.c

Abstract:

  GUIDs used for SMM drivers

--*/

#include "Tiano.h"
#include EFI_GUID_DEFINITION (Smm)

EFI_GUID  gEfiSmmStartImageGuid = EFI_SMM_START_IMAGE_GUID;

EFI_GUID_STRING (&gEfiSmmStartImageGuid, "Guid for SMM start image", "Guid for SMM start image");

EFI_GUID  gEfiSmmCoreImageGuid = EFI_SMM_CORE_IMAGE_GUID;

EFI_GUID_STRING (&gEfiSmmCoreImageGuid, "Guid for SMM core image", "Guid for SMM core image");

EFI_GUID  gEfiSmmCoreDispatcherGuid = EFI_SMM_CORE_DISPATCHER_GUID;

EFI_GUID_STRING (&gEfiSmmCoreDispatcherGuid, "Guid for SMM core dispatcher image", "Guid for SMM core dispatcher image");

EFI_GUID  gEfiSmmApImageGuid = EFI_SMM_AP_IMAGE_GUID;

EFI_GUID_STRING (&gEfiSmmApImageGuid, "Guid for SMM AP handler image", "Guid for SMM AP handler image");
