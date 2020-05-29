/** @file
  Defination for the USB mass storage Control/Bulk/Interrupt transpor.

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

#ifndef _EFI_USBMASS_CBI_H_
#define _EFI_USBMASS_CBI_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UsbCore.h>

enum {
  USB_CBI_MAX_PACKET_NUM        = 16,
  USB_CBI_RESET_CMD_LEN         = 12,

  //
  // Usb CBI retry times
  //
  USB_CBI_MAX_RETRY             = 3,

  //
  // Usb Cbi transfer timeout
  //
  USB_CBI_RESET_TIMEOUT         = 1 * USB_MASS_STALL_1_S,
};

//
// Put Interface at the first field is to make it easy to get by Context, which
// could be BOT/CBI Protocol instance
//
typedef struct {
  USB_MASS_TRANSPORT_PROTOCOL   Transport;
  EFI_USB_CORE_PROTOCOL         *UsbCore;
  EFI_USB_INTERFACE_DESCRIPTOR  Interface;
  EFI_USB_ENDPOINT_DESCRIPTOR   *BulkInEndpoint;
  EFI_USB_ENDPOINT_DESCRIPTOR   *BulkOutEndpoint;
  EFI_USB_ENDPOINT_DESCRIPTOR   *InterruptEndpoint;
  EFI_USB_IO_PROTOCOL           *UsbIo;
} USB_CBI_PROTOCOL;

#pragma pack(1)
typedef struct {
  UINT8               Type;
  UINT8               Value;
} USB_CBI_STATUS;
#pragma pack()

/**

  Initialize the USB mass storage class CBI transport protocol.
  If Context isn't NULL, it will save its context in it.

  @param  UsbIo                 The USB IO to use
  @param  Controller            The device controller
  @param  Context               The variable to save context in

  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The device isn't supported
  @retval EFI_SUCCESS           The CBI protocol is initialized.

**/
STATIC
EFI_STATUS
UsbCbi0Init (
  IN  EFI_USB_IO_PROTOCOL   *UsbIo,
  IN  EFI_HANDLE            Controller,
  OUT VOID                  **Context       OPTIONAL
  );

/**

  Initialize the USB mass storage class CBI transport protocol.
  If Context isn't NULL, it will save its context in it.

  @param  UsbIo                 The USB IO to use
  @param  Controller            The device controller
  @param  Context               The variable to save context in

  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The device isn't supported
  @retval EFI_SUCCESS           The CBI protocol is initialized.

**/
STATIC
EFI_STATUS
UsbCbi1Init (
  IN  EFI_USB_IO_PROTOCOL   *UsbIo,
  IN  EFI_HANDLE            Controller,
  OUT VOID                  **Context       OPTIONAL
  );

/**

  Execute USB mass storage command through the CBI0/CBI1 transport protocol

  @param  Context               The USB CBI device
  @param  Cmd                   The command to transfer to device
  @param  CmdLen                The length of the command
  @param  DataDir               The direction of data transfer
  @param  Data                  The buffer to hold the data
  @param  DataLen               The length of the buffer
  @param  Timeout               The time to wait 
  @param  CmdStatus             The result of the command execution

  @retval EFI_SUCCESS           The command is executed OK and result in CmdStatus.
  @retval EFI_DEVICE_ERROR      Failed to execute the command

**/
STATIC
EFI_STATUS
UsbCbiExecCommand (
  IN  VOID                    *Context,
  IN  VOID                    *Cmd,
  IN  UINT8                   CmdLen,
  IN  EFI_USB_DATA_DIRECTION  DataDir,
  IN  VOID                    *Data,
  IN  UINT32                  DataLen,
  IN  UINT32                  Timeout,
  OUT UINT32                  *CmdStatus
  );

/**

  Call the Usb mass storage class transport protocol to
  reset the device. The reset is defined as a Non-Data
  command. Don't use UsbCbiExecCommand to send the command
  to device because that may introduce recursive loop.

  @param  Context               The USB CBI device protocol

  @retval EFI_SUCCESS           the device is reset
  @retval Others                Failed to reset the device

**/
STATIC
EFI_STATUS
UsbCbiResetDevice (
  IN  VOID                    *Context,
  IN  BOOLEAN                  ExtendedVerification
  );

/*++

  Clean up the CBI protocol's resource

  @param  Context               The CBI protocol 

  @retval EFI_SUCCESS           The resource is cleaned up.

--*/
STATIC
EFI_STATUS
UsbCbiFini (
  IN  VOID                   *Context
  );

#endif
