/** @file
  Adjust the width of blocks.

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

#include <Library/DxeOemSvcKernelLib.h>

/*++

Todo:
  Define the relateaed data.
  
LANG_MAP_TABLE   OemLangMapTable[] = { {NON_INIT, {'e', 'n', 'g'}, {{0x00, 0x1A, 0x1A},  //Exit
                                                                    {0x01, 0x1A, 0x1A},  //Boot
                                                                    {0x02, 0x1A, 0x1A},  //Power
                                                                    {0x03, 0x1A, 0x1A},  //Security
                                                                    {0x04, 0x1A, 0x1A},  //Advanced
                                                                    {0x05, 0x1A, 0x1A}   //Main
                                       }},
                                     };

--*/

/**
  According to the setting of language and the menu of SCU to adjust the width of blocks 
  (PromptBlock, OptionBlock, and HelpBlock).

  @param[in]   *Data                 Point to EFI_IFR_DARA_ARRAY. It provides service to get current language setting.
  @param[in]   KeepCurRoot           The index of current menu (Exit, Boot, Power, etc..) in SCU.
  @param[out]  *PromptBlockWidth     The width of Prompt Block in the menu. The detail refers to the following graph.
  @param[out]  *OptionBlockWidth     The width of Option Block in the menu. The detail refers to the following graph.
  @param[out]  *HelpBlockWidth       The width of Help Block in the menu. The detail refers to the following graph.
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
   
  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           Get the block width success.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Base on OEM design.
**/
EFI_STATUS
OemSvcUpdateFormLen (
  IN     UINT8                          *Data,
  IN     UINT8                          KeepCurRoot,
  IN OUT CHAR8                          *PromptBlockWidth,
  IN OUT CHAR8                          *OptionBlockWidth,
  IN OUT CHAR8                          *HelpBlockWidth
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
