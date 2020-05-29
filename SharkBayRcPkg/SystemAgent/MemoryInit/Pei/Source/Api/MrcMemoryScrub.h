/**
This file contains an 'Intel Peripheral Driver' and uniquely
identified as "Intel Reference Module" and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor.  This file may
be modified by the user, subject to additional terms of the
license agreement.

Copyright (c) 2012 - 2012 Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MrcMemoryScrub.h

@brief:
  This file contains memory scrubbing and alias checking related information.
**/

#ifndef _MrcMemoryScrub_h_
#define _MrcMemoryScrub_h_
#pragma pack (push, 1)

#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcCommon.h>
#include <MrcDebugHook.h>
#include <MrcGlobal.h>

/**

@brief

  This function sets all the memory to a known value when ECC is enabled and
  either we are not in warm boot or we are in warm boot and TXT is set.

  @param[in] MrcData - Include all MRC global data.

  @retval mrcSuccess if the clean succeeded, otherwise an error status.

**/
extern
MrcStatus
MrcEccClean (
  IN     MrcParameters *const MrcData
  );

/**

@brief

  This function performs a memory alias check.

  @param[in] MrcData - The global host structure

  @retval mrcSuccess or error value.

**/
extern
MrcStatus
MrcAliasCheck (
  IN OUT MrcParameters *const MrcData
  );

/**

@brief

  This function runs the srcubbing test reporting any timeouts/errors.

  @param[in] MrcData - The global host structure
  @param[in] ChBitMask - Bitmask of channels the test is run on.

  @retval mrcSuccess or error value.

**/
extern
MrcStatus
MrcRunMemoryScrub (
  IN OUT MrcParameters *const MrcData,
  IN     U8                   ChBitMask
  );

#pragma pack (pop)
#endif
