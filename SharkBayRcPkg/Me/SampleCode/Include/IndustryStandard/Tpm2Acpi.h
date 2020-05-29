/** @file
  TPM2 ACPI table definition.

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/

#ifndef _TPM2_ACPI_H_
#define _TPM2_ACPI_H_

#include <IndustryStandard/Acpi.h>

#pragma pack (1)

#define EFI_TPM2_ACPI_TABLE_REVISION  3

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT32                      Flags;
  UINT64                      AddressOfControlArea;
  UINT32                      StartMethod;
//UINT8                       PlatformSpecificParameters[];
} EFI_TPM2_ACPI_TABLE;

#define EFI_TPM2_ACPI_TABLE_START_METHOD_ACPI  2

typedef struct {
  UINT32   Reserved;
  UINT32   Error;
  UINT32   Cancel;
  UINT32   Start;
  UINT64   InterruptControl;
  UINT32   CommandSize;
  UINT64   Command;
  UINT32   ResponseSize;
  UINT64   Response;
} EFI_TPM2_ACPI_CONTROL_AREA;

#pragma pack ()

#endif
