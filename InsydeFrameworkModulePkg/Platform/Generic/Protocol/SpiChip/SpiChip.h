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

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SpiChip.h

Abstract:

  EFI SPI Chip Protocol

--*/

#ifndef _EFI_SPI_CHIP_H_
#define _EFI_SPI_CHIP_H_

#define EFI_SPI_CHIP_PROTOCOL_GUID  \
  {0x37fc3233, 0xa95c, 0x4f0f, 0xab, 0x8d, 0xbb, 0x06, 0xaa, 0xff, 0x53, 0x05}

EFI_FORWARD_DECLARATION (EFI_SPI_CHIP_PROTOCOL);


#define SPI_COMMAND_WRITE_INDEX              0
#define SPI_COMMAND_READ_INDEX               1
#define SPI_COMMAND_ERASE_INDEX              2
#define SPI_COMMAND_ERASE_BLOCK_4K_INDEX     3
#define SPI_COMMAND_READ_S_INDEX             4
#define SPI_COMMAND_READ_ID_INDEX            5
#define SPI_COMMAND_WRITE_S_INDEX            6
#define SPI_COMMAND_WRITE_S_EN_INDEX         7
#define SPI_COMMAND_WRITE_ENABLE_INDEX       8


#define SPI_COMMAND_NUMBER                   9

typedef struct _SPI_CHIP_LAYOUT {
  UINT32 BlockSize;    // size == 0 is last block
  UINT32 NumOfBlock;
} SPI_CHIP_LAYOUT;

typedef
EFI_STATUS
(EFIAPI *EFI_SPICHIP_GET_COMMAND_LIST_OPERATION) (
  IN      EFI_SPI_CHIP_PROTOCOL         *This,
  OUT     UINT8                         *CommandList
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SPICHIP_IS_SUPPORTED_OPERATION) (
  IN      EFI_SPI_CHIP_PROTOCOL         *This,
  IN      UINT16                        ChipID
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SPI_GET_LAYOUT_OPERATION) (
  IN      EFI_SPI_CHIP_PROTOCOL         *This,
  OUT     SPI_CHIP_LAYOUT               *Layout
  );


typedef
struct _EFI_SPI_CHIP_PROTOCOL {
  EFI_SPICHIP_GET_COMMAND_LIST_OPERATION   GetCommandList;
  EFI_SPICHIP_IS_SUPPORTED_OPERATION       IsSupported;
  EFI_SPI_GET_LAYOUT_OPERATION             GetLayout;
} EFI_SPI_CHIP_PROTOCOL;

extern EFI_GUID gEfiSpiChipProtocolGuid;

#endif
