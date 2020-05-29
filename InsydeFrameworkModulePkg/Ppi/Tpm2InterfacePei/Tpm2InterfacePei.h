//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TPM2_INTERFACE_PEI_H_
#define _TPM2_INTERFACE_PEI_H_

#define TPM2_INTERFACE_PPI_GUID \
  {0x9d910b61, 0x85bd, 0x4473, 0x81, 0x31, 0x40, 0x95, 0x9d, 0x60, 0xc7, 0xee}

EFI_FORWARD_DECLARATION (PEI_TPM2_INTERFACE_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_SUMMIT_COMMAND) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  );

typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_REQUEST_USE) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This
  );

typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_SEND) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn
  );

typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_RECEIVE) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  );


typedef struct _PEI_TPM2_INTERFACE_PPI {
  PEI_TPM2_SUMMIT_COMMAND                 SummitCommand;
  PEI_TPM2_REQUEST_USE                    RequestUse;
  PEI_TPM2_SEND                           Send;
  PEI_TPM2_RECEIVE                        Receive;
};

extern EFI_GUID gPeiTpm2InterfacePpiGuid;
#endif
