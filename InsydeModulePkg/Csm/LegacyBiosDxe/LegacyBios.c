/** @file
  Legacy Bios Driver

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
#include "LegacyTpm.h"

#define PHYSICAL_ADDRESS_TO_POINTER(Address)  ((VOID *) ((UINTN) Address))
#define EBDA(a)                               (*(UINT8*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (a)))
//
// define maximum number of HDD system supports
//
#define MAX_HDD_ENTRIES 0x30

//
// Module Global:
//  Since this driver will only ever produce one instance of the Private Data
//  protocol you are not required to dynamically allocate the PrivateData.
//
LEGACY_BIOS_INSTANCE  mPrivateData;

H2O_TABLE             *H2oTablePtr = NULL;
EFI_HANDLE            mImageHandle;
EFI_SYSTEM_TABLE      *mSystemTable;
EFI_PHYSICAL_ADDRESS  mEbda = 0;

/**
  Do an AllocatePages () of type AllocateMaxAddress for EfiBootServicesCode
  memory.

  @param  AllocateType               Allocated Legacy Memory Type
  @param  StartPageAddress           Start address of range
  @param  Pages                      Number of pages to allocate
  @param  Result                     Result of allocation

  @retval EFI_SUCCESS                Legacy16 code loaded
  @retval Other                      No protocol installed, unload driver.

**/
EFI_STATUS
AllocateLegacyMemory (
  IN  EFI_ALLOCATE_TYPE         AllocateType,
  IN  EFI_PHYSICAL_ADDRESS      StartPageAddress,
  IN  UINTN                     Pages,
  OUT EFI_PHYSICAL_ADDRESS      *Result
  )
{
  EFI_STATUS            Status;
  EFI_PHYSICAL_ADDRESS  MemPage;

  //
  // Allocate Pages of memory less <= StartPageAddress
  //
  MemPage = (EFI_PHYSICAL_ADDRESS) (UINTN) StartPageAddress;
  Status = gBS->AllocatePages (
                  AllocateType,
                  EfiBootServicesCode,
                  Pages,
                  &MemPage
                  );
  //
  // Do not ASSERT on Status error but let caller decide since some cases
  // memory is already taken but that is ok.
  //
  if (!EFI_ERROR (Status)) {
    *Result = (EFI_PHYSICAL_ADDRESS) (UINTN) MemPage;
  }
  //
  // If reach here the status = EFI_SUCCESS
  //
  return Status;
}


/**
  Check LegacyRegion(E0000~ FFFFF) lock status

  @retval True                Lock
  @retval False               Unlock

**/
BOOLEAN
EFIAPI
GetLockStatus (
  )
{
  UINT32                CheckValue;

  CheckValue = *(UINT32 *)((UINTN)0xF0000);
  if (CheckValue) {
    SetMem ((VOID *) (UINT32 *)((UINTN)0xF0000), sizeof(UINT32), 0x0);
  } else {
    SetMem ((VOID *) (UINT32 *)((UINTN)0xF0000), sizeof(UINT32), 0xff);
  }
  if (CheckValue != *(UINT32 *)((UINTN)0xF0000)) {
    *(UINT32 *)((UINTN)0xF0000) = CheckValue;
    return FALSE;
  }

  return TRUE;
}

/**
  This function is called when EFI needs to reserve an area in the 0xE0000 or 0xF0000
  64 KB blocks.

  Note: inconsistency with the Framework CSM spec. Per the spec, this function may be
  invoked only once. This limitation is relaxed to allow multiple calls in this implemenation.

  @param  This                       Protocol instance pointer.
  @param  LegacyMemorySize           Size of required region
  @param  Region                     Region to use. 00 = Either 0xE0000 or 0xF0000
                                     block Bit0 = 1 0xF0000 block Bit1 = 1 0xE0000
                                     block
  @param  Alignment                  Address alignment. Bit mapped. First non-zero
                                     bit from right is alignment.
  @param  LegacyMemoryAddress        Region Assigned

  @retval EFI_SUCCESS                Region assigned
  @retval EFI_ACCESS_DENIED          Procedure previously invoked
  @retval Other                      Region not assigned

**/
EFI_STATUS
EFIAPI
LegacyBiosGetLegacyRegion (
  IN    EFI_LEGACY_BIOS_PROTOCOL *This,
  IN    UINTN                    LegacyMemorySize,
  IN    UINTN                    Region,
  IN    UINTN                    Alignment,
  OUT   VOID                     **LegacyMemoryAddress
  )
{

  LEGACY_BIOS_INSTANCE  *Private;
  EFI_IA32_REGISTER_SET Regs;
  EFI_STATUS            Status;
  UINT32                Granularity;
  BOOLEAN               LockStatus;

  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);

  LockStatus = GetLockStatus ();

  if (LockStatus) {
    Private->LegacyRegion->UnLock (Private->LegacyRegion, 0xE0000, 0x20000, &Granularity);
  }

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.BX = (UINT16) Region;
  Regs.X.CX = (UINT16) LegacyMemorySize;
  Regs.X.DX = (UINT16) Alignment;
  Private->LegacyBios.FarCall86 (
     &Private->LegacyBios,
     Private->Legacy16CallSegment,
     Private->Legacy16CallOffset,
     &Regs,
     NULL,
     0
     );

  if (Regs.X.AX == 0) {
    *LegacyMemoryAddress  = (VOID *) (UINTN) ((Regs.X.DS << 4) + Regs.X.BX);
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_OUT_OF_RESOURCES;
  }

  Private->Cpu->FlushDataCache (Private->Cpu, 0xE0000, 0x20000, EfiCpuFlushTypeWriteBackInvalidate);

  if (LockStatus) {
    Private->LegacyRegion->Lock (Private->LegacyRegion, 0xE0000, 0x20000, &Granularity);
  }
  return Status;
}


/**
  This function is called when copying data to the region assigned by
  EFI_LEGACY_BIOS_PROTOCOL.GetLegacyRegion().

  @param  This                       Protocol instance pointer.
  @param  LegacyMemorySize           Size of data to copy
  @param  LegacyMemoryAddress        Legacy Region destination address Note: must
                                     be in region assigned by
                                     LegacyBiosGetLegacyRegion
  @param  LegacyMemorySourceAddress  Source of data

  @retval EFI_SUCCESS                The data was copied successfully.
  @retval EFI_ACCESS_DENIED          Either the starting or ending address is out of bounds.
**/
EFI_STATUS
EFIAPI
LegacyBiosCopyLegacyRegion (
  IN EFI_LEGACY_BIOS_PROTOCOL *This,
  IN    UINTN                 LegacyMemorySize,
  IN    VOID                  *LegacyMemoryAddress,
  IN    VOID                  *LegacyMemorySourceAddress
  )
{

  LEGACY_BIOS_INSTANCE  *Private;
  UINT32                Granularity;
  BOOLEAN               LockStatus;

  if ((LegacyMemoryAddress < (VOID *)(UINTN)0xE0000 ) ||
      ((UINTN) LegacyMemoryAddress + LegacyMemorySize > (UINTN) 0x100000)
        ) {
    return EFI_ACCESS_DENIED;
  }
  //
  // There is no protection from writes over lapping if this function is
  // called multiple times.
  //
  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  LockStatus = GetLockStatus ();
  if (LockStatus) {
    Private->LegacyRegion->UnLock (Private->LegacyRegion, 0xE0000, 0x20000, &Granularity);
  }
  CopyMem (LegacyMemoryAddress, LegacyMemorySourceAddress, LegacyMemorySize);

  Private->Cpu->FlushDataCache (Private->Cpu, 0xE0000, 0x20000, EfiCpuFlushTypeWriteBackInvalidate);
  if (LockStatus) {
    Private->LegacyRegion->Lock (Private->LegacyRegion, 0xE0000, 0x20000, &Granularity);
  }
  return EFI_SUCCESS;
}

/**
  This function is invoked when gEfiAcpiSupportProtocolGuid installed or ReadyToBoot event signaled

  @param  Event                 Event type
  @param  Context               Context for the event

**/
STATIC
VOID
InstallAcpiTimer (
  IN  EFI_EVENT         Event,
  IN  VOID              *Context
  )
{
  EFI_STATUS                    Status;
  EFI_ACPI_SUPPORT_PROTOCOL     *AcpiSupport;
  UINTN                         Index;
  EFI_ACPI_DESCRIPTION_HEADER   *Table;
  UINTN                         Handle;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_EVENT                     ReadyToBootEvent;

  if (Event) gBS->CloseEvent (Event);
  Status = gBS->LocateProtocol (
                  &gEfiAcpiSupportProtocolGuid,
                  NULL,
                  (VOID **)&AcpiSupport
                  );
  if (EFI_ERROR(Status)) {
    return;
  }
  for (Index = 0, Status = EFI_SUCCESS; Status == EFI_SUCCESS; Index ++) {
    Status = AcpiSupport->GetAcpiTable (
                            AcpiSupport,
                            Index,
                            (VOID **)&Table,
                            &Version,
                            &Handle
                            );
    if (!EFI_ERROR (Status)) {
      if (Table->Signature == EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE) {
        ((H2O_TABLE*)Context)->AcpiTimer = (UINT16)((EFI_ACPI_3_0_FIXED_ACPI_DESCRIPTION_TABLE*)Table)->PmTmrBlk;
        Status = EFI_ABORTED;
      }
      gBS->FreePool (Table);
    }
  }
  if (((H2O_TABLE*)Context)->AcpiTimer == 0) {
    //
    // Register event on ready to boot to ensure the ACPI timer address actually get updated
    //
    Status = EfiCreateEventReadyToBootEx (
               TPL_CALLBACK,
               InstallAcpiTimer,
               Context,
               &ReadyToBootEvent
               );
  }
}

/**
  Find Legacy16 BIOS image in the FLASH device and shadow it into memory. Find
  the $EFI table in the shadow area. Thunk into the Legacy16 code after it had
  been shadowed.

  @param  Private                    Legacy BIOS context data

  @retval EFI_SUCCESS                Legacy16 code loaded
  @retval Other                      No protocol installed, unload driver.

**/
EFI_STATUS
ShadowAndStartLegacy16 (
  IN  LEGACY_BIOS_INSTANCE  *Private
  )
{
  EFI_STATUS                        Status;
  UINT8                             *Ptr;
  UINT8                             *PtrEnd;
  BOOLEAN                           Done;
  EFI_COMPATIBILITY16_TABLE         *Table;
  UINT8                             CheckSum;
  EFI_IA32_REGISTER_SET             Regs;
  EFI_TO_COMPATIBILITY16_INIT_TABLE *EfiToLegacy16InitTable;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  VOID                              *LegacyBiosImage;
  UINTN                             LegacyBiosImageSize;
  UINTN                             E820Size;
  UINT32                            *ClearPtr;
  BBS_TABLE                         *BbsTable;
  LEGACY_EFI_HDD_TABLE              *LegacyEfiHddTable;
  UINTN                             Index;
  UINTN                             Location;
  UINTN                             Alignment;
  UINTN                             TempData;
  EFI_PHYSICAL_ADDRESS              Address;
  UINT16                            OldMask;
  UINT16                            NewMask;
  UINT32                            Granularity;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR   Descriptor;
  EFI_ACPI_SUPPORT_PROTOCOL         *AcpiSupport;
  EFI_EVENT                         Event;
  VOID                              *Registration;

  Location  = 0;
  Alignment = 0;

  //
  // we allocate the C/D/E/F segment as RT code so no one will use it any more.
  //
  Address = 0xC0000;
  gDS->GetMemorySpaceDescriptor (Address, &Descriptor);
  if (Descriptor.GcdMemoryType == EfiGcdMemoryTypeSystemMemory) {
    //
    // If it is already reserved, we should be safe, or else we allocate it.
    //
    Status = gBS->AllocatePages (
                    AllocateAddress,
                    EfiRuntimeServicesCode,
                    0x40000/EFI_PAGE_SIZE,
                    &Address
                    );
    if (EFI_ERROR (Status)) {
      //
      // Bugbug: need to figure out whether C/D/E/F segment should be marked as reserved memory.
      //
      DEBUG ((DEBUG_ERROR, "Failed to allocate the C/D/E/F segment Status = %r", Status));
    }
  }

  EfiToLegacy16BootTable = &Private->IntThunk->EfiToLegacy16BootTable;
  Status = Private->LegacyBiosPlatform->GetPlatformInfo (
                                          Private->LegacyBiosPlatform,
                                          EfiGetPlatformBinarySystemRom,
                                          &LegacyBiosImage,
                                          &LegacyBiosImageSize,
                                          &Location,
                                          &Alignment,
                                          0,
                                          0
                                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private->BiosStart            = (UINT32) (0x100000 - LegacyBiosImageSize);
  Private->OptionRom            = 0xc0000;
  Private->LegacyBiosImageSize  = (UINT32) LegacyBiosImageSize;

  //
  // Can only shadow into memory allocated for legacy useage.
  //
  ASSERT (Private->BiosStart > Private->OptionRom);

  //
  // Shadow Legacy BIOS. Turn on memory and copy image
  //
  Private->LegacyRegion->UnLock (Private->LegacyRegion, 0xc0000, 0x40000, &Granularity);

  ClearPtr = (VOID *) ((UINTN) 0xc0000);

  //
  // Initialize region from 0xc0000 to start of BIOS to all ffs. This allows unused
  // regions to be used by EMM386 etc.
  //
  SetMem ((VOID *) ClearPtr, (UINTN) (0x40000 - LegacyBiosImageSize), 0xff);

  TempData = Private->BiosStart;

  CopyMem (
    (VOID *) TempData,
    LegacyBiosImage,
    (UINTN) LegacyBiosImageSize
    );

  //
  // TPM feature - Measure Legacy BIOS Image
  //
  Status = TcgMeasureLegacyBiosImage ((EFI_PHYSICAL_ADDRESS)Private->BiosStart, (UINTN)LegacyBiosImageSize);

  Private->Cpu->FlushDataCache (Private->Cpu, 0xc0000, 0x40000, EfiCpuFlushTypeWriteBackInvalidate);

  //
  // Search for Legacy16 table in Shadowed ROM
  //
  Done  = FALSE;
  Table = NULL;
  for (Ptr = (UINT8 *) TempData; Ptr < (UINT8 *) ((UINTN) 0x100000) && !Done; Ptr += 0x10) {
    if (*(UINT32 *) Ptr == SIGNATURE_32 ('I', 'F', 'E', '$')) {
      Table   = (EFI_COMPATIBILITY16_TABLE *) Ptr;
      PtrEnd  = Ptr + Table->TableLength;
      for (CheckSum = 0; Ptr < PtrEnd; Ptr++) {
        CheckSum = (UINT8) (CheckSum +*Ptr);
      }

      if (CheckSum == 0) {
        Done = TRUE;
      }
    }
  }

  //
  //  Search RSD Ptr and update signature to uninitialized
  //
  for (Ptr = (UINT8 *)((UINTN)0xFE000); Ptr < (UINT8 *)((UINTN)0xFF000); Ptr += 0x10) {
    if (*(UINT64 *)Ptr == EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE) {
      *(UINT8 *)Ptr = '_';
      break;
    }
  }
  if (Table == NULL) {
    DEBUG ((EFI_D_ERROR, "No Legacy16 table found\n"));
    return EFI_NOT_FOUND;
  }

  if (!Done) {
    //
    // Legacy16 table header checksum error.
    //
    DEBUG ((EFI_D_ERROR, "Legacy16 table found with bad talbe header checksum\n"));
  }

  //
  // Remember location of the Legacy16 table
  //
  Private->Legacy16Table            = Table;
  Private->Legacy16CallSegment      = Table->Compatibility16CallSegment;
  Private->Legacy16CallOffset       = Table->Compatibility16CallOffset;
  EfiToLegacy16InitTable            = &Private->IntThunk->EfiToLegacy16InitTable;
  Private->Legacy16InitPtr          = EfiToLegacy16InitTable;
  Private->Legacy16BootPtr          = &Private->IntThunk->EfiToLegacy16BootTable;
  Private->InternalIrqRoutingTable  = NULL;
  Private->NumberIrqRoutingEntries  = 0;
  Private->BbsTablePtr              = NULL;
  Private->LegacyEfiHddTable        = NULL;
  Private->DiskEnd                  = 0;
  Private->Disk4075                 = 0;
  Private->HddTablePtr              = &Private->IntThunk->EfiToLegacy16BootTable.HddInfo;
  Private->NumberHddControllers     = MAX_IDE_CONTROLLER;
  Private->Dump[0]                  = 'D';
  Private->Dump[1]                  = 'U';
  Private->Dump[2]                  = 'M';
  Private->Dump[3]                  = 'P';

  ZeroMem (
    Private->Legacy16BootPtr,
    sizeof (EFI_TO_COMPATIBILITY16_BOOT_TABLE)
    );

  //
  // Store away a copy of the EFI System Table
  //
  Table->EfiSystemTable = (UINT32) (UINTN) gST;

  //
  // IPF CSM integration -Bug
  //
  // Construct the Legacy16 boot memory map. This sets up number of
  // E820 entries.
  //
  LegacyBiosBuildE820 (Private, &E820Size);
  //
  // Initialize BDA and EBDA standard values needed to load Legacy16 code
  //
  LegacyBiosInitBda (Private);
  LegacyBiosInitCmos (Private);


  //
  // First assign SW_SMI_PORT to CSM16, Search "$H2O" to find SwSmiport.
  //
  for (Ptr = (UINT8 *)((UINTN)0xFE000); Ptr < (UINT8 *)((UINTN)0xFF000); Ptr += 0x10) {
    if (*(UINT32 *) Ptr == SIGNATURE_32 ('O', '2', 'H', '$')) {
      H2oTablePtr = (H2O_TABLE*)Ptr;
      H2oTablePtr->SwSmiPort        = (UINT16)PcdGet16(PcdSoftwareSmiPort);
      H2oTablePtr->DebugPort        = (UINT16)PcdGet16(PcdH2ODebugPort);
      H2oTablePtr->PCI30Support    |= EFI_SUPPORT_PCI3_0;
      H2oTablePtr->E820SupportExtAttributes |= EFI_SUPPORT_E820_EXT_ATTRIBUTES;
      H2oTablePtr->BootTableSegment = NORMALIZE_EFI_SEGMENT(*(UINT32*)&EfiToLegacy16BootTable);
      H2oTablePtr->BootTableOffset  = NORMALIZE_EFI_OFFSET (*(UINT32*)&EfiToLegacy16BootTable);
      //
      // Register Legacy SMI Handler
      //
      Private->LegacyBiosPlatform->SmmInit (
                                     Private->LegacyBiosPlatform,
                                     EfiToLegacy16BootTable
                                     );
      //
      // OemServices
      //
      H2oTablePtr->CSM16OemSwitch = PcdGet16 (PcdDefaultCsm16ReferSwitch);
      Status = OemSvcCsm16ReferSwitch (
                 &H2oTablePtr->CSM16OemSwitch
                 );
      //
      // Go through the ACPI table to get the AcpiTimer address for stall operation in CSM16
      //
      Status = gBS->LocateProtocol (
                      &gEfiAcpiSupportProtocolGuid,
                      NULL,
                      (VOID **)&AcpiSupport
                      );
      if (EFI_ERROR(Status)) {
        //
        // Register AcpiSupportProtocol notification to install AcpiTimer address
        //
        Status = gBS->CreateEvent (
                        EVT_NOTIFY_SIGNAL,
                        TPL_CALLBACK,
                        InstallAcpiTimer,
                        (VOID*)H2oTablePtr,
                        &Event
                        );
        if (!EFI_ERROR (Status)) {
          Status = gBS->RegisterProtocolNotify (
                          &gEfiAcpiSupportProtocolGuid,
                          Event,
                          &Registration
                          );
        }
      } else {
        InstallAcpiTimer (NULL, H2oTablePtr);
      }
      break;
    }
  }

  //
  // All legacy interrupt should be masked when do initialization work from legacy 16 code.
  //
  Private->Legacy8259->GetMask(Private->Legacy8259, &OldMask, NULL, NULL, NULL);
  NewMask = 0xFFFF;
  Private->Legacy8259->SetMask(Private->Legacy8259, &NewMask, NULL, NULL, NULL);
  //
  // Call into Legacy16 code to do an INIT
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16InitializeYourself;
  Regs.X.ES = EFI_SEGMENT (*((UINT32 *) &EfiToLegacy16InitTable));
  Regs.X.BX = EFI_OFFSET (*((UINT32 *) &EfiToLegacy16InitTable));

  Private->LegacyBios.FarCall86 (
                        &Private->LegacyBios,
                        Table->Compatibility16CallSegment,
                        Table->Compatibility16CallOffset,
                        &Regs,
                        NULL,
                        0
                        );

  //
  // Restore original legacy interrupt mask value
  //
  Private->Legacy8259->SetMask(Private->Legacy8259, &OldMask, NULL, NULL, NULL);

  if (Regs.X.AX != 0) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Copy E820 table after InitializeYourself is completed
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.CX = (UINT16) E820Size;
  Regs.X.BX = 1;
  Private->LegacyBios.FarCall86 (
                        &Private->LegacyBios,
                        Table->Compatibility16CallSegment,
                        Table->Compatibility16CallOffset,
                        &Regs,
                        NULL,
                        0
                        );

  Table->E820Pointer  = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);
  Table->E820Length   = (UINT32) E820Size;
  if (Regs.X.AX != 0) {
    DEBUG ((EFI_D_ERROR, "Legacy16 E820 length insufficient\n"));
  } else {
    TempData = Table->E820Pointer;
    CopyMem ((VOID *) TempData, Private->E820Table, E820Size);
  }
  //
  // Get PnPInstallationCheck Info.
  //
  Private->PnPInstallationCheckSegment  = Table->PnPInstallationCheckSegment;
  Private->PnPInstallationCheckOffset   = Table->PnPInstallationCheckOffset;

  //
  // Check if PCI Express is supported. If yes, Save base address.
  //
  Status = Private->LegacyBiosPlatform->GetPlatformInfo (
                                          Private->LegacyBiosPlatform,
                                          EfiGetPlatformPciExpressBase,
                                          NULL,
                                          NULL,
                                          &Location,
                                          &Alignment,
                                          0,
                                          0
                                          );
  if (!EFI_ERROR (Status)) {
    Private->Legacy16Table->PciExpressBase  = (UINT32)Location;
    Location = 0;
  }

  //
  // Check if TPM is supported. If yes get a region in E0000,F0000 to copy it
  // into, copy it and update pointer to binary image. This needs to be
  // done prior to any OPROM for security purposes.
  //
  Status = LoadTpmBinaryImage (Private, Table);

  //
  // Lock the Legacy BIOS region
  //
  Private->Cpu->FlushDataCache (Private->Cpu, Private->BiosStart, (UINT32) LegacyBiosImageSize, EfiCpuFlushTypeWriteBackInvalidate);
  Private->LegacyRegion->Lock (Private->LegacyRegion, Private->BiosStart, (UINT32) LegacyBiosImageSize, &Granularity);

  //
  // Get the BbsTable from LOW_MEMORY_THUNK
  //
  BbsTable = (BBS_TABLE *)(UINTN)Private->IntThunk->BbsTable;
  ZeroMem ((VOID *)BbsTable, sizeof (Private->IntThunk->BbsTable));

  EfiToLegacy16BootTable->BbsTable  = (UINT32)(UINTN)BbsTable;
  Private->BbsTablePtr              = (VOID *) BbsTable;
  //
  // Skip Floppy and possible onboard IDE drives
  //
  EfiToLegacy16BootTable->NumberBbsEntries = 1 + 2 * MAX_IDE_CONTROLLER;

  for (Index = 0; Index < (sizeof (Private->IntThunk->BbsTable) / sizeof (BBS_TABLE)); Index++) {
    BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
  }
  //
  // Allocate space for Legacy HDD table
  //
  LegacyEfiHddTable = (LEGACY_EFI_HDD_TABLE *) AllocateZeroPool ((UINTN) MAX_HDD_ENTRIES * sizeof (LEGACY_EFI_HDD_TABLE));
  ASSERT (LegacyEfiHddTable);

  Private->LegacyEfiHddTable      = LegacyEfiHddTable;
  Private->LegacyEfiHddTableIndex = 0x00;

  POST_CODE (DXE_AFTER_CSM16_INIT); //PostCode = 0x5A, Legacy interrupt function initial
  return EFI_SUCCESS;
}

/**
  Shadow all legacy16 OPROMs that haven't been shadowed.
  Warning: Use this with caution. This routine disconnects all EFI
  drivers. If used externally then caller must re-connect EFI
  drivers.

  @param  This                    Protocol instance pointer.

  @retval EFI_SUCCESS             OPROMs shadowed

**/
EFI_STATUS
EFIAPI
LegacyBiosShadowAllLegacyOproms (
  IN EFI_LEGACY_BIOS_PROTOCOL *This
  )
{
  LEGACY_BIOS_INSTANCE  *Private;

  //
  //  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL    *LegacyBiosPlatform;
  //  EFI_LEGACY16_TABLE                   *Legacy16Table;
  //
  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);

  //
  //  LegacyBiosPlatform       = Private->LegacyBiosPlatform;
  //  Legacy16Table            = Private->Legacy16Table;
  //
  // Shadow PCI ROMs. We must do this near the end since this will kick
  // of Native EFI drivers that may be needed to collect info for Legacy16
  //
  //  WARNING: PciIo is gone after this call.
  //
  PciProgramAllInterruptLineRegisters (Private);

  PciShadowRoms (Private);

  //
  // Shadow PXE base code, BIS etc.
  //
  //  LegacyBiosPlatform->ShadowServiceRoms (LegacyBiosPlatform,
  //                       &Private->OptionRom,
  //                       Legacy16Table);
  //
  return EFI_SUCCESS;
}

/**
  Get the PCI BIOS interface version.

  @param  Private  Driver private data.

  @return The PCI interface version number in Binary Coded Decimal (BCD) format.
          E.g.: 0x0210 indicates 2.10, 0x0300 indicates 3.00

**/
UINT16
GetPciInterfaceVersion (
  IN LEGACY_BIOS_INSTANCE *Private
  )
{
  EFI_IA32_REGISTER_SET Reg;
  BOOLEAN               ThunkFailed;
  UINT16                PciInterfaceVersion;

  PciInterfaceVersion = 0;

  Reg.X.AX = 0xB101;
  Reg.E.EDI = 0;

  ThunkFailed = Private->LegacyBios.Int86 (&Private->LegacyBios, 0x1A, &Reg);
  if (!ThunkFailed) {
    //
    // From PCI Firmware 3.0 Specification:
    //   If the CARRY FLAG [CF] is cleared and AH is set to 00h, it is still necessary to examine the
    //   contents of [EDX] for the presence of the string "PCI" + (trailing space) to fully validate the
    //   presence of the PCI function set. [BX] will further indicate the version level, with enough
    //   granularity to allow for incremental changes in the code that don't affect the function interface.
    //   Version numbers are stored as Binary Coded Decimal (BCD) values. For example, Version 2.10
    //   would be returned as a 02h in the [BH] registers and 10h in the [BL] registers.
    //
    if ((Reg.X.Flags.CF == 0) && (Reg.H.AH == 0) && (Reg.E.EDX == SIGNATURE_32 ('P', 'C', 'I', ' '))) {
      PciInterfaceVersion = Reg.X.BX;
    }
  }
  return PciInterfaceVersion;
}

/**
  Produce Legacy BIOS protocol.

  @param  ImageHandle  Handle of driver image.
  @param  SystemTable  Pointer to system table.

  @retval EFI_SUCCESS  Legacy BIOS protocol installed
  @retval No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
LegacyBiosInstallInstance (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                         Status;
  LEGACY_BIOS_INSTANCE               *Private;
  EFI_TO_COMPATIBILITY16_INIT_TABLE  *EfiToLegacy16InitTable;
  EFI_PHYSICAL_ADDRESS               MemoryAddress;
  UINTN                              Index;
  UINT32                             *BaseVectorMaster;
  EFI_PHYSICAL_ADDRESS               StartAddress;
  UINT32                             *ClearPtr;
  EFI_PHYSICAL_ADDRESS               MemStart;
  UINT32                             IntRedirCode;
  UINT32                             Granularity;
  BOOLEAN                            DecodeOn;
  UINT32                             MemorySize;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR    Descriptor;
  UINT64                             Length;
  UINTN                              DataSize;
  UINT8                              QuickBootSetting;
  UINT32                             *IdtArray;
  UINTN                              MemLength;
  EFI_IA32_REGISTER_SET              Regs;
  VOID                               *InterruptRedirectionCodePtr;


  Private = &mPrivateData;
  ZeroMem (Private, sizeof (LEGACY_BIOS_INSTANCE));

  //
  // Grab a copy of all the protocols we depend on. Any error would
  // be a dispatcher bug!.
  //
  Status = gBS->LocateProtocol (&gEfiCpuArchProtocolGuid, NULL, (VOID **) &Private->Cpu);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiTimerArchProtocolGuid, NULL, (VOID **) &Private->Timer);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiLegacyRegion2ProtocolGuid, NULL, (VOID **) &Private->LegacyRegion);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, (VOID **) &Private->LegacyBiosPlatform);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiLegacy8259ProtocolGuid, NULL, (VOID **) &Private->Legacy8259);
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (&gEfiLegacyInterruptProtocolGuid, NULL, (VOID **) &Private->LegacyInterrupt);
  ASSERT_EFI_ERROR (Status);

  //
  // Locate Memory Test Protocol if exists
  //
  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  (VOID **) &Private->GenericMemoryTest
                  );
  ASSERT_EFI_ERROR (Status);

  DataSize = sizeof (QuickBootSetting);
  Status = gRT->GetVariable (
                  L"QuickBoot",
                  &gEfiGenericVariableGuid,
                  NULL,
                  &DataSize,
                  &QuickBootSetting
                  );

  //
  // QuickBoot variable exists and QuickBoot is disabled, then do memory test from
  // 1M to 16M. Otherwise, skip the memory test
  //
  if (!EFI_ERROR (Status) && (QuickBootSetting == 0)) {
    //
    // Make sure all memory from 1MB to 16MB is tested and added to memory map
    //
    for (StartAddress = BASE_1MB; StartAddress < BASE_16MB; ) {
      gDS->GetMemorySpaceDescriptor (StartAddress, &Descriptor);
      if (Descriptor.GcdMemoryType != EfiGcdMemoryTypeReserved) {
        StartAddress = Descriptor.BaseAddress + Descriptor.Length;
        continue;
      }
      Length = MIN (Descriptor.Length, BASE_16MB - StartAddress);
      Private->GenericMemoryTest->CompatibleRangeTest (
                                    Private->GenericMemoryTest,
                                    StartAddress,
                                    Length
                                    );
      StartAddress = StartAddress + Length;
    }
  }

  Private->Signature = LEGACY_BIOS_INSTANCE_SIGNATURE;

  Private->LegacyBios.Int86 = LegacyBiosInt86;
  Private->LegacyBios.FarCall86 = LegacyBiosFarCall86;
  Private->LegacyBios.CheckPciRom = LegacyBiosCheckPciRom;
  Private->LegacyBios.InstallPciRom = LegacyBiosInstallPciRom;
  Private->LegacyBios.LegacyBoot = LegacyBiosLegacyBoot;
  Private->LegacyBios.UpdateKeyboardLedStatus = LegacyBiosUpdateKeyboardLedStatus;
  Private->LegacyBios.GetBbsInfo = LegacyBiosGetBbsInfo;
  Private->LegacyBios.ShadowAllLegacyOproms = LegacyBiosShadowAllLegacyOproms;
  Private->LegacyBios.PrepareToBootEfi = LegacyBiosPrepareToBootEfi;
  Private->LegacyBios.GetLegacyRegion = LegacyBiosGetLegacyRegion;
  Private->LegacyBios.CopyLegacyRegion = LegacyBiosCopyLegacyRegion;
  Private->LegacyBios.BootUnconventionalDevice = LegacyBiosBootUnconventionalDevice;

  Private->ImageHandle = ImageHandle;

  //
  // Enable read attribute of legacy region.
  //
  DecodeOn = TRUE;
  Private->LegacyRegion->Decode (
                           Private->LegacyRegion,
                           0xc0000,
                           0x40000,
                           &Granularity,
                           &DecodeOn
                           );
  //
  // Set Cachebility for legacy region
  // BUGBUG: Comments about this legacy region cacheability setting
  //         This setting will make D865GCHProduction CSM Unhappy
  //
  if (PcdGetBool (PcdLegacyBiosCacheLegacyRegion)) {
    gDS->SetMemorySpaceAttributes (
           0x0,
           0xA0000,
           EFI_MEMORY_WB
           );
    gDS->SetMemorySpaceAttributes (
           0xc0000,
           0x40000,
           EFI_MEMORY_WB
           );
  }

  gDS->SetMemorySpaceAttributes (
         0xA0000,
         0x20000,
         EFI_MEMORY_UC
         );

  //
  // Allocate 0 - 4K for real mode interupt vectors and BDA.
  //
  Status = AllocateLegacyMemory (
             AllocateAddress,
             0,
             1,
             &MemoryAddress
             );
  if (!EFI_ERROR (Status)) {
    ClearPtr = (VOID *) ((UINTN) 0x0000);

    //
    // Initialize region from 0x0000 to 4k. This initializes interrupt vector
    // range.
    //
    gBS->SetMem ((VOID *) ClearPtr, 0x400, INITIAL_VALUE_BELOW_1K);
    ZeroMem ((VOID *) ((UINTN)ClearPtr + 0x400), 0xC00);
  }
  //
  // Allocate pages for the usage of EBDA and OPROM
  //
  MemorySize    = PcdGet32 (PcdEbdaReservedMemorySize);
  MemoryAddress = CONVENTIONAL_MEMORY_TOP - MemorySize;
  ASSERT ((MemorySize & 0xFFF) == 0);
  Status = gBS->AllocatePages (
                  AllocateAddress,
                  EfiRuntimeServicesData,
                  EFI_SIZE_TO_PAGES (MemorySize),
                  &MemoryAddress
                  );
  if (!EFI_ERROR (Status)) {
    ZeroMem ((VOID *) ((UINTN) MemoryAddress), MemorySize);
  }
  //
  // Allocate space for CSM thunk code/data
  //
  Status = AllocateLegacyMemory (
             AllocateMaxAddress,
             CONVENTIONAL_MEMORY_TOP,
             EFI_SIZE_TO_PAGES (sizeof (LOW_MEMORY_THUNK)),
             &MemoryAddress
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem ((VOID *) ((UINTN) MemoryAddress), sizeof(LOW_MEMORY_THUNK));
  Private->IntThunk                        = (LOW_MEMORY_THUNK *) (UINTN) MemoryAddress;
  EfiToLegacy16InitTable                   = &Private->IntThunk->EfiToLegacy16InitTable;
  EfiToLegacy16InitTable->ThunkStart       = (UINT32) (EFI_PHYSICAL_ADDRESS) (UINTN) MemoryAddress;
  EfiToLegacy16InitTable->ThunkSizeInBytes = (UINT32) (sizeof (LOW_MEMORY_THUNK));
  Status = LegacyBiosInitializeThunk (Private);
  //
  // Allocate memory for Legacy OPROMs that doesn't use PMM but look for zeroed memory.
  // Note that various non-BBS SCSIs expect different areas to be free
  // Use for-loop to allocate memory page by page to skip occupied memory 
  //
  MemoryAddress = PcdGet32 (PcdOpromReservedMemoryBase);
  MemorySize    = PcdGet32 (PcdOpromReservedMemorySize);
  for (StartAddress = MemoryAddress, MemorySize = EFI_SIZE_TO_PAGES (MemorySize); StartAddress < CONVENTIONAL_MEMORY_TOP && MemorySize > 0; StartAddress += 0x1000) {
    Status = AllocateLegacyMemory (
               AllocateAddress,
               StartAddress,
               1,
               &MemoryAddress
               );
    if (!EFI_ERROR (Status)) {
      ZeroMem ((VOID *) ((UINTN) MemoryAddress), 0x1000);
      MemorySize --;
    }
  }
  //
  // Allocate low PMM memory and zero it out
  //
  MemorySize = PcdGet32 (PcdLowPmmMemorySize);
  ASSERT ((MemorySize & 0xFFF) == 0);
  Status = AllocateLegacyMemory (
             AllocateMaxAddress,
             CONVENTIONAL_MEMORY_TOP,
             EFI_SIZE_TO_PAGES (MemorySize),
             &MemoryAddress
             );
  if (!EFI_ERROR (Status)) {
    ZeroMem ((VOID *) ((UINTN) MemoryAddress), MemorySize);
  }
  //
  // Init the legacy memory map in memory < 1 MB.
  //
  EfiToLegacy16InitTable->BiosLessThan1MB         = (UINT32) MemoryAddress;
  EfiToLegacy16InitTable->LowPmmMemory            = (UINT32) MemoryAddress;
  EfiToLegacy16InitTable->LowPmmMemorySizeInBytes = MemorySize;

  MemorySize = PcdGet32 (PcdHighPmmMemorySize);
  ASSERT ((MemorySize & 0xFFF) == 0);
  //
  // Allocate high PMM Memory under 16 MB
  //
  Status = AllocateLegacyMemory (
             AllocateMaxAddress,
             0x1000000,
             EFI_SIZE_TO_PAGES (MemorySize),
             &MemoryAddress
             );
  if (EFI_ERROR (Status)) {
    //
    // If it fails, allocate high PMM Memory under 4GB
    //   
    Status = AllocateLegacyMemory (
               AllocateMaxAddress,
               0xFFFFFFFF,
               EFI_SIZE_TO_PAGES (MemorySize),
               &MemoryAddress
               );    
  }
  if (!EFI_ERROR (Status)) {
    EfiToLegacy16InitTable->HiPmmMemory            = (UINT32) (EFI_PHYSICAL_ADDRESS) (UINTN) MemoryAddress;
    EfiToLegacy16InitTable->HiPmmMemorySizeInBytes = MemorySize;
  }

  //
  // Allocate 2 MB or 4MB of Memory under 16 MB for Legacy OPROMs that
  // don't use PMM but look for zeroed memory. Note some old add-in cards.
  //
  MemorySize = PcdGet32 (PcdNonPmmMemorySize);
  ASSERT ((MemorySize & 0xFFF) == 0);
  if(MemorySize != 0 && MemorySize <= 0x400000){
    Status = AllocateLegacyMemory (
               AllocateMaxAddress,
               0x1000000,
               EFI_SIZE_TO_PAGES (MemorySize),
               &MemoryAddress
               );
    ASSERT_EFI_ERROR (Status);

    if(!EFI_ERROR(Status)) {
      ZeroMem((VOID *) ((UINTN) MemoryAddress), MemorySize);
    }
  }
  //
  // Allocate(reserve) 0x2000 for CSM16 DISP.ROM at 0xE000 - 0x10000
  //
  Status = AllocateLegacyMemory (
             AllocateAddress,
             0xE000,
             2,
             &MemoryAddress
             );

  //
  // Initial INT0 to INTFFh vectors before Legacy16InitializeYourself
  // Prevent some HWs send unknown IRQ after the CPU switchs 16bit Real Mode.
  //
  CopyMem (
    Private->IntThunk->DummyInterruptCode,
    (VOID *) (UINTN) DummyInterruptTemplate,
    sizeof (Private->IntThunk->DummyInterruptCode)
    );

  for (Index = 0; Index < 0x100; Index++) {
    IdtArray = (UINT32 *) (UINTN) (Index * 4);    
    *IdtArray = (EFI_SEGMENT ((UINTN) Private->IntThunk->DummyInterruptCode) << 16) | EFI_OFFSET ((UINTN) Private->IntThunk->DummyInterruptCode);
  }
  //
  //  ShutdownAPs();
  //
  // Start the Legacy BIOS;
  //
  Status = ShadowAndStartLegacy16 (Private);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Add code to "find" the $DISPROM and reserve the space it uses
  //
  // Allocate space needed for Insyde's $DISPROM segment.  This area will be
  // identified with the sigature "$DISPROM", followed by the length of the
  // segment (a 16-bit length).  This segment, if present, will always start
  // in the 1st 64K segment of the system and will start on a paragraph addr.
  // Note, this code "must" come after the call to ShadowAndStartLegacy16()
  // because the $DISPROM code is not present in memory until then...
  //
  for (MemStart = 0x400; MemStart < 0x10000; MemStart += 0x10) {
    if (!CompareMem(PHYSICAL_ADDRESS_TO_POINTER(MemStart), "$DISPROM", 8)) { // Found it!

      MemLength = (*((UINTN *)PHYSICAL_ADDRESS_TO_POINTER(MemStart+8)));

      DEBUG ((EFI_D_ERROR, "$DISPROM found at address = %lx, length = %x \n",
              MemStart,
              MemLength
              ));

      Status = AllocateLegacyMemory (
                 AllocateAddress,
                 (MemStart & ~(EFI_PAGE_SIZE - 1)),
                 (MemLength + EFI_PAGE_SIZE - 1)/ EFI_PAGE_SIZE,
                 &MemoryAddress
                 );

      DEBUG ((EFI_D_ERROR, "$DISPROM memory allocation of %d pages at address %x, Status = %x \n",
              (MemLength + EFI_PAGE_SIZE - 1)/ EFI_PAGE_SIZE,
              (MemStart & ~(EFI_PAGE_SIZE - 1)),
              Status
              ));

      break;
    }
  }

  Private->LegacyRegion->UnLock (Private->LegacyRegion, 0xE0000, 0x20000, NULL);
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.BX = (UINT16) 0;
  Regs.X.CX = (UINT16) sizeof (Private->IntThunk->InterruptRedirectionCode);
  Regs.X.DX = 0xfff0;
  Status = LegacyBiosFarCall86 (
             &Private->LegacyBios,
             Private->Legacy16CallSegment,
             Private->Legacy16CallOffset,
             &Regs,
             NULL,
             0
             );

  InterruptRedirectionCodePtr  = (VOID *) (UINTN) ((Regs.X.DS << 4) + Regs.X.BX);

  ZeroMem (InterruptRedirectionCodePtr, sizeof (Private->IntThunk->InterruptRedirectionCode));

  //
  // Initialize interrupt redirection code and entries;
  // IDT Vectors 0x68-0x6f must be redirected to IDT Vectors 0x08-0x0f.
  //
  CopyMem (
    (VOID *) (UINTN) InterruptRedirectionCodePtr,
    (VOID *) (UINTN) InterruptRedirectionTemplate,
    sizeof (Private->IntThunk->InterruptRedirectionCode)
    );
  Private->LegacyRegion->Lock (Private->LegacyRegion, 0xE0000, 0x20000, NULL);

  //
  // Save Unexpected interrupt vector so can restore it just prior to boot
  //
  BaseVectorMaster = (UINT32 *) (sizeof (UINT32) * PROTECTED_MODE_BASE_VECTOR_MASTER);
  Private->BiosUnexpectedInt = BaseVectorMaster[0];
  IntRedirCode = (UINT32) (UINTN) InterruptRedirectionCodePtr;
  for (Index = 0; Index < 8; Index++) {
    BaseVectorMaster[Index] = (EFI_SEGMENT (IntRedirCode + Index * 4) << 16) | EFI_OFFSET (IntRedirCode + Index * 4);
  }
  //
  // Save EFI value
  //
  Private->ThunkSeg = (UINT16) (EFI_SEGMENT (IntRedirCode));

  //
  // Make a new handle and install the protocol
  //
  Private->Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Private->Handle,
                  &gEfiLegacyBiosProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &Private->LegacyBios
                  );
  Private->Csm16PciInterfaceVersion = GetPciInterfaceVersion (Private);

  DEBUG ((EFI_D_INFO, "CSM16 PCI BIOS Interface Version: %02x.%02x\n",
          (UINT8) (Private->Csm16PciInterfaceVersion >> 8),
          (UINT8) Private->Csm16PciInterfaceVersion
        ));
  ASSERT (Private->Csm16PciInterfaceVersion != 0);
  return Status;
}

/**
  This function is invoked when gLegacyBiosDependencyGuid is installed

  @param                        NONE
  @retval                       NONE

**/
VOID
EFIAPI
LegacyBiosInstallNotifyFunction (
  IN EFI_EVENT            Event,
  IN VOID                 *Context
  )
{
  EFI_STATUS                Status;

  gBS->CloseEvent (Event);

  Status = LegacyBiosInstallInstance (mImageHandle, mSystemTable);

  ASSERT_EFI_ERROR (Status);
}

/**
  Install Driver to produce Legacy BIOS protocol.

  @param  ImageHandle  Handle of driver image.
  @param  SystemTable  Pointer to system table.

  @retval EFI_SUCCESS  Legacy BIOS protocol installed
  @retval No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
LegacyBiosInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                         Status;
  LEGACY_BIOS_INSTANCE               *Private;
  EFI_PHYSICAL_ADDRESS               MemoryAddress;
  EFI_PHYSICAL_ADDRESS               StartAddress;
  UINT32                             *ClearPtr;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR    Descriptor;
  UINT64                             Length;
  EFI_EVENT                          LegacyBiosPlatformEvent;
  VOID                               *Registration;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL  *LegacyBiosPlatform;
  BDA_STRUC                          *Bda;
  VOID                               *LegacyFree;

  POST_CODE (DXE_BEFORE_CSM16_INIT); //PostCode = 0x59, Legacy BIOS initial
  //
  // Load this driver's image to memory
  //
  Status = RelocateImageUnder4GIfNeeded (ImageHandle, SystemTable);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Private = &mPrivateData;
  ZeroMem (Private, sizeof (LEGACY_BIOS_INSTANCE));
  mImageHandle = ImageHandle;
  mSystemTable = SystemTable;

  //
  // Locate Memory Test Protocol if exists
  //
  Status = gBS->LocateProtocol (
                  &gEfiGenericMemTestProtocolGuid,
                  NULL,
                  (VOID **) &Private->GenericMemoryTest
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Make sure all memory from 0-640K is tested
  //
  for (StartAddress = 0; StartAddress < 0xa0000; ) {
    gDS->GetMemorySpaceDescriptor (StartAddress, &Descriptor);
    if (Descriptor.GcdMemoryType != EfiGcdMemoryTypeReserved) {
      StartAddress = Descriptor.BaseAddress + Descriptor.Length;
      continue;
    }
    Length = MIN (Descriptor.Length, 0xa0000 - StartAddress);
    Private->GenericMemoryTest->CompatibleRangeTest (
                                  Private->GenericMemoryTest,
                                  StartAddress,
                                  Length
                                  );
    StartAddress = StartAddress + Length;
  }

  Status = gBS->LocateProtocol (&gLegacyFreeGuid, NULL, (VOID **)&LegacyFree);
  if (!EFI_ERROR (Status)) {
    //
    // Allocate 0 - 4K for real mode interupt vectors and BDA.
    //
    Status = AllocateLegacyMemory (
               AllocateAddress,
               0,
               1,
               &MemoryAddress
               );
    if (!EFI_ERROR (Status)) {
      ClearPtr = (VOID *) ((UINTN) 0x0000);

      //
      // Initialize region from 0x0000 to 4k. This initializes interrupt vector
      // range.
      //
      gBS->SetMem ((VOID *) ClearPtr, 0x400, INITIAL_VALUE_BELOW_1K);
      ZeroMem ((VOID *) ((UINTN)ClearPtr + 0x400), 0xC00);
    }
    //
    // Initial BDA
    //
    Bda   = (BDA_STRUC *) ((UINTN) 0x400);
    //
    // Initial EBDA
    //
    mEbda = CONVENTIONAL_MEMORY_TOP;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiRuntimeServicesData,
                    24,
                    &mEbda
                    );
    ASSERT_EFI_ERROR (Status);

    Bda->Ebda  = 0x9fc0;

    ZeroMem ((VOID *)(UINTN)mEbda, 0x18000);

    //
    // Set size of the EBDA
    //
    EBDA(0) = 1;

    //
    // 640k-1k for EBDA
    //
    Bda->MemSize        = 0x27f;
    Bda->KeyHead        = 0x1e;
    Bda->KeyTail        = 0x1e;
    Bda->FloppyData     = 0x00;
    Bda->FloppyTimeout  = 0xff;

    Bda->KeyStart       = 0x001E;
    Bda->KeyEnd         = 0x003E;
    Bda->KeyboardStatus = 0x10;


    //
    // Move LPT time out here and zero out LPT4 since some SCSI OPROMS
    // use this as scratch pad (LPT4 is Reserved)
    //
    Bda->Lpt1_2Timeout  = 0x1414;
    Bda->Lpt3_4Timeout  = 0x1400;
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (&gEfiLegacyBiosPlatformProtocolGuid, NULL, (VOID **)&LegacyBiosPlatform);
  if (EFI_ERROR (Status)) {
    Status = gBS->CreateEvent (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    LegacyBiosInstallNotifyFunction,
                    NULL,
                    &LegacyBiosPlatformEvent
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Status = gBS->RegisterProtocolNotify (
                    &gEfiLegacyBiosPlatformProtocolGuid,
                    LegacyBiosPlatformEvent,
                    &Registration
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    return EFI_SUCCESS;
  }

  Status = LegacyBiosInstallInstance (ImageHandle, SystemTable);

  return Status;
}
