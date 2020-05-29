/** @file

  Delay for at least the request number of microseconds.  Timer used is DMA refresh timer, which
  has 15us granularity. You can call with any number of microseconds, but this implementation 
  cannot support 1us granularity.

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

/**
  Delay for at least the request number of microseconds.  Timer used is DMA refresh timer, which
  has 15us granularity. You can call with any number of microseconds, but this implementation 
  cannot support 1us granularity.

 @param[in]       parm1 Number of microseconds to delay.
 
 @retval none
*/

#include <Library/IoLib.h>

VOID
Stall (
  IN  UINTN   Microseconds
  )
{
  UINT8 Data;
  UINT8 InitialState;
  UINTN CycleIterations;

  CycleIterations = 0;
  Data            = 0;
  InitialState    = 0;

  //
  // The time-source is 15 us granular, so calibrate the timing loop
  // based on this baseline
  // Error is possible 15us.
  //
  CycleIterations = (Microseconds / 15) + 1;

  //
  // Use the DMA Refresh timer in port 0x61.  Cheap but effective.
  // The only issue is that the granularity is 15us, and we want to
  // guarantee "at least" one full transition to avoid races.
  //
  //
  //   _____________/----------\__________/--------
  //
  //                |<--15us-->|
  //
  // --------------------------------------------------> Time (us)
  //
  while (CycleIterations--) {
    Data = IoRead8 (0x61);
    InitialState = Data;

    //
    // Capture first transition (strictly less than one period)
    //
    while (InitialState == Data) {
      Data = IoRead8 (0x61);
    }

    InitialState = Data;
    //
    // Capture next transition (guarantee at least one full pulse)
    //
    while (InitialState == Data) {
      Data = IoRead8 (0x61);
    }
  }
}
