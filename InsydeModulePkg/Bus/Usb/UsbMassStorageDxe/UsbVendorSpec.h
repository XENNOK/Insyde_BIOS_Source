/** @file
  The vendor specific devices list header

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_USBVENDOR_SPEC_H_
#define _EFI_USBVENDOR_SPEC_H_

typedef struct {
  UINT16        VendorID;
  UINT16        ProductID;
  UINT8         Protocol;
  UINT8         Reserve[3];
} USB_VENDOR_SPEC;

EFI_STATUS
UsbVendorSpecificDevice (
  IN  EFI_USB_IO_PROTOCOL       *UsbIo,
  IN  UINT16                    VendorID,
  IN  UINT16                    ProductID,
  OUT UINT8                     *Protocol
  );

#endif
