/** @file

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

#ifndef _H2O_SWITCHABLE_GRAPHICS_PPI_H_
#define _H2O_SWITCHABLE_GRAPHICS_PPI_H_

#define H2O_SWITCHABLE_GRAPHICS_PPI_GUID \
  { \
    0x8daf8e3d, 0xf580, 0x4b2e, {0xa7, 0xbe, 0xc4, 0xfa, 0xe5, 0x42, 0xab, 0x8e} \
  }

typedef
BOOLEAN
(EFIAPI *SG_GPIO_READ) (
  IN UINT8                                    Value,
  IN BOOLEAN                                  IsUlt
  );

typedef
VOID
(EFIAPI *SG_GPIO_WRITE) (
  IN UINT8                                    Value,
  IN BOOLEAN                                  Level,
  IN BOOLEAN                                  IsUlt
  );

typedef
EFI_STATUS
(EFIAPI *SG_STALL) (
  IN UINTN                                    Microseconds
  );

typedef struct _H2O_SWITCHABLE_GRAPHICS_PPI {
  SG_GPIO_READ                                GpioRead;
  SG_GPIO_WRITE                               GpioWrite;
  SG_STALL                                    Stall;
} H2O_SWITCHABLE_GRAPHICS_PPI;

extern EFI_GUID gH2OSwitchableGraphicsPpiGuid;

#endif
