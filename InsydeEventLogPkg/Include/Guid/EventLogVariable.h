/** @file

   The definition of Event Log Variable.

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


#ifndef _EVENT_LOG_VARIABLE_H_
#define _EVENT_LOG_VARIABLE_H_

#include <Protocol/BiosEventLog.h>

extern EFI_GUID gH2OEventLogVariableGuid;

#pragma pack(1)

typedef struct _H2O_EVENT_LOG_VARIABLE {
  BOOLEAN                   SmmEventLogReady;                        
} H2O_EVENT_LOG_VARIABLE;

#pragma pack()

#endif
