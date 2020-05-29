/** @file
  Main implementation source file for the Io Trap SMM driver

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2006 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  IoTrap.c

@brief
  Main implementation source file for the Io Trap SMM driver
  
**/
#include <IoTrap.h>

//
// Module global variables
//
EFI_SMM_BASE_PROTOCOL           *mSmmBase;
EFI_SMM_SYSTEM_TABLE            *mSmst;
EFI_HANDLE                      mDriverImageHandle;
EFI_SMM_ICHN_DISPATCH_PROTOCOL  *mIchnDispatch;
EFI_SMM_ICHN_DISPATCH_CONTEXT   mIchnContext;
EFI_HANDLE                      mIchnHandle;
UINT32                          mPchRootComplexBar;

IO_TRAP_INSTANCE                mIoTrapData;
IO_TRAP_RECORD                  *mIoTrapRecord;

static CONST UINT16             mLengthTable[7] = { 4, 8, 16, 32, 64, 128, 256 };

/**
  Register a new IO Trap SMI dispatch function with a parent SMM driver.
  The caller will provide information about the IO trap characteristics via
  the context.  This includes base address, length, read vs. r/w, etc.
  This function will autoallocate IO base address from a common pool if the base address is 0,
  and the RegisterContext Address field will be updated.
  The service will not perform GCD allocation if the base address is non-zero.
  In this case, the caller is responsible for the existence and allocation of the
  specific IO range.
  This function looks for the suitable handler and Register a new IchnIoTrap handler
  if the IO Trap handler is not used. It also enable the IO Trap Range to generate
  SMI.

  @param[in] This                 Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchFunction     Pointer to dispatch function to be invoked for
                                  this SMI source.
  @param[in, out] RegisterContext Pointer to the dispatch function's context.
                                  The caller fills this context in before calling
                                  the register function to indicate to the register
                                  function the IO trap SMI source for which the dispatch
                                  function should be invoked.  This may not be NULL.
  @param[out] DispatchHandle      Handle of dispatch function, for when interfacing
                                  with the parent SMM driver, will be the address of linked
                                  list link in the call back record.  This may not be NULL.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  registered and the SMI source has been enabled.
  @retval EFI_DEVICE_ERROR        The driver was unable to enable the SMI source.
  @retval EFI_OUT_OF_RESOURCES    Insufficient resources are available
  @retval EFI_INVALID_PARAMETER   Address requested is already in use.
**/
static
EFI_STATUS
EFIAPI
IoTrapRegister (
  IN     EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL            *This,
  IN     EFI_SMM_IO_TRAP_DISPATCH_CALLBACK            DispatchFunction,
  IN OUT EFI_SMM_IO_TRAP_DISPATCH_REGISTER_CONTEXT    *RegisterContext,
  OUT EFI_HANDLE                                      *DispatchHandle
  )
{
  EFI_STATUS            Status;
  static CONST UINT16   IoTrapHandlerList[IO_TRAP_HANDLER_NUM] = { IchnIoTrap0, IchnIoTrap1, IchnIoTrap2, IchnIoTrap3 };
  EFI_PHYSICAL_ADDRESS  NextBaseAddress;
  UINT32                NextUsedLength;
  UINT8                 NextTrapHandlerNum;
  EFI_PHYSICAL_ADDRESS  BaseAddress;
  UINT32                UsedLength;
  UINT8                 TrapHandlerNum;
  UINT32                IoTrapRegLowDword;
  UINT32                IoTrapRegHighDword;
  UINT8                 LengthIndex;

  //
  // Return error if the type is invalid
  //
  if (RegisterContext->Type >= IoTrapTypeMaximum) {
    DEBUG ((EFI_D_ERROR, "The Dispatch Type %0X is invalid! \n", RegisterContext->Type));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Return error if the Length is invalid
  //
  if (RegisterContext->Length < 1 || RegisterContext->Length > 0x100) {
    DEBUG ((EFI_D_ERROR, "The Dispatch Length %0X is invalid! \n", RegisterContext->Length));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Return error if the address is invalid
  //
  if (RegisterContext->Address % 4 != 0) {
    DEBUG ((EFI_D_ERROR, "The Dispatch address %0X is invalid! \n", RegisterContext->Address));
    return EFI_INVALID_PARAMETER;
  }
  //
  // Loop through the first IO Trap handler, looking for the suitable handler
  //
  for (TrapHandlerNum = 0; TrapHandlerNum < IO_TRAP_HANDLER_NUM; TrapHandlerNum++) {
    //
    // Get information from Io Trap handler register
    //
    IoTrapRegLowDword = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8);

    //
    // Check if the IO Trap handler is not used
    //
    if ((IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD) == 0) {
      //
      //  Search available IO address and allocate it if the IO address is 0
      //
      BaseAddress = RegisterContext->Address;
      if (BaseAddress == 0) {
        //
        // Allocate 256 byte range from GCD for common pool usage
        //
        Status = gDS->AllocateIoSpace (
                        EfiGcdAllocateAnySearchBottomUp,
                        EfiGcdIoTypeIo,
                        8,
                        0x100,
                        &BaseAddress,
                        mDriverImageHandle,
                        NULL
                        );
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Can't find any available IO address! \n"));
          return EFI_OUT_OF_RESOURCES;
        }

        RegisterContext->Address                    = (UINT16) BaseAddress;
        UsedLength = 0x100;
        mIoTrapData.TrapUsedLength[TrapHandlerNum] = RegisterContext->Length;
      } else {
        //
        // PCH only support dword * power of 2 alignment
        //
        for (LengthIndex = 0; LengthIndex < sizeof (mLengthTable) / sizeof (UINT16); LengthIndex++) {
          if (RegisterContext->Length == mLengthTable[LengthIndex]) {
            break;
          }
        }
        //
        // Return error if the alignment is not dword * power of 2
        //
        if (LengthIndex >= sizeof (mLengthTable) / sizeof (UINT16)) {
          DEBUG ((EFI_D_ERROR, "The PCH only support dword * power of 2 alignment! \n"));
          DEBUG ((EFI_D_ERROR, "The Dispatch Length %0X is 0 or invalid! \n", RegisterContext->Length));
          return EFI_INVALID_PARAMETER;
        }

        UsedLength = RegisterContext->Length;
      }

      //
      // Register a new IchnIoTrap handler
      //
      mIchnContext.Type = IoTrapHandlerList[TrapHandlerNum];
      mIchnHandle       = NULL;
      Status = mIchnDispatch->Register (
                                mIchnDispatch,
                                IoTrapCallback,
                                &mIchnContext,
                                &mIchnHandle
                                );
      ASSERT_EFI_ERROR (Status);
      mIoTrapData.IchnIoTrapHandle[TrapHandlerNum] = mIchnHandle;
      //
      // Fill in the Length, address and Enable the IO Trap SMI
      //
      IoTrapRegLowDword = (UINT32) (((UsedLength - 1) & ~(BIT1 + BIT0)) << 16) |
                          (UINT16) BaseAddress |
                          B_PCH_RCRB_IO_TRAP_TRSE;

      MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8,
        (UINT32) (IoTrapRegLowDword)
        );
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint32,
        (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8),
        1,
        &IoTrapRegLowDword
        );

      IoTrapRegHighDword  = 0x000000F0 | (UINT32) (RegisterContext->Type << N_PCH_RCRB_IO_TRAP_RWIO);
      MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4,
        (UINT32) (IoTrapRegHighDword)
        );
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint32,
        (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4),
        1,
        &IoTrapRegHighDword
        );

      //
      // Set MergeDisable flag of the registered IoTrap
      //
      mIoTrapData.MergeDisable[TrapHandlerNum] = RegisterContext->MergeDisable;
    } else {
      //
      // Check next handler if MergeDisable is TRUE or the registered IoTrap if MergeDisable is TRUE
      //
      if ((RegisterContext->MergeDisable == TRUE) || (mIoTrapData.MergeDisable[TrapHandlerNum] == TRUE)) {
        continue;
      }
      //
      // The IO Trap handler is used, calculate the Length
      //
      UsedLength  = ((IoTrapRegLowDword >> 16) & 0xFC) + 4;
      BaseAddress = IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD;
      //
      //  Assign an addfress from common pool if the caller's address is 0
      //
      if (RegisterContext->Address == 0) {
        //
        //  Check next handler if it's fully used
        //
        if (mIoTrapData.TrapUsedLength[TrapHandlerNum] >= 0x100) {
          continue;
        }
        //
        // Check next handler if it's not for a common pool
        //
        if (UsedLength < 0x100) {
          continue;
        }
        //
        // Check next handler if the size is too big
        //
        if (RegisterContext->Length >= (UINT16) 0x100 - mIoTrapData.TrapUsedLength[TrapHandlerNum]) {
          continue;
        }
        //
        // For common pool, we don't need to change the BaseAddress and UsedLength
        //
        RegisterContext->Address = (UINT16) (BaseAddress + mIoTrapData.TrapUsedLength[TrapHandlerNum]);
        mIoTrapData.TrapUsedLength[TrapHandlerNum] += RegisterContext->Length;
      } else {
        //
        // Check next handler if the address is smaller than the IO trap handler's start address
        //
        if (RegisterContext->Address < (UINT16) BaseAddress) {
          continue;
        }
        //
        // Check next handler if the max address is bigger than IO trap handler's range
        //
        if ((RegisterContext->Address + RegisterContext->Length) > (UINT16) (BaseAddress + 256)) {
          continue;
        }
        //
        // If this handler is used for common pool, assert if the caller's address is within the range
        //
        if (mIoTrapData.TrapUsedLength[TrapHandlerNum] != 0) {
          DEBUG ((EFI_D_ERROR, "The Dispatch address %0x is used for common pool! \n", RegisterContext->Address));
          return EFI_INVALID_PARAMETER;
        }
        //
        // Calculate the Length which is maximum use address - start address
        //
        UsedLength = RegisterContext->Address + RegisterContext->Length - (UINT16) BaseAddress;
        //
        // Check the alignment is dword * power of 2 or not
        //
        for (LengthIndex = 0; LengthIndex < sizeof (mLengthTable) / sizeof (UINT16); LengthIndex++) {
          if (UsedLength == mLengthTable[LengthIndex]) {
            break;
          }
        }
        //
        // Check next handler if the alignment is not dword * power of 2
        //
        if (LengthIndex >= sizeof (mLengthTable) / sizeof (UINT16)) {
          continue;
        }
        //
        // Merge the overlap range: remove next Io Trap handler if next Io Trap handler's range is within this handler's range
        //
        for (NextTrapHandlerNum = TrapHandlerNum + 1; NextTrapHandlerNum != TrapHandlerNum; NextTrapHandlerNum++) {
          //
          // Check if NextTrapHandlerNum overflow
          //
          if (NextTrapHandlerNum >= IO_TRAP_HANDLER_NUM) {
            NextTrapHandlerNum = 0;
          }
          //
          // Get information from Io Trap handler register
          //
          IoTrapRegLowDword = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + NextTrapHandlerNum * 8);
          //
          // Check next handler if the IO Trap handler is not used
          //
          if ((IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD) == 0) {
            continue;
          }
          //
          // Check if next Io Trap handler's range is within this handler's range
          //
          NextUsedLength  = ((IoTrapRegLowDword >> 16) & 0xFC) + 4;
          NextBaseAddress = IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD;
          if ((BaseAddress > NextBaseAddress) || ((BaseAddress + UsedLength) < (NextBaseAddress + NextUsedLength))) {
            continue;
          }
          //
          // Unregister the IO Trap handler
          //
          mIchnHandle       = mIoTrapData.IchnIoTrapHandle[NextTrapHandlerNum];
          mIchnContext.Type = IoTrapHandlerList[NextTrapHandlerNum];
          Status = mIchnDispatch->UnRegister (
                                    mIchnDispatch,
                                    mIchnHandle
                                    );
          ASSERT_EFI_ERROR (Status);

          //
          // Clear the Io Trap handler register
          //
          IoTrapRegLowDword   = 0;
          IoTrapRegHighDword  = 0;
          MmioWrite32 (
            mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + NextTrapHandlerNum * 8 + 4,
            (UINT32) (IoTrapRegHighDword)
            );
          S3BootScriptSaveMemWrite (
            S3BootScriptWidthUint32,
            (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + NextTrapHandlerNum * 8 + 4),
            1,
            &IoTrapRegHighDword
            );

          MmioWrite32 (
            mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + NextTrapHandlerNum * 8,
            (UINT32) (IoTrapRegLowDword)
            );
          S3BootScriptSaveMemWrite (
            S3BootScriptWidthUint32,
            (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + NextTrapHandlerNum * 8),
            1,
            &IoTrapRegLowDword
            );
        }
        //
        // Update the Length
        //
        IoTrapRegLowDword = (UINT32) (((UsedLength - 1) & ~(BIT1 + BIT0)) << 16) |
                            (UINT16) BaseAddress |
                            B_PCH_RCRB_IO_TRAP_TRSE;
        MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8,
          (UINT32) (IoTrapRegLowDword)
          );
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint32,
          (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8),
          1,
          &IoTrapRegLowDword
          );
      }
      //
      // Only set RWM bit when we need both read and write cycles.
      //
      IoTrapRegHighDword = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4);
      if ((IoTrapRegHighDword & B_PCH_RCRB_IO_TRAP_RWM) == 0 &&
          (UINT32) ((IoTrapRegHighDword & B_PCH_RCRB_IO_TRAP_RWIO) >> N_PCH_RCRB_IO_TRAP_RWIO) !=
          (UINT32) RegisterContext->Type) {
        IoTrapRegHighDword = ((IoTrapRegHighDword | B_PCH_RCRB_IO_TRAP_RWM) & ~B_PCH_RCRB_IO_TRAP_RWIO);
        MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4,
          (UINT32) (IoTrapRegHighDword)
          );
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint32,
          (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4),
          1,
          &IoTrapRegHighDword
          );
      }
    }
    break;
  }

  if (TrapHandlerNum >= IO_TRAP_HANDLER_NUM) {
    DEBUG ((EFI_D_ERROR, "All IO Trap handler is used, no available IO Trap handler! \n"));
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Create database record and add to database
  //
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (IO_TRAP_RECORD),
                    (VOID **) &mIoTrapRecord
                    );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for mIoTrapRecord! \n"));
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Gather information about the registration request
  //
  mIoTrapRecord->Signature  = IO_TRAP_RECORD_SIGNATURE;
  mIoTrapRecord->Context    = *RegisterContext;
  mIoTrapRecord->Callback   = DispatchFunction;

  InsertTailList (&mIoTrapData.CallbackDataBase, &mIoTrapRecord->Link);

  //
  // Child's handle will be the address linked list link in the record
  //
  *DispatchHandle = (EFI_HANDLE) (&mIoTrapRecord->Link);

  DEBUG ((EFI_D_INFO, "RegisterContext->Address:%x! \n", RegisterContext->Address));
  DEBUG ((EFI_D_INFO, "RegisterContext->Length:%x! \n", RegisterContext->Length));

  return EFI_SUCCESS;
}

/**
  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of dispatch function to deregister.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  unregistered and the SMI source has been disabled
                                  if there are no other registered child dispatch
                                  functions for this SMI source.
  @retval EFI_INVALID_PARAMETER   Handle is invalid.
**/
EFI_STATUS
EFIAPI
IoTrapUnRegister (
  IN EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL    *This,
  IN EFI_HANDLE                           *DispatchHandle
  )
{
  EFI_STATUS            Status;
  IO_TRAP_RECORD        *RecordToDelete;
  UINT32                IoTrapRegLowDword;
  UINT32                IoTrapRegHighDword;
  EFI_PHYSICAL_ADDRESS  BaseAddress;
  UINT32                UsedLength;
  UINT8                 TrapHandlerNum;
  UINT8                 LengthIndex;

  if (*DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RecordToDelete = IO_TRAP_RECORD_FROM_LINK (*DispatchHandle);

  //
  // Take the entry out of the linked list
  //
  if (RecordToDelete->Link.ForwardLink == (LIST_ENTRY *) (UINTN) EFI_BAD_POINTER) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Loop through the first IO Trap handler, looking for the suitable handler
  //
  for (TrapHandlerNum = 0; TrapHandlerNum < IO_TRAP_HANDLER_NUM; TrapHandlerNum++) {
    //
    // Get information from Io Trap handler register
    //
    IoTrapRegLowDword = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8);

    //
    // Check next Io Trap handler if the IO Trap handler is not used
    //
    if ((IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD) == 0) {
      continue;
    }

    UsedLength  = ((IoTrapRegLowDword >> 16) & 0xFC) + 4;
    BaseAddress = IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD;

    //
    // Check if it's the maximum address of the Io Trap handler
    //
    if (BaseAddress + UsedLength == RecordToDelete->Context.Address + RecordToDelete->Context.Length) {

      if (BaseAddress == RecordToDelete->Context.Address) {
        //
        // Disable the IO Trap handler if it's the only child of the Trap handler
        //
        mIchnHandle       = mIoTrapData.IchnIoTrapHandle[TrapHandlerNum];
        mIchnContext.Type = IchnIoTrap0 + TrapHandlerNum;
        Status = mIchnDispatch->UnRegister (
                                  mIchnDispatch,
                                  mIchnHandle
                                  );
        ASSERT_EFI_ERROR (Status);

        //
        // Clear the Io Trap handler register
        //
        IoTrapRegLowDword   = 0;
        IoTrapRegHighDword  = 0;
        MmioWrite32 (
          mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4,
          (UINT32) (IoTrapRegHighDword)
          );
        S3BootScriptSaveMemWrite (
          S3BootScriptWidthUint32,
          (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8 + 4),
          1,
          &IoTrapRegHighDword
          );

      } else {
        //
        // Calculate the new IO Trap handler Length
        //
        UsedLength = UsedLength - RecordToDelete->Context.Length;
        //
        // Check the alignment is dword * power of 2 or not
        //
        for (LengthIndex = 0; LengthIndex < sizeof (mLengthTable) / sizeof (UINT16); LengthIndex++) {
          if (UsedLength == mLengthTable[LengthIndex]) {
            break;
          }
        }
        //
        // Do not decrease the length if the alignment is not dword * power of 2
        //
        if (LengthIndex >= sizeof (mLengthTable) / sizeof (UINT16)) {
          break;
        }
        //
        // Decrease the length to prevent the IO trap SMI
        //
        IoTrapRegLowDword = (UINT32) ((((UsedLength - 1) &~(BIT1 + BIT0)) << 16) | BaseAddress | B_PCH_RCRB_IO_TRAP_TRSE);
      }

      MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8, (UINT32) (IoTrapRegLowDword));
      S3BootScriptSaveMemWrite (
        S3BootScriptWidthUint32,
        (UINT64) (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8),
        1,
        &IoTrapRegLowDword
        );
      break;
    }
  }

  RemoveEntryList (&RecordToDelete->Link);
  Status = mSmst->SmmFreePool (RecordToDelete);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  This I/O Trap SMI handler invokes the ACPI reference code to handle the SMI.
  It currently assumes it owns all of the IO trap SMI.

  @param[in] DispatchHandle       Not used
  @param[in] DispatchContext      Not used

  @retval None
**/
VOID
EFIAPI
IoTrapCallback (
  IN  EFI_HANDLE                      DispatchHandle,
  IN  EFI_SMM_ICHN_DISPATCH_CONTEXT   *DispatchContext
  )
{
  IO_TRAP_RECORD                            *RecordInDb;
  LIST_ENTRY                                *LinkInDb;
  EFI_SMM_IO_TRAP_DISPATCH_CALLBACK_CONTEXT CurrentIoTrapData;
  UINT16                                    BaseAddress;
  UINT16                                    StartAddress;
  UINT16                                    EndAddress;

  if (!IsListEmpty (&mIoTrapData.CallbackDataBase)) {
    BaseAddress   = MmioRead16 (mPchRootComplexBar + R_PCH_RCRB_TRCR) & B_PCH_RCRB_TRCR_TIOA;
    StartAddress  = (UINT16) ((MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_TRCR) & B_PCH_RCRB_TRCR_AHBE) >> 16);
    //
    // StartAddress and EndAddress will be equal if it's byte access
    //
    EndAddress    = (UINT16) (HighBitSet32 ((UINT32) (StartAddress))) + BaseAddress;
    StartAddress  = (UINT16) (LowBitSet32 ((UINT32) (StartAddress))) + BaseAddress;

    CurrentIoTrapData.Type      = (BOOLEAN) ((MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_TRCR) & B_PCH_RCRB_TRCR_RWI) != 0);
    CurrentIoTrapData.WriteData = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_TRWDR);

    LinkInDb                    = GetFirstNode (&mIoTrapData.CallbackDataBase);

    while (!IsNull (&mIoTrapData.CallbackDataBase, LinkInDb)) {
      RecordInDb = IO_TRAP_RECORD_FROM_LINK (LinkInDb);
      if ((RecordInDb->Context.Address <= StartAddress) &&
          (RecordInDb->Context.Address + RecordInDb->Context.Length > EndAddress)) {
        if (RecordInDb->Context.Type == ReadWriteTrap || RecordInDb->Context.Type == CurrentIoTrapData.Type) {
          //
          // Pass the IO trap context information
          //
          CurrentIoTrapData.Address = StartAddress;
          CurrentIoTrapData.Context = RecordInDb->Context.Context;
          RecordInDb->Callback (&RecordInDb->Link, &CurrentIoTrapData);
          break;
        }
      } else {
        LinkInDb = GetNextNode (&mIoTrapData.CallbackDataBase, &RecordInDb->Link);
        if (IsNull (&mIoTrapData.CallbackDataBase, LinkInDb)) {
          //
          // An IO access was trapped that does not have a handler registered.
          // Since this is an invalid state, we will loop here.
          // It may be appropriate to remove this loop in production systems to avoid potential user issues.
          // But, this indicates an error condition.
          //
          DEBUG_CODE (
            CpuDeadLoop ();
          );
        }
      }
    }
  }
}

/**
  Pause IoTrap callback function.

  This function disables the SMI enable of IoTrap according to the DispatchHandle,
  which is returned by IoTrap callback registration. It only supports the DispatchHandle
  with MergeDisable TRUE and address not zero.

  @param[in] This                 Pointer to the PCH_SMM_IO_TRAP_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of the child service to change state.

  @retval EFI_SUCCESS             This operation is complete.
  @retval EFI_INVALID_PARAMETER   The DispatchHandle is invalid.
  @retval EFI_ACCESS_DENIED       The SMI status is alrady PAUSED.
**/
EFI_STATUS
EFIAPI
IoTrapControlPause (
  IN PCH_SMM_IO_TRAP_CONTROL_PROTOCOL   *This,
  IN EFI_HANDLE                         DispatchHandle
  )
{
  IO_TRAP_RECORD                        *IoTrapRecord;
  UINT32                                IoTrapRegLowDword;
  EFI_PHYSICAL_ADDRESS                  BaseAddress;
  UINT32                                UsedLength;
  UINT8                                 TrapHandlerNum;

  if (DispatchHandle == 0) {
    return EFI_INVALID_PARAMETER;
  }

  IoTrapRecord = IO_TRAP_RECORD_FROM_LINK (DispatchHandle);

  if ((IoTrapRecord->Signature != IO_TRAP_RECORD_SIGNATURE) ||
      (IoTrapRecord->Context.MergeDisable != TRUE)          ||
      (IoTrapRecord->Context.Address == 0)                  ||
      (IoTrapRecord->Context.Length == 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  for (TrapHandlerNum = 0; TrapHandlerNum < IO_TRAP_HANDLER_NUM; TrapHandlerNum++) {
    //
    // This IoTrap register should be merge disabled.
    //
    if (mIoTrapData.MergeDisable[TrapHandlerNum] != TRUE) {
      continue;
    }

    IoTrapRegLowDword = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8);

    UsedLength  = ((IoTrapRegLowDword >> 16) & 0xFC) + 4;
    BaseAddress = IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD;

    //
    // The address and length of record matches the IoTrap register's.
    //
    if ((BaseAddress == IoTrapRecord->Context.Address) &&
        (UsedLength  == IoTrapRecord->Context.Length )) {
      //
      // Check if status matched.
      // If this is already Paused, return warning status.
      //
      if ((IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_TRSE) == 0) {
        return EFI_ACCESS_DENIED;
      }
      //
      // Clear IoTrap register SMI enable bit
      //
      IoTrapRegLowDword &= (~B_PCH_RCRB_IO_TRAP_TRSE);
      MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8, (UINT32) (IoTrapRegLowDword));
      return EFI_SUCCESS;
    }
  }
  return EFI_INVALID_PARAMETER;
}

/**
  Resume IoTrap callback function.

  This function enables the SMI enable of IoTrap according to the DispatchHandle,
  which is returned by IoTrap callback registration. It only supports the DispatchHandle
  with MergeDisable TRUE and address not zero.

  @param[in] This                 Pointer to the PCH_SMM_IO_TRAP_CONTROL_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of the child service to change state.

  @retval EFI_SUCCESS             This operation is complete.
  @retval EFI_INVALID_PARAMETER   The DispatchHandle is invalid.
  @retval EFI_ACCESS_DENIED       The SMI status is alrady RESUMED.
**/
EFI_STATUS
EFIAPI
IoTrapControlResume (
  IN PCH_SMM_IO_TRAP_CONTROL_PROTOCOL   *This,
  IN EFI_HANDLE                         DispatchHandle
  )
{
  IO_TRAP_RECORD                        *IoTrapRecord;
  UINT32                                IoTrapRegLowDword;
  EFI_PHYSICAL_ADDRESS                  BaseAddress;
  UINT32                                UsedLength;
  UINT8                                 TrapHandlerNum;

  if (DispatchHandle == 0) {
    return EFI_INVALID_PARAMETER;
  }

  IoTrapRecord = IO_TRAP_RECORD_FROM_LINK (DispatchHandle);

  if ((IoTrapRecord->Signature != IO_TRAP_RECORD_SIGNATURE) ||
      (IoTrapRecord->Context.MergeDisable != TRUE)          ||
      (IoTrapRecord->Context.Address == 0)                  ||
      (IoTrapRecord->Context.Length == 0))
  {
    return EFI_INVALID_PARAMETER;
  }

  for (TrapHandlerNum = 0; TrapHandlerNum < IO_TRAP_HANDLER_NUM; TrapHandlerNum++) {
    //
    // This IoTrap register should be merge disabled.
    //
    if (mIoTrapData.MergeDisable[TrapHandlerNum] != TRUE) {
      continue;
    }

    IoTrapRegLowDword = MmioRead32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8);

    UsedLength  = ((IoTrapRegLowDword >> 16) & 0xFC) + 4;
    BaseAddress = IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_IOAD;

    //
    // The address and length of record matches the IoTrap register's.
    //
    if ((BaseAddress == IoTrapRecord->Context.Address) &&
        (UsedLength  == IoTrapRecord->Context.Length )) {
      //
      // Check if status matched.
      // If this is already Resume, return warning status.
      //
      if ((IoTrapRegLowDword & B_PCH_RCRB_IO_TRAP_TRSE) != 0) {
        return EFI_ACCESS_DENIED;
      }
      //
      // Set IoTrap register SMI enable bit
      //
      IoTrapRegLowDword |= (B_PCH_RCRB_IO_TRAP_TRSE);
      MmioWrite32 (mPchRootComplexBar + R_PCH_RCRB_IO_TRAP_0 + TrapHandlerNum * 8, (UINT32) (IoTrapRegLowDword));
      return EFI_SUCCESS;
    }
  }
  return EFI_INVALID_PARAMETER;
}

/**
  IO Trap SMM driver entry point function.

  @param[in] ImageHandle          Image handle for this driver image
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             Driver initialization completed successfully
**/
EFI_STATUS
EFIAPI
InstallIoTrap (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS  Status;
  BOOLEAN     InSmm;
  //
  // Initialize the EFI SMM driver library
  //

  //
  // Find the SMM base protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBase);
  ASSERT_EFI_ERROR (Status);

  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    mSmmBase->InSmm (mSmmBase, &InSmm);
  }
  if (!InSmm) {
    return EFI_SUCCESS;
  }

  //
  // Initialize the EFI SMM driver library
  //
  mDriverImageHandle = ImageHandle;

  //
  // Initialize global variables.
  //
  mSmmBase->GetSmstLocation (mSmmBase, &mSmst);

  //
  // Add other initialization code
  //
  //
  // PCH RCBA must be initialized prior to run this driver.
  //
  mPchRootComplexBar = PCH_RCRB_BASE;
  ASSERT (mPchRootComplexBar != 0);

  //
  // Locate the ICHn Dispatch protocol
  //
  Status = gBS->LocateProtocol (&gEfiSmmIchnDispatchProtocolGuid, NULL, (VOID **) &mIchnDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize the IO trap protocol we produce
  //
  mIoTrapData.Signature = IO_TRAP_INSTANCE_SIGNATURE;
  mIoTrapData.EfiSmmIoTrapDispatchProtocol.Register   = IoTrapRegister;
  mIoTrapData.EfiSmmIoTrapDispatchProtocol.UnRegister = IoTrapUnRegister;

  //
  // Initialize the Io trap control protocol.
  //
  mIoTrapData.PchSmmIoTrapControlProtocol.Pause       = IoTrapControlPause;
  mIoTrapData.PchSmmIoTrapControlProtocol.Resume      = IoTrapControlResume;

  //
  // Install protocol interface
  //
  mIoTrapData.Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mIoTrapData.Handle,
                  &gEfiSmmIoTrapDispatchProtocolGuid,
                  &mIoTrapData.EfiSmmIoTrapDispatchProtocol,
                  &gPchSmmIoTrapControlGuid,
                  &mIoTrapData.PchSmmIoTrapControlProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize IO TRAP Callback DataBase
  //
  InitializeListHead (&mIoTrapData.CallbackDataBase);

  return EFI_SUCCESS;
}
