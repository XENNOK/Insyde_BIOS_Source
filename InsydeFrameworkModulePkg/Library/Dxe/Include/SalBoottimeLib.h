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

Copyright (c)  2005 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

    SalBoottimeLib.h

Abstract:

  Light weight Sal lib to support boot time drivers.

--*/

#ifndef _SAL_BOOTTIME_LIB_H_
#define _SAL_BOOTTIME_LIB_H_

#include "EfiStatusCode.h"
#include "EfiCommonLib.h"

#include EFI_GUID_DEFINITION (DxeServices)
#include EFI_PROTOCOL_DEFINITION (ExtendedSalGuid)
#include EFI_PROTOCOL_DEFINITION (ExtendedSalBootService)
#include EFI_PROTOCOL_DEFINITION (MchkService)
#include "SalApi.h"
#include "PalApi.h"

//
// Driver Lib Globals.
//
extern EFI_BOOT_SERVICES  *gBS;
extern EFI_SYSTEM_TABLE   *gST;
extern EFI_DXE_SERVICES   *gDS;

EFI_STATUS
EfiInitializeSalBoottimeLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

  Intialize Runtime Driver Lib if it has not yet been initialized.

Arguments:

  ImageHandle     - The firmware allocated handle for the EFI image.
  SystemTable     - A pointer to the EFI System Table.

Returns:

  EFI_STATUS always returns EFI_SUCCESS

--*/
;

SAL_RETURN_REGS
EfiCallEsalService (
  IN  EFI_GUID                                      *ClassGuid,
  IN  UINT64                                        FunctionId,
  IN  UINT64                                        Arg2,
  IN  UINT64                                        Arg3,
  IN  UINT64                                        Arg4,
  IN  UINT64                                        Arg5,
  IN  UINT64                                        Arg6,
  IN  UINT64                                        Arg7,
  IN  UINT64                                        Arg8
  )
/*++

Routine Description:

  Call module that is not linked direclty to this module. This code is IP
  relative and hides the binding issues of virtual or physical calling. The
  function that gets dispatched has extra arguments that include the registered
  module global and a boolean flag to indicate if the system is in virutal mode.

Arguments:
  ClassGuid   - GUID of function
  FunctionId  - Function in ClassGuid to call
  Arg2        - Argument 2 ClassGuid/FunctionId defined
  Arg3        - Argument 3 ClassGuid/FunctionId defined
  Arg4        - Argument 4 ClassGuid/FunctionId defined
  Arg5        - Argument 5 ClassGuid/FunctionId defined
  Arg6        - Argument 6 ClassGuid/FunctionId defined
  Arg7        - Argument 7 ClassGuid/FunctionId defined
  Arg8        - Argument 8 ClassGuid/FunctionId defined

Returns:
  Status of ClassGuid/FuncitonId

--*/
;
EFI_STATUS
EfiReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Status Code reporter

Arguments:

  CodeType    - Type of Status Code.
  Value       - Value to output for Status Code.
  Instance    - Instance Number of this status code.
  CallerId    - ID of the caller of this status code.
  Data        - Optional data associated with this status code.

Returns:

  Status code

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

  Call pal proc.

Arguments:

  Arg1          - Pal call index
  Arg2          - First arg
  Arg3          - Second arg
  Arg4          - Third arg

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
SalRegisterNewPalEntry (
  IN  BOOLEAN                     PhysicalPalAddress,
  IN  EFI_PHYSICAL_ADDRESS        NewPalAddress
  )
/*++

Routine Description:

  Register Pal entry.

Arguments:

  PhysicalPalAddress      - The address is physical or virtual
  NewPalAddress           - New Pal entry address

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
SalGetPalEntryPointer (
  IN  BOOLEAN                     PhysicalPalAddress
  )
/*++

Routine Description:

  Get Pal entry.

Arguments:

  PhysicalPalAddress      - The address is physical or virtual

Returns:

  Status code

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

  Set vectors.

Arguments:

  SalVectorType     - Vector type
  PhyAddr1          - OS MCA entry point
  Gp1               - GP for OS MCA entry
  LengthCs1         - Length of OS MCA
  PhyAddr2          - OS INIT entry point
  Gp2               - GP for OS Init entry
  LengthCs2         - Length of OS INIT

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
SalProcMcRendez (
  VOID
  )
/*++

Routine Description:

  Mc rendezvous function.

Arguments:

  None

Returns:

  Output regs

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

  Set MCA parameters.

Arguments:

  ParamType     - Parameter type
  IntOrMem      - Interrupt or memory address
  IntOrMemVal   - Interrupt number or memory address value
  Timeout       - Time out value
  McaOpt        - Option for MCA

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
EsalProcGetVectors (
  IN  UINT64                      VectorType
  )
/*++

Routine Description:

  Get OS MCA vector.

Arguments:

  VectorType      - Vector type

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
EsalProcMcGetParams (
  IN  UINT64                      ParamInfoType
  )
/*++

Routine Description:

  Get MCA parameter.

Arguments:

  ParamInfoType     - Parameter info type

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
EsalProcMcGetMcParams (
  VOID
  )
/*++

Routine Description:

  Get MCA parameter.

Arguments:

  None

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
EsalProcGetMcCheckinFlags (
  IN  UINT64                      ProcessorUnit
  )
/*++

Routine Description:

  Get process status.

Arguments:

  ProcessorUnit     - Processor Index

Returns:

  Output regs

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

  Init threshold structure.

Arguments:

  ThresholdStruct     - Threshold structure
  Count               - Threshold count
  Duration            - Duration

Returns:

  Output regs

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

  Bump threshold count.

Arguments:

  ThresholdStruct     - Threshold structure
  Count               - Threshold count
  Duration            - Duration

Returns:

  Output regs

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

  Get threshold structure.

Arguments:

  ThresholdStruct     - Threshold structure
  Count               - Threshold count
  Duration            - Duration

Returns:

  Output regs

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

  Register MCA handler.

Arguments:

  McaHandler      - MCA handler
  ModuleGlobal    - Module global for function
  MakeFirst       - Make it as first?
  MakeLast        - Make it as last?

Returns:

  Status code

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

  Register PMI handler.

Arguments:

  PmiHandler      - PMI handler
  ModuleGlobal    - Module global for function
  MakeFirst       - Make it as first?
  MakeLast        - Make it as last?

Returns:

  Status code

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

  Register INIT handler.

Arguments:

  InitHandler     - INIT handler
  ModuleGlobal    - Module global for function
  MakeFirst       - Make it as first?
  MakeLast        - Make it as last?

Returns:

  Status code

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

  Add CPU data.

Arguments:

  CpuGlobalId         - CPU ID
  Enabled             - Enabled or not
  PalCompatability    - Pal compatability

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibMpRemoveCpuData (
  IN    UINT64      CpuGlobalId
  )
/*++

Routine Description:

  Remove CPU data.

Arguments:

  CpuGlobalId         - CPU ID

Returns:

  Output regs

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

  Modify CPU data.

Arguments:

  CpuGlobalId         - CPU ID
  Enabled             - Enabled or not
  PalCompatability    - Pal compatability

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibMpGetCpuDataByID (
  IN    UINT64      CpuGlobalId,
  IN    BOOLEAN     IndexByEnabledCpu
  )
/*++

Routine Description:

  Get CPU data.

Arguments:

  CpuGlobalId         - CPU ID
  IndexByEnabledCpu   - Whether indexed by enabled CPU

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibMpGetCpuDataByIndex (
  IN    UINT64      Index,
  IN    BOOLEAN     IndexByEnabledCpu
  )
/*++

Routine Description:

  Get CPU data.

Arguments:

  Index               - CPU index
  IndexByEnabledCpu   - Whether indexed by enabled CPU

Returns:

  Output regs

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

  Send IPI.

Arguments:

  ProcessorNumber         - Processor number
  VectorNumber            - Vector number
  DeliveryMode            - Delivery mode
  IRFlag                  - Interrupt Redirection flag

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibMpCurrentProcessor (
  IN    BOOLEAN     IndexByEnabledCpu
  )
/*++

Routine Description:

  Get current processor index.

Arguments:

  IndexByEnabledCpu       - Whether indexed by enabled CPU

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibGetNumProcessors (
  VOID
  )
/*++

Routine Description:

  Get number of processors.

Arguments:

  None

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibMpSaveMinStatePointer (
  IN    UINT64                CpuGlobalId,
  IN    EFI_PHYSICAL_ADDRESS  MinStatePointer
  )
/*++

Routine Description:

  Register pointer to save min state.

Arguments:

  CpuGlobalId       - CPU global ID
  MinStatePointer   - Pointer to save min state

Returns:

  Output regs

--*/
;

SAL_RETURN_REGS
LibMpRestoreMinStatePointer (
  IN    UINT64                CpuGlobalId
  )
/*++

Routine Description:

  Restore pointer to save min state.

Arguments:

  CpuGlobalId       - CPU global ID

Returns:

  Output regs

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

  MCA get state info.

Arguments:

  CpuId                   - CPU ID
  StateBufferPointer      - Pointer of state buffer
  RequiredStateBufferSize - Size of required state buffer

Returns:

  Status code

--*/
;

EFI_STATUS
LibMcaRegisterCpu (
  IN  UINT64                                      CpuId,
  IN  EFI_PHYSICAL_ADDRESS                        StateBufferAddress
  )
/*++

Routine Description:

  MCA register CPU state info.

Arguments:

  CpuId                   - CPU ID
  StateBufferAddress      - Pointer of state buffer

Returns:

  Status code

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

  Get state info.

Arguments:

  McaType                 - MCA type
  McaBuffer               - Info buffer provided by caller
  Size                    - Size of info

Returns:

  Status code

--*/
;

EFI_STATUS
LibSalGetStateInfoSize (
  IN  UINT64                                      McaType,
  OUT UINTN                                       *Size
  )
/*++

Routine Description:

  Get state info size.

Arguments:

  McaType                   - MCA type
  Size                      - Size required

Returns:

  Status code

--*/
;

EFI_STATUS
LibSalClearStateInfo (
  IN  UINT64                                      McaType
  )
/*++

Routine Description:

  Clear state info.

Arguments:

  McaType                   - MCA type

Returns:

  Status code

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

  Get state buffer.

Arguments:

  McaType                   - MCA type
  McaBuffer                 - MCA buffer
  Index                     - CPU index

Returns:

  Status code

--*/
;

EFI_STATUS
LibEsalSaveStateBuffer (
  IN  UINT64                                      McaType
  )
/*++

Routine Description:

  Save state buffer.

Arguments:

  McaType                   - MCA type

Returns:

  Status code

--*/
;

#endif
