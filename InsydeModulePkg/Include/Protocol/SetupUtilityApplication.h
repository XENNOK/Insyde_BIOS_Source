/** @file
  Protocol for describing Setup Application..

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

#ifndef _SETUP_UTILITY_APPLICATION_PROTOCOL_H_
#define _SETUP_UTILITY_APPLICATION_PROTOCOL_H_

#define SETUP_UTILITY_APPLICATION_PROTOCOL_GUID  \
  { 0xe7a97370, 0x00ff, 0x4bba, 0xbb, 0xa0, 0xd0, 0xbc, 0x20, 0x0c, 0xce, 0x98 }

typedef struct _EFI_SETUP_UTILITY_APPLICATION_PROTOCOL   EFI_SETUP_UTILITY_APPLICATION_PROTOCOL;

typedef enum {
  InitializeSetupUtility,
  ShutdownSetupUtility,
  VfrDriverStateMax
} VFR_DRIVER_STATE;

struct _EFI_SETUP_UTILITY_APPLICATION_PROTOCOL {
  VFR_DRIVER_STATE                 VfrDriverState;
};

extern EFI_GUID gEfiSetupUtilityApplicationProtocolGuid;

#endif
