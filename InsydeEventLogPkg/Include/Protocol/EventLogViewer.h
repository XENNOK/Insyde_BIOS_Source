/** @file

  Definition of Event Log Viewer protocol.
    
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


#ifndef _EVENT_LOG_VIEWER_PROTOCOL_H_
#define _EVENT_LOG_VIEWER_PROTOCOL_H_

typedef struct _EVENT_LOG_VIEWER_PROTOCOL EVENT_LOG_VIEWER_PROTOCOL;

/**
 Excute the utility of Event Log Viewer.

 @param[in]   This   This interface.
 
 @retval EFI Status                  
*/
typedef
EFI_STATUS
(EFIAPI *EXECUTE_EVENT_LOG_VIEWER) (
  IN  EVENT_LOG_VIEWER_PROTOCOL           *This
);

struct _EVENT_LOG_VIEWER_PROTOCOL {
  EXECUTE_EVENT_LOG_VIEWER                      ExecuteEventLogViewer;
};

extern EFI_GUID gH2OEventLogViewerProtocolGuid;

#endif
