/** @file
  USB Mass Storage BOT Driver Header

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

#ifndef _EFI_USBMASS_BOT_H_
#define _EFI_USBMASS_BOT_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Protocol/UsbCore.h>

enum {
  //
  // Usb Bulk-Only class specfic request
  //
  USB_BOT_RESET_REQUEST    = 0xFF,       // Bulk-Only Mass Storage Reset
  USB_BOT_GETLUN_REQUEST   = 0xFE,       // Get Max Lun
  USB_BOT_CBW_SIGNATURE    = 0x43425355, // dCBWSignature, tag the packet as CBW
  USB_BOT_CSW_SIGNATURE    = 0x53425355, // dCSWSignature, tag the packet as CSW
  USB_BOT_MAX_LUN          = 0x0F,       // Lun number is from 0 to 15
  USB_BOT_MAX_CMDLEN       = 16,         // Maxium number of command from command set
  
  //
  // Usb BOT command block status values
  //
  USB_BOT_COMMAND_OK       = 0x00, // Command passed, good status
  USB_BOT_COMMAND_FAILED   = 0x01, // Command failed
  USB_BOT_COMMAND_ERROR    = 0x02, // Phase error, need to reset the device

  //
  // Usb Bot retry times
  //
  USB_BOT_GET_STATUS_RETRY = 3,

  //
  // Usb Bot stall time
  //  
  USB_BOT_RESET_STALL      = 100 * USB_MASS_STALL_1_MS,
  
  //
  // Usb Bot transfer timeout
  //
  USB_BOT_CBW_TIMEOUT      = 1 * USB_MASS_STALL_1_S,
  USB_BOT_CSW_TIMEOUT      = 10 * USB_MASS_STALL_1_S,
  USB_BOT_DATA_TIMEOUT     = 12 * USB_MASS_STALL_1_S,
  USB_BOT_RESET_TIMEOUT    = 3 * USB_MASS_STALL_1_S,
};

//
// The CBW (Command Block Wrapper) and CSW (Command Status Wrapper)
// structures used by the Usb BOT protocol.
//
#pragma pack(1)
typedef struct {
  UINT32              Signature;
  UINT32              Tag;
  UINT32              DataLen;  // Length of data between CBW and CSW
  UINT8               Flag;     // Bit 7, 0 ~ Data-Out, 1 ~ Data-In
  UINT8               Lun;      // Lun number. Bits 0~3 are used
  UINT8               CmdLen;   // Length of the command. Bits 0~4 are used
  UINT8               CmdBlock[USB_BOT_MAX_CMDLEN];
} USB_BOT_CBW;

typedef struct {
  UINT32              Signature;
  UINT32              Tag;
  UINT32              DataResidue;
  UINT8               CmdStatus;
} USB_BOT_CSW;
#pragma pack()

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
  EFI_USB_IO_PROTOCOL           *UsbIo;
  UINT32                        CbwTag;
  UINT32                        BypassCswTimeout;
  BOOLEAN                       FirstReadPassed;
  BOOLEAN                       FirstReadCapPassed;
  BOOLEAN                       FirstRequestSensePassed;
  BOOLEAN                       FirstInquiryPassed;
} USB_BOT_PROTOCOL;

/**

  Initialize the USB mass storage class BOT transport protocol.
  It will save its context which is a USB_BOT_PROTOCOL structure
  in the Context if Context isn't NULL.

  @param  UsbIo                 The USB IO protocol to use
  @param  Controller            The controller to init
  @param  Context               The variable to save the context to

  @retval EFI_SUCCESS           The device is supported and protocol initialized.
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory
  @retval EFI_UNSUPPORTED       The transport protocol doesn't support the device.

**/
STATIC
EFI_STATUS
UsbBotInit (
  IN  EFI_USB_IO_PROTOCOL       * UsbIo,
  IN  EFI_HANDLE                Controller,
  OUT VOID                      **Context OPTIONAL
  );

/**

  Call the Usb mass storage class transport protocol to issue
  the command/data/status circle to execute the commands

  @param  Context               The context of the BOT protocol, that is, USB_BOT_PROTOCOL
  @param  Cmd                   The high level command
  @param  CmdLen                The command length
  @param  DataDir               The direction of the data transfer
  @param  Data                  The buffer to hold data
  @param  DataLen               The length of the data
  @param  Timeout               The time to wait command 
  @param  CmdStatus             The result of high level command execution

  @retval EFI_SUCCESS           The command is executed OK, and result in CmdStatus
  @retval EFI_DEVICE_ERROR      Failed to excute command

**/
STATIC
EFI_STATUS
UsbBotExecCommand (
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

  Reset the mass storage device by BOT protocol

  @param  Context               The context of the BOT protocol, that is, USB_BOT_PROTOCOL

  @param  EFI_SUCCESS           The device is reset
  @param  Others                Failed to reset the device.

**/
STATIC
EFI_STATUS
UsbBotResetDevice (
  IN  VOID                    *Context,
  IN  BOOLEAN                  ExtendedVerification
  );

/**

  Clean up the resource used by this BOT protocol

  @param  Context               The context of the BOT protocol, that is, USB_BOT_PROTOCOL

  @retval EFI_SUCCESS           The resource is cleaned up.

**/
STATIC
EFI_STATUS
UsbBotFini (
  IN  VOID                    *Context
  );

#endif
