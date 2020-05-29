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

#ifndef _SIO_F71808A_PEI_H_
#define _SIO_F71808A_PEI_H_

#include <PostCode.h>
#include <IndustryStandard/Pci22.h>
#include <Include/SioDummyReg.h>
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
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN UINT16                   IndexPort
  );

/**
  To get correct SIO data  

  @retval EFI_SUCCESS           Found SIO data. 
  @retval EFI_NOT_FOUND         Not found.                                     
**/
EFI_STATUS
CheckDevice ( 
  VOID
  ); 

#endif

