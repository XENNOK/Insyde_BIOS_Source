/** @file

Declaration file for SioMainPei.c

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

#ifndef _SIO_NCT6683DLU_PEI_H_
#define _SIO_NCT6683DLU_PEI_H_

#include <PostCode.h>
#include <IndustryStandard/Pci22.h>
#include <Include/SioNct6683dluReg.h>
#include <Ppi/SioInitialized.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/IoLib.h>
#include <Library/SioLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PostCodeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Sio/SioCommon.h>
#include <Guid/SioHiiResourceForm.h>

/**
 Enter SIO mode.

 @param[in]   ConfigPort             SIO config port.
**/
VOID
EnterConfigMode (
  IN UINT16  ConfigPort
  );

/**
 Exit SIO mode.

 @param[in]   ConfigPort             SIO config port.
**/
VOID
ExitConfigMode (
  IN UINT16  ConfigPort
  );

/**
  Decode IO Port By Different Platform, Like AMD¡BINTEL¡K 

  @retval     EFI_SUCCESS          The operation performed successfully.
**/
EFI_STATUS
BeforeSioInit (
  VOID
  );

/**
  After Init SIO, You Need To Check, or Change Some Value.

  @param[in]   IndexPort            This SIO config port.

  @retval      EFI_SUCCESS          The operation performed successfully.
**/
EFI_STATUS
AfterSioInit (
  IN UINT16               IndexPort
  );

/**
  To get correct SIO data  

  @param[out]  *SioInstance         if success, return which SIO instance pointer.

  @retval     IndexPort             return value as zero, it means "not match"
                                    return value is not zero, it means "match".                                    
**/
UINT16
CheckDevice ( 
  UINT8               *SioInstance
  ); 

/**
  Check ID of SIO whether it's correct.

  @param[in]  *DeviceTable          SIO_DEVICE_LIST_TABLE from PCD structure.
  @param[in]  *IndexPoart           SIO config port.

  @retval     IndexPort             return value as zero, it means "not match" .                                 return value is not zero, it means "match"
**/
VOID
CheckConfig (
  IN SIO_DEVICE_LIST_TABLE      *DeviceTable,
  IN UINT16                     *IndexPoart
  ); 

#endif