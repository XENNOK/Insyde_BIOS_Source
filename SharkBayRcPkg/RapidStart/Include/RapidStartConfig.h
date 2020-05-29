/** @file
  Header file to define the default build options

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2011 - 2013 Intel Corporation. All rights reserved
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
#ifndef RAPID_START_CONFIG_H_
#define RAPID_START_CONFIG_H_

///
/// uncomment #define to enable the setting.
/// Don't just change the value to 0 to disable setting.
///
#ifndef RAPID_START_TRIM_ON_RESUME
#define RAPID_START_TRIM_ON_RESUME  1
#endif

#ifndef RAPID_START_NO_SMRAM_INTEGRITY_CHECK
#define RAPID_START_NO_SMRAM_INTEGRITY_CHECK
#endif

//[-start-120809-IB03780457-modify]//
#ifndef RAPID_START_USE_OLD_ACPI_VARIABLE_SET
//#define RAPID_START_USE_OLD_ACPI_VARIABLE_SET
#endif
//[-end-120809-IB03780457-modify]//

#ifndef RAPID_START_WHOLE_MEMORY_CHECK
// #define RAPID_START_WHOLE_MEMORY_CHECK
#endif

#ifndef RAPID_START_ON_MEMORY_INSTALLED
#define RAPID_START_ON_MEMORY_INSTALLED
#endif

#ifndef RAPID_START_ON_START_PPI_INSTALLED
// #define RAPID_START_ON_START_PPI_INSTALLED
#endif

//[-start-120815-IB06460429-modify]//
#ifndef RAPID_START_SCAN_ZERO_PAGE
#define RAPID_START_SCAN_ZERO_PAGE
#endif
//[-end-120815-IB06460429-modify]//

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
///
/// SW SMI callback function to check memory
///
#ifndef SW_SMI_WHOLE_MEMORY_CHECK
#define SW_SMI_WHOLE_MEMORY_CHECK 0x51
#endif
///
/// Default granularity 1MB
///
#ifndef MEMORY_CRC32_GRANULARITY
#define MEMORY_CRC32_GRANULARITY  0x100000
#endif
#endif // RAPID_START_WHOLE_MEMORY_CHECK
#endif
