/** @file

Init SIO in PEI stage.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SioInitPei.h"

EFI_SIO_TABLE mSioTable[] = {
  //======Default Start======//
  {SIO_LDN, SIO_KYBD_MOUSE},
  {SIO_BASE_IO_ADDR1_MSB, 0x00},
  {SIO_BASE_IO_ADDR1_LSB, 0x60},
  {SIO_BASE_IO_ADDR2_MSB, 0x00},
  {SIO_BASE_IO_ADDR2_LSB, 0x64},
  {SIO_IRQ_SET, 0x01},
  {SIO_MOUSE_IRQ_SET, 0x0C},
  {SIO_DEV_ACTIVE, TRUE},
  {SIO_POWER_DOWN1, 0x3C},
  {SIO_POWER_DOWN1, 0xBC},
  //====== Default End ======//
  {0x00 , 0x00}
};


UINT16  mSioCfgPortList[]    = {0x2E, 0x4E, 0x162E, 0x164E};

//[-start-140116-IB12970054-modify]//
UINT16  mSioCfgPortListSize  = sizeof (mSioCfgPortList) / sizeof (UINT16);
//[-end-140116-IB12970054-modify]//

UINT8   EnterConfig         = ENTER_CONFIG;
UINT8   ExitConfig          = EXIT_CONFIG;

static EFI_PEI_PPI_DESCRIPTOR mSioInitializedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSioInitializedPpiGuid,
  NULL
};

/**
  SIO pei initial

  @param[in] FfsHeader            Pointer to EFI_FFS_FILE_HEADER
  @param[in] PeiServices          Pointer's pointer to EFI_PEI_SERVICES

  @retval     EFI_SUCCESS         The operation performed successfully.
**/
EFI_STATUS
EFIAPI
SioInitPeim (
  IN EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES      **PeiServices
  )
{
  EFI_STATUS                     Status;
  UINT16                         IndexPort;
  UINT16                         DataPort;
  EFI_SIO_TABLE                  *OemTable;
  UINT8                          SioInstance;

  OemTable    = NULL;
  SioInstance = NULL_ID;

  PostCode (PEI_SIO_INIT); //PostCode = 0x70, Super I/O initial
  
  //
  // OemService
  //
  Status = OemSvcBeforeInitSioPei ();
  if (Status == EFI_UNSUPPORTED) {
    BeforeSioInit ();
  }

  // Get correct config port, if it's wrong, return fail.
  IndexPort = CheckDevice (&SioInstance); 
  if (IndexPort == 0) {
    return EFI_UNSUPPORTED;
  }
  DataPort = IndexPort + 1;  
  
  //
  // OemService
  //
  Status = OemSvcRegSioPei (
             SioInstance, 
             &OemTable
             );
  if (Status != EFI_SUCCESS) {
    OemTable = mSioTable;
  } 
  
  while ((OemTable->Register != 0) || (OemTable->Value != 0)) {
    IoWrite8 (IndexPort, OemTable->Register);
    IoWrite8 (DataPort, OemTable->Value);
    OemTable++;
  }
 
  //
  // OemService
  //
  Status = OemSvcAfterInitSioPei (
             SioInstance, 
             IndexPort
             );
  if (Status == EFI_UNSUPPORTED) {
    AfterSioInit (IndexPort);
  }
      
  //
  // Exit Config Mode
  //
  ExitConfigMode (IndexPort);   
  Status = (**PeiServices).InstallPpi (
                             PeiServices, 
                             &mSioInitializedPpi
                             );  
  return Status;
}

/**
  Decode IO Port By Different Platform, Like AMD¡BINTEL¡K 

  @retval     EFI_SUCCESS          The operation performed successfully.
**/
EFI_STATUS
BeforeSioInit (
  VOID
  )
{
  //
  // Program and Enable SIO Base Addresses for Com1(3F8-3FF)/Com2(2F8-2FF)/EC(62/66)/KBC(60/64) 
  // and enable decode range 2E,2F/4E,4F
  //
  IoWrite32 (0xcf8, 0x8000f880);
  IoWrite32 (0xcfc, 0x3c030010);

  return EFI_SUCCESS;
}

/**
  After Init SIO, You Need To Check, or Change Some Value.

  @param[in]   IndexPort            This SIO config port.

  @retval      EFI_SUCCESS          The operation performed successfully.
**/
EFI_STATUS
AfterSioInit (
  IN UINT16               IndexPort
  )
{

  return EFI_SUCCESS;
}

/**
 Enter SIO mode.

 @param[in]   ConfigPort             SIO config port.
**/
VOID
EnterConfigMode (
  IN UINT16  ConfigPort
  )
{
  IoWrite8 (ConfigPort, EnterConfig);
  IoWrite8 (ConfigPort, EnterConfig);
}

/**
 Exit SIO mode.

 @param[in]   ConfigPort             SIO config port.
**/
VOID
ExitConfigMode (
  IN UINT16  ConfigPort
  )    
{
  IoWrite8 (ConfigPort, ExitConfig);
}

/**
  To get correct SIO data.

  @param[out]  *SioInstance         if success, return which SIO instance pointer.

  @retval     IndexPort             return value as zero, it means "not match"
                                    return value is not zero, it means "match".                                    
**/
UINT16
CheckDevice (
  UINT8           *SioInstance
  ) 
{
  SIO_DEVICE_LIST_TABLE              *mTablePtr;
  UINT16                             IndexPort;

  IndexPort = 0;
  mTablePtr  = (SIO_DEVICE_LIST_TABLE *)PcdGetPtr (PcdSioNct6683dluCfg);

  //
  // Calculate the number of non-zero entries in the table
  //
  while (!((mTablePtr->TypeH == NONE_ID) && (mTablePtr->TypeL == NONE_ID))) {
    if (mTablePtr->Device == CFG_ID){
      CheckConfig (mTablePtr, &IndexPort);
      if (IndexPort != 0) {
        *SioInstance = mTablePtr->TypeInstance;
        return IndexPort;          
      } 
    }
    mTablePtr++;
  } 
  return IndexPort;
}

/**
  Check ID of SIO whether it's correct.

  @param[in]  *DeviceTable          SIO_DEVICE_LIST_TABLE from PCD structure.
  @param[in]  *IndexPoart           SIO config port.

  @retval     IndexPort             return value as zero, it means "not match" .                                 return value is not zero, it means "match"
**/
VOID
CheckConfig (
  IN SIO_DEVICE_LIST_TABLE      *DeviceTable,
  IN UINT16                     *Port
  )
{
  UINT16                IndexPort;
  UINT16                DataPort;
  UINT8                 Index;

  IndexPort = UpsideDown (DeviceTable->DeviceBaseAdr);
  DataPort  = IndexPort + 1;

  if (IndexPort != 0) {
    //
    // Enter Config Mode
    //
    EnterConfigMode (IndexPort);
  
    IoWrite8 (IndexPort, SIO_CHIP_ID1);
    if (IoRead8 (DataPort) != DeviceTable->TypeH) {
      IndexPort = 0;
    }

    if ((DeviceTable->TypeL != NONE_ID) && (IndexPort != 0)) {
      IoWrite8 (IndexPort, SIO_CR21);
      if (IoRead8 (DataPort) != DeviceTable->TypeL) {
        IndexPort = 0;
      }
    }
  }  

  if (IndexPort == 0) {
    
//[-start-140116-IB12970054-modify]//
    for (Index = 0; Index < mSioCfgPortListSize; Index++) {
//[-end-140116-IB12970054-modify]//

      IndexPort = mSioCfgPortList[Index];
      DataPort  = IndexPort + 1;
      
      //
      // Enter Config Mode
      //
      EnterConfigMode (IndexPort);
  
      IoWrite8 (IndexPort, SIO_CHIP_ID1);
      if (IoRead8 (DataPort) == DeviceTable->TypeH) {
        if (DeviceTable->TypeL != NONE_ID) {
          IoWrite8 (IndexPort, SIO_CR21);
          if (IoRead8 (DataPort) == DeviceTable->TypeL) {
            break;
          }
        } else {
          break;
        }
      }
    }

//[-start-140116-IB12970054-modify]//
    if (Index == mSioCfgPortListSize) {  // if true, means not found SIO ID
//[-end-140116-IB12970054-modify]//

      IndexPort = 0;
    }
  }

  *Port = IndexPort;
}
