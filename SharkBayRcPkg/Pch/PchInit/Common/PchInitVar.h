/** @file
  This file defines variable shared between PCH Init DXE driver and PCH
  Init S3 Resume PEIM.
  
@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/


#ifndef _PCH_INIT_VAR_H_
#define _PCH_INIT_VAR_H_


///
/// Define the PCH Init Var GUID
///
///
/// EDK and EDKII have different GUID formats
///

#define PCH_INIT_PEI_VARIABLE_GUID \
  { \
    0xa31b27a4, 0xcae6, 0x48ff, \
    { \
      0x8c, 0x5a, 0x29, 0x42, 0x21, 0xe6, 0xf3, 0x89 \
    } \
  }
///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gPchInitPeiVariableGuid;

#define PCH_INIT_PEI_VARIABLE_NAME  L"PchInitPei"

///
/// Define the Pch Init Variable structure
///

typedef struct _PCH_LATE_INIT_SMM_VARIABLE {
  UINT16  IoTrapAddress;
  UINT32  PciMemBase;
} PCH_LATE_INIT_SMM_VARIABLE;

#endif
