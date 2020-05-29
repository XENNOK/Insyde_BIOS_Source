/** @file

  GUID definition for the SG ACPI table storage file name
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _SG_ACPI_TABLE_STORAGE_H_
#define _SG_ACPI_TABLE_STORAGE_H_

///
/// {CACB3817-81E6-497e-87FF-C8FA8F24EC28}
///
#define SG_ACPI_TABLE_STORAGE_GUID \
  { 0xcacb3817, 0x81e6, 0x497e, { 0x87, 0xff, 0xc8, 0xfa, 0x8f, 0x24, 0xec, 0x28 } }

extern EFI_GUID gSgAcpiTableStorageGuid;

#endif


#ifndef _SG_ACPI_TABLE_PCH_STORAGE_H_
#define _SG_ACPI_TABLE_PCH_STORAGE_H_

///
/// {CBCB3817-81E6-497e-87FF-C8FA8F24EC28}
///
#define SG_ACPI_TABLE_PCH_STORAGE_GUID \
  { 0xcbcb3817, 0x81e6, 0x497e, { 0x87, 0xff, 0xc8, 0xfa, 0x8f, 0x24, 0xec, 0x28 } }

extern EFI_GUID gSgAcpiTablePchStorageGuid;

#endif
