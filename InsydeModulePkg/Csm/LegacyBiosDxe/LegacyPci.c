/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosInterface.h"
#include <IndustryStandard/Pci30.h>
#include <PortNumberMap.h>

#define DEFAULT_TIMER_TICK_DURATION     549254

#define PCI_START_ADDRESS(x)   (((x) + 0x7ff) & ~0x7ff)

#define MAX_BRIDGE_INDEX  0x20
typedef struct {
  UINTN PciSegment;
  UINTN PciBus;
  UINTN PciDevice;
  UINTN PciFunction;
  UINT8 PrimaryBus;
  UINT8 SecondaryBus;
  UINT8 SubordinateBus;
} BRIDGE_TABLE;

#define ROM_MAX_ENTRIES 24
BRIDGE_TABLE                        *mBridges = NULL;
UINTN                               *mSortedBridgeIndex = NULL;
UINTN                               mNumberOfBridges = 0x00;
LEGACY_PNP_EXPANSION_HEADER         *mBasePnpPtr;
UINT16                              mBbsRomSegment;
UINTN                               mHandleCount;
EFI_HANDLE                          mVgaHandle;
BOOLEAN                             mIgnoreBbsUpdateFlag;
BOOLEAN                             mVgaInstallationInProgress  = FALSE;
UINT32                              mRomCount                   = 0x00;
ROM_INSTANCE_ENTRY                  mRomEntry[ROM_MAX_ENTRIES];
BOOLEAN                             mDispatchOemHook = FALSE;

/**
  Query shadowed legacy ROM parameters registered by RomShadow() previously.

  @param  PciHandle        PCI device whos ROM has been shadowed
  @param  DiskStart        DiskStart value from EFI_LEGACY_BIOS_PROTOCOL.InstallPciRom
  @param  DiskEnd          DiskEnd value from EFI_LEGACY_BIOS_PROTOCOL.InstallPciRom
  @param  RomShadowAddress Address where ROM was shadowed
  @param  ShadowedSize     Runtime size of ROM

  @retval EFI_SUCCESS      Query Logging successful.
  @retval EFI_NOT_FOUND    No logged data found about PciHandle.

**/
EFI_STATUS
GetShadowedRomParameters (
  IN EFI_HANDLE                         PciHandle,
  OUT UINT8                             *DiskStart,         OPTIONAL
  OUT UINT8                             *DiskEnd,           OPTIONAL
  OUT VOID                              **RomShadowAddress, OPTIONAL
  OUT UINTN                             *ShadowedSize       OPTIONAL
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINTN               Index;
  UINTN               PciSegment;
  UINTN               PciBus;
  UINTN               PciDevice;
  UINTN               PciFunction;

  //
  // Get the PCI I/O Protocol on PciHandle
  //
  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Get the location of the PCI device
  //
  PciIo->GetLocation (
           PciIo,
           &PciSegment,
           &PciBus,
           &PciDevice,
           &PciFunction
           );

  for(Index = 0; Index < mRomCount; Index++) {
    if ((mRomEntry[Index].PciSegment == PciSegment) &&
        (mRomEntry[Index].PciBus == PciBus)         &&
        (mRomEntry[Index].PciDevice == PciDevice)   &&
        (mRomEntry[Index].PciFunction == PciFunction)) {
      break;
    }
  }

  if (Index == mRomCount) {
    return EFI_NOT_FOUND;
  }

  if (DiskStart != NULL) {
    *DiskStart = mRomEntry[Index].DiskStart;
  }

  if (DiskEnd != NULL) {
    *DiskEnd = mRomEntry[Index].DiskEnd;
  }

  if (RomShadowAddress != NULL) {
    *RomShadowAddress = (VOID *)(UINTN)mRomEntry[Index].ShadowAddress;
  }

  if (ShadowedSize != NULL) {
    *ShadowedSize = mRomEntry[Index].ShadowedSize;
  }

  return EFI_SUCCESS;
}

/**
  Every legacy ROM that is shadowed by the Legacy BIOS driver will be
  registered into this API so that the policy code can know what has
  happend

  @param  PciHandle              PCI device whos ROM is being shadowed
  @param  ShadowAddress          Address that ROM was shadowed
  @param  ShadowedSize           Runtime size of ROM
  @param  DiskStart              DiskStart value from
                                 EFI_LEGACY_BIOS_PROTOCOL.InstallPciRom
  @param  DiskEnd                DiskEnd value from
                                 EFI_LEGACY_BIOS_PROTOCOL.InstallPciRom

  @retval EFI_SUCCESS            Logging successful.
  @retval EFI_OUT_OF_RESOURCES   No remaining room for registering another option
                                 ROM.

**/
EFI_STATUS
RomShadow (
  IN  EFI_HANDLE                                  PciHandle,
  IN  UINT32                                      ShadowAddress,
  IN  UINT32                                      ShadowedSize,
  IN  UINT8                                       DiskStart,
  IN  UINT8                                       DiskEnd
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;

  //
  // See if there is room to register another option ROM
  //
  if (mRomCount >= ROM_MAX_ENTRIES) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Get the PCI I/O Protocol on PciHandle
  //
  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the location of the PCI device
  //
  PciIo->GetLocation (
           PciIo,
           &mRomEntry[mRomCount].PciSegment,
           &mRomEntry[mRomCount].PciBus,
           &mRomEntry[mRomCount].PciDevice,
           &mRomEntry[mRomCount].PciFunction
           );
  mRomEntry[mRomCount].ShadowAddress = ShadowAddress;
  mRomEntry[mRomCount].ShadowedSize  = ShadowedSize;
  mRomEntry[mRomCount].DiskStart     = DiskStart;
  mRomEntry[mRomCount].DiskEnd       = DiskEnd;

  mRomCount++;

  return EFI_SUCCESS;
}


/**
  Return EFI_SUCCESS if PciHandle has had a legacy BIOS ROM shadowed. This
  information represents every call to RomShadow ()

  @param  PciHandle              PCI device to get status for

  @retval EFI_SUCCESS            Legacy ROM loaded for this device
  @retval EFI_NOT_FOUND          No Legacy ROM loaded for this device

**/
EFI_STATUS
IsLegacyRom (
  IN  EFI_HANDLE                PciHandle
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  UINTN               Index;
  UINTN               Segment;
  UINTN               Bus;
  UINTN               Device;
  UINTN               Function;

  //
  // Get the PCI I/O Protocol on PciHandle
  //
  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Get the location of the PCI device
  //
  PciIo->GetLocation (
           PciIo,
           &Segment,
           &Bus,
           &Device,
           &Function
           );

  //
  // See if the option ROM from PciHandle has been previously posted
  //
  for (Index = 0; Index < mRomCount; Index++) {
    if (mRomEntry[Index].PciSegment == Segment &&
        mRomEntry[Index].PciBus == Bus &&
        mRomEntry[Index].PciDevice == Device &&
        mRomEntry[Index].PciFunction == Function
        ) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Find the PC-AT ROM Image in the raw PCI Option ROM. Also return the
  related information from the header.

  @param  Csm16Revision           The PCI interface version of underlying CSM16
  @param  VendorId                Vendor ID of the PCI device
  @param  DeviceId                Device ID of the PCI device
  @param  Rom                     On input pointing to beginning of the raw PCI OpROM
                                  On output pointing to the first legacy PCI OpROM
  @param  ImageSize               On input is the size of Raw PCI Rom
                                  On output is the size of the first legacy PCI ROM
  @param  MaxRuntimeImageLength   The max runtime image length only valid if OpRomRevision >= 3
  @param  OpRomRevision           Revision of the PCI Rom
  @param  ConfigUtilityCodeHeader Pointer to Configuration Utility Code Header

  @retval EFI_SUCCESS             Successfully find the legacy PCI ROM
  @retval EFI_NOT_FOUND           Failed to find the legacy PCI ROM

**/
EFI_STATUS
GetPciLegacyRom (
  IN     UINT16 Csm16Revision,
  IN     UINT16 VendorId,
  IN     UINT16 DeviceId,
  IN OUT VOID   **Rom,
  IN OUT UINTN  *ImageSize,
  OUT    UINTN  *MaxRuntimeImageLength,   OPTIONAL
  OUT    UINT8  *OpRomRevision,           OPTIONAL
  OUT    VOID   **ConfigUtilityCodeHeader OPTIONAL
  )
{
  BOOLEAN                 Match;
  UINT16                  *DeviceIdList;
  EFI_PCI_ROM_HEADER      RomHeader;
  PCI_3_0_DATA_STRUCTURE  *Pcir;
  VOID                    *BackupImage;
  VOID                    *BestImage;
  UINT8                   OpromRevision;

  if (*ImageSize < sizeof (EFI_PCI_ROM_HEADER)) {
    return EFI_NOT_FOUND;
  }

  BestImage     = NULL;
  BackupImage   = NULL;
  RomHeader.Raw = *Rom;
  while (RomHeader.Generic->Signature == PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
    if (*ImageSize < RomHeader.Raw - (UINT8 *) *Rom + RomHeader.Generic->PcirOffset + sizeof (PCI_DATA_STRUCTURE)) {        
      break;
    }

    Pcir = (PCI_3_0_DATA_STRUCTURE *) (RomHeader.Raw + RomHeader.Generic->PcirOffset);
    //
    // Check signature in the PCI Data Structure.
    //
    if (Pcir->Signature != PCI_DATA_STRUCTURE_SIGNATURE) {
      break;
    }

    if ((UINTN)(RomHeader.Raw - (UINT8 *) *Rom) + Pcir->ImageLength * 512 > *ImageSize) {
      break;
    }

    if (Pcir->CodeType == PCI_CODE_TYPE_PCAT_IMAGE) {
      Match = FALSE;
      if (Pcir->VendorId == VendorId) {
        if (Pcir->DeviceId == DeviceId) {
          Match = TRUE;
        } else if ((Pcir->Revision >= 3) && (Pcir->DeviceListOffset != 0)) {
          DeviceIdList = (UINT16 *)(((UINT8 *) Pcir) + Pcir->DeviceListOffset);
          //
          // Checking the device list
          //
          while (*DeviceIdList != 0) {
            if (*DeviceIdList == DeviceId) {
              Match = TRUE;
              break;
            }
            DeviceIdList ++;
          }
        }
      }

      if (Match) {
        if (Csm16Revision >= 0x0300) {
          //
          // Case 1: CSM16 support PCI 3.0
          //
          if (Pcir->Revision >= 3) {
            //
            // case 1.1: meets OpRom 3.0
            //           Perfect!!!
            //
            BestImage  = RomHeader.Raw;
            break;
          } else {
            //
            // case 1.2: meets OpRom 2.x
            //           Store it and try to find the OpRom 3.0
            //
            BackupImage = RomHeader.Raw;
          }
        } else {
          //
          // Case 2: CSM16 doesn't support PCI 3.0
          //
          if (Pcir->Revision >= 3) {
            //
            // case 2.1: meets OpRom 3.0
            //           Store it and try to find the OpRom 2.x
            //
            BackupImage = RomHeader.Raw;
          } else {
            //
            // case 2.2: meets OpRom 2.x
            //           Perfect!!!
            //
            BestImage   = RomHeader.Raw;
            break;
          }
        }
      } else {
        DEBUG ((EFI_D_ERROR, "GetPciLegacyRom - OpRom not match (%04x-%04x)\n", (UINTN)VendorId, (UINTN)DeviceId));
      }
    }

    if ((Pcir->Indicator & 0x80) == 0x80) {
      break;
    } else {
      RomHeader.Raw += 512 * Pcir->ImageLength;
    }
  }

  if (BestImage == NULL) {
    if (BackupImage == NULL) {
      return EFI_NOT_FOUND;
    }
    //
    // The versions of CSM16 and OpRom don't match exactly
    //
    BestImage = BackupImage;
  }
  RomHeader.Raw = BestImage;
  Pcir = (PCI_3_0_DATA_STRUCTURE *) (RomHeader.Raw + RomHeader.Generic->PcirOffset);
  *Rom       = BestImage;
  *ImageSize = Pcir->ImageLength * 512;

  OpromRevision = Pcir->Revision;
  if (OpromRevision == 0) {
    //
    // Using PCIR structure length to recognize the actual revision if PCIR Revision value is zero
    //
    if (Pcir->Length >= 0x1c) {
      OpromRevision = 3;
    } else if (Pcir->Length >= 0x18) {
      OpromRevision = 2;
    }
  }
  if (OpRomRevision != NULL) {
    //
    // Optional return PCI Data Structure revision
    //
    *OpRomRevision = OpromRevision;
  }
  if (MaxRuntimeImageLength != NULL) {
    if ((OpromRevision < 3) || (Pcir->Length < 0x1c)) {
      *MaxRuntimeImageLength = 0;
    } else {
      *MaxRuntimeImageLength = Pcir->MaxRuntimeImageLength * 512;
    }
  }
  if (ConfigUtilityCodeHeader != NULL) {
    //
    // Optional return ConfigUtilityCodeHeaderOffset supported by the PC-AT ROM
    //
    if ((OpromRevision < 3) || (Pcir->Length < 0x1c) || (Pcir->ConfigUtilityCodeHeaderOffset == 0)) {
      *ConfigUtilityCodeHeader = NULL;
    } else {
      *ConfigUtilityCodeHeader = RomHeader.Raw + Pcir->ConfigUtilityCodeHeaderOffset;
    }
  }

  return EFI_SUCCESS;
}

/**
  Build a table of bridge info for PIRQ translation.

  @retval EFI_SUCCESS          New Subordinate bus.
  @retval EFI_NOT_FOUND        No more Subordinate busses.

**/
EFI_STATUS
CreateBridgeTable (
  VOID
  )
{
  EFI_STATUS          Status;
  UINTN               HandleCount;
  UINTN               BridgeIndex;
  UINTN               Index;
  UINTN               Index1;
  EFI_PCI_IO_PROTOCOL *PciIo;
  PCI_TYPE01          PciConfigHeader;
  UINTN               SortedBridgeIndexChange;
  EFI_HANDLE          *HandleBuffer = NULL;
  BRIDGE_TABLE        *TempBridgesTable = NULL;
  UINTN               *TempSortedBridgeIndex = NULL;

  BridgeIndex = 0x00;

  //
  // Assumption is table is built from low bus to high bus numbers.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  //
  // Allocate temp buffer to store PCI bridges info.
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (BRIDGE_TABLE) * (HandleCount),
                  (VOID **) &TempBridgesTable
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  SetMem ((VOID*)TempBridgesTable, sizeof (BRIDGE_TABLE) * (HandleCount), 0);

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (UINTN) * (HandleCount),
                  (VOID **) &TempSortedBridgeIndex
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  SetMem ((VOID*)TempSortedBridgeIndex, sizeof (UINTN) * (HandleCount), 0);

  //
  // Collect P2P Bridges and store into BridgeTable.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0,
                 sizeof (PciConfigHeader) / sizeof (UINT32),
                 &PciConfigHeader
                 );

    if (IS_PCI_P2P (&PciConfigHeader)) {
      PciIo->GetLocation (
               PciIo,
               &TempBridgesTable[BridgeIndex].PciSegment,
               &TempBridgesTable[BridgeIndex].PciBus,
               &TempBridgesTable[BridgeIndex].PciDevice,
               &TempBridgesTable[BridgeIndex].PciFunction
               );

      TempBridgesTable[BridgeIndex].PrimaryBus     = PciConfigHeader.Bridge.PrimaryBus;

      TempBridgesTable[BridgeIndex].SecondaryBus   = PciConfigHeader.Bridge.SecondaryBus;

      TempBridgesTable[BridgeIndex].SubordinateBus = PciConfigHeader.Bridge.SubordinateBus;
      ++BridgeIndex;
    }
  }

  mNumberOfBridges = BridgeIndex;

  //
  // Sort bridges low to high by Secondary bus followed by subordinate bus
  //
  if (mNumberOfBridges > 1) {
    Index = 0;
    do {
      TempSortedBridgeIndex[Index] = Index;
      ++Index;
    } while (Index < mNumberOfBridges);

    for (Index = 0; Index < mNumberOfBridges - 1; Index++) {
      for (Index1 = Index + 1; Index1 < mNumberOfBridges; Index1++) {
        if (TempBridgesTable[Index].SecondaryBus > TempBridgesTable[Index1].SecondaryBus) {
          SortedBridgeIndexChange   = TempSortedBridgeIndex[Index];
          TempSortedBridgeIndex[Index]  = TempSortedBridgeIndex[Index1];
          TempSortedBridgeIndex[Index1] = SortedBridgeIndexChange;
        }

        if ((TempBridgesTable[Index].SecondaryBus == TempBridgesTable[Index1].SecondaryBus) &&
            (TempBridgesTable[Index].SubordinateBus > TempBridgesTable[Index1].SubordinateBus)
            ) {
          SortedBridgeIndexChange   = TempSortedBridgeIndex[Index];
          TempSortedBridgeIndex[Index]  = TempSortedBridgeIndex[Index1];
          TempSortedBridgeIndex[Index1] = SortedBridgeIndexChange;
        }
      }
    }
  }

  //
  // In POST time the CreateBridgeTable () will be called many times.
  // Free the last result.
  //
  if (mBridges != NULL) {
    FreePool (mBridges);
    FreePool (mSortedBridgeIndex);
  }

  //
  // Allocate pool for Bridges and SortedBridgeIndex to store PCI bridge info.
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (BRIDGE_TABLE) * (mNumberOfBridges),
                  (VOID **) &mBridges
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  CopyMem ((VOID*)mBridges, (VOID*)TempBridgesTable, sizeof (BRIDGE_TABLE) * (mNumberOfBridges));

  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (UINTN) * (mNumberOfBridges),
                  (VOID **) &mSortedBridgeIndex
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    goto Done;
  }

  CopyMem ((VOID*)mSortedBridgeIndex, (VOID*)TempSortedBridgeIndex, sizeof (UINTN) * (mNumberOfBridges));

  Done:

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if (TempBridgesTable != NULL) {
    FreePool (TempBridgesTable);
  }

  if (TempSortedBridgeIndex != NULL) {
    FreePool (TempSortedBridgeIndex);
  }

  return Status;
}


/**
  Find base Bridge for device.

  @param  Private                Legacy  BIOS Instance data
  @param  PciBus                 Input = Bus of device.
  @param  PciDevice              Input = Device.
  @param  RoutingTable           The platform specific routing table
  @param  RoutingTableEntries    Number of entries in table

  @retval EFI_SUCCESS            At base bus.
  @retval EFI_NOT_FOUND          Behind a bridge.

**/
EFI_STATUS
GetBaseBus (
  IN  LEGACY_BIOS_INSTANCE        *Private,
  IN  UINTN                        PciBus,
  IN  UINTN                        PciDevice,
  IN  EFI_LEGACY_IRQ_ROUTING_ENTRY *RoutingTable,
  IN  UINTN                        RoutingTableEntries
  )
{
  UINTN Index;
  for (Index = 0; Index < RoutingTableEntries; Index++) {
    if ((RoutingTable[Index].Bus == PciBus) && (RoutingTable[Index].Device == (PciDevice << 3))) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Translate PIRQ through busses

  @param  Private              Legacy  BIOS Instance data
  @param  PciBus               Input = Bus of device. Output = Translated Bus
  @param  PciDevice            Input = Device. Output = Translated Device
  @param  PciFunction          Input = Function. Output = Translated Function
  @param  PirqIndex            Input = Original PIRQ index. If single function
                                  device then 0, otherwise 0-3.
                               Output = Translated Index
  @param  RoutingTable         The platform specific routing table
  @param  RoutingTableEntries  Number of entries in table

  @retval EFI_SUCCESS          Pirq successfully translated.
  @retval EFI_NOT_FOUND        The device is not behind any known bridge.

**/
EFI_STATUS
TranslateBusPirq (
  IN  LEGACY_BIOS_INSTANCE            *Private,
  IN OUT UINTN                        *PciBus,
  IN OUT UINTN                        *PciDevice,
  IN OUT UINTN                        *PciFunction,
  IN OUT UINT8                        *PirqIndex,
  IN     EFI_LEGACY_IRQ_ROUTING_ENTRY *RoutingTable,
  IN     UINTN                        RoutingTableEntries
  )
{
  /*
  This routine traverses the PCI busses from base slot
  and translates the PIRQ register to the appropriate one.

  Example:

  Bus 0, Device 1 is PCI-PCI bridge that all PCI slots reside on.
    Primary bus# = 0
    Secondary bus # = 1
    Subordinate bus # is highest bus # behind this bus
       Bus 1, Device 0 is Slot 0 and is not a bridge.
       Bus 1, Device 1 is Slot 1 and is a bridge.
         Slot PIRQ routing is A,B,C,D.
         Primary bus # = 1
         Secondary bus # = 2
         Subordinate bus # = 5
            Bus 2, Device 6 is a bridge. It has no bridges behind it.
              Primary bus # = 2
              Secondary bus # = 3
              Subordinate bus # = 3
              Bridge PIRQ routing is C,D,A,B
            Bus 2, Device 7 is a bridge. It has 1 bridge behind it.
              Primary bus # = 2
              Secondary bus = 4   Device 6 takes bus 2.
              Subordinate bus = 5.
              Bridge PIRQ routing is D,A,B,C
                 Bus 4, Device 2 is a bridge. It has no bridges behind it.
                   Primary bus # = 4
                   Secondary bus # = 5
                   Subordinate bus = 5
                   Bridge PIRQ routing is B,C,D,A
                      Bus 5, Device 1 is to be programmed.
                         Device PIRQ routing is C,D,A,B


Search busses starting from slot bus for final bus >= Secondary bus and
final bus <= Suborninate bus. Assumption is bus entries increase in bus
number.
Starting PIRQ is A,B,C,D.
Bus 2, Device 7 satisfies search criteria. Rotate (A,B,C,D) left by device
  7 modulo 4 giving (D,A,B,C).
Bus 4, Device 2 satisfies search criteria. Rotate (D,A,B,C) left by 2 giving
  (B,C,D,A).
No other busses match criteria. Device to be programmed is Bus 5, Device 1.
Rotate (B,C,D,A) by 1 giving C,D,A,B. Translated PIRQ is C.

*/
  UINTN LocalBus;
  UINTN LocalDevice;
  UINTN BaseBus;
  UINTN BaseDevice;
  UINTN BaseFunction;
  UINT8 LocalPirqIndex;
  BOOLEAN BaseIndexFlag;
  UINTN BridgeIndex;
  UINTN SBridgeIndex;
  UINTN      ShiftCount;
  EFI_STATUS Status;

  BaseIndexFlag   = FALSE;
  BridgeIndex     = 0x00;
  ShiftCount      = 0;
  LocalPirqIndex  = *PirqIndex;
  LocalBus        = *PciBus;
  LocalDevice     = *PciDevice;
  BaseBus         = *PciBus;
  BaseDevice      = *PciDevice;
  BaseFunction    = *PciFunction;

  for (BridgeIndex = 0; BridgeIndex < mNumberOfBridges; BridgeIndex++) {
    SBridgeIndex = mSortedBridgeIndex[BridgeIndex];
     //
     // Searching the base bridge from node (EndPoint) to root (Root Bridge).
     // The base bridge is the first bridge which be created by add-in card.
     // It means that base bridge has the slot routing info.
     // Check if device behind this bridge
     //
    if ((LocalBus >= mBridges[SBridgeIndex].SecondaryBus) && (LocalBus <= mBridges[SBridgeIndex].SubordinateBus)) {
      //
      // If BaseIndexFlag = FALSE then have found base bridge, i.e
      // bridge in slot. Save info for use by IRQ routing table.
      //
      if (!BaseIndexFlag) {
        BaseBus       = mBridges[SBridgeIndex].PciBus;
        BaseDevice    = mBridges[SBridgeIndex].PciDevice;
        BaseFunction  = mBridges[SBridgeIndex].PciFunction;
        BaseIndexFlag = TRUE;
      }
      //
      // Try to find the base bridge by checking the routing table.
      // Base bridge must has the slot routing info.
      //
      Status = GetBaseBus (
                 Private,
                 mBridges[SBridgeIndex].PciBus,
                 mBridges[SBridgeIndex].PciDevice,
                 RoutingTable,
                 RoutingTableEntries
                 );

      if (Status == EFI_SUCCESS) {
        ShiftCount = 0;
        BaseBus       = mBridges[SBridgeIndex].PciBus;
        BaseDevice    = mBridges[SBridgeIndex].PciDevice;
        BaseFunction  = mBridges[SBridgeIndex].PciFunction;
      } else {
        if (BaseBus != mBridges[SBridgeIndex].PciBus) {
          //
          //
          // Virtual and actual P2P bridge must map the INT# x on the secondary side of bridge
          // according to the device number of the secondary side.
          // Use the ShiftCount to store the bridge's device number.
          // All bridge's device number (except the base bridge) should be counted to shift INT A/B/C/D
          //
          ShiftCount += mBridges[SBridgeIndex].PciDevice;
        }
      }

      if (mBridges[SBridgeIndex].SecondaryBus == (UINT8) LocalBus) {
        //
        // If the device is found, shift INT A/B/C/D and break
        //
        ShiftCount += LocalDevice;
        break;
      }
    }
  }

  LocalPirqIndex = (UINT8) ((LocalPirqIndex + ShiftCount) % 4);

  *PirqIndex    = LocalPirqIndex;
  *PciBus       = BaseBus;
  *PciDevice    = BaseDevice;
  *PciFunction  = BaseFunction;

  return EFI_SUCCESS;
}


/**
  Copy the $PIR table as required.

  @param  Private                Legacy  BIOS Instance data
  @param  RoutingTable           Pointer to IRQ routing table
  @param  RoutingTableEntries    IRQ routing table entries
  @param  PirqTable              Pointer to $PIR table
  @param  PirqTableSize          Length of table

**/
VOID
CopyPirqTable (
  IN  LEGACY_BIOS_INSTANCE                *Private,
  IN EFI_LEGACY_IRQ_ROUTING_ENTRY         *RoutingTable,
  IN UINTN                                RoutingTableEntries,
  IN EFI_LEGACY_PIRQ_TABLE_HEADER         *PirqTable,
  IN UINTN                                PirqTableSize
  )
{
  EFI_IA32_REGISTER_SET Regs;
  UINT32                Granularity;

  //
  // Copy $PIR table, if it exists.
  //
  if (PirqTable != NULL) {
    Private->LegacyRegion->UnLock (
                            Private->LegacyRegion,
                            0xE0000,
                            0x20000,
                            &Granularity
                            );

    Private->InternalIrqRoutingTable  = RoutingTable;
    Private->NumberIrqRoutingEntries  = (UINT16) (RoutingTableEntries);
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));

    Regs.X.AX = Legacy16GetTableAddress;
    Regs.X.CX = (UINT16) PirqTableSize;
    //
    // Allocate at F segment according to PCI IRQ Routing Table Specification
    //
    Regs.X.BX = (UINT16) 0x1;
    //
    // 16-byte boundary alignment requirement according to
    // PCI IRQ Routing Table Specification
    //
    Regs.X.DX = 0x10;
    Private->LegacyBios.FarCall86 (
      &Private->LegacyBios,
      Private->Legacy16CallSegment,
      Private->Legacy16CallOffset,
      &Regs,
      NULL,
      0
      );

    Private->Legacy16Table->IrqRoutingTablePointer = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);
    if (Regs.X.AX != 0) {
      DEBUG ((EFI_D_ERROR, "PIRQ table length insufficient - %x\n", PirqTableSize));
    } else {
      DEBUG ((EFI_D_INFO, "PIRQ table in legacy region - %x\n", Private->Legacy16Table->IrqRoutingTablePointer));
      Private->Legacy16Table->IrqRoutingTableLength = (UINT32)PirqTableSize;
      CopyMem (
        (VOID *) (UINTN)Private->Legacy16Table->IrqRoutingTablePointer,
        PirqTable,
        PirqTableSize
        );
    }

    Private->Cpu->FlushDataCache (Private->Cpu, 0xE0000, 0x20000, EfiCpuFlushTypeWriteBackInvalidate);
    Private->LegacyRegion->Lock (
                             Private->LegacyRegion,
                             0xE0000,
                             0x20000,
                             &Granularity
                             );
  }

  Private->PciInterruptLine = TRUE;
  mHandleCount              = 0;
}

/**
  Dump EFI_LEGACY_INSTALL_PCI_HANDLER structure information.

  @param  PciHandle               The pointer to EFI_LEGACY_INSTALL_PCI_HANDLER structure

**/
VOID
DumpPciHandle (
  IN EFI_LEGACY_INSTALL_PCI_HANDLER  *PciHandle
  )
{
  DEBUG ((EFI_D_INFO, "PciBus             - %02x\n", (UINTN)PciHandle->PciBus));
  DEBUG ((EFI_D_INFO, "PciDeviceFun       - %02x\n", (UINTN)PciHandle->PciDeviceFun));
  DEBUG ((EFI_D_INFO, "PciSegment         - %02x\n", (UINTN)PciHandle->PciSegment));
  DEBUG ((EFI_D_INFO, "PciClass           - %02x\n", (UINTN)PciHandle->PciClass));
  DEBUG ((EFI_D_INFO, "PciSubclass        - %02x\n", (UINTN)PciHandle->PciSubclass));
  DEBUG ((EFI_D_INFO, "PciInterface       - %02x\n", (UINTN)PciHandle->PciInterface));

  DEBUG ((EFI_D_INFO, "PrimaryIrq         - %02x\n", (UINTN)PciHandle->PrimaryIrq));
  DEBUG ((EFI_D_INFO, "PrimaryReserved    - %02x\n", (UINTN)PciHandle->PrimaryReserved));
  DEBUG ((EFI_D_INFO, "PrimaryControl     - %04x\n", (UINTN)PciHandle->PrimaryControl));
  DEBUG ((EFI_D_INFO, "PrimaryBase        - %04x\n", (UINTN)PciHandle->PrimaryBase));
  DEBUG ((EFI_D_INFO, "PrimaryBusMaster   - %04x\n", (UINTN)PciHandle->PrimaryBusMaster));

  DEBUG ((EFI_D_INFO, "SecondaryIrq       - %02x\n", (UINTN)PciHandle->SecondaryIrq));
  DEBUG ((EFI_D_INFO, "SecondaryReserved  - %02x\n", (UINTN)PciHandle->SecondaryReserved));
  DEBUG ((EFI_D_INFO, "SecondaryControl   - %04x\n", (UINTN)PciHandle->SecondaryControl));
  DEBUG ((EFI_D_INFO, "SecondaryBase      - %04x\n", (UINTN)PciHandle->SecondaryBase));
  DEBUG ((EFI_D_INFO, "SecondaryBusMaster - %04x\n", (UINTN)PciHandle->SecondaryBusMaster));
  return;
}

/**
  Copy the $PIR table as required.

  @param  Private                Legacy  BIOS Instance data
  @param  PciIo                  Pointer to PCI_IO protocol
  @param  PciIrq                 Pci IRQ number
  @param  PciConfigHeader        Type00 Pci configuration header

**/
VOID
InstallLegacyIrqHandler (
  IN LEGACY_BIOS_INSTANCE       *Private,
  IN EFI_PCI_IO_PROTOCOL        *PciIo,
  IN UINT8                      PciIrq,
  IN PCI_TYPE00                 *PciConfigHeader
  )
{
  EFI_IA32_REGISTER_SET     Regs;
  UINT16                    LegMask;
  UINTN                     PciSegment;
  UINTN                     PciBus;
  UINTN                     PciDevice;
  UINTN                     PciFunction;
  EFI_LEGACY_8259_PROTOCOL  *Legacy8259;
  UINT16                    PrimaryMaster;
  UINT16                    SecondaryMaster;
  UINTN                     TempData;
  UINTN                     RegisterAddress;
  UINT32                    Granularity;
  UINT16                    RegValue;

  PrimaryMaster   = 0;
  SecondaryMaster = 0;
  Legacy8259      = Private->Legacy8259;
  //
  // Disable interrupt in PIC, in case shared, to prevent an
  // interrupt from occuring.
  //
  Legacy8259->GetMask (
                Legacy8259,
                &LegMask,
                NULL,
                NULL,
                NULL
                );

  LegMask = (UINT16) (LegMask | (UINT16) (1 << PciIrq));

  Legacy8259->SetMask (
                Legacy8259,
                &LegMask,
                NULL,
                NULL,
                NULL
                );

  PciIo->GetLocation (
          PciIo,
          &PciSegment,
          &PciBus,
          &PciDevice,
          &PciFunction
          );
  Private->IntThunk->PciHandler.PciBus              = (UINT8) PciBus;
  Private->IntThunk->PciHandler.PciDeviceFun        = (UINT8) ((PciDevice << 3) + PciFunction);
  Private->IntThunk->PciHandler.PciSegment          = (UINT8) PciSegment;
  Private->IntThunk->PciHandler.PciClass            = PciConfigHeader->Hdr.ClassCode[2];
  Private->IntThunk->PciHandler.PciSubclass         = PciConfigHeader->Hdr.ClassCode[1];
  Private->IntThunk->PciHandler.PciInterface        = PciConfigHeader->Hdr.ClassCode[0];

  //
  // Use native mode base address registers in two cases:
  // 1. Programming Interface (PI) register indicates Primary Controller is
  // in native mode OR
  // 2. PCI device Sub Class Code is not IDE
  //
  Private->IntThunk->PciHandler.PrimaryBusMaster  = (UINT16)(PciConfigHeader->Device.Bar[4] & 0xfffc);
  if (((PciConfigHeader->Hdr.ClassCode[0] & 0x01) != 0) || (PciConfigHeader->Hdr.ClassCode[1] != PCI_CLASS_MASS_STORAGE_IDE)) {
    Private->IntThunk->PciHandler.PrimaryIrq      = PciIrq;
    Private->IntThunk->PciHandler.PrimaryBase     = (UINT16) (PciConfigHeader->Device.Bar[0] & 0xfffc);
    Private->IntThunk->PciHandler.PrimaryControl  = (UINT16) ((PciConfigHeader->Device.Bar[1] & 0xfffc) + 2);
  } else {
    Private->IntThunk->PciHandler.PrimaryIrq      = 14;
    Private->IntThunk->PciHandler.PrimaryBase     = 0x1f0;
    Private->IntThunk->PciHandler.PrimaryControl  = 0x3f6;
  }
  //
  // Secondary controller data
  //
  if (Private->IntThunk->PciHandler.PrimaryBusMaster != 0) {
    Private->IntThunk->PciHandler.SecondaryBusMaster  = (UINT16) ((PciConfigHeader->Device.Bar[4] & 0xfffc) + 8);
    PrimaryMaster = (UINT16) (Private->IntThunk->PciHandler.PrimaryBusMaster + 2);
    SecondaryMaster = (UINT16) (Private->IntThunk->PciHandler.SecondaryBusMaster + 2);

    //
    // Clear pending interrupts in Bus Master registers
    //

    RegValue = IoRead16 (PrimaryMaster) | 0x04;
    IoWrite16 (PrimaryMaster, RegValue);
    RegValue = IoRead16 (SecondaryMaster) | 0x04;
    IoWrite16 (SecondaryMaster, RegValue);

  }

  //
  // Use native mode base address registers in two cases:
  // 1. Programming Interface (PI) register indicates Secondary Controller is
  // in native mode OR
  // 2. PCI device Sub Class Code is not IDE
  //
  if (((PciConfigHeader->Hdr.ClassCode[0] & 0x04) != 0) || (PciConfigHeader->Hdr.ClassCode[1] != PCI_CLASS_MASS_STORAGE_IDE)) {
    Private->IntThunk->PciHandler.SecondaryIrq      = PciIrq;
    Private->IntThunk->PciHandler.SecondaryBase     = (UINT16) (PciConfigHeader->Device.Bar[2] & 0xfffc);
    Private->IntThunk->PciHandler.SecondaryControl  = (UINT16) ((PciConfigHeader->Device.Bar[3] & 0xfffc) + 2);
  } else {

    Private->IntThunk->PciHandler.SecondaryIrq      = 15;
    Private->IntThunk->PciHandler.SecondaryBase     = 0x170;
    Private->IntThunk->PciHandler.SecondaryControl  = 0x376;
  }

  //
  // Clear pending interrupts in IDE Command Block Status reg before we
  // Thunk to CSM16 below.  Don't want a pending Interrupt before we
  // install the handlers as wierd corruption would occur and hang system.
  //
  //
  // Read IDE CMD blk status reg to clear out any pending interrupts.
  // Do here for Primary and Secondary IDE channels
  //
  RegisterAddress = (UINT16)Private->IntThunk->PciHandler.PrimaryBase + 0x07;
  IoRead8 (RegisterAddress);
  RegisterAddress = (UINT16)Private->IntThunk->PciHandler.SecondaryBase + 0x07;
  IoRead8 (RegisterAddress);

  Private->IntThunk->PciHandler.PrimaryReserved   = 0;
  Private->IntThunk->PciHandler.SecondaryReserved = 0;
  Private->LegacyRegion->UnLock (
                           Private->LegacyRegion,
                           0xE0000,
                           0x20000,
                           &Granularity
                           );

  Regs.X.AX = Legacy16InstallPciHandler;
  TempData  = (UINTN) &Private->IntThunk->PciHandler;
  Regs.X.ES = EFI_SEGMENT ((UINT32) TempData);
  Regs.X.BX = EFI_OFFSET ((UINT32) TempData);

  DumpPciHandle (&Private->IntThunk->PciHandler);

  Private->LegacyBios.FarCall86 (
    &Private->LegacyBios,
    Private->Legacy16CallSegment,
    Private->Legacy16CallOffset,
    &Regs,
    NULL,
    0
    );

  Private->Cpu->FlushDataCache (Private->Cpu, 0xE0000, 0x20000, EfiCpuFlushTypeWriteBackInvalidate);
  Private->LegacyRegion->Lock (
                           Private->LegacyRegion,
                           0xE0000,
                           0x20000,
                           &Granularity
                           );

}


/**
  Program the interrupt routing register in all the PCI devices. On a PC AT system
  this register contains the 8259 IRQ vector that matches it's PCI interrupt.

  @param  Private                Legacy  BIOS Instance data

  @retval EFI_SUCCESS            Succeed.
  @retval EFI_ALREADY_STARTED    All PCI devices have been processed.

**/
EFI_STATUS
PciProgramAllInterruptLineRegisters (
  IN  LEGACY_BIOS_INSTANCE      *Private
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  EFI_LEGACY_8259_PROTOCOL          *Legacy8259;
  EFI_LEGACY_INTERRUPT_PROTOCOL     *LegacyInterrupt;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *LegacyBiosPlatform;
  UINT8                             InterruptPin;
  UINTN                             Index;
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             MassStorageHandleCount;
  EFI_HANDLE                        *MassStorageHandleBuffer;
  UINTN                             MassStorageHandleIndex;
  UINT8                             PciIrq;
  UINT16                            Command;
  UINTN                             PciSegment;
  UINTN                             PciBus;
  UINTN                             PciDevice;
  UINTN                             PciFunction;
  EFI_LEGACY_IRQ_ROUTING_ENTRY      *RoutingTable;
  UINTN                             RoutingTableEntries;
  UINT16                            LegMask;
  UINT16                            LegEdgeLevel;
  PCI_TYPE00                        PciConfigHeader;
  EFI_LEGACY_PIRQ_TABLE_HEADER      *PirqTable;
  UINTN                             PirqTableSize;
  UINTN                             Flags;
  HDD_INFO                          *HddInfo;
  UINT64                            Supports;

  //
  // Note - This routine use to return immediately if Private->PciInterruptLine
  //        was true. Routine changed since resets etc can cause not all
  //        PciIo protocols to be registered the first time through.
  // New algorithm is to do the copy $PIR table on first pass and save
  // HandleCount on first pass. If subsequent passes LocateHandleBuffer gives
  // a larger handle count then proceed with body of function else return
  // EFI_ALREADY_STARTED. In addition check if PCI device InterruptLine != 0.
  // If zero then function unprogrammed else skip function.
  //
  Legacy8259          = Private->Legacy8259;
  LegacyInterrupt     = Private->LegacyInterrupt;
  LegacyBiosPlatform  = Private->LegacyBiosPlatform;
  CreateBridgeTable ();
  LegacyBiosPlatform->GetRoutingTable (
                        Private->LegacyBiosPlatform,
                        (VOID *) &RoutingTable,
                        &RoutingTableEntries,
                        (VOID *) &PirqTable,
                        &PirqTableSize,
                        NULL,
                        NULL
                        );

  if (!Private->PciInterruptLine) {
    CopyPirqTable (
      Private,
      RoutingTable,
      RoutingTableEntries,
      PirqTable,
      PirqTableSize
      );
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  if (HandleCount == mHandleCount) {
    FreePool (HandleBuffer);
    return EFI_ALREADY_STARTED;
  }

  if (mHandleCount == 0x00) {
    mHandleCount = HandleCount;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    //
    // If VGA then only do VGA to allow drives fore time to spin up
    // otherwise assign PCI IRQs to all potential devices.
    //
    if ((mVgaInstallationInProgress) && (HandleBuffer[Index] != mVgaHandle)) {
      continue;
    } else {
      //
      // Force code to go through all handles next time called if video.
      // This will catch case where HandleCount doesn't change but want
      //  to get drive info etc.
      //
      mHandleCount = 0x00;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    //
    // Test whether the device can be enabled or not.
    // If it can't be enabled, then just skip it to avoid further operation.
    //
    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0,
                 sizeof (PciConfigHeader) / sizeof (UINT32),
                 &PciConfigHeader
                 );
    Command = PciConfigHeader.Hdr.Command;

    //
    // Note PciIo->Attributes does not program the PCI command register
    //
    Status = PciIo->Attributes (
                      PciIo,
                      EfiPciIoAttributeOperationSupported,
                      0,
                      &Supports
                      );
    if (!EFI_ERROR (Status)) {
      Supports &= EFI_PCI_DEVICE_ENABLE;
      Status = PciIo->Attributes (
                        PciIo,
                        EfiPciIoAttributeOperationEnable,
                        Supports,
                        NULL
                        );
    }
    PciIo->Pci.Write (PciIo, EfiPciIoWidthUint16, 0x04, 1, &Command);

    if (EFI_ERROR (Status)) {
      continue;
    }

    InterruptPin = PciConfigHeader.Device.InterruptPin;

    if ((InterruptPin == 0) && (IS_PCI_BRIDGE(&PciConfigHeader))) {

      PciIrq = PCI_INT_LINE_UNKNOWN;

      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint8,
                   0x3c,
                   1,
                   &PciIrq
                   );
      continue;
    }

    if ((InterruptPin != 0) && (PciConfigHeader.Device.InterruptLine == PCI_INT_LINE_UNKNOWN)) {
      PciIo->GetLocation (
               PciIo,
               &PciSegment,
               &PciBus,
               &PciDevice,
               &PciFunction
               );
      //
      // Translate PIRQ index back thru busses to slot bus with InterruptPin
      // zero based
      //
      InterruptPin -= 1;

      Status = GetBaseBus (
                 Private,
                 PciBus,
                 PciDevice,
                 RoutingTable,
                 RoutingTableEntries
                 );

      if (Status == EFI_NOT_FOUND) {
        //
        // Routing table can not provide the routing info to an add-in card which implements bridges.
        // Pci to Pci Bridge spec provides the "Interrupt Binding" method to solve this problem.
        //
        TranslateBusPirq (
          Private,
          &PciBus,
          &PciDevice,
          &PciFunction,
          &InterruptPin,
          RoutingTable,
          RoutingTableEntries
          );
      }
      //
      // Translate InterruptPin(0-3) into PIRQ
      //
      Status = LegacyBiosPlatform->TranslatePirq (
                                     LegacyBiosPlatform,
                                     PciBus,
                                     (PciDevice << 3),
                                     PciFunction,
                                     &InterruptPin,
                                     &PciIrq
                                     );
      //
      // TranslatePirq() should never fail or we are in trouble
      // If it does return failure status, check your PIRQ routing table to see if some item is missing or incorrect
      //
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Translate Pirq Failed - Status = %r\n ", Status));
        continue;
      }

      Status = LegacyInterrupt->WritePirq (
                         LegacyInterrupt,
                         InterruptPin,
                         PciIrq
                         );
      if (EFI_ERROR (Status)) {
        continue;
      }
      //
      // Check if device has an OPROM associated with it.
      // If not invoke special 16-bit function, to allow 16-bit
      // code to install an interrupt handler.
      //
      Status = LegacyBiosCheckPciRom (
                 &Private->LegacyBios,
                 HandleBuffer[Index],
                 NULL,
                 NULL,
                 &Flags
                 );
      if ((EFI_ERROR (Status)) && (PciConfigHeader.Hdr.ClassCode[2] == PCI_CLASS_MASS_STORAGE)) {
        //
        // Device has no OPROM associated with it and is a mass storage
        // device. It needs to have an PCI IRQ handler installed. To
        // correctly install the handler we need to insure device is
        // connected. The device may just have register itself but not
        // been connected. Re-read PCI config space after as it can
        // change
        //
        //
        // Get IDE Handle. If matches handle then skip ConnectController
        // since ConnectController may force native mode and we don't
        // want that for primary IDE controller
        //
        MassStorageHandleCount = 0;
        MassStorageHandleBuffer = NULL;
        LegacyBiosPlatform->GetPlatformHandle (
                              Private->LegacyBiosPlatform,
                              EfiGetPlatformIdeHandle,
                              0,
                              &MassStorageHandleBuffer,
                              &MassStorageHandleCount,
                              NULL
                              );

        HddInfo = &Private->IntThunk->EfiToLegacy16BootTable.HddInfo[0];

        LegacyBiosBuildIdeData (Private, &HddInfo, 0);
        PciIo->Pci.Read (
                     PciIo,
                     EfiPciIoWidthUint32,
                     0,
                     sizeof (PciConfigHeader) / sizeof (UINT32),
                     &PciConfigHeader
                     );

        for (MassStorageHandleIndex = 0; MassStorageHandleIndex < MassStorageHandleCount; MassStorageHandleIndex++) {
          if (MassStorageHandleBuffer[MassStorageHandleIndex] == HandleBuffer[Index]) {
            //
            // InstallLegacyIrqHandler according to Platform requirement
            //
            InstallLegacyIrqHandler (
              Private,
              PciIo,
              PciIrq,
              &PciConfigHeader
              );
            break;
          }
        }
      }
      //
      // Write InterruptPin and enable 8259.
      //
      PciIo->Pci.Write (
                   PciIo,
                   EfiPciIoWidthUint8,
                   0x3c,
                   1,
                   &PciIrq
                   );
      Private->IntThunk->EfiToLegacy16BootTable.PciIrqMask = (UINT16) (Private->IntThunk->EfiToLegacy16BootTable.PciIrqMask | (UINT16) (1 << PciIrq));

      Legacy8259->GetMask (
                    Legacy8259,
                    &LegMask,
                    &LegEdgeLevel,
                    NULL,
                    NULL
                    );

      LegMask       = (UINT16) (LegMask & (UINT16)~(1 << PciIrq));
      LegEdgeLevel  = (UINT16) (LegEdgeLevel | (UINT16) (1 << PciIrq));
      Legacy8259->SetMask (
                    Legacy8259,
                    &LegMask,
                    &LegEdgeLevel,
                    NULL,
                    NULL
                    );
    }
  }
  FreePool (HandleBuffer);
  return EFI_SUCCESS;
}


/**
  Find & verify PnP Expansion header in ROM image

  @param  Private                Protocol instance pointer.
  @param  FirstHeader            1 = Find first header, 0 = Find successive headers
  @param  PnpPtr                 Input Rom start if FirstHeader =1, Current Header
                                 otherwise Output Next header, if it exists

  @retval EFI_SUCCESS            Next Header found at BasePnpPtr
  @retval EFI_NOT_FOUND          No more headers

**/
EFI_STATUS
FindNextPnpExpansionHeader (
  IN  LEGACY_BIOS_INSTANCE             *Private,
  IN BOOLEAN                           FirstHeader,
  IN OUT LEGACY_PNP_EXPANSION_HEADER   **PnpPtr

  )
{
  UINTN                       TempData;
  LEGACY_PNP_EXPANSION_HEADER *LocalPnpPtr;
  LocalPnpPtr = *PnpPtr;
  if (FirstHeader == FIRST_INSTANCE) {
    mBasePnpPtr     = LocalPnpPtr;
    mBbsRomSegment  = (UINT16) ((UINTN) mBasePnpPtr >> 4);
    //
    // Offset 0x1a gives offset to PnP expansion header for the first
    // instance, there after the structure gives the offset to the next
    // structure
    //
    LocalPnpPtr = (LEGACY_PNP_EXPANSION_HEADER *) ((UINT8 *) LocalPnpPtr + 0x1a);
    TempData    = (*((UINT16 *) LocalPnpPtr));
  } else {
    TempData = (UINT16) LocalPnpPtr->NextHeader;
  }

  LocalPnpPtr = (LEGACY_PNP_EXPANSION_HEADER *) (((UINT8 *) mBasePnpPtr + TempData));

  //
  // Search for PnP table in Shadowed ROM
  //
  *PnpPtr = LocalPnpPtr;
  if (*(UINT32 *) LocalPnpPtr == SIGNATURE_32 ('$', 'P', 'n', 'P')) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}


/**
  Update list of Bev or BCV table entries.

  @param  Private                Protocol instance pointer.
  @param  RomStart               Table of ROM start address in RAM/ROM. PciIo  _
                                 Handle to PCI IO for this device
  @param  PciIo                  Instance of PCI I/O Protocol

  @retval EFI_SUCCESS            Always should succeed.
**/
EFI_STATUS
UpdateBevBcvTable (
  IN  LEGACY_BIOS_INSTANCE             *Private,
  IN  EFI_LEGACY_EXPANSION_ROM_HEADER  *RomStart,
  IN  EFI_PCI_IO_PROTOCOL              *PciIo
  )
{
  VOID                            *RomEnd;
  BBS_TABLE                       *BbsTable;
  UINTN                           BbsIndex;
  EFI_LEGACY_EXPANSION_ROM_HEADER *PciPtr;
  LEGACY_PNP_EXPANSION_HEADER     *PnpPtr;
  BOOLEAN                         Instance;
  EFI_STATUS                      Status;
  UINTN                           Segment;
  UINTN                           Bus;
  UINTN                           Device;
  UINTN                           Function;
  UINT8                           Class;
  UINT8                           SubClass;
  UINT16                          DeviceType;

  Segment     = 0;
  Bus         = 0;
  Device      = 0;
  Function    = 0;
  Class       = 0;
  DeviceType  = BBS_UNKNOWN;
  SubClass    = 0;

  //
  // Skip floppy and 2*onboard IDE controller entries(Master/Slave per
  // controller).
  //
  BbsIndex  = Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries;

  BbsTable  = (BBS_TABLE*)(UINTN) Private->IntThunk->EfiToLegacy16BootTable.BbsTable;
  PnpPtr    = (LEGACY_PNP_EXPANSION_HEADER *) RomStart;
  PciPtr    = (EFI_LEGACY_EXPANSION_ROM_HEADER *) RomStart;

  RomEnd    = (VOID *) (PciPtr->Size512 * 512 + (UINTN) PciPtr);
  Instance  = FIRST_INSTANCE;

  //
  // OPROMs like PXE may not be tied to a piece of hardware and thus
  // don't have a PciIo associated with them
  //
  if (PciIo != NULL) {
    PciIo->GetLocation (
             PciIo,
             &Segment,
             &Bus,
             &Device,
             &Function
             );
    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint8,
                 0x0b,
                 1,
                 &Class
                 );
    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint8,
                 0x0a,
                 1,
                 &SubClass
                 );

    if (Class == PCI_CLASS_MASS_STORAGE) {
      DeviceType = BBS_HARDDISK;
    } else {
      if (Class == PCI_CLASS_NETWORK) {
        DeviceType = BBS_EMBED_NETWORK;
      }
    }
  }

  while (TRUE) {
    Status    = FindNextPnpExpansionHeader (Private, Instance, &PnpPtr);
    Instance  = NOT_FIRST_INSTANCE;
    if (EFI_ERROR (Status)) {
      break;
    }
    //
    // There can be additional $PnP headers within the OPROM.
    // Example: SCSI can have one per drive.
    //
    BbsTable[BbsIndex].BootPriority             = BBS_UNPRIORITIZED_ENTRY;
    BbsTable[BbsIndex].DeviceType               = DeviceType;
    BbsTable[BbsIndex].Bus                      = (UINT32) Bus;
    BbsTable[BbsIndex].Device                   = (UINT32) Device;
    BbsTable[BbsIndex].Function                 = (UINT32) Function;
    BbsTable[BbsIndex].StatusFlags.OldPosition  = 0;
    BbsTable[BbsIndex].StatusFlags.Reserved1    = 0;
    BbsTable[BbsIndex].StatusFlags.Enabled      = 0;
    BbsTable[BbsIndex].StatusFlags.Failed       = 0;
    BbsTable[BbsIndex].StatusFlags.MediaPresent = 0;
    BbsTable[BbsIndex].StatusFlags.Reserved2    = 0;
    BbsTable[BbsIndex].Class                    = Class;
    BbsTable[BbsIndex].SubClass                 = SubClass;
    BbsTable[BbsIndex].DescStringOffset         = PnpPtr->ProductNamePointer;
    BbsTable[BbsIndex].DescStringSegment        = mBbsRomSegment;
    BbsTable[BbsIndex].MfgStringOffset          = PnpPtr->MfgPointer;
    BbsTable[BbsIndex].MfgStringSegment         = mBbsRomSegment;
    BbsTable[BbsIndex].BootHandlerSegment       = mBbsRomSegment;

    //
    // Have seen case where PXE base code have PnP expansion ROM
    // header but no Bcv or Bev vectors.
    //
    if (PnpPtr->Bcv != 0) {
      BbsTable[BbsIndex].BootHandlerOffset = PnpPtr->Bcv;
      ++BbsIndex;
    }

    if (PnpPtr->Bev != 0) {
      BbsTable[BbsIndex].BootHandlerOffset  = PnpPtr->Bev;
      BbsTable[BbsIndex].DeviceType         = BBS_BEV_DEVICE;
      ++BbsIndex;
    }

    if ((PnpPtr == (LEGACY_PNP_EXPANSION_HEADER *) PciPtr) || (PnpPtr > (LEGACY_PNP_EXPANSION_HEADER *) RomEnd)) {
      break;
    }
  }

  BbsTable[BbsIndex].BootPriority = BBS_IGNORE_ENTRY;
  Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries = (UINT32) BbsIndex;
  return EFI_SUCCESS;
}


/**
  According the Bus, Device, Function to check this controller is in Port Number Map table or not.
  If yes, then this is a on board PCI device.

  @param  Bus                    Pci Bus number.
  @param  Device                 Pci Device number.
  @param  Function               Pci Function number.

  @retval TRUE                   Onboard Pci device.
  @retval FALSE                  Not Onboard Pci device.
**/
BOOLEAN
IsOnBoardPciDevice (
  IN     UINT32                              Bus,
  IN     UINT32                              Device,
  IN     UINT32                              Function
  )
{
  UINTN                         Index;
  PORT_NUMBER_MAP               *PortMappingTable;
  PORT_NUMBER_MAP               EndEntry;
  UINTN                         NoPorts;
  
  PortMappingTable      = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  if (NoPorts == 0) {
    return FALSE;
  }

  for (Index = 0; Index < NoPorts; Index++) {
    if ((PortMappingTable[Index].Bus == Bus) &&
        (PortMappingTable[Index].Device == Device) &&
        (PortMappingTable[Index].Function == Function)) {
      return TRUE;
    }
  }

  return FALSE;
}




/**
  Shadow all the PCI legacy ROMs. Use data from the Legacy BIOS Protocol
  to chose the order. Skip any devices that have already have legacy
  BIOS run.

  @param  Private                Protocol instance pointer.

  @retval EFI_SUCCESS            Succeed.
  @retval EFI_UNSUPPORTED        Cannot get VGA device handle.

**/
EFI_STATUS
PciShadowRoms (
  IN  LEGACY_BIOS_INSTANCE      *Private
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  PCI_TYPE00                        Pci;
  UINTN                             Index;
  UINTN                             HandleCount = 0;
  EFI_HANDLE                        *HandleBuffer;
  EFI_HANDLE                        VgaHandle = NULL;
  EFI_HANDLE                        FirstHandle;
  VOID                              **RomStart;
  UINTN                             Flags;
  PCI_TYPE00                        PciConfigHeader;
  UINT16                            *Command;
  UINT64                            Supports;
  UINTN                             Segment;
  UINTN                             Bus;
  UINTN                             Device;
  UINTN                             Function;

  //
  // Make the VGA device first
  //
  Status = Private->LegacyBiosPlatform->GetPlatformHandle (
                                          Private->LegacyBiosPlatform,
                                          EfiGetPlatformVgaHandle,
                                          0,
                                          &HandleBuffer,
                                          &HandleCount,
                                          NULL
                                          );
  if (HandleCount != 0) {
    VgaHandle = HandleBuffer[0];
  }
  
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Place the VGA handle as first.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    if (HandleBuffer[Index] == VgaHandle) {
      FirstHandle         = HandleBuffer[0];
      HandleBuffer[0]     = HandleBuffer[Index];
      HandleBuffer[Index] = FirstHandle;
      break;
    }
  }
  //
  // Allocate memory to save Command WORD from each device. We do this
  // to restore devices to same state as EFI after switching to legacy.
  //
  Command = (UINT16 *) AllocatePool (
                         sizeof (UINT16) * (HandleCount + 1)
                         );
  if (NULL == Command) {
    FreePool (HandleBuffer);
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Disconnect all EFI devices first. This covers cases where alegacy BIOS
  // may control multiple PCI devices.
  //
  for (Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    //
    // Save command register for "connect" loop
    //
    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0,
                 sizeof (PciConfigHeader) / sizeof (UINT32),
                 &PciConfigHeader
                 );
    Command[Index] = PciConfigHeader.Hdr.Command;
    //
    // Skip any device that has no option ROM attached
    //
    Status = LegacyBiosCheckPciRom (
               &Private->LegacyBios,
               HandleBuffer[Index],
               NULL,
               NULL,
               &Flags
               );
    if (EFI_ERROR (Status)) {
      continue;
    }
    //
    // Skip any device that already has a legacy ROM run
    //
    Status = IsLegacyRom (HandleBuffer[Index]);
    if (!EFI_ERROR (Status)) {
      continue;
    }
    //
    // Stop EFI Drivers with oprom.
    //
    gBS->DisconnectController (
           HandleBuffer[Index],
           NULL,
           NULL
           );
  }
  //
  // For every device that has not had a legacy ROM started. Start a legacy ROM.
  //
  for (Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );

    ASSERT_EFI_ERROR (Status);

    //
    // Here make sure if one VGA have been shadowed,
    // then wil not shadowed another one.
    //
    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0,
                 sizeof (Pci) / sizeof (UINT32),
                 &Pci
                 );

    //
    // Only one Video OPROM can be given control in BIOS phase. If there are multiple Video devices,
    // one will work in legacy mode (OPROM will be given control) and
    // other Video devices will work in native mode (OS driver will handle these devices).
    //
    if (IS_PCI_DISPLAY (&Pci) && Index != 0) {
      continue;
    }

    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);

    //
    // Skip any device that already has a legacy ROM run
    //
    Status = IsLegacyRom (HandleBuffer[Index]);
    if (!EFI_ERROR (Status)) {
      continue;
    }
    //
    // Install legacy ROM by using Private->LegacyBios.InstallPciRom instead of
    // static LegacyBiosInstallPciRom function call to make shim function has
    // chance to be called 
    //
    Status = Private->LegacyBios.InstallPciRom (
               &Private->LegacyBios,
               HandleBuffer[Index],
               NULL,
               &Flags,
               NULL,
               NULL,
               (VOID **) &RomStart,
               NULL
               );
    if (EFI_ERROR (Status)) {
      if (!((Status == EFI_UNSUPPORTED) && (Flags == NO_ROM))) {
        continue;
      }
    }
    //
    // Restore Command register so legacy has same devices enabled or disabled
    // as EFI.
    // If Flags = NO_ROM use command register as is. This covers the
    //            following cases:
    //              Device has no ROMs associated with it.
    //              Device has ROM associated with it but was already
    //              installed.
    //          = ROM_FOUND but not VALID_LEGACY_ROM, disable it.
    //          = ROM_FOUND and VALID_LEGACY_ROM, enable it.
    //
    if ((Flags & ROM_FOUND) == ROM_FOUND) {
      if ((Flags & VALID_LEGACY_ROM) == 0) {
        Command[Index] = 0;
      } else {
        //
        // For several VGAs, only one of them can be enabled.
        //
        Status = PciIo->Attributes (
                          PciIo,
                          EfiPciIoAttributeOperationSupported,
                          0,
                          &Supports
                          );
        if (!EFI_ERROR (Status)) {
          Supports &= EFI_PCI_DEVICE_ENABLE;
          Status = PciIo->Attributes (
                            PciIo,
                            EfiPciIoAttributeOperationEnable,
                            Supports,
                            NULL
                            );
        }
        if (!EFI_ERROR (Status)) {
          Command[Index] = 0x1f;
        }
      }
    }

    PciIo->Pci.Write (
                 PciIo,
                 EfiPciIoWidthUint16,
                 0x04,
                 1,
                 &Command[Index]
                 );
  }

  FreePool (Command);
  FreePool (HandleBuffer);
  return EFI_SUCCESS;
}


/**
  Test to see if a legacy PCI ROM exists for this device. Optionally return
  the Legacy ROM instance for this PCI device.

  @param  This                   Protocol instance pointer.
  @param  PciHandle              The PCI PC-AT OPROM from this devices ROM BAR will
                                 be loaded
  @param  RomImage               Return the legacy PCI ROM for this device
  @param  RomSize                Size of ROM Image
  @param  Flags                  Indicates if ROM found and if PC-AT.

  @retval EFI_SUCCESS            Legacy Option ROM availible for this device
  @retval EFI_UNSUPPORTED        Legacy Option ROM not supported.

**/
EFI_STATUS
EFIAPI
LegacyBiosCheckPciRom (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This,
  IN  EFI_HANDLE                        PciHandle,
  OUT VOID                              **RomImage, OPTIONAL
  OUT UINTN                             *RomSize, OPTIONAL
  OUT UINTN                             *Flags
  )
{
  return LegacyBiosCheckPciRomEx (
           This,
           PciHandle,
           RomImage,
           RomSize,
           NULL,
           Flags,
           NULL,
           NULL
           );

}

/**

  Routine Description:
    Test to see if a legacy PCI ROM exists for this device. Optionally return
    the Legacy ROM instance for this PCI device.

    @param[in] This          Protocol instance pointer.
    @param[in] PciHandle               The PCI PC-AT OPROM from this devices ROM BAR will be loaded
    @param[out] RomImage               Return the legacy PCI ROM for this device
    @param[out] RomSize                Size of ROM Image
    @param[out] RuntimeImageLength     Runtime size of ROM Image
    @param[out] Flags                  Indicates if ROM found and if PC-AT.
    @param[out] OpromRevision          Revision of the PCI Rom
    @param[out] ConfigUtilityCodeHeaderPointer of Configuration Utility Code Header

    @return EFI_SUCCESS            Legacy Option ROM availible for this device
    @return EFI_ALREADY_STARTED    This device is already managed by its Oprom
    @return EFI_UNSUPPORTED        Legacy Option ROM not supported.

**/
EFI_STATUS
LegacyBiosCheckPciRomEx (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This,
  IN  EFI_HANDLE                        PciHandle,
  OUT VOID                              **RomImage, OPTIONAL
  OUT UINTN                             *RomSize, OPTIONAL
  OUT UINTN                             *RuntimeImageLength, OPTIONAL
  OUT UINTN                             *Flags, OPTIONAL
  OUT UINT8                             *OpromRevision, OPTIONAL
  OUT VOID                              **ConfigUtilityCodeHeader OPTIONAL
  )
{
  EFI_STATUS                      Status;
  LEGACY_BIOS_INSTANCE            *Private;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINTN                           LocalRomSize;
  VOID                            *LocalRomImage;
  PCI_TYPE00                      PciConfigHeader;
  VOID                            *LocalConfigUtilityCodeHeader;

  *Flags = NO_ROM;
  Status = gBS->HandleProtocol (
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // See if the option ROM for PciHandle has already been executed
  //
  Status = IsLegacyRom (PciHandle);
  if (!EFI_ERROR (Status)) {
    *Flags |= (ROM_FOUND | VALID_LEGACY_ROM);
    return EFI_SUCCESS;
  }
  //
  // Check for PCI ROM Bar
  //
  LocalRomSize  = (UINTN) PciIo->RomSize;
  LocalRomImage = PciIo->RomImage;
  if (LocalRomSize != 0) {
    *Flags |= ROM_FOUND;
  }

  //
  // PCI specification states you should check VendorId and Device Id.
  //
  PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0,
               sizeof (PciConfigHeader) / sizeof (UINT32),
               &PciConfigHeader
               );

  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  Status = GetPciLegacyRom (
             Private->Csm16PciInterfaceVersion,
             PciConfigHeader.Hdr.VendorId,
             PciConfigHeader.Hdr.DeviceId,
             &LocalRomImage,
             &LocalRomSize,
             RuntimeImageLength,
             OpromRevision,
             &LocalConfigUtilityCodeHeader
             );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  *Flags |= VALID_LEGACY_ROM;

  //
  // See if Configuration Utility Code Header valid
  //
  if (LocalConfigUtilityCodeHeader != NULL) {
    *Flags |= ROM_WITH_CONFIG;
  }

  if (ConfigUtilityCodeHeader != NULL) {
    *ConfigUtilityCodeHeader = LocalConfigUtilityCodeHeader;
  }

  if (RomImage != NULL) {
    *RomImage = LocalRomImage;
  }

  if (RomSize != NULL) {
    *RomSize = LocalRomSize;
  }

  return EFI_SUCCESS;
}

/**
  Load a legacy PC-AT OPROM on the PciHandle device. Return information
  about how many disks were added by the OPROM and the shadow address and
  size. DiskStart & DiskEnd are INT 13h drive letters. Thus 0x80 is C:

  @retval EFI_SUCCESS   Legacy ROM loaded for this device
  @retval EFI_NOT_FOUND No PS2 Keyboard found

**/
EFI_STATUS
EnablePs2Keyboard (
  VOID
  )
{
  EFI_STATUS                          Status;
  EFI_HANDLE                          *HandleBuffer;
  UINTN                               HandleCount;
  EFI_ISA_IO_PROTOCOL                 *IsaIo;
  UINTN                               Index;

  //
  // Get SimpleTextIn and find PS2 controller
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleTextInProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    //
    // Open the IO Abstraction(s) needed to perform the supported test
    //
    Status = gBS->OpenProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaIoProtocolGuid,
                    (VOID **) &IsaIo,
                    NULL,
                    HandleBuffer[Index],
                    EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL
                    );

    if (!EFI_ERROR (Status)) {
      //
      // Use the ISA I/O Protocol to see if Controller is the Keyboard
      // controller
      //
      if (IsaIo->ResourceList->Device.HID != EISA_PNP_ID (0x303) || IsaIo->ResourceList->Device.UID != 0) {
        Status = EFI_UNSUPPORTED;
      }

      gBS->CloseProtocol (
             HandleBuffer[Index],
             &gEfiIsaIoProtocolGuid,
             NULL,
             HandleBuffer[Index]
             );
    }

    if (!EFI_ERROR (Status)) {
      gBS->ConnectController (HandleBuffer[Index], NULL, NULL, FALSE);
    }
  }
  FreePool (HandleBuffer);
  return EFI_SUCCESS;
}


/**
  Load a legacy PC-AT OpROM for VGA controller.

  @param  Private                Driver private data.

  @retval EFI_SUCCESS            Legacy ROM successfully installed for this device.
  @retval EFI_DEVICE_ERROR       No VGA device handle found, or native EFI video
                                 driver cannot be successfully disconnected, or VGA
                                 thunk driver cannot be successfully connected.

**/
EFI_STATUS
LegacyBiosInstallVgaRom (
  IN  LEGACY_BIOS_INSTANCE            *Private
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           VgaHandle;
  UINTN                                HandleCount;
  EFI_HANDLE                           *HandleBuffer;
  EFI_HANDLE                           *ConnectHandleBuffer;
  EFI_PCI_IO_PROTOCOL                  *PciIo;
  PCI_TYPE00                           PciConfigHeader;
  UINT64                               Supports;
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY  *OpenInfoBuffer;
  UINTN                                EntryCount;
  UINTN                                Index;
  VOID                                 *Interface;

  //
  // EfiLegacyBiosGuild attached to a device implies that there is a legacy
  // BIOS associated with that device.
  //
  // There are 3 cases to consider.
  //   Case 1: No EFI driver is controlling the video.
  //     Action: Return EFI_SUCCESS from DisconnectController, search
  //             video thunk driver, and connect it.
  //   Case 2: EFI driver is controlling the video and EfiLegacyBiosGuid is
  //           not on the image handle.
  //     Action: Disconnect EFI driver.
  //             ConnectController for video thunk
  //   Case 3: EFI driver is controlling the video and EfiLegacyBiosGuid is
  //           on the image handle.
  //     Action: Do nothing and set Private->VgaInstalled = TRUE.
  //             Then this routine is not called any more.
  //
  //
  // Get the VGA device.
  //
  Status = Private->LegacyBiosPlatform->GetPlatformHandle (
                                          Private->LegacyBiosPlatform,
                                          EfiGetPlatformVgaHandle,
                                          0,
                                          &HandleBuffer,
                                          &HandleCount,
                                          NULL
                                          );
  if (EFI_ERROR (Status)) {
    return EFI_DEVICE_ERROR;
  }

  VgaHandle = HandleBuffer[0];

  //
  // Check whether video thunk driver already starts.
  //
  Status = gBS->OpenProtocolInformation (
                  VgaHandle,
                  &gEfiPciIoProtocolGuid,
                  &OpenInfoBuffer,
                  &EntryCount
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < EntryCount; Index++) {
    if ((OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_DRIVER) != 0) {
      Status = gBS->HandleProtocol (
                      OpenInfoBuffer[Index].AgentHandle,
                      &gEfiLegacyBiosGuid,
                      (VOID **) &Interface
                      );
      if (!EFI_ERROR (Status)) {
        //
        // This should be video thunk driver which is managing video device
        // So it need not start again
        //
        DEBUG ((EFI_D_INFO, "Video thunk driver already start! Return!\n"));
        Private->VgaInstalled = TRUE;
        return EFI_SUCCESS;
      }
    }
  }

  //
  // Kick off the native EFI driver
  //
  Status = gBS->DisconnectController (
                  VgaHandle,
                  NULL,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    if (Status != EFI_NOT_FOUND) {
      return EFI_DEVICE_ERROR;
    } else {
      return Status;
    }
  }
  //
  // Find all the Thunk Driver
  //
  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLegacyBiosGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);
  ConnectHandleBuffer = (EFI_HANDLE *) AllocatePool (sizeof (EFI_HANDLE) * (HandleCount + 1));
  ASSERT (ConnectHandleBuffer != NULL);
  if(ConnectHandleBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }      

  CopyMem (
    ConnectHandleBuffer,
    HandleBuffer,
    sizeof (EFI_HANDLE) * HandleCount
    );
  ConnectHandleBuffer[HandleCount] = NULL;

  FreePool (HandleBuffer);

  //
  // Enable the device and make sure VGA cycles are being forwarded to this VGA device
  //
  Status = gBS->HandleProtocol (
                  VgaHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo
                  );
  ASSERT_EFI_ERROR (Status);
  PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0,
               sizeof (PciConfigHeader) / sizeof (UINT32),
               &PciConfigHeader
               );

  Status = PciIo->Attributes (
                    PciIo,
                    EfiPciIoAttributeOperationSupported,
                    0,
                    &Supports
                    );
  if (!EFI_ERROR (Status)) {
    Supports &= EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | \
                EFI_PCI_IO_ATTRIBUTE_VGA_IO | EFI_PCI_IO_ATTRIBUTE_VGA_IO_16;
    Status = PciIo->Attributes (
                      PciIo,
                      EfiPciIoAttributeOperationEnable,
                      Supports,
                      NULL
                      );
  }

  if (Status == EFI_SUCCESS) {
    Private->VgaInstalled = TRUE;

    //
    // Attach the VGA thunk driver.
    // Assume the video is installed. This prevents potential of infinite recursion.
    //
    Status = gBS->ConnectController (
                    VgaHandle,
                    ConnectHandleBuffer,
                    NULL,
                    TRUE
                    );
  }

  FreePool (ConnectHandleBuffer);

  if (EFI_ERROR (Status)) {

    Private->VgaInstalled = FALSE;

    //
    // Reconnect the EFI VGA driver.
    //
    gBS->ConnectController (VgaHandle, NULL, NULL, TRUE);
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Dispatch Oem Hook Rom image.

  @param  This                   Protocol instance pointer.
  @param  Private                Driver's private data

  @retval EFI_SUCCESS            Dispatch Oem Hook Rom Image Success
  @retval EFI_ALREADY_STARTED    Oem Hook Rom Images already dispatched
  @retval EFI_LOAD_ERROR         Load the image failed or not found Oem Rom image
**/
EFI_STATUS
DispatchOemHooK(
  IN    EFI_LEGACY_BIOS_PROTOCOL  * This,
  IN    LEGACY_BIOS_INSTANCE      * Private
)
{
  VOID                  *Table;
  UINTN                 TableSize;
  UINTN                 Location;
  UINTN                 Alignment;
  EFI_IA32_REGISTER_SET Regster;
  UINTN                 TablePtr;
  EFI_STATUS            Status;
  UINT16                Count;

  if (mDispatchOemHook) {
    return EFI_ALREADY_STARTED;
  }

  mDispatchOemHook = TRUE;
  for (Count = 0; ; Count++) {
    Status = Private->LegacyBiosPlatform->GetPlatformInfo(
                                            Private->LegacyBiosPlatform,
                                            EfiGetPlatformBinaryOemIntData,
                                            (VOID *) &Table,
                                            &TableSize,
                                            &Location,
                                            &Alignment,
                                            Count,
                                            0
                                            );
    if (EFI_ERROR (Status)) {
      if (Count == 0) {
        return Status;
      } else {
        return EFI_SUCCESS;
      }
    }

    //
    // To find the required size of availabe free memory
    //
    ZeroMem (&Regster, sizeof (EFI_IA32_REGISTER_SET));

    Regster.X.AX = Legacy16GetTableAddress;
    Regster.X.CX = (UINT16) TableSize;
    Regster.X.BX = (UINT16) Location;
    Regster.X.DX = (UINT16) Alignment;

    Status = Private->LegacyBios.FarCall86 (
                                   &Private->LegacyBios,
                                   Private->Legacy16CallSegment,
                                   Private->Legacy16CallOffset,
                                   &Regster,
                                   NULL,
                                   0
                                   );
    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }

    //
    // To adjust the Segment and Offset
    //
    Private->Legacy16Table->OemIntSegment  = Regster.X.DS + (Regster.X.BX >> 4);
    Private->Legacy16Table->OemIntOffset   = 0;

    //
    // To copy the image to legacy memory
    //
    TablePtr = (UINT32) (Regster.X.DS * 16 + Regster.X.BX);

    CopyMem((VOID *) TablePtr, Table, TableSize);

    //
    // call into our BIN file for change the INT15
    //
    Status = Private->LegacyBios.FarCall86 (
                                   &Private->LegacyBios,
                                   Private->Legacy16Table->OemIntSegment,
                                   Private->Legacy16Table->OemIntOffset + 0x03,
                                   &Regster,
                                   NULL,
                                   0
                                   );
    if (EFI_ERROR (Status)) {
      return EFI_LOAD_ERROR;
    }
  }
}

/**
  If there are the same controller on the same bus, those controllers should be enabled before dispatch Oprom.

  @param  Private                Driver's private data
  @param  PciIo                  Instance of PCI I/O Protocol

  @retval EFI_SUCCESS            Enables those other controllers success
  @retval EFI_UNSUPPORTED        No any Pci device
  @retval EFI_NOT_FOUND          No any same vid/did on the same controllers
  @retval Other                  failed to enables those other controllers
**/
EFI_STATUS
CheckOtherController (
  IN  LEGACY_BIOS_INSTANCE            *Private,
  IN  EFI_PCI_IO_PROTOCOL             *PciIo
)
{
  EFI_STATUS                  Status;
  UINT32                      VidDid;
  UINT32                      OtherVidDid;
  UINTN                       PciSegment;
  UINTN                       PciBus;
  UINTN                       PciDevice;
  UINTN                       PciFunction;
  UINTN                       OtherPciSegment;
  UINTN                       OtherPciBus;
  UINTN                       OtherPciDevice;
  UINTN                       OtherPciFunction;
  UINTN                       Index, HandleCount;
  EFI_HANDLE                  *HandleBuffer;
  EFI_PCI_IO_PROTOCOL         *OtherPciIo;

  //
  // Get Device ID and vendor ID
  //
  PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0,
               1,
               &VidDid
               );
  //
  // Get controller's address
  //
  PciIo->GetLocation (
           PciIo,
           &PciSegment,
           &PciBus,
           &PciDevice,
           &PciFunction
           );

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiPciIoProtocolGuid,
         NULL,
         &HandleCount,
         &HandleBuffer
         );
  if (HandleCount == 0) {
    Status = EFI_UNSUPPORTED;
  }

  Status = EFI_NOT_FOUND;
  //
  // Get all PciIo protocols. If controller's bus number is the same with 'PciIo', check the VID/DID.
  // If VID/DID is equaled, it means there are two controllers on a PCI card.
  //
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&OtherPciIo
                    );
    if (EFI_ERROR(Status)) {
      Status = EFI_NOT_FOUND;
      continue;
    }

    if (OtherPciIo == PciIo) {
      continue;
    }

    //
    // Get controller's address
    //
    OtherPciIo->GetLocation (
                  PciIo,
                  &OtherPciSegment,
                  &OtherPciBus,
                  &OtherPciDevice,
                  &OtherPciFunction
                  );

    if (PciBus != OtherPciBus) {
      continue;
    }

    OtherPciIo->Pci.Read (
                      OtherPciIo,
                      EfiPciIoWidthUint32,
                      0,
                      1,
                      &OtherVidDid
                      );
    if (OtherVidDid != VidDid) {
      continue;
    }

    //
    // If VId/Did is the same and controllers on the same bus, enables controllers.
    //
    Status = OtherPciIo->Attributes (
                           OtherPciIo,
                           EfiPciIoAttributeOperationEnable,
                           EFI_PCI_DEVICE_ENABLE,
                           NULL
                           );
  }

  return Status;
}


/**
  Load a legacy PC-AT OpROM.

  @param  This                   Protocol instance pointer.
  @param  Private                Driver's private data.
  @param  PciHandle              The EFI handle for the PCI device. It could be
                                 NULL if the  OpROM image is not associated with
                                 any device.
  @param  OpromRevision          The revision of PCI PC-AT ROM image.
  @param  RomImage               Pointer to PCI PC-AT ROM image header. It must not be NULL.
  @param  ImageSize              Size of the PCI PC-AT ROM image.
  @param  RuntimeImageLength     On input is the max runtime image length indicated by the PCIR structure
                                 On output is the actual runtime image length
  @param  DiskStart              Disk number of first device hooked by the ROM. If
                                 DiskStart is the same as DiskEnd no disked were hooked.
  @param  DiskEnd                Disk number of the last device hooked by the ROM.
  @param  RomShadowAddress       Shadow address of PC-AT ROM

  @retval EFI_SUCCESS            Legacy ROM loaded for this device
  @retval EFI_OUT_OF_RESOURCES   No more space for this ROM
**/
EFI_STATUS
EFIAPI
LegacyBiosInstallRom (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This,
  IN LEGACY_BIOS_INSTANCE               *Private,
  IN EFI_HANDLE                         PciHandle,
  IN UINT8                              OpromRevision,
  IN VOID                               *RomImage,
  IN UINTN                              ImageSize,
  IN OUT UINTN                          *RuntimeImageLength,
  OUT UINT8                             *DiskStart, OPTIONAL
  OUT UINT8                             *DiskEnd, OPTIONAL
  OUT VOID                              **RomShadowAddress OPTIONAL
  )
{
  EFI_STATUS            Status;
  EFI_STATUS            PciEnableStatus;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINT8                 LocalDiskStart;
  UINT8                 LocalDiskEnd;
  UINTN                 Segment;
  UINTN                 Bus;
  UINTN                 Device;
  UINTN                 Function;
  EFI_IA32_REGISTER_SET Regs;
  UINT8                 VideoMode;
  EFI_TIME              BootTime;
  UINT32                *BdaPtr;
  UINT32                LocalTime;
  UINT32                StartBbsIndex;
  UINT32                EndBbsIndex;
  UINTN                 TempData;
  UINTN                 InitAddress;
  UINTN                 RuntimeAddress;
  EFI_PHYSICAL_ADDRESS  PhysicalAddress;
  UINT32                Granularity;
  BOOLEAN               IsOpRomTempFree;
  UINT32                EndOpromShadowAddress;
  UINTN                 UnlockLength;
  UINT64                TimerPeriod;
  EFI_TIMER_ARCH_PROTOCOL *Timer;

  PciIo           = NULL;
  LocalDiskStart  = 0;
  LocalDiskEnd    = 0;
  Segment         = 0;
  Bus             = 0;
  Device          = 0;
  Function        = 0;
  VideoMode       = 0;
  PhysicalAddress = 0;
  IsOpRomTempFree = FALSE;

  PciProgramAllInterruptLineRegisters (Private);

  if ((EndOpromShadowAddress = PcdGet32 (PcdEndOpromShadowAddress)) == 0) {
    EndOpromShadowAddress = Private->BiosStart;
  }

  if ((OpromRevision >= 3) && (Private->Csm16PciInterfaceVersion >= 0x0300)) {
    //
    // CSM16 3.0 meets PCI 3.0 OpROM
    //   first test if there is enough space for its INIT code
    //
    PhysicalAddress = CONVENTIONAL_MEMORY_TOP;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiBootServicesCode,
                    EFI_SIZE_TO_PAGES (ImageSize),
                    &PhysicalAddress
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "return LegacyBiosInstallRom(%d): EFI_OUT_OF_RESOURCES (no more space for OpROM)\n", __LINE__));
      return EFI_OUT_OF_RESOURCES;
    }
    InitAddress = (UINTN) PhysicalAddress;
    //
    //  then test if there is enough space for its RT code
    //
    //
    //  According to PCI Firmware Specification Revision 3.0
    //
    //  5.2.1.17. Expansion ROM Placement Alignment
    //
    //      Traditional ISA legacy Expansion ROMs were placed in memory at addresses aligned on a 2-KB
    //    boundary. This tradition has carried through into PCI and it results in an average 1-KB gap between
    //    Expansion ROMs.
    //
    //      PCI 3.0 compliant Expansion ROMs may be placed at addresses evenly aligned on a 512-byte
    //    boundary. However, the BIOS must be aware of legacy operating systems and legacy applications
    //    that will continue to search for Expansion ROMs on the traditional 2-KB boundaries. The BIOS is
    //    responsible for determining if there are unacceptable risks in placing the Option ROMs at nonlegacy
    //    addresses.
    //
    RuntimeAddress    = PCI_START_ADDRESS (Private->OptionRom);
    if (RuntimeAddress + *RuntimeImageLength > EndOpromShadowAddress) {
      DEBUG ((EFI_D_ERROR, "return LegacyBiosInstallRom(%d): EFI_OUT_OF_RESOURCES (no more space for OpROM)\n", __LINE__));
      gBS->FreePages (PhysicalAddress, EFI_SIZE_TO_PAGES (ImageSize));
      return EFI_OUT_OF_RESOURCES;
    }
  } else {
    // CSM16 3.0 meets PCI 2.x OpROM
    // CSM16 2.x meets PCI 2.x/3.0 OpROM
    //   test if there is enough space for its INIT code
    //
    InitAddress    = PCI_START_ADDRESS (Private->OptionRom);
    if (InitAddress + ImageSize > EndOpromShadowAddress) {
      DEBUG ((EFI_D_ERROR, "return LegacyBiosInstallRom(%d): EFI_OUT_OF_RESOURCES (no more space for OpROM)\n", __LINE__));
      return EFI_OUT_OF_RESOURCES;
    }

    RuntimeAddress = InitAddress;
  }

  Private->LegacyRegion->UnLock (
                           Private->LegacyRegion,
                           0xE0000,
                           0x20000,
                           &Granularity
                           );

  //
  // Set legacy region unlock length to proper length(large than zero) in case it may crashed in unlock routine
  //
  UnlockLength = ImageSize;
  if (OpromRevision >= 3 && Private->Csm16PciInterfaceVersion >= 0x0300 && *RuntimeImageLength > 0) {
    UnlockLength = *RuntimeImageLength;
  }
  Status = Private->LegacyRegion->UnLock (
                                    Private->LegacyRegion,
                                    (UINT32)RuntimeAddress,
                                    (UINT32)UnlockLength,
                                    &Granularity
                                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
   goto Done;
  }

  DEBUG ((EFI_D_INFO, " Shadowing OpROM init/runtime/isize = %x/%x/%x\n", InitAddress, RuntimeAddress, ImageSize));

  CopyMem ((VOID *) InitAddress, RomImage, ImageSize);

  //
  // Read the highest disk number "installed: and assume a new disk will
  // show up on the first drive past the current value.
  // There are several considerations here:
  // 1. Non-BBS compliant drives will change 40:75 but 16-bit CSM will undo
  //    the change until boot selection time frame.
  // 2. BBS compliants drives will not change 40:75 until boot time.
  // 3. Onboard IDE controllers will change 40:75
  //
  LocalDiskStart = (UINT8) ((*(UINT8 *) ((UINTN) 0x475)) + 0x80);
  if ((Private->Disk4075 + 0x80) < LocalDiskStart) {
    //
    // Update table since onboard IDE drives found
    //
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciSegment        = 0xff;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciBus            = 0xff;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciDevice         = 0xff;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciFunction       = 0xff;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].StartDriveNumber  = (UINT8) (Private->Disk4075 + 0x80);
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].EndDriveNumber    = LocalDiskStart;
    Private->LegacyEfiHddTableIndex ++;
    Private->Disk4075 = (UINT8) (LocalDiskStart & 0x7f);
    Private->DiskEnd  = LocalDiskStart;
  }

  if (PciHandle != mVgaHandle) {

    EnablePs2Keyboard ();

    //
    // Store current mode settings since PrepareToScanRom may change mode.
    //
    VideoMode = *(UINT8 *) ((UINTN) (0x400 + BDA_VIDEO_MODE));
  }
  //
  // Notify the platform that we are about to scan the ROM
  //
  Status = Private->LegacyBiosPlatform->PlatformHooks (
                                          Private->LegacyBiosPlatform,
                                          EfiPlatformHookPrepareToScanRom,
                                          0,
                                          PciHandle,
                                          &InitAddress,
                                          NULL,
                                          NULL
                                          );

  //
  // If Status returned is EFI_UNSUPPORTED then abort due to platform
  // policy.
  //
  if (Status == EFI_UNSUPPORTED) {
    goto Done;
  }

  //
  // Report corresponding status code
  //
  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_CSM_LEGACY_ROM_INIT)
    );

  //
  // Generate number of ticks since midnight for BDA. Some OPROMs require
  // this. Place result in 40:6C-6F
  //
  gRT->GetTime (&BootTime, NULL);
  LocalTime = BootTime.Hour * 3600 + BootTime.Minute * 60 + BootTime.Second;

  //
  // Multiply result by 18.2 for number of ticks since midnight.
  // Use 182/10 to avoid floating point math.
  //
  LocalTime = (LocalTime * 182) / 10;
  BdaPtr    = (UINT32 *) ((UINTN) 0x46C);
  *BdaPtr   = LocalTime;
  //
  // Hook Oem Int
  //
  DispatchOemHooK(This, Private);

  //
  // Pass in handoff data
  //
  PciEnableStatus = EFI_SUCCESS;
  ZeroMem (&Regs, sizeof (Regs));
  if (PciHandle != NULL) {

    Status = gBS->HandleProtocol (
                    PciHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    //
    // Enable command register.
    //
    PciEnableStatus = PciIo->Attributes (
                               PciIo,
                               EfiPciIoAttributeOperationEnable,
                               EFI_PCI_DEVICE_ENABLE,
                               NULL
                               );

    PciIo->GetLocation (
             PciIo,
             &Segment,
             &Bus,
             &Device,
             &Function
             );
    DEBUG ((EFI_D_INFO, "Shadowing OpROM on the PCI device %x/%x/%x\n", Bus, Device, Function));

    CheckOtherController (Private, PciIo);
  }

  mIgnoreBbsUpdateFlag  = FALSE;
  Regs.X.AX             = Legacy16DispatchOprom;

  //
  // Generate DispatchOpRomTable data
  //
  Private->IntThunk->DispatchOpromTable.PnPInstallationCheckSegment = Private->Legacy16Table->PnPInstallationCheckSegment;
  Private->IntThunk->DispatchOpromTable.PnPInstallationCheckOffset  = Private->Legacy16Table->PnPInstallationCheckOffset;
  Private->IntThunk->DispatchOpromTable.OpromSegment                = (UINT16) (InitAddress >> 4);
  Private->IntThunk->DispatchOpromTable.PciBus                      = (UINT8) Bus;
  Private->IntThunk->DispatchOpromTable.PciDeviceFunction           = (UINT8) ((Device << 3) | Function);
  Private->IntThunk->DispatchOpromTable.NumberBbsEntries            = (UINT8) Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries;
  Private->IntThunk->DispatchOpromTable.BbsTablePointer             = (UINT32) (UINTN) Private->BbsTablePtr;
  Private->IntThunk->DispatchOpromTable.RuntimeSegment              = (UINT16)((OpromRevision < 3) ? 0xffff : (RuntimeAddress >> 4));
  TempData = (UINTN) &Private->IntThunk->DispatchOpromTable;
  Regs.X.ES = EFI_SEGMENT ((UINT32) TempData);
  Regs.X.BX = EFI_OFFSET ((UINT32) TempData);
  //
  // Skip dispatching ROM for those PCI devices that can not be enabled by PciIo->Attributes
  // Otherwise, it may cause the system to hang in some cases
  //
  if (!EFI_ERROR (PciEnableStatus)) {
    //
    // Get current timer tick and set the timer tick to 18.2hz to make whole timer interrupt
    // handler operate in proper frequency under legacy environment
    //
    TimerPeriod = DEFAULT_TIMER_TICK_DURATION;
    Timer       = NULL;
    Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &Timer);
    if (!EFI_ERROR (Status)) {
      Timer->GetTimerPeriod (Timer, &TimerPeriod);
      if (TimerPeriod != DEFAULT_TIMER_TICK_DURATION) {
        Timer->SetTimerPeriod (Timer, DEFAULT_TIMER_TICK_DURATION);
      }
    }
    DEBUG ((EFI_D_INFO, " Legacy16DispatchOprom - %02x/%02x/%02x\n", Bus, Device, Function));
    Private->LegacyBios.FarCall86 (
                          &Private->LegacyBios,
                          Private->Legacy16CallSegment,
                          Private->Legacy16CallOffset,
                          &Regs,
                          NULL,
                          0
                          );
    //
    // Restore the timer tick to make whole timer interrupt handler operate in original frequency
    //
    if (TimerPeriod != DEFAULT_TIMER_TICK_DURATION && Timer != NULL) {
      Timer->SetTimerPeriod (Timer, TimerPeriod);
    }
  } else {
    Regs.X.BX = 0;
  }

  if (Private->IntThunk->DispatchOpromTable.NumberBbsEntries != (UINT8) Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries) {
    Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries  = (UINT8) Private->IntThunk->DispatchOpromTable.NumberBbsEntries;
    mIgnoreBbsUpdateFlag = TRUE;
  }
  //
  // Check if non-BBS compliant drives found
  //
  if (Regs.X.BX != 0) {
    LocalDiskEnd  = (UINT8) (LocalDiskStart + Regs.H.BL);
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciSegment        = (UINT8) Segment;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciBus            = (UINT8) Bus;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciDevice         = (UINT8) Device;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciFunction       = (UINT8) Function;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].StartDriveNumber  = Private->DiskEnd;
    Private->DiskEnd = LocalDiskEnd;
    Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].EndDriveNumber = Private->DiskEnd;
    Private->LegacyEfiHddTableIndex += 1;
  }

    if (((EFI_LEGACY_EXPANSION_ROM_HEADER *) (RuntimeAddress))->Signature == PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      //
      // It could occur that an OpROM is not shadowed to RuntimeAddress
      // Thus Size512 will be meaningful if the signature presents
      //
      *RuntimeImageLength = ((EFI_LEGACY_EXPANSION_ROM_HEADER *) (RuntimeAddress))->Size512 * 512;

    } else {
      //
      // If the OpROM is not shadowed to RuntimeAddress, then it does not occupy the memory space
      // Hence the RuntimeImageLength should be updated in order to satisfy the reality
      //
      *RuntimeImageLength = 0;

    }
    DEBUG ((EFI_D_ERROR, " fsize = %x\n", *RuntimeImageLength));

    if (OpromRevision < 3) {
      if (*RuntimeImageLength < ImageSize) {
        //
        // Make area from end of shadowed rom to end of original rom all ffs
        //
        gBS->SetMem ((VOID *) (InitAddress + *RuntimeImageLength), ImageSize - *RuntimeImageLength, 0xff);
      }
    } else {
      gBS->FreePages (InitAddress, EFI_SIZE_TO_PAGES (ImageSize));
    }
    IsOpRomTempFree = TRUE;
  LocalDiskEnd = (UINT8) ((*(UINT8 *) ((UINTN) 0x475)) + 0x80);

  //
  // Allow platform to perform any required actions after the
  // OPROM has been initialized.
  //
  Status = Private->LegacyBiosPlatform->PlatformHooks (
                                          Private->LegacyBiosPlatform,
                                          EfiPlatformHookAfterRomInit,
                                          0,
                                          PciHandle,
                                          &RuntimeAddress,
                                          NULL,
                                          NULL
                                          );
  if (PciHandle != NULL) {
    //
    // If no PCI Handle then no header or Bevs.
    //
    if ((*RuntimeImageLength != 0) && (!mIgnoreBbsUpdateFlag) && (mVgaInstallationInProgress != TRUE)) {
      StartBbsIndex = Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries;
      TempData      = RuntimeAddress;
      UpdateBevBcvTable (
        Private,
        (EFI_LEGACY_EXPANSION_ROM_HEADER *) TempData,
        PciIo
        );
      EndBbsIndex   = Private->IntThunk->EfiToLegacy16BootTable.NumberBbsEntries;
      LocalDiskEnd  = (UINT8) (LocalDiskStart + (UINT8) (EndBbsIndex - StartBbsIndex));
      if (LocalDiskEnd != LocalDiskStart) {
        Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciSegment        = (UINT8) Segment;
        Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciBus            = (UINT8) Bus;
        Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciDevice         = (UINT8) Device;
        Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].PciFunction       = (UINT8) Function;
        Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].StartDriveNumber  = Private->DiskEnd;
        Private->DiskEnd = LocalDiskEnd;
        Private->LegacyEfiHddTable[Private->LegacyEfiHddTableIndex].EndDriveNumber = Private->DiskEnd;
        Private->LegacyEfiHddTableIndex += 1;
      }
    }
    //
    // Mark PCI device as having a legacy BIOS ROM loaded.
    //
    RomShadow (
      PciHandle,
      (UINT32) RuntimeAddress,
      (UINT32) *RuntimeImageLength,
      LocalDiskStart,
      LocalDiskEnd
      );
  }

  //
  // Stuff caller's OPTIONAL return parameters.
  //
  if (RomShadowAddress != NULL) {
    *RomShadowAddress = (VOID *) RuntimeAddress;
  }

  if (DiskStart != NULL) {
    *DiskStart = LocalDiskStart;
  }

  if (DiskEnd != NULL) {
    *DiskEnd = LocalDiskEnd;
  }

  Private->OptionRom = (UINT32) (RuntimeAddress + *RuntimeImageLength);

  Status = EFI_SUCCESS;

Done:

  if (!IsOpRomTempFree) {
    if (OpromRevision < 3) {
      if (*RuntimeImageLength < ImageSize) {
        //
        // Make area from end of shadowed rom to end of original rom all ffs
        //
        gBS->SetMem ((VOID *) (InitAddress + *RuntimeImageLength), ImageSize - *RuntimeImageLength, 0xff);
      }
    } else {
      if (PhysicalAddress != 0) {
        //
        // Free pages when OpROM is 3.0
        //
        gBS->FreePages (PhysicalAddress, EFI_SIZE_TO_PAGES (ImageSize));
      }
    }
  }



  //
  // Insure all shadowed  areas are locked
  //
  Private->LegacyRegion->Lock (
                           Private->LegacyRegion,
                           0xC0000,
                           0x40000,
                           &Granularity
                           );

  return Status;
}

/**
  Load a legacy PC-AT OPROM on the PciHandle device. Return information
  about how many disks were added by the OPROM and the shadow address and
  size. DiskStart & DiskEnd are INT 13h drive letters. Thus 0x80 is C:

  @param  This                   Protocol instance pointer.
  @param  PciHandle              The PCI PC-AT OPROM from this devices ROM BAR will
                                 be loaded. This value is NULL if RomImage is
                                 non-NULL. This is the normal case.
  @param  RomImage               A PCI PC-AT ROM image. This argument is non-NULL
                                 if there is no hardware associated with the ROM
                                 and thus no PciHandle, otherwise is must be NULL.
                                 Example is PXE base code.
  @param  Flags                  Indicates if ROM found and if PC-AT.
  @param  DiskStart              Disk number of first device hooked by the ROM. If
                                 DiskStart is the same as DiskEnd no disked were
                                 hooked.
  @param  DiskEnd                Disk number of the last device hooked by the ROM.
  @param  RomShadowAddress       Shadow address of PC-AT ROM
  @param  RomShadowedSize        Size of RomShadowAddress in bytes

  @retval EFI_SUCCESS            Legacy ROM loaded for this device
  @retval EFI_INVALID_PARAMETER  PciHandle not found
  @retval EFI_UNSUPPORTED        There is no PCI ROM in the ROM BAR or no onboard
                                 ROM

**/
EFI_STATUS
EFIAPI
LegacyBiosInstallPciRom (
  IN EFI_LEGACY_BIOS_PROTOCOL           * This,
  IN  EFI_HANDLE                        PciHandle,
  IN  VOID                              **RomImage,
  OUT UINTN                             *Flags,
  OUT UINT8                             *DiskStart, OPTIONAL
  OUT UINT8                             *DiskEnd, OPTIONAL
  OUT VOID                              **RomShadowAddress, OPTIONAL
  OUT UINT32                            *RomShadowedSize OPTIONAL
  )
{
  EFI_STATUS                      Status;
  LEGACY_BIOS_INSTANCE            *Private;
  VOID                            *LocalRomImage;
  UINTN                           ImageSize;
  UINTN                           RuntimeImageLength;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  PCI_TYPE01                      PciConfigHeader;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           PciSegment;
  UINTN                           PciBus;
  UINTN                           PciDevice;
  UINTN                           PciFunction;
  UINTN                           LastBus;
  UINTN                           Index;
  UINT8                           OpromRevision;
  UINT32                          Granularity;
  PCI_3_0_DATA_STRUCTURE          *Pcir;

  OpromRevision = 0;

  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  if (Private->Legacy16Table->LastPciBus == 0) {
    //
    // Get last bus number if not already found
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );

    LastBus = 0;
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiPciIoProtocolGuid,
                      (VOID **) &PciIo
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      Status = PciIo->GetLocation (
                        PciIo,
                        &PciSegment,
                        &PciBus,
                        &PciDevice,
                        &PciFunction
                        );
      if (PciBus > LastBus) {
        LastBus = PciBus;
      }
    }

    Private->LegacyRegion->UnLock (
                             Private->LegacyRegion,
                             0xE0000,
                             0x20000,
                             &Granularity
                             );
    Private->Legacy16Table->LastPciBus = (UINT8) LastBus;
    Private->LegacyRegion->Lock (
                             Private->LegacyRegion,
                             0xE0000,
                             0x20000,
                             &Granularity
                             );
  }

  *Flags = 0;
  if ((PciHandle != NULL) && (RomImage == NULL)) {
    //
    // If PciHandle has OpRom to Execute
    // and OpRom are all associated with Hardware
    //
    Status = gBS->HandleProtocol (
                    PciHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );

    if (!EFI_ERROR (Status)) {
      PciIo->Pci.Read (
                   PciIo,
                   EfiPciIoWidthUint32,
                   0,
                   sizeof (PciConfigHeader) / sizeof (UINT32),
                   &PciConfigHeader
                   );

      //
      // if video installed & OPROM is video return
      //
      if (
          (
           ((PciConfigHeader.Hdr.ClassCode[2] == PCI_CLASS_OLD) &&
            (PciConfigHeader.Hdr.ClassCode[1] == PCI_CLASS_OLD_VGA))
           ||
           ((PciConfigHeader.Hdr.ClassCode[2] == PCI_CLASS_DISPLAY) &&
            (PciConfigHeader.Hdr.ClassCode[1] == PCI_CLASS_DISPLAY_VGA))
          )
          &&
          (!Private->VgaInstalled)
         ) {
        mVgaInstallationInProgress = TRUE;

        //
        //      return EFI_UNSUPPORTED;
        //
      }
    }
    //
    // To run any legacy image, the VGA needs to be installed first.
    // if installing the video, then don't need the thunk as already installed.
    //
    Status = Private->LegacyBiosPlatform->GetPlatformHandle (
                                            Private->LegacyBiosPlatform,
                                            EfiGetPlatformVgaHandle,
                                            0,
                                            &HandleBuffer,
                                            &HandleCount,
                                            NULL
                                            );

    if (!EFI_ERROR (Status)) {
      mVgaHandle = HandleBuffer[0];
      if ((!Private->VgaInstalled) && (PciHandle != mVgaHandle)) {
        //
        // A return status of EFI_NOT_FOUND is considered valid (No EFI
        // driver is controlling video.
        //
        mVgaInstallationInProgress  = TRUE;
        Status                      = LegacyBiosInstallVgaRom (Private);
        if (EFI_ERROR (Status)) {
          if (Status != EFI_NOT_FOUND) {
            mVgaInstallationInProgress = FALSE;
            return Status;
          }
        } else {
          mVgaInstallationInProgress = FALSE;
        }
      }
    }
    //
    // See if the option ROM for PciHandle has already been executed
    //
    Status = IsLegacyRom (PciHandle);

    if (!EFI_ERROR (Status)) {
      mVgaInstallationInProgress = FALSE;
      GetShadowedRomParameters (
        PciHandle,
        DiskStart,
        DiskEnd,
        RomShadowAddress,
        (UINTN *) RomShadowedSize
        );
      return EFI_SUCCESS;
    }

    Status = LegacyBiosCheckPciRomEx (
               &Private->LegacyBios,
               PciHandle,
               &LocalRomImage,
               &ImageSize,
               &RuntimeImageLength,
               Flags,
               &OpromRevision,
               NULL
               );
    if (EFI_ERROR (Status)) {
      //
      // There is no PCI ROM in the ROM BAR or no onboard ROM
      //
      mVgaInstallationInProgress = FALSE;
      return EFI_UNSUPPORTED;
    }
  } else {
    if ((RomImage == NULL) || (*RomImage == NULL)) {
      //
      // If PciHandle is NULL, and no OpRom is to be associated
      //
      mVgaInstallationInProgress = FALSE;
      return EFI_UNSUPPORTED;
    }

    Status = Private->LegacyBiosPlatform->GetPlatformHandle (
                                            Private->LegacyBiosPlatform,
                                            EfiGetPlatformVgaHandle,
                                            0,
                                            &HandleBuffer,
                                            &HandleCount,
                                            NULL
                                            );

    if (!EFI_ERROR (Status)) {
      if (!Private->VgaInstalled) {
        //
        // A return status of EFI_NOT_FOUND is considered valid (No EFI
        // driver is controlling video.
        //
        mVgaInstallationInProgress  = TRUE;
        Status                      = LegacyBiosInstallVgaRom (Private);
        if (EFI_ERROR (Status)) {
          if (Status != EFI_NOT_FOUND) {
            mVgaInstallationInProgress = FALSE;
            return Status;
          }
        } else {
          mVgaInstallationInProgress = FALSE;
        }
      }
    }

    LocalRomImage = *RomImage;
    if (((PCI_EXPANSION_ROM_HEADER *) LocalRomImage)->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE ||
        ((PCI_EXPANSION_ROM_HEADER *) LocalRomImage)->PcirOffset == 0 ||
        (((PCI_EXPANSION_ROM_HEADER *) LocalRomImage)->PcirOffset & 3 ) != 0) {
      mVgaInstallationInProgress = FALSE;
      return EFI_UNSUPPORTED;
    }

    Pcir = (PCI_3_0_DATA_STRUCTURE *)
           ((UINT8 *) LocalRomImage + ((PCI_EXPANSION_ROM_HEADER *) LocalRomImage)->PcirOffset);

    if (Pcir->Signature != PCI_DATA_STRUCTURE_SIGNATURE) {
      mVgaInstallationInProgress = FALSE;
      return EFI_UNSUPPORTED;
    }

    ImageSize = Pcir->ImageLength * 512;
    OpromRevision = Pcir->Revision;
    //
    // Using PCIR structure length to recognize the actual revision if PCIR Revision value is zero
    //
    if (OpromRevision == 0) {
      if (Pcir->Length >= 0x1c) {
        OpromRevision = 3;
      } else if (Pcir->Length >= 0x18) {
        OpromRevision = 2;
      }
    }
    if (OpromRevision < 3 || Pcir->Length < 0x1c) {
      RuntimeImageLength = 0;
    } else {
      RuntimeImageLength = Pcir->MaxRuntimeImageLength * 512;
    }
  }
  //
  // Shadow and initialize the OpROM.
  //
  ASSERT (Private->TraceIndex < 0x200);
  Private->Trace[Private->TraceIndex] = LEGACY_PCI_TRACE_000;
  Private->TraceIndex ++;
  Private->TraceIndex = (UINT16) (Private->TraceIndex % 0x200);
  Status = LegacyBiosInstallRom (
             This,
             Private,
             PciHandle,
             OpromRevision,
             LocalRomImage,
             ImageSize,
             &RuntimeImageLength,
             DiskStart,
             DiskEnd,
             RomShadowAddress
             );
  if (RomShadowedSize != NULL) {
    *RomShadowedSize = (UINT32) RuntimeImageLength;
  }

  mVgaInstallationInProgress = FALSE;
  return Status;
}

