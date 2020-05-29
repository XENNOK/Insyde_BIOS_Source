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

#include <OemAcpiPlatform.h>

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER               *Table;
  EFI_ACPI_TABLE_VERSION                    Version;
  UINTN                                     Handle;
} ACPI_TABLE_INFO;

//[-start-120504-IB06710071-add]//
EFI_EVENT  mUpdateOemTableIDEvent;
//[-end-120504-IB06710071-add]//


//[-start-120504-IB06710071-add]//
/**
 Update OEM Table ID information event

 @param [in]   Event            Event instance.
 @param [in]   Context          Event Context.

 @retval EFI_SUCCESS            Update Table ID Success

**/
VOID
EFIAPI
UpdateOemTableIDEventNotify (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                                Status;
  EFI_ACPI_DESCRIPTION_HEADER               *Table;
  INTN                                      Index = 0;
  INTN                                      NumberOfTable;
  UINTN                                     Handle;
  EFI_ACPI_TABLE_VERSION                    Version;
  ACPI_TABLE_INFO                           *AcpiTableInfo;
//[-start-130912-IB12360024-add]//
  UINTN                                     Length;

  Length = 0;
//[-end-130912-IB12360024-add]//
  //
  // 1. Calculate how many ACPI Table we have
  //
  while (1) {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **)&Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }
    Index++;
  }
  NumberOfTable = Index;
  //
  // 2. Allocate pool to store ACPI Table Info
  //
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (ACPI_TABLE_INFO) * NumberOfTable, (VOID **)&AcpiTableInfo);
  if (EFI_ERROR (Status)) {
    return;
  }
  //
  // 3. Get ACPI Table and store table information into AcpiTableInfo structure
  //    And also update table content.
  //
  for (Index = 0; Index < NumberOfTable; Index++) {
    Status = mAcpiSupport->GetAcpiTable (mAcpiSupport, Index, (VOID **)&Table, &Version, &Handle);
    if (Status == EFI_NOT_FOUND) {
      break;
    }
    AcpiTableInfo[Index].Table = Table;
    AcpiTableInfo[Index].Version = Version;
    AcpiTableInfo[Index].Handle = Handle;
    if (Table->Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
      continue;
    }
//[-start-120731-IB10820094-modify]//
    if (FeaturePcdGet(PcdXtuSupported)) { 
      if (Table->Signature == SIGNATURE_32('A', 'S', 'P', 'T')) {
        *(UINT32*)(Table->OemId)      = SIGNATURE_32 ('I', 'N', 'S', 'Y');
        *(UINT16*)(Table->OemId + 4)  = SIGNATURE_16 ('D', 'E');
        Table->OemTableId             = SIGNATURE_64 ('P','e','r','f','T','u','n','e');
        //
        // Creator ID & Revision are used for ASL compiler
        //
        Table->CreatorId              = SIGNATURE_32 ('M','S','F','T');
        Table->CreatorRevision        = 0x00040000;
        continue;
      }
    }
    
//[-start-130912-IB12360024-modify]//
    //
	// Update OemId
	//
    Length = AsciiStrLen ((CHAR8 *) PcdGetPtr (PcdOemId));
    if ((Length > sizeof (Table->OemId))) {
	  Length = sizeof (Table->OemId);
      DEBUG ((EFI_D_INFO, "OemId length is invalid.\n"));
    }
    ZeroMem (&(Table->OemId), sizeof (Table->OemId));
    CopyMem (&(Table->OemId), PcdGetPtr (PcdOemId), Length);
    
	//
	// Update OemTableId
	//
    Length = AsciiStrLen ((CHAR8 *) PcdGetPtr (PcdOemTableId));
    if ((Length > sizeof (Table->OemTableId))) {
      Length = sizeof (Table->OemTableId);
      DEBUG ((EFI_D_INFO, "OemTableId length is invalid.\n"));
    }
    ZeroMem (&(Table->OemTableId), sizeof (Table->OemTableId));
    CopyMem (&(Table->OemTableId), PcdGetPtr (PcdOemTableId), Length);
//[-end-130912-IB12360024-modify]//
    //
    // Creator ID & Revision are used for ASL compiler
    //
    Table->CreatorId              = SIGNATURE_32 ('A', 'C', 'P', 'I');
    Table->CreatorRevision        = 0x00040000;
//[-start-121109-IB10820156-modify]//
//[-start-130524-IB05160451-modify]//
    Status = OemSvcModifyAcpiDescriptionHeader ( Table );
    DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcModifyAcpiDescriptionHeader, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//
//[-end-121109-IB10820156-modify]//
  }
  //
  // 4. All Table content has be updated, Set all ACPI Table
  //
  for (Index = 0; Index < NumberOfTable; Index++) {
    Table = AcpiTableInfo[Index].Table;
    Version = AcpiTableInfo[Index].Version;
    Handle = AcpiTableInfo[Index].Handle;
    if (Table->Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE) {
      continue;
    }
    mAcpiSupport->SetAcpiTable (mAcpiSupport, Table, TRUE, Version, &Handle);
  }

}
//[-end-120504-IB06710071-add]//


//[-start-120504-IB06710071-modify]//
/**
 Update OEM Table ID information for all ACPI TAbles

 @param None

 @retval EFI_SUCCESS            Update Table ID Success

**/
EFI_STATUS
OemUpdateOemTableID (
  VOID
  )
{
  EFI_STATUS  Status;
  
  Status = EfiCreateEventReadyToBootEx (
                  TPL_CALLBACK-1,
                  UpdateOemTableIDEventNotify,
                  NULL,
                  &mUpdateOemTableIDEvent
                  );
  ASSERT_EFI_ERROR (Status);
  return EFI_SUCCESS;
}
//[-end-120504-IB06710071-modify]//

