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

#include "PlatformIde.h"
#include "OemPlatformIde.h"


//
// [Mark for Reduce Code]
//
#if 0
EFI_STATUS
DetectCableType (
  IN  EFI_PCI_IO_PROTOCOL               *PciIo,
  IN  CHIPSET_CONFIGURATION              *SetupVariable,
  IN  UINT8                             Channel
  )
{
  EFI_STATUS                            Status;
  UINT32                                IdeConfig;
  UINT32                                IdePataDetectValue;
  EFI_CPU_IO_PROTOCOL                   *CpuIo;

  BOOLEAN                               Flag;

  Flag = TRUE;
  Status = OemDetectCableType (PciIo, SetupVariable, Channel, &Flag);

  if (!Flag) {

  //
  // Check GPIO9(IDE_PATADET)
  //
  Status  = gBS->LocateProtocol (
              &gEfiCpuIoProtocolGuid,
              NULL,
              (VOID **)&CpuIo
              );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = CpuIo->Io.Read (
                        CpuIo,
                        EfiCpuIoWidthUint32,
                        PCH_GPIO_BASE_ADDRESS + GPIO_LVL,
                        1,
                        &IdePataDetectValue
                        );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        R_ICH_IDE_CONFIG,
                        1,
                        &IdeConfig
                        );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  IdeConfig &= ~(BIT4 | BIT5);
  //
  // Detect & set cable type
  //
  if ((IdePataDetectValue & BIT9) == 0) {
    //
    // Bit0 is zero, indicating 80-pin installed, set cable bit
    //
    if (SetupVariable->PataCableType != SETUP_IDE_CABLE_TYPE_40_PIN) {
      IdeConfig |= BIT4 | BIT5;
    }
  }

  Status = PciIo->Pci.Write (
                        PciIo,
                        EfiPciIoWidthUint32,
                        R_ICH_IDE_CONFIG,
                        1,
                        &IdeConfig
                        );

    Flag = FALSE;
    Status = OemDetectCableType (PciIo, SetupVariable, Channel, &Flag);
  }
  return Status;

}
#endif
