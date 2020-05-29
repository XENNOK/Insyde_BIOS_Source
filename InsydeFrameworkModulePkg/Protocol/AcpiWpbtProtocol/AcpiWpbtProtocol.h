//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _ACPI_WPBT_PROTOCOL_H_
#define _ACPI_WPBT_PROTOCOL_H_

#define EFI_ACPI_WPBT_PROTOCOL_GUID \
  { \
    0x720AD23B, 0x0375, 0x43e8, 0x89, 0x34, 0xFC, 0x75, 0x13, 0xDB, 0xBA, 0x39 \
  }

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_ACPI_WPBT_PROTOCOL  EFI_ACPI_WPBT_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_PLATFORM_BINARY)(
  IN EFI_ACPI_WPBT_PROTOCOL            *This,
  IN EFI_GUID                          *FileGuid,
  IN UINT16                            ArgLength,
  IN CHAR16                            *Arg
  );
/*++

Routine Description:
  Function of EFI_ACPI_WPBT_PROTOCOL, 
  for sample driver to pass the file guid of binary.

Arguments:
  *This                 - Instance of EFI_ACPI_WPBT_PROTOCOL 
  *FileGuid             - The file guid of binary 
  ArgLength             - The length (in bytes) of the input string for the binary execution
  *Arg                  - The input string for the binary execution

Returns:
  EFI_SUCCESS
  
--*/

struct _EFI_ACPI_WPBT_PROTOCOL {
  EFI_LOCATE_PLATFORM_BINARY        LocatePlatformBinary;
};

extern EFI_GUID gEfiAcpiWpbtProtocolGuid;

#endif
