/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "Tiano.h"
#include "EfiCommonLib.h"
#include "SetupConfig.h"
#include EFI_GUID_DEFINITION (SetupDefaultGuid)

#define VARIABLE_NAME     L"Setup"

extern UINT8  BootVfrSystemConfigDefault0000[];
extern UINT8  PowerVfrSystemConfigDefault0000[];
extern UINT8  SecurityVfrSystemConfigDefault0000[];
extern UINT8  AdvanceVfrSystemConfigDefault0000[];
extern UINT8  MainVfrSystemConfigDefault0000[];

VOID
ExtractVfrDefault (
    UINT8 *NvData,
    UINT8 *VfrDefault
  )
{
   UINTN   VfrBufSize;
   UINTN   DataSize;
   UINTN   VfrIndex;
   UINTN   NvDataIndex;

   VfrBufSize = (UINTN)( *(UINT32 *)VfrDefault );
   VfrIndex = sizeof (UINT32);
   do {
     NvDataIndex  = *(UINT16 *)(VfrDefault + VfrIndex);
//     ASSERT (NvDataIndex < sizeof(SYSTEM_CONFIGURATION) );
     VfrIndex    += 2;
     DataSize     = *(UINT16 *)(VfrDefault + VfrIndex);
     VfrIndex    += 2;
     EfiCommonLibCopyMem(NvData+NvDataIndex, VfrDefault+VfrIndex, DataSize);
     VfrIndex += DataSize;
   } while (VfrIndex < VfrBufSize);
}


VOID
ExtractSetupDefault (
  UINT8 *SetupDefault
  )
{
  SETUP_DEFAULT_HEADER  *SetupDefaultHeader;
  UINT8                 *SetupData;
  EFI_GUID  SetupVariableGuid = SYSTEM_CONFIGURATION_GUID;
  CHAR16    SetupVariableName[] = VARIABLE_NAME;

  UINT8  *VfrDefaults[] = {
    BootVfrSystemConfigDefault0000,
    PowerVfrSystemConfigDefault0000,
    SecurityVfrSystemConfigDefault0000,
    AdvanceVfrSystemConfigDefault0000,
    MainVfrSystemConfigDefault0000,
    NULL
  };
  UINTN Index;
  
  SetupDefaultHeader = (SETUP_DEFAULT_HEADER *)SetupDefault;
  SetupDefaultHeader->Signature[0] = 'S';
  SetupDefaultHeader->Signature[1] = 'N';
  SetupDefaultHeader->Signature[2] = 'V';
  SetupDefaultHeader->Signature[3] = 'D';

  EfiCommonLibCopyMem (&(SetupDefaultHeader->VariableGuid), &SetupVariableGuid, sizeof (EFI_GUID));
  SetupDefaultHeader->VariableNameSize = sizeof (VARIABLE_NAME);
  EfiCommonLibCopyMem (SetupDefault + sizeof(SETUP_DEFAULT_HEADER), (VOID*)SetupVariableName, SetupDefaultHeader->VariableNameSize);

  SetupData = SetupDefault + sizeof(SETUP_DEFAULT_HEADER) + SetupDefaultHeader->VariableNameSize;
  EfiCommonLibZeroMem (SetupData, sizeof(SYSTEM_CONFIGURATION));
  Index = 0;
  while (VfrDefaults[Index] != NULL) {
    ExtractVfrDefault (
      SetupData,
      VfrDefaults[Index]
    );
    Index++;
  }
}