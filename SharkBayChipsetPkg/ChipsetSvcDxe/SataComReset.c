/** @file
 DXE Chipset Services Library.
 	
 This file contains only one function that is DxeCsSvcSataComReset().
 The function DxeCsSvcSataComReset() use chipset services to reset specified SATA port.
	
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
#include <Library/UefiBootServicesTableLib.h>
#include <PchAccess.h>
#include <PortNumberMap.h>
#include <SaAccess.h>

//#define ICH_ACPI_TIMER_MAX_VALUE      0x1000000 // The timer is 24 bit overflow
//#define ICH_ACPI_TIMER_ADDR           (FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x08)

// PCS-Port Control and Status Register. Address Offset: 92h-93h.
// Bit0:Port0 Enable-(R/W), Bit1:Port1 Enable-(R/W)
// Bit2:Port2 Enable-(R/W), Bit3:Port3 Enable-(R/W)
// Bit4:Port4 Enable-(R/W), Bit5:Port5 Enable-(R/W)
UINT16 mAhciPortEnableBit[6] = {BIT0, BIT1, BIT2, BIT3, BIT4, BIT5};
UINT16 mIdePortEnableBit[6]  = {BIT0, BIT1, BIT2, BIT3, BIT0, BIT1};

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
  UINT8                             SataMode;
  UINT32                            BusNumber;
  UINT32                            DeviceNumber;
  UINT32                            FunctionNumber;
  UINTN                             Index;
  UINTN                             NoPorts;
  PORT_NUMBER_MAP                   *PortNumberMapTable;   //retrieved from Pcd
  PORT_NUMBER_MAP                   EndEntry;
  UINT16                            PortEnableBit;

  BusNumber               = 0;
  DeviceNumber            = 0;
  FunctionNumber          = 0;
  PortEnableBit           = 0;
  PortNumberMapTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));
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
    PortNumberMapTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

    NoPorts = 0;
    while (CompareMem (&EndEntry, &PortNumberMapTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
      NoPorts++;
    }

    if (NoPorts == 0) {
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

  MmPci16And ( 0, BusNumber, DeviceNumber, FunctionNumber, R_PCH_SATA_PCS, ~PortEnableBit);
  gBS->Stall (400);
  MmPci16Or ( 0, BusNumber, DeviceNumber, FunctionNumber, R_PCH_SATA_PCS, PortEnableBit);
  gBS->Stall (400);

  return EFI_SUCCESS;
}
