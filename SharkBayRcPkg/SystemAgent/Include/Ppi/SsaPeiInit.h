/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2012 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SsaPeiInit.h

@brief
  Interface definition between MRC and SsaInitPeim driver.
**/
#ifndef _SSA_PEI_INIT_H_
#define _SSA_PEI_INIT_H_

///
/// Define the SSA PEI Init PPI GUID
///
#define SSA_BIOS_CALLBACKS_PPI_GUID {0x99B56126, 0xE16C, 0x4D9B, 0xBB, 0x71, 0xAA, 0x35, 0x46, 0x1A, 0x70, 0x2F}
#define SSA_BIOS_SERVICES_PPI_GUID  {0x55750D10, 0x6D3D, 0x4BF5, 0x89, 0xD8, 0xE3, 0x5E, 0xF0, 0xB0, 0x90, 0xF4}

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gSsaBiosCallBacksPpiGuid;
extern EFI_GUID gSsaBiosServicesPpiGuid;

#endif
