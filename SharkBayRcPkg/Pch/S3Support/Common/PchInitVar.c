/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchInitVar.c
  
@brief
  This file defines variable shared between PCH Init DXE driver and PCH
  Init S3 Resume PEIM.

**/

//
// Include the protocol header file
//
#include "PchInitVar.h"

//
// Protocol GUID definition
//
EFI_GUID  gPchInitVariableGuid = PCH_INIT_VARIABLE_GUID;
