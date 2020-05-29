/** @file
  Provide OEM to modifying each MISC BIOS record.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/DxeOemSvcKernelLib.h>

#if 0  //sample implementation

#include <Library/BaseLib.h>
#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>


STATIC
VOID
SetSlotStatus (
  IN OUT  SMBIOS_TABLE_TYPE9    *Type9
  )
{
  UINT8          Bus;
  UINT8          Device;
  UINT8          Function;
  UINT32         PcieCapIdAddress;
  UINT8          PcieCapIdOffset;
  UINT16         PcieCapCapReg;
  UINT16         SlotStatus;
  EFI_STATUS     Status;
  
  Bus       = Type9->BusNum;
  Device    = Type9->DevFuncNum >> 3;  
  Function  = Type9->DevFuncNum & 0x07;

  Type9->CurrentUsage = SlotUsageAvailable;
  Status = PciFindCapId (Bus, Device, Function, EFI_PCI_CAPABILITY_ID_PCIEXP, &PcieCapIdOffset);

  if (!EFI_ERROR(Status)) {
    PcieCapIdAddress = PCI_EXPRESS_LIB_ADDRESS (Bus, Device, Function, PcieCapIdOffset);
    PcieCapCapReg  = PciExpressRead16 (PcieCapIdAddress + 0x2);
    PciExpressWrite16 (PcieCapIdAddress + 0x2, PcieCapCapReg | BIT8);
    SlotStatus = PciExpressRead16 (PcieCapIdAddress + 0x1A);

    if (((SlotStatus >> 6 ) & BIT0) == 1) {
      Type9->CurrentUsage = SlotUsageInUse;
    }
    
    PciExpressWrite16 (PcieCapIdAddress + 0x2, PcieCapCapReg);
  }

  return;
}

#endif

/**
  This service will be call by each time add SMBIOS record.
  OEM can modify SMBIOS record in run time.
  
  Notice the SMBIOS protocol is unusable when service is runing.
  
  @param[in, out]  *RecordBuffer         Each SMBIOS record data.
                                         The max length of this buffer is SMBIOS_TABLE_MAX_LENGTH.
  
  @retval    EFI_UNSUPPORTED       Returns unsupported by default.
  @retval    EFI_SUCCESS           Don't add this SMBIOS record to system.
  @retval    EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
**/
EFI_STATUS
OemSvcDxeUpdateSmbiosRecord (
  IN OUT EFI_SMBIOS_TABLE_HEADER *RecordBuffer
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  

  EFI_STATUS Status;
  
  Status = EFI_UNSUPPORTED;
  
#if 0   //sample implementation
  if (RecordBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_UNSUPPORTED;
  switch (RecordBuffer->Type) {
  case EFI_SMBIOS_TYPE_SYSTEM_SLOTS:
    SetSlotStatus((SMBIOS_TABLE_TYPE9 *)RecordBuffer);
    Status = EFI_MEDIA_CHANGED;
    break;
  default:
    break;
  } 
#endif

  return Status;
}
