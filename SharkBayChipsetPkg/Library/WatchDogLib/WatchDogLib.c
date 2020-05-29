/** @file

  Watch Dog library implementation.

  These functions need to be SMM safe.
  
  These functions require the SMM IO library to be present.
  Caller must link those libraries and have the proper include path.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PchAccess.h>
#include <PchRegsLpc.h>
#include <Library/WatchDogLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>


//
//  TCO Timer Initial Value
//  Values of 0000h or 0001h will be ignored and should not be attempted. The timer is 
//  clocked at approximately 0.6 seconds, and thus allows timeouts ranging from 1.2 second to 
//  613.8 seconds. 
//

UINT16 
TcoWatchDogTable [] = {
0xFFFF, //start
0x03E8, //sec
0x03E8, //pei
0x03E8, //dxe
0x03E8, //smm
0x03E8, //bds
0x03E8, //bds connect
0x03E8, //oprom
0x03E8, //os
0xFFFF  //end
};

VOID
WatchDogStop (
  VOID
  )
{

  UINT16 pmbase = 0;
  UINT16 Data16 = 0;
  UINT8  Message1 = 0;
  
  if (FeaturePcdGet(PcdH2ODdtSupported)) {
  //  return;
  }

  if (!FeaturePcdGet (PcdH2OWatchDogSupported)) {
    return;
  }

  pmbase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;

  Message1 = IoRead8 (pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE1);

  //
  // if error rest, do not start/stop watch dog until log complete
  //
  if (Message1 == WATCH_DOG_ERROR) {
    return;
  }

  Data16 = IoRead16 (pmbase + PCH_TCO_BASE + R_PCH_TCO1_CNT);
  
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  IoWrite16(pmbase + PCH_TCO_BASE + R_PCH_TCO1_CNT, Data16);

  WatchDogMessageClear ();
  
  MmioOr32 (PcdGet32 (PcdRcbaBaseAddress)+ R_PCH_RCRB_GCS, (UINT32) (B_PCH_RCRB_GCS_NR));
}

VOID
WatchDogSetTimer (
  TCO_WATCHDOG_TYPE WatchDogType
  )
{
  UINT16 pmbase       = 0;
  UINT16 WachDogTimer = 0;
  
  if (FeaturePcdGet(PcdH2ODdtSupported)) {
  //  return;
  }
  
  if (!FeaturePcdGet (PcdH2OWatchDogSupported)) {
    return;
  }

  if (WatchDogType > END_WATCH_DOG) {
    return;
  }

  pmbase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;

  WachDogTimer = TcoWatchDogTable[WatchDogType];
  
  IoWrite16(pmbase + PCH_TCO_BASE + R_PCH_TCO_TMR, WachDogTimer);
  IoWrite16(pmbase + PCH_TCO_BASE + R_PCH_TCO_RLD, WachDogTimer);
}

VOID
WatchDogStart (
  TCO_WATCHDOG_TYPE WatchDogType
  )
{
  UINT16 pmbase   = 0;
  UINT16 Data16   = 0;
  UINT8  Message1 = 0;

  if (FeaturePcdGet(PcdH2ODdtSupported)) {
    return;
  }

  if (!FeaturePcdGet (PcdH2OWatchDogSupported)) {
    return;
  }

  if (WatchDogType > END_WATCH_DOG) {
    return;
  }

  if ((PcdGet8(PcdH2OWatchDogMask) & 0x01) == 0) {
    WatchDogStop();
    return;
  }

  pmbase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;
  
  Message1 = IoRead8 (pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE1);

  if ((Message1 == WATCH_DOG_DIRTY_BIT) && WatchDogType <= SEC_WATCH_DOG){
    //
    // Dirty bit exist, set error reset bit.
    //
    IoWrite8(pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE1, 0xC0);
    
    Data16 = IoRead16 (pmbase + PCH_TCO_BASE + R_PCH_TCO1_CNT);
    Data16 |= B_PCH_TCO_CNT_TMR_HLT;
    IoWrite16(pmbase + PCH_TCO_BASE + R_PCH_TCO1_CNT, Data16);

    MmioOr32 (PcdGet32 (PcdRcbaBaseAddress)+ R_PCH_RCRB_GCS, (UINT32) (B_PCH_RCRB_GCS_NR));
    return;
  }


  WatchDogSetTimer (WatchDogType);

  MmioAnd32 (PcdGet32 (PcdRcbaBaseAddress)+ R_PCH_RCRB_GCS, (UINT32) (~B_PCH_RCRB_GCS_NR));

  Data16 = IoRead16 (pmbase + PCH_TCO_BASE + R_PCH_TCO1_CNT);
  
  Data16 &= ~B_PCH_TCO_CNT_TMR_HLT;
  IoWrite16(pmbase + PCH_TCO_BASE + R_PCH_TCO1_CNT, Data16);

  //
  // Message1 bit7 ==> DirtyBit
  // Message1 bit6 ==> Error Reset Flag 
  // Message2      ==> Watch Dog Phase
  //
  //
  // if error rest, do not start/stop watch dog until log complete
  // error bit may be set in sec phase
  //
  //
  
  if (Message1 == WATCH_DOG_ERROR){
    return;
  }else {
    IoWrite8(pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE1, 0x80);
    IoWrite8(pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE2, (UINT8)WatchDogType);
  }
}

VOID
WatchDogMessageClear (
  VOID
  )
{
  UINT16 pmbase   = 0;

  pmbase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;

  IoWrite8(pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE1, 0x00);
  IoWrite8(pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE2, 0x00);

}

VOID
WatchDogMessageGet (
  UINT8 *Message1,
  UINT8 *Message2
  )
{
  UINT16 pmbase   = 0;

  pmbase = PchLpcPciCfg16 (R_PCH_LPC_ACPI_BASE) & 0xFFFE;

  *Message1 = IoRead8 (pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE1);
  *Message2 = IoRead8 (pmbase + PCH_TCO_BASE + R_PCH_TCO_MESSAGE2);
}
