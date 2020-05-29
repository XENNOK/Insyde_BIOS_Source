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

Copyright (c)  2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

   AsfAddress.c

Abstract:

   GUIDs used for Asf Address variables.

--*/

#include "Tiano.h"

#include EFI_GUID_DEFINITION (AsfAddress)

EFI_GUID gEfiAsfAddressGuid = EFI_ASF_ADDRESS_GUID;

EFI_GUID_STRING (&gEfiAsfAddressGuid, "EFI", "Efi Asf Address GUID")

CHAR16   gEfiAsfAddressName[] = EFI_ASF_ADDRESS_VARIABLE_NAME;

