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
  
    NvidiaAcpiTablePchGuid.h
    
Abstract:

    TPV ACPI H file.

--*/

#ifndef _NVIDIA_ACPI_TABLE_PCH_GUID_H_
#define _NVIDIA_ACPI_TABLE_PCH_GUID_H_

#define NVIDIA_ACPI_TABLE_PCH_GUID \
  { \
    0x7f1cabe3, 0x34d8, 0x4f54, 0x83, 0x1c, 0x9e, 0x1d, 0x52, 0xf4, 0x8f, 0x8e \
  }

#define NVIDIA_ACPI_TABLE_PCH_NAME L"NvidiaAcpiTablePch"

extern EFI_GUID gNvidiaAcpiTablePchGuid;

#endif
