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

Copyright (c)  1999 - 2001 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MemoryConfigData.c

Abstract:

  GUID used for Memory Configuration Data entries in the HOB list.

--*/

#include "Tiano.h"
#include EFI_GUID_DEFINITION (MemoryConfigData)

EFI_GUID  gEfiMemoryConfigDataGuid  = EFI_MEMORY_CONFIG_DATA_GUID;
CHAR16    EfiMemoryConfigVariable[] = L"MemoryConfig";

EFI_GUID_STRING (&gEfiMemoryConfigDataGuid, "Memory Config Data", "Memory Config Data HOB GUID for HOB list.");
