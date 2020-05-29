//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TPM2_INTERFACE_H_
#define _TPM2_INTERFACE_H_

#define TPM2_INTERFACE_GUID \
  {0x18d1324c, 0x7c8f, 0x489e, 0x91, 0x8d, 0x3b, 0x7b, 0x78, 0x60, 0x52, 0x33}

EFI_FORWARD_DECLARATION (TPM2_INTERFACE_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *TPM2_SUMMIT_COMMAND) (
  IN     TPM2_INTERFACE_PROTOCOL              *This,
  IN     UINT8                                *BufferIn,
  IN     UINT32                               SizeIn,
  IN OUT UINT8                                *BufferOut,
  IN OUT UINT32                               *SizeOut
  );

typedef
EFI_STATUS
(EFIAPI *TPM2_REQUEST_USE) (
  IN     TPM2_INTERFACE_PROTOCOL              *This
  );

typedef
EFI_STATUS
(EFIAPI *TPM2_SEND) (
  IN     TPM2_INTERFACE_PROTOCOL              *This,
  IN     UINT8                                *BufferIn,
  IN     UINT32                               SizeIn
  );

typedef
EFI_STATUS
(EFIAPI *TPM2_RECEIVE) (
  IN     TPM2_INTERFACE_PROTOCOL              *This,
  IN OUT UINT8                                *BufferOut,
  IN OUT UINT32                               *SizeOut
  );

typedef struct _TPM2_INTERFACE_PROTOCOL {
  TPM2_SUMMIT_COMMAND                         SummitCommand;
  TPM2_REQUEST_USE                            RequestUse;
  TPM2_SEND                                   Send;
  TPM2_RECEIVE                                Receive;
} TPM2_INTERFACE_PROTOCOL;

extern EFI_GUID gTpm2InterfaceGuid;
#endif
