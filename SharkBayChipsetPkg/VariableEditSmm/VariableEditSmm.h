/** @file
  Content file contains function definitions for Variable Edit Smm Driver.

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _VARIABLE_EDIT_SMM_H
#define _VARIABLE_EDIT_SMM_H

#include <ChipsetSmiTable.h>
#include <ChipsetSetupConfig.h>
#include <PiSmm.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PostCodeLib.h>

#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmVariable.h>
#include <Protocol/SetupCallbackSync.h>
#include <Protocol/SmmCpu.h>

//[-start-140721-IB08620401-add]//
#include <Guid/VariableEditFunctionDisable.h>
//[-end-140721-IB08620401-add]//

#define IVE_EBX_SIGNATURE            0x24495645      //$IVE
#define MAXPCISLOT                   2
#define VARIABLE_EDIT_PTR_ALIGNMENT  0x10
#define VARIABLE_EDIT_PTR_SIGNATURE  SIGNATURE_64 ('$', 'I', 'F', 'R', 'P', 'K', 'G', 1)

#define IS_VARIABLE_EDIT_PTR(p)          ((p)->Signature == VARIABLE_EDIT_PTR_SIGNATURE)

#pragma pack(1)

//
// CallBackList struct
//
typedef struct _CALLBACK_DATA {
  UINT16                CallbackID;
  CHAR8                 StrData[1];   // Variable length
                                      // null-terminated ascii string 
} CALLBACK_DATA;

typedef struct _CALLBACK_LIST {
  UINT32                ListCount;
  CALLBACK_DATA         List[1];     // Variable size
} CALLBACK_LIST;

//
// ResultList struct
//
typedef struct _RESULT_DATA {
  UINT16                CallbackID;
  UINT16                Result;
} RESULT_DATA;

typedef struct _RESULT_ARRAY {
  UINT32                ArrayCount;
  RESULT_DATA           Array[1];     // Variable size
} RESULT_ARRAY;

typedef struct _VARIABLE_EDIT_PTR {
  UINT64                Signature;
  UINT32                Address;
  UINT32                Size;
} VARIABLE_EDIT_PTR;

#pragma pack()

EFI_STATUS
EFIAPI
SetupCallbackSyncCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

#endif
