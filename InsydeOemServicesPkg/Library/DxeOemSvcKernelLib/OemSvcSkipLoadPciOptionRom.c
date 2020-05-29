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
  To handle a special case, more than 1 PCI devices which has the same VID/DID 
  exist on the system but not all of them has PCI option ROM to be loaded. 
  So provide an OEM service, let project owner to decide if this device has 
  PCI Option ROM to be loaded or not. if SkipGetPciRom is TRUE, 
  means there is no option ROM for this device, otherwise, 
  option ROM described on PCI Option ROM table will be loaded for this device. 
  This OEM service will only be invoked if the VID/DID of PCI device could be found in PCI Option ROM Table.	

  @param[in]  Segment               Segment number.
  @param[in]  Bus                   Bus number.
  @param[in]  Device                Device number.
  @param[in]  Function              Function number.
  @param[in]  VendorId              Device vendor ID.
  @param[in]  DeviceId              Device ID.
  @param[out] *SkipGetPciRom        If SkipGetPciRom == TRUE means that there is no 
			            ROM in this device.
  
  @retval     EFI_UNSUPPORTED       Returns unsupported by default.
  @retval     EFI_SUCCESS           The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval     Others                Based on OEM design.
**/
EFI_STATUS
OemSvcSkipLoadPciOptionRom (
  IN  UINTN                                 Segment,
  IN  UINTN                                 Bus,
  IN  UINTN                                 Device,
  IN  UINTN                                 Function,
  IN  UINT16                                VendorId,
  IN  UINT16                                DeviceId,
  OUT BOOLEAN                               *SkipGetPciRom
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/

  return EFI_UNSUPPORTED;
}
