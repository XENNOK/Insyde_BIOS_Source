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

  //====== Default End ======//
  {0x00                  , 0x00                     }
};

CHAR16 *mSioVariableName = (CHAR16*) PcdGetPtr (PcdSioDummyCfg);

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
  EFI_SIO_TABLE                  *OemTable;

  OemTable    = NULL;

  PostCode (PEI_SIO_INIT); //PostCode = 0x70, Super I/O initial
  
  //
  // OemService
  //
  Status = OemSvcBeforeInitSioPei ();
  if (Status == EFI_UNSUPPORTED) {
    BeforeSioInit ();
  }

  // Get correct config port, if it's wrong, return fail.
  Status = CheckDevice (); 
  if ( EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Install PPI
  //
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
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN UINT16                    IndexPort
  )
{
  
  return EFI_SUCCESS;
}

/**
  To get correct SIO data.

  @retval EFI_SUCCESS           Found SIO data. 
  @retval EFI_NOT_FOUND         Not found.                                    
**/
EFI_STATUS
CheckDevice (
  VOID
  ) 
{
  SIO_DEVICE_LIST_TABLE              *mTablePtr;
  UINT16                             IndexPort;

  IndexPort = 0;
  mTablePtr  = (SIO_DEVICE_LIST_TABLE *)PcdGetPtr (PcdSioDummyCfg);

  //
  // Calculate the number of non-zero entries in the table
  //
  while (!((mTablePtr->TypeH == NONE_ID) && (mTablePtr->TypeL == NONE_ID))) {
    if ((mTablePtr->TypeH == SIO_ID1) && (mTablePtr->Device == COM) && (mTablePtr->DeviceEnable == TRUE)) {
      return EFI_SUCCESS;          
    }
    mTablePtr++;
  } 
  return EFI_NOT_FOUND;
}
