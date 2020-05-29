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
  BdatAccess.h

@brief
  This file abstracts Bdat Access Handler Protocol.

**/

#ifndef _BDAT_ACCESS_PROTOCOL_H_
#define _BDAT_ACCESS_PROTOCOL_H_

#define EFI_BDAT_ACCESS_GUID \
  { 0xb979746a, 0x8c1f, 0x4a2b, { 0x97, 0xe4, 0x78, 0xe9, 0x3a, 0x71, 0xa7, 0x0a } }

typedef struct _BDAT_ACCESS_PROTOCOL {
  UINTN bdat;
} BDAT_ACCESS_PROTOCOL;

extern EFI_GUID gBdatAccessGuid;

#endif
