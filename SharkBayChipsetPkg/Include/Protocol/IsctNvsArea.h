/** @file

  Note:  Data structures defined in this protocol are not naturally aligned.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;*Copyright (c)  2012 Intel Corporation. All rights reserved
;*This software and associated documentation (if any) is furnished
;*under a license and may only be used or copied in accordance
;*with the terms of the license. Except as permitted by such
;*license, no part of this software or documentation may be
;*reproduced, stored in a retrieval system, or transmitted in any
;*form or by any means without the express written consent of
;*Intel Corporation.
;******************************************************************************

*/

#ifndef _ISCT_NVS_AREA_H_
#define _ISCT_NVS_AREA_H_

//
// Isct NVS Area Protocol GUID
//
#define ISCT_NVS_AREA_PROTOCOL_GUID \
  { \
    0x6614a586, 0x788c, 0x47e2, 0x89, 0x2d, 0x72, 0xe2, 0xc, 0x34, 0x48, 0x90 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gIsctNvsAreaProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _ISCT_NVS_AREA_PROTOCOL ISCT_NVS_AREA_PROTOCOL;


//
// Isct NVS Area definition
//
#pragma pack(1)
typedef struct {
  UINT8       IsctWakeReason;      //(0): Wake Reason
  UINT8       IsctEnabled;         //(1): 1 - Enabled, 0 - Disabled 
  UINT8       RapidStartEnabled;   //(2): IFFS service enabled
  UINT8       IsctTimerChoice;     //(3): 1 - RTC timer, 0 - EC timer 
  UINT32      EcDurationTime;      //(4): EC Duration Time
  UINT32      RtcDurationTime;     //(8): RTC Duration Time
  UINT32      IsctNvsPtr;          //(12): Ptr of Isct GlobalNvs
  UINT8       IsctOverWrite;       //(16): 1 - Isct , 0 - OS RTC
} ISCT_NVS_AREA;
#pragma pack()

//
// Isct NVS Area Protocol
//
typedef struct _ISCT_NVS_AREA_PROTOCOL {
  ISCT_NVS_AREA     *Area;
  VOID              *IsctData;  
} ISCT_NVS_AREA_PROTOCOL;

#endif
