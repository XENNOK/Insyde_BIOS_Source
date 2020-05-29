/** @file

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

 LegacySpeaker.h

@brief:

  Speaker enabling related data

**/

#ifndef _DXE_EFI_LEGACY_SPEAKER_H
#define _DXE_EFI_LEGACY_SPEAKER_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Protocol/Speaker.h>

//
// Speaker Related Port Information
//
#define EFI_TIMER_COUNTER_PORT      0x40
#define EFI_TIMER_CONTROL_PORT      0x43
#define EFI_TIMER_2_PORT            0x42
#define EFI_SPEAKER_CONTROL_PORT    0x61

#define EFI_SPEAKER_OFF_MASK        0xFC

#define EFI_DEFAULT_BEEP_FREQUENCY  0x500

//
// Default Intervals/Beep Duration
//
#define EFI_DEFAULT_LONG_BEEP_DURATION  0x70000
#define EFI_DEFAULT_SHORT_BEEP_DURATION 0x50000
#define EFI_DEFAULT_BEEP_TIME_INTERVAL  0x20000

EFI_STATUS
EFIAPI
InitializeSpeakerInterfaceEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

#endif
