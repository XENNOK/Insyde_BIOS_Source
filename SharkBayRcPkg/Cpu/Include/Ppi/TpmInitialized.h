/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  TpmInitialized.h

@brief
  Tpm Initialized PPI definition, used to notify any code after TPM initialized.

**/
#ifndef _PEI_TPM_INITIALIZED_PPI_H_
#define _PEI_TPM_INITIALIZED_PPI_H_

#define PEI_TPM_INITIALIZED_PPI_GUID \
  { 0xe9db0d58, 0xd48d, 0x47f6, { 0x9c, 0x6e, 0x6f, 0x40, 0xe8, 0x6c, 0x7b, 0x41 } }

extern EFI_GUID gPeiTpmInitializedPpiGuid;

#endif
