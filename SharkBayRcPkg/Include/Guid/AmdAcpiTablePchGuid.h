//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/*++

Copyright (c)  2011 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
  
    AmdAcpiTablePchGuid.h
    
Abstract:

    TPV ACPI H file.

--*/

#ifndef _AMD_ACPI_TABLE_PCH_GUID_H_
#define _AMD_ACPI_TABLE_PCH_GUID_H_

#define AMD_ACPI_TABLE_PCH_GUID \
  { \
    0x77aed82e, 0x77de, 0x42ca, 0x8c, 0x27, 0xe9, 0xd7, 0x1d, 0xf6, 0x06, 0xc7 \
  }

#define AMD_ACPI_TABLE_PCH_NAME L"AmdAcpiTablePch"

extern EFI_GUID gAmdAcpiTablePchGuid;

#endif
