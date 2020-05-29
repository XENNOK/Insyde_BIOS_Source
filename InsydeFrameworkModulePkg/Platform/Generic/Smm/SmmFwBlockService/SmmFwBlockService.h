//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SMM_FWBLOCK_SERVICES_H_
#define _SMM_FWBLOCK_SERVICES_H_

#include "Tiano.h"
#include "EfiSmmDriverLib.h"
#include "FdSupportLib.h"
#include "ChipsetLib.h"
#include "EcLib.h"
#include "SmmDriverLib.h"

#include EFI_PROTOCOL_DEFINITION (SmmBase)
#include EFI_PROTOCOL_DEFINITION (SmmRtProtocol)
#include EFI_PROTOCOL_DEFINITION (SmmFwBlockService)
#include EFI_PROTOCOL_CONSUMER (NonVolatileVariable)

extern EFI_SMM_SYSTEM_TABLE             *mSmst;

#define FVB_MAX_RETRY_TIMES             10000000

typedef struct {
  UINT32                                Signature;
  FLASH_DEVICE                          *DevicePtr;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     SmmFwbService;
} SMM_FW_BLOCK_SERVICE_INSTANCE;

#define SMM_FW_BLOCK_SERVICE_SIGNATURE  EFI_SIGNATURE_32 ('s', 'm', 'f', 'b')

#define INSTANCE_FROM_EFI_SMM_FW_BLOCK_SERVICE_THIS(a)  CR (a, SMM_FW_BLOCK_SERVICE_INSTANCE, SmmFwbService, SMM_FW_BLOCK_SERVICE_SIGNATURE)

EFI_STATUS
FlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  OUT UINT8                             *Buffer
  );

EFI_STATUS
FlashFdRead (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             Offset,
  IN OUT UINTN                          *NumBytes,
  OUT UINT8                             *Buffer
  );

EFI_STATUS
FlashFdWrite (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN OUT UINTN                          *NumBytes,
  IN  UINT8                             *Buffer
  );

EFI_STATUS
FlashFdEraseBlock (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             *NumBytes
  );

EFI_STATUS
FlashFdGetFlashTable (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  );

EFI_STATUS
FlashFdGetSpiFlashNumber (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                              *FlashNumber
  );
  
EFI_STATUS
FlashFdConvertToSpiAddress (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  );

extern UINT8                            mSmmFlashMode;
#endif
