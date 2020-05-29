//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  EndOfBdsBootSelectionProtocol.h

Abstract:

  This protocol will be installed on the end of BdsBootSelection
  
--*/

#ifndef _END_OF_BDS_BOOT_SELECTION_PROTOCOL_H_
#define _END_OF_BDS_BOOT_SELECTION_PROTOCOL_H_

#define EFI_END_OF_BDS_BOOT_SELECTION_PROTOCOL_GUID  \
  {0xC9C83B9B, 0x81E7, 0x4d76, 0xA3, 0x5F, 0x06, 0x8B, 0xC0, 0xCB, 0x97, 0xAE}

EFI_FORWARD_DECLARATION (EFI_END_OF_BDS_BOOT_SELECTION_PROTOCOL);

extern EFI_GUID gEfiEndOfBdsBootSelectionProtocolGuid;

#endif
