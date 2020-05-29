/** @file

  Framework PEIM to provide WdttPei.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _WDTT_PEI_H_
#define _WDTT_PEI_H_

#include <Ppi/Wdtt/Wdtt.h>
#include <Ppi/CpuIo.h>
#include <Library/DebugLib.h>

#define MAX_TIMER_COUNT             0x3FF
#define MIN_TIMER_COUNT             0x2
#define DEFAULT_TIMER_COUNT         0xA      // 10*0.6 sec

typedef struct {
  UINT16    TcoTmr    :10;
  UINT16    TcoEn     :1;
  UINT16    NoReboot  :1;
  UINT16    Reserved  :4;
} TCO_PRESERVE_REGS;

#ifndef BIT0
#define  BIT0  1
#define  BIT1  (1 << 1)
#define  BIT2  (1 << 2)
#define  BIT3  (1 << 3)
#define  BIT4  (1 << 4)
#define  BIT5  (1 << 5)
#define  BIT6  (1 << 6)
#define  BIT7  (1 << 7)
#define  BIT8  (1 << 8)
#define  BIT9  (1 << 9)
#define  BIT10 (1 << 10)
#define  BIT11 (1 << 11)
#define  BIT12 (1 << 12)
#define  BIT13 (1 << 13)
#define  BIT14 (1 << 14)
#define  BIT15 (1 << 15)
#define  BIT16 (1 << 16)
#define  BIT17 (1 << 17)
#define  BIT18 (1 << 18)
#define  BIT19 (1 << 19)
#define  BIT20 (1 << 20)
#define  BIT21 (1 << 21)
#define  BIT22 (1 << 22)
#define  BIT23 (1 << 23)
#define  BIT24 (1 << 24)
#define  BIT25 (1 << 25)
#define  BIT26 (1 << 26)
#define  BIT27 (1 << 27)
#define  BIT28 (1 << 28)
#define  BIT29 (1 << 29)
#define  BIT30 (1 << 30)
#define  BIT31 (1 << 31)
#endif


//
// Ppi private data
//
#define PEI_WATT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('p', 'w', 'a', 't')

typedef struct {
  UINTN                                 Signature;
  EFI_PEI_CPU_IO_PPI                        *CpuIo;
  PEI_WDTT_PPI                          WdttPpi;
  EFI_PEI_PPI_DESCRIPTOR                PpiDescriptor;
  UINT8                                 WatchdogRunningFlag;
  TCO_PRESERVE_REGS                     TcoPreserveRegs;
} PEI_WATT_PRIVATE_DATA;

#define PEI_WATT_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
      PEI_WATT_PRIVATE_DATA, \
      WdttPpi, \
      PEI_WATT_PRIVATE_DATA_SIGNATURE \
    )

//
// Prototypes
//

/**
  
  Init TCO Timer and preserve original setting.
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  TimerInitValue         Init Value for TCO Timer

  @retval EFI_SUCCESS             The function completed successfully.
  @retval !EFI_SUCCESS            The PEIM failed to execute.

**/
EFI_STATUS
PrepareTCO (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     UINT16                     TimerInitValue    OPTIONAL
  );

/**
  
  Recover TCO Timer setting
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi

  @retval EFI_SUCCESS             The function completed successfully.
  @retval !EFI_SUCCESS            The PEIM failed to execute.

**/
EFI_STATUS
RecoverTCO (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This
  );

/**
  
  Start TCO Timer
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status
  @param  TimerInitValue         Init Value for TCO Timer

  @retval EFI_SUCCESS                    The function completed successfully.
  @retval EFI_ALREADY_STARTED     The Watchdog is running now.

**/
EFI_STATUS
StartWDT (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag,
  IN     UINT16                     TimerInitValue    OPTIONAL
  );

/**
  
  Stop TCO Timer
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status

  @retval EFI_SUCCESS                    The function completed successfully.

**/
EFI_STATUS
StopWDT (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  );

/**
  
  Check Watchdog reboot has occured or not
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status

  @retval WdttRunning             Watchdog is running.
  @retval WdttTriggered           Watchdog has reboot system in last POST.
  @retval WdttNormalBoot        Current POST is normally boot.

**/
PEI_WDTT_STATUS
CheckWdtStatus (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  );

/**
  
  Clear Watchdog status
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status

  @retval EFI_SUCCESS                   The function completed successfully.
  @retval EFI_ACCESS_DENIED        Access denied, because of watchdog is running.

**/
EFI_STATUS
ClearWdtStatus (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  );

/**
  
  Do memory initialization.
  
  @param  FfsHeader              Not used.
  @param  PeiServices             General purpose services available to every PEIM.

  @retval EFI_SUCCESS          Memory initialization completed successfully.
  All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
WdttEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  );
#endif 
