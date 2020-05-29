/** @file
  FAT format data structures

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
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

    FatFmt.h

Abstract:

  FAT format data structures

--*/

#ifndef _FAT_FMT_H_
#define _FAT_FMT_H_

#include <Uefi.h>

//
// Definitions
//
#define FAT_LFN_LAST                      0x40  // Ordinal field
#define FAT_MAX_LFN_ENTRIES               20
#define FAT_ENTRY_INFO_OFFSET             13
#define FAT_DELETE_ENTRY_MARK             0xE5
#define FAT_EMPTY_ENTRY_MARK              0x00
#define FAT_ATTR_READ_ONLY                0x01
#define FAT_ATTR_HIDDEN                   0x02
#define FAT_ATTR_SYSTEM                   0x04
#define FAT_ATTR_VOLUME_ID                0x08
#define FAT_ATTR_DIRECTORY                0x10
#define FAT_ATTR_ARCHIVE                  0x20
#define FAT_ATTR_LFN                      (FAT_ATTR_READ_ONLY | FAT_ATTR_HIDDEN | FAT_ATTR_SYSTEM | FAT_ATTR_VOLUME_ID)

//
// FAT entry values
//
#define FAT_CLUSTER_SPECIAL_EXT       (-1 & (~0xF))
#define FAT_CLUSTER_SPECIAL           ((FAT_CLUSTER_SPECIAL_EXT) | 0x7)
#define FAT_CLUSTER_FREE              0
#define FAT_CLUSTER_RESERVED          (FAT_CLUSTER_SPECIAL)
#define FAT_CLUSTER_BAD               (FAT_CLUSTER_SPECIAL)
#define FAT_CLUSTER_LAST              (-1)

#define FAT_MIN_CLUSTER               2
#define FAT_MAX_FAT12_CLUSTER         0xFF5
#define FAT_MAX_FAT16_CLUSTER         0xFFF5
#define FAT_CLUSTER_SPECIAL_FAT12     0xFF7
#define FAT_CLUSTER_SPECIAL_FAT16     0xFFF7
#define FAT_CLUSTER_SPECIAL_FAT32     0x0FFFFFF7
#define FAT_CLUSTER_MASK_FAT12        0xFFF
#define FAT_CLUSTER_UNMASK_FAT12      0xF000
#define FAT_CLUSTER_MASK_FAT32        0x0FFFFFFF
#define FAT_CLUSTER_UNMASK_FAT32      0xF0000000
#define FAT_POS_FAT12(a)              ((a) * 3 / 2)
#define FAT_POS_FAT16(a)              ((a) * 2)
#define FAT_POS_FAT32(a)              ((a) * 4)
#define FAT_ODD_CLUSTER_FAT12(a)      (((a) & 1) != 0)

#define FAT_CLUSTER_FUNCTIONAL(Cluster)   (((Cluster) == 0) || ((Cluster) >= FAT_CLUSTER_SPECIAL))
#define FAT_CLUSTER_END_OF_CHAIN(Cluster) ((Cluster) > (FAT_CLUSTER_SPECIAL))

//
// Some 8.3 File Name definitions
//
#define FAT_MAIN_NAME_LEN                 8
#define FAT_EXTEND_NAME_LEN               3
#define FAT_NAME_LEN                      (FAT_MAIN_NAME_LEN + FAT_EXTEND_NAME_LEN)

#define DELETE_ENTRY_MARK                 0xE5
#define EMPTY_ENTRY_MARK                  0x00

#define FILE_DESCRIPTION_BLOCK_SIZE       32
#define FAT_MAX_YEAR_FROM_1980            0x7f
#define FAT_MAX_DIRENTRY_COUNT            0xFFFF

//
// Directory Entry
//
#pragma pack(1)

typedef struct {
  UINT16  Day : 5;
  UINT16  Month : 4;
  UINT16  Year : 7;                 // From 1980
} FAT_DATE;

typedef struct {
  UINT16  DoubleSecond : 5;
  UINT16  Minute : 6;
  UINT16  Hour : 5;
} FAT_TIME;

typedef struct {
  FAT_TIME  Time;
  FAT_DATE  Date;
} FAT_DATE_TIME;

typedef struct {
  CHAR8         FileName[11];       // 8.3 filename
  UINT8         Attributes;
  UINT8         CaseFlag;
  UINT8         CreateMillisecond;  // (creation milliseconds - ignored)
  FAT_DATE_TIME FileCreateTime;
  FAT_DATE      FileLastAccess;
  UINT16        FileClusterHigh;    // >= FAT32
  FAT_DATE_TIME FileModificationTime;
  UINT16        FileCluster;
  UINT32        FileSize;
} FAT_DIRECTORY_ENTRY;

typedef struct {
  UINT8   Ordinal;
  CHAR8   Name1[10];                // (Really 5 chars, but not WCHAR aligned)
  UINT8   Attributes;
  UINT8   Type;
  UINT8   Checksum;
  CHAR16  Name2[6];
  UINT16  MustBeZero;
  CHAR16  Name3[2];
} FAT_DIRECTORY_LFN;

//
// Boot Sector
//
typedef struct {

  UINT8   Ia32Jump[3];
  CHAR8   OemId[8];

  UINT16  SectorSize;
  UINT8   SectorsPerCluster;
  UINT16  ReservedSectors;
  UINT8   NoFats;
  UINT16  RootEntries;          // < FAT32, root dir is fixed size
  UINT16  Sectors;
  UINT8   Media;                // (ignored)
  UINT16  SectorsPerFat;        // < FAT32
  UINT16  SectorsPerTrack;      // (ignored)
  UINT16  Heads;                // (ignored)
  UINT32  HiddenSectors;        // (ignored)
  UINT32  LargeSectors;         // => FAT32
  UINT8   PhysicalDriveNumber;  // (ignored)
  UINT8   CurrentHead;          // holds boot_sector_dirty bit
  UINT8   Signature;            // (ignored)
  CHAR8   Id[4];
  CHAR8   FatLabel[11];
  CHAR8   SystemId[8];

} PEI_FAT_BOOT_SECTOR;

typedef struct {

  UINT8   Ia32Jump[3];
  CHAR8   OemId[8];

  UINT16  SectorSize;
  UINT8   SectorsPerCluster;
  UINT16  ReservedSectors;
  UINT8   NoFats;
  UINT16  RootEntries;          // < FAT32, root dir is fixed size
  UINT16  Sectors;
  UINT8   Media;                // (ignored)
  UINT16  SectorsPerFat;        // < FAT32
  UINT16  SectorsPerTrack;      // (ignored)
  UINT16  Heads;                // (ignored)
  UINT32  HiddenSectors;        // (ignored)
  UINT32  LargeSectors;         // Used if Sectors==0
  UINT32  LargeSectorsPerFat;   // FAT32
  UINT16  ExtendedFlags;        // FAT32 (ignored)
  UINT16  FsVersion;            // FAT32 (ignored)
  UINT32  RootDirFirstCluster;  // FAT32
  UINT16  FsInfoSector;         // FAT32
  UINT16  BackupBootSector;     // FAT32
  UINT8   Reserved[12];         // FAT32 (ignored)
  UINT8   PhysicalDriveNumber;  // (ignored)
  UINT8   CurrentHead;          // holds boot_sector_dirty bit
  UINT8   Signature;            // (ignored)
  CHAR8   Id[4];
  CHAR8   FatLabel[11];
  CHAR8   SystemId[8];

} PEI_FAT_BOOT_SECTOR_EX;

#pragma pack()

#endif
