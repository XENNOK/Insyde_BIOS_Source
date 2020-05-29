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

#ifndef _RETURN_FROM_IMAGE_H_
#define _RETURN_FROM_IMAGE_H_

#define EFI_RETURN_FROM_IMAGE_GUID \
  {0x7183f96c, 0xae5, 0x457c, 0x89, 0x26, 0x2f, 0x85, 0x2e, 0xa7, 0xa6, 0x26}

extern EFI_GUID gEfiReturnFromImageGuid;

#endif
