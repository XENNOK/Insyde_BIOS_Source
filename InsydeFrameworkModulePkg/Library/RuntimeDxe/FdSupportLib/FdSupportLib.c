//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "EfiRuntimeLib.h"
#include "FdSupportLib.h"
#include "FWH_LPC.h"
#include "ChipsetLib.h"
#include "PostCode.h"

BOOLEAN gFwhLpcDev = FALSE;
BOOLEAN mSPIDev = FALSE;
UINTN   gCurrentFdIndex = FD_INDEX_INVALID;
UINT64  gCurrentFdId    = FD_ID_INVALID;

EFI_STATUS
Stall (
  IN  UINTN                             TenMicroSeconds
  )
/*++

Routine Description:
  Delay time. Out one 0xED I/O port will take one micro second

Arguments:
  TenMicroseconds -             Ten Micro second unit.

Returns:
  EFI_SUCCESS

--*/
{

  return EFI_SUCCESS;
}

EFI_STATUS
AutoDetectDevice (
  FLASH_DEVICE                              **FlashOperationPtr,
  EFI_SMM_SYSTEM_TABLE                      *mSmst
  )
{
  EFI_STATUS                                Status = EFI_SUCCESS;
  UINTN                                     RecognizeIndex, Count;
  FLASH_SPI_DEVICE                          **SPIFdSupportTable = NULL;
#ifndef EC_SPI_INTERFACE
  UINT64                                    DeviceId;
#endif

  *FlashOperationPtr = NULL;
  mSPIDev = FALSE;

#ifndef EC_SPI_INTERFACE
  DeviceId = 0;
  gFwhLpcDev = FALSE;

  if (IsLpcFlashDevice()) {
    Status = GetFdSupportNumber(&Count);

    for (RecognizeIndex = 0; RecognizeIndex < Count; RecognizeIndex++) {
      FdSupportTable[RecognizeIndex]->Init(mSmst);
      DeviceId = (FdSupportTable[RecognizeIndex])->DeviceInfo.Id;
      if (!EFI_ERROR ((FdSupportTable[RecognizeIndex])->Recognize(DeviceId))) {
        *FlashOperationPtr = FdSupportTable[RecognizeIndex];
        gFwhLpcDev = TRUE;

        return EFI_SUCCESS;
      }
    }
  }
#endif

  if (IsSpiFlashDevice()) {
    Status = GetSpiFdSupportTable((VOID ***)&SPIFdSupportTable);
    if (EFI_ERROR(Status)) {

      return Status;
    }

    Status = GetSpiSupportNumber(&Count);
    if ((gCurrentFdIndex < Count && gCurrentFdIndex != FD_INDEX_INVALID) && (gCurrentFdId != FD_ID_INVALID)) {
      //
      // gCurrentFdIndex of FdTableInfo Hob is initialized
      //
      SPIFdSupportTable[gCurrentFdIndex]->Init (mSmst);
      if ((!EFI_ERROR ((SPIFdSupportTable[gCurrentFdIndex])->Recognize (SPIFdSupportTable[gCurrentFdIndex])))&&
         (((SPIFdSupportTable[gCurrentFdIndex])->DeviceInfo.Id == gCurrentFdId))) {
          *FlashOperationPtr = (FLASH_DEVICE *) SPIFdSupportTable[gCurrentFdIndex];
          mSPIDev = TRUE;
          return EFI_SUCCESS;
      }
    }
    //
    // gCurrentFdIndex is not the index of the connected flash device, Re-recognition is needed
    //
    for (RecognizeIndex = 0; RecognizeIndex < Count; RecognizeIndex++) {
      SPIFdSupportTable[RecognizeIndex]->Init (mSmst);
      if (!EFI_ERROR ((SPIFdSupportTable[RecognizeIndex])->Recognize (SPIFdSupportTable[RecognizeIndex]))) {
        *FlashOperationPtr = (FLASH_DEVICE *)SPIFdSupportTable[RecognizeIndex];
        gCurrentFdIndex = RecognizeIndex;
        gCurrentFdId    = (SPIFdSupportTable[RecognizeIndex])->DeviceInfo.Id;
        mSPIDev = TRUE;
        return EFI_SUCCESS;
      }
    } 
  }
  POSTCODE (DXE_FLASH_PART_NONSUPPORT);
  EFI_DEADLOOP ();
  return EFI_UNSUPPORTED;
}

EFI_STATUS
FdSupportConvertPointer (
  OUT FLASH_DEVICE                      **FlashOperationPtr
  )
{
  UINTN                                 Index;
  UINTN                                 FdSupportNum;
  UINTN                                 Status;
  FLASH_SPI_DEVICE                      **SPIFdSupportTable = NULL;

  //
  // Convert LPC_FWH support services code for virtual memory.
  //
  GetFdSupportNumber (&FdSupportNum);

  for (Index = 0; Index < FdSupportNum; Index++) {
    (FdSupportTable[Index])->ConvertPointer ();
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*FdSupportTable[Index]).Recognize));
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*FdSupportTable[Index]).Erase));
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*FdSupportTable[Index]).Program));
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*FdSupportTable[Index]).Read));
  }
  //
  // Convert SPI support services code for virtual memory.
  //
  Status = GetSpiSupportNumber(&FdSupportNum);
  if (EFI_ERROR(Status)) {

    return Status;
  }

  Status = GetSpiFdSupportTable((VOID ***)&SPIFdSupportTable);
  if (EFI_ERROR(Status)) {

    return Status;
  }

  for (Index = 0; Index < FdSupportNum; Index++) {
    (SPIFdSupportTable[Index])->ConvertPointer ();
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*SPIFdSupportTable[Index]).Recognize));
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*SPIFdSupportTable[Index]).Erase));
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*SPIFdSupportTable[Index]).Program));
    EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID *) &((*SPIFdSupportTable[Index]).Read));
  }

  EfiConvertPointer (EFI_INTERNAL_POINTER, (VOID **) FlashOperationPtr);

  return EFI_SUCCESS;
}

EFI_STATUS
GetPlatformFlashTable (
  IN OUT   UINT8    *DataBuffer
  )
/*++
Routine Description:

  Get flash table from platform.

Arguments:

  DataBuffer      -IN: the input buffer address
                   OUT:the flash region table from rom file
Returns:

  EFI_SUCCESS           - Successfully returns

--*/
{

  return GetFlashTable (DataBuffer);
}

EFI_STATUS
GetPlatformSpiFlashNumber (
  OUT   UINT8    *FlashNumber
  )
/*++
Routine Description:

  Get flash number from platform.

Arguments:

  DataBuffer      - the flash number from SPI descriptor
Returns:

  EFI_SUCCESS           - Successfully returns
  EFI_UNSUPPORT         - Platform is not using SPI ROM

--*/
{

  return GetFlashNumber (FlashNumber);
}

EFI_STATUS
ConvertToSpiAddress (
  IN UINTN       Source,
  OUT UINTN      *Destination
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
  return SpiMemoryToBiosRegionAddress (Source, Destination, mSPIDev);
}
