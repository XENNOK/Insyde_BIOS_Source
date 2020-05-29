//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    ReturnFromImage.h

Abstract:

    Guid used to signal the return of the control back to BIOS after starting image.

--*/

#ifndef _UNPLUG_VGA_GUID_H_
#define _UNPLUG_VGA_GUID_H_

#define EFI_UNPLUG_VGA_GUID \
  {0xddf0d3ef, 0x3c6b, 0x4ccc, 0xae, 0x8d, 0xc3, 0x4b, 0x41, 0xed, 0x0b, 0x74}  
  
extern EFI_GUID gEfiUnPlugVGAGuid;

#endif
