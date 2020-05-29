/** @file
  Fill with table to program register in PEI stage.

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

#include <Library/PeiOemSvcKernelLib.h>

//
// Follow {offset, value} to create structure, table pointer will return DXE driver,
// then driver will auto to program.
//
EFI_SIO_TABLE CreateTable[] = {
  {0x00,     0x00}
};


/**
  Write register to SIO in PEI stage.

  @param[in] SioInstance       Which SIO pass in.
  @param[in, out] RegTable     Follow the table to program SIO.

  @retval EFI_UNSUPPORTED      Returns unsupported by default.
  @retval EFI_SUCCESS          The service is customized in the project.
  @retval EFI_MEDIA_CHANGED    The value of IN OUT parameter is changed. 
  @retval Others               Depends on customization.
**/
EFI_STATUS
OemSvcRegSioPei (
  IN UINT8                           SioInstance,
  IN OUT EFI_SIO_TABLE               **RegTable
  )
{
  /*++
    Todo:
      Add project specific code in here.
  --*/
  
  *RegTable = CreateTable;

  return EFI_UNSUPPORTED;
}
