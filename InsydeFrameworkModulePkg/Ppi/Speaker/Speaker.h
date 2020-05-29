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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
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

#ifndef _PEI_SPEAKER_IF_H
#define _PEI_SPEAKER_IF_H

//
// Global ID Speaker Interface
//
#define PEI_SPEAKER_INTERFACE_PPI_GUID \
  { \
    0x30ac275e, 0xbb30, 0x4b84, 0xa1, 0xcd, 0x0a, 0xf1, 0x32, 0x2c, 0x89, 0xc0 \
  }

EFI_FORWARD_DECLARATION (PEI_SPEAKER_IF_PPI);

//
// Beep Base On Beep Table
//
typedef
EFI_STATUS
(EFIAPI *EFI_SPEAKER_MEMORY_ERROR_BEEP) (
  IN EFI_PEI_SERVICES             **PeiServices
  );

//
// Beep Code
//
typedef
EFI_STATUS
(EFIAPI *EFI_SPEAKER_GENERATE_BEEP) (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN     UINTN                        NumberOfBeep,
  IN     UINTN                        BeepDuration,
  IN     UINTN                        TimeInterval
  );

//
// Set Frequency
//
typedef
EFI_STATUS
(EFIAPI *EFI_SPEAKER_SET_FREQUENCY) (
  IN EFI_PEI_SERVICES                 **PeiServices,
  IN     UINT16                       Frequency
  );

//
// Protocol definition
//
typedef struct _PEI_SPEAKER_IF_PPI {
  EFI_SPEAKER_SET_FREQUENCY SetSpeakerToneFrequency;
  EFI_SPEAKER_GENERATE_BEEP GenerateBeep;
  EFI_SPEAKER_MEMORY_ERROR_BEEP       MemoryErrorBeep;
} PEI_SPEAKER_IF_PPI;

extern EFI_GUID gPeiSpeakerInterfacePpiGuid;
#endif
