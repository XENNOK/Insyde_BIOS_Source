/** @file

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

#include <Library/SmmOemSvcChipsetLib.h>
#if 0 // Sample Implementation
#define FILL_PCI_REG_NUM            0x100
#define PCIE_PEG_BRIDGE {0x00, 0x01, 0x00}

UINT32 PEGPCI_Fun0 [FILL_PCI_REG_NUM];
UINT32 PEGPCI_Fun1 [FILL_PCI_REG_NUM];

SR_OEM_DEVICE
mPciDeviceOemList [] = {
//==============================================================================================
//          Register Number               | Bus | Dev | Fun | Reg Table   | Reg Table Save Area
//==============================================================================================
  {{ (FILL_PCI_REG_NUM),                     0x00, 0x00, 0x00, NULL,         PEGPCI_Fun0   }, PCIE_PEG_BRIDGE},
  {{ (FILL_PCI_REG_NUM),                     0x00, 0x00, 0x01, NULL,         PEGPCI_Fun1   }, PCIE_PEG_BRIDGE},
  {{ 0, 0, 0, 0, 0}, 0},
};

#endif
/**
 This function provides OEM to add additional PCI registers which is not listed 
 on global register table PciDeviceSubResList [].It will replace global register
 table PciDeviceOemSubResList[].

 Boot to OS/"S3 resume", call this function to get OEM PCI register table to 
 save/restore OEM PCI register Value. 

 @param[out]        *PciDeviceOemList   Points to the list of OEM PCI device registers which must be saved on an S3.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcGetSaveRestorePciDeviceOemList (
  OUT SR_OEM_DEVICE             **PciDeviceOemList
  )
{
/*++
  Todo:
  Add project specific code in here.
--*/
#if 0 // Sample Implementation
  (*PciDeviceOemList) = mPciDeviceOemList;

#endif
  return EFI_UNSUPPORTED;
}
