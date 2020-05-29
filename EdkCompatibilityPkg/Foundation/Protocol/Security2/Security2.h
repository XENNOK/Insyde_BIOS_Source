/*++

Copyright (c) 2004, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  Security2.h

Abstract:

  Security2 Architectural Protocol as defined in the DXE CIS

  Used to provide Security services.  Specifically, dependening upon the
  authentication state of a discovered driver in a Firmware Volume, the
  portable DXE Core Dispatcher will call into the Security2 Architectural
  Protocol (SAP2) with the authentication state of the driver.

  This call-out allows for OEM-specific policy decisions to be made, such
  as event logging for attested boots, locking flash in response to discovering
  an unsigned driver or failed signature check, or other exception response.

  The SAP can also change system behavior by having the DXE core put a driver
  in the Schedule-On-Request (SOR) state.  This will allow for later disposition
  of the driver by platform agent, such as Platform BDS.

--*/

#ifndef _ARCH_PROTOCOL_SECURITY2_H_
#define _ARCH_PROTOCOL_SECURITY2_H_

//
// Global ID for the Security2 Code Architectural Protocol
//
#define EFI_SECURITY2_ARCH_PROTOCOL_GUID  \
  { 0x94ab2f58, 0x1438, 0x4ef1, 0x91, 0x52, 0x18, 0x94, 0x1a, 0x3a, 0x0e, 0x68 }

EFI_FORWARD_DECLARATION (EFI_SECURITY2_ARCH_PROTOCOL);

typedef EFI_STATUS
(EFIAPI *EFI_SECURITY2_FILE_AUTHENTICATION) (
  IN CONST EFI_SECURITY2_ARCH_PROTOCOL *This,
  IN EFI_DEVICE_PATH_PROTOCOL          *DevicePath,
  IN VOID                              *FileBuffer,
  IN UINTN                             FileSize,
  IN BOOLEAN                           BootPolicy
  )
/*++

Routine Description:

  The DXE Foundation uses this service to measure and/or verify a UEFI image.

  This service abstracts the invocation of Trusted Computing Group (TCG) measured boot, UEFI
  Secure boot, and UEFI User Identity infrastructure. For the former two, the DXE Foundation
  invokes the FileAuthentication() with a DevicePath and corresponding image in
  FileBuffer memory. The TCG measurement code will record the FileBuffer contents into the
  appropriate PCR. The image verification logic will confirm the integrity and provenance of the
  image in FileBuffer of length FileSize . The origin of the image will be DevicePath in
  these cases.
  If the FileBuffer is NULL, the interface will determine if the DevicePath can be connected
  in order to support the User Identification policy.

Arguments:

  This                   - The EFI_SECURITY2_ARCH_PROTOCOL instance.
  File                   - A pointer to the device path of the file that is
                           being dispatched. This will optionally be used for logging.
  FileBuffer             - A pointer to the buffer with the UEFI file image.
  FileSize               - The size of the file.
  BootPolicy             - A boot policy that was used to call LoadImage() UEFI service. If
                           FileAuthentication() is invoked not from the LoadImage(),
                           BootPolicy must be set to FALSE.

Returns:

  EFI_SUCCESS            - The file specified by DevicePath and non-NULL
                           FileBuffer did authenticate, and the platform policy dictates
                           that the DXE Foundation may use the file.
  EFI_SUCCESS            - The device path specified by NULL device path DevicePath
                           and non-NULL FileBuffer did authenticate, and the platform
                           policy dictates that the DXE Foundation may execute the image in
                           FileBuffer.
  EFI_SUCCESS            - FileBuffer is NULL and current user has permission to start
                           UEFI device drivers on the device path specified by DevicePath.
  EFI_SECURITY_VIOLATION - The file specified by DevicePath and FileBuffer did not
                           authenticate, and the platform policy dictates that the file should be
                           placed in the untrusted state. The image has been added to the file
                           execution table.
  EFI_ACCESS_DENIED      - The file specified by File and FileBuffer did not
                           authenticate, and the platform policy dictates that the DXE
                           Foundation may not use File.
  EFI_SECURITY_VIOLATION - FileBuffer is NULL and the user has no
                           permission to start UEFI device drivers on the device path specified
                           by DevicePath.
  EFI_SECURITY_VIOLATION - FileBuffer is not NULL and the user has no permission to load
                           drivers from the device path specified by DevicePath. The
                           image has been added into the list of the deferred images.

--*/
;

///
/// The EFI_SECURITY2_ARCH_PROTOCOL is used to abstract platform-specific policy from the
/// DXE Foundation. This includes measuring the PE/COFF image prior to invoking, comparing the
/// image against a policy (whether a white-list/black-list of public image verification keys
/// or registered hashes).
///
struct _EFI_SECURITY2_ARCH_PROTOCOL {
  EFI_SECURITY2_FILE_AUTHENTICATION FileAuthentication;
};

extern EFI_GUID gEfiSecurity2ArchProtocolGuid;

#endif
