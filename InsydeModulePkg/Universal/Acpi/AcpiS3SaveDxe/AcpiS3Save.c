/** @file
  ACPI S3 Save Protocol driver

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/** 
  This is an implementation of the ACPI S3 Save protocol.  This is defined in
  S3 boot path specification 0.9.

Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>

This program and the accompanying materials
are licensed and made available under the terms and conditions
of the BSD License which accompanies this distribution.  The
full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/LockBoxLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeChipsetSvcLib.h>
#include <Guid/AcpiVariableSet.h>
#include <Guid/AcpiS3Context.h>
#include <Guid/Acpi.h>
#include <Protocol/AcpiS3Save.h>
#include <Protocol/FrameworkMpService.h>
#include <IndustryStandard/Acpi.h>

#include "AcpiS3Save.h"

/**
  Update AcpiVariableSet variable for ECP mode

  @param AcpiS3Context   ACPI s3 context
**/
VOID
UpdateAcpiVariableSet (
  IN ACPI_S3_CONTEXT      *AcpiS3Context
  );

UINTN     mLegacyRegionSize;

EFI_ACPI_S3_SAVE_PROTOCOL mS3Save = {
  LegacyGetS3MemorySize,
  S3Ready,
};

EFI_GUID              mAcpiS3IdtrProfileGuid = {
  0xdea652b0, 0xd587, 0x4c54, 0xb5, 0xb4, 0xc6, 0x82, 0xe7, 0xa0, 0xaa, 0x3d
};

/**
  Allocate EfiACPIMemoryNVS below 4G memory address.

  This function allocates EfiACPIMemoryNVS below 4G memory address.

  @param  Size         Size of memory to allocate.
  
  @return Allocated address for output.

**/
VOID*
AllocateAcpiNvsMemoryBelow4G (
  IN   UINTN   Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;

  Pages = EFI_SIZE_TO_PAGES (Size);
  Address = 0xffffffff;

  Status  = gBS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  ASSERT_EFI_ERROR (Status);

  Buffer = (VOID *) (UINTN) Address;
  ZeroMem (Buffer, Size);

  return Buffer;
}

/**
  To find Facs in Acpi tables.
 
  To find Firmware ACPI control strutcure in Acpi Tables since the S3 waking vector is stored 
  in the table.

  @param AcpiTableGuid   The guid used to find ACPI table in UEFI ConfigurationTable.
  
  @return  Facs table pointer.
**/
EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *
FindAcpiFacsTableByAcpiGuid (
  IN EFI_GUID  *AcpiTableGuid
  )
{
  EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Rsdt;
  EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE     *Fadt;
  EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *Facs;
  UINTN                                         Index;
  UINT32                                        Data32;
  Rsdp  = NULL;
  Rsdt  = NULL;
  Fadt  = NULL;
  //
  // found ACPI table RSD_PTR from system table
  //
  for (Index = 0; Index < gST->NumberOfTableEntries; Index++) {
    if (CompareGuid (&(gST->ConfigurationTable[Index].VendorGuid), AcpiTableGuid)) {
      //
      // A match was found.
      //
      Rsdp = gST->ConfigurationTable[Index].VendorTable;
      break;
    }
  }

  if (Rsdp == NULL) {
    return NULL;
  }

  Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN) Rsdp->RsdtAddress;
  if (Rsdt == NULL || Rsdt->Signature != EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
    return NULL;
  }

  for (Index = sizeof (EFI_ACPI_DESCRIPTION_HEADER); Index < Rsdt->Length; Index = Index + sizeof (UINT32)) {

    Data32  = *(UINT32 *) ((UINT8 *) Rsdt + Index);
    Fadt    = (EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE *) (UINT32 *) (UINTN) Data32;
    if (Fadt->Header.Signature == EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
      break;
    }
  }

  if (Fadt == NULL || Fadt->Header.Signature != EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
    return NULL;
  }

  Facs = (EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *)(UINTN)Fadt->FirmwareCtrl;

  return Facs;
}

/**
  To find Facs in Acpi tables.
 
  To find Firmware ACPI control strutcure in Acpi Tables since the S3 waking vector is stored 
  in the table.
  
  @return  Facs table pointer.
**/
EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE  *
FindAcpiFacsTable (
  VOID
  )
{
  EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE *Facs;

  Facs = FindAcpiFacsTableByAcpiGuid (&gEfiAcpi20TableGuid);
  if (Facs != NULL) {
    return Facs;
  }

  return FindAcpiFacsTableByAcpiGuid (&gEfiAcpi10TableGuid);
}

/**
  Allocates and fills in the Page Directory and Page Table Entries to
  establish a 1:1 Virtual to Physical mapping.
  If BootScriptExector driver will run in 64-bit mode, this function will establish the 1:1 
  virtual to physical mapping page table.
  If BootScriptExector driver will not run in 64-bit mode, this function will do nothing. 
  
  @return  the 1:1 Virtual to Physical identity mapping page table base address. 

**/
EFI_PHYSICAL_ADDRESS
S3CreateIdentityMappingPageTables (
  VOID
  )
{  
  if (FeaturePcdGet (PcdDxeIplSwitchToLongMode)) {
    UINT32                                        RegEax;
    UINT32                                        RegEdx;
    UINT8                                         PhysicalAddressBits;
    EFI_PHYSICAL_ADDRESS                          PageAddress;
    UINTN                                         IndexOfPml4Entries;
    UINTN                                         IndexOfPdpEntries;
    UINTN                                         IndexOfPageDirectoryEntries;
    UINT32                                        NumberOfPml4EntriesNeeded;
    UINT32                                        NumberOfPdpEntriesNeeded;
    PAGE_MAP_AND_DIRECTORY_POINTER                *PageMapLevel4Entry;
    PAGE_MAP_AND_DIRECTORY_POINTER                *PageMap;
    PAGE_MAP_AND_DIRECTORY_POINTER                *PageDirectoryPointerEntry;
    PAGE_TABLE_ENTRY                              *PageDirectoryEntry;
    EFI_PHYSICAL_ADDRESS                          S3NvsPageTableAddress;
    UINTN                                         TotalPageTableSize;
    VOID                                          *Hob;
    BOOLEAN                                       Page1GSupport;
    PAGE_TABLE_1G_ENTRY                           *PageDirectory1GEntry;

    Page1GSupport = FALSE;
    if (PcdGetBool(PcdUse1GPageTable)) {
      AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
      if (RegEax >= 0x80000001) {
        AsmCpuid (0x80000001, NULL, NULL, NULL, &RegEdx);
        if ((RegEdx & BIT26) != 0) {
          Page1GSupport = TRUE;
        }
      }
    }

    //
    // Get physical address bits supported.
    //
    Hob = GetFirstHob (EFI_HOB_TYPE_CPU);
    if (Hob != NULL) {
      PhysicalAddressBits = ((EFI_HOB_CPU *) Hob)->SizeOfMemorySpace;
    } else {
      AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
      if (RegEax >= 0x80000008) {
        AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL);
        PhysicalAddressBits = (UINT8) RegEax;
      } else {
        PhysicalAddressBits = 36;
      }
    }
    
    //
    // IA-32e paging translates 48-bit linear addresses to 52-bit physical addresses.
    //
    ASSERT (PhysicalAddressBits <= 52);
    if (PhysicalAddressBits > 48) {
      PhysicalAddressBits = 48;
    }

    //
    // Calculate the table entries needed.
    //
    if (PhysicalAddressBits <= 39 ) {
      NumberOfPml4EntriesNeeded = 1;
      NumberOfPdpEntriesNeeded = (UINT32)LShiftU64 (1, (PhysicalAddressBits - 30));
    } else {
      NumberOfPml4EntriesNeeded = (UINT32)LShiftU64 (1, (PhysicalAddressBits - 39));
      NumberOfPdpEntriesNeeded = 512;
    }

    //
    // We need calculate whole page size then allocate once, because S3 restore page table does not know each page in Nvs.
    //
    if (!Page1GSupport) {
      TotalPageTableSize = (UINTN)(1 + NumberOfPml4EntriesNeeded + NumberOfPml4EntriesNeeded * NumberOfPdpEntriesNeeded);
    } else {
      TotalPageTableSize = (UINTN)(1 + NumberOfPml4EntriesNeeded);
    }
    DEBUG ((EFI_D_INFO, "TotalPageTableSize - %x pages\n", TotalPageTableSize));

    //
    // By architecture only one PageMapLevel4 exists - so lets allocate storgage for it.
    //
    S3NvsPageTableAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateAcpiNvsMemoryBelow4G (EFI_PAGES_TO_SIZE(TotalPageTableSize));
    ASSERT (S3NvsPageTableAddress != 0);
    PageMap = (PAGE_MAP_AND_DIRECTORY_POINTER *)(UINTN)S3NvsPageTableAddress;
    S3NvsPageTableAddress += SIZE_4KB;

    PageMapLevel4Entry = PageMap;
    PageAddress        = 0;
    for (IndexOfPml4Entries = 0; IndexOfPml4Entries < NumberOfPml4EntriesNeeded; IndexOfPml4Entries++, PageMapLevel4Entry++) {
      //
      // Each PML4 entry points to a page of Page Directory Pointer entires.
      // So lets allocate space for them and fill them in in the IndexOfPdpEntries loop.
      //
      PageDirectoryPointerEntry = (PAGE_MAP_AND_DIRECTORY_POINTER *)(UINTN)S3NvsPageTableAddress;
      S3NvsPageTableAddress += SIZE_4KB;
      //
      // Make a PML4 Entry
      //
      PageMapLevel4Entry->Uint64 = (UINT64)(UINTN)PageDirectoryPointerEntry;
      PageMapLevel4Entry->Bits.ReadWrite = 1;
      PageMapLevel4Entry->Bits.Present = 1;
    
      if (Page1GSupport) {
        PageDirectory1GEntry = (PAGE_TABLE_1G_ENTRY *)(UINTN)PageDirectoryPointerEntry;
    
        for (IndexOfPageDirectoryEntries = 0; IndexOfPageDirectoryEntries < 512; IndexOfPageDirectoryEntries++, PageDirectory1GEntry++, PageAddress += SIZE_1GB) {
          //
          // Fill in the Page Directory entries
          //
          PageDirectory1GEntry->Uint64 = (UINT64)PageAddress;
          PageDirectory1GEntry->Bits.ReadWrite = 1;
          PageDirectory1GEntry->Bits.Present = 1;
          PageDirectory1GEntry->Bits.MustBe1 = 1;
        }
      } else {
        for (IndexOfPdpEntries = 0; IndexOfPdpEntries < NumberOfPdpEntriesNeeded; IndexOfPdpEntries++, PageDirectoryPointerEntry++) {
          //
          // Each Directory Pointer entries points to a page of Page Directory entires.
          // So allocate space for them and fill them in in the IndexOfPageDirectoryEntries loop.
          //       
          PageDirectoryEntry = (PAGE_TABLE_ENTRY *)(UINTN)S3NvsPageTableAddress;
          S3NvsPageTableAddress += SIZE_4KB;
      
          //
          // Fill in a Page Directory Pointer Entries
          //
          PageDirectoryPointerEntry->Uint64 = (UINT64)(UINTN)PageDirectoryEntry;
          PageDirectoryPointerEntry->Bits.ReadWrite = 1;
          PageDirectoryPointerEntry->Bits.Present = 1;
      
          for (IndexOfPageDirectoryEntries = 0; IndexOfPageDirectoryEntries < 512; IndexOfPageDirectoryEntries++, PageDirectoryEntry++, PageAddress += SIZE_2MB) {
            //
            // Fill in the Page Directory entries
            //
            PageDirectoryEntry->Uint64 = (UINT64)PageAddress;
            PageDirectoryEntry->Bits.ReadWrite = 1;
            PageDirectoryEntry->Bits.Present = 1;
            PageDirectoryEntry->Bits.MustBe1 = 1;
          }
        }
      }
    }
    return (EFI_PHYSICAL_ADDRESS) (UINTN) PageMap;
  } else {
    //
    // If DXE is running 32-bit mode, no need to establish page table.
    //
    return  (EFI_PHYSICAL_ADDRESS) 0;  
  }
}

/**
  Gets the buffer of legacy memory below 1 MB 
  This function is to get the buffer in legacy memory below 1MB that is required during S3 resume.

  @param This           A pointer to the EFI_ACPI_S3_SAVE_PROTOCOL instance.
  @param Size           The returned size of legacy memory below 1 MB.

  @retval EFI_SUCCESS           Size is successfully returned.
  @retval EFI_INVALID_PARAMETER The pointer Size is NULL.

**/
EFI_STATUS
EFIAPI
LegacyGetS3MemorySize (
  IN  EFI_ACPI_S3_SAVE_PROTOCOL   *This,
  OUT UINTN                       *Size
  )
{
  if (Size == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Size = mLegacyRegionSize;
  return EFI_SUCCESS;
}

/**
  Prepares all information that is needed in the S3 resume boot path.
  
  Allocate the resources or prepare informations and save in ACPI variable set for S3 resume boot path  
  
  @param This                 A pointer to the EFI_ACPI_S3_SAVE_PROTOCOL instance.
  @param LegacyMemoryAddress  The base address of legacy memory.

  @retval EFI_NOT_FOUND         Some necessary information cannot be found.
  @retval EFI_SUCCESS           All information was saved successfully.
  @retval EFI_OUT_OF_RESOURCES  Resources were insufficient to save all the information.
  @retval EFI_INVALID_PARAMETER The memory range is not located below 1 MB.

**/
EFI_STATUS
EFIAPI
S3Ready (
  IN EFI_ACPI_S3_SAVE_PROTOCOL    *This,
  IN VOID                         *LegacyMemoryAddress
  )
{
  EFI_STATUS                                    Status;
  EFI_PHYSICAL_ADDRESS                          AcpiS3ContextBuffer;
  ACPI_S3_CONTEXT                               *AcpiS3Context;
  STATIC BOOLEAN                                AlreadyEntered;
  IA32_DESCRIPTOR                               *Idtr;
  IA32_IDT_GATE_DESCRIPTOR                      *IdtGate;

  DEBUG ((EFI_D_INFO, "S3Ready!\n"));

  //
  // Platform may invoke AcpiS3Save->S3Save() before ExitPmAuth, because we need save S3 information there, while BDS ReadyToBoot may invoke it again.
  // So if 2nd S3Save() is triggered later, we need ignore it.
  //
  if (AlreadyEntered) {
    return EFI_SUCCESS;
  }
  AlreadyEntered = TRUE;

  AcpiS3Context = AllocateAcpiNvsMemoryBelow4G (sizeof(*AcpiS3Context));
  ASSERT (AcpiS3Context != NULL);
  AcpiS3ContextBuffer = (EFI_PHYSICAL_ADDRESS)(UINTN)AcpiS3Context;

  //
  // Get ACPI Table because we will save its position to variable
  //
  AcpiS3Context->AcpiFacsTable = (EFI_PHYSICAL_ADDRESS)(UINTN)FindAcpiFacsTable ();
  ASSERT (AcpiS3Context->AcpiFacsTable != 0);

  IdtGate = AllocateAcpiNvsMemoryBelow4G (sizeof(IA32_IDT_GATE_DESCRIPTOR) * 0x100 + sizeof(IA32_DESCRIPTOR));
  Idtr = (IA32_DESCRIPTOR *)(IdtGate + 0x100);
  Idtr->Base  = (UINTN)IdtGate;
  Idtr->Limit = (UINT16)(sizeof(IA32_IDT_GATE_DESCRIPTOR) * 0x100 - 1);
  AcpiS3Context->IdtrProfile = (EFI_PHYSICAL_ADDRESS)(UINTN)Idtr;

  Status = SaveLockBox (
             &mAcpiS3IdtrProfileGuid,
             (VOID *)(UINTN)Idtr,
             (UINTN)sizeof(IA32_DESCRIPTOR)
             );
  ASSERT_EFI_ERROR (Status);

  Status = SetLockBoxAttributes (&mAcpiS3IdtrProfileGuid, LOCK_BOX_ATTRIBUTE_RESTORE_IN_PLACE);
  ASSERT_EFI_ERROR (Status);

  //
  // Allocate page table
  //
  AcpiS3Context->S3NvsPageTableAddress = S3CreateIdentityMappingPageTables ();

  //
  // Allocate stack
  //
  AcpiS3Context->BootScriptStackSize = PcdGet32 (PcdS3BootScriptStackSize);
  AcpiS3Context->BootScriptStackBase = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateAcpiNvsMemoryBelow4G (PcdGet32 (PcdS3BootScriptStackSize));
  ASSERT (AcpiS3Context->BootScriptStackBase != 0);

  //
  // Allocate a code buffer < 4G for S3 debug to load external code
  //
  AcpiS3Context->S3DebugBufferAddress = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateAcpiNvsMemoryBelow4G (EFI_PAGE_SIZE);

  DEBUG((EFI_D_INFO, "AcpiS3Context: AcpiFacsTable is 0x%8x\n", AcpiS3Context->AcpiFacsTable));
  DEBUG((EFI_D_INFO, "AcpiS3Context: IdtrProfile is 0x%8x\n", AcpiS3Context->IdtrProfile));
  DEBUG((EFI_D_INFO, "AcpiS3Context: S3NvsPageTableAddress is 0x%8x\n", AcpiS3Context->S3NvsPageTableAddress));
  DEBUG((EFI_D_INFO, "AcpiS3Context: S3DebugBufferAddress is 0x%8x\n", AcpiS3Context->S3DebugBufferAddress));

  Status = SaveLockBox (
             &gEfiAcpiVariableGuid,
             &AcpiS3ContextBuffer,
             sizeof(AcpiS3ContextBuffer)
             );
  ASSERT_EFI_ERROR (Status);

  Status = SaveLockBox (
             &gEfiAcpiS3ContextGuid,
             (VOID *)(UINTN)AcpiS3Context,
             (UINTN)sizeof(*AcpiS3Context)
             );
  ASSERT_EFI_ERROR (Status);

  Status = SetLockBoxAttributes (&gEfiAcpiS3ContextGuid, LOCK_BOX_ATTRIBUTE_RESTORE_IN_PLACE);
  ASSERT_EFI_ERROR (Status);

  //
  // Launch UpdateAcpiVariableSet without condition as it will call out DxeCsSvcUpdateAcpiVariableSet
  // to let chipset has chance to process chipset RC customized ACPI_VARIABLE_SET if any
  //
  UpdateAcpiVariableSet (AcpiS3Context);

  return EFI_SUCCESS;
}

/**
  The Driver Entry Point.
  
  The function is the driver Entry point which will produce AcpiS3SaveProtocol.
  
  @param ImageHandle   A handle for the image that is initializing this driver
  @param SystemTable   A pointer to the EFI system table

  @retval EFI_SUCCESS:              Driver initialized successfully
  @retval EFI_LOAD_ERROR:           Failed to Initialize or has been loaded
  @retval EFI_OUT_OF_RESOURCES      Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InstallAcpiS3Save (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS        Status;

  if (!FeaturePcdGet(PcdPlatformCsmSupport)) {
    //
    // More memory for no CSM tip, because GDT need relocation
    //
    mLegacyRegionSize = 0x250;
  } else {
    mLegacyRegionSize = 0x100;
  }

  //
  // Launch UpdateAcpiVariableSet without condition as it will call out DxeCsSvcUpdateAcpiVariableSet
  // to let chipset has chance to process chipset RC customized ACPI_VARIABLE_SET if any
  //
  UpdateAcpiVariableSet (NULL);

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiAcpiS3SaveProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mS3Save
                  );
  ASSERT_EFI_ERROR (Status);
  return Status;
}

/**
  Update the content of AcpiVariableSet variable under ECP mode

  @param AcpiS3Context   ACPI s3 context
**/
VOID
UpdateAcpiVariableSet (
  IN ACPI_S3_CONTEXT      *AcpiS3Context
  )
{
  EFI_STATUS                           Status;
  BOOLEAN                              IsS3Ready;
  UINTN                                VarSize;
  ACPI_VARIABLE_SET                    *AcpiVariableSet;
  UINT32                               S3AcpiReservedMemorySize;
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL   *FrameworkMpService;
  EFI_PEI_HOB_POINTERS                 Hob;
  UINT64                               MemoryLengthBelow4G;
  UINT64                               MemoryLengthAbove4G;

  DEBUG ((EFI_D_INFO, "UpdateAcpiVariableSet\n"));

  S3AcpiReservedMemorySize = PcdGet32 (PcdS3AcpiReservedMemorySize);
  //
  // Calling UpdateAcpiVariableSet chipset services for chipset customized AcpiVariableSet approach
  //
  IsS3Ready = (AcpiS3Context == NULL) ? FALSE : TRUE;
  Status = DxeCsSvcUpdateAcpiVariableSet (IsS3Ready, S3AcpiReservedMemorySize);
  if (Status != EFI_UNSUPPORTED) {
    //
    // Chipset services already done the job, quit directly
    //
    return;
  }
  if (!IsS3Ready) {
    //
    // Now in initial state, see is it need to create new AcpiVariableSet based on ECP MpService protocol
    //
    Status = gBS->LocateProtocol (
                    &gFrameworkEfiMpServiceProtocolGuid,
                    NULL,
                    (VOID**) &FrameworkMpService
                    );
    if (EFI_ERROR (Status)) {
      //
      // The ECP MpService protocol not found, create new AcpiVariableSet variable
      //
      AcpiVariableSet = AllocateAcpiNvsMemoryBelow4G (sizeof(ACPI_VARIABLE_SET));
      if (AcpiVariableSet != NULL) {
        Status = gRT->SetVariable (
                        ACPI_GLOBAL_VARIABLE,
                        &gAcpiVariableSetGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        sizeof (ACPI_VARIABLE_SET*),
                        &AcpiVariableSet
                        );
      }
    }
  } else {
    //
    // Now in S3 ready state, update related fields in AcpiVariableSet for use of S3 resume
    //
    AcpiVariableSet = NULL;
    VarSize = sizeof (ACPI_VARIABLE_SET*);
    Status = gRT->GetVariable (
                    ACPI_GLOBAL_VARIABLE,
                    &gAcpiVariableSetGuid,
                    NULL,
                    &VarSize,
                    &AcpiVariableSet
                    );
    if (EFI_ERROR (Status) || AcpiVariableSet == NULL) {
      //
      // Unable to get AcpiVariableSet, quit directly
      //
      return;
    }

    DEBUG((EFI_D_INFO, "AcpiVariableSet is 0x%8x\n", AcpiVariableSet));

    //
    // Allocate ACPI reserved memory under 4G
    //
    AcpiVariableSet->AcpiReservedMemoryBase = (EFI_PHYSICAL_ADDRESS)(UINTN)AllocateAcpiNvsMemoryBelow4G (S3AcpiReservedMemorySize);
    if (AcpiVariableSet->AcpiReservedMemoryBase == 0) {
      return;
    }
    AcpiVariableSet->AcpiReservedMemorySize = S3AcpiReservedMemorySize;
    //
    // Calculate the system memory length by memory hobs
    //
    MemoryLengthBelow4G  = 0x100000;
    MemoryLengthAbove4G  = 0;
    Hob.Raw = GetFirstHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR);
    ASSERT (Hob.Raw != NULL);
    while ((Hob.Raw != NULL) && (!END_OF_HOB_LIST (Hob))) {
      if (Hob.ResourceDescriptor->ResourceType == EFI_RESOURCE_SYSTEM_MEMORY) {
        if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000000) {
          MemoryLengthAbove4G += Hob.ResourceDescriptor->ResourceLength;
        }
        //
        // Skip the memory region below 1MB
        //
        else if (Hob.ResourceDescriptor->PhysicalStart >= 0x100000) {
          MemoryLengthBelow4G += Hob.ResourceDescriptor->ResourceLength;
        }
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
      Hob.Raw = GetNextHob (EFI_HOB_TYPE_RESOURCE_DESCRIPTOR, Hob.Raw);
    }

    AcpiVariableSet->SystemMemoryLengthBelow4GB = MemoryLengthBelow4G;
    AcpiVariableSet->SystemMemoryLengthAbove4GB = MemoryLengthAbove4G;

    DEBUG((EFI_D_INFO, "AcpiVariableThunkPlatform: AcpiMemoryBase is 0x%8x\n", AcpiVariableSet->AcpiReservedMemoryBase));
    DEBUG((EFI_D_INFO, "AcpiVariableThunkPlatform: AcpiMemorySize is 0x%8x\n", AcpiVariableSet->AcpiReservedMemorySize));
    DEBUG((EFI_D_INFO, "AcpiVariableThunkPlatform: SystemMemoryLength below 4G is 0x%8x\n", AcpiVariableSet->SystemMemoryLengthBelow4GB));
    DEBUG((EFI_D_INFO, "AcpiVariableThunkPlatform: SystemMemoryLength above 4G is 0x%8x\n", AcpiVariableSet->SystemMemoryLengthAbove4GB));
  }
}