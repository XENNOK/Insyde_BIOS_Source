/** @file
Implementation of SmbusLib Library Class
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
/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Intel ICH9 SMBUS library implementation built upon I/O library.

  Copyright (c) 2010, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/
#include <Uefi.h>
#include <SaAccess.h>
#include <Library/TbtLib.h>
//[-start-130221-IB03780481-add]//
#include <PchAccess.h>
//[-end-130221-IB03780481-add]//
//[-start-130425-IB05160441-add]//
#include <Library/PciLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
//[-end-130425-IB05160441-add]//

//[-start-130221-IB03780481-modify]//
//Platform dependent way to stall for Usec
VOID
TbtStall (UINTN Usec)
{
  UINTN   Ticks;
  UINTN   Counts;
  UINTN   CurrentTick;
  UINTN   OriginalTick;
  UINTN   RemainingTick;
  UINT16  AcpiBaseAddr;

  if (Usec == 0) {
    return;
  }
  ///
  /// Please use PciRead here, it will link to MmioRead
  /// if the caller is a Runtime driver, please use PchDxeRuntimePciLibPciExpress library, refer
  /// PciExpressRead() on Library\DxeRuntimePciLibPciExpress\DxeRuntimePciLibPciExpress.c for the details.
  /// For the rest please use EdkIIGlueBasePciLibPciExpress library
  ///
  AcpiBaseAddr = PciRead16 (
                  PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                  PCI_DEVICE_NUMBER_PCH_LPC,
                  PCI_FUNCTION_NUMBER_PCH_LPC,
                  R_PCH_LPC_ACPI_BASE)
                  ) & B_PCH_LPC_ACPI_BASE_BAR;

  OriginalTick  = IoRead32 ((UINTN) (AcpiBaseAddr + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;
  CurrentTick   = OriginalTick;

  ///
  /// The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
  ///
  Ticks = Usec * 358 / 100 + OriginalTick + 1;

  ///
  /// The loops needed by timer overflow
  ///
  Counts = Ticks / V_PCH_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// Remaining clocks within one loop
  ///
  RemainingTick = Ticks % V_PCH_ACPI_PM1_TMR_MAX_VAL;

  ///
  /// not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
  /// one I/O operation, and maybe generate SMI
  ///
  while ((Counts != 0) || (RemainingTick > CurrentTick)) {
    CurrentTick = IoRead32 ((UINTN) (AcpiBaseAddr + R_PCH_ACPI_PM1_TMR)) & B_PCH_ACPI_PM1_TMR_VAL;
    ///
    /// Check if timer overflow
    ///
    if ((CurrentTick < OriginalTick)) {
      if (Counts != 0) {
        Counts--;
      } else {
        ///
        /// If timer overflow and Counts equ to 0, that means we already stalled more than
        /// RemainingTick, break the loop here
        ///
        break;
      }
    }

    OriginalTick = CurrentTick;
  }
}
//[-end-130221-IB03780481-modify]//

#define PCIE2TBT_R 0x54C
#define TBT2PCIE_R 0x548
#define PCIE2TBT_VLD_B 1
#define TBT2PCIE_DON_R 1


//[-start-130220-IB03780481-modify]//
BOOLEAN
TbtSetPCIe2TBTCommand (
	IN	UINT8		command,    // command to apply
	IN	UINT8		data,       // command specific data
  IN  UINT8   TBT_US_BUS,   // Primary bus number of RR host router upstream port
  OUT UINT32  *Tbt2PcieData // return data from TBT2PCIE register 
  )
{
	UINT32 REG_VAL;
	UINT32 max_wait_Iter = 50; // Wait 5 sec

	MmPci32 (0x00, TBT_US_BUS, 0x00, 0x00, PCIE2TBT_R) = (data << 8) | (command << 1) | PCIE2TBT_VLD_B;

	while(max_wait_Iter-- > 0)
	{
		REG_VAL = MmPci32(0x00, TBT_US_BUS, 0x00, 0x00, TBT2PCIE_R);
		if(0xFFFFFFFF == REG_VAL)
		{
			// Device is not here return now
			return FALSE;
		}
		if(REG_VAL & TBT2PCIE_DON_R)
		{
		  *Tbt2PcieData = REG_VAL;
			break;
		}
		TbtStall(100 * 1000);
	}
	MmPci32 (0x00, TBT_US_BUS, 0x00, 0x00, PCIE2TBT_R) = 0;
	return TRUE;
}
//[-end-130220-IB03780481-modify]//

//[-start-130220-IB03780481-modify]//
BOOLEAN
TbtWaitForFastLink (
	IN	UINT8		TBT_US_BUS  // Primary bus number of RR host router upstream port
  )
{
	UINT32 REG_VAL;
	UINT32 max_wait_Iter = 50; // Wait 5 sec

  REG_VAL = MmPci32(0x00, TBT_US_BUS, 0x00, 0x00, TBT2PCIE_R);
  if(0xFFFFFFFF == REG_VAL)
  {
    // Device is not here return now
    return FALSE;
  }

  MmPci32 (0x00, TBT_US_BUS, 0x00, 0x00, PCIE2TBT_R) = (0x04 << 1) | PCIE2TBT_VLD_B;

  while(max_wait_Iter-- > 0)
  {
    REG_VAL = MmPci32(0x00, TBT_US_BUS, 0x00, 0x00, 0x00);
    if(0xFFFFFFFF != REG_VAL)
    {
      break;
    }
    TbtStall(100 * 1000);
  }
  MmPci32 (0x00, TBT_US_BUS, 0x00, 0x00, PCIE2TBT_R) = 0;

  return TRUE;
}
//[-end-130220-IB03780481-modify]//

