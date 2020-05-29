/** @file
  USB Mass Storage Transport Protocol Header

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

#ifndef _EFI_USBMASS_TRANSPORT_H_
#define _EFI_USBMASS_TRANSPORT_H_

#include <Uefi.h>
#include <IndustryStandard/Usb.h>
#include <Protocol/UsbIo.h>

#define USB_IS_IN_ENDPOINT(EndPointAddr)      (((EndPointAddr) & 0x80) == 0x80)
#define USB_IS_OUT_ENDPOINT(EndPointAddr)     (((EndPointAddr) & 0x80) == 0)
#define USB_IS_BULK_ENDPOINT(Attribute)       (((Attribute) & 0x03) == 0x02)
#define USB_IS_INTERRUPT_ENDPOINT(Attribute)  (((Attribute) & 0x03) == 0x03)
#define USB_IS_ERROR(Result, Error)           (((Result) & (Error)) != 0)

#define USB_MASS_TRANSPORT_PROTOCOL_GUID \
{ \
  0x8aafb853, 0xfb0e, 0x4a4d, 0xb1, 0xb2, 0x81, 0xba, 0x84, 0x2e, 0xd9, 0xfc \
}

enum {
  //
  // Usb mass storage class code
  //
  USB_MASS_STORE_CLASS        = 0x08,
  USB_VENDOR_SPECIFIC_CLASS   = 0xff,
  
  //
  // Usb mass storage subclass code, specify the command set used.
  //
  USB_MASS_STORE_RBC          = 0x01, // Reduced Block Commands
  USB_MASS_STORE_8020I        = 0x02, // SFF-8020i, typically a CD/DVD device
  USB_MASS_STORE_QIC          = 0x03, // Typically a tape device
  USB_MASS_STORE_UFI          = 0x04, // Typically a floopy disk driver device
  USB_MASS_STORE_8070I        = 0x05, // SFF-8070i, typically a floppy disk driver device.
  USB_MASS_STORE_SCSI         = 0x06, // SCSI transparent command set
  
  //
  // Usb mass storage protocol code, specify the transport protocol
  //
  USB_MASS_STORE_CBI0         = 0x00, // CBI protocol with command completion interrupt
  USB_MASS_STORE_CBI1         = 0x01, // CBI protocol without command completion interrupt
  USB_MASS_STORE_BOT          = 0x50, // Bulk-Only Transport
  
  USB_MASS_STALL_1_MS         = 1000,
  USB_MASS_STALL_1_S          = 1000 * USB_MASS_STALL_1_MS,
  
  USB_MASS_CMD_SUCCESS        = 0,
  USB_MASS_CMD_FAIL,
  USB_MASS_CMD_PHASE_ERROR,
  USB_MASS_CMD_PERSISTENT,
};

typedef
EFI_STATUS
(*USB_MASS_INIT_TRANSPORT) (
  IN  EFI_USB_IO_PROTOCOL     *Usb,
  IN  EFI_HANDLE              Controller,
  OUT VOID                    **Context    OPTIONAL
  );

typedef
EFI_STATUS
(*USB_MASS_EXEC_COMMAND) (
  IN  VOID                    *Context,
  IN  VOID                    *Cmd,
  IN  UINT8                   CmdLen,
  IN  EFI_USB_DATA_DIRECTION  DataDir,
  IN  VOID                    *Data,
  IN  UINT32                  DataLen,
  IN  UINT32                  Timeout,
  OUT UINT32                  *CmdStatus
  );

typedef
EFI_STATUS
(*USB_MASS_RESET) (
  IN  VOID                    *Context,
  IN  BOOLEAN                 ExtendedVerification
  );

typedef
EFI_STATUS
(*USB_MASS_FINI) (
  IN  VOID                    *Context
  );

//
// This structure contains information necessary to select the
// proper transport protocol. The mass storage class defines
// two transport protocols. One is the CBI, and the other is BOT.
// CBI is being obseleted. The design is made modular by this
// structure so that the CBI protocol can be easily removed when
// it is no longer necessary.
//
typedef struct _USB_MASS_TRANSPORT_PROTOCOL {
  UINT8                   Protocol;
  USB_MASS_INIT_TRANSPORT Init;        // Initialize the mass storage transport protocol
  USB_MASS_EXEC_COMMAND   ExecCommand; // Transport command to the device then get result
  USB_MASS_RESET          Reset;       // Reset the device
  USB_MASS_FINI           Fini;        // Clean up the resources.
} USB_MASS_TRANSPORT_PROTOCOL;

#endif
