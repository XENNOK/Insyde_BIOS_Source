/** @file
  GUID definition for the Dptf ACPI table storage file name

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;******************************************************************************
@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

#ifndef _DPTF_ACPI_TABLE_STORAGE_H_
#define _DPTF_ACPI_TABLE_STORAGE_H_

#define DPTF_ACPI_TABLE_STORAGE_GUID \
  { 0xea139578, 0xfea0, 0x4dd2, { 0x91, 0xb5, 0x69, 0x53, 0x81, 0x9e, 0xf1, 0xe4 } }

extern EFI_GUID gDptfAcpiTableStorageGuid;

#define DPTF_ACPI_TABLE_STORAGE_GUID_FFRD \
  { 0x2820908b, 0x87f6, 0x446b, { 0xa1, 0x00, 0x74, 0xee, 0xe3, 0x6e, 0x29, 0x18 } }

extern EFI_GUID gDptfAcpiTableStorageGuidFfrd;

#endif
