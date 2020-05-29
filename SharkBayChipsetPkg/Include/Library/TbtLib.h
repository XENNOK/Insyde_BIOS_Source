/** @file

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

/** 
  Internal header file for Stoutland SEC Platform hook library.

  Copyright (c)  2008, Intel Corporation. All rights reserved. <BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.


**/

#ifndef _TBT_LIB_H_
#define _TBT_LIB_H_

//
// PCIE to TBT command definition
//
#define GO2SX                      0x02
#define GO2SX_NO_WAKE              0x03
#define SX_EXIT_TBT_CONNECTED      0x04
#define SX_EXIT_NO_TBT_CONNECTED   0x05
//[-start-130220-IB03780481-add]//
#define OS_UP                      0x06
//[-end-130220-IB03780481-add]//
#define SET_SECURITY_LEVEL         0x08
#define GET_SECURITY_LEVEL         0x09

#define SIO_SAMPLE_IO_BASE                                  0x500
#define GPIO_SET_1                                          0
#define GPIO_SET_2                                          1
#define GPIO_SET_3                                          2
#define GPIO_SET_4                                          3
#define GPIO_SET_5                                          4
#define GPIO_SET_6                                          5

//[-start-130226-IB03780481-add]//
//
// This depend on Platform design
//
#define TBT_PWR_EN_GPIO40                                    0x1
#define InputMode                                            0
#define OutputMode                                           1
#define HIGH                                                 1
#define LOW                                                  0
#define SIO_LOCK                                             1
#define SIO_UNLOCK                                           0
//[-end-130226-IB03780481-add]//

//[-start-130220-IB03780481-modify]//
BOOLEAN
TbtSetPCIe2TBTCommand (
  IN  UINT8   command,      // command to apply
  IN  UINT8   data,         // command specific data
  IN  UINT8   TBT_US_BUS,   // Primary bus number of RR host router upstream port
  OUT UINT32  *Tbt2PcieData // return data from TBT2PCIE register 
  );
//[-end-130220-IB03780481-modify]//

BOOLEAN
TbtWaitForFastLink (
    IN  UINT8       TBT_US_BUS  // Primary bus number of RR host router upstream port
  );

//[-start-130205-IB03780481-add]//
VOID
TbtSetGpio3 (
  IN  BOOLEAN          PullHigh
  );

VOID
SetTBTGPIO9(
  IN BOOLEAN           Output,
  IN BOOLEAN           High,
  IN BOOLEAN           SIOLock
);

VOID
EnterTbtSxState (
  IN UINT8             TbtWakeFromDevice
  );
//[-end-130205-IB03780481-add]//

#endif
