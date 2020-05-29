/** @file
  Header file for Smm Int15 Service Driver's Data Structures

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INT15_SERVICE_SMM_H_
#define _INT15_SERVICE_SMM_H_

#include <PiSmm.h>
#include <Framework/SmmCis.h>
#include <SmiTable.h>
#include <OemSmmInt15.h>

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmCpu.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/LoadFile.h>
#include <Protocol/LoadFile2.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/SmmInt15Service.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/LegacyRegion2.h>
#include <Protocol/LegacyBios.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/DebugLib.h>

#include <IndustryStandard/PeImage.h>

//
// INT15 Private data guid
//
extern EFI_GUID gInt15PrivateDataGuid;


#pragma pack(1)

typedef struct {
  UINT16                 FunctionListSegment;
  UINT16                 CpuRegistersSegment;
} INT15_PRIVATE_DATA;

typedef struct {
EFI_IA32_REGISTER_SET    CpuRegisters;
UINT16                   InSmmFlag;
UINT64                   SmmInt15CallbackPtr;
} INT15_PRIVATE_INFO;

typedef
VOID
(EFIAPI *SMM_INT15_CALLBACK_FUNCTION) (
  IN  OUT EFI_IA32_REGISTER_SET   *CpuRegs
  );

#define INT15_DATA_SIGNATURE    SIGNATURE_32 ('I', '1', '5', 'D')
typedef struct {
  UINTN                         Signature;
  LIST_ENTRY                    FunctionLinkList;
  UINT16                        FunctionNum;
  UINTN                         FunctionPtr;
  UINTN                         ContextPtr;
} INT15_DATA_PACKET;

#define INT15_SERVICE_INSTANCE_SIGNATURE  SIGNATURE_32 ('I', '1', '5', 'S')
typedef struct {
  UINT32                          Signature;
  UINT16                          FunctionListSegment;
  UINT16                          CpuRegistersSegment;
  INT15_DATA_PACKET               *Int15DataPacketDummy;
  EFI_SMM_INT15_SERVICE_PROTOCOL  Int15Service;
  EFI_OEM_INT15_CALLBACK          *OemInt15Callback;
} INT15_SERVICE_INSTANCE;

#pragma pack()

/**
  Install Int15 function interface

  @param[in]  This               Point to EFI_INT15_SERVICE_PROTOCOL.
  @param[in]  FunctionNum        Oem Int15 function number
  @param[in]  FunctionPtr        Oem Int15 function point
  @param[in]  Context            Oem Int15 Context point

  @retval EFI_SUCCESS            The call returned successfully.
  @retval EFI_ALREADY_STARTED    The same as INT15 function number
  @retval EFI_INVALID_PARAMETER  Invalid parameter
  @retval EFI_OUT_OF_RESOURCES   The Int15 function count is greater than MAX_OEM_INT15_FN_COUNT

**/
EFI_STATUS
EFIAPI
InstallInt15ProtocolInterface (
  IN EFI_SMM_INT15_SERVICE_PROTOCOL   *This,
  IN UINT16                           FunctionNum,
  IN EFI_OEM_INT15_CALLBACK_FUNCTION  FunctionPtr,
  IN VOID                             *Context  OPTIONAL
  )
;

/**
  Re-Install Int15 function interface

  @param[in]  This               Point to EFI_INT15_SERVICE_PROTOCOL.
  @param[in]  FunctionNum        Oem Int15 function number
  @param[in]  FunctionPtr        Oem Int15 function point
  @param[in]  Context            Oem Int15 Context point

  @retval EFI_SUCCESS            The call returned successfully.
  @retval EFI_NOT_FOUND          Can't find the Int15 function number
  @retval EFI_INVALID_PARAMETER  Invalid parameter

**/
EFI_STATUS
EFIAPI
ReinstallInt15ProtocolInterface (
  IN EFI_SMM_INT15_SERVICE_PROTOCOL   *This,
  IN UINT16                           FunctionNum,
  IN EFI_OEM_INT15_CALLBACK_FUNCTION  FunctionPtr,
  IN VOID                             *Context  OPTIONAL
  )
;

#endif
