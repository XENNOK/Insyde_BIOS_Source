/** @file
  MonitorKeyFilter protocol header file

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

#ifndef _MONITOR_KEY_FILTER_H_
#define _MONITOR_KEY_FILTER_H_

#define EFI_MONITOR_KEY_FILTER_PROTOCOL_GUID \
  {0x9e43e128, 0xc74d, 0x42f4, 0x8c, 0xc, 0x48, 0x11, 0x67, 0x4a, 0x17, 0xb5}

typedef struct _EFI_MONITOR_KEY_FILTER_PROTOCOL   EFI_MONITOR_KEY_FILTER_PROTOCOL;

/**
  Returns a bit map of which, if any, of the monitored keys were seen.
  The internal equivalent of KeyDetected is set to zero after each invocation.

  @param  This              Indicates the calling context.
  @param  KeyDetected       A bit map of the monitored keys found.
                            Bit N corresponds to KeyList[N] as provided by the
                            GetUsbPlatformOptions() API.
							
  @retval EFI_SUCCESS       Key detected status is returned successfully.
  @retval Other             No keys detected.

**/
typedef
EFI_STATUS
(EFIAPI *EFI_GET_MONITORED_KEYS) (
  IN  EFI_MONITOR_KEY_FILTER_PROTOCOL   *This,
  OUT UINT32	                        *KeyDetected
  );

struct _EFI_MONITOR_KEY_FILTER_PROTOCOL {
  EFI_GET_MONITORED_KEYS            GetMonitoredKeys;
};

extern EFI_GUID gEfiMonitorKeyFilterProtocolGuid;

#endif
