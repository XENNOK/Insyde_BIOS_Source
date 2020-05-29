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

#include <SetupUtility.h>

/**
 Check resource conflict of Isa devices

 @param [in]   MyIfrNVData      Setup variable

 @retval TRUE                   Resources of ISA devices conflict.
 @retval FALSE                  Resources of ISA devices don't conflict.

**/
BOOLEAN
CheckSioConflict (
  IN  CHIPSET_CONFIGURATION                    *MyIfrNVData
  )
{
  BOOLEAN                               ConflictFlag = FALSE;

  //
  //  Check Resource Conflict:IRQ,IObase,DMA
  //
  if (MyIfrNVData->ComPortB == 2) {
    if (MyIfrNVData->ComPortA == 2) {
      if ((MyIfrNVData->ComPortABaseIo == MyIfrNVData->ComPortBBaseIo) ||
          (MyIfrNVData->ComPortAInterrupt == MyIfrNVData->ComPortBInterrupt)) {
        MyIfrNVData->ComPortA = 1;
        MyIfrNVData->ComPortB = 0;
        ConflictFlag = TRUE;
      }
    }
  }

  //
  //  Check ComPortA Auto & disable setting
  //
  if (MyIfrNVData->ComPortA < 2) {
    if ((MyIfrNVData->ComPortB == 2) && (MyIfrNVData->ComPortBBaseIo == 3)) {
      MyIfrNVData->ComPortABaseIo = 1;
    } else {
      MyIfrNVData->ComPortABaseIo = 3;
    }
    if ((MyIfrNVData->ComPortB == 2) && (MyIfrNVData->ComPortBInterrupt == 4)) {
      MyIfrNVData->ComPortAInterrupt = 3;
    } else {
      MyIfrNVData->ComPortAInterrupt = 4;
    }
  }

  //
  //  Check ComPortB Auto & disable setting
  //
  if (MyIfrNVData->ComPortB < 2) {
    if ((MyIfrNVData->ComPortA == 2) && (MyIfrNVData->ComPortABaseIo == 1)) {
      MyIfrNVData->ComPortBBaseIo = 3;
    } else {
      MyIfrNVData->ComPortBBaseIo = 1;
    }
    if ((MyIfrNVData->ComPortA == 2) && (MyIfrNVData->ComPortAInterrupt == 3)) {
      MyIfrNVData->ComPortBInterrupt = 4;
    } else {
      MyIfrNVData->ComPortBInterrupt = 3;
    }
    if (MyIfrNVData->ComPortB == 0) {
      MyIfrNVData->ComPortBMode = 0;
    }
  }

  return ConflictFlag;
}
/**
 Check resource conflict of Isa devices and PCI solts.

 @param [in]   MyIfrNVData      Setup variable
 @param [in]   PciIrqData       Setup variable of PCI solt
 @param [out]  DeviceKind       Return conflict device.

 @retval TRUE                   Resources of ISA devices conflict.
 @retval FALSE                  Resources of ISA devices don't conflict.

**/
BOOLEAN
CheckSioAndPciSoltConflict (
  IN  CHIPSET_CONFIGURATION                    *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  )
{
  BOOLEAN                               ConflictFlag = FALSE;
  UINT8                                 SlotNum;
  BOOLEAN                               ComPortIrq[2] = {FALSE, FALSE};

  *DeviceKind = 0;

  for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {
    if (*(PciIrqData + SlotNum) == 3) {
      ComPortIrq[0] = TRUE;
    }
    if (*(PciIrqData + SlotNum) == 4) {
      ComPortIrq[1] = TRUE;
    }
  }
  //
  //Check ComPort A auto or ComPort B auto
  //
  if (!ComPortIrq[0] && ComPortIrq[1]) {
    if (MyIfrNVData->ComPortA == 1 && MyIfrNVData->ComPortB == 0) {
      MyIfrNVData->ComPortAInterrupt = 3;
    }
    if (MyIfrNVData->ComPortA == 0 && MyIfrNVData->ComPortB == 1) {
      MyIfrNVData->ComPortBInterrupt = 3;
    }
  }
  if (ComPortIrq[0] && !ComPortIrq[1]) {
    if (MyIfrNVData->ComPortA == 1 && MyIfrNVData->ComPortB == 0) {
      MyIfrNVData->ComPortAInterrupt = 4;
    }
    if (MyIfrNVData->ComPortA == 0 && MyIfrNVData->ComPortB == 1) {
      MyIfrNVData->ComPortBInterrupt = 4;
    }
  }

  if (MyIfrNVData->ComPortA > 0) {
    for(SlotNum =0; SlotNum < MAXPCISLOT; SlotNum++ ) {
      if ( MyIfrNVData->ComPortAInterrupt == *(PciIrqData + SlotNum)) {
        ConflictFlag = TRUE;
        *DeviceKind = 0;
        break;
      }
    }
  }
  if (MyIfrNVData->ComPortB > 0) {
    for(SlotNum =0; SlotNum < MAXPCISLOT; SlotNum++ ) {
      if ( MyIfrNVData->ComPortBInterrupt == *(PciIrqData + SlotNum)) {
        ConflictFlag = TRUE;
        *DeviceKind = 1;
        break;
      }
    }
  }


  return ConflictFlag;
}

/**
 Check IRQ resorces setting of Isa devices and Pci solt

 @param [in]   MyIfrNVData      Setup variable
 @param [in]   PciIrqData       First PCI solt variable
 @param [out]  DeviceKind       conflict device
                                0: No devcice conflict
                                1: COMPORT A
                                2: COMPORT B
                                3: LPT PORT

 @retval TRUE                   Resources of ISA devices and PCI solt conflict.
 @retval FALSE                  Resources of ISA devices and PCI solt don't conflict.

**/
BOOLEAN
CheckPciSioConflict (
  IN  CHIPSET_CONFIGURATION                    *MyIfrNVData,
  IN  UINT8                                   *PciIrqData,
  OUT UINT8                                   *DeviceKind
  )
{
  BOOLEAN                               ConflictFlag = FALSE;
  UINTN                                 SlotNum;
  BOOLEAN                               ComPortIrq[2] = {FALSE, FALSE};

  *DeviceKind = 0;

  for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {
    if (*(PciIrqData + SlotNum) == 3) {
      ComPortIrq[0] = TRUE;
    }
    if (*(PciIrqData + SlotNum) == 4) {
      ComPortIrq[1] = TRUE;
    }
  }
  //
  //Check ComPort A auto or ComPort B auto
  //
  if (!ComPortIrq[0] && ComPortIrq[1]) {
    if (MyIfrNVData->ComPortA == 1 && MyIfrNVData->ComPortB == 0) {
      MyIfrNVData->ComPortAInterrupt = 3;
    }
    if (MyIfrNVData->ComPortA == 0 && MyIfrNVData->ComPortB == 1) {
      MyIfrNVData->ComPortBInterrupt = 3;
    }
  }
  if (ComPortIrq[0] && !ComPortIrq[1]) {
    if (MyIfrNVData->ComPortA == 1 && MyIfrNVData->ComPortB == 0) {
      MyIfrNVData->ComPortAInterrupt = 4;
    }
    if (MyIfrNVData->ComPortA == 0 && MyIfrNVData->ComPortB == 1) {
      MyIfrNVData->ComPortBInterrupt = 4;
    }
  }
  //
  //  Check Resource Conflict:IRQ
  //
  for (SlotNum = 0; SlotNum < MAXPCISLOT; SlotNum++) {

    //
    // check COMPORT A status and COMPORT B status
    //
    if (MyIfrNVData->ComPortA == 0 && MyIfrNVData->ComPortB == 0) {
      break;
    }


    if (*(PciIrqData + SlotNum) == MyIfrNVData->ComPortAInterrupt &&
        MyIfrNVData->ComPortA != 0 ) {
      //
      //  PCI IRQ and comprot A IRQ have conflict
      //
      ConflictFlag = TRUE;
      *DeviceKind = 1;
      //
      //  set the PCI solt IRQ Auto
      //
      *(PciIrqData + SlotNum) = 0;
      break;
    }
    if (*(PciIrqData + SlotNum) == MyIfrNVData->ComPortBInterrupt &&
        MyIfrNVData->ComPortB != 0 ) {
      //
      //  PCI IRQ and comprot B IRQ have conflict
      //
      ConflictFlag = TRUE;
      *DeviceKind = 2;
      *(PciIrqData + SlotNum) = 0 ;
      break;
    }
  }

  return ConflictFlag;
}
