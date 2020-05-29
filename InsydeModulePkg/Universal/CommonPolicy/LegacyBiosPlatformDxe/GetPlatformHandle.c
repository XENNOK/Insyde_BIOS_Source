/** @file
  The EFI Legacy BIOS Patform Protocol member function

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "LegacyBiosPlatformDxe.h"

#include <Library/DevicePathLib.h>
#include <PortNumberMap.h>
#include <Protocol/DiskInfo.h>

#define ISA_BRIDGE_VENDOR_ID    0xFFFF    // don't care
#define PCI_SUBCLASS_DONT_CARE  0xFF

//
// Symbol definition, for PCI IDE configuration field
//

#define AHCI_ENABLE                       0x8000
#define AHCI_PORT0                        0x0100
#define AHCI_PORT2                        0x0200
#define AHCI_PORT4                        0x0400
#define AHCI_PORT6                        0x0800

#define ACTIVE_VGA_VAR_NAME    L"ActiveVgaDev"
#define PCI_MAX_SEGMENT        0

#pragma pack(1)
typedef struct {
  EFI_HANDLE  Handle;
  UINT16      Vid;
  UINT16      Did;
  UINT16      SvId;
  UINT16      SysId;
} DEVICE_STRUCTURE;
#pragma pack()

EFI_HANDLE                          mVideoHandles[0x20];
EFI_HANDLE                          mDiskHandles[0x20];
EFI_HANDLE                          mIsaHandles[0x20];

static
EFI_STATUS
FindAllDeviceTypes (
  IN UINT8                  BaseCodeValue,
  IN UINT8                  SubClassCodeValue,
  IN OUT DEVICE_STRUCTURE   *DeviceTable,
  IN OUT UINT16             *DeviceIndex,
  IN BOOLEAN                DeviceFlags
  );

static
VOID
SortIdeHandles (
  DEVICE_STRUCTURE  *DeviceArray,
  UINT32            RegionalIndex
  );

static
VOID
FillHddInfo (
  DEVICE_STRUCTURE  *DeviceArray,
  UINT32            ArraySize,
  HDD_INFO          *HddInfo
  );

/**
  Returns a buffer of handles for the requested subfunction.

  @param  This                  The protocol instance pointer.
  @param  Mode                  Specifies what handle to return. See EFI_GET_PLATFORM_HANDLE_MODE enum.
  @param  Type                  Mode specific. See EFI_GET_PLATFORM_HANDLE_MODE enum.
  @param  HandleBuffer          Mode specific. See EFI_GET_PLATFORM_HANDLE_MODE enum.
  @param  HandleCount           Mode specific. See EFI_GET_PLATFORM_HANDLE_MODE enum.
  @param  AdditionalData        Mode specific. See EFI_GET_PLATFORM_HANDLE_MODE enum.

  @retval EFI_SUCCESS           Handle is valid.
  @retval EFI_UNSUPPORTED       Mode is not supported on the platform.
  @retval EFI_NOT_FOUND         Handle is not known.

**/
EFI_STATUS
EFIAPI
GetPlatformHandle (
  IN  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL           *This,
  IN  EFI_GET_PLATFORM_HANDLE_MODE                Mode,
  IN  UINT16                                      Type,
  OUT EFI_HANDLE                                  **HandleBuffer,
  OUT UINTN                                       *HandleCount,
  OUT VOID                                        **AdditionalData OPTIONAL
  )
{
  DEVICE_STRUCTURE              RegionalDevice[0x40];
  UINT32                        RegionalIndex = 0;
  UINT32                        Index;
  DEVICE_STRUCTURE              BufferDevice;

  HDD_INFO                      *HddInfo = NULL;
  static BOOLEAN                Link  = FALSE;
  UINT8                         IdeSubClassCodeValue[] =
                                { PCI_CLASS_MASS_STORAGE_IDE,
                                  PCI_CLASS_MASS_STORAGE_SATADPA,
                                  PCI_CLASS_MASS_STORAGE_RAID
                                };
  DEVICE_STRUCTURE              TempDevice;
  VOID                          *VarConout;
  UINTN                         DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  BOOLEAN                       Found;
  EFI_STATUS                    Status;
  EFI_DEVICE_PATH_PROTOCOL      *PrimaryVgaDevicePath;

  PrimaryVgaDevicePath = NULL;
  VarConout      = NULL;
  DevicePathSize = 0;
  Found          = FALSE;

  switch (Mode) {

  case EfiGetPlatformVgaHandle:
    //
    // Find VGA-compatible devices
    //
    FindAllDeviceTypes (
                  (UINT8) PCI_CLASS_OLD,
                  (UINT8) PCI_CLASS_OLD_VGA,
                  (DEVICE_STRUCTURE *) (&RegionalDevice[0]),
                  (UINT16 *) (&RegionalIndex),
                  0
                  );
    //
    // Find VGA-compatible controllers or 8514-compatible controllers
    //
    FindAllDeviceTypes (
                  (UINT8) PCI_CLASS_DISPLAY,
                  (UINT8) PCI_SUBCLASS_DONT_CARE,
                  (DEVICE_STRUCTURE *) (&RegionalDevice[RegionalIndex]),
                  (UINT16 *) (&RegionalIndex),
                  0
                  );
    if (RegionalIndex == 0) {
      return EFI_NOT_FOUND;
    }

    PrimaryVgaDevicePath = CommonGetVariableData (ACTIVE_VGA_VAR_NAME, &gEfiGenericVariableGuid);
    if (PrimaryVgaDevicePath == NULL) {
      //
      // For backward compability, supporting the ConOut case.
      // #Note: This should be removed in the future.
      //
      VarConout = CommonGetVariableData (L"ConOut", &gEfiGlobalVariableGuid);
      PrimaryVgaDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)VarConout;
    }

    for ( Index = 0; (Index < RegionalIndex) && !Found; Index ++) {
      Status = gBS->HandleProtocol (
                                RegionalDevice[Index].Handle,
                                &gEfiDevicePathProtocolGuid,
                                (VOID **)&DevicePath
                                );

      if (CompareMem (
            PrimaryVgaDevicePath,
            DevicePath,
            GetDevicePathSize (DevicePath) - END_DEVICE_PATH_LENGTH
            ) == 0) {

        TempDevice = RegionalDevice[0];
        RegionalDevice[0] = RegionalDevice[Index];
        RegionalDevice[Index] = TempDevice;

        Found = TRUE;
      }
    }

    for (Index = 0; Index < RegionalIndex; Index++) {
      mVideoHandles[Index] = RegionalDevice[Index].Handle;
    }
    *HandleBuffer = &mVideoHandles[0];
    *HandleCount = RegionalIndex;
    return  EFI_SUCCESS;

  case EfiGetPlatformIdeHandle:
    //
    // Case 1: In GenericLegacyBoot(), the 1st handle returned will be choose to InitLegacyIdeController(),
    //             which will clear IO BAR if controller is in legacy mode
    // Case 2: In LegacyBiosBuildIdeData(), the 1st handle returned will get re-connect,
    //             when called in GenericLegacyBoot()
    // Case 3:  In LegacyBiosBuildIdeData(), the HddInfo[] is collected, which is used by LegacyBiosBuildBbs()
    //              Note: The HddInfo[] is only 8 entries from EfiToLegacy16BootTable.HddInfo[]
    //              An entry is a primary or secondary channel. note: one channel could have master/slave devices
    //  Case 4: In EnableIdeController(), the 1st handle returned will be choose to enable command byte to 0x1F,
    //              which is called by EnableAllControllers(). It is called in GenericLegacyBoot()
    // Case 5: In PciProgramAllInterruptLineRegisters(), the handle returned will be checked to
    //             InstallLegacyIrqHandler() if the controller is in legacy mode
    //
    for (Index = 0; Index < (sizeof (IdeSubClassCodeValue) / sizeof (UINT8)); Index++) {
      FindAllDeviceTypes (
                  (UINT8) PCI_CLASS_MASS_STORAGE,
                  (UINT8) IdeSubClassCodeValue[Index],
                  (DEVICE_STRUCTURE *) (&RegionalDevice[RegionalIndex]),
                  (UINT16 *) (&RegionalIndex),
                  1
                  );
    }
    if (RegionalIndex == 0) {
      return EFI_NOT_FOUND;
    }
    //
    // Make sure all IDE controllers are connected. This is absolutely necessary
    // in NO_CONFIG_CHANGE boot path to ensure IDE controller is correctly
    // initialized and all IDE drives are enumerated
    //
    if (!Link) {
      for ( Index = 0; Index < RegionalIndex; Index ++) {
        gBS->ConnectController (
                            RegionalDevice[Index].Handle,
                            NULL,
                            NULL,
                            FALSE
                            );
      }
    }
    //
    // We have connected all IDE controllers once. No more needed
    //
    Link = TRUE;

    SortIdeHandles(&RegionalDevice[0], RegionalIndex);

    *HandleBuffer = &mDiskHandles[0];
    *HandleCount  = RegionalIndex;

    if (AdditionalData != NULL) {
      HddInfo = (HDD_INFO *) *AdditionalData;
    } else {
      return EFI_SUCCESS;
    }

    FillHddInfo (&RegionalDevice[0], RegionalIndex, HddInfo);
    break;

  case EfiGetPlatformIsaBusHandle:
    //
    // Locate all found block io devices
    //
    FindAllDeviceTypes (
      (UINT8) PCI_CLASS_BRIDGE,
      (UINT8) PCI_CLASS_BRIDGE_ISA_PDECODE,
      (DEVICE_STRUCTURE *) (&RegionalDevice[0]),
      (UINT16 *) (&RegionalIndex),
      1
      );

    FindAllDeviceTypes (
      (UINT8) PCI_CLASS_BRIDGE,
      (UINT8) PCI_CLASS_BRIDGE_ISA,
      (DEVICE_STRUCTURE *) (&RegionalDevice[RegionalIndex]),
      (UINT16 *) (&RegionalIndex),
      1
      );
    if (RegionalIndex == 0) {
      return EFI_NOT_FOUND;
    }

    for (Index = 0; Index < RegionalIndex; Index++) {
      if (RegionalDevice[Index].Vid == ISA_BRIDGE_VENDOR_ID) {
        BufferDevice          = RegionalDevice[0];
        RegionalDevice[0]      = RegionalDevice[Index];
        RegionalDevice[Index]  = BufferDevice;
      }
    }

    for (Index = 0; Index < RegionalIndex; Index++) {
      mIsaHandles[Index] = RegionalDevice[Index].Handle;
    }

    *HandleBuffer = &mIsaHandles[0];
    *HandleCount  = RegionalIndex;
    return EFI_SUCCESS;

  case EfiGetPlatformUsbHandle:   // Nothing implemented in Kernel layer
  default:
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Return the handles and assorted information for the specified PCI Class code

  @param  BaseCodeValue         Base part of class code
  @param  SubclassCodeValue     SubClass part of class code
  @param  DeviceTable           Table to place handles etc in.
  @param  DeviceIndex           Number of devices found
  @param  DeviceFlags           Indicate the device require a valid legacy ROM or not

  @retval EFI_SUCCESS           One or more devices found
  @retval EFI_NOT_FOUND         No device found

**/
static
EFI_STATUS
FindAllDeviceTypes (
  IN UINT8                  BaseCodeValue,
  IN UINT8                  SubClassCodeValue,
  IN OUT DEVICE_STRUCTURE   *DeviceTable,
  IN OUT UINT16             *DeviceIndex,
  IN BOOLEAN                DeviceFlags
  )
{
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     Index;
  UINTN                     StartIndex;
  PCI_TYPE00                PciConfigHeader00;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_LEGACY_BIOS_PROTOCOL  *LegacyBios;
  UINTN                     Flags;
  EFI_STATUS                Status;

  StartIndex = *DeviceIndex;
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  ASSERT_EFI_ERROR (Status);

  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiPciIoProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );
  for (Index = 0; Index < HandleCount; Index++) {
    gBS->HandleProtocol (
          HandleBuffer[Index],
          &gEfiPciIoProtocolGuid,
          (VOID **)&PciIo
          );
    PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint32,
                0,
                sizeof (PciConfigHeader00) / sizeof (UINT32),
                &PciConfigHeader00
                );
    if ((PciConfigHeader00.Hdr.ClassCode[2] == BaseCodeValue) &&
      ((PciConfigHeader00.Hdr.ClassCode[1] == SubClassCodeValue) || (SubClassCodeValue == PCI_SUBCLASS_DONT_CARE))) {
      DeviceTable->Handle = HandleBuffer[Index];
      LegacyBios->CheckPciRom (
                    LegacyBios,
                    HandleBuffer[Index],
                    NULL,
                    NULL,
                    &Flags
                    );

      //
      // Verify that results of OPROM check match request.
      // The two valid requests are:
      //   DeviceFlags = 0 require a valid legacy ROM
      //   DeviceFlags = 1 require either no ROM or a valid legacy ROM
      //
      if (((DeviceFlags != 0) && (Flags != ROM_FOUND)) ||
          ((DeviceFlags == 0) && (Flags == (ROM_FOUND | VALID_LEGACY_ROM)))
          ) {
        DeviceTable->Vid    = PciConfigHeader00.Hdr.VendorId;
        DeviceTable->Did    = PciConfigHeader00.Hdr.DeviceId;
        DeviceTable->SvId   = PciConfigHeader00.Device.SubsystemVendorID;
        DeviceTable->SysId  = PciConfigHeader00.Device.SubsystemID;
        ++ *DeviceIndex;
        DeviceTable++;
      }
    }
  }
  //
  // Free any allocated buffers
  //
  gBS->FreePool (HandleBuffer);

  if (*DeviceIndex != StartIndex) {
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

/**
  Sort all IDE controller informations in DeviceArray[].
  It will put entries that matches PortNumberMap array in same order ahead and
  sort entries that bus/device/function are same and segment are different from
  small segment to large. Then sort remaining entries with corresponding PCI
  location from small to large.

  @param  DeviceArray           Information array about all found mass storage storage controllers.
  @param  ArraySize             Entry count of DeviceArray.

**/
static
VOID
SortIdeHandles (
  DEVICE_STRUCTURE  *DeviceArray,
  UINT32            ArraySize
  )
{
  EFI_STATUS                    Status;
  UINTN                         Seg, Bus, Dev, Fun;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT32                        Index, Index1;
  DEVICE_STRUCTURE              BufferDevice;
  UINT32                        StartSortIndex;
  PORT_NUMBER_MAP               *PortMappingTable;
  PORT_NUMBER_MAP               EndEntry;
  UINTN                         NoPorts;
  UINT32                        PciLocationArray[0x20];
  UINT32                        CurrentPciLocation;
  UINT32                        PciLocation;
  INT32                         FindIndex;

  PortMappingTable = NULL;

  ZeroMem (&EndEntry, sizeof (PORT_NUMBER_MAP));

  //
  // Get PCI location of each device and encode it to UINT32.
  // When we do sorting, it will be more efficient to compare integer and
  // we need not to call GetLocation() function there.
  //
  for (Index = 0; Index < ArraySize; Index++) {
      Status = gBS->HandleProtocol (
                      DeviceArray[Index].Handle,
                      &gEfiPciIoProtocolGuid,
                      (VOID **)&PciIo
                      );
      ASSERT_EFI_ERROR(Status);
      PciIo->GetLocation (PciIo, &Seg, &Bus, &Dev, &Fun);
      PciLocationArray[Index] = (UINT32)((Seg << 24) |
                                         (Bus << 16) |
                                         (Dev << 8 ) |
                                         (Fun      ));
  }

  //
  // Find devices and sort existed items as the order as PortNumMap table.
  //
  PortMappingTable = (PORT_NUMBER_MAP *)PcdGetPtr (PcdPortNumberMapTable);

  NoPorts = 0;
  while (CompareMem (&EndEntry, &PortMappingTable[NoPorts], sizeof (PORT_NUMBER_MAP)) != 0) {
    NoPorts++;
  }

  ASSERT(NoPorts > 0);

  CurrentPciLocation = 0xffffff;
  StartSortIndex = 0;
  for (Index = 0; Index < NoPorts; Index++) {

    PciLocation = (((UINT32)PortMappingTable[Index].Bus    << 16) |
                   ((UINT32)PortMappingTable[Index].Device << 8 ) |
                   ((UINT32)PortMappingTable[Index].Function    ));

    if (CurrentPciLocation != PciLocation) {

      CurrentPciLocation = PciLocation;

      //
      // Try to find all correspond devices in DeviceArray for each segments and sort them
      //
      do {

        FindIndex = -1;
        Seg = PCI_MAX_SEGMENT + 1;
        for (Index1 = StartSortIndex ; Index1 < ArraySize; Index1++) {

          if ((PciLocationArray[Index1] & 0xffffff) != CurrentPciLocation) {
            continue;
          }

          if ((FindIndex < 0) || ((PciLocationArray[Index1] >> 24) <= Seg)) {
            FindIndex = Index1;
            Seg = PciLocationArray[Index1] >> 24;
          }
        }

        if (FindIndex >= 0) {
          if ((UINT32)FindIndex != StartSortIndex) {
            BufferDevice = DeviceArray[StartSortIndex];
            DeviceArray[StartSortIndex] = DeviceArray[FindIndex];
            DeviceArray[FindIndex] = BufferDevice;

            PciLocation = PciLocationArray[StartSortIndex];
            PciLocationArray[StartSortIndex] = PciLocationArray[FindIndex];
            PciLocationArray[FindIndex] = PciLocation;
          }

          StartSortIndex ++;
        }
      } while (FindIndex >= 0);
    }
  }

  //
  // The smaller bus/device/function number is put ahead
  //
  for (Index = StartSortIndex; Index < ArraySize; Index++) {
    for (Index1 = Index + 1; Index1 < ArraySize; Index1++) {

      if (PciLocationArray[Index1] > PciLocationArray[Index]) {
        continue;
      }
      //
      // (Seg1, Bus1, Dev1, Fun1) < (Seg, Bus, Dev, Fun)
      //
      BufferDevice = DeviceArray[Index];
      DeviceArray[Index] = DeviceArray[Index1];
      DeviceArray[Index1] = BufferDevice;

      PciLocation = PciLocationArray[Index];
      PciLocationArray[Index] = PciLocationArray[Index1];
      PciLocationArray[Index1] = PciLocation;
    }
  }

  for (Index = 0; Index < ArraySize; Index++) {
    mDiskHandles[Index] = DeviceArray[Index].Handle;
  }
}

/**
  Fill the HddInfo structure which is passed by CSM code.
  Note every controller will have two channels in a HddInfo structure

  @param  DeviceArray           All ide controller handles sorted.
  @param  ArraySize             Number of handles in DeviceArray[].
  @param  HddInfo               EfiToLegacy16BootTable.HddInfo with 8 entries

**/
static
VOID
FillHddInfo (
  DEVICE_STRUCTURE  *DeviceArray,
  UINT32            ArraySize,
  HDD_INFO          *HddInfo
  )
{
  EFI_STATUS                    Status;
  UINT32                        Index;
  UINT32                        Index2;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  PCI_TYPE00                    PciConfigHeader00;
  UINTN                         Seg;
  UINTN                         Bus;
  UINTN                         Dev;
  UINTN                         Fun;
  UINT32                        HddInfoIndex;
  UINT32                        AdditionalHddInfoEntry;
  UINT32                        IdeChannel;
  UINT32                        IdeDevice;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  EFI_DISK_INFO_PROTOCOL        *DiskInfo;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePathNode;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePathNode2;
  PCI_DEVICE_PATH               *PciDevicePath;

  HddInfoIndex = 0;
  if (ArraySize > (MAX_IDE_CONTROLLER / 2)) {
    ArraySize = (MAX_IDE_CONTROLLER / 2);
    DEBUG ((EFI_D_ERROR, "HddInfo table length insufficient\n"));
  }

  for (Index = 0; Index < ArraySize; Index++) {
    Status = gBS->HandleProtocol (
                    DeviceArray[Index].Handle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);
    PciIo->GetLocation (
            PciIo,
            &Seg,
            &Bus,
            &Dev,
            &Fun
            );
    //
    // Examine if it is AHCI/RAID mode and needs larger array to store HDD info.
    //
    AdditionalHddInfoEntry = 0;
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
    if (Status == EFI_SUCCESS) {
      for (Index2 = 0; Index2 < HandleCount; Index2++) {
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index2],
                        &gEfiDiskInfoProtocolGuid,
                        (VOID **) &DiskInfo
                        );
        if ((Status !=  EFI_SUCCESS) ||
            (!CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid))) {
          //
          // If error occurs or it is not installed by AHCI/RAID controller,
          // additional process is not needed.
          //
          continue;
        }
        Status = gBS->HandleProtocol (
                        HandleBuffer[Index2],
                        &gEfiDevicePathProtocolGuid,
                        (VOID *) &DevicePath
                        );
        if (Status !=  EFI_SUCCESS) {
          continue;
        }
        PciDevicePath = NULL;
        DevicePathNode = DevicePath;
        while (!IsDevicePathEnd (DevicePathNode)) {
          DevicePathNode2 = NextDevicePathNode (DevicePathNode);
          if ((DevicePathType (DevicePathNode) == HARDWARE_DEVICE_PATH) &&
               (DevicePathSubType (DevicePathNode) == HW_PCI_DP) &&
               (DevicePathType(DevicePathNode2) == MESSAGING_DEVICE_PATH) &&
               (DevicePathSubType(DevicePathNode2) == MSG_SATA_DP)) {
            PciDevicePath = (PCI_DEVICE_PATH *) DevicePathNode;
            break;
          }
          DevicePathNode = NextDevicePathNode (DevicePathNode);
        }
        if ((PciDevicePath == NULL) ||
            !((PciDevicePath->Device == Dev) && (PciDevicePath->Function == Fun))) {
          continue;
        }
        IdeChannel = 0;
        IdeDevice = 0;
        Status = DiskInfo->WhichIde (DiskInfo, &IdeChannel, &IdeDevice);
        IdeChannel >>= 1;
        if ((Status ==  EFI_SUCCESS) &&
            (IdeChannel >= 2) &&
            (IdeChannel < 4) &&
            (IdeChannel > AdditionalHddInfoEntry)) {
          //
          // CSM can handle at most 16 SATA devices.
          //
          AdditionalHddInfoEntry = IdeChannel;
        }
      }
    }
    PciIo->Pci.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 0,
                 sizeof (PciConfigHeader00) / sizeof (UINT32),
                 &PciConfigHeader00
                 );
    for (Index2 = 0; Index2 <= AdditionalHddInfoEntry; Index2 += 2) {
      //
      // Primary controller data
      //
      HddInfo[HddInfoIndex].Status |= HDD_PRIMARY;
      if ((PciConfigHeader00.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SATADPA) ||
          (PciConfigHeader00.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
        HddInfo[HddInfoIndex].Status |= AHCI_ENABLE | (AHCI_PORT0 << HddInfoIndex);
      }
      HddInfo[HddInfoIndex].Bus           = (UINT32) Bus;
      HddInfo[HddInfoIndex].Device        = (UINT32) Dev;
      HddInfo[HddInfoIndex].Function      = (UINT32) Fun;
      HddInfo[HddInfoIndex].BusMasterAddress = (UINT16) (PciConfigHeader00.Device.Bar[4] & 0xfffc);
      if (((PciConfigHeader00.Hdr.ClassCode[0] & 0x01) != 0) ||
          ((PciConfigHeader00.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SATADPA) ||
           (PciConfigHeader00.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID))) {
        HddInfo[HddInfoIndex].CommandBaseAddress = (UINT16) (PciConfigHeader00.Device.Bar[0] & 0xfffc);
        HddInfo[HddInfoIndex].ControlBaseAddress = (UINT16) ((PciConfigHeader00.Device.Bar[1] & 0xfffc) + 2);
        HddInfo[HddInfoIndex].HddIrq               = PciConfigHeader00.Device.InterruptLine;
      } else {
        HddInfo[HddInfoIndex].HddIrq                = 14;
        HddInfo[HddInfoIndex].CommandBaseAddress    = 0x1f0;
        HddInfo[HddInfoIndex].ControlBaseAddress    = 0x3f6;
      }

      //
      // Secondary controller data
      //
      HddInfo[HddInfoIndex + 1].Status |= HDD_SECONDARY;
      if ((PciConfigHeader00.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_SATADPA) ||
          (PciConfigHeader00.Hdr.ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID)) {
        HddInfo[HddInfoIndex + 1].Status |= AHCI_ENABLE | (AHCI_PORT2<< HddInfoIndex);
      }
      HddInfo[HddInfoIndex + 1].Bus       = (UINT32) Bus;
      HddInfo[HddInfoIndex + 1].Device    = (UINT32) Dev;
      HddInfo[HddInfoIndex + 1].Function  = (UINT32) Fun;
      HddInfo[HddInfoIndex + 1].BusMasterAddress = (UINT16) (HddInfo[HddInfoIndex].BusMasterAddress + 8);
      if (((PciConfigHeader00.Hdr.ClassCode[0] & 0x04) != 0) ||
          ((PciConfigHeader00.Hdr.ClassCode[1]==0x06) ||
           (PciConfigHeader00.Hdr.ClassCode[1]==0x04))){
        HddInfo[HddInfoIndex + 1].CommandBaseAddress  = (UINT16) (PciConfigHeader00.Device.Bar[2] & 0xfffc);
        HddInfo[HddInfoIndex + 1].ControlBaseAddress  = (UINT16) ((PciConfigHeader00.Device.Bar[3] & 0xfffc) + 2);
        HddInfo[HddInfoIndex + 1].HddIrq              = PciConfigHeader00.Device.InterruptLine;
      } else {
        HddInfo[HddInfoIndex + 1].HddIrq              = 15;
        HddInfo[HddInfoIndex + 1].CommandBaseAddress  = 0x170;
        HddInfo[HddInfoIndex + 1].ControlBaseAddress  = 0x376;
      }
      HddInfoIndex += 2;
    }
  }
}

