//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#include "SmmFwBlockService.h"
#include "PostCode.h"
#include "CpuIA32.h"
#include "EfiHobLib.h"
#include "BootMode.h"
#include "PeiHob.h"
#include EFI_GUID_DEFINITION (Hob)
#include EFI_GUID_DEFINITION (FdTableInfoHob)

EFI_SMM_SYSTEM_TABLE                    *mSmst;
SMM_FW_BLOCK_SERVICE_INSTANCE           *mSmmFwbGlobal;
UINT8                                   mSmmFlashMode;

EFI_STATUS
GetBootMode (
  EFI_BOOT_MODE                         *BootMode
  )
/*++

Routine Description:

  Get boot mode by looking up configuration table and parsing HOB list

Arguments:

  BootMode - Boot mode from PEI handoff HOB.

Returns:

  EFI_SUCCESS - Successfully get boot mode

  EFI_NOT_FOUND - Can not find the current system boot mode

--*/

{
  VOID                                  *HobList;
  EFI_PEI_HOB_POINTERS                  TempHob;
  EFI_STATUS                            Status;

  //
  // Get Hob list
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (EFI_ERROR (Status)) {
    *BootMode = 0;
    return EFI_NOT_FOUND;
  }

  TempHob.Raw = HobList;
  if (TempHob.Header->HobType != EFI_HOB_TYPE_HANDOFF) {
    return EFI_NOT_FOUND;
  }

  *BootMode = TempHob.HandoffInformationTable->BootMode;
  return EFI_SUCCESS;
};

//
// Use C_FLAGS to define NO_FLUSH_NVSTORE_CACHE to disable code to flush cache
//

EFI_DRIVER_ENTRY_POINT(SmmFwBlockServiceInit)

#ifndef NO_FLUSH_NVSTORE_CACHE
STATIC
VOID
WBCacheModify (
  IN  UINTN                     FlashAddress,
  IN  UINTN                     Size
  )
/*++

  Routine Description:

    Modify the SPI write address so as to modify the cacheline
    This is used for later EfiWbinvd() to invalid cache which mapped to SPI ROM
    Note: The address might be a virtual address in EFI OS runtime

  Arguments:

    FlashAddress - the memory address mapped to SPI ROM.
    Size              - the memory size modified

  Returns:

    EFI_SUCCESS       Operation success.
    EFI_DEVICE_ERROR  The block device is not functioning correctly and the operation failed.

--*/
{
  volatile UINT8  *ByteData;
  volatile UINT32  *DwordData;

  DwordData = (volatile UINT32*) FlashAddress;
  while (Size >= 4) {
    *DwordData ^= 0xFFFFFFFF;
    Size -= 4;
    DwordData ++;
  }
  ByteData = (volatile UINT8*) DwordData;
  while (Size > 0) {
    *ByteData ^= 0xFF;
    Size -= 1;
    ByteData ++;
  }
}
#endif

VOID
EFIAPI
NonVolatileVariableNotifyFunction (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
/*++

Routine Description:

  Initialize Flash device if flash device hasn't been initialized

Arguments:

  (Standard EFI notify event - EFI_EVENT_NOTIFY)

Returns:

  None

--*/
{
  EFI_STATUS     Status;
  FD_TABLE_HOB_DATA                   *FdTableHobPtr = NULL;
  UINTN                               FdTableHobSize;
  VOID                                *HobList;
  //
  // Get FdTableInfoHob to get the connected Flash device index
  //
  Status = EfiLibGetSystemConfigurationTable (&gEfiHobListGuid, &HobList);
  if (!EFI_ERROR (Status)) {
    //
    // Search for the FdTableInfo GUID HOB
    //
    Status = GetNextGuidHob (
               &HobList,
               &gFdTableInfoHobGuid,
               &FdTableHobPtr,
               &FdTableHobSize
               );
    if (!EFI_ERROR (Status)) {
      //
      // FdTableInfoHob is found.
      // Initialize gCurrentFdIndex for AutoDetectDevice
      //
      if ((FdTableHobPtr->CurrentFdIndex != FD_INDEX_INVALID) && (FdTableHobPtr->CurrentFdId != FD_ID_INVALID)) {
        gCurrentFdIndex = FdTableHobPtr->CurrentFdIndex;
        gCurrentFdId    = FdTableHobPtr->CurrentFdId;
      }
    }
  }
  ASSERT_EFI_ERROR (Status);

  if (mSmmFwbGlobal->DevicePtr == NULL) {
    Status = FlashFdDetectDevice (&mSmmFwbGlobal->SmmFwbService, NULL);
    ASSERT_EFI_ERROR (Status);
  }
  return;
}

EFI_STATUS
FlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  OUT UINT8                             *Buffer
  )
/*++

Routine Description:

  Smart detect current flash device type

Arguments:

Returns:

  EFI status

--*/
{
  EFI_STATUS                            Status;
  EnableFvbWrites (TRUE);

  POSTCODE (SMM_IDENTIFY_FLASH_DEVICE); //PostCode = 0xA0, Identify Flash device in SMM

  Status = AutoDetectDevice (
             &(mSmmFwbGlobal->DevicePtr),
             mSmst
             );
  if (Buffer != NULL) {
    EfiCopyMem (Buffer, mSmmFwbGlobal->DevicePtr, sizeof (FLASH_DEVICE));
  }
  EnableFvbWrites (FALSE);

  return Status;
}

EFI_STATUS
FlashFdRead (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             Offset,
  IN OUT UINTN                          *NumBytes,
  OUT UINT8                             *Buffer
  )
{
  UINT8                                 *Address;
  UINTN                                 SpiAddress;
  EFI_STATUS                            Status;

  if (mSmmFwbGlobal->DevicePtr == NULL) {
    FlashFdDetectDevice (&mSmmFwbGlobal->SmmFwbService, NULL);
  }

  Address = (UINT8 *)((UINTN)(WriteAddress + Offset));
  EfiWbinvd ();
  Status = ConvertToSpiAddress ((UINTN)Address,&SpiAddress);
  if (EFI_ERROR (Status) || (mSmmFlashMode == SMM_FW_FLASH_MODE)) {
    Status = mSmmFwbGlobal->DevicePtr->Read (
                                  Buffer,
                                  Address,
                                  *NumBytes
                                  );
  } else {
    Status = mSmmFwbGlobal->DevicePtr->Read (
                                         Buffer,
                                         (UINT8 *)SpiAddress,
                                         *NumBytes
                                         );
  }
  ASSERT_EFI_ERROR (Status);
  return Status;
}

EFI_STATUS
FlashFdWrite (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN OUT UINTN                          *NumBytes,
  IN  UINT8                             *Buffer
  )
/*++

Routine Description:
  Writes specified number of bytes from the input buffer to the address

Arguments:
  WriteAddress -                Write address
  Address -                     Write address where align the 0x10000
  *NumBytes -                   The pointer of data size
  *Buffer -                     The pointer of the data
  LbaLength -                   Writable size

Returns:
  The status of programming flash

--*/
{
  UINT8                                 *Src;
  UINT8                                 *Dest;
  EFI_STATUS                            Status;
  UINTN                                 SpiAddress;

  if (mSmmFwbGlobal->DevicePtr == NULL) {
    FlashFdDetectDevice (&mSmmFwbGlobal->SmmFwbService, NULL);
  }

  EnableFvbWrites (TRUE);

  //
  // Write data one byte at a time, don't write if the src and dest bytes match
  //
  Dest = (UINT8 *) WriteAddress;
  Src = Buffer;

  Status = ConvertToSpiAddress ((UINTN)Dest,&SpiAddress);
  if (EFI_ERROR (Status) || (mSmmFlashMode == SMM_FW_FLASH_MODE)) {
    Status = mSmmFwbGlobal->DevicePtr->Program (
                    Dest,
                    Src,
                    NumBytes,
                    WriteAddress
                    );
  } else {
    Status = mSmmFwbGlobal->DevicePtr->Program (
                    (UINT8 *)SpiAddress,
                    Src,
                    NumBytes,
                    WriteAddress
                    );
  }

#ifndef NO_FLUSH_NVSTORE_CACHE
  WBCacheModify (WriteAddress, *NumBytes);
#endif

  EnableFvbWrites (FALSE);
  EfiWbinvd ();

  return Status;
}

EFI_STATUS
FlashFdEraseBlocks (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             *NumBytes
  )
/*++

Routine Description:
  Erase a certain block from address LbaWriteAddress

Arguments:
  WriteAddress                 -Write address
  *NumBytes                    -Writable size

Returns:
  The status of erase flash

--*/
{
  EFI_STATUS                            Status;
  UINTN                                 SpiAddress;

  if (mSmmFwbGlobal->DevicePtr == NULL) {
    FlashFdDetectDevice (&mSmmFwbGlobal->SmmFwbService, NULL);
  }

  EnableFvbWrites (TRUE);
  Status = ConvertToSpiAddress (WriteAddress,&SpiAddress);
  if (EFI_ERROR (Status) || (mSmmFlashMode == SMM_FW_FLASH_MODE)) {
    Status = mSmmFwbGlobal->DevicePtr->Erase (
                                       WriteAddress,
                                       *NumBytes
                                       );
  } else {
    Status = mSmmFwbGlobal->DevicePtr->Erase (
                                       SpiAddress,
                                       *NumBytes
                                       );
  }

#ifndef NO_FLUSH_NVSTORE_CACHE
  WBCacheModify (WriteAddress, *NumBytes);
#endif

  EnableFvbWrites (FALSE);
  EfiWbinvd ();

  return Status;
}
EFI_STATUS
FlashFdGetFlashTable (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  )
/*++

Routine Description:
  Get Platform flash table

Arguments:

  *FlashTable           - IN: the input buffer address
                          OUT:the flash region table from Platform

Returns:
  EFI_SUCCESS           - Successfully returns

--*/
{
  EFI_STATUS        Status;

  Status = GetPlatformFlashTable (FlashTable);
  return Status;
}
EFI_STATUS
SmmFwBlockServiceInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               InSmm;
  EFI_HANDLE                            Handle;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  SMM_FW_BLOCK_SERVICE_INSTANCE         *SmmFwbGlobal;
  EFI_BOOT_MODE                         BootMode;
  EFI_EVENT                             Event;
  VOID                                  *ProtocolInterface;

  Event = 0;

  Handle = NULL;

  //
  // Initialize the EFI Runtime Library
  //
  InsydeEfiInitializeSmmDriverLib (ImageHandle, SystemTable, &InSmm);

  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (InSmm) {

    //
    // Great!  We're now in SMM!
    //
    //
    // Initialize global variables
    //
    Status = gSMM->GetSmstLocation(gSMM, &mSmst);
   	if (EFI_ERROR (Status)) {
      return Status;
   	}

    Status = gSMM->SmmAllocatePool (
                     gSMM,
                     EfiRuntimeServicesData,
                     sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE),
                     &mSmmFwbGlobal
                     );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    EfiZeroMem (mSmmFwbGlobal, sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE));

    Status = SmmRT->LocateProtocol (
                      &gEfiSmmRuntimeProtocolGuid,
                      NULL,
                      &mSmmFwbGlobal->SmmRT
                          );
   	if (EFI_ERROR (Status)) {
      return Status;
   	}
    mSmmFlashMode = SMM_FW_DEFAULT_MODE;
    mSmmFwbGlobal->Signature                      = SMM_FW_BLOCK_SERVICE_SIGNATURE;
    mSmmFwbGlobal->SmmFwbService.FlashMode        = &mSmmFlashMode;
    mSmmFwbGlobal->SmmFwbService.DetectDevice     = FlashFdDetectDevice;
    mSmmFwbGlobal->SmmFwbService.Read             = FlashFdRead;
    mSmmFwbGlobal->SmmFwbService.Write            = FlashFdWrite;
    mSmmFwbGlobal->SmmFwbService.EraseBlocks      = FlashFdEraseBlocks;
    mSmmFwbGlobal->SmmFwbService.GetFlashTable    = FlashFdGetFlashTable;
    mSmmFwbGlobal->SmmFwbService.GetSpiFlashNumber = FlashFdGetSpiFlashNumber;
    mSmmFwbGlobal->SmmFwbService.ConvertToSpiAddress = FlashFdConvertToSpiAddress;

    //
    // Install the Driver within the SMM scope so that drivers created within the SMM scope can find it
    // and use the functionality. Runtime Library searches the protocol for performing the above functions.
    //
  Status = GetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);
  Status = mSmmFwbGlobal->SmmRT->LocateProtocol (&gEfiNonVolatileVariableProtocolGuid, NULL, &ProtocolInterface);
  if ((!EFI_ERROR (Status)) || (BootMode == BOOT_IN_RECOVERY_MODE)) {
    NonVolatileVariableNotifyFunction (Event, NULL);
  } else {
    Status = mSmmFwbGlobal->SmmRT->EnableProtocolNotify (
                                     NonVolatileVariableNotifyFunction,
                                     NULL,
                                     &gEfiNonVolatileVariableProtocolGuid,
                                     &Event
                                     );
    ASSERT_EFI_ERROR (Status);
  }

    Handle = NULL;
    Status = mSmmFwbGlobal->SmmRT->InstallProtocolInterface (
                                     &Handle,
                                     &gEfiSmmFwBlockServiceProtocolGuid,
                                     EFI_NATIVE_INTERFACE,
                                     &(mSmmFwbGlobal->SmmFwbService)
                                     );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE), &SmmFwbGlobal);
    EfiZeroMem (SmmFwbGlobal, sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE));

    //
    //  Install the Protocol Interface in the Boot Time Space. This is requires to satisfy the
    //  dependency within the drivers that are dependent upon Smm Runtime Driver.
    //
    Handle = NULL;
    EfiCopyMem (SmmFwbGlobal, mSmmFwbGlobal, sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE));

    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &(SmmFwbGlobal->SmmFwbService)
                    );
    InitFlashMode (mSmmFwbGlobal->SmmFwbService.FlashMode);
  }

  return Status;
}

EFI_STATUS
FlashFdGetSpiFlashNumber (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                              *FlashNumber
  )
/*++

Routine Description:
  Get Platform SPI Flash Number from SPI descriptor

Arguments:

  *FlashNumber           - the flash number

Returns:
  EFI_SUCCESS           - Successfully returns
  EFI_UNSUPPORTED       - Platform is not using SPI flash rom
                          SPI is not in descriptor mode

--*/
{
  EFI_STATUS        Status;

  Status = GetPlatformSpiFlashNumber (FlashNumber);
  return Status;
}

EFI_STATUS
FlashFdConvertToSpiAddress (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  )
/*++
Routine Description:

  Convert address for SPI descriptor mode

Arguments:

  Source      - The source address
  Destination - The destination address

Returns:

  EFI_SUCCESS           - Successfully returns
  EFI_UNSUPPORT         - Platform is not using SPI ROM
                        - The SPI flash is not in Descriptor mode
                        - The input address does not belong to BIOS memory mapped region

--*/
{
  EFI_STATUS        Status;

  Status = ConvertToSpiAddress (MemAddress, SpiAddress);
  return Status;
}
