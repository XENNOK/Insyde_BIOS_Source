//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  BeepCodeTable.c

Abstract:

  Table which can be overwritten by platform policy. This table can be customized/enhanced as
  per platform need. No code will be required to be updated except this table

--*/

#include "BeepStatusCode.h"
#include EFI_PPI_DEPENDENCY (Stall)

#define MAX_BEEP_LOOP 4

typedef struct {
  UINT32  ClassSubClass;
  UINT32  Operation;
  UINT32  BeepNum;
} EFI_PLATFORM_BEEP_CODE_TABLE;

EFI_PLATFORM_BEEP_CODE_TABLE  BeepCodeTable[] = {
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_EC_NOT_SUPPORTED,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_EC_NOT_DETECTED,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_EC_NOT_CONFIGURED,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_MEMORY_EC_INVALID_TYPE,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_MEMORY_EC_INVALID_SPEED,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_MEMORY_EC_CORRECTABLE,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_MEMORY_EC_UNCORRECTABLE,
    3
  },
  {
    EFI_COMPUTING_UNIT_MEMORY,
    EFI_CU_MEMORY_EC_INVALID_SIZE,
    3
  },
  {
    EFI_COMPUTING_UNIT_HOST_PROCESSOR,
    EFI_CU_HP_EC_MISMATCH,
    2
  },
  {
    EFI_COMPUTING_UNIT_HOST_PROCESSOR,
    EFI_CU_HP_EC_MICROCODE_UPDATE,
    2
  },
  {
    EFI_COMPUTING_UNIT_HOST_PROCESSOR,
    EFI_CU_HP_EC_INVALID_TYPE,
    2
  },
  {
    EFI_SOFTWARE_PEI_MODULE,
    EFI_SW_EC_ILLEGAL_SOFTWARE_STATE,
    1
  },
};

EFI_STATUS
PlatformBeepCode (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN  PEI_SPEAKER_IF_PPI      *Speaker,
  IN  EFI_STATUS_CODE_VALUE   Value
  )
/*++

Routine Description:

  This function walks through the table based upon Value field of Status Code, generate the beep for
  number of times specified as per platform policy
Arguments:

  FfsHeader   - FV this PEIM was loaded from.
  PeiServices - General purpose services available to every PEIM.

Returns:

  None

--*/
// GC_TODO:    Speaker - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    EFI_NOT_FOUND - add return value to function comment
// GC_TODO:    EFI_SUCCESS - add return value to function comment
{
  UINTN         TableSize;
  UINT32        BeepNum;
  EFI_STATUS    Status;
  UINTN         Index;
  PEI_STALL_PPI *StallPpi;

  Status = (**PeiServices).LocatePpi (
                            PeiServices,
                            &gPeiStallPpiGuid,
                            0,
                            NULL,
                            &StallPpi
                            );

  TableSize = (sizeof (BeepCodeTable) / sizeof (EFI_PLATFORM_BEEP_CODE_TABLE));

  BeepNum   = 0;

  for (Index = 0; Index < TableSize; Index++) {
    if (((Value & 0xFFFF0000) == BeepCodeTable[Index].ClassSubClass) &&
        ((Value & 0x0000FFFF) == BeepCodeTable[Index].Operation)
        ) {
      BeepNum = BeepCodeTable[Index].BeepNum;
      break;
    }
  }

  if (BeepNum == 0) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < MAX_BEEP_LOOP; Index++) {
    Speaker->GenerateBeep (PeiServices, BeepNum, 0, 0);
    //
    // Need to fix this value of Stall once Stall ppi is fixed.
    //
    StallPpi->Stall (PeiServices, StallPpi, 0x7000);
  }

  return EFI_SUCCESS;
}
