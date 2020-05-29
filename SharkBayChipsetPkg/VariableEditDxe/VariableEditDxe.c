/** @file
  Content file contains Implementation for Variable Edit Driver

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#include "VariableEditDxe.h"

//
//  Module-defined variable
//
EFI_GUID     mVariableEditIdentifyGuid   = VARIABLE_EDIT_IDENTIFY_GUID;
EFI_EVENT    mReadyToBootEvent           = NULL;

VARIABLE_EDIT_TABLE          *mTable     = NULL;
BOOLEAN                       mIsPage    = FALSE;
UINTN                         mPageNo    = 0;
SETUP_UTILITY_BROWSER_DATA   *mSuBrowser = NULL;
VOID                         *mFEaddress = 0;
EFI_SETUP_UTILITY_PROTOCOL   *mScUtility = NULL;
SETUP_CALLBACK_SYNC_PROTOCOL  mCallbackSync = {0};
VARIABLE_EDIT_PROTOCOL       *mVariableEditor    = NULL;

/**
  Check supervisor or use password is set or not.

  @param  This                 The EFI_SYS_PASSWORD_SERVICE_PROTOCOL instance.
  @param  SupervisorOrUser     determine to disable supervisor or use password

  @retval EFI_SUCCESS          password is set
  @retval EFI_NOT_FOUND        password isn't set

**/
EFI_STATUS
EFIAPI
GetPasswordStatus (
  IN  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *This,
  IN  BOOLEAN                              SupervisorOrUser
  )
{
  DEBUG ( (EFI_D_INFO, "[InitializeVariableEditTable] GetPasswordStatus hook call.\n"));
  return EFI_NOT_FOUND;
}

VOID
EFIAPI
FreeTable (
  VOID
  )
{

  if (NULL == mTable) {
    return;
  }

  if (mIsPage == TRUE) {
    gBS->FreePages ( (EFI_PHYSICAL_ADDRESS) mTable, mPageNo);

  } else {
    gBS->FreePool (mTable);
  }

  return;
}

/**
  Get Package Info.

  @param[in]  TotalSize       the size to filll
  @param[in]  PairCount       the number of pair

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetRequiredSize (
  OUT UINTN          *TotalSize,
  OUT UINTN          *PairCount
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  EFI_SETUP_UTILITY_BROWSER_PROTOCOL   *Interface = NULL;
  UINTN                                 Index = 0;
  UINTN                                 Count = 0;
  UINTN                                 Size = 0;
  EFI_HII_HANDLE                        HiiHandles[MAX_HII_HANDLES] = {0};

  if (NULL == TotalSize || NULL == PairCount) {
    return EFI_INVALID_PARAMETER;
  }
  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityBrowserProtocolGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *TotalSize = sizeof (VARIABLE_EDIT_TABLE);
  mSuBrowser = EFI_SETUP_UTILITY_BROWSER_FROM_THIS (Interface);

  //
  // Determine place the order
  //
  HiiHandles[0] = mSuBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle;
  HiiHandles[1] = mSuBrowser->SUCInfo->MapTable[BootHiiHandle].HiiHandle;
  HiiHandles[2] = mSuBrowser->SUCInfo->MapTable[PowerHiiHandle].HiiHandle;
  HiiHandles[3] = mSuBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle;
  HiiHandles[4] = mSuBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle;
  HiiHandles[5] = mSuBrowser->SUCInfo->MapTable[MainHiiHandle].HiiHandle;

  for (Index = 0x0U; Index < MAX_HII_HANDLES; ++Index) {
    if (HiiHandles[Index] == 0) {
      continue;
    }

    //
    // Get size and Check size is Legal
    //
    Size = 0;
    mSuBrowser->HiiDatabase->ExportPackageLists (
                               mSuBrowser->HiiDatabase,
                               HiiHandles[Index],
                               &Size,
                               NULL
                               );
    if (Size <= sizeof (EFI_HII_PACKAGE_LIST_HEADER)) {
      continue;
    }

    //
    // Calculating the size of the HiiHandle
    //
    Size -= sizeof (EFI_HII_PACKAGE_LIST_HEADER);

    *TotalSize += Size;
    Count += 1;
  }

  //
  // Reserved Space for the VFR_STR_OFFSET_PAIR
  //
  *TotalSize += Count * sizeof (VFR_STR_OFFSET_PAIR);
  *PairCount = Count;

  return EFI_SUCCESS;
}

/**
  Allocate memory to store Variable Edit Table

  @param[in]  PairCount       the number of pair to fill

  @retval EFI_SUCCESS        Allocate success

**/
EFI_STATUS
EFIAPI
AllocateReservedMemory (
  IN  UINTN          NeedSize,
  OUT UINTN         *AllocateSize
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  VOID                 *TempAddress = NULL;
  EFI_PHYSICAL_ADDRESS  PhyAddress = 0;

  if (NULL == AllocateSize) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->AllocatePool (
             EfiReservedMemoryType,
             NeedSize,
             (VOID **)&TempAddress
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Check whether the address is smaller than the 4G
  //
  if ( (UINTN) TempAddress > VARIABLE_EDIT_TABLE_MAX_ADDRESS) {
    Status = gBS->FreePool (TempAddress);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    PhyAddress = VARIABLE_EDIT_TABLE_MAX_ADDRESS;
    Status = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (NeedSize),
                    &PhyAddress
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    mIsPage = TRUE;
    mPageNo = EFI_SIZE_TO_PAGES (NeedSize);
    *AllocateSize = EFI_PAGES_TO_SIZE (EFI_SIZE_TO_PAGES (NeedSize));
    mTable = (VARIABLE_EDIT_TABLE *) (UINTN) PhyAddress;

  } else {
    mIsPage = FALSE;
    *AllocateSize = NeedSize;
    mTable = (VARIABLE_EDIT_TABLE *) TempAddress;
  }

  return EFI_SUCCESS;
}

/**
  Fill data into Variable Edit Table

  @param[in]  PairCount       the number of pair to fill

  @retval EFI_SUCCESS        Fill success

**/
EFI_STATUS
EFIAPI
FillAllData (
  IN  UINTN          PairCount
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINTN                         Index = 0;
  UINTN                         Size = 0;
  UINTN                         Count = 0;
  VFR_STR_OFFSET_PAIR          *PairPtr = NULL;
  UINT8                        *DataPtr = NULL;
  EFI_HII_HANDLE                HiiHandles[MAX_HII_HANDLES] = {0};
  EFI_HII_PACKAGE_HEADER       *PkgHdr  = NULL;
  EFI_HII_PACKAGE_LIST_HEADER  *PackageList  = NULL;

  PairPtr = (VFR_STR_OFFSET_PAIR *) (mTable + 1);
  DataPtr = (UINT8 *) (PairPtr + PairCount);

  //
  // Determine place the order
  //
  HiiHandles[0] = mSuBrowser->SUCInfo->MapTable[ExitHiiHandle].HiiHandle;
  HiiHandles[1] = mSuBrowser->SUCInfo->MapTable[BootHiiHandle].HiiHandle;
  HiiHandles[2] = mSuBrowser->SUCInfo->MapTable[PowerHiiHandle].HiiHandle;
  HiiHandles[3] = mSuBrowser->SUCInfo->MapTable[SecurityHiiHandle].HiiHandle;
  HiiHandles[4] = mSuBrowser->SUCInfo->MapTable[AdvanceHiiHandle].HiiHandle;
  HiiHandles[5] = mSuBrowser->SUCInfo->MapTable[MainHiiHandle].HiiHandle;

  for (Index = 0x0U; Index < MAX_HII_HANDLES; ++Index) {
    if (HiiHandles[Index] == 0) {
      continue;
    }

    //
    // Get size and Check size is Legal
    //
    Size = 0;
    mSuBrowser->HiiDatabase->ExportPackageLists (
                               mSuBrowser->HiiDatabase,
                               HiiHandles[Index],
                               &Size,
                               NULL
                               );
    if (Size <= sizeof (EFI_HII_PACKAGE_LIST_HEADER)) {
      continue;
    }

    //
    // Copy Data to VARIABLE_EDIT_TABLE
    //
    PackageList = (EFI_HII_PACKAGE_LIST_HEADER *) AllocatePool (Size);
    Status = mSuBrowser->HiiDatabase->ExportPackageLists (
                                        mSuBrowser->HiiDatabase,
                                        HiiHandles[Index],
                                        &Size,
                                        PackageList
                                        );
    if (EFI_ERROR (Status)) {
      if (PackageList != NULL) {
        gBS->FreePool (PackageList);
      }

      return Status;
    }
    Size -= sizeof (EFI_HII_PACKAGE_LIST_HEADER);
    CopyMem (DataPtr, PackageList + 1, Size);
    if (PackageList != NULL) {
      gBS->FreePool (PackageList);
    }

    PkgHdr = (EFI_HII_PACKAGE_HEADER *) DataPtr;
    PairPtr[Count].VfrBinOffset = (UINT32) ( (UINTN) DataPtr - (UINTN) mTable);
    PairPtr[Count].StrPkgOffset = (UINT32) (PairPtr[Count].VfrBinOffset + PkgHdr->Length);

    DataPtr += Size;
    Count += 1;
  }

  mTable->Signature  = VARIABLE_EDIT_TABLE_SIGNATURE;
  mTable->SmiNumber  = mCallbackSync.CallbackSmiNumber;
  mTable->BootTypeOrderSize     =       FIELD_SIZE (CHIPSET_CONFIGURATION, BootTypeOrder);
  mTable->BootTypeOrderOffset   = EFI_FIELD_OFFSET (CHIPSET_CONFIGURATION, BootTypeOrder);
  mTable->PairCount  = (UINT32) PairCount;

  return EFI_SUCCESS;
}

/**
  Find a suitable memory space to store Variable Edit Ptr
  find the E/F segment first. If there is no memory space, allocate it in high memory then store
  the address in the Variable - Varedit

  @param[in]  AllocateSize       the Variable Edit table size
  @param[in]  FEAddress         the address to store the signature

  @retval TODO

**/
EFI_STATUS
EFIAPI
GetFEsegmentTableAddress (
  IN  UINTN         AllocateSize,
  OUT VOID        **FEAddress
  )
{
  EFI_STATUS                   Status;
  LEGACY_BIOS_INSTANCE        *BiosPrivate   = NULL;
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios    = NULL;
  EFI_LEGACY_REGION2_PROTOCOL *LegacyRegion  = NULL;
  EFI_IA32_REGISTER_SET        Regs = {0};
  VARIABLE_EDIT_PTR           *Point = NULL;

  if (NULL == FEAddress) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // LocateProtocol : LegacyBios
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  if (EFI_ERROR (Status)) {
    //
    //  Pure EFI Boot. Allocate high memory to store the signature, and store the address in the variable
    //
    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (VARIABLE_EDIT_PTR),
                    (VOID **)&Point
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Point->Signature  = VARIABLE_EDIT_PTR_SIGNATURE;
    Point->Address    = (UINT32) (UINTN) mTable;
    Point->Size       = (UINT32) AllocateSize;

    *FEAddress = (VOID *) Point;

    DEBUG ( (EFI_D_INFO, "GetFEsegmentTableAddress:  Table          at 0x%x\n", *FEAddress));

    Status = gRT->SetVariable (
                   L"VarEdit",
                   &gVarableEditGuid,
                   EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                   sizeof (VARIABLE_EDIT_PTR),
                   (VOID *)Point
                   );

    if (EFI_ERROR (Status)) {
      return Status;
    }
    return EFI_SUCCESS;
  }

  //
  // LocateProtocol : LegacyRegion
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyRegion2ProtocolGuid,
                  NULL,
                  (VOID **)&LegacyRegion
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Get FE segment Table Address
  //
  LegacyRegion->UnLock (LegacyRegion, 0xE0000, 0x20000, NULL);

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16GetTableAddress;
  Regs.X.BX = F0000Region;
  Regs.X.CX = (UINT16) sizeof (VARIABLE_EDIT_PTR);
  Regs.X.DX = VARIABLE_EDIT_PTR_ALIGNMENT;

  BiosPrivate = LEGACY_BIOS_INSTANCE_FROM_THIS (LegacyBios);
  Status = LegacyBios->FarCall86 (
                         LegacyBios,
                         BiosPrivate->Legacy16CallSegment,
                         BiosPrivate->Legacy16CallOffset,
                         &Regs,
                         NULL,
                         0
                         );

  if (!EFI_ERROR (Regs.X.AX)) {
    Point = (VARIABLE_EDIT_PTR *) (UINTN) (Regs.X.DS * 16 + Regs.X.BX);
    Point->Signature  = VARIABLE_EDIT_PTR_SIGNATURE;
    Point->Address    = (UINT32) (UINTN) mTable;
    Point->Size       = (UINT32) AllocateSize;
  }

  LegacyRegion->Lock (LegacyRegion, 0xE0000, 0x20000, NULL);

  if (EFI_ERROR (Regs.X.AX)) {
    return Regs.X.AX;
  }

  //
  //  Return value
  //
  *FEAddress = (VOID *) Point;

  return EFI_SUCCESS;
}

/**
  Print the Variable Edit ptr and table

  @param[in]  the variable edit ptr address which store in E/F segment or variabel (VarEdit)

**/
VOID
EFIAPI
PrintDebug (
  IN  VOID        *FEAddress
  )
{
  VARIABLE_EDIT_PTR         *Point  = NULL;
  VARIABLE_EDIT_TABLE       *Table  = NULL;
  EFI_HII_PACKAGE_HEADER    *PkgHeader = NULL;
  UINTN                      Index  = 0;
  VFR_STR_OFFSET_PAIR       *PairPtr = NULL;

  if (NULL == FEAddress) {
    return;
  }

  Point = (VARIABLE_EDIT_PTR *) FEAddress;
  if (!IS_VARIABLE_EDIT_PTR (Point)) {
    return;
  }
  DEBUG ( (EFI_D_INFO, "  Point          at 0x%x\n", Point));
  DEBUG ( (EFI_D_INFO, "  Point->Address is 0x%x\n", Point->Address));
  DEBUG ( (EFI_D_INFO, "  Point->Size    is 0x%x\n", Point->Size));

  Table = (VARIABLE_EDIT_TABLE *) (UINTN) Point->Address;
  if (!IS_VARIABLE_EDIT_TABLE (Table)) {
    return;
  }
  DEBUG ( (EFI_D_INFO, "  Table          at 0x%x\n", Table));

  PairPtr = (VFR_STR_OFFSET_PAIR *) (Table + 1);
  for (Index = 0; Index < Table->PairCount; ++Index) {
    PkgHeader = (EFI_HII_PACKAGE_HEADER *) (PairPtr[Index].VfrBinOffset + (UINTN) Table);
    DEBUG ( (EFI_D_INFO, "    Pair[Vfr] at 0x%x -> Size: 0x%x\n", PkgHeader, PkgHeader->Length));

    PkgHeader = (EFI_HII_PACKAGE_HEADER *) (PairPtr[Index].StrPkgOffset + (UINTN) Table);
    DEBUG ( (EFI_D_INFO, "    Pair[Str] at 0x%x -> Size: 0x%x\n", PkgHeader, PkgHeader->Length));
  }
  return;
}

/**
  TODO

  @param[in]  Event       TODO
  @param[in]  Context    TODO

  @retval TODO

**/
VOID
EFIAPI
InitializeVariableEditTable (
  IN  EFI_EVENT   Event,
  IN  VOID       *Context
  )
{

  EFI_STATUS          Status    = EFI_SUCCESS;
  UINTN               TotalSize = 0;
  UINTN               PairCount = 0;
  UINTN               AllocateSize = 0;

  EFI_SYS_PASSWORD_SERVICE_PROTOCOL     OldSysPasswordInstance = {0};
  EFI_SYS_PASSWORD_SERVICE_PROTOCOL    *OldSysPasswordPoint    = NULL;

  DEBUG ( (EFI_D_INFO, "[InitializeVariableEditTable] %a Start\n", __FUNCTION__));

  //
  // Off events to prevent re-entry into the function
  //
  Status = gBS->CloseEvent (mReadyToBootEvent);
  if (EFI_ERROR (Status)) {
    //
    //  There is no need to free memory, this function Automatically free memory
    //
    return;
  }

   Status = gBS->LocateProtocol (
                  &gVariableEditProtocolGuid,
                  NULL,
                  (VOID **)&mVariableEditor
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&mScUtility
                  );
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  //  Disable System password check temporarily.
  //
  Status = gBS->LocateProtocol (
                  &gEfiSysPasswordServiceProtocolGuid,
                  NULL,
                  (VOID **)&OldSysPasswordPoint
                  );
  if (EFI_ERROR (Status)) {
    OldSysPasswordPoint = NULL;
  } else {
    CopyMem (&OldSysPasswordInstance, OldSysPasswordPoint, sizeof (EFI_SYS_PASSWORD_SERVICE_PROTOCOL));
    OldSysPasswordPoint->GetStatus = GetPasswordStatus;
  }

  Status = mVariableEditor->InstallSetupHii (mScUtility, TRUE);
  if (OldSysPasswordPoint != NULL) {
    //
    //  Enable System password check.
    //
    OldSysPasswordPoint->GetStatus = OldSysPasswordInstance.GetStatus;
  }
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  //  Get Package Info. Data
  //
  Status = GetRequiredSize (&TotalSize, &PairCount);
  if (EFI_ERROR (Status)) {
    //
    //  There is no need to free memory, this function Automatically free memory
    //
    return;
  }

  //
  //  Allocate Reserved Memory
  //
  Status = AllocateReservedMemory (TotalSize, &AllocateSize);
  if (EFI_ERROR (Status)) {
    //
    //  There is no need to free memory, this function Automatically free memory
    //
    return;
  }

  //
  //  Copy String, VfrBin
  //
  Status = FillAllData (PairCount);
  if (EFI_ERROR (Status)) {
    //
    //  It should free memory
    //
    FreeTable();
    return;
  }

  Status = mVariableEditor->InstallSetupHii (mScUtility, FALSE);
  if (EFI_ERROR (Status)) {
    FreeTable();
    return;
  }

  //
  //  Fill FE segment
  //
  Status = GetFEsegmentTableAddress (AllocateSize, &mFEaddress);
  if (EFI_ERROR (Status)) {
    //
    //  It should free memory
    //
    FreeTable();
    return;
  }
  DEBUG ( (EFI_D_INFO, "[InitializeVariableEditTable] %a End\n", __FUNCTION__));

  //
  // Print Information
  //
  DEBUG ( (EFI_D_INFO, "Variable Edit PTR   at 0x%x\n", mFEaddress));
  DEBUG ( (EFI_D_INFO, "Variable Edit TABLE at 0x%x\n", mTable));
  PrintDebug (mFEaddress);

  return;
}


/**
  Unload function of Content driver

  Disconnect the driver specified by ImageHandle from all the devices in the handle database.
  Uninstall all the protocols installed in the driver entry point.

  @param[in]  ImageHandle       The drivers' driver image.

  @retval EFI_SUCCESS           The image is unloaded.
  @retval Others                Failed to unload the image.

**/
EFI_STATUS
EFIAPI
VariableEditDriverUnload (
  IN  EFI_HANDLE  ImageHandle
  )
{
  EFI_STATUS           Status = EFI_SUCCESS;
  VARIABLE_EDIT_PTR   *Point  = NULL;

  FreeTable();
  Point = (VARIABLE_EDIT_PTR *) mFEaddress;
  Point->Signature  = 0;
  Point->Address    = 0;
  Point->Size       = 0;
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ImageHandle,
                  &mVariableEditIdentifyGuid,
                  NULL,
                  NULL
                  );

  return Status;
}

/**
  Register ready to boot event to initialize Variable edit table.

 @param [in]   ImageHandle      Pointer to the loaded image protocol for this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval Status                 EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
VariableEditDriverEntry (
  IN      EFI_HANDLE          ImageHandle,
  IN      EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                   Status;
  VOID                        *Interface = NULL;
//[-start-140721-IB08620401-add]//
  VARIABLE_EDIT_FUNCTION_DISABLE_SETTING    VarEditFunDis = {0};
  UINTN                        VarEditFunDisSize = sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING);
//[-end-140721-IB08620401-add]//

  DEBUG ( (EFI_D_INFO, "[VariableEditDriverEntry] %a Start\n", __FUNCTION__));


//[-start-140721-IB08620401-add]//
  Status = gRT->GetVariable (
                  VARIABLE_EDIT_FUNCTION_DISABLE_VARIABLE_NAME,
                  &gVarableEditFunctionDisableGuid,
                  NULL,
                  &VarEditFunDisSize,
                  &VarEditFunDis
                  );

  if ( VarEditFunDisSize != sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING)) {
    DEBUG ( (EFI_D_INFO, "Please check sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING) in Variable Store."));
  }
  if ( (!EFI_ERROR (Status)) &&
       (VarEditFunDis.VariableEditFunDis == TRUE) &&
       (VarEditFunDisSize == sizeof (VARIABLE_EDIT_FUNCTION_DISABLE_SETTING))
     ) {
    return EFI_UNSUPPORTED;
  }
//[-end-140721-IB08620401-add]//

  //
  // Check if the custom-define protocol is installed or not
  //
  Status = gBS->LocateProtocol (
                  &mVariableEditIdentifyGuid,
                  NULL,
                  (VOID **)&Interface
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Create "ReadyToBoot" Event
  //
  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             InitializeVariableEditTable,
             NULL,
             &mReadyToBootEvent
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // Install custom protocol
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &mVariableEditIdentifyGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mCallbackSync.CallbackSmiNumber = 0xFF;
  Status = gBS->InstallProtocolInterface(
                  &ImageHandle,
                  &gSetupCallbackSyncGuid,
                  EFI_NATIVE_INTERFACE,
                  &mCallbackSync
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DEBUG ( (EFI_D_INFO, "[VariableEditDriverEntry] %a End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

