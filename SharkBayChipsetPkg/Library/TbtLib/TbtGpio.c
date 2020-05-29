/** @file
Implementation of SmbusLib Library Class
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Library/IoLib.h>
#include <Numbers.h>
#include <Library/TbtLib.h>

VOID
TbtSetGpio3 (
  IN  BOOLEAN          PullHigh
  )
/*++

Routine Description:

  Program Thunderbolt GPIO3
  
Arguments:

  PullHigh: TURE = Pull High, FALSE = Pull Low.

Returns:

  None

--*/  
{
  UINT8        GpioReg8;

  if (PullHigh) {
    //
    // Pull high for TBT GPIO 3 - SIO/EC GPIO 21
    //
    GpioReg8 = IoRead8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2);
    GpioReg8 |= BIT1;
    IoWrite8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2, GpioReg8);
  } else {
    //
    // Pull low for TBT GPIO 3 - SIO/EC GPIO 21
    //
    GpioReg8 = IoRead8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2);
    GpioReg8 &= ~BIT1;
    IoWrite8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2, GpioReg8);
  }

  return;
}

VOID
EnterTbtSxState (
  IN UINT8            TbtWakeFromDevice
  )
/*++

Routine Description:

  When enable Wake from TBT device then make TBT to enter Sx state 
  When disable Wake from TBT device then power off CR chip/ Pull down GPIO9
  
Arguments:

  TbtWakeFromDevice: 0:disable wake from TBT device, 1: enable wake from TBT device

Returns:

  None


--*/  
{
  UINT8   GpioReg8;

  if (TbtWakeFromDevice) {
    //
    // Asserted TBT GPIO 2 GO2SX - SIO/EC GPIO 20
    //
    GpioReg8 = IoRead8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2);
    GpioReg8 |= BIT0;
    IoWrite8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2, GpioReg8);
    //
    // BIOS should poll GPIO 9 OK2GO2SX_N_OD pin -  - SIO/EC GPIO 22 
    // Upon completion of all preparations, Host Router will assert OK2GO2SX_N_OD pin to
    // indicate readiness for Sx entry
    //
    do
    {
      GpioReg8 = IoRead8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2);
    } while((GpioReg8 & BIT2) == 0);
  } else {
    if (TBT_PWR_EN_GPIO40) {
      //
      // Pull Low TBT CR HR PWR - SIO/EC GPIO 40 to Power Off CR
      // SIO GPIO Set#4 base address, BIT 0 for GPIO 40
      //
      GpioReg8 = IoRead8(SIO_SAMPLE_IO_BASE + GPIO_SET_4) | BIT0;  
      IoWrite8 (SIO_SAMPLE_IO_BASE + GPIO_SET_4, GpioReg8); 
    } else {
       SetTBTGPIO9 (OutputMode, LOW, SIO_LOCK);
    }
  }

  return;
}

VOID
SetTBTGPIO9(
  IN BOOLEAN            Output,
  IN BOOLEAN            High,
  IN BOOLEAN            SIOLock
)
/*++

Routine Description:

  Set TBT GPIO 9 
  1.Input Mode
  2.Output Mode and set High/Low 

Arguments:

  Output:   TURE : Output Mode, FALSE : Input Mode
  High:     TURE : Pull High  , FALSE : Pull Low
  SIOLock:  TURE : Lock SIO Config space when exit, FALSE : Do not Lock SIO config space when exit

Returns:

  None


--*/
{
UINT8                        SioGpioValue;

  if ( IoRead8(0x2E) == 0xFF ) {
      //
      // Enter Config Mode
      //
      IoWrite8 (0x2E, 0x87);
      IoWrite8 (0x2E, 0x55);
      IoWrite8 (0x2E, 0x01);
      IoWrite8 (0x2E, 0x01);
    }
  //
  // Set LND 07
  //
  IoWrite8 (0x2E, 0x07);
  IoWrite8 (0x2F, 0x07);
 
  if (Output) {
    //
    // TBT GPIO 9 (SIO GPIO 22, SET 2, BIT 2) to Output  
    //
    IoWrite8 (0x2E, 0xC9);
    SioGpioValue = IoRead8 (0x2F) | BIT2;
    IoWrite8 (0x2F, SioGpioValue); 
    if (High) {
      //
      // Pull High GPIO_9__OK2GO2SX_N_OD - SIO/EC GPIO 22
      // EC GPIO Set#2 base address, BIT2 for GPIO22
      //
      SioGpioValue = IoRead8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2) | BIT2;  
      IoWrite8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2, SioGpioValue );    
    } else {
      //
      // Pull Low GPIO_9__OK2GO2SX_N_OD - SIO/EC GPIO 22
      // EC GPIO Set#2 base address, BIT2 for GPIO22
      //
      SioGpioValue = IoRead8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2)& ~BIT2;  
      IoWrite8 (SIO_SAMPLE_IO_BASE + GPIO_SET_2, SioGpioValue );      
    }
  } else {
    //
    // TBT GPIO 9 (SIO GPIO 22, SET 2, BIT 2) to Input  
    //
    IoWrite8 (0x2E, 0xC9);
    SioGpioValue = IoRead8 (0x2F) & ~BIT2;
    IoWrite8 (0x2F, SioGpioValue);   
  }
  if (SIOLock) {
   //
   // Exit Config Mode
   //
   IoWrite8 (0x2E, 0x02);
   SioGpioValue = IoRead8 (0x2F) | BIT1;
   IoWrite8 (0x2F, SioGpioValue);    
  }

}
