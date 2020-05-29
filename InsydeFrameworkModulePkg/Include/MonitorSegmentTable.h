//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c) 2006 Intel Corporation.  All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  MonitorSegmentTable.h

Abstract:

  The definition for ACPI Monitor Segment (MSEG) Table.
  It is defined in "Intel LaGrande Technology (LT)
  SMM STM Specification" document.

--*/

#ifndef _EFI_MONITOR_SEGMENT_TABLE_H_
#define _EFI_MONITOR_SEGMENT_TABLE_H_

//
// "MSEG" MSEG Description Table Signature
//
#define EFI_ACPI_MSEG_DESCRIPTION_TABLE_SIGNATURE  0x4745534D

//
// MSEG Revision
//
#define EFI_ACPI_MSEG_DESCRIPTION_TABLE_REVISION   0x01

//
// MSEG OEM Table ID ("LT_MSEG ")
//
#define EFI_ACPI_MSEG_OEM_TABLE_ID                 0x204745534D5F544C

//
// MSEG LtSmmFeatureFlags
//
#define EFI_ACPI_MSEG_LT_SMM_FEATURE_FLAGS_64BIT_ADDRESSING_SUPPORT  0x1

#define EFI_ACPI_MSEG_VENDOR_SMI_COMMAND_NUMBER     16
#define EFI_ACPI_MSEG_VENDOR_STM_INTERFACE_NUMBER   16

//
// Ensure proper structure formats
//
#pragma pack (1)

//
// Definition for Monitor Segment ACPI Table
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER          Header;
  UINT32                               SmramRangeBufferSize;
  UINT64                               SynchronizationPtr;
  UINT32                               MinimumStmSmmRevId;
  UINT8                                GetMsegStatusCmd;
  UINT8                                UpdateMsegSizeCmd;
  UINT8                                LoadStmCmd;
  UINT8                                UnloadStmCmd;
  UINT8                                GetSmramRangesCmd;
  UINT8                                Reserved_57[11];
  UINT32                               LtSmmFeatureFlags;
  UINT8                                VendorSmiCommands[EFI_ACPI_MSEG_VENDOR_SMI_COMMAND_NUMBER];
  EFI_GUID                             VendorSmiCommandGuids[EFI_ACPI_MSEG_VENDOR_SMI_COMMAND_NUMBER];
  EFI_GUID                             VendorStmInterfaceGuids[EFI_ACPI_MSEG_VENDOR_STM_INTERFACE_NUMBER];
} EFI_ACPI_MSEG_DESCRIPTION_TABLE;

#pragma pack()

#endif

