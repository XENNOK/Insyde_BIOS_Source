/** @file
  EFI Mbr Partition Generic Driver Header. This header contains Data structures and Definitions
  needed to recognize partitions in a MBR

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2006 - 2012 Intel Corporation. All rights reserved
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
#ifndef _MBR_H
#define _MBR_H

#define MBR_SIGNATURE       0xAA55
#define NUM_MBR_PARTITIONS  4

///
/// Type definitions for the Hard drive device path
///
#define MBR_TYPE_MASTER_BOOT_RECORD         0x01
#define MBR_TYPE_EFI_PARTITION_TABLE_HEADER 0x02

///
/// Needed Partition Type definitions for MBR
///
#define NO_PARTITION              0x0
#define EXTENDED_PARTITION        0x05
#define WIN95_EXTENDED_PARTITION  0x0F

//
// Data Structures needed for MBR
//
#pragma pack(1)

///
/// Data Structure definition for each partition in the MBR
///
typedef struct _MBR_PARTITION {
  UINT8   BootIndicator;
  UINT8   StartHead;
  UINT8   StartSector;
  UINT8   StartTrack;
  UINT8   OSType;
  UINT8   EndHead;
  UINT8   EndSector;
  UINT8   EndTrack;
  UINT32  StartingLba;
  UINT32  SizeInLba;
} MBR_PARTITION;

///
/// Data Structure definition of the MBR located in the first block on an MBR drive
///
typedef struct _MASTER_BOOT_RECORD {
  UINT8         BootCode[440];
  UINT32        UniqueMbrSig;
  UINT16        Unknown;
  MBR_PARTITION PartRec[4];
  UINT16        Sig;
} MASTER_BOOT_RECORD;

#pragma pack()
#endif
