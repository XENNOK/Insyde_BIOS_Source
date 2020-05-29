/** @file

  Delay for at least the request number of microseconds.

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
#ifndef _STALLLIB_H_
#define _STALLLIB_H_

/**
  Delay for at least the request number of microseconds. 

 @param[in]       parm1 Number of microseconds to delay.
 
 @retval none
*/
VOID
Stall (
  IN  UINTN   Microseconds
  );

#endif
