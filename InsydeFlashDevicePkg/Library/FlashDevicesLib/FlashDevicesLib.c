/** @file
  Flash Device Initialization and Recognition.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/H2OFlashLib.h>
#include <LpcFlashDevice.h>
#include <SpiFlashDevice.h>

FLASH_DEVICE              mOldTypeFlashDevice;
H2O_FLASH_DEVICE          mNewTypeFlashDevice;
LPC_FLASH_DEVICE_INFO     mLpcInfo;
H2O_FLASH_LPC_DEVICE      mH2OLpcDevice;
SPI_CONFIG_BLOCK          mSpiConfig;
H2O_FLASH_SPI_DEVICE      mH2OSpiDevice;

/**
  Convert H2O_FLASH_DEVICE structure to FLASH_DEVICE structure

  @param H2OFlashDevice         pointer to H2O_FLASH_DEVICE structure
  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           Flash device structure is successfully converted
  @retval EFI_INVALID_PARAMETER Parameters given are invalid (NULL)

**/
EFI_STATUS
ConvertFlashDevice (
  H2O_FLASH_DEVICE      *H2OFlashDevice,
  FLASH_DEVICE          *FlashDevice
  )
{
  UINT32 DeviceSize;

  if (H2OFlashDevice == NULL || FlashDevice == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  FlashDevice->DeviceType = H2OFlashDevice->DeviceType;
  FlashDevice->DeviceInfo.Id = H2OFlashDevice->Id;
  FlashDevice->DeviceInfo.ExtId = H2OFlashDevice->ExtId;
  FlashDevice->DeviceInfo.BlockMap.BlockSize = H2OFlashDevice->BlockMap[0].Size;
  FlashDevice->DeviceInfo.BlockMap.Mutiple = H2OFlashDevice->BlockMap[0].Count;
  FlashDevice->DeviceInfo.BlockMap.EOS = 0xffff;

  DeviceSize = (UINT32)H2OFlashDevice->BlockMap[0].Size * (UINT32)H2OFlashDevice->BlockMap[0].Count * 256;
  switch (DeviceSize) {
    case 128 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_128K;
      break;
    case 256 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_256K;
      break;
    case 512 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_512K;
      break;
    case 1024 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_1024K;
      break;
    case 2048 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_2048K;
      break;
    case 4096 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_4096K;
      break;
    case 8192 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_8192K;
      break;
    case 16384 * SIZE_1KB:
      FlashDevice->DeviceInfo.Size = FLASH_SIZE_16384K;
      break;
    default:
      FlashDevice->DeviceInfo.Size = 0xff;
      break;
  }

  switch (H2OFlashDevice->DeviceType) {
  case LpcFlashType1:
  case LpcFlashType2:
  case LpcFlashType3:
    CopyMem (
      &mLpcInfo,
      &((H2O_FLASH_LPC_DEVICE *)H2OFlashDevice->DeviceTypeData)->IdAddress,
      sizeof (LPC_FLASH_DEVICE_INFO)
      );
    FlashDevice->TypeSpecificInfo = (VOID *)&mLpcInfo;
    break;

  case SpiFlashType:
    //
    // Copy OpCodeMenu, OpType, PrefixMenu
    //
    CopyMem(mSpiConfig.OpCodeMenu,  &(((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->ReadIdCommandOp), 12);
    mSpiConfig.GlobalProtect = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->GlobalProtectAvailable;
    mSpiConfig.BlockProtect = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BlockProtectAvailable;
    mSpiConfig.BlockProtectDataRequired = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BlockProtectCodeRequired;
    mSpiConfig.ProgramGranularity = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->MultiByteProgramAvailable;
    mSpiConfig.FlashIDSize = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BytesOfId;
    mSpiConfig.MinBytesPerOp = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->MinBytesPerProgRead;
    mSpiConfig.NVStatusBit = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->NonVolStatusAvailable;
    mSpiConfig.GlobalProtectCode = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->GlobalProtectCode;
    mSpiConfig.GlobalUnprotectCode = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->GlobalUnprotectCode;
    mSpiConfig.BlockProtectCode = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BlockProtectCode;
    mSpiConfig.BlockUnprotectCode = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BlockUnprotectCode;
    mSpiConfig.DeviceSize = DeviceSize;
    mSpiConfig.BlockEraseSize = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BlockEraseSize;
    mSpiConfig.BlockProtectSize = ((H2O_FLASH_SPI_DEVICE *)H2OFlashDevice->DeviceTypeData)->BlockProtectSize;
    FlashDevice->TypeSpecificInfo = (VOID *)&mSpiConfig;

    break;
  default:
    return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}


/**
  Detect and Initialize SPI flash part OpCode and other parameter through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to Recoginize the SPI device

**/
EFI_STATUS
EFIAPI
RecognizeFlashDevice (
  IN H2O_FLASH_DEVICE            *H2OFlashDevice
  )
{
  ConvertFlashDevice (H2OFlashDevice, &mOldTypeFlashDevice);
  SetFlashDevice (&mOldTypeFlashDevice);
  return FlashRecognize ();
}


/**
  Recognize Flash Device through PCDex Token number

  @param PcdExTokenNumber       pointer to PCDex Token number

  @retval H2O_FLASH_DEVICE*     pointer to H2O_FLASH_DEVICE, NULL if not recognize any flash device

**/
H2O_FLASH_DEVICE *
RecognizeFlashDeviceByPcdEx (
   UINTN                *PcdExTokenNumber
){
  UINTN                 StrucSize;
  UINTN                 PcdSize;
  UINTN                 ConfigSize;
  H2O_PCD_FLASH_DEVICE  *PcdFlashDevice;
  VOID                  *DeviceConfig;
  EFI_STATUS            Status;
  UINTN                 TokenNum;

  
  TokenNum = *PcdExTokenNumber;
      
  PcdSize = LibPcdGetExSize(&gH2OFlashDeviceGuid, TokenNum);
  StrucSize = OFFSET_OF(H2O_FLASH_DEVICE, DeviceType) + PcdSize;
  mNewTypeFlashDevice.Size = (UINT32)StrucSize;
  PcdFlashDevice = (H2O_PCD_FLASH_DEVICE *)LibPcdGetExPtr(&gH2OFlashDeviceGuid, TokenNum);
  if (PcdFlashDevice == NULL) {
    ASSERT (PcdFlashDevice != NULL);
    return NULL;
  }
  CopyMem((VOID *)(UINTN)&mNewTypeFlashDevice.DeviceType, (VOID *)PcdFlashDevice, PcdSize);
  if (mNewTypeFlashDevice.DeviceType < SpiFlashDeviceType) {
    mNewTypeFlashDevice.DeviceTypeData = (VOID *)&mH2OLpcDevice;
  } else if (mNewTypeFlashDevice.DeviceType == SpiFlashDeviceType) {
    mNewTypeFlashDevice.DeviceTypeData = (VOID *)&mH2OSpiDevice;
  } else {
    ASSERT_EFI_ERROR (EFI_DEVICE_ERROR);
    return NULL;
  }
  ConfigSize = LibPcdGetExSize(&gH2OFlashDeviceConfigGuid, TokenNum);
  DeviceConfig = LibPcdGetExPtr (&gH2OFlashDeviceConfigGuid, TokenNum);
  if (DeviceConfig == NULL) {
    ASSERT (DeviceConfig != NULL);
    return NULL;
  }
  CopyMem (mNewTypeFlashDevice.DeviceTypeData, DeviceConfig, ConfigSize);
  Status = RecognizeFlashDevice (&mNewTypeFlashDevice);
  if (Status == EFI_SUCCESS) {
    *PcdExTokenNumber = TokenNum;
    return &mNewTypeFlashDevice;
  }
      
  return NULL; 
}


H2O_FLASH_DEVICE *
DetectH2OFlashDevice (
  VOID
  )
{
  UINTN                 PcdTokenNumber;
  UINT8                 IsPollingMode;  
  H2O_FLASH_DEVICE      *FlashDeviceInstance;

  FlashDeviceInstance = NULL;
  IsPollingMode = FALSE;   
  
  PcdTokenNumber = PcdGet32 (PcdActiveFlashDeviceId);
   
  if (PcdTokenNumber == 0){
    IsPollingMode = TRUE;
  } 

  do {

    if (IsPollingMode){
       PcdTokenNumber = LibPcdGetNextToken(&gH2OFlashDeviceGuid, PcdTokenNumber);        
       if (PcdTokenNumber == 0) {
         break;
       }
    }   
    
    FlashDeviceInstance = RecognizeFlashDeviceByPcdEx(&PcdTokenNumber);
    //
    // Rescan flash devices if flash device with PcdActiveFlashDeviceId cannot be recognized
    //    
    if ((FlashDeviceInstance == NULL) && (!IsPollingMode)){
      IsPollingMode = TRUE;
      PcdTokenNumber = 0;
    }
  
  } while (!FlashDeviceInstance);

  PcdSet32 (PcdActiveFlashDeviceId, (UINT32)PcdTokenNumber);
  return FlashDeviceInstance;
          
}

/**
  FlashDevicesLib Library Class Constructor

  @retval EFI_SUCCESS:   Module initialized successfully
  @retval Others     :   Module initialization failed

**/
EFI_STATUS
EFIAPI
FlashDevicesLibInit (
  VOID
  )
{
  DetectH2OFlashDevice();
  return EFI_SUCCESS;
}
