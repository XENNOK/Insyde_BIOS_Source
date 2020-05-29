//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
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

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

   HiiDriverHealthFormset.c

Abstract:

   GUIDs used for HII Driver Health Formset.

--*/

#include "Tiano.h"
#include EFI_GUID_DEFINITION (HiiDriverHealthFormset)

EFI_GUID  gEfiHiiDriverHealthFormsetGuid = EFI_HII_DRIVER_HEALTH_FORMSET_GUID;

EFI_GUID_STRING (&gEfiHiiDriverHealthFormsetGuid, "HII Driver Health Formset", "HII Driver Health Formset GUID");
