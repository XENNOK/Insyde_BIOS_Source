/** @file

  Definition of H2O Event Log String Overwrite protocol.
    
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EVENT_LOG_STRING_OVERWRITE_H_
#define _EVENT_LOG_STRING_OVERWRITE_H_

#include <Protocol/EventLogString.h>

/**
 Base on the Event information to get the event string.

 @param[in]   EventInfo       The event information for string translation.
 @param[out]  EventString     Event string returned.
 @param[out]  StringSize      Size of EventString.
 
 @retval EFI Status                  
*/
typedef
EFI_STATUS
(EFIAPI *H2O_EVENT_LOG_STRING_OVERWRITE_HANDLER) (
  IN VOID                                   *EventInfo,
  IN OUT CHAR16                             **EventString,
  IN OUT UINTN                              *StringSize
);

typedef struct _H2O_EVENT_LOG_STRING_OVERWRITE_PROTOCOL {
  H2O_EVENT_LOG_STRING_OVERWRITE_HANDLER                   EventLogStringOverwriteHandler;
} H2O_EVENT_LOG_STRING_OVERWRITE_PROTOCOL;


extern EFI_GUID gH2OEventLogStringOverwriteProtocolGuid;

#endif
