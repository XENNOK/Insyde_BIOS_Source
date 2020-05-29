/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c)  2012 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  SsaCallbackPeim.h

@brief
  Header file for the SSA BIOS Callback Init PEIM
**/
#ifndef _SSA_CALLBACK_PEIM_H_
#define _SSA_CALLBACK_PEIM_H_

#include "MrcTypes.h"
#include "MrcDebugHook.h"
#include "MrcGlobal.h"

struct _SSA_BIOS_CALLBACKS_PPI;

extern EFI_GUID gSsaBiosCallBacksPpiGuid;
typedef int MRC_OEM_STATUS_COMMAND;

typedef 
MrcStatus
(EFIAPI * MRC_CHECKPOINT) (
  EFI_PEI_SERVICES                 **PeiServices,
  struct _SSA_BIOS_CALLBACKS_PPI   *SsaBiosCallBacksPpi,
  MRC_OEM_STATUS_COMMAND           StatusCommand,
  VOID                             *CheckpointData
  );

typedef struct _SSA_BIOS_CALLBACKS_PPI {
  UINT32                      Revision;
  MRC_CHECKPOINT              MrcCheckpoint;
  VOID                        *ModuleState;
} SSA_BIOS_CALLBACKS_PPI;


#endif
