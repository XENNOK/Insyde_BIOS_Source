/** @file
  Adjust the width of blocks.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>

/**
  According to the setting of language and the menu of SCU to adjust the width of blocks 
  (PromptBlock, OptionBlock, and HelpBlock).

  @param  *Data                 Point to EFI_IFR_DARA_ARRAY. It provides service to get current language setting.
  @param  KeepCurRoot           The index of current menu (Exit, Boot, Power, etc..) in SCU.
  @param  *PromptBlockWidth     The width of Prompt Block in the menu. The detail refers to the following graph.
  @param  *OptionBlockWidth     The width of Option Block in the menu. The detail refers to the following graph.
  @param  *HelpBlockWidth       The width of Help Block in the menu. The detail refers to the following graph.
   |--------------------------------------------------|
   |   SCU menu        (Boot, Exit..etc)              |
   |--------------------------------------------------|
   |                |                |                |
   |                |                |                |
   |PromptBlockWidth|OptionBlockWidth| HelpBlockWidth |
   |                |                |                |
   |                |                |                |
   |                |                |                |
   |--------------------------------------------------|
   
  @retval EFI_SUCCESS           Get the block width success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcUpdateFormLenThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINT8                                 *Data;
  UINT8                                 KeepCurRoot;
  CHAR8                                 *PromptBlockWidth;
  CHAR8                                 *OptionBlockWidth;
  CHAR8                                 *HelpBlockWidth;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);

  Data                 = VA_ARG (Marker, UINT8 *);
  KeepCurRoot          = VA_ARG (Marker, UINT8);
  PromptBlockWidth     = VA_ARG (Marker, CHAR8 *);
  OptionBlockWidth     = VA_ARG (Marker, CHAR8 *);
  HelpBlockWidth       = VA_ARG (Marker, CHAR8 *);

  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcUpdateFormLen (
             Data,
             KeepCurRoot,
             PromptBlockWidth,
             OptionBlockWidth,
             HelpBlockWidth
             );

  return Status;
}
