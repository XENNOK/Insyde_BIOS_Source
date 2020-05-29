/** @file
  Header file for PCH USB precondition feature support in DXE phase

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchUsbPrecondition.h

@brief
  Header file for PCH USB precondition feature support in DXE phase

--*/
#ifndef _PCH_USB_PRECONDITION_H_
#define _PCH_USB_PRECONDITION_H_

#include <PchAccess.h>
#include <Protocol/UsbHcPortPrecondition.h>

#define USB_HC_RESET_STALL_US        10 * 1000 ///< 10ms
#define USB_ROOT_PORT_RESET_STALL_US 50 * 1000 ///< 50ms
#define USB_TDRSTR_CHECK_INTERVAL_US 100
#define EHCI_PRECONDITION_DEV_SIGN SIGNATURE_32 ('e','p','r','e')
#define EHC_PRECONDITION_FROM_THIS(a) CR(a, USB_EHCI_PRECONDITION_DEV, Protocol, EHCI_PRECONDITION_DEV_SIGN)

typedef struct _USB_EHCI_PRECONDITION_DEV {
  UINTN                                 Signature;
  EFI_USB_HC_PORT_PRECONDITION          Protocol;
  UINTN                                 PortResetBitMap;
} USB_EHCI_PRECONDITION_DEV;

#define XHCI_PRECONDITION_DEV_SIGN SIGNATURE_32 ('x','p','r','e')
#define XHC_PRECONDITION_FROM_THIS(a) CR(a, USB_XHCI_PRECONDITION_DEV, Protocol, XHCI_PRECONDITION_DEV_SIGN)

typedef struct _USB_XHCI_PRECONDITION_DEV {
  UINTN                                 Signature;
  EFI_USB_HC_PORT_PRECONDITION          Protocol;
  UINTN                                 HsPortCount;
  UINTN                                 PortResetBitMap;
  UINTN                                 PortResetDoneBitMap;
  BOOLEAN                               PORTSCxResumeDoneFlag;
} USB_XHCI_PRECONDITION_DEV;

#endif
