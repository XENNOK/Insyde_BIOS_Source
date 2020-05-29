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

Copyright (c) 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  UsbHcPolicy.h
 
Abstract:

  Usb Host Controller Policy PPI as defined in EFI 2.0

--*/

#ifndef _PEI_USBHC_POLICY_PPI_H
#define _PEI_USBHC_POLICY_PPI_H

#define PEI_USBHC_POLICY_PPI_GUID \
  { \
    0xe467d145, 0x937a, 0x4ac0, 0xba, 0x90, 0xc5, 0xdb, 0xe6, 0x69, 0xc9, 0x27 \
  }

EFI_FORWARD_DECLARATION (PEI_USBHC_POLICY_PPI);

typedef
UINTN
(EFIAPI *GET_UHC_IO_BASE_ADDR) (
  IN  EFI_PEI_SERVICES                   **PeiServices
  );

typedef
UINTN
(EFIAPI *GET_EHC_MEM_BASE_ADDR) (
  IN  EFI_PEI_SERVICES                   **PeiServices
  );

typedef struct _PEI_USBHC_POLICY_PPI {
  GET_UHC_IO_BASE_ADDR    GetUhcIoBaseAddr;
  GET_EHC_MEM_BASE_ADDR   GetEhcMemBaseAddr;
} PEI_USBHC_POLICY_PPI;

extern EFI_GUID gPeiUsbHcPolicyPpiGuid;

#endif
