/** @file
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table.

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

//=======================================
//         Pci Option Rom Table
//=======================================
PCI_OPTION_ROM_TABLE      PciOptionRomTable[] = {
  {
    NULL_ROM_FILE_GUID,
    0,
    0,
    0,
    0,
    0xffff,
    0xffff
  }
};


//=======================================
//        Non Pci Option Rom Table
//=======================================
//
// System Rom table
//
SYSTEM_ROM_TABLE    SystemRomTable[] = {
  {
    //
    // CSM16 binary
    //
    SYSTEM_ROM_FILE_GUID,
    TRUE,
    SYSTEM_ROM
  },
  {
    NULL_ROM_FILE_GUID,
    FALSE
    MAX_NUM
  }
};
.
.
.
--*/

/**
  Provide OEM to install the PCI Option ROM table and Non-PCI Option ROM table. 
  The detail refers to the document "OptionRomTable Restructure User Guide".

  @param[in]   *RomType              The type of option rom. This parameter decides which kind of option ROM table will be access.
  @param[out]  **RomTable            A pointer to the option ROM table.
  
  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           Get Option ROM Table info success.
  @retval      EFI_NOT_FOUND         Get Option ROM Table info failed.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Depends on customization.
**/
EFI_STATUS 
OemSvcInstallOptionRomTable (
  IN  UINT8                                 RomType,
  OUT VOID                                  **mOptionRomTable
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}

