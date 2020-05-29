//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reservedb
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Speaker.h

Abstract:

  EFI Speaker Interface Protocol

Revision History

--*/

#ifndef _EFI_SPEAKER_H
#define _EFI_SPEAKER_H

//
// Global Id for Speaker Interface
//
#define EFI_SPEAKER_INTERFACE_PROTOCOL_GUID \
  { \
    0x400b4476, 0x3081, 0x11d6, 0x87, 0xed, 0x00, 0x06, 0x29, 0x45, 0xc3, 0xb9 \
  }

EFI_FORWARD_DECLARATION (EFI_SPEAKER_IF_PROTOCOL);

//
// Beep Code
//
typedef
EFI_STATUS
(EFIAPI *EFI_GENERATE_BEEP) (
  IN EFI_SPEAKER_IF_PROTOCOL                * This,
  IN     UINTN                              NumberOfBeep,
  IN     UINTN                              BeepDuration,
  IN     UINTN                              TimeInterval
  );

//
// Set Frequency
//
typedef
EFI_STATUS
(EFIAPI *EFI_SPEAKER_SET_FREQUENCY) (
  IN EFI_SPEAKER_IF_PROTOCOL               * This,
  IN     UINT16                            Frequency
  );

//
// Protocol definition
//
typedef struct _EFI_SPEAKER_IF_PROTOCOL {
  EFI_SPEAKER_SET_FREQUENCY SetSpeakerToneFrequency;
  EFI_GENERATE_BEEP         GenerateBeep;
} EFI_SPEAKER_IF_PROTOCOL;

extern EFI_GUID gEfiSpeakerInterfaceProtocolGuid;
#endif
