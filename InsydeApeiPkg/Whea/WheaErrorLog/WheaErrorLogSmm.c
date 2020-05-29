/** @file

  Whea Error log driver

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

#include "WheaErrorLogSmm.h"

EFI_WHEA_ERROR_RECORD_INFORMATION      mErrorRecordInfo =  {
  NULL,                               // SmmFwBlock
  (EFI_PHYSICAL_ADDRESS)(UINTN)-1,    // BaseAddress
  0,                                  // ElogLength (excluding FV HeaderLength)
  0,                                  // TotalRecordBytes
  0,                                  // FreeRecordBytes
  (EFI_PHYSICAL_ADDRESS)(UINTN)-1,    // NextRecordAddress
  0,                                  // HeaderLength
  0,                                  // FvBlockAttrubutes
  0,                                  // FvBlockPhysicalAddress
  NULL,                               // ParaRegion
  0                                   // Defragment Policy
};
//
// WHEA Serialization Table
//
STATIC
APEI_WHEA_ERST_SERIALIZATION_INSTRUCTION    mERST[16] = {
  {
    ACTION_BEGIN_WRITE_OPERATION,                 // Serialization Action - 0
    INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    ACTION_BEGIN_WRITE_OPERATION,                 // Value for writing error record
    0x0000FFFF                                    // Mask
  },
  {
    ACTION_BEGIN_READ_OPERATION,                  // Serialization Action - 1
    INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    ACTION_BEGIN_READ_OPERATION,                  // Value for reading error record
    0x0000FFFF                                    // Mask
  },
  {
    ACTION_BEGIN_CLEAR_OPERATION,                 // Serialization Action - 2
    INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    ACTION_BEGIN_CLEAR_OPERATION,                 // Value for clearing error record
    0x0000FFFF                                    // Mask
  },
  {
    ACTION_END_OPERATION,                         // Serialization Action - 3
    INSTRUCTION_NO_OPERATION,                     // Instruction
    APEI_WHEA_ERST_PRESERVE_REGISTER,             // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    ACTION_END_OPERATION,                         // Value
    0x0000FFFF                                    // Mask
  },
  {
    ACTION_SET_RECORD_OFFSET,                     // Serialization Action - 4
    INSTRUCTION_WRITE_REGISTER,                   // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    ACTION_SET_RECORD_OFFSET,                     // Value
    0xFFFFFFFF                                    // Mask
  },
  {
    ACTION_EXECUTE_OPEARTION,                     // Serialization Action - 5
    INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterExecute,                          // GAS (BYTE IO), this is fixed to IO 0xB2 port
    WHEA_ERROR_RECORD_SWSMI,                      // Value, this is the WHEA SMM Handler trigger number
    0x0000FFFF                                    // Mask
  },
  {
    ACTION_CHECK_BUSY_STATUS,                     // Serialization Action - 6
    INSTRUCTION_READ_REGISTER_VALUE,              // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0x00000001,                                   // Value
    0x00000001                                    // Mask
  },
  {
    ACTION_GET_COMMAND_STATUS,                    // Serialization Action - 7
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    0x000001fe                                    // Mask
  },
  {
    ACTION_GET_RECORD_IDENTIFIER,                 // Serialization Action - 8
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    ERST_MASK                                     // Mask
  },
  {
    ACTION_SET_RECORD_IDENTIFIER,                 // Serialization Action - 9
    INSTRUCTION_WRITE_REGISTER,                   // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    ERST_MASK                                     // Mask
  },
  {
    ACTION_GET_RECORD_COUNT,                      // Serialization Action - 10
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    0xFFFFFFFF                                    // Mask
  },
  {
    ACTION_BEGIN_DUMMY_WRITE_OPERATION,           // Serialization Action - 11
    INSTRUCTION_WRITE_REGISTER_VALUE,             // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    ACTION_BEGIN_DUMMY_WRITE_OPERATION,           // Value
    0xFFFF                                        // Mask
  },
  {
    ACTION_GET_NEXT_RECORD_IDENTIFIER,            // Serialization Action - 12
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    0xFFFFFFFF                                    // Mask
  },
  {
    ACTION_GET_ERROR_LOG_ADDRESS_RANGE,           // Serialization Action - 13
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    ERST_MASK                                     // Mask
  },
  {
    ACTION_GET_ERROR_LOG_ADDRESS_RANGE_LENGTH,    // Serialization Action - 14
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    ERST_MASK                                     // Mask
  },
  {
    ACTION_GET_ERROR_LOG_ADDRESS_RANGE_ATTRIBUTES,// Serialization Action - 15
    INSTRUCTION_READ_REGISTER,                    // Instruction
    FLAG_NOTHING,                                 // Flags
    0x00,                                         // Reserved
    ErstRegisterFiller,                           // GAS (QWORD Memory) The actual address will be assigned in InsydeWheaUpdateERST()
    0,                                            // Value
    ERST_MASK                                     // Mask
  }
};

VOID
InsydeWheaMemcpy (
  IN VOID                              *Destination,
  IN VOID                              *Source,
  IN UINTN                             Length
  )
{
  CHAR8                                *Destination8;
  CHAR8                                *Source8;

  Destination8 = Destination;
  Source8 = Source;
  while (Length--) {
    *(Destination8++) = *(Source8++);
  }
}

/**
  Initializes the SMM Handler Driver, update the Mailbox between BIOS and OS.

  @param [in] ImageHandle         The image handle of the DXE Driver, DXE Runtime Driver, 
                                  DXE SMM Driver, or UEFI Driver.
  @param [in] SystemTable         A pointer to the EFI System Table.

  @retval EFI_SUCCESS             Driver initialized successfully.
**/
EFI_STATUS
EFIAPI
InsydeWheaErrorLogSmm (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_SMM_SW_DISPATCH2_PROTOCOL        *SwDispatch;  
  EFI_SMM_SW_REGISTER_CONTEXT          SwContext;  
  EFI_HANDLE                           SwHandle;
  EFI_STATUS                           Status;

 DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  if (InSmm ()) {
    //
    // We're now in SMM
    //
    Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    &SwDispatch
                    );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "[APEI_ERST] ERROR: Locate gEfiSmmSwDispatch2ProtocolGuid Status = %r\n", Status));
      return Status;
    }
    //
    // Initialize and Configure the Persistent Store for WHEA ERROR LOG 
    // If fails Mostly Persistent Store is NOT Available or NOT Responding
    //
    Status = InsydeWheaRomInit ();
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "[APEI_ERST] ERROR: InsydeWheaRomInit failed! Status = %r\n", Status));
      return Status;
    }
    //
    // Update and publish the ERST, create mail box between BIOS and OS.
    //
    Status = InsydeWheaUpdateERST ();
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "[APEI_ERST] ERROR: InsydeWheaUpdateERST failed! Status = %r\n", Status));
      return Status;
    }
    //
    // Install the SW SMI Handler that handle WHEA error records.
    //
    SwContext.SwSmiInputValue = WHEA_ERROR_RECORD_SWSMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           InsydeWheaElogSwSmiCallback,
                           &SwContext,
                           &SwHandle
                           );
    if (EFI_ERROR (Status)) {
      DEBUG((DEBUG_ERROR, "[APEI_ERST] ERROR: Install WHEA_ERROR_RECORD_SWSMI failed! Status = %r\n", Status));
      return Status;
    }
  }  

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}


/**
  Update ERST information and install it. And update mailbox information.

  @param [in] None

  @retval EFI_SUCCESS             All procedures work successfully.
**/
EFI_STATUS
InsydeWheaUpdateERST (
  VOID
  )
{
  EFI_WHEA_SUPPORT_PROTOCOL            *WheaSupport;
  ERROR_RECORD_HEADER                  *ErrRecHdr;  
  EFI_STATUS                           Status;
  UINTN                                ElarSize;
  UINT32                               TotalRecordBytes;
  UINTN                                Count;
  UINTN                                *ElogAddress;
  BOOLEAN                              SetCurRecordID;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  TotalRecordBytes = 0;
  Count            = 0;
  SetCurRecordID   = FALSE;
  //
  // WHEA Support Protocol is a DXE Driver. And this function will be called during DXE phase,
  // So, it's ok to use Boot Service for locating WHEA Support Protocol.
  // 
  Status = gBS->LocateProtocol (&gEfiWheaSupportProtocolGuid, NULL, &WheaSupport);
  if (EFI_ERROR (Status) || (WheaSupport == NULL)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Locate Whea Support Protocol => (%r)\n", Status));
    return Status;
  }
  //
  // WHEA Support Protocol function InsydeWheaGetElar() will return 
  // the address of Parameter Region + Error Record buffer.
  //
  Status = WheaSupport->InsydeWheaGetElar (WheaSupport, &ElarSize, &ElogAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Set up Mailbox (Parameter Region).
  //
  mErrorRecordInfo.ParaRegion = (WHEA_PARAMETER_REGION *)(UINTN)ElogAddress;
  mErrorRecordInfo.ParaRegion->ErrorLogAddressRange      = (UINTN)ElogAddress + sizeof (WHEA_PARAMETER_REGION);
  mErrorRecordInfo.ParaRegion->ErrorLogAddressLength     = ElarSize - sizeof (WHEA_PARAMETER_REGION);
  mErrorRecordInfo.ParaRegion->ErrorLogAddressAttributes = 0x4;
  mErrorRecordInfo.ParaRegion->CurRecordID               = (UINT64)-1;
  //
  // Calculate the Record Count in NV. Then, update it to the Parameter Region.
  //
  ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN) mErrorRecordInfo.BaseAddress;
//[-start-131212-IB10310042-add]//
  if ((ErrRecHdr->Signature != EFI_ERROR_SIGNATURE) && (ErrRecHdr->Signature != 0xFFFFFFFF)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: The first Error Records structure does not following Spec.\n"));
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ErrRecHdr ptr = 0x%08x\n", ErrRecHdr));
    Status = mErrorRecordInfo.SmmFwBlock->EraseBlocks (
                                            mErrorRecordInfo.SmmFwBlock,
                                            (UINTN)mErrorRecordInfo.FvBlockPhysicalAddress,
                                            (UINTN *)&mErrorRecordInfo.ElogLength
                                            );
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: Erase whole APEI region! From 0x%08x, size 0x%08x. Status = %r\n", mErrorRecordInfo.FvBlockPhysicalAddress, mErrorRecordInfo.ElogLength, Status));
    // Update information
    if (!EFI_ERROR(Status)) {
      mErrorRecordInfo.TotalRecordBytes  = 0;
      mErrorRecordInfo.FreeRecordBytes   = mErrorRecordInfo.ElogLength - mErrorRecordInfo.TotalRecordBytes;
      mErrorRecordInfo.NextRecordAddress = mErrorRecordInfo.BaseAddress;
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: TotalRecordBytes  = 0x%08x\n", mErrorRecordInfo.TotalRecordBytes));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: FreeRecordBytes   = 0x%08x\n", mErrorRecordInfo.FreeRecordBytes));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: NextRecordAddress = 0x%08x\n", mErrorRecordInfo.NextRecordAddress));
    }
  }
//[-end-131212-IB10310042-add]//
  while (ErrRecHdr->Signature == EFI_ERROR_SIGNATURE) {
    if (ErrRecHdr->PersistenceInfoAttribute == RECORD_IN_USE) {
      if (!SetCurRecordID) {
        mErrorRecordInfo.ParaRegion->CurRecordID = ErrRecHdr->RecordID;
        SetCurRecordID = TRUE;
      }
      Count++;
    }
    TotalRecordBytes += ErrRecHdr->RecordLength;
    ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)(((EFI_PHYSICAL_ADDRESS) ErrRecHdr) + ErrRecHdr->RecordLength);
//[-start-131212-IB10310042-add]//
    if ((UINTN)ErrRecHdr > ((UINTN)mErrorRecordInfo.BaseAddress + mErrorRecordInfo.ElogLength - 1)) {
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: Over the Bottom of APEI Store. Skip!\n"));
      break;
    }
    if ((ErrRecHdr->Signature != EFI_ERROR_SIGNATURE) && (ErrRecHdr->Signature != 0xFFFFFFFF)) {
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: One of Error Records structure does not following Spec.\n"));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ErrRecHdr ptr = 0x%08x\n", ErrRecHdr));
      Status = mErrorRecordInfo.SmmFwBlock->EraseBlocks (
                                              mErrorRecordInfo.SmmFwBlock,
                                              (UINTN)mErrorRecordInfo.FvBlockPhysicalAddress,
                                              (UINTN *)&mErrorRecordInfo.ElogLength
                                              );
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: Erase whole APEI region! From 0x%08x, size 0x%08x. Status = %r\n", mErrorRecordInfo.FvBlockPhysicalAddress, mErrorRecordInfo.ElogLength, Status));
      // Update information
      if (!EFI_ERROR(Status)) {
        Count            = 0;
        TotalRecordBytes = 0;
        mErrorRecordInfo.ParaRegion->CurRecordID = (UINT64)-1;
      }
    }
//[-end-131212-IB10310042-add]//
  }

  mErrorRecordInfo.ParaRegion->RecordCount = Count;
  mErrorRecordInfo.TotalRecordBytes  = TotalRecordBytes;
  mErrorRecordInfo.NextRecordAddress = mErrorRecordInfo.BaseAddress + TotalRecordBytes;
  mErrorRecordInfo.FreeRecordBytes   = mErrorRecordInfo.ElogLength - mErrorRecordInfo.TotalRecordBytes;

//[-add-131212-IB10310042-add]//
  //
  // Dump Error Log parameter information.
  //
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: BaseAddress                = 0x%016x\n", mErrorRecordInfo.BaseAddress));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ElogLength                 = 0x%08x\n", mErrorRecordInfo.ElogLength));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: TotalRecordBytes           = 0x%08x\n", mErrorRecordInfo.TotalRecordBytes));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: FreeRecordBytes            = 0x%08x\n", mErrorRecordInfo.FreeRecordBytes));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: NextRecordAddress          = 0x%016x\n", mErrorRecordInfo.NextRecordAddress));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: HeaderLength               = 0x%04x\n", mErrorRecordInfo.HeaderLength));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: FvBlockPhysicalAddress     = 0x%016x\n", mErrorRecordInfo.FvBlockPhysicalAddress));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ParaRegion:\n"));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   Command                   = 0x%016x\n", mErrorRecordInfo.ParaRegion->Command));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   LogOffset                 = 0x%016x\n", mErrorRecordInfo.ParaRegion->LogOffset));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   BusyFlag                  = 0x%016x\n", mErrorRecordInfo.ParaRegion->BusyFlag));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   CommandStatus             = 0x%016x\n", mErrorRecordInfo.ParaRegion->CommandStatus));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   CurRecordID               = 0x%016x\n", mErrorRecordInfo.ParaRegion->CurRecordID));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   RecordID                  = 0x%016x\n", mErrorRecordInfo.ParaRegion->RecordID));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   RecordCount               = 0x%016x\n", mErrorRecordInfo.ParaRegion->RecordCount));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   ErrorLogAddressRange      = 0x%016x\n", mErrorRecordInfo.ParaRegion->ErrorLogAddressRange));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   ErrorLogAddressLength     = 0x%016x\n", mErrorRecordInfo.ParaRegion->ErrorLogAddressLength));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO:   ErrorLogAddressAttributes = 0x%016x\n", mErrorRecordInfo.ParaRegion->ErrorLogAddressAttributes));
  DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: DefragmentPolicy            = 0x%08x\n", mErrorRecordInfo.DefragmentPolicy));
//[-end-131212-IB10310042-add]//

  //
  // Assign address for ERST Table Actions.
  //
  mERST[0].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->Command);
  mERST[1].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->Command);
  mERST[2].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->Command);
  mERST[3].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->Command);
  mERST[4].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->LogOffset);
  mERST[6].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->BusyFlag);
  mERST[7].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->CommandStatus);
  mERST[8].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->CurRecordID);
  mERST[9].RegisterRegion.Address  = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->RecordID);
  mERST[10].RegisterRegion.Address = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->RecordCount);
  mERST[11].RegisterRegion.Address = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->Command);
  mERST[12].RegisterRegion.Address = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->Reserved);
  mERST[13].RegisterRegion.Address = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->ErrorLogAddressRange);
  mERST[14].RegisterRegion.Address = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->ErrorLogAddressLength);
  mERST[15].RegisterRegion.Address = (UINTN)&(((WHEA_PARAMETER_REGION *)ElogAddress)->ErrorLogAddressAttributes);

  //
  // Use WHEA Support Protocol to intall ERST table.
  //
  Status = WheaSupport->InsydeWheaInstallErrorRecordMethod (WheaSupport, 16, &mERST);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Update ERST table information => (%r)\n", Status));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  BIOS SMI handler for handling WHEA error records.

  @param [in]      DispatchHandle The handle of this callback, obtained when registering.
  @param [in]      Context        The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer     Common Buffer.
  @param [in, out] CommBufferSize Common Buffer Size.

  @retval EFI_SUCCESS             Error Injection Log successfully.
**/
EFI_STATUS
InsydeWheaElogSwSmiCallback (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *Context,        OPTIONAL
  IN OUT   VOID                        *CommBuffer,     OPTIONAL
  IN OUT   UINTN                       *CommBufferSize  OPTIONAL
  )
{
  UINT64                               WheaStatus;

  WheaStatus = ELOG_PROCESS_SUCCESS;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  //
  // Set the BusyFlag to inform OS that the procedure is now processing.
  //
  mErrorRecordInfo.ParaRegion->BusyFlag = TRUE;
  switch (mErrorRecordInfo.ParaRegion->Command) {

  case ACTION_BEGIN_WRITE_OPERATION:
    DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: ACTION_BEGIN_WRITE_OPERATION\n"));
    //
    // Defragment the ELOG Store to eliminate space occupied by records marked as cleared.
    // The defragment policy is set in SCU.
    //
    InsydeWheaRomDefragment ();
    WheaStatus = InsydeWheaRomWrite((mErrorRecordInfo.ParaRegion->ErrorLogAddressRange + mErrorRecordInfo.ParaRegion->LogOffset));
    break;

  case ACTION_BEGIN_READ_OPERATION:
    DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: ACTION_BEGIN_READ_OPERATION\n"));
    WheaStatus = InsydeWheaRomRead (
                   (EFI_PHYSICAL_ADDRESS)mErrorRecordInfo.ParaRegion->ErrorLogAddressRange + mErrorRecordInfo.ParaRegion->LogOffset,
                   mErrorRecordInfo.ParaRegion->RecordID
                   );
    break;

  case ACTION_BEGIN_CLEAR_OPERATION:
    DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: ACTION_BEGIN_CLEAR_OPERATION\n"));
    WheaStatus = InsydeWheaRomClear(mErrorRecordInfo.ParaRegion->RecordID);
    break;

  case ACTION_BEGIN_DUMMY_WRITE_OPERATION:
    DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: ACTION_BEGIN_DUMMY_WRITE_OPERATION\n"));
    break;

  default:
    break;
  }
  //
  // Set the Command Status to report to OS.
  //
  mErrorRecordInfo.ParaRegion->CommandStatus = WheaStatus;
  //
  // To inform OS that the procedure is complete.
  //  
  mErrorRecordInfo.ParaRegion->BusyFlag = FALSE;  

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  return EFI_UNSUPPORTED;
}

/**
  Update information in EFI_WHEA_ERROR_RECORD_INFORMATION structure for using under OS run time.
  Clear the OEM FV logo in the FV at the first time post.

  @param None

  @retval EFI_SUCCESS             Error Injection Log successfully.
**/
EFI_STATUS
InsydeWheaRomInit (
  VOID
  )
{
  EFI_PHYSICAL_ADDRESS                 PhysicalAddress;
  EFI_STATUS                           Status;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  PhysicalAddress = FixedPcdGet32 (PcdFlashApeiStoreBase);
  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: APEI Store = 0x%08x\n", PhysicalAddress));
  if (PhysicalAddress == 0x00000000) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: APEI Store not found!!\n"));
    return EFI_NOT_FOUND;
  }
  //
  // Update ErrorRecordInfo for OS SMM handler usage.
  //
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    NULL,
                    &mErrorRecordInfo.SmmFwBlock
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Locate gEfiSmmFwBlockServiceProtocolGuid Protocol => %r\n", Status));
    return Status;
  }

  mErrorRecordInfo.FvBlockPhysicalAddress = PhysicalAddress;
  mErrorRecordInfo.ElogLength             = FixedPcdGet32 (PcdFlashApeiStoreSize);
  mErrorRecordInfo.BaseAddress            = PhysicalAddress;
  mErrorRecordInfo.DefragmentPolicy       = PcdGet8 (PcdApeiDefrag);

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}


/**
  Do defragment depending on the SCU setting.
  Allocate buffer for storing the NV header and all the IN USE error records.
  Erase the total OEM NV, and write the buffer content to OEM NV.

  @param [in] None

  @retval EFI_SUCCESS
**/
EFI_STATUS
InsydeWheaRomDefragment (
  VOID
  )
{
  ERROR_RECORD_HEADER                  *ErrRecHdr;
  EFI_STATUS                           Status;
  UINTN                                InUseRecordByte;  
  UINTN                                NvSize;
  UINTN                                WriteSize;
  UINT8                                *NvBuffer;
  UINT64                               RecordId;
  BOOLEAN                              FindNextRecord;
  BOOLEAN                              SetCurRecordID;
//[-start-131212-IB10310042-add]//
  UINT32                               TotalRecordBytes;
  BOOLEAN                              IsDoDefrag;
//[-end-131212-IB10310042-add]//

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  RecordId         = 0;
  InUseRecordByte  = 0;
  WriteSize        = 0;
  FindNextRecord   = FALSE;
  SetCurRecordID   = FALSE;
//[-start-131212-IB10310042-add]//
  TotalRecordBytes = 0;
  IsDoDefrag       = FALSE;
//[-end-131212-IB10310042-add]//

  //
  // Calculate the size of the FV
  //
  NvSize = (UINTN)mErrorRecordInfo.ElogLength;
  //
  // Allocate Temp Buffer  for storing Record data.
  //
  Status = gSmst->SmmAllocatePool (EfiRuntimeServicesData, NvSize, &NvBuffer);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: SmmAllocatePool() => (%r)\n", Status));
    return Status;
  }

  //
  // Start copying the actual ELOG IN USE Records 
  // And recalculate the NV total size, free size, next address.
  //
  ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN) mErrorRecordInfo.BaseAddress;  
//[-start-131212-IB10310042-add]//
  if ((ErrRecHdr->Signature != EFI_ERROR_SIGNATURE) && (ErrRecHdr->Signature != 0xFFFFFFFF)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: The First Error Records structure does not following Spec.\n"));
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ErrRecHdr ptr = 0x%08x\n", ErrRecHdr));
    Status = mErrorRecordInfo.SmmFwBlock->EraseBlocks (
                                            mErrorRecordInfo.SmmFwBlock,
                                            (UINTN)mErrorRecordInfo.FvBlockPhysicalAddress,
                                            &NvSize
                                            );
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: Erase whole APEI region! From 0x%08x, size 0x%08x. Status = %r\n", mErrorRecordInfo.FvBlockPhysicalAddress, NvSize, Status));
    // Update information
    if (!EFI_ERROR(Status)) {
      mErrorRecordInfo.TotalRecordBytes  = 0;
      mErrorRecordInfo.FreeRecordBytes   = mErrorRecordInfo.ElogLength - mErrorRecordInfo.TotalRecordBytes;
      mErrorRecordInfo.NextRecordAddress = mErrorRecordInfo.BaseAddress;
      mErrorRecordInfo.ParaRegion->CurRecordID = (UINT64)-1;
      mErrorRecordInfo.ParaRegion->RecordCount = 0;
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: TotalRecordBytes  = 0x%08x\n", mErrorRecordInfo.TotalRecordBytes));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: FreeRecordBytes   = 0x%08x\n", mErrorRecordInfo.FreeRecordBytes));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: NextRecordAddress = 0x%08x\n", mErrorRecordInfo.NextRecordAddress));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ParaRegion->CurRecordID = 0x%08x\n", mErrorRecordInfo.ParaRegion->CurRecordID));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ParaRegion->RecordCount = 0x%08x\n", mErrorRecordInfo.ParaRegion->RecordCount));
    }
    gSmst->SmmFreePool (NvBuffer);
    return EFI_SUCCESS;
  }
//[-end-131212-IB10310042-add]//

//[-start-131212-IB10310042-remove]//
//  mErrorRecordInfo.TotalRecordBytes = 0;
//[-end-131212-IB10310042-remove]//
//[-start-131212-IB10310042-modify]//
  while (ErrRecHdr->Signature == EFI_ERROR_SIGNATURE) {
    if (ErrRecHdr->PersistenceInfoAttribute == RECORD_IN_USE) {
      if (!SetCurRecordID) {
        mErrorRecordInfo.ParaRegion->CurRecordID = ErrRecHdr->RecordID;
        SetCurRecordID = TRUE;
      }
      InsydeWheaMemcpy (
        (VOID *)(NvBuffer + InUseRecordByte),
        (VOID *)(UINTN)ErrRecHdr,
        ErrRecHdr->RecordLength
        );
      InUseRecordByte += ErrRecHdr->RecordLength;
    } else {
      IsDoDefrag = TRUE;
    }

    TotalRecordBytes += ErrRecHdr->RecordLength;
    ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)(mErrorRecordInfo.BaseAddress + TotalRecordBytes);
    if ((UINTN)ErrRecHdr > ((UINTN)mErrorRecordInfo.BaseAddress + mErrorRecordInfo.ElogLength - 1)) {
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: Over the Bottom of APEI Store. Skip!\n"));
      break;
    }
    if ((ErrRecHdr->Signature != EFI_ERROR_SIGNATURE) && (ErrRecHdr->Signature != 0xFFFFFFFF)) {
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: One of Error Records structure does not following Spec.\n"));
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ErrRecHdr ptr = 0x%08x\n", ErrRecHdr));
      Status = mErrorRecordInfo.SmmFwBlock->EraseBlocks (
                                              mErrorRecordInfo.SmmFwBlock,
                                              (UINTN)mErrorRecordInfo.FvBlockPhysicalAddress,
                                              &NvSize
                                              );
      DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: Erase whole APEI region! From 0x%08x, size 0x%08x. Status = %r\n", mErrorRecordInfo.FvBlockPhysicalAddress, NvSize, Status));
      // Update information
      if (!EFI_ERROR(Status)) {
        mErrorRecordInfo.TotalRecordBytes  = 0;
        mErrorRecordInfo.FreeRecordBytes   = mErrorRecordInfo.ElogLength - mErrorRecordInfo.TotalRecordBytes;
        mErrorRecordInfo.NextRecordAddress = mErrorRecordInfo.BaseAddress;
        mErrorRecordInfo.ParaRegion->CurRecordID = (UINT64)-1;
        mErrorRecordInfo.ParaRegion->RecordCount = 0;
        DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: TotalRecordBytes = 0x%08x\n", mErrorRecordInfo.TotalRecordBytes));
        DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: FreeRecordBytes = 0x%08x\n", mErrorRecordInfo.FreeRecordBytes));
        DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: NextRecordAddress = 0x%08x\n", mErrorRecordInfo.NextRecordAddress));
        DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ParaRegion->CurRecordID = 0x%08x\n", mErrorRecordInfo.ParaRegion->CurRecordID));
        DEBUG ((DEBUG_ERROR, "[APEI_ERST] INFO: ParaRegion->RecordCount = 0x%08x\n", mErrorRecordInfo.ParaRegion->RecordCount));
      }
      gSmst->SmmFreePool (NvBuffer);
      return EFI_SUCCESS;
    }
  } 

  //
  // No any space can be free.
  // Skip defragment.
  //
  if (!IsDoDefrag) {
    gSmst->SmmFreePool (NvBuffer);
    DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));
    return EFI_SUCCESS;
  }
  mErrorRecordInfo.FreeRecordBytes = mErrorRecordInfo.ElogLength - TotalRecordBytes;
//[-end-131212-IB10310042-modify]//

  WriteSize = InUseRecordByte;
  //
  // Do defragment depending on SCU setting.
  //
  switch (mErrorRecordInfo.DefragmentPolicy) {

  case DEFRAG_BELOW_ONE_FOURTH:
    if (mErrorRecordInfo.ElogLength > mErrorRecordInfo.FreeRecordBytes * 4) {
      InsydeWheaRomEraseWrite (&NvSize, &WriteSize, NvBuffer);
    }
    break;

  case DEFRAG_BELOW_ONE_THIRD:
    if (mErrorRecordInfo.ElogLength > mErrorRecordInfo.FreeRecordBytes * 3) {
      InsydeWheaRomEraseWrite (&NvSize, &WriteSize, NvBuffer);
    }
    break;

  case DEFRAG_BELOW_HALF:
    if (mErrorRecordInfo.ElogLength > mErrorRecordInfo.FreeRecordBytes * 2) {
      InsydeWheaRomEraseWrite (&NvSize, &WriteSize, NvBuffer);
    }
    break;

  case DEFRAG_EVERY_ERROR:
    InsydeWheaRomEraseWrite (&NvSize, &WriteSize, NvBuffer);
    break;

  default:
    break;
  }
  
  gSmst->SmmFreePool (NvBuffer);
  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  Erase WHEA Blocks in FV and Write new data from Buffer. 
  And maintain the global variable "ErrorRecordInfo".

  @param [in] EraseSize           Erase size.
  @param [in] WriteSize           Write size.
  @param [in] WriteBuffer         Point of Write buffer.

  @retval None
**/
VOID
InsydeWheaRomEraseWrite (
  IN UINTN    *EraseSize, 
  IN UINTN    *WriteSize,
  IN UINT8    *WriteBuffer
  )
{
  EFI_STATUS                             Status;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

//[-start-131212-IB10310042-add]//
  // Check everytime we write, make sure record is writing inside APEI region only.
  if ((mErrorRecordInfo.FvBlockPhysicalAddress + *WriteSize) > \
    (mErrorRecordInfo.BaseAddress + mErrorRecordInfo.ElogLength)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Out of range! Abort.\n"));
    return;
  }
//[-end-131212-IB10310042-add]//

  Status = mErrorRecordInfo.SmmFwBlock->EraseBlocks (
                                          mErrorRecordInfo.SmmFwBlock,
                                          (UINTN)mErrorRecordInfo.FvBlockPhysicalAddress,
                                          EraseSize
                                          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Erase Flash => (%r)\n", Status));
    return;
  }

  Status = mErrorRecordInfo.SmmFwBlock->Write (
                                mErrorRecordInfo.SmmFwBlock,
                                (UINTN)mErrorRecordInfo.FvBlockPhysicalAddress,
                                WriteSize,
                                WriteBuffer
                                );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Write Flash => (%r)\n", Status));
    return;
  }
  //
  // If defragment is successful, update the ErrorRecordInfo.
  //
  if (!EFI_ERROR(Status)) {
    mErrorRecordInfo.TotalRecordBytes  = (UINT32)(*WriteSize);
    mErrorRecordInfo.FreeRecordBytes   = mErrorRecordInfo.ElogLength - mErrorRecordInfo.TotalRecordBytes;
//[-start-131212-IB10310042-modify]//
    mErrorRecordInfo.NextRecordAddress = mErrorRecordInfo.BaseAddress + mErrorRecordInfo.TotalRecordBytes;
//[-end-131212-IB10310042-modify]//
  }
  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

}
 
/**
  Find a ELOG record using RecordID as Key.
  If the RecordID is 0, out the first record physical address.

  @param [in] RecordID            Identifier of the record that should be searched
  @param [in] PhyAddress          The matched error record address if found
  @param [in] FindNextRecordID    If true, search the entire FV for updating the Current Record ID in Parameter Region.
  @param [in] FirstRecordID       If not NULL, return the first IN USE error record in the NV.

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x04  Record Store Empty
                                  0x05  Record Not Found
**/
UINT64
InsydeWheaRomSearch (
  IN   UINT64                          RecordID,
  OUT  UINTN                           *PhyAddress,
  IN   BOOLEAN                         FindNextRecordID,
  OUT  UINT64                          *FirstRecordID
  )
{
  ERROR_RECORD_HEADER                  *ErrRecHdr;
  UINTN                                Count;
  BOOLEAN                              FindMatchId;
  BOOLEAN                              SetCurRecordID;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  Count          = 0;
  FindMatchId    = FALSE;
  SetCurRecordID = FALSE;

  ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)mErrorRecordInfo.BaseAddress;

  while (ErrRecHdr->Signature == EFI_ERROR_SIGNATURE) {
    if (ErrRecHdr->PersistenceInfoAttribute == RECORD_IN_USE) {
      if (Count == 0 && FirstRecordID != NULL) {
        *FirstRecordID = ErrRecHdr->RecordID; /// For Record NOT Found usage.
      }
      if (ErrRecHdr->RecordID == RecordID || RecordID == 0) {
        *PhyAddress = (UINTN)ErrRecHdr;
        if (!FindNextRecordID) {
          return ELOG_PROCESS_SUCCESS;
        } else {
          FindMatchId = TRUE;
        }
      } else {
        if (FindNextRecordID && !SetCurRecordID) {
          mErrorRecordInfo.ParaRegion->CurRecordID = ErrRecHdr->RecordID;
          SetCurRecordID = TRUE;
        }
      }
      Count ++;
    }
    ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)(((EFI_PHYSICAL_ADDRESS) ErrRecHdr) + ErrRecHdr->RecordLength);
  }

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  if (Count == 0) {
    return ELOG_RECORD_STORE_EMPTY;
  } else {
    if (FindMatchId) {
      return ELOG_PROCESS_SUCCESS;
    } else {
      return ELOG_RECORD_NOT_FOUND;
    }
  }

}

/**
  Read a ELOG record into the buffer provided.
  
  @param [in] RecordBuffer        Buffer where the record should be returned
  @param [in] RecordID            Identifier of the record that should be searched and read to buffer

  @retval Defined by WHEA Specification as below
                    0x00  Success
                    0x04  Record Store Empty
                    0x05  Record Not Found
**/
UINT64
InsydeWheaRomRead (
  IN EFI_PHYSICAL_ADDRESS              RecordBuffer,
  IN UINT64                            RecordID
  )
{
  EFI_PHYSICAL_ADDRESS                 ErrRecAddr;
  ERROR_RECORD_HEADER                  *ErrRecHdr;
  UINT64                               WheaStatus;
  UINT64                               FirstRecordID;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  FirstRecordID = 0;

  WheaStatus = InsydeWheaRomSearch (RecordID, (UINTN *)&ErrRecAddr, TRUE, &FirstRecordID);
  switch (WheaStatus) {

  case ELOG_PROCESS_SUCCESS:
    ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)ErrRecAddr;
    InsydeWheaMemcpy((VOID *)(UINTN)RecordBuffer, (VOID *)(UINTN)ErrRecAddr, ErrRecHdr->RecordLength);
    break;

  case ELOG_RECORD_STORE_EMPTY:
    mErrorRecordInfo.ParaRegion->CurRecordID = (UINT64)-1;
    break;

  case ELOG_RECORD_NOT_FOUND:
    mErrorRecordInfo.ParaRegion->CurRecordID = FirstRecordID;
    break;

  default:
    break;
  }

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() End\n", __FUNCTION__));

  return WheaStatus;
}

/**
  If the record already exists, mark it as cleared.
  Always append the new record in the end.
 
  @param [in] NewRecord           The address of new record which is to be written..

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x01  Not Enough Space
                                  0x02  Hardware Not Available
                                  0x03  Failed
**/
UINT64
InsydeWheaRomWrite(
  IN EFI_PHYSICAL_ADDRESS              NewRecord
  )
{
  ERROR_RECORD_HEADER                  *ErrRecHdr;
  EFI_PHYSICAL_ADDRESS                 ExistErrRecAddr;
  EFI_STATUS                           Status;
  UINT64                               NewRecordID;
  UINTN                                NewRecordLength;
  UINTN                                ByteNumber;
  UINT8                                Clear;
  UINT64                               WheaStatus;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  ByteNumber = 1;
  Clear      = 0;
  ErrRecHdr  = (ERROR_RECORD_HEADER *)(UINTN)NewRecord;
  //
  // Set the new record attribute to "RECORD_IN_USE" first before writting the record into NV.
  //
  ErrRecHdr->PersistenceInfoAttribute = RECORD_IN_USE;
  NewRecordID                         = ErrRecHdr->RecordID;
  NewRecordLength                     = ErrRecHdr->RecordLength;
  //
  // If there is no enough space in NV for storing the new record,
  // return ELOG_NOT_ENOUGH_SPACE.
  //  
  if (NewRecordLength > mErrorRecordInfo.FreeRecordBytes) {
    return ELOG_NOT_ENOUGH_SPACE;
  }

//[-start-131212-IB10310042-add]//
  //
  // UEFI 2.4 Table 234. Error record header.
  // Check Signature "CPER", If not, new record is invalid.
  //
  if (ErrRecHdr->Signature != EFI_ERROR_SIGNATURE) {
    UINT32 Signature;
    Signature = ErrRecHdr->Signature;
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Invalid Record Signature(%a).\n", &Signature));
    return ELOG_FAILED;
  }
  //
  // UEFI 2.4 Table 234. Error record header.
  // Check Signature End [0xFFFFFFFF], If not, new record is invalid.
  //
  if (ErrRecHdr->SignatureEnd != 0xFFFFFFFF) {
    UINT32 SignatureEnd;
    SignatureEnd = ErrRecHdr->SignatureEnd;
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Invalid Record Signature(%x).\n", SignatureEnd));
    return ELOG_FAILED;
  }
  //
  // Check everytime we write, make sure new record length less than size of Error Log Range.
  //
  if (NewRecordLength > mErrorRecordInfo.ParaRegion->ErrorLogAddressLength) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Invalid Record Length.\n"));
    return ELOG_FAILED;
  }
  // Check everytime we write, make sure record is writing inside APEI region only.
  if ((mErrorRecordInfo.NextRecordAddress + NewRecordLength) > \
    (mErrorRecordInfo.BaseAddress + mErrorRecordInfo.ElogLength)) {
    DEBUG ((DEBUG_ERROR, "[APEI_ERST] ERROR: Out of range! Abort.\n"));
    return ELOG_NOT_ENOUGH_SPACE;
  }
//[-end-131212-IB10310042-add]//

  WheaStatus = InsydeWheaRomSearch (NewRecordID, (UINTN *)&ExistErrRecAddr, FALSE, NULL);
  //
  // If the RecordID is exist. Mark the old record attribute as RECORD_CLEAR.
  //
  if (WheaStatus == ELOG_PROCESS_SUCCESS) {
    ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)ExistErrRecAddr;

    Status = mErrorRecordInfo.SmmFwBlock->Write (
                                            mErrorRecordInfo.SmmFwBlock,
                                            (UINTN)&ErrRecHdr->PersistenceInfoAttribute,
                                            &ByteNumber,
                                            &Clear
                                            );
    if (EFI_ERROR(Status)) {
      return ELOG_FAILED;
    }
    mErrorRecordInfo.ParaRegion->RecordCount --;
  }

  //
  // Then append the new record in the end.
  //
  Status = mErrorRecordInfo.SmmFwBlock->Write (
                                          mErrorRecordInfo.SmmFwBlock,
                                          (UINTN)mErrorRecordInfo.NextRecordAddress,
                                          &NewRecordLength,
                                          (UINT8 *)(UINTN)NewRecord
                                          );
  if (EFI_ERROR(Status)) {
    return ELOG_FAILED;
  }
  mErrorRecordInfo.NextRecordAddress += NewRecordLength;
  mErrorRecordInfo.ParaRegion->RecordCount ++;

  if (mErrorRecordInfo.ParaRegion->CurRecordID == (UINT64)-1) {
    mErrorRecordInfo.ParaRegion->CurRecordID = NewRecordID;
  }

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  return ELOG_PROCESS_SUCCESS;
}

/**
  Search for the record with ID. if found, clear the error record.
 
  @param [in] RecordID            The Record which OS want to clear.

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x01  Not Enough Space
                                  0x02  Hardware Not Available
                                  0x03  Failed
                                  0x04  Record Store Empty
                                  0x05  Record Not Found
**/
UINT64
InsydeWheaRomClear(
  IN  UINT64                             RecordID
  )
{
  EFI_PHYSICAL_ADDRESS                 ErrRecAddr;
  ERROR_RECORD_HEADER                  *ErrRecHdr;
  EFI_STATUS                           Status;
  UINTN                                ByteNumber;
  UINT8                                Clear;
  UINT64                               WheaStatus;

  DEBUG ((DEBUG_INFO, "[APEI_ERST] INFO: %a() Start\n", __FUNCTION__));

  ByteNumber = 1;
  Clear      = 0;

  WheaStatus = InsydeWheaRomSearch (RecordID, (UINTN *)&ErrRecAddr, FALSE, NULL);
  if (WheaStatus == ELOG_PROCESS_SUCCESS) {
    ErrRecHdr = (ERROR_RECORD_HEADER *)(UINTN)ErrRecAddr;   
    //
    // If the Record ID is found, mark the Error Record attribute to "clear".
    //
    Status = mErrorRecordInfo.SmmFwBlock->Write (
                                            mErrorRecordInfo.SmmFwBlock,
                                            (UINTN)&ErrRecHdr->PersistenceInfoAttribute,
                                            &ByteNumber,
                                            &Clear
                                            );
    if (EFI_ERROR(Status)) {
      return ELOG_FAILED;
    }
    mErrorRecordInfo.ParaRegion->RecordCount --;
    //
    // If there is no any record in the NV, set Current Record ID to 0xFFFFFFFFFFFFFFFF.
    //
    if (mErrorRecordInfo.ParaRegion->RecordCount == 0) {
      mErrorRecordInfo.ParaRegion->CurRecordID = (UINT64)-1;
    }
    return ELOG_PROCESS_SUCCESS;
  } else {
    //
    // If the Record ID is not found in the NV, just return status to report to OS.
    //
    return WheaStatus;
  }

}

