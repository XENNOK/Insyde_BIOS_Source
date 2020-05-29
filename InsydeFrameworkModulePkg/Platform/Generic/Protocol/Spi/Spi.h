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

  Spi.h

Abstract:

  EFI SPI Protocol

--*/

#ifndef _EFI_SPI_H
#define _EFI_SPI_H

#define EFI_SPI_HC_PROTOCOL_GUID  \
  {0x95823e5f, 0x5c35, 0x4c4d, 0xa9, 0x9e, 0x34, 0x2a, 0x22, 0x3b, 0x5e, 0x8c}

EFI_FORWARD_DECLARATION (EFI_SPI_HC_PROTOCOL);

typedef
enum _EFI_SPI_DEVICE_COMMAND {
  SpiRead,
  SpiWrite,
  SpiEraseBlock,
  SpiGetJedecId
} EFI_SPI_DEVICE_COMMAND;

typedef
EFI_STATUS
(EFIAPI *EFI_SPI_HC_EXECUTE_OPERATION) (
  IN EFI_SPI_HC_PROTOCOL *This,
  IN      EFI_SPI_DEVICE_COMMAND        Command,
  IN      UINTN                         Address,
  IN OUT  UINTN                         *Length,
  IN OUT  VOID                          *Buffer
  );

typedef
struct _EFI_SPI_HC_PROTOCOL {
  EFI_SPI_HC_EXECUTE_OPERATION   Execute;
} EFI_SPI_HC_PROTOCOL;

extern EFI_GUID gEfiSpiProtocolGuid;
#endif
