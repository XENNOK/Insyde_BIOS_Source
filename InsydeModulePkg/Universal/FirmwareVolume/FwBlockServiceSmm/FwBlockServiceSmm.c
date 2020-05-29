/** @file
  This driver provides Firmware block services interface in SMM mode

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

#include "FwBlockServiceSmm.h"
#include <PostCode.h>

#define EFI_SMM_FWB_READY_PROTOCOL_GUID \
  { \
    0x3aeaec5f, 0x945a, 0x4ba5, 0xab, 0x98, 0xbc, 0x23, 0x4, 0x97, 0xb9, 0x8d  \
  }
EFI_GUID     mEfiSmmFwbReadyProtocolGuid = EFI_SMM_FWB_READY_PROTOCOL_GUID;

SMM_FW_BLOCK_SERVICE_INSTANCE           *mSmmFwbGlobal;
UINT8                                   mSmmFlashMode;
EFI_SMM_SYSTEM_TABLE2                   *mSmst;
EFI_SMM_COMMUNICATION_PROTOCOL          *mSmmCommunication;
UINT8                                   *mFwbBuffer;

typedef struct  {
  UINT64              Id;
  UINT8               Size;
  CHAR8               VendorName[MAX_STRING];
  CHAR8               DeviceName[MAX_STRING];
  FD_BLOCK_MAP        BlockMap;
} ECP_FD_INFO;

typedef struct  {
   FD_INFO            DeviceInfo;
   UINT8              *DummyPtr[6];
} ECP_FLASH_DEVICE;

/**
  Creates and returns a notification event and registers that event with all the protocol
  instances specified by ProtocolGuid.

  This function causes the notification function to be executed for every protocol of type
  ProtocolGuid instance that exists in the system when this function is invoked. In addition,
  every time a protocol of type ProtocolGuid instance is installed or reinstalled, the notification
  function is also executed. This function returns the notification event that was created.
  If ProtocolGuid is NULL, then ASSERT().
  If NotifyTpl is not a legal TPL value, then ASSERT().
  If NotifyFunction is NULL, then ASSERT().
  If Registration is NULL, then ASSERT().


  @param[in]  ProtocolGuid    Supplies GUID of the protocol upon whose installation the event is fired.
  @param[in]  NotifyTpl       Supplies the task priority level of the event notifications.
  @param[in]  NotifyFunction  Supplies the function to notify when the event is signaled.
  @param[in]  NotifyContext   The context parameter to pass to NotifyFunction.
  @param[out] Registration    A pointer to a memory location to receive the registration value.
                              This value is passed to LocateHandle() to obtain new handles that
                              have been added that support the ProtocolGuid-specified protocol.

  @return The notification event that was created.
**/
EFI_EVENT
EFIAPI
SmmFwCreateProtocolNotifyEvent (
  IN  EFI_GUID          *ProtocolGuid,
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT VOID              **Registration
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  ASSERT (ProtocolGuid != NULL);
  ASSERT (NotifyFunction != NULL);
  ASSERT (Registration != NULL);

  //
  // Create the event
  //

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  NotifyTpl,
                  NotifyFunction,
                  NotifyContext,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //

  Status = gBS->RegisterProtocolNotify (
                  ProtocolGuid,
                  Event,
                  Registration
                  );

  ASSERT_EFI_ERROR (Status);

  return Event;
}

/**
  Callback for SMM runtime.

  For compatibility with EDK, if EFI_SMM_RUNTIME_PROTOCOL exists, system also installs
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL to database in EFI_SMM_RUNTIME_PROTOCOL instance.

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.
**/
VOID
EFIAPI
SmmRuntimeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_HANDLE                            Handle;
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwbService;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }
  if (mSmmFwbGlobal->SmmRT == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
    if (EFI_ERROR (Status)) {
      return;
    }
    Status = SmmRT->LocateProtocol (
                      &gEfiSmmRuntimeProtocolGuid,
                      NULL,
                      (VOID **)&mSmmFwbGlobal->SmmRT
                      );
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  SmmRT = mSmmFwbGlobal->SmmRT;

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL),
                    (VOID **)&SmmFwbService
                    );
  if (EFI_ERROR (Status)) {
    return;
  }
  CopyMem (SmmFwbService, &mSmmFwbGlobal->SmmFwbService, sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE));
  SmmFwbService->DetectDevice = EcpFlashFdDetectDevice;

  Handle = NULL;
  Status = SmmRT->InstallProtocolInterface (
                    &Handle,
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    SmmFwbService
                    );
  ASSERT_EFI_ERROR (Status);
  //
  // Use gBS to install gEfiSmmFwBlockServiceProtocolGuid for ECP driver dependency compatible
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiSmmFwBlockServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  return;
}

/**
  Initialize boot time SMM firmware block service and install protocol.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

**/
VOID
EFIAPI
SmmFwbReady (
  IN  EFI_EVENT                             Event,
  IN  VOID                                  *Context
  )
{
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL     *SmmFwbService;
  VOID                                  *SmmFwbReady;
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;

  Status = gBS->LocateProtocol (&mEfiSmmFwbReadyProtocolGuid, NULL, (VOID **)&SmmFwbReady);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->AllocatePool (EfiBootServicesData, SMM_FWB_COMMUNICATE_ACCESS_FVB_SIZE + BLOCK_SIZE, (VOID **)&mFwbBuffer);
  ASSERT (mFwbBuffer != NULL);
  if (mFwbBuffer == NULL) {
    return;
  }

  gBS->AllocatePool (EfiBootServicesData, sizeof (EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL), (VOID **)&SmmFwbService);
  ASSERT (SmmFwbService != NULL);
  if (SmmFwbService == NULL) {
    return;
  }
  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &mSmmCommunication);
  ASSERT_EFI_ERROR (Status);


  SmmFwbService->DetectDevice        = BsFlashFdDetectDevice;
  SmmFwbService->Read                = BsFlashFdRead;
  SmmFwbService->Write               = BsFlashFdWrite;
  SmmFwbService->EraseBlocks         = BsFlashFdEraseBlocks;
  SmmFwbService->GetFlashTable       = BsFlashFdGetFlashTable;
  SmmFwbService->GetSpiFlashNumber   = BsFlashFdGetSpiFlashNumber;
  SmmFwbService->ConvertToSpiAddress = BsFlashFdConvertToSpiAddress;
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiSmmFwBlockServiceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  SmmFwbService
                  );
}

/**
  Communication service SMI Handler entry.
  This SMI handler provides services for the firmware volume access through SMI.

  @param[in]     DispatchHandle               The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]     RegisterContext              Points to an optional handler context which was specified when the
                                              handler was registered.
  @param[in, out] CommBuffer                  A pointer to a collection of data in memory that will
                                              be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize              The size of the CommBuffer.

  @retval EFI_SUCCESS                         The interrupt was handled and quiesced. No other handlers
                                              should still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_QUIESCED  The interrupt has been quiesced but other handlers should
                                              still be called.
  @retval EFI_WARN_INTERRUPT_SOURCE_PENDING   The interrupt is still pending and other handlers should still
                                              be called.
  @retval EFI_INTERRUPT_PENDING               The interrupt could not be quiesced.
**/
EFI_STATUS
EFIAPI
SmmFwBlockServiceHandler (
  IN     EFI_HANDLE                                DispatchHandle,
  IN     CONST VOID                                *RegisterContext,
  IN OUT VOID                                      *CommBuffer,
  IN OUT UINTN                                     *CommBufferSize
  )
{
  SMM_FWB_COMMUNICATE_HEADER                *SmmFwbFunctionHeader;
  SMM_FWB_COMMUNICATE_ACCESS_FVB            *SmmFwbAccessFvb;
  EFI_STATUS                                Status;

  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_SUCCESS;
  }

  if (*CommBufferSize != SMM_FWB_COMMUNICATE_ACCESS_FVB_SIZE + BLOCK_SIZE - SMM_COMMUNICATE_HEADER_SIZE) {
    return EFI_SUCCESS;
  }

  SmmFwbFunctionHeader = (SMM_FWB_COMMUNICATE_HEADER *) CommBuffer;
  SmmFwbAccessFvb      = (SMM_FWB_COMMUNICATE_ACCESS_FVB *) SmmFwbFunctionHeader->Data;

  switch (SmmFwbFunctionHeader->Function) {

  case SMM_FWB_FUNCTION_READ:
    Status = FlashFdRead (NULL, SmmFwbAccessFvb->Address, 0, &SmmFwbAccessFvb->NumBytes, (UINT8 *) (SmmFwbAccessFvb + 1));
    SmmFwbFunctionHeader->ReturnStatus = Status;
    break;

  case SMM_FWB_FUNCTION_WRITE:
    Status = FlashFdWrite (NULL, SmmFwbAccessFvb->Address, &SmmFwbAccessFvb->NumBytes, (UINT8 *) (SmmFwbAccessFvb + 1));
    SmmFwbFunctionHeader->ReturnStatus = Status;
    break;

  case SMM_FWB_FUNCTION_ERASE_BLOCKS:
    Status = FlashFdEraseBlocks (NULL, SmmFwbAccessFvb->Address, &SmmFwbAccessFvb->NumBytes);
    SmmFwbFunctionHeader->ReturnStatus = Status;
    break;

  default:
    ASSERT (FALSE);
    break;
  }

  return EFI_SUCCESS;
}


/**
  The driver's entry point.

  This driver provides Firmware block services interface in SMM mode.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
SmmFwBlockServiceInitialize (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_EVENT                             Event;
  VOID                                  *Registration;
  EFI_SMM_BASE2_PROTOCOL                *SmmBase;
  BOOLEAN                               InSmm;
  VOID                                  *SmmFwbRegistration;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (!InSmm) {
    EfiCreateProtocolNotifyEvent (
      &mEfiSmmFwbReadyProtocolGuid,
      TPL_CALLBACK,
      SmmFwbReady,
      NULL,
      &SmmFwbRegistration
      );
    return EFI_SUCCESS;
  }

  Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE),
                    (VOID **)&mSmmFwbGlobal
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (mSmmFwbGlobal, sizeof (SMM_FW_BLOCK_SERVICE_INSTANCE));

  mSmmFwbGlobal->Signature                         = SMM_FW_BLOCK_SERVICE_SIGNATURE;
  mSmmFlashMode                                    = SMM_FW_DEFAULT_MODE;
  mSmmFwbGlobal->SmmFwbService.FlashMode           = &mSmmFlashMode;

  mSmmFwbGlobal->SmmFwbService.DetectDevice        = FlashFdDetectDevice;
  mSmmFwbGlobal->SmmFwbService.Read                = FlashFdRead;
  mSmmFwbGlobal->SmmFwbService.Write               = FlashFdWrite;
  mSmmFwbGlobal->SmmFwbService.EraseBlocks         = FlashFdEraseBlocks;
  mSmmFwbGlobal->SmmFwbService.GetFlashTable       = FlashFdGetFlashTable;
  mSmmFwbGlobal->SmmFwbService.GetSpiFlashNumber   = FlashFdGetSpiFlashNumber;
  mSmmFwbGlobal->SmmFwbService.ConvertToSpiAddress = FlashFdConvertToSpiAddress;

  Status = gBS->LocateProtocol(&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  if (!EFI_ERROR(Status)) {
    Status = SmmRT->LocateProtocol (
                      &gEfiSmmRuntimeProtocolGuid,
                      NULL,
                      (VOID **)&mSmmFwbGlobal->SmmRT
                      );
    ASSERT_EFI_ERROR (Status);
    SmmRuntimeCallback (NULL, NULL);
  } else {
    Event = SmmFwCreateProtocolNotifyEvent (
               &gEfiSmmRuntimeProtocolGuid,
               TPL_CALLBACK,
               SmmRuntimeCallback,
               NULL,
               &Registration
               );
  }

  Handle = NULL;
  Status = mSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiSmmFwBlockServiceProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mSmmFwbGlobal->SmmFwbService
                    );
  InitFlashMode (mSmmFwbGlobal->SmmFwbService.FlashMode);

  Handle = NULL;
  Status = mSmst->SmiHandlerRegister (SmmFwBlockServiceHandler, &gEfiSmmFwBlockServiceProtocolGuid, &Handle);
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &mEfiSmmFwbReadyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  return Status;
}

/**
  Detect target flash device and copy whole flash device to output buffer.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_SUCCESS           The flash device is recognized successfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
EFI_STATUS
EFIAPI
FlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  OUT UINT8                             *Buffer
  )
{
  EFI_STATUS        Status;

  if (Buffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetFlashDevice (&mSmmFwbGlobal->FlashDevice);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (Buffer, mSmmFwbGlobal->FlashDevice, sizeof (FLASH_DEVICE));
  return EFI_SUCCESS;
}


/**
  Detect target flash device and copy whole flash device to output buffer for ECP driver.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_SUCCESS           The flash device is recognized successfully.
  @retval EFI_NOT_FOUND         Cannot find supported flash device.
  @retval EFI_INVALID_PARAMETER Invalid function parameter.
**/
EFI_STATUS
EFIAPI
EcpFlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  OUT UINT8                             *Buffer
  )
{

  ECP_FLASH_DEVICE           EcpFlashDevice;
  EFI_STATUS                 Status;

  Status = FlashFdDetectDevice (This, Buffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (&EcpFlashDevice, sizeof (ECP_FLASH_DEVICE));
  CopyMem (&EcpFlashDevice.DeviceInfo,  &(((FLASH_DEVICE *) Buffer)->DeviceInfo), sizeof (ECP_FD_INFO));
  CopyMem (Buffer, &EcpFlashDevice, sizeof (ECP_FLASH_DEVICE));

  return EFI_SUCCESS;
}

/**
 Reads data beginning at Lba:Offset from FV and places the data in Buffer.
 The read terminates either when *NumBytes of data have been read, or when
 a block boundary is reached.  *NumBytes is updated to reflect the actual
 number of bytes read.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      WriteAddress    Start LBA address which want to read.
 @param [in]      Offset          Offset in the block at which to begin read
 @param [in, out] NumBytes        IN:  The requested read size.
                                  OUT: The data size by bytes has been read
 @param [out]     Buffer          Data buffer in which to place data read.

 @retval EFI_SUCCESS              Read data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Read data failed caused by device error.
**/
EFI_STATUS
EFIAPI
FlashFdRead (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             Offset,
  IN OUT UINTN                          *NumBytes,
  OUT UINT8                             *Buffer
  )
{
  return FlashRead (Buffer, (UINT8 *) (WriteAddress + Offset), *NumBytes);
}

/**
 Writes data beginning at Lba:Offset from FV. The write terminates either
 when *NumBytes of data have been written, or when a block boundary is
 reached.  *NumBytes is updated to reflect the actual number of bytes
 written.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      Offset          Start address to be written.
 @param [in, out] NumBytes        IN:  The requested write size.
                                  OUT: The data size by bytes has been written.
 @param [out]     Buffer          Data buffer want to write.

 @retval EFI_SUCCESS              Write data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Write data failed caused by device error.
**/
EFI_STATUS
EFIAPI
FlashFdWrite (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN OUT UINTN                          *NumBytes,
  IN  UINT8                             *Buffer
  )
{
  return FlashProgram ((UINT8 *) WriteAddress, Buffer, NumBytes, WriteAddress);
}

/**
 The EraseBlock() function erases one or more blocks as denoted by the
 variable argument list. The entire parameter list of blocks must be verified
 prior to erasing any blocks.  If a block is requested that does not exist
 within the associated firmware volume (it has a larger index than the last
 block of the firmware volume), the EraseBlock() function must return
 EFI_INVALID_PARAMETER without modifying the contents of the firmware volume.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      LbaWriteAddress Start address to be erased.
 @param [in, out] NumBytes        IN:  The requested erase size.
                                  OUT: The data size by bytes has been erased.

 @retval EFI_SUCCESS              Erase data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Erase data failed caused by device error.
**/
EFI_STATUS
EFIAPI
FlashFdEraseBlocks (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             *NumBytes
  )
{
  return FlashErase (WriteAddress, *NumBytes);
}

/**
 The GetFlashTable() function get flash region table from platform.
 if the descriptor is valid, the FlashTable will be filled.
 if the descriptor is invalid,the FlashTable will be filled RegionTypeEos(0xff) directly.

 @param [in]      This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in, out] FlashTable    IN:  Input buffer which want to save flash table.
                                OUT: Buffer to save flash table.

 @retval EFI_SUCCESS            Get flash table successful.
 @retval EFI_UNSUPPORTED        Flash device doesn't support this feature.
**/
EFI_STATUS
EFIAPI
FlashFdGetFlashTable (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  )
{
  return GetPlatformFlashTable (FlashTable);
}

/**
 Get Platform SPI Flash Number from SPI descriptor

 @param [in]  This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [out] FlashNumber   Number of SPI flash devices returned.

 @retval EFI_SUCCESS        Successfully returns.
 @retval EFI_UNSUPPORTED    Platform is not using SPI flash rom or SPI is not in descriptor mode.
**/
EFI_STATUS
EFIAPI
FlashFdGetSpiFlashNumber (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                              *FlashNumber
  )
{
  return GetPlatformSpiFlashNumber (FlashNumber);
}


/**
 Convert address for SPI descriptor mode

 @param [in] This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param[in]  MemAddress    BIOS memory mapped address.
 @param[out] SpiAddress    Flash ROM start address + BIOS address in flash ROM.

 @retval EFI_SUCCESS       Successfully returns.
 @retval EFI_UNSUPPORT     Platform is not using SPI ROM.
                           The SPI flash is not in Descriptor mode.
                           The input address does not belong to BIOS memory mapped region.
**/
EFI_STATUS
EFIAPI
FlashFdConvertToSpiAddress (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  )
{
  return ConvertToSpiAddress (MemAddress, SpiAddress);
}

/**
  Initialize the communicate buffer using DataSize and Function.

  The communicate size is: SMM_FWB_COMMUNICATE_ACCESS_FVB_SIZE.

  @param[in]  Function            The function number to initialize the communicate header.
  @param[out] DataPtr             Points to the data in the communicate buffer.

  @retval EFI_SUCCESS             Find the specified variable.
  @retval EFI_INVALID_PARAMETER   The data size is too big.
**/
EFI_STATUS
InitCommunicateBuffer (
  IN      UINTN                             Function,
  OUT     VOID                              **DataPtr OPTIONAL
  )
{
  EFI_SMM_COMMUNICATE_HEADER                *SmmCommunicateHeader;
  SMM_FWB_COMMUNICATE_HEADER                *SmmFwbFunctionHeader;


  SmmCommunicateHeader = (EFI_SMM_COMMUNICATE_HEADER *) mFwbBuffer;
  CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiSmmFwBlockServiceProtocolGuid);
  SmmCommunicateHeader->MessageLength = SMM_FWB_COMMUNICATE_ACCESS_FVB_SIZE + BLOCK_SIZE - SMM_COMMUNICATE_HEADER_SIZE;

  SmmFwbFunctionHeader = (SMM_FWB_COMMUNICATE_HEADER *) SmmCommunicateHeader->Data;
  SmmFwbFunctionHeader->Function = Function;
  if (DataPtr != NULL) {
    *DataPtr = SmmFwbFunctionHeader->Data;
  }

  return EFI_SUCCESS;
}

/**
  Send the data in communicate buffer to SMM.

  @retval      EFI_SUCCESS            Success is returned from the functin in SMM.
  @retval      Others                 Failure is returned from the function in SMM.
**/
EFI_STATUS
SendCommunicateBuffer (
  VOID
  )
{
  EFI_STATUS                                Status;
  UINTN                                     CommSize;
  EFI_SMM_COMMUNICATE_HEADER                *SmmCommunicateHeader;
  SMM_FWB_COMMUNICATE_HEADER                *SmmFwbFunctionHeader;

  CommSize = SMM_FWB_COMMUNICATE_ACCESS_FVB_SIZE + BLOCK_SIZE;
  Status = mSmmCommunication->Communicate (mSmmCommunication, mFwbBuffer, &CommSize);
  ASSERT_EFI_ERROR (Status);

  SmmCommunicateHeader    = (EFI_SMM_COMMUNICATE_HEADER *) mFwbBuffer;
  SmmFwbFunctionHeader    = (SMM_FWB_COMMUNICATE_HEADER *) SmmCommunicateHeader->Data;
  return  SmmFwbFunctionHeader->ReturnStatus;
}

/**
  Detect target flash device and copy whole flash device to output buffer.

  This function always returns EFI_UNSUPPORTED.

  @param[in]  This               Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
  @param[out] SystemTable        Buffer to saved target flash device information

  @retval EFI_UNSUPPORTED        Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
EFIAPI
BsFlashFdDetectDevice (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  OUT UINT8                             *Buffer
  )
{
  return EFI_UNSUPPORTED;
}

/**
 Reads data beginning at Lba:Offset from FV and places the data in Buffer.
 The read terminates either when *NumBytes of data have been read, or when
 a block boundary is reached.  *NumBytes is updated to reflect the actual
 number of bytes read.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      WriteAddress    Start LBA address which want to read.
 @param [in]      Offset          Offset in the block at which to begin read
 @param [in, out] NumBytes        IN:  The requested read size.
                                  OUT: The data size by bytes has been read
 @param [out]     Buffer          Data buffer in which to place data read.

 @retval EFI_SUCCESS              Read data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Read data failed caused by device error.
**/
EFI_STATUS
EFIAPI
BsFlashFdRead (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             Offset,
  IN OUT UINTN                          *NumBytes,
  OUT UINT8                             *Buffer
  )
{
  SMM_FWB_COMMUNICATE_ACCESS_FVB         *FwbAccessHeader;
  UINTN                                  ReadBytes;
  EFI_STATUS                             Status;

  InitCommunicateBuffer (SMM_FWB_FUNCTION_READ, (VOID **) &FwbAccessHeader);
  if (FwbAccessHeader == NULL) {
    return EFI_UNSUPPORTED;
  }

  for (ReadBytes = 0; ReadBytes + BLOCK_SIZE < *NumBytes; ReadBytes += BLOCK_SIZE) {
    FwbAccessHeader->Address   = WriteAddress + Offset + ReadBytes;
    FwbAccessHeader->NumBytes  = BLOCK_SIZE;
    Status = SendCommunicateBuffer ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
    CopyMem (Buffer + ReadBytes, FwbAccessHeader + 1, BLOCK_SIZE);
  }
  if (ReadBytes != *NumBytes) {
    FwbAccessHeader->Address  = WriteAddress + Offset + ReadBytes;
    FwbAccessHeader->NumBytes = *NumBytes - ReadBytes;
    Status = SendCommunicateBuffer ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
    CopyMem (Buffer + ReadBytes, FwbAccessHeader + 1, *NumBytes - ReadBytes);
  }
  return EFI_SUCCESS;
}

/**
 Writes data beginning at Lba:Offset from FV. The write terminates either
 when *NumBytes of data have been written, or when a block boundary is
 reached.  *NumBytes is updated to reflect the actual number of bytes
 written.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      Offset          Start address to be written.
 @param [in, out] NumBytes        IN:  The requested write size.
                                  OUT: The data size by bytes has been written.
 @param [out]     Buffer          Data buffer want to write.

 @retval EFI_SUCCESS              Write data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Write data failed caused by device error.
**/
EFI_STATUS
EFIAPI
BsFlashFdWrite (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN OUT UINTN                          *NumBytes,
  IN  UINT8                             *Buffer
  )
{
  SMM_FWB_COMMUNICATE_ACCESS_FVB         *FwbAccessHeader;
  UINTN                                  WriteBytes;
  EFI_STATUS                             Status;

  InitCommunicateBuffer (SMM_FWB_FUNCTION_WRITE, (VOID **) &FwbAccessHeader);
  if (FwbAccessHeader == NULL) {
    return EFI_UNSUPPORTED;
  }

  for (WriteBytes = 0; WriteBytes + BLOCK_SIZE < *NumBytes; WriteBytes += BLOCK_SIZE) {
    FwbAccessHeader->Address   = WriteAddress + WriteBytes;
    FwbAccessHeader->NumBytes  = BLOCK_SIZE;
    CopyMem (FwbAccessHeader + 1, Buffer + WriteBytes, BLOCK_SIZE);
    Status = SendCommunicateBuffer ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  if (WriteBytes != *NumBytes) {
    FwbAccessHeader->Address   = WriteAddress + WriteBytes;
    FwbAccessHeader->NumBytes = *NumBytes - WriteBytes;
    CopyMem (FwbAccessHeader + 1, Buffer + WriteBytes, *NumBytes - WriteBytes);
    Status = SendCommunicateBuffer ();
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
 The EraseBlock() function erases one or more blocks as denoted by the
 variable argument list. The entire parameter list of blocks must be verified
 prior to erasing any blocks.  If a block is requested that does not exist
 within the associated firmware volume (it has a larger index than the last
 block of the firmware volume), the EraseBlock() function must return
 EFI_INVALID_PARAMETER without modifying the contents of the firmware volume.

 @param [in]      This            Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in]      LbaWriteAddress Start address to be erased.
 @param [in, out] NumBytes        IN:  The requested erase size.
                                  OUT: The data size by bytes has been erased.

 @retval EFI_SUCCESS              Erase data successful.
 @retval EFI_INVALID_PARAMETER    Input function parameters are invalid.
 @retval EFI_UNSUPPORTED          The flash device is not supported
 @retval EFI_DEVICE_ERROR         Erase data failed caused by device error.
**/
EFI_STATUS
EFIAPI
BsFlashFdEraseBlocks (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL *This,
  IN  UINTN                             WriteAddress,
  IN  UINTN                             *NumBytes
  )
{
  SMM_FWB_COMMUNICATE_ACCESS_FVB         *FwbAccessHeader;

  InitCommunicateBuffer (SMM_FWB_FUNCTION_ERASE_BLOCKS, (VOID **) &FwbAccessHeader);
  if (FwbAccessHeader == NULL) {
    return EFI_UNSUPPORTED;
  }
  FwbAccessHeader->Address   = WriteAddress;
  FwbAccessHeader->NumBytes = *NumBytes;
  return SendCommunicateBuffer ();
}

/**
 The GetFlashTable() function get flash region table from platform.
 if the descriptor is valid, the FlashTable will be filled.
 if the descriptor is invalid,the FlashTable will be filled RegionTypeEos(0xff) directly.

 This function always returns EFI_UNSUPPORTED.

 @param [in]      This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [in, out] FlashTable    IN:  Input buffer which want to save flash table.
                                OUT: Buffer to save flash table.

 @retval EFI_UNSUPPORTED        Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
EFIAPI
BsFlashFdGetFlashTable (
  IN EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN OUT UINT8                          *FlashTable
  )
{
  return EFI_UNSUPPORTED;
}

/**
 Get Platform SPI Flash Number from SPI descriptor

 This function always returns EFI_UNSUPPORTED.

 @param [in]  This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param [out] FlashNumber   Number of SPI flash devices returned.

 @retval EFI_UNSUPPORT      Always return EFI_UNSUPPORTED.
**/
EFI_STATUS
EFIAPI
BsFlashFdGetSpiFlashNumber (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  OUT UINT8                              *FlashNumber
  )
{
  return EFI_UNSUPPORTED;
}

/**
 Convert address for SPI descriptor mode

 This function always returns EFI_UNSUPPORTED.

 @param [in] This          Pointer to EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL instance.
 @param[in]  MemAddress    BIOS memory mapped address.
 @param[out] SpiAddress    Flash ROM start address + BIOS address in flash ROM.

 @retval EFI_UNSUPPORT     Always return EFI_UNSUPPORTED.
 **/
EFI_STATUS
EFIAPI
BsFlashFdConvertToSpiAddress (
  IN  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *This,
  IN  UINTN                              MemAddress,
  OUT UINTN                              *SpiAddress
  )
{
  return EFI_UNSUPPORTED;
}