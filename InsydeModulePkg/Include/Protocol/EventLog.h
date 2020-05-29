/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Copyright (c)  1999 - 2003 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    LogEvent.h

Abstract:

    Log Event include file

Revision History

--*/
#ifndef _EVENT_LOG_PROTO_H_
#define _EVENT_LOG_PROTO_H_

//#include "Tiano.h"
//#include EFI_GUID_DEFINITION (DataHubRecords)
//#include EFI_GUID_DEFINITION (EventLogHob)

#define EFI_EVENT_LOG_PROTOCOL_GUID \
  { \
    0x6a73deeb, 0x63c0, 0x49c6, 0xa5, 0xd4, 0x94, 0xcf, 0xe0, 0x10, 0xf, 0xe3 \
  }

//EFI_FORWARD_DECLARATION (EFI_EVENT_LOG_PROTOCOL);
typedef struct  _EFI_EVENT_LOG_PROTOCOL EFI_EVENT_LOG_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_EVENT_LOGDETECT_DEVICE) (
  IN EFI_EVENT_LOG_PROTOCOL             *This,
  OUT UINT8                             *Buffer
  )
;

typedef
EFI_STATUS
(EFIAPI *EFI_EVENT_LOG_WRITE) (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  IN  UINT8                             EventLogType,
  IN  UINT32                            PostBitmap1,
  IN  UINT32                            PostBitmap2,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_EVENT_LOG_CLEAR) (
  IN  EFI_EVENT_LOG_PROTOCOL            *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_EVENT_LOG_READ_NEXT) (
  IN  EFI_EVENT_LOG_PROTOCOL            *This,
  IN OUT VOID                           **EventListAddress
  )
;

//
// SMM RUNTIME PROTOCOL
//
struct _EFI_EVENT_LOG_PROTOCOL {
  EFI_EVENT_LOGDETECT_DEVICE             DetectDevice;
  EFI_EVENT_LOG_WRITE                     Write;
  EFI_EVENT_LOG_CLEAR                     Clear;
  EFI_EVENT_LOG_READ_NEXT                 ReadNext;
};

extern EFI_GUID gEfiEventLogProtocolGuid;

#endif
