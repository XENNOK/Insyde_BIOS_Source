/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  SaBuildFlags.h

@brief
  Defines SA build flags

**/

#ifndef _SA_BUILD_FLAGS_H_
#define _SA_BUILD_FLAGS_H_

///
/// These build flags can be overridden by defining them
/// in compiling environment variable
///
#ifndef SA_PCIE_ASPM_IN_DXE
#define SA_PCIE_ASPM_IN_DXE 1
#endif

#ifndef SA_PCIE_ASPM_IN_SMM
#define SA_PCIE_ASPM_IN_SMM 1
#endif

#endif
