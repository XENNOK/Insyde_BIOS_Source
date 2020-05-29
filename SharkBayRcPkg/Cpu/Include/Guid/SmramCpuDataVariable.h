/** @file
  
  GUIDs used for SMRAM CPU DATA Variable to keep 2 locations for copying data from
  regular memory to SMM memory
  
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

#ifndef _SMRAM_CPU_DATA_VARIABLE_H_
#define _SMRAM_CPU_DATA_VARIABLE_H_

///
/// CPU Driver will maintain ACPI CPU DATA in regular memory,
/// 1. CpuInitDxe driver will allocate SMM memory, and normal memory
/// 2. Save both location in SMRAM_CPU_DATA_VARIABLE
/// 3. Issue SMM_FROM_CPU_DRIVER_SAVE_INFO SW SMI to ask SMM driver to
///    copy data to SMRAM with signature.
/// 4. In S3 path, CPUS3 retrieves data by search signature in SMRAM.
///
#define SMRAM_CPU_DATA_VARIABLE_GUID \
  { 0x429501d9, 0xe447, 0x40f4, { 0x86, 0x7b, 0x75, 0xc9, 0x3a, 0x1d, 0xb5, 0x4e } }

#define SMRAM_CPU_DATA_VARIABLE L"SmramCpuDataVar"

typedef struct {
  EFI_PHYSICAL_ADDRESS  LockBoxData;
  EFI_PHYSICAL_ADDRESS  SmramCpuData;
  UINT64                LockBoxSize;
} SMRAM_CPU_DATA_ADDRESS;

extern EFI_GUID gSmramCpuDataVariableGuid;

#endif
