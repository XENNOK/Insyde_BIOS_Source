/** @file
Function definition for timer
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

#ifndef _H2O_FORM_BROWSER_TIMER_H_
#define _H2O_FORM_BROWSER_TIMER_H_

#define H2O_FORM_BROWSER_TIMER_ID_REFRESH_LIST    0

typedef enum {
  H2O_FORM_BROWSER_TIMER_TYPE_PERIODIC,
  H2O_FORM_BROWSER_TIMER_TYPE_RELATIVE,
  H2O_FORM_BROWSER_TIMER_TYPE_MAX
} H2O_FORM_BROWSER_TIMER_TYPE;

#define H2O_FORM_BROWSER_TIMER_INFO_SIGNATURE    SIGNATURE_32 ('H', 'F', 'T', 'I')

typedef struct _TIMERINFO {
   UINT32                                  Signature;
   LIST_ENTRY                              Link;
   INT32                                   Target;
   UINT32                                  TimerId;
   H2O_FORM_BROWSER_TIMER_TYPE             Type;
   H2O_DISPLAY_ENGINE_EVT                  *NotifyEvent;
   UINT64                                  TimeoutInNanoSec;
   UINT64                                  ClockExpiresInNanoSec;
 } TIMERINFO;

#define TIMERINFO_FROM_LINK(a)  CR((a), TIMERINFO, Link, H2O_FORM_BROWSER_TIMER_INFO_SIGNATURE)

EFI_STATUS
FBSetTimer (
  IN INT32                                 Target,
  IN UINT32                                TimerId,
  IN H2O_FORM_BROWSER_TIMER_TYPE           Type,
  IN CONST H2O_DISPLAY_ENGINE_EVT          *NotifyEvent,
  IN UINT64                                TimeoutInNanoSec
  );

#endif
