/** @file

  Definitions for WHEA platforrm protocol.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _WHEA_PLATFORM_PROTOCOL_H_
#define _WHEA_PLATFORM_PROTOCOL_H_

//
// Includes
//
#include <WheaDefs.h>

typedef struct _EFI_WHEA_PLATFORM_PROTOCOL       EFI_WHEA_PLATFORM_PROTOCOL;

#define EFI_WHEA_PLATFORM_GUID \
  { 0xBDFECBC9, 0x8932, 0x4029, { 0xBC, 0x75, 0x4A, 0x2F, 0xC6, 0x64, 0x11, 0x44 }}


#pragma pack(1)

typedef struct {
  WHEA_ERROR_TYPE                      Type;
  UINT16                               SourceId;
  UINTN                                Flags;
  VOID                                 *SourceData;
} SYSTEM_NATIVE_ERROR_SOURCE;

typedef struct {
  UINT16                               Type;
  UINT16                               SourceId;
  UINTN                                Flags;
  VOID                                 *SourceData;
} SYSTEM_GENERIC_ERROR_SOURCE;

//
// Memory Error sub-type definitions
//
//
#define MEM_ERROR                       0xf0
#define MEM_LINK_ERROR                  0x01
#define MEM_LANE_FAILOVER               0x02
#define MEM_DDR_PARITY_ERROR            0x03
#define MEM_CRC_ERROR                   0x04
#define MEM_ECC_ERROR                   0x05
#define MEM_MIRROR_FAILOVER             0x06

#define EFI_WHEA_ENABLE_SWSMI           0x9E
#define EFI_WHEA_DISABLE_SWSMI          0x9D

typedef struct {
    UINT64  ValidBits;
    UINT16  UefiErrorRecordRevision;
    UINT16  ErrorType;
    UINT16  Node;
    UINT16  Branch;
    UINT16  Channel;
    UINT16  VmseCh;
    UINT16  Dimm;
    UINT16  Device;
    UINT16  Bank;
    UINT16  Rank;
    UINT16  Row;
    UINT16  Column;
    UINT64  PhyAddr;
    UINT16  SmBiosCardHandle;    // Type 16 handle;
    UINT16  SmBiosModuleHandle;	 // Type 17 handle;
} MEMORY_DEV_INFO;

typedef struct {
  UINT8                                Segment;
  UINT8                                Bus;
  UINT8                                Device;
  UINT8                                Function;
} PCIE_PCI_DEV_INFO;

#pragma pack()

typedef
EFI_STATUS
(EFIAPI *WHEA_PLATFORM_ELOG_RESTART_ALL) (
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This
);

typedef
EFI_STATUS
(EFIAPI *WHEA_PLATFORM_ELOG_RESTART) (
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            Type
);

typedef
EFI_STATUS
(EFIAPI *WHEA_PLATFORM_ELOG_END) (
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This
);

typedef
EFI_STATUS
(EFIAPI *WHEA_PLATFORM_ELOG_MEMORY) (
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType,
  IN MEMORY_DEV_INFO                   *MemInfo
);

typedef
EFI_STATUS
(EFIAPI *WHEA_PLATFORM_ELOG_PCIE_ROOT_DEV_BRIDGE) (
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev
);

typedef
EFI_STATUS
(EFIAPI *WHEA_PLATFORM_ELOG_PCI_DEV) (
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev
);

typedef struct _EFI_WHEA_PLATFORM_PROTOCOL {
  WHEA_PLATFORM_ELOG_RESTART_ALL                 InsydeWheaPlatformRestartAllElog;
  WHEA_PLATFORM_ELOG_RESTART                     InsydeWheaPlatformRestartElog;
  WHEA_PLATFORM_ELOG_END                         InsydeWheaPlatformEndElog;
  WHEA_PLATFORM_ELOG_MEMORY                      InsydeWheaPlatformElogMemory;
  WHEA_PLATFORM_ELOG_PCIE_ROOT_DEV_BRIDGE        InsydeWheaPlatformElogPcieRootDevBridge;
  WHEA_PLATFORM_ELOG_PCI_DEV                     InsydeWheaPlatformElogPciDev;
};

extern EFI_GUID  gEfiWheaPlatformProtocolGuid;

#endif  

