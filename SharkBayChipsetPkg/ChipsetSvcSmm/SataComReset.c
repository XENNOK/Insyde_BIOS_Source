/** @file
 SMM Chipset Services Library.
 	
 This file contains only one function that is SmmCsSvcSataComReset().
 The function SmmCsSvcSataComReset() use chipset services to reset specified SATA port.
	
***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <PchAccess.h>
#include <PortNumberMap.h>

// Power On Code
//[-start-121102-IB05300351-modify]//
//#define ICH_ACPI_TIMER_ADDR       0x0408
//#define ICH_ACPI_TIMER_MAX_VALUE  0x1000000 // The timer is 24 bit overflow
#define ACPI_TIMER_ADDR       (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)
#define ACPI_TIMER_MAX_VALUE  0x1000000
//[-end-121102-IB05300351-modify]//

// PCS-Port Control and Status Register. Address Offset: 92h-93h.
// Bit0:Port0 Enable-(R/W), Bit1:Port1 Enable-(R/W)
// Bit2:Port2 Enable-(R/W), Bit3:Port3 Enable-(R/W)
// Bit4:Port4 Enable-(R/W), Bit5:Port5 Enable-(R/W)
UINT16 mAhciPortEnableBit[6] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5};
UINT16 mIdePortEnableBit[6]  = {BIT0, BIT1, BIT2, BIT3, BIT0, BIT1};

/**
 Waits for at least the given number of microseconds.

 @param[in]         Microseconds        Desired length of time to wait.

 @retval            EFI_SUCCESS         If the desired amount of time passed. 
*/
STATIC
EFI_STATUS
Stall ( 
  IN UINTN              Microseconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINT32  CurrentTick;
  UINT32  OriginalTick;
  UINT32  RemainingTick;

  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }
  //
  // Don't use CpuIO PPI for IO port access here, it will result 915
  // platform recovery fail when using the floppy,because the CpuIO PPI is
  // located at the flash.Use the ASM file to replace it.
  //
  OriginalTick  = IoRead32 (ACPI_TIMER_ADDR);

  CurrentTick   = OriginalTick;

  //
  // The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  //
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
  //
  // The loops needed by timer overflow
  //
  Counts = Ticks / ACPI_TIMER_MAX_VALUE;
  //
  // remaining clocks within one loop
  //
  RemainingTick = (UINT32) Ticks % ACPI_TIMER_MAX_VALUE;
  //
  // not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  // one I/O operation, and maybe generate SMI
  //
  while (Counts != 0) {
    CurrentTick = IoRead32 (ACPI_TIMER_ADDR);

    if (CurrentTick <= OriginalTick) {
      Counts--;
    }

    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick)) {
    OriginalTick  = CurrentTick;
    CurrentTick   = (UINT32) IoRead32 (ACPI_TIMER_ADDR);
  }

  return EFI_SUCCESS;
}

/**
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset
                    
 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
*/
EFI_STATUS
SataComReset (
  IN  UINTN         PortNumber
  )
{
//[-start-121108-IB05280008-remove]//
//  EFI_STATUS                        Status;
//[-end-121108-IB05280008-remove]//
  UINT8                             SataMode;
  UINT32                            BusNumber;
  UINT32                            DeviceNumber;
  UINT32                            FunctionNumber;
  UINTN                             Index;
  UINTN                             NoPorts;
  PORT_NUMBER_MAP                   *PortNumberMapTable;
  UINT16                            PortEnableBit;
//[-start-121108-IB05280008-add]//
  PORT_NUMBER_MAP                   EndEntry;

  PortNumberMapTable = NULL;
  
  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));
//[-end-121108-IB05280008-add]//

  BusNumber               = 0;
  DeviceNumber            = 0;
  FunctionNumber          = 0;
  PortEnableBit           = 0;
  //
  //Get Sata Mode
  //
  SataMode = PchSataPciCfg8 (R_PCH_SATA_SUB_CLASS_CODE);

  if ((SataMode == V_PCH_SATA_SUB_CLASS_CODE_AHCI) || (SataMode == V_PCH_SATA_SUB_CLASS_CODE_RAID)) {
    //
    //AHCI Mode or RAID Mode => D31:F2
    //
    BusNumber               = DEFAULT_PCI_BUS_NUMBER_PCH;
    DeviceNumber            = PCI_DEVICE_NUMBER_PCH_SATA;
    FunctionNumber          = PCI_FUNCTION_NUMBER_PCH_SATA;
    PortEnableBit           = mAhciPortEnableBit[PortNumber];

  } else if (SataMode == V_PCH_SATA_SUB_CLASS_CODE_IDE) {
    //
    //IDE Mode => Get Bus, Device and Function from PortNumberMapTable.
    //
//[-start-121108-IB05280008-modify]//
  PortNumberMapTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortNumberMapTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
//[-end-121108-IB05280008-modify]//
      return EFI_DEVICE_ERROR;
    }

    for (Index = 0; Index < NoPorts; Index++) {
      if (PortNumber == PortNumberMapTable[Index].PortNum) {
        BusNumber       = PortNumberMapTable[Index].Bus;
        DeviceNumber    = PortNumberMapTable[Index].Device;
        FunctionNumber  = PortNumberMapTable[Index].Function;
        PortEnableBit   = mIdePortEnableBit[PortNumber];

        if (DeviceNumber != PCI_DEVICE_NUMBER_PCH_SATA) {
          //
          //IDER Mode no ComReset
          //
          return EFI_SUCCESS;
        }

        break;
      }
      //
      //It isn't same PortNumber with PortNumberMapTable.
      //
      if (Index == (NoPorts - 1)) {

        return EFI_DEVICE_ERROR;
      }
    }
  } else {

    return EFI_DEVICE_ERROR;
  }

  MmioAnd16 (MmPciAddress(0, BusNumber, DeviceNumber, FunctionNumber, R_PCH_SATA_PCS), ~PortEnableBit);
  Stall (400);
  MmioOr16 ( MmPciAddress (0, BusNumber, DeviceNumber, FunctionNumber, R_PCH_SATA_PCS), PortEnableBit);
  Stall (400);

  return EFI_SUCCESS;
}
