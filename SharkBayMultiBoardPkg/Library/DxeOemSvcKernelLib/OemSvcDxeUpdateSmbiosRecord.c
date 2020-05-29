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

//[-start-131231-IB07390120-add]//
#include <Library/BaseLib.h>
#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>

//[-start-140114-IB07390123-remove]//
//STATIC
//VOID
//SetBiosInfoStrings (
//  IN OUT  SMBIOS_TABLE_TYPE0    *Type0
//  )
//{
//  CHAR8                 *RecordPtr;
//  UINT8                  Index;
//  CHAR8                  StrBuffer[1024] = {0};
//  CHAR8                 *BufferPtr;
//  CHAR16                *BiosVersionStr;
//  CHAR16                *ReleaseDateStr;
//  UINTN                  TatolLength;
//
//  RecordPtr = ((CHAR8 *)Type0) + Type0->Hdr.Length;
//  BufferPtr = StrBuffer;
//  BiosVersionStr = (CHAR16 *) PcdGetPtr(PcdFirmwareVersionString);
//  ReleaseDateStr = (CHAR16 *) PcdGetPtr(PcdFirmwareReleaseDateString);
//
//  //
//  // BIOS Version UniString 'CRB_NAME.AB.CD.EF'
//  //
//  while (*BiosVersionStr++ != L'.');
//
//  Type0->SystemBiosMajorRelease = (UINT8)((*BiosVersionStr - L'0') * 10 + (*(BiosVersionStr + 1) - L'0'));
//  Type0->SystemBiosMinorRelease = (UINT8)((*(BiosVersionStr + 3) - L'0') * 10 + (*(BiosVersionStr + 4) - L'0'));
//
//  Index = 1;
//  TatolLength = 0;
//  while (*RecordPtr != '\0' || *(RecordPtr + 1) != '\0') {
//    if (Index == Type0->BiosVersion) {
//      UnicodeStrToAsciiStr (BiosVersionStr, BufferPtr);
//    } else if (Index == Type0->BiosReleaseDate) {
//      UnicodeStrToAsciiStr (ReleaseDateStr, BufferPtr);
//    } else {
//      AsciiStrCpy(BufferPtr, RecordPtr);
//    }
//
//    ++Index;
//    RecordPtr += AsciiStrLen(RecordPtr) + 1;
//    BufferPtr += AsciiStrLen(BufferPtr) + 1;
//  }
//
//  BufferPtr = StrBuffer;
//  RecordPtr = ((CHAR8 *)Type0) + Type0->Hdr.Length;
//  while (*BufferPtr != '\0' || *(BufferPtr + 1) != '\0') {
//    *(RecordPtr++) = *(BufferPtr++);
//  }
//
//  while (*RecordPtr != '\0' || *(RecordPtr + 1) != '\0') {
//    *(RecordPtr++) = '\0';
//  }
//
//  return;
//}
//[-end-140114-IB07390123-remove]//
//[-end-131231-IB07390120-add]//

//[-start-131231-IB07390121-add]//
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
//[-end-131231-IB07390121-add]//

/**
  This service will be call by each time add SMBIOS record.
  OEM can modify SMBIOS record in run time.
  
  Notice the SMBIOS protocol is unusable when call this service.
  
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

//[-start-131231-IB07390120-add]//
  EFI_STATUS Status;

  if (RecordBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_UNSUPPORTED;
  switch (RecordBuffer->Type) {
//[-start-140114-IB07390123-remove]//
//  case EFI_SMBIOS_TYPE_BIOS_INFORMATION:
//    SetBiosInfoStrings((SMBIOS_TABLE_TYPE0 *)RecordBuffer);
//    Status = EFI_MEDIA_CHANGED;
//    break;
//[-end-140114-IB07390123-remove]//
//[-start-131231-IB07390121-add]//
  case EFI_SMBIOS_TYPE_SYSTEM_SLOTS:
    SetSlotStatus((SMBIOS_TABLE_TYPE9 *)RecordBuffer);
    Status = EFI_MEDIA_CHANGED;
    break;
//[-end-131231-IB07390121-add]//
  default:
    break;
  }
//[-end-131231-IB07390120-add]//

  return Status;
}
