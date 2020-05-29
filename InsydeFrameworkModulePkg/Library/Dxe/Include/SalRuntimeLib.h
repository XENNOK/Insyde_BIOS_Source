//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
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

  SalRuntimeLib.h

Abstract:

  SAL Runtime Lib

Revision History

--*/

#ifndef _SAL_RUNTIME_LIB_H_
#define _SAL_RUNTIME_LIB_H_

#include EFI_PROTOCOL_DEFINITION (ExtendedSalGuid)
#include EFI_PROTOCOL_DEFINITION (ExtendedSalBootService)
#include EFI_PROTOCOL_DEFINITION (MchkService)
#include "SalApi.h"

SAL_RETURN_REGS
GetIrrData (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

VOID
PrepareApsForHandOverToOS (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

VOID
HandOverApsToOS (
  IN UINT64  a1,
  IN UINT64  a2,
  IN UINT64  a3
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  a1  - GC_TODO: add argument description
  a2  - GC_TODO: add argument description
  a3  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
GetPsrData (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

//
//  PAL PROC Class
//
SAL_RETURN_REGS
SalPalProc (
  IN  UINT64            Arg1,
  IN  UINT64            Arg2,
  IN  UINT64            Arg3,
  IN  UINT64            Arg4
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Arg1  - GC_TODO: add argument description
  Arg2  - GC_TODO: add argument description
  Arg3  - GC_TODO: add argument description
  Arg4  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
SalRegisterNewPalEntry (
  IN  BOOLEAN                     PhysicalPalAddress,
  IN  EFI_PHYSICAL_ADDRESS        NewPalAddress
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  PhysicalPalAddress  - GC_TODO: add argument description
  NewPalAddress       - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
SalGetPalEntryPointer (
  IN  BOOLEAN                     PhysicalPalAddress
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  PhysicalPalAddress  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
//  SAL BASE Class
//
SAL_RETURN_REGS
SalProcSetVectors (
  IN  UINT64                      SalVectorType,
  IN  UINT64                      PhyAddr1,
  IN  UINT64                      Gp1,
  IN  UINT64                      LengthCs1,
  IN  UINT64                      PhyAddr2,
  IN  UINT64                      Gp2,
  IN  UINT64                      LengthCs2
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  SalVectorType - GC_TODO: add argument description
  PhyAddr1      - GC_TODO: add argument description
  Gp1           - GC_TODO: add argument description
  LengthCs1     - GC_TODO: add argument description
  PhyAddr2      - GC_TODO: add argument description
  Gp2           - GC_TODO: add argument description
  LengthCs2     - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
SalProcMcRendez (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
SalProcMcSetParams (
  IN  UINT64                      ParamType,
  IN  UINT64                      IntOrMem,
  IN  UINT64                      IntOrMemVal,
  IN  UINT64                      Timeout,
  IN  UINT64                      McaOpt
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ParamType   - GC_TODO: add argument description
  IntOrMem    - GC_TODO: add argument description
  IntOrMemVal - GC_TODO: add argument description
  Timeout     - GC_TODO: add argument description
  McaOpt      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
EsalProcGetVectors (
  IN  UINT64                      VectorType
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  VectorType  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
EsalProcMcGetParams (
  IN  UINT64                      ParamInfoType
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ParamInfoType - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
EsalProcMcGetMcParams (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
EsalProcGetMcCheckinFlags (
  IN  UINT64                      ProcessorUnit
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ProcessorUnit - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
//  Sal Base Class enums
//
typedef enum {
  McaVector,
  BspInitVector,
  BootRendezVector,
  ApInitVector
} ESAL_GET_VECTOR_TYPE;

SAL_RETURN_REGS
SalInitializeThreshold (
  IN  VOID                        *ThresholdStruct,
  IN  UINT64                      Count,
  IN  UINT64                      Duration
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ThresholdStruct - GC_TODO: add argument description
  Count           - GC_TODO: add argument description
  Duration        - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
SalBumpThresholdCount (
  IN  VOID                        *ThresholdStruct,
  IN  UINT64                      Count,
  IN  UINT64                      Duration
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ThresholdStruct - GC_TODO: add argument description
  Count           - GC_TODO: add argument description
  Duration        - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
SalGetThresholdCount (
  IN  VOID                        *ThresholdStruct,
  IN  UINT64                      Count,
  IN  UINT64                      Duration
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ThresholdStruct - GC_TODO: add argument description
  Count           - GC_TODO: add argument description
  Duration        - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
// MCA PMI INIT Registeration Functions.
//
EFI_STATUS
LibRegisterMcaFunction (
  IN  EFI_SAL_MCA_HANDLER                   McaHandler,
  IN  VOID                                  *ModuleGlobal,
  IN  BOOLEAN                               MakeFirst,
  IN  BOOLEAN                               MakeLast
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  McaHandler    - GC_TODO: add argument description
  ModuleGlobal  - GC_TODO: add argument description
  MakeFirst     - GC_TODO: add argument description
  MakeLast      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibRegisterPmiFunction (
  IN  EFI_SAL_PMI_HANDLER                   PmiHandler,
  IN  VOID                                  *ModuleGlobal,
  IN  BOOLEAN                               MakeFirst,
  IN  BOOLEAN                               MakeLast
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  PmiHandler    - GC_TODO: add argument description
  ModuleGlobal  - GC_TODO: add argument description
  MakeFirst     - GC_TODO: add argument description
  MakeLast      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibRegisterInitFunction (
  IN  EFI_SAL_INIT_HANDLER                  InitHandler,
  IN  VOID                                  *ModuleGlobal,
  IN  BOOLEAN                               MakeFirst,
  IN  BOOLEAN                               MakeLast
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  InitHandler   - GC_TODO: add argument description
  ModuleGlobal  - GC_TODO: add argument description
  MakeFirst     - GC_TODO: add argument description
  MakeLast      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
//  MP Class Functions
//
SAL_RETURN_REGS
LibMpAddCpuData (
  IN    UINT64      CpuGlobalId,
  IN    BOOLEAN     Enabled,
  IN    UINT64      PalCompatability
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuGlobalId       - GC_TODO: add argument description
  Enabled           - GC_TODO: add argument description
  PalCompatability  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpRemoveCpuData (
  IN    UINT64      CpuGlobalId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuGlobalId - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpModifyCpuData (
  IN    UINT64      CpuGlobalId,
  IN    BOOLEAN     Enabled,
  IN    UINT64      PalCompatability
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuGlobalId       - GC_TODO: add argument description
  Enabled           - GC_TODO: add argument description
  PalCompatability  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpGetCpuDataByID (
  IN    UINT64      CpuGlobalId,
  IN    BOOLEAN     IndexByEnabledCpu
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuGlobalId       - GC_TODO: add argument description
  IndexByEnabledCpu - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpGetCpuDataByIndex (
  IN    UINT64      Index,
  IN    BOOLEAN     IndexByEnabledCpu
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Index             - GC_TODO: add argument description
  IndexByEnabledCpu - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpSendIpi (
  IN  UINT64                ProcessorNumber,
  IN  UINT64                VectorNumber,
  IN  EFI_DELIVERY_MODE     DeliveryMode,
  IN  BOOLEAN               IRFlag
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ProcessorNumber - GC_TODO: add argument description
  VectorNumber    - GC_TODO: add argument description
  DeliveryMode    - GC_TODO: add argument description
  IRFlag          - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpCurrentProcessor (
  IN    BOOLEAN     IndexByEnabledCpu
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  IndexByEnabledCpu - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibGetNumProcessors (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpSaveMinStatePointer (
  IN    UINT64                CpuGlobalId,
  IN    EFI_PHYSICAL_ADDRESS  MinStatePointer
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuGlobalId     - GC_TODO: add argument description
  MinStatePointer - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

SAL_RETURN_REGS
LibMpRestoreMinStatePointer (
  IN    UINT64                CpuGlobalId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuGlobalId - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
//  MCA Class Functions
//
EFI_STATUS
LibMcaGetStateInfo (
  IN  UINT64                                      CpuId,
  OUT EFI_PHYSICAL_ADDRESS                        *StateBufferPointer,
  OUT UINT64                                      *RequiredStateBufferSize
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuId                   - GC_TODO: add argument description
  StateBufferPointer      - GC_TODO: add argument description
  RequiredStateBufferSize - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibMcaRegisterCpu (
  IN  UINT64                                      CpuId,
  IN  EFI_PHYSICAL_ADDRESS                        StateBufferAddress
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuId               - GC_TODO: add argument description
  StateBufferAddress  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
//  MCA Class Functions
//
EFI_STATUS
LibMcaGetStateInfo (
  IN  UINT64                                      CpuId,
  OUT EFI_PHYSICAL_ADDRESS                        *StateBufferPointer,
  OUT UINT64                                      *RequiredStateBufferSize
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuId                   - GC_TODO: add argument description
  StateBufferPointer      - GC_TODO: add argument description
  RequiredStateBufferSize - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibMcaRegisterCpu (
  IN  UINT64                                      CpuId,
  IN  EFI_PHYSICAL_ADDRESS                        StateBufferAddress
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CpuId               - GC_TODO: add argument description
  StateBufferAddress  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

//
// SAL ELOG Functions
//
EFI_STATUS
LibSalGetStateInfo (
  IN  UINT64                                      McaType,
  IN  UINT8                                       *McaBuffer,
  OUT UINTN                                       *Size
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  McaType   - GC_TODO: add argument description
  McaBuffer - GC_TODO: add argument description
  Size      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibSalGetStateInfoSize (
  IN  UINT64                                      McaType,
  OUT UINTN                                       *Size
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  McaType - GC_TODO: add argument description
  Size    - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibSalClearStateInfo (
  IN  UINT64                                      McaType
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  McaType - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibEsalGetStateBuffer (
  IN  UINT64                                      McaType,
  OUT UINT8                                       **McaBuffer,
  OUT UINTN                                       *Index
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  McaType   - GC_TODO: add argument description
  McaBuffer - GC_TODO: add argument description
  Index     - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
LibEsalSaveStateBuffer (
  IN  UINT64                                      McaType
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  McaType - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif
