/** @file
  Include file to suport EDD 3.0.

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

/**
  Include file to suport EDD 3.0.
  This file is coded to T13 D1386 Revision 3
  Availible on http://www.t13.org/#Project drafts
  Currently at ftp://fission.dt.wdc.com/pub/standards/x3t13/project/d1386r3.pdf

Copyright (c) 1999 - 2010, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef _EDD_H_
#define _EDD_H_

//
// packing with no compiler padding, so that the fields
// of the following architected structures can be
// properly accessed from C code.
//
#pragma pack(1)

typedef struct {
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT8   Controller;
  UINT32  Reserved;
} EDD_PCI;

typedef struct {
  UINT16  Base;
  UINT16  Reserved;
  UINT32  Reserved2;
} EDD_LEGACY;

typedef union {
  EDD_PCI     Pci;
  EDD_LEGACY  Legacy;
} EDD_INTERFACE_PATH;

typedef struct {
  UINT8 Master;
  UINT8 Reserved[15];
} EDD_ATA;

typedef struct {
  UINT8 Master;
  UINT8 Lun;
  UINT8 Reserved[14];
} EDD_ATAPI;

typedef struct {
  UINT16  Pun;
  UINT64  Lun;
  UINT8   Reserved[6];
} EDD_SCSI;

typedef struct {
  UINT64  SerialNumber;
  UINT64  Reserved;
} EDD_USB;

typedef struct {
  UINT64  Guid;
  UINT64  Reserved;
} EDD_1394;

typedef struct {
  UINT64  Wwn;
  UINT64  Lun;
} EDD_FIBRE;

typedef struct {
  UINT64  IdentityTag;
  UINT64  Reserved;
} EDD_I2O;

typedef struct {
  UINT8   PortNumber;
  UINT8   Reserved1;
  UINT16  Reserved2;
  UINT32  Reserved3;
  UINT64  Reserved4;
} EDD_SATA;

typedef struct {
  UINT64  SASAddress;
  UINT64  Lun;
} EDD_SAS;

typedef struct {
  UINT32  RAIDArrayNumber;
  UINT32  Reserved1;
  UINT32  Reserved2;
} EDD_RAID;

typedef union {
  EDD_ATA   Ata;
  EDD_ATAPI Atapi;
  EDD_SCSI  Scsi;
  EDD_USB   Usb;
  EDD_1394  FireWire;
  EDD_FIBRE FibreChannel;
  EDD_I2O   I2O;
  EDD_SATA  Sata;
  EDD_SAS   SAS;
  EDD_RAID  RAID;
} EDD_DEVICE_PATH;

typedef struct {
  UINT16              StructureSize;
  UINT16              Flags;
  UINT32              MaxCylinders;
  UINT32              MaxHeads;
  UINT32              SectorsPerTrack;
  UINT64              PhysicalSectors;
  UINT16              BytesPerSector;
  UINT32              Fdpt;
  UINT16              Key;
  UINT8               DevicePathLength;
  UINT8               Reserved1;
  UINT16              Reserved2;
  CHAR8               HostBusType[4];
  CHAR8               InterfaceType[8];
  EDD_INTERFACE_PATH  InterfacePath;
  EDD_DEVICE_PATH     DevicePath;
  UINT8               Reserved3;
  UINT8               Checksum;
} EDD_DRIVE_PARAMETERS;

//
// EDD_DRIVE_PARAMETERS.Flags defines
//
#define EDD_GEOMETRY_VALID          0x02
#define EDD_DEVICE_REMOVABLE        0x04
#define EDD_WRITE_VERIFY_SUPPORTED  0x08
#define EDD_DEVICE_CHANGE           0x10
#define EDD_DEVICE_LOCKABLE         0x20

//
// BUGBUG: This bit does not follow the spec. It tends to be always set
//  to work properly with Win98.
//
#define EDD_DEVICE_GEOMETRY_MAX 0x40

typedef struct {
  //
  // 0x18
  //
  UINT8   PacketSizeInBytes;
  UINT8   Zero;
  //
  // Max 0x7f
  //
  UINT8   NumberOfBlocks;
  UINT8   Zero2;
  UINT32  SegOffset;
  UINT64  Lba;
  UINT64  TransferBuffer;
  //
  // Max 0xffffffff
  //
  UINT32  ExtendedBlockCount;
  UINT32  Zero3;
} EDD_DEVICE_ADDRESS_PACKET;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  UINT16                          HBAPortNumber;
  UINT16                          PortMultiplierPortNumber;
  UINT16                          Lun;
} BIOS_BLOCK_SATA_DEVICE_PATH;

#define EDD_VERSION_30  0x30

//
// Int 13 BIOS Errors
//
#define BIOS_PASS                   0x00
#define BIOS_WRITE_PROTECTED        0x03
#define BIOS_SECTOR_NOT_FOUND       0x04
#define BIOS_RESET_FAILED           0x05
#define BIOS_DISK_CHANGED           0x06
#define BIOS_DRIVE_DOES_NOT_EXIST   0x07
#define BIOS_DMA_ERROR              0x08
#define BIOS_DATA_BOUNDRY_ERROR     0x09
#define BIOS_BAD_SECTOR             0x0a
#define BIOS_BAD_TRACK              0x0b
#define BIOS_MEADIA_TYPE_NOT_FOUND  0x0c
#define BIOS_INVALED_FORMAT         0x0d
#define BIOS_ECC_ERROR              0x10
#define BIOS_ECC_CORRECTED_ERROR    0x11
#define BIOS_HARD_DRIVE_FAILURE     0x20
#define BIOS_SEEK_FAILED            0x40
#define BIOS_DRIVE_TIMEOUT          0x80
#define BIOS_DRIVE_NOT_READY        0xaa
#define BIOS_UNDEFINED_ERROR        0xbb
#define BIOS_WRITE_FAULT            0xcc
#define BIOS_SENSE_FAILED           0xff

#define MAX_EDD11_XFER              0x1000

typedef struct {
  //
  // General Configuration
  //
  UINT16  config;
  //
  // Number of Cylinders
  //
  UINT16  cylinders;
  UINT16  reserved_2;
  //
  // Number of logical heads
  //
  UINT16  heads;
  UINT16  vendor_data1;
  UINT16  vendor_data2;
  UINT16  sectors_per_track;
  UINT16  vendor_specific_7_9[3];
  CHAR8   SerialNo[20];
  UINT16  vendor_specific_20_21[2];
  UINT16  ecc_bytes_available;
  CHAR8   FirmwareVer[8];
  CHAR8   ModelName[40];
  UINT16  multi_sector_cmd_max_sct_cnt;
  UINT16  reserved_48;
  UINT16  capabilities;
  UINT16  reserved_50;
  UINT16  pio_cycle_timing;
  UINT16  reserved_52;
  UINT16  field_validity;
  UINT16  current_cylinders;
  UINT16  current_heads;
  UINT16  current_sectors;
  UINT16  CurrentCapacityLsb;
  UINT16  CurrentCapacityMsb;
  UINT16  reserved_59;
  UINT16  user_addressable_sectors_lo;
  UINT16  user_addressable_sectors_hi;
  UINT16  reserved_62;
  UINT16  multi_word_dma_mode;
  UINT16  advanced_pio_modes;
  UINT16  min_multi_word_dma_cycle_time;
  UINT16  rec_multi_word_dma_cycle_time;
  UINT16  min_pio_cycle_time_without_flow_control;
  UINT16  min_pio_cycle_time_with_flow_control;
  UINT16  reserved_69_79[11];
  UINT16  major_version_no;
  UINT16  minor_version_no;
  UINT16  command_set_supported_82;
  UINT16  command_set_supported_83;
  UINT16  command_set_feature_extn;
  UINT16  command_set_feature_enb_85;
  UINT16  command_set_feature_enb_86;
  UINT16  command_set_feature_default;
  UINT16  ultra_dma_mode;
  UINT16  reserved_89_127[39];
  UINT16  security_status;
  UINT16  vendor_data_129_159[31];
  UINT16  reserved_160_255[96];
} IDENTIFY_DATA;

//
// Internal Data Structures
//
typedef struct {
  CHAR8                 Letter;
  UINT8                 Number;
  UINT8                 EddVersion;
  BOOLEAN               ExtendedInt13;
  BOOLEAN               DriveLockingAndEjecting;
  BOOLEAN               Edd;
  BOOLEAN               Extensions64Bit;
  BOOLEAN               ParametersValid;
  UINT8                 ErrorCode;
  VOID                  *FdptPointer;
  BOOLEAN               Floppy;
  BOOLEAN               AtapiFloppy;
  UINT8                 MaxHead;
  UINT8                 MaxSector;
  UINT16                MaxCylinder;
  UINT16                Pad;
  EDD_DRIVE_PARAMETERS  Parameters;
  UINT32                INT13Vector;
  IDENTIFY_DATA         IdentifyData;
} BIOS_LEGACY_DRIVE;

#define BIOS_CONSOLE_BLOCK_IO_DEV_SIGNATURE SIGNATURE_32 ('b', 'b', 'i', 'o')
typedef struct {
  UINTN                     Signature;
  EFI_HANDLE                Handle;
  EFI_HANDLE                ControllerHandle;
  EFI_BLOCK_IO_PROTOCOL     BlockIo;
  EFI_BLOCK_IO_MEDIA        BlockMedia;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  BIOS_LEGACY_DRIVE         Bios;
  BOOLEAN                   MbrReadError;
} BIOS_BLOCK_IO_DEV;

#pragma pack()

#define BIOS_BLOCK_IO_FROM_THIS(a)  CR (a, BIOS_BLOCK_IO_DEV, BlockIo, BIOS_CONSOLE_BLOCK_IO_DEV_SIGNATURE)

#endif
