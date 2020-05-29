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

    Gpio.h

Abstract:

    EFI GPIO Protocol

Revision History

--*/

#ifndef _EFI_GPIO_H
#define _EFI_GPIO_H

//
// Global ID for the GPIO Protocol
//
#define EFI_GPIO_PROTOCOL_GUID \
  { \
    0xb5d09084, 0x80ad, 0x4759, 0xb5, 0x1c, 0x27, 0x54, 0x8a, 0xfb, 0x8b, 0x8d \
  }

EFI_FORWARD_DECLARATION (EFI_GPIO_PROTOCOL);

typedef enum _EFI_GPIO_METHOD
{
  EfiGpioMultipleOperation= 0,
  EfiGpioSingleOperation  = 1,
  EfiGpioMaxOperation     = 2
} EFI_GPIO_METHOD;

typedef
EFI_STATUS
(EFIAPI *EFI_GPIO_PROTOCOL_GET_STATE) (
  IN EFI_GPIO_PROTOCOL           * This,
  IN UINT32                      *GpioNumberSet,
  IN UINT32                      GpioArraySize,
  IN EFI_GPIO_METHOD             GpioMethod,
  IN VOID                        *DeviceSelector,
  IN OUT VOID                    *GpioState
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GPIO_PROTOCOL_SET_STATE) (
  IN EFI_GPIO_PROTOCOL          * This,
  IN UINT32                     *GpioNumberSet,
  IN UINT32                     GpioArraySize,
  IN EFI_GPIO_METHOD            GpioMethod,
  IN VOID                       *DeviceSelector,
  IN VOID                       *GpioState
  );

//
// Interface structure for the GPIO Protocol
//
typedef struct _EFI_GPIO_PROTOCOL {
  EFI_GPIO_PROTOCOL_GET_STATE GetState;
  EFI_GPIO_PROTOCOL_SET_STATE SetState;
} EFI_GPIO_PROTOCOL;

extern EFI_GUID gEfiGpioProtocolGuid;

#endif
