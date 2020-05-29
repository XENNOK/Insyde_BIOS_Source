//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/

/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file
  PciHostBridge.c

@brief
    Pci Host Bridge driver:
    Provides the basic interfaces to abstract a PCI Host Bridge Resource Allocation

Revision History

**/
#include "PciHostBridge.h"
#include "PciRootBridge.h"

///
/// Support 64 K IO space
/// Moving RES_IO_BASE due to new ACPI Base address 0x1800
///
#define RES_IO_BASE   0x2000
#define RES_IO_LIMIT  0xFFFF

///
/// Support 4G address space
///
#define RES_MEM_LIMIT_1 (PcdGet64 (PcdPciExpressBaseAddress) - 1)
//[-start-140521-IB05400527-add]//
#define MEM_EQU_4GB     0x100000000ULL
//[-end-140521-IB05400527-add]//

///
/// Hard code: Root Bridge Number within the host bridge
///            Root Bridge's attribute
///            Root Bridge's device path
///            Root Bridge's resource aperture
///
static UINTN                              RootBridgeNumber[1] = { 1 };
///
/// Hard code EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM  0 to support prefetchable memory allocation
///
static UINT64                             RootBridgeAttribute[1][1] = { EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM };
static EFI_PCI_ROOT_BRIDGE_DEVICE_PATH    mEfiPciRootBridgeDevicePath[1][1] = {
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0A03),
    0,
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

static PCI_ROOT_BRIDGE_RESOURCE_APERTURE  mResAperture[1][1] = { { 0, 255, 0, 0xffffffff, 0, 1 << 16 } };
static EFI_HANDLE                         mDriverImageHandle;
//[-start-140521-IB05400527-add]//
static DXE_PLATFORM_SA_POLICY_PROTOCOL    *mDxePlatformSaPolicy = NULL;
static BOOLEAN                            mEnableAbove4GBMmioBiosAssignemnt = FALSE;
//[-end-140521-IB05400527-add]//

//[-start-111222-IB02382238-add]//
EFI_STATUS
EFIAPI
GcdConvertReservedMemToSystemMem (
  IN EFI_PHYSICAL_ADDRESS                    MemPtr,
  IN UINT64                                  Size
  )
/*++

Routine Description:

  Covert GCD memory type from EfiGcdMemoryTypeReserved to EfiGcdMemoryTypeSystemMemory in the range 
  [MemPtr, MemPtr + Size).

Arguments:

  MemPtr      - Memory address
  Size        - Memory size

Returns:

  EFI_SUCCESS - Convert successfully
  EFI_ABORTED - In the memory range [MemPtr, MemPtr + Size), there is one space that is not 
                EfiGcdMemoryTypeSystemMemory or EfiGcdMemoryTypeReserved.
  otherwise   - GetMemorySpaceDescriptor() fail or RemoveMemorySpace() fail or AddMemorySpace() fail.
  
--*/
{
  EFI_PHYSICAL_ADDRESS                    TargetMemPtr;
  EFI_STATUS                              Status;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR         GcdDescriptor;
  UINT64                                  RemainSize;
  UINT64                                  UpdateSize;
  

  if (Size == 0) {
    return EFI_SUCCESS;
  }

  RemainSize = Size;
  TargetMemPtr = MemPtr + Size;
  do {
    Status = gDS->GetMemorySpaceDescriptor (MemPtr, &GcdDescriptor);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Only EfiGcdMemoryTypeReserved can be prompted to EfiGcdMemoryTypeSystemMemory.
    // If it is EfiGcdMemoryTypeSystemMemory, keep going. Otherwiese, return error.
    //
    if (GcdDescriptor.GcdMemoryType != EfiGcdMemoryTypeReserved &&
        GcdDescriptor.GcdMemoryType != EfiGcdMemoryTypeSystemMemory) {
      return EFI_ABORTED;
    }

    if (TargetMemPtr > (GcdDescriptor.BaseAddress + GcdDescriptor.Length)) {
      UpdateSize = (GcdDescriptor.BaseAddress + GcdDescriptor.Length) - MemPtr;
    } else {
      UpdateSize = RemainSize;
    }

    if (GcdDescriptor.GcdMemoryType == EfiGcdMemoryTypeReserved) {
      Status = gDS->RemoveMemorySpace (
                      MemPtr,
                      UpdateSize
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
      
      Status = gDS->AddMemorySpace (
                      EfiGcdMemoryTypeSystemMemory,
                      MemPtr,
                      UpdateSize,
                      GcdDescriptor.Capabilities &~(EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED | EFI_MEMORY_RUNTIME)
                      );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    if (TargetMemPtr <= (GcdDescriptor.BaseAddress + GcdDescriptor.Length)) {
      break;
    }
    
    MemPtr = GcdDescriptor.BaseAddress + GcdDescriptor.Length;
    RemainSize = TargetMemPtr - MemPtr;
  } while (TRUE);
  
  return EFI_SUCCESS;
}
//[-end-111222-IB02382238-add]//
//EFI_DRIVER_ENTRY_POINT (PciHostBridgeEntryPoint)
///
/// Implementation
///
/**

  Entry point of this driver

  @param[in] ImageHandle -
  @param[in] SystemTable -

  @retval EFI_SUCCESS       - Driver Start OK
  @retval EFI_DEVICE_ERROR  - Fail to install PCI_ROOT_BRIDGE_IO protocol.

**/
EFI_STATUS
EFIAPI
PciHostBridgeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                Status;
  UINTN                     Loop1;
  UINTN                     Loop2;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridge;
  PCI_ROOT_BRIDGE_INSTANCE  *PrivateData;
  EFI_PHYSICAL_ADDRESS      PciBaseAddress;
  EFI_PHYSICAL_ADDRESS      RemapBase;
  EFI_PHYSICAL_ADDRESS      RemapLimit;
  EFI_PHYSICAL_ADDRESS      MeSegMask;
  EFI_PHYSICAL_ADDRESS      MeStolenSize;
  BOOLEAN                   MeStolenEnable;
  UINT32                    Tolud;
  UINT64                    Length;
  EFI_PHYSICAL_ADDRESS      BaseAddress;
  CPU_FAMILY                CpuFamilyId;
  CPU_STEPPING              CpuSteppingId;
//[-start-140521-IB05400527-add]//
  EFI_PHYSICAL_ADDRESS      Touud;
//[-end-140521-IB05400527-add]//

  //INITIALIZE_SCRIPT (ImageHandle, SystemTable);

  mDriverImageHandle = ImageHandle;

  ///
  /// This system has one Host Bridge (one Root Bridge in this Host Bridge)
  ///
  ///
  /// Create Host Bridge Device Handle
  ///
  for (Loop1 = 0; Loop1 < HOST_BRIDGE_NUMBER; Loop1++) {
    Status = (gBS->AllocatePool) (EfiBootServicesData, sizeof (PCI_HOST_BRIDGE_INSTANCE), (VOID **) &HostBridge);
    ASSERT (!EFI_ERROR (Status));

    HostBridge->Signature         = PCI_HOST_BRIDGE_SIGNATURE;
    HostBridge->RootBridgeNumber  = RootBridgeNumber[Loop1];
    HostBridge->ResourceSubmited  = FALSE;
    HostBridge->CanRestarted      = TRUE;

    ///
    /// InitializeListHead (&HostBridge->Head);
    ///
    HostBridge->ResAlloc.NotifyPhase          = NotifyPhase;
    HostBridge->ResAlloc.GetNextRootBridge    = GetNextRootBridge;
    HostBridge->ResAlloc.GetAllocAttributes   = GetAttributes;
    HostBridge->ResAlloc.StartBusEnumeration  = StartBusEnumeration;
    HostBridge->ResAlloc.SetBusNumbers        = SetBusNumbers;
    HostBridge->ResAlloc.SubmitResources      = SubmitResources;
    HostBridge->ResAlloc.GetProposedResources = GetProposedResources;
    HostBridge->ResAlloc.PreprocessController = PreprocessController;
    HostBridge->HostBridgeHandle              = NULL;
    Status = gBS->InstallProtocolInterface (
                    &HostBridge->HostBridgeHandle,
                    &gEfiPciHostBridgeResourceAllocationProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &HostBridge->ResAlloc
                    );
    if (EFI_ERROR (Status)) {
      (gBS->FreePool) (HostBridge);
      return EFI_DEVICE_ERROR;
    }
    ///
    /// Create Root Bridge Device Handle in this Host Bridge
    ///
    InitializeListHead (&HostBridge->Head);
    for (Loop2 = 0; Loop2 < HostBridge->RootBridgeNumber; Loop2++) {
      Status = (gBS->AllocatePool) (EfiBootServicesData, sizeof (PCI_ROOT_BRIDGE_INSTANCE), (VOID **) &PrivateData);
      ASSERT (!EFI_ERROR (Status));

      PrivateData->Signature  = PCI_ROOT_BRIDGE_SIGNATURE;
      PrivateData->DevicePath = (EFI_DEVICE_PATH_PROTOCOL *) &mEfiPciRootBridgeDevicePath[Loop1][Loop2];
      RootBridgeConstructor (
        &PrivateData->Io,
        HostBridge->HostBridgeHandle,
        RootBridgeAttribute[Loop1][Loop2],
        &mResAperture[Loop1][Loop2]
        );
      PrivateData->Handle = NULL;
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &PrivateData->Handle,
                      &gEfiDevicePathProtocolGuid,
                      PrivateData->DevicePath,
                      &gEfiPciRootBridgeIoProtocolGuid,
                      &PrivateData->Io,
                      NULL
                      );
      if (EFI_ERROR (Status)) {
        (gBS->FreePool) (PrivateData);
        return EFI_DEVICE_ERROR;
      }

      InsertTailList (&HostBridge->Head, &PrivateData->Link);
    }
  }
  ///
  /// Allocate 60 KB of I/O space [0x1000..0xFFFF]
  ///
  Status = gDS->AddIoSpace (
                  EfiGcdIoTypeIo,
                  RES_IO_BASE,
                  RES_IO_LIMIT - RES_IO_BASE + 1
                  );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Allocate PCI memory space.
  ///
  ///
  /// Read memory map registers
  ///
  RemapBase       = McD0PciCfg64 (R_SA_REMAPBASE) & B_SA_REMAPBASE_REMAPBASE_MASK;
  RemapLimit      = McD0PciCfg64 (R_SA_REMAPLIMIT) & B_SA_REMAPLIMIT_REMAPLMT_MASK;
  Tolud           = McD0PciCfg32 (R_SA_TOLUD) & B_SA_TOLUD_TOLUD_MASK;
  PciBaseAddress  = Tolud;
  MeSegMask       = McD0PciCfg64 (R_SA_MESEG_MASK);
  MeStolenEnable  = (BOOLEAN) ((MeSegMask & B_SA_MESEG_MASK_ME_STLEN_EN_MASK) != 0);

  ///
  /// First check if memory remap is used
  ///
  if ((RemapBase > RemapLimit) && (MeStolenEnable)) {
    MeStolenSize = MeSegMask & B_SA_MESEG_MASK_MEMASK_MASK;
    if (MeStolenSize != 0) {
      MeStolenSize = 0x8000000000L - MeStolenSize;
    }
    ///
    /// Remap is disabled -> PCI starts at TOLUD + ME Stolen size
    ///
    PciBaseAddress += MeStolenSize;
  }

  Length = RES_MEM_LIMIT_1 - PciBaseAddress + 1;

  if (Length != 0) {
    DEBUG (
      (
      EFI_D_INFO, " Allocating PCI space from 0x%X to 0x%X\n", (UINT32) PciBaseAddress, (UINT32)
      (PciBaseAddress + Length - 1)
      )
      );
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeMemoryMappedIo,
                    PciBaseAddress,
                    Length,
                    0
                    );
    ASSERT_EFI_ERROR (Status);
  }

//[-start-140521-IB05400527-add]//
  //
  // Always provide available above 4GB MMIO resource
  // BIOS will only use it when policy EnableAbove4GBMmio enabled
  //
  Touud = McD0PciCfg64 (R_SA_TOUUD) & B_SA_TOUUD_TOUUD_MASK;
  if (Touud > MEM_EQU_4GB) {
    PciBaseAddress = Touud;
  } else {
    PciBaseAddress = MEM_EQU_4GB;
  }
  Length = PcdGet64 (PcdMmioMem64Length);
  DEBUG ((EFI_D_ERROR, " Allocating PCI space from 0x%lX to 0x%lX\n", (UINT64)PciBaseAddress, (UINT64)(PciBaseAddress + Length - 1)));
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  PciBaseAddress,
                  Length,
                  0
                  );
  ASSERT_EFI_ERROR (Status);
//[-end-140521-IB05400527-add]//
  
  ///
  /// Get CPU Family and Stepping ID
  ///
  CpuFamilyId = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  ///
  /// WA for HSW A0. To exclude the ranges 20000000-201FFFFF (2MB) and 0x40004000-0x40004FFF (4KB).
  ///
  if ((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId == EnumHswA0)) {
    if (McD0PciCfg32 (R_SA_DEVEN) & B_SA_DEVEN_D2EN_MASK) {
      BaseAddress = 0x20000000;
      Status = (gBS->AllocatePages) (AllocateAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (0x00200000), &BaseAddress);
//[-start-120426-IB06460394-modify]//
//[-start-111222-IB02382238-add]//
      if (EFI_ERROR (Status)) {
        GcdConvertReservedMemToSystemMem (BaseAddress, 0x00200000);
        Status = (gBS->AllocatePages) (AllocateAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (0x00200000), &BaseAddress);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Status of 20000000-201FFFFF: %r\n", Status));
          ASSERT_EFI_ERROR (Status);          
        }
      }
//[-end-111222-IB02382238-add]//      
//[-end-120426-IB06460394-modify]//
      
      BaseAddress = 0x40004000;
      Status = (gBS->AllocatePages) (AllocateAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (0x00001000), &BaseAddress);
//[-start-120426-IB06460394-modify]//
//[-start-111222-IB02382238-add]//
      if (EFI_ERROR (Status)) {
        GcdConvertReservedMemToSystemMem (BaseAddress, 0x00001000);
        Status = (gBS->AllocatePages) (AllocateAddress, EfiReservedMemoryType, EFI_SIZE_TO_PAGES (0x00001000), &BaseAddress);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Status of 0x40004000-0x40004FFF: %r\n", Status));
          ASSERT_EFI_ERROR (Status);
        }
      }
//[-end-111222-IB02382238-add]//      
//[-end-120426-IB06460394-modify]//
    }
  }

  return EFI_SUCCESS;
}

//[-start-130709-IB05400426-add]//
/**
 Check PCI MMIO allocated status. If MMIO is not enough or too much, adjust
 when dynamic TOLUD function is enable.

 @param[in]         Status              Status after AllocateMemorySpace.
 @param[in]         BaseAddress         Allocated memory space base address.
 @param[in]         AddrLen             Desired memory space size.

*/
VOID
CheckResource (
  IN  EFI_STATUS                Status,
  IN  EFI_PHYSICAL_ADDRESS      BaseAddress,
  IN  UINT64                    AddrLen
  )
{
  UINT32                    BestTolud;
  UINT32                    CurrentTolud;
  UINT16                    AdjustTolud;
  EFI_PHYSICAL_ADDRESS      TempBaseAddress;



  //
  // If dynamic TOLUD function is enabled, check resource allocating status.
  // If status is OK then check if it is best TOLUD or not.
  // If status is not OK, adjust to lowest TOLUD and reset system to allocate again.
  //
  if (PcdGetBool (PcdDynamicTolud)) {
    if (!EFI_ERROR (Status)) {
      //
      // Allocate status is OK, check if this TOLUD is best or not
      //  
      CurrentTolud = McD0PciCfg32 (R_SA_TOLUD);
      CurrentTolud &= (~0xFFFFF);
      DEBUG ((EFI_D_ERROR, "Current TOLUD  = %x\n", CurrentTolud));
      
      AdjustTolud = PcdGet16 (PcdAdjustTolud);
      DEBUG ((EFI_D_ERROR, "Adjusted TOLUD = %x\n", AdjustTolud));
      
      TempBaseAddress = BaseAddress; 
      if ((CurrentTolud == (UINT32)(TempBaseAddress & (~0x0FFFFF))) || (AdjustTolud == (UINT16)(TempBaseAddress >> 20))) {
        //
        // Best TOLUD, Do Nothing!!
        //
        return;
      } else {
        //
        // Not Best TOLUD, assign new TOLUD and reboot
        //
        BestTolud = (UINT32)(TempBaseAddress & (~0x0FFFFF));
        DEBUG ((EFI_D_ERROR, "Best TOLUD     = %x\n", BestTolud));
        DEBUG((EFI_D_ERROR, "Not best TOLUD, assign new TOLUD and reboot!!\n"));
      }

    } else {
      //
      // Allocate status is not OK, Set BestTolud as MAX MMIO size (1024M)
      //
      BestTolud = 0x40000000;
      DEBUG ((EFI_D_ERROR, "Out of PCI memory! AllocateMemorySpace() returned %r\n", Status));
      DEBUG ((EFI_D_ERROR, "Size requested: 0x%lX bytes\n", AddrLen));
      DEBUG((EFI_D_ERROR, "Assign new TOLUD to %x and reboot!!\n", BestTolud));
      
    }

    gRT->SetVariable (
           L"BestTolud",
           &gEfiGenericVariableGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof (UINT32),
           &BestTolud
           );
    
    //
    // Reset System and ME, Set PMIR.CF9GR (Bit20) and Write 0xE to 0xCF9 port.
    //
    PchLpcPciCfg32Or (R_PCH_LPC_PMIR, B_PCH_LPC_PMIR_CF9GR);
    IoWrite8 (0xCF9, 0x0E);

    //
    // Wait for Reboot
    //
    while (TRUE);

  } else if (EFI_ERROR (Status)) {
    EFI_DEADLOOP ();
  }

}
//[-end-130709-IB05400426-add]//

/**

  Enter a certain phase of the PCI enumeration process

  @param[in] This  - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL instance
  @param[in] Phase - The phase during enumeration

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong phase parameter passed in.
  @retval EFI_NOT_READY          -  Resources have not been submitted yet.

**/
EFI_STATUS
EFIAPI
NotifyPhase (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PHASE    Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  PCI_RESOURCE_TYPE         Index;
  LIST_ENTRY                *List;
  EFI_PHYSICAL_ADDRESS      BaseAddress;
  UINT64                    AddrLen;
  UINTN                     BitsOfAlignment;
  UINT64                    Alignment;
  EFI_STATUS                Status;
  EFI_STATUS                ReturnStatus;

  HostBridgeInstance = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  switch (Phase) {
  case EfiPciHostBridgeBeginEnumeration:
    if (HostBridgeInstance->CanRestarted) {
      ///
      /// Reset the Each Root Bridge
      ///
      List = HostBridgeInstance->Head.ForwardLink;
      while (List != &HostBridgeInstance->Head) {
        RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
        for (Index = TypeIo; Index < TypeMax; Index++) {
          RootBridgeInstance->ResAllocNode[Index].Type    = Index;
          RootBridgeInstance->ResAllocNode[Index].Base    = 0;
          RootBridgeInstance->ResAllocNode[Index].Length  = 0;
          RootBridgeInstance->ResAllocNode[Index].Status  = ResNone;
        }

        List = List->ForwardLink;
      }

      HostBridgeInstance->ResourceSubmited  = FALSE;
      HostBridgeInstance->CanRestarted      = TRUE;

//[-start-140521-IB05400527-add]//
      //
      // Retrieve DxePlatformSaPolicy before host bridge enumeration
      // See if above 4GB MMIO BIOS assignment enabled
      //
      if (mDxePlatformSaPolicy == NULL) {
        Status = gBS->LocateProtocol (&gDxePlatformSaPolicyGuid, NULL, (VOID**)&mDxePlatformSaPolicy);
        if ((!EFI_ERROR (Status)) && (mDxePlatformSaPolicy != NULL) && (mDxePlatformSaPolicy->MiscConfig->EnableAbove4GBMmio)) {
          mEnableAbove4GBMmioBiosAssignemnt = TRUE;
        }
      }
//[-end-140521-IB05400527-add]//
    } else {
      ///
      /// Can not restart
      ///
      return EFI_NOT_READY;
    }
    break;

  case EfiPciHostBridgeBeginBusAllocation:
    ///
    /// No specific action is required here, can perform any chipset specific programing
    ///
    HostBridgeInstance->CanRestarted = FALSE;
    return EFI_SUCCESS;
    break;

  case EfiPciHostBridgeEndBusAllocation:
    ///
    /// No specific action is required here, can perform any chipset specific programing
    ///
    return EFI_SUCCESS;
    break;

  case EfiPciHostBridgeBeginResourceAllocation:
    ///
    /// No specific action is required here, can perform any chipset specific programing
    ///
    return EFI_SUCCESS;
    break;

  case EfiPciHostBridgeAllocateResources:
    ReturnStatus = EFI_SUCCESS;
    if (HostBridgeInstance->ResourceSubmited) {
      ///
      /// Take care of the resource dependencies between the root bridges
      ///
      List = HostBridgeInstance->Head.ForwardLink;
      while (List != &HostBridgeInstance->Head) {
        RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
        for (Index = TypeIo; Index < TypeBus; Index++) {
          if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
            AddrLen   = RootBridgeInstance->ResAllocNode[Index].Length;
            Alignment = RootBridgeInstance->ResAllocNode[Index].Alignment;

            ///
            /// Get the number of '1' in Alignment.
            ///
            for (BitsOfAlignment = 0; Alignment != 0; BitsOfAlignment++) {
              Alignment = RShiftU64 (Alignment, 1);
            }

            switch (Index) {
            case TypeIo:
              ///
              /// It is impossible for this chipset to align 0xFFFF for IO16
              /// So clear it
              ///
              if (BitsOfAlignment >= 16) {
                BitsOfAlignment = 0;
              }

              Status = gDS->AllocateIoSpace (
                              EfiGcdAllocateAnySearchBottomUp,
                              EfiGcdIoTypeIo,
                              BitsOfAlignment,
                              AddrLen,
                              &BaseAddress,
                              mDriverImageHandle,
                              NULL
                              );
              if (!EFI_ERROR (Status)) {
                RootBridgeInstance->ResAllocNode[Index].Base    = (UINTN) BaseAddress;
                RootBridgeInstance->ResAllocNode[Index].Status  = ResAllocated;
              } else {
                ///
                /// Not able to allocate enough I/O memory - critical stop
                ///
                DEBUG ((EFI_D_ERROR, "Out of I/O space! AllocateIoSpace() returned %r\n", Status));
                DEBUG ((EFI_D_ERROR, "Size requested: 0x%lX bytes\n", AddrLen));
                ReturnStatus = Status;
              }
              break;

            case TypeMem32:
            case TypePMem32:
              ///
              /// It is impossible for this chipset to align 0xFFFFFFFF for Mem32
              /// So clear it
              ///
              if (BitsOfAlignment >= 32) {
                BitsOfAlignment = 0;
              }
//[-start-140521-IB05400527-add]//
              Status = gDS->AllocateMemorySpace (
                                EfiGcdAllocateAnySearchBottomUp,
                                EfiGcdMemoryTypeMemoryMappedIo,
                                BitsOfAlignment,
                                AddrLen,
                                &BaseAddress,
                                mDriverImageHandle,
                                NULL
                                );
              CheckResource (Status, BaseAddress, AddrLen);
              if (!EFI_ERROR (Status)) {
                ///
                /// We were able to allocate the PCI memory
                ///
                RootBridgeInstance->ResAllocNode[Index].Base    = (UINTN) BaseAddress;
                RootBridgeInstance->ResAllocNode[Index].Status  = ResAllocated;
              } else {
                ///
                /// Not able to allocate enough PCI memory - critical stop
                ///
                DEBUG ((EFI_D_ERROR, "Out of PCI memory! AllocateMemorySpace() returned %r\n", Status));
                DEBUG ((EFI_D_ERROR, "Size requested: 0x%lX bytes\n", AddrLen));
                ReturnStatus = Status;
              }
              break;
//[-end-140521-IB05400527-add]//

            ///
            /// Fall through to TypeMem64 / TypePMem64...
            ///
            case TypeMem64:
            case TypePMem64:
//[-start-140521-IB05400527-modify]//
              //
              // It is impossible for this chipset to align 0xFFFFFFFFFFFFFFFF for Mem64
              // So clear it
              //
              if (BitsOfAlignment >= 64) {
                BitsOfAlignment = 0;
              }
              //
              // Enable BIOS above 4GB MMIO assignment only when policy enabled.
              //
              if (mEnableAbove4GBMmioBiosAssignemnt) {
                DEBUG ((EFI_D_ERROR, "64Bit MMIO BIOS assignment enabled\n"));
                BaseAddress = MAX_ADDRESS;
                Status = gDS->AllocateMemorySpace (
                                EfiGcdAllocateAnySearchTopDown,
                                EfiGcdMemoryTypeMemoryMappedIo,
                                BitsOfAlignment,
                                AddrLen,
                                &BaseAddress,
                                mDriverImageHandle,
                                NULL
                                );
              } else {
                Status = gDS->AllocateMemorySpace (
                                EfiGcdAllocateAnySearchBottomUp,
                                EfiGcdMemoryTypeMemoryMappedIo,
                                BitsOfAlignment,
                                AddrLen,
                                &BaseAddress,
                                mDriverImageHandle,
                                NULL
                                );
//[-start-130709-IB05400426-add]//
                CheckResource (Status, BaseAddress, AddrLen);
//[-end-130709-IB05400426-add]//
              }
//[-end-140521-IB05400527-modify]//
              if (!EFI_ERROR (Status)) {
                ///
                /// We were able to allocate the PCI memory
                ///
                RootBridgeInstance->ResAllocNode[Index].Base    = (UINTN) BaseAddress;
                RootBridgeInstance->ResAllocNode[Index].Status  = ResAllocated;
              } else {
                ///
                /// Not able to allocate enough PCI memory - critical stop
                ///
                DEBUG ((EFI_D_ERROR, "Out of PCI memory! AllocateMemorySpace() returned %r\n", Status));
                DEBUG ((EFI_D_ERROR, "Size requested: 0x%lX bytes\n", AddrLen));
                ReturnStatus = Status;
              }
              break;
            default:
              break;
            }
            ///
            /// end switch
            ///
          }
        }

        List = List->ForwardLink;
      }

      return ReturnStatus;
    } else {
      return EFI_NOT_READY;
    }
    break;

  case EfiPciHostBridgeSetResources:
    break;

  case EfiPciHostBridgeFreeResources:
    ReturnStatus  = EFI_SUCCESS;

    List          = HostBridgeInstance->Head.ForwardLink;
    while (List != &HostBridgeInstance->Head) {
      RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
      for (Index = TypeIo; Index < TypeBus; Index++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status == ResAllocated) {
          AddrLen     = RootBridgeInstance->ResAllocNode[Index].Length;
          BaseAddress = RootBridgeInstance->ResAllocNode[Index].Base;
          switch (Index) {
          case TypeIo:
            Status = gDS->FreeIoSpace (BaseAddress, AddrLen);
            break;

          case TypeMem32:
          case TypePMem32:
          case TypeMem64:
          case TypePMem64:
            Status = gDS->FreeMemorySpace (BaseAddress, AddrLen);
            break;

          default:
            Status = EFI_INVALID_PARAMETER;

          }

          if (EFI_ERROR (Status)) {
            ReturnStatus = Status;
          }
          ///
          /// end switch
          ///
          RootBridgeInstance->ResAllocNode[Index].Type    = Index;
          RootBridgeInstance->ResAllocNode[Index].Base    = 0;
          RootBridgeInstance->ResAllocNode[Index].Length  = 0;
          RootBridgeInstance->ResAllocNode[Index].Status  = ResNone;
        }
      }

      List = List->ForwardLink;
    }

    HostBridgeInstance->ResourceSubmited  = FALSE;
    HostBridgeInstance->CanRestarted      = TRUE;
    return ReturnStatus;
    break;

  case EfiPciHostBridgeEndResourceAllocation:
    HostBridgeInstance->CanRestarted = FALSE;
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// end switch
  ///
  return EFI_SUCCESS;
}

/**

  Return the device handle of the next PCI root bridge that is associated with
  this Host Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - Returns the device handle of the next PCI Root Bridge.
                      On input, it holds the RootBridgeHandle returned by the most
                      recent call to GetNextRootBridge().The handle for the first
                      PCI Root Bridge is returned if RootBridgeHandle is NULL on input

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_NOT_FOUND          -  Next PCI root bridge not found.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameter passed in.

**/
EFI_STATUS
EFIAPI
GetNextRootBridge (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN OUT EFI_HANDLE                                   *RootBridgeHandle
  )
{
  BOOLEAN                   NoRootBridge;
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;

  NoRootBridge        = TRUE;
  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    NoRootBridge        = FALSE;
    RootBridgeInstance  = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (*RootBridgeHandle == NULL) {
      ///
      /// Return the first Root Bridge Handle of the Host Bridge
      ///
      *RootBridgeHandle = RootBridgeInstance->Handle;
      return EFI_SUCCESS;
    } else {
      if (*RootBridgeHandle == RootBridgeInstance->Handle) {
        ///
        /// Get next if have
        ///
        List = List->ForwardLink;
        if (List != &HostBridgeInstance->Head) {
          RootBridgeInstance  = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
          *RootBridgeHandle   = RootBridgeInstance->Handle;
          return EFI_SUCCESS;
        } else {
          return EFI_NOT_FOUND;
        }
      }
    }

    List = List->ForwardLink;
  }
  ///
  /// end while
  ///
  if (NoRootBridge) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}

/**

  Returns the attributes of a PCI Root Bridge.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle  - The device handle of the PCI Root Bridge
                      that the caller is interested in
  @param[in] Attributes        - The pointer to attributes of the PCI Root Bridge

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Attributes parameter passed in is NULL or
                            RootBridgeHandle is not an EFI_HANDLE
                            that was returned on a previous call to
                            GetNextRootBridge().

**/
EFI_STATUS
EFIAPI
GetAttributes (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT UINT64                                          *Attributes
  )
{
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;

  if (Attributes == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
//[-start-140521-IB05400527-add]//
      if (mEnableAbove4GBMmioBiosAssignemnt) {
        RootBridgeInstance->RootBridgeAttrib |= EFI_PCI_HOST_BRIDGE_MEM64_DECODE;
      }
//[-end-140521-IB05400527-add]//
      *Attributes = RootBridgeInstance->RootBridgeAttrib;
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }
  ///
  /// RootBridgeHandle is not an EFI_HANDLE
  /// that was returned on a previous call to GetNextRootBridge()
  ///
  return EFI_INVALID_PARAMETER;
}

/**

  This is the request from the PCI enumerator to set up
  the specified PCI Root Bridge for bus enumeration process.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance.
  @param[in] RootBridgeHandle  - The PCI Root Bridge to be set up.
  @param[in] Configuration     - Pointer to the pointer to the PCI bus resource descriptor.

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_OUT_OF_RESOURCES   -  Not enough pool to be allocated.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
EFIAPI
StartBusEnumeration (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  VOID                      *Buffer;
  UINT8                     *Temp;
  EFI_STATUS                Status;
  UINT64                    BusStart;
  UINT64                    BusEnd;

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      ///
      /// Set up the Root Bridge for Bus Enumeration
      ///
      BusStart  = RootBridgeInstance->BusBase;
      BusEnd    = RootBridgeInstance->BusLimit;

      ///
      /// Program the Hardware(if needed) if error return EFI_DEVICE_ERROR
      ///
      Status = (gBS->AllocatePool) (
          EfiBootServicesData, sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR), &Buffer
        );
      if (EFI_ERROR (Status)) {
        return EFI_OUT_OF_RESOURCES;
      }

      Temp  = (UINT8 *) Buffer;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->Desc                  = ACPI_ADDRESS_SPACE_DESCRIPTOR;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->Len                   = 0x2B;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->ResType               = ACPI_ADDRESS_SPACE_TYPE_BUS;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->GenFlag               = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->SpecificFlag          = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrSpaceGranularity  = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrRangeMin          = BusStart;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrRangeMax          = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrTranslationOffset = 0;
      ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp)->AddrLen               = BusEnd - BusStart + 1;

      Temp = Temp + sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);

      ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Desc = ACPI_END_TAG_DESCRIPTOR;
      ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Checksum = 0x0;
      *Configuration = Buffer;
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  This function programs the PCI Root Bridge hardware so that
  it decodes the specified PCI bus range

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge whose bus range is to be programmed
  @param[in] Configuration     - The pointer to the PCI bus resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameters passed in.

**/
EFI_STATUS
EFIAPI
SetBusNumbers (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                *List;
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  UINT8                     *Ptr;
  UINTN                     BusStart;
  UINTN                     BusEnd;
  UINTN                     BusLen;

  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr = Configuration;

  ///
  /// Check the Configuration is valid
  ///
  if (*Ptr != ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  if (((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Ptr)->ResType != ACPI_ADDRESS_SPACE_TYPE_BUS) {
    return EFI_INVALID_PARAMETER;
  }

  Ptr += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  if (*Ptr != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;
  Ptr                 = Configuration;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      BusStart  = (UINTN) ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Ptr)->AddrRangeMin;
      BusLen    = (UINTN) ((EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Ptr)->AddrLen;
      BusEnd    = BusStart + BusLen - 1;
      if (BusStart > BusEnd) {
        return EFI_INVALID_PARAMETER;
      }

      if ((BusStart < RootBridgeInstance->BusBase) || (BusEnd > RootBridgeInstance->BusLimit)) {
        return EFI_INVALID_PARAMETER;
      }
      ///
      /// Update the Bus Range
      ///
      RootBridgeInstance->ResAllocNode[TypeBus].Base    = BusStart;
      RootBridgeInstance->ResAllocNode[TypeBus].Length  = BusLen;
      RootBridgeInstance->ResAllocNode[TypeBus].Status  = ResAllocated;

      ///
      /// Program the Root Bridge Hardware
      ///
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  Submits the I/O and memory resource requirements for the specified PCI Root Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge whose I/O and memory resource requirements
                      are being submitted
  @param[in] Configuration     - The pointer to the PCI I/O and PCI memory resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  Wrong parameters passed in.

**/
EFI_STATUS
EFIAPI
SubmitResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  IN VOID                                             *Configuration
  )
{
  LIST_ENTRY                        *List;
  PCI_HOST_BRIDGE_INSTANCE          *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE          *RootBridgeInstance;
  UINT8                             *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *ptr;
  UINT64                            AddrLen;
  UINT64                            Alignment;

  ///
  /// Check the input parameter: Configuration
  ///
  if (Configuration == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;
  Temp                = (UINT8 *) Configuration;
  while (*Temp == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
    Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
  }

  if (*Temp != ACPI_END_TAG_DESCRIPTOR) {
    return EFI_INVALID_PARAMETER;
  }

  Temp = (UINT8 *) Configuration;
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      while (*Temp == ACPI_ADDRESS_SPACE_DESCRIPTOR) {
        ptr = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp;

        ///
        /// Check address range alignment
        ///
//[-start-140623-IB05400535-modify]//
        if (ptr->AddrRangeMax >= (UINT64) (-1) ||
            ptr->AddrRangeMax != (Power2MaxMemory (ptr->AddrRangeMax + 1) - 1)
            ) {
          return EFI_INVALID_PARAMETER;
        }
//[-end-140623-IB05400535-modify]//

        switch (ptr->ResType) {
        case ACPI_ADDRESS_SPACE_TYPE_MEM:
          ///
          /// check the memory resource request is supported by PCI root bridge
          ///
          /// Hard code EFI_PCI_HOST_BRIDGE_COMBINE_MEM_PMEM  0 to support prefetchable memory allocation
          ///
          if (RootBridgeInstance->RootBridgeAttrib == 0 && ptr->SpecificFlag == 0x06) {
            return EFI_INVALID_PARAMETER;
          }

          AddrLen   = (UINT64) ptr->AddrLen;
          Alignment = (UINT64) ptr->AddrRangeMax;
          if (ptr->AddrSpaceGranularity == 32) {
            if ((ptr->SpecificFlag & 0x06) == 0x06) {
              ///
              /// Apply from GCD
              ///
              RootBridgeInstance->ResAllocNode[TypePMem32].Length     = AddrLen;
              RootBridgeInstance->ResAllocNode[TypePMem32].Alignment  = Alignment;
              RootBridgeInstance->ResAllocNode[TypePMem32].Status     = ResRequested;
              HostBridgeInstance->ResourceSubmited                    = TRUE;

            } else {
              RootBridgeInstance->ResAllocNode[TypeMem32].Length    = AddrLen;
              RootBridgeInstance->ResAllocNode[TypeMem32].Alignment = Alignment;
              RootBridgeInstance->ResAllocNode[TypeMem32].Status    = ResRequested;
              HostBridgeInstance->ResourceSubmited                  = TRUE;
            }
          }

          if (ptr->AddrSpaceGranularity == 64) {
            if ((ptr->SpecificFlag & 0x06) == 0x06) {
              RootBridgeInstance->ResAllocNode[TypePMem64].Length     = AddrLen;
              RootBridgeInstance->ResAllocNode[TypePMem64].Alignment  = Alignment;
              RootBridgeInstance->ResAllocNode[TypePMem64].Status     = ResSubmitted;
              HostBridgeInstance->ResourceSubmited                    = TRUE;

            } else {
              RootBridgeInstance->ResAllocNode[TypeMem64].Length    = AddrLen;
              RootBridgeInstance->ResAllocNode[TypeMem64].Alignment = Alignment;
              RootBridgeInstance->ResAllocNode[TypeMem64].Status    = ResSubmitted;
              HostBridgeInstance->ResourceSubmited                  = TRUE;
            }
          }
          break;

        case ACPI_ADDRESS_SPACE_TYPE_IO:
          AddrLen   = (UINT64) ptr->AddrLen;
          Alignment = (UINT64) ptr->AddrRangeMax;
          RootBridgeInstance->ResAllocNode[TypeIo].Length     = AddrLen;
          RootBridgeInstance->ResAllocNode[TypeIo].Alignment  = Alignment;
          RootBridgeInstance->ResAllocNode[TypeIo].Status     = ResRequested;
          HostBridgeInstance->ResourceSubmited                = TRUE;
          break;

        default:
          break;
        }

        Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
      }

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  This function returns the proposed resource settings for the specified
  PCI Root Bridge

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge handle
  @param[in] Configuration     - The pointer to the pointer to the PCI I/O
                      and memory resource descriptor

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_OUT_OF_RESOURCES   -  Not enough pool to be allocated.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
EFIAPI
GetProposedResources (
  IN EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL *This,
  IN EFI_HANDLE                                       RootBridgeHandle,
  OUT VOID                                            **Configuration
  )
{
  LIST_ENTRY                        *List;
  PCI_HOST_BRIDGE_INSTANCE          *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE          *RootBridgeInstance;
  UINTN                             Index;
  UINTN                             Number;
  VOID                              *Buffer;
  UINT8                             *Temp;
  EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *ptr;
  EFI_STATUS                        Status;
  UINT64                            ResStatus;

  Buffer  = NULL;
  Number  = 0;

  ///
  /// Get the Host Bridge Instance from the resource allocation protocol
  ///
  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  ///
  /// Enumerate the root bridges in this host bridge
  ///
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      for (Index = 0; Index < TypeBus; Index++) {
        if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
          Number++;
        }
      }

      if (Number > 0) {
        Status = (gBS->AllocatePool)
          (
            EfiBootServicesData, Number * sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) +
            sizeof (EFI_ACPI_END_TAG_DESCRIPTOR), &Buffer
          );
        if (EFI_ERROR (Status)) {
          return EFI_OUT_OF_RESOURCES;
        }

        ZeroMem (Buffer, sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR) * Number + sizeof (EFI_ACPI_END_TAG_DESCRIPTOR));

        Temp = Buffer;
        for (Index = 0; Index < TypeBus; Index++) {
          if (RootBridgeInstance->ResAllocNode[Index].Status != ResNone) {
            ptr       = (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR *) Temp;
            ResStatus = RootBridgeInstance->ResAllocNode[Index].Status;
            switch (Index) {
            case TypeIo:
              ///
              /// Io
              ///
              ptr->Desc                   = 0x8A;
              ptr->Len                    = 0x2B;
              ptr->ResType                = 1;
              ptr->GenFlag                = 0;
              ptr->SpecificFlag           = 0;
              ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
              ptr->AddrRangeMax           = 0;
              ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
              ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
              break;

            case TypeMem32:
              ///
              /// Memory 32
              ///
              ptr->Desc                   = 0x8A;
              ptr->Len                    = 0x2B;
              ptr->ResType                = 0;
              ptr->GenFlag                = 0;
              ptr->SpecificFlag           = 0;
              ptr->AddrSpaceGranularity   = 32;
              ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
              ptr->AddrRangeMax           = 0;
              ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
              ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
              break;

            case TypePMem32:
              ///
              /// Prefetch memory 32
              ///
              ptr->Desc                   = 0x8A;
              ptr->Len                    = 0x2B;
              ptr->ResType                = 0;
              ptr->GenFlag                = 0;
              ptr->SpecificFlag           = 6;
              ptr->AddrSpaceGranularity   = 32;
              ptr->AddrRangeMin           = 0;
              ptr->AddrRangeMax           = 0;
              ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
              ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
              break;

            case TypeMem64:
              ///
              /// Memory 64
              ///
              ptr->Desc                   = 0x8A;
              ptr->Len                    = 0x2B;
              ptr->ResType                = 0;
              ptr->GenFlag                = 0;
              ptr->SpecificFlag           = 0;
              ptr->AddrSpaceGranularity   = 64;
//[-start-111230-IB06460349-modify]//
              ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
//[-end-111230-IB06460349-modify]//
              ptr->AddrRangeMax           = 0;
              ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
              ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
              break;

            case TypePMem64:
              ///
              /// Prefetch memory 64
              ///
              ptr->Desc                   = 0x8A;
              ptr->Len                    = 0x2B;
              ptr->ResType                = 0;
              ptr->GenFlag                = 0;
              ptr->SpecificFlag           = 6;
              ptr->AddrSpaceGranularity   = 64;
//[-start-140623-IB05400535-modify]//
              ptr->AddrRangeMin           = RootBridgeInstance->ResAllocNode[Index].Base;
//[-end-140623-IB05400535-modify]//
              ptr->AddrRangeMax           = 0;
              ptr->AddrTranslationOffset  = (ResStatus == ResAllocated) ? EFI_RESOURCE_SATISFIED : EFI_RESOURCE_LESS;
              ptr->AddrLen                = RootBridgeInstance->ResAllocNode[Index].Length;
              break;
            }

            Temp += sizeof (EFI_ACPI_ADDRESS_SPACE_DESCRIPTOR);
          }
        }

        ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Desc      = 0x79;
        ((EFI_ACPI_END_TAG_DESCRIPTOR *) Temp)->Checksum  = 0x0;
        *Configuration = Buffer;
      }

      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  This function is called for all the PCI controllers that the PCI
  bus driver finds. Can be used to Preprogram the controller.

  @param[in] This              - The EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_ PROTOCOL instance
  @param[in] RootBridgeHandle  - The PCI Root Bridge handle
  @param[in] PciAddress        - Address of the controller on the PCI bus
  @param[in] Phase             - The Phase during resource allocation

  @retval EFI_SUCCESS            -  Succeed.
  @retval EFI_INVALID_PARAMETER  -  RootBridgeHandle is not a valid handle.

**/
EFI_STATUS
EFIAPI
PreprocessController (
  IN  EFI_PCI_HOST_BRIDGE_RESOURCE_ALLOCATION_PROTOCOL          *This,
  IN  EFI_HANDLE                                                RootBridgeHandle,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_PCI_ADDRESS               PciAddress,
  IN  EFI_PCI_CONTROLLER_RESOURCE_ALLOCATION_PHASE              Phase
  )
{
  PCI_HOST_BRIDGE_INSTANCE  *HostBridgeInstance;
  PCI_ROOT_BRIDGE_INSTANCE  *RootBridgeInstance;
  LIST_ENTRY                *List;

  HostBridgeInstance  = INSTANCE_FROM_RESOURCE_ALLOCATION_THIS (This);
  List                = HostBridgeInstance->Head.ForwardLink;

  ///
  /// Enumerate the root bridges in this host bridge
  ///
  while (List != &HostBridgeInstance->Head) {
    RootBridgeInstance = DRIVER_INSTANCE_FROM_LIST_ENTRY (List);
    if (RootBridgeHandle == RootBridgeInstance->Handle) {
      return EFI_SUCCESS;
    }

    List = List->ForwardLink;
  }

  return EFI_INVALID_PARAMETER;
}

/**

  Calculate max memory of power 2

  @param[in] MemoryLength  -  Input memory length.

  @retval Returned Maximum length.

**/
UINT64
Power2MaxMemory (
  IN UINT64                     MemoryLength
  )
{
  UINT64  Result;

  if (RShiftU64 (MemoryLength, 32)) {
    Result = LShiftU64 ((UINT64) GetPowerOfTwo64 (RShiftU64 (MemoryLength, 32)), 32);
  } else {
    Result = (UINT64) GetPowerOfTwo64 (MemoryLength);
  }

  return Result;
}
