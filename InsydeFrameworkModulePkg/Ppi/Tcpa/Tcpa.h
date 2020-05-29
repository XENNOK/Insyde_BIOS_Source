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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Tcpa.h

Abstract:

  TCPA PPI as defined in EFI 2.0

--*/

#ifndef _PEI_TCPA_H_
#define _PEI_TCPA_H_

#define PEI_TCPA_PPI_GUID \
  {0xe9db0d58, 0xd48d, 0x47f6, 0x9c, 0x6e, 0x6f, 0x40, 0xe8, 0x6c, 0x7b, 0x41}


typedef
EFI_STATUS
(EFIAPI *PEI_TCPA_INITIALIZE_TPM) (
  IN EFI_PEI_SERVICES    **PeiServices,
  IN TCPA_COMMAND_CODE   Ordinal
  );

typedef
EFI_STATUS
(EFIAPI *PEI_HASH_ALL_EXTEND_TPM) (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN UINT8              *InputBuffer,
  IN UINT64             InputBufferSize,
  IN TCPA_PCRINDEX      PcrIndex
  );

typedef
EFI_STATUS
(EFIAPI *PEI_PHYSICAL_PRESENCE_TPM) (
  IN EFI_PEI_SERVICES   **PeiServices,
  IN TCPA_COMMAND_CODE  PhysicalPresenceCommand
  );

typedef struct {
  PEI_TCPA_INITIALIZE_TPM   InitializeTpm;
  PEI_HASH_ALL_EXTEND_TPM   HashAllExtendTpm;
  PEI_PHYSICAL_PRESENCE_TPM PhysicalPresenceTpm;
} PEI_TCPA_PPI;

extern EFI_GUID gPeiTcpaPpiGuid;

#endif
