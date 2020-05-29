/** @file

  Starting point for the core memory reference code.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _MRC_START_MEMORY_CONFIGURATION_H_
#define _MRC_START_MEMORY_CONFIGURATION_H_

//
// Include files
//
#include <MrcTypes.h>
#include <MrcApi.h>

typedef U16 MrcPostCode;
#define POST_CODE_NO_OVR  ((1 << ((sizeof (MrcPostCode) * 8) - 1)) + ((1 << ((sizeof (MrcPostCode) * 8) - 1)) - 1))

typedef enum {
  MRC_PF_NULL,                ///< All policy flags turned off.
  MRC_PF_COLD     = (1 << 0), ///< Execute MRC function on cold reset.
  MRC_PF_FAST     = (1 << 1), ///< Execute MRC function on cold reset when S3 data is present.
  MRC_PF_WARM     = (1 << 2), ///< Execute MRC function on warm reset.
  MRC_PF_S3       = (1 << 3), ///< Execute MRC function on S3 exit.
  MRC_PF_FULL_MRC = (1 << 4), ///< Execute MRC function when in Full MRC mode.
  MRC_PF_MINI_MRC = (1 << 5), ///< Execute MRC function when in Mini-MRC mode.
  MRC_PF_UNUSED   = (3 << 6), ///< Unused policy flags.
  MRC_PF_ALL      = (0xF)     ///< All policy flags turned off.
} PFSelector;

typedef U8 PolicyFlag;

#pragma pack(push, 1)
typedef struct {
  MrcStatus (*mrc_task) (MrcParameters * const MrcData);  ///< Ptr to function to execute, with parameter list.
  MrcPostCode   post_code_ovr;                            ///< BIOS post code output to the debug port if value <> 0.
  U32           oem_cmd;                                  ///< OEM function to execute prior to MRC function.
  PolicyFlag    policy_flag;                              ///< Call table flags
  MrcIteration  iteration;                                ///< Maximum number of CPU only resets.
#ifdef MRC_DEBUG_PRINT
  char          *String;                                  ///< Output string describing this task (potentially output to debug serial port).
#endif // MRC_DEBUG_PRINT
} CallTableEntry;
#pragma pack(pop)

/**
  Initializes the memory controller and DIMMs.

  @param[in, out] MrcData - Include all MRC global data.

  @retval mrcSuccess if the initalization suceeded, otherwise an error status indicating the failure.
*/
extern
MrcStatus
MrcStartMemoryConfiguration (
  IN OUT MrcParameters *const MrcData
  );

#endif
