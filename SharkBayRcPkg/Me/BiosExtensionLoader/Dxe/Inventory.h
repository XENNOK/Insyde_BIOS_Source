/** @file

  Include for AMT Bios Extentions Loader Inventory Functions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _INVENTORY_H_
#define _INVENTORY_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/IderControllerDriver.h>

///
/// Common part of the PCI configuration space header for devices, P2P bridges,
/// and cardbus bridges
///
typedef struct {
  UINT16  VendorId;
  UINT16  DeviceId;

  UINT16  Command;
  UINT16  Status;

  UINT8   RevisionId;
  UINT8   ClassCode[3];

  UINT8   CacheLineSize;
  UINT8   PrimaryLatencyTimer;
  UINT8   HeaderType;
  UINT8   BIST;

} PCI_COMMON_HEADER;

///
/// PCI configuration space header for devices(after the common part)
///
typedef struct {
  UINT32  Bar[6];           /// Base Address Registers
  UINT32  CardBusCISPtr;    /// CardBus CIS Pointer
  UINT16  SubVendorId;      /// Subsystem Vendor ID
  UINT16  SubSystemId;      /// Subsystem ID
  UINT32  ROMBar;           /// Expansion ROM Base Address
  UINT8   CapabilitiesPtr;  /// Capabilities Pointer
  UINT8   Reserved[3];

  UINT32  Reserved1;

  UINT8   InterruptLine;    ///< Interrupt Line
  UINT8   InterruptPin;     ///< Interrupt Pin
  UINT8   MinGnt;           ///< Min_Gnt
  UINT8   MaxLat;           ///< Max_Lat
} PCI_DEVICE_HEADER;

///
/// PCI configuration space header for pci-to-pci bridges(after the common part)
///
typedef struct {
  UINT32  Bar[2];                 ///< Base Address Registers
  UINT8   PrimaryBus;             ///< Primary Bus Number
  UINT8   SecondaryBus;           ///< Secondary Bus Number
  UINT8   SubordinateBus;         ///< Subordinate Bus Number
  UINT8   SecondaryLatencyTimer;  ///< Secondary Latency Timer
  UINT8   IoBase;                 ///< I/O Base
  UINT8   IoLimit;                ///< I/O Limit
  UINT16  SecondaryStatus;        ///< Secondary Status
  UINT16  MemoryBase;             ///< Memory Base
  UINT16  MemoryLimit;            ///< Memory Limit
  UINT16  PrefetchableMemBase;    ///< Pre-fetchable Memory Base
  UINT16  PrefetchableMemLimit;   ///< Pre-fetchable Memory Limit
  UINT32  PrefetchableBaseUpper;  ///< Pre-fetchable Base Upper 32 bits
  UINT32  PrefetchableLimitUpper; ///< Pre-fetchable Limit Upper 32 bits
  UINT16  IoBaseUpper;            ///< I/O Base Upper 16 bits
  UINT16  IoLimitUpper;           ///< I/O Limit Upper 16 bits
  UINT8   CapabilitiesPtr;        ///< Capabilities Pointer
  UINT8   Reserved[3];

  UINT32  ROMBar;                 ///< Expansion ROM Base Address
  UINT8   InterruptLine;          ///< Interrupt Line
  UINT8   InterruptPin;           ///< Interrupt Pin
  UINT16  BridgeControl;          ///< Bridge Control
} PCI_BRIDGE_HEADER;

///
/// PCI configuration space header for cardbus bridges(after the common part)
///
typedef struct {
  UINT32  CardBusSocketReg;       ///< Cardus Socket/ExCA Base Address Register
  UINT16  Reserved;
  UINT16  SecondaryStatus;        ///< Secondary Status
  UINT8   PciBusNumber;           ///< PCI Bus Number
  UINT8   CardBusBusNumber;       ///< CardBus Bus Number
  UINT8   SubordinateBusNumber;   ///< Subordinate Bus Number
  UINT8   CardBusLatencyTimer;    ///< CardBus Latency Timer
  UINT32  MemoryBase0;            ///< Memory Base Register 0
  UINT32  MemoryLimit0;           ///< Memory Limit Register 0
  UINT32  MemoryBase1;
  UINT32  MemoryLimit1;
  UINT32  IoBase0;
  UINT32  IoLimit0;               ///< I/O Base Register 0
  UINT32  IoBase1;                ///< I/O Limit Register 0
  UINT32  IoLimit1;
  UINT8   InterruptLine;          ///< Interrupt Line
  UINT8   InterruptPin;           ///< Interrupt Pin
  UINT16  BridgeControl;          ///< Bridge Control
} PCI_CARDBUS_HEADER;

typedef struct {
  PCI_COMMON_HEADER Common;
  union {
    PCI_DEVICE_HEADER   Device;
    PCI_BRIDGE_HEADER   Bridge;
    PCI_CARDBUS_HEADER  CardBus;
  } NonCommon;
  UINT32  Data[48];
} PCI_CONFIG_SPACE;

/**
  AMT only need to know removable PCI device information.

  @param[in] None

  @retval EFI_SUCCESS             mAmtPciFru will be update.
  @retval EFI_OUT_OF_RESOURCES    System on-board device list is larger than
                                  MAX_FILTER_OUT_DEVICE_NUMBER supported.
**/
EFI_STATUS
BuildPciFru (
  VOID
  );

/**
  Collect all media devices.

  @param[in] None

  @retval None
**/
VOID
EFIAPI
BuildMediaList (
  VOID
  );

#endif /// _INVENTORY_H_
