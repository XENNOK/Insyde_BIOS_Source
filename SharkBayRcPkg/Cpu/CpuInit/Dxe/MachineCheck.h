/** @file
  The header file for Machine check register initialization driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _MACHINE_CHECK_INIT_H
#define _MACHINE_CHECK_INIT_H

#include "CpuInitDxe.h"
#include "MpCommon.h"

#pragma pack(1)

typedef struct {
  UINT32 FPU : 1;
  UINT32 VME : 1;
  UINT32 DE  : 1;
  UINT32 PSE : 1;
  UINT32 TSC : 1;
  UINT32 MSR : 1;
  UINT32 PAE : 1;
  UINT32 MCE : 1;

  UINT32 CX8       : 1;
  UINT32 APIC      : 1;
  UINT32 Reserved0 : 1;
  UINT32 SEP       : 1;
  UINT32 MTRR      : 1;
  UINT32 PGE       : 1;
  UINT32 MCA       : 1;
  UINT32 CMOV      : 1;

  UINT32 PAT       : 1;
  UINT32 PSE_36    : 1;
  UINT32 PSN       : 1;
  UINT32 CLFSH     : 1;
  UINT32 Reserved1 : 1;
  UINT32 DS        : 1;
  UINT32 ACPI      : 1;
  UINT32 MMX       : 1;

  UINT32 FXSR      : 1;
  UINT32 SSE       : 1;
  UINT32 SSE2      : 1;
  UINT32 SS        : 1;
  UINT32 HTT       : 1;
  UINT32 TM        : 1;
  UINT32 Reserved2 : 1;
  UINT32 PBE       : 1;
} CPU_FEATURE;

typedef struct {
  UINT32 Count        : 8;
  UINT32 MCG_CTL_P    : 1;
  UINT32 MCG_EXT_P    : 1;
  UINT32 EXT_CORR_ERR : 1;
  UINT32 MCG_TES_P    : 1;
  UINT32 Reserved0    : 4;
  UINT32 MCG_EXT_CNT  : 8;
  UINT32 Reserved1    : 8;
} CPU_IA32_MCG_CAP_LOW_REGISTER;

#pragma pack()

/**
  Initialize all the Machine-Check registers.

  @param[in] Buffer      - Pointer to private data. Not Used.
  @param[in] MchkEnable  - Enable Mchk or not.
**/
VOID
InitializeMachineCheckRegisters (
  IN VOID    *Buffer,
  IN BOOLEAN MchkEnable
  );

/**
  Enable MCE feature for current CPU.

  @param[in] MchkEnable  - Enable Mchk or not.
**/
VOID
InitializeMce (
  IN BOOLEAN MchkEnable
  );

/**
  Enable "Machine Check Enable"
**/
VOID
EnableMce (
  VOID
  );

#endif
