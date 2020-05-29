/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  TxtMemoryUnlocked.h

@brief
  This file contains PPI definition that is installed
  by TXT PEIM

**/
#ifndef _PEI_TXT_MEMORY_UNLOCKED_PPI_H_
#define _PEI_TXT_MEMORY_UNLOCKED_PPI_H_

#define PEI_TXT_MEMORY_UNLOCKED_PPI_GUID \
  { 0x38cdd10b, 0x767d, 0x4f6e, { 0xa7, 0x44, 0x67, 0xee, 0x1d, 0xfe, 0x2f, 0xa5 } }

extern EFI_GUID gPeiTxtMemoryUnlockedPpiGuid;

#endif
