/** @file
  GUID definition for the RapidStart Persistent Data

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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
#ifndef _RAPID_START_PERSISTENT_DATA_H_
#define _RAPID_START_PERSISTENT_DATA_H_

///
/// Publish GUID/Name definition for platform code to identify if Rapid Start
/// function is enabled or not. Platform code is able to provide related setup
/// options for further control and configuration by detecting
/// RAPID_START_PERSISTENT_DATA_GUID/ RAPID_START_PERSISTENT_DATA_NAME exists or not
/// in BIOS setup.
///
#define RAPID_START_PERSISTENT_DATA_GUID \
  { \
    0xf9f0b131, 0xf346, 0x4f16, 0x80, 0xdd, 0xf9, 0x41, 0x07, 0x2b, 0x3a, 0x7d \
  }

#define RAPID_START_PERSISTENT_DATA_NAME  L"RapidStartData"

extern EFI_GUID gRapidStartPersistentDataGuid;

#endif
