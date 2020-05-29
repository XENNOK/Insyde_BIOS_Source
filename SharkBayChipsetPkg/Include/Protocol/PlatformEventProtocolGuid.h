/** @file
 PlatformEventProtocolGuid.h

 Platform Event Protocol GUID definition file.

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

#ifndef _PLATFORM_EVENT_PROTOCOL_GUID_H_
#define _PLATFORM_EVENT_PROTOCOL_GUID_H_


//
// Start/End of Shadow Rom
//
#define START_OF_SHADOW_ROM_PROTOCOL_GUID \
  {0xE85EA9C5, 0x8525, 0x4363, 0x84, 0x70, 0xB1, 0x75, 0x63, 0x01, 0x53, 0x8E}

extern EFI_GUID gStartOfShadowRomProtocolGuid;

#define END_OF_SHADOW_ROM_PROTOCOL_GUID \
  {0x72DAD631, 0x376E, 0x4cfa, 0xB5, 0x73, 0x6A, 0x16, 0x52, 0xA8, 0x41, 0x10}

extern EFI_GUID gEndOfShadowRomProtocolGuid;


//
// Start/End of Front Page
//
#define START_OF_FRONT_PAGE_PROTOCOL_GUID \
  {0xF5D329EF, 0x355B, 0x42e9, 0x8F, 0x0F, 0xFF, 0xBD, 0xC2, 0x94, 0xE9, 0x25}

extern EFI_GUID gStartOfFrontPageProtocolGuid;

#define END_OF_FRONT_PAGE_PROTOCOL_GUID \
  {0x34DAA774, 0x1417, 0x46a9, 0x92, 0x40, 0xC1, 0xB6, 0xA3, 0xB9, 0xE3, 0x46}

extern EFI_GUID gEndOfFrontPageProtocolGuid;


//
// Start/End of Setup Utility
//
#define START_OF_SETUP_UTILITY_PROTOCOL_GUID \
  {0x88D470E2, 0xD173, 0x4ee7, 0x81, 0x23, 0x70, 0x57, 0x26, 0xDA, 0x08, 0x32}

extern EFI_GUID gStartOfSetupUtilityProtocolGuid;

#define END_OF_SETUP_UTILITY_PROTOCOL_GUID \
  {0xDDE0BC2D, 0x0191, 0x46e4, 0x86, 0xCD, 0x1A, 0xC2, 0x42, 0xDE, 0xA3, 0xD1}

extern EFI_GUID gEndOfSetupUtilityProtocolGuid;


#endif

