/** @file

   Watch Dog library functions and definitions.

  This library provides basic KSC interface.  It is deemed simple enough and uses in 
  so few cases that there is not currently benefit to implementing a protocol.
  If more consumers are added, it may be benefitial to implement as a protocol.

  There may be different libraries for different environments (PEI, BS, RT, SMM).
  Make sure you meet the requirements for the library (protocol dependencies, use

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

#ifndef _WATCH_DOG_LIB_H_
#define _WATCH_DOG_LIB_H_

#define WATCH_DOG_DIRTY_BIT 0x80
#define WATCH_DOG_ERROR     0xC0

typedef enum {
  START_WATCH_DOG = 0,
  SEC_WATCH_DOG,
  PEI_WATCH_DOG,
  DXE_WATCH_DOG,
  SMM_WATCH_DOG,
  BDS_WATCH_DOG,
  BDS_CONNECT_WATCH_DOG,
  OPROM_WATCH_DOG,
  OS_WATCH_DOG,
  END_WATCH_DOG
} TCO_WATCHDOG_TYPE;

VOID
WatchDogStart (
  TCO_WATCHDOG_TYPE WatchDogType
  );

VOID
WatchDogStop (
  VOID
  );

VOID
WatchDogSetTimer (
  TCO_WATCHDOG_TYPE WatchDogType
  );

VOID
WatchDogMessageClear (
  VOID
  );

VOID
WatchDogMessageGet (
  UINT8 *Message1,
  UINT8 *Message2
  );


#endif
