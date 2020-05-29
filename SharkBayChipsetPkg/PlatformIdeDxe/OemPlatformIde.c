/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "OemPlatformIde.h"

/**

    This is the OEM hook routine. It may be called twice in the
    begin and end of PlatformIdeCheckController.

    @param    Flag     IN,  TRUE : This function.
                                     FALSE: Function call after chipset operation.
                             OUT, TRUE : Skip all the operations after first time call this OEM hook routine.
                                     FALSE: Continue all the operations.	 

    @retval     
 
**/
EFI_STATUS
OemPlatformIdeCheckController (
  IN      EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN      EFI_HANDLE                        *Controller,
  IN OUT  BOOLEAN                           *Flag
)
{
  BOOLEAN             OP1;

  OP1 = *Flag;

  //True  : Skip all the operations after Operation 1.
  //False : Continue all the operations after Operation 1.
  *Flag = FALSE;

  if (OP1)
  {
    //Operation 1
    //Add OEM operation before chipset operation here.
  } else {

    //Operation 2
    //If Flag = FALSE all the operation here will be skip.
    //Add OEM operation after chipset operation here.
  }

  return EFI_SUCCESS;
}

/**

    This is the OEM hook routine. It may be called twice in the
    begin and end of PlatformIdeCheckController.

    @param    Flag     IN,  TRUE : This function.
                                     FALSE: Function call after chipset operation.
                             OUT, TRUE : Skip all the operations after first time call this OEM hook routine.
                                     FALSE: Continue all the operations.	 

    @retval     
 
**/
EFI_STATUS
OemPlatformIdeCalculateMode (
  IN      EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN      UINT8                             Channel,
  IN      UINT8                             Device,
  IN OUT  EFI_ATA_COLLECTIVE_MODE           *SupportedModes,
  IN OUT  BOOLEAN                           *Flag
  )
{
  BOOLEAN             OP1;

  OP1 = *Flag;

  //True  : Skip all the operations after Operation 1.
  //False : Continue all the operations after Operation 1.
  *Flag = FALSE;

  if (OP1)
  {
    //Operation 1
    //Add OEM operation before chipset operation here.
  } else {

    //Operation 2
    //If Flag = FALSE all the operation here will be skip.
    //Add OEM operation after chipset operation here.
  }

  return EFI_SUCCESS;
}

/**

    This is the OEM hook routine. It may be called twice in the
    begin and end of PlatformIdeCheckController.

    @param    Flag     IN,  TRUE : This function.
                                     FALSE: Function call after chipset operation.
                             OUT, TRUE : Skip all the operations after first time call this OEM hook routine.
                                     FALSE: Continue all the operations.	 

    @retval     
 
**/
EFI_STATUS
OemDetectCableType (
  IN  EFI_PCI_IO_PROTOCOL               *PciIo,
  IN  CHIPSET_CONFIGURATION              *SetupVariable,
  IN  UINT8                             Channel,
  IN OUT  BOOLEAN                       *Flag
  )
{
  BOOLEAN             OP1;

  OP1 = *Flag;

  //True  : Skip all the operations after Operation 1.
  //False : Continue all the operations after Operation 1.
  *Flag = FALSE;

  if (OP1)
  {
    //Operation 1
    //Add OEM operation before chipset operation here.
  } else {

    //Operation 2
    //If Flag = FALSE all the operation here will be skip.
    //Add OEM operation after chipset operation here.
  }

  return EFI_SUCCESS;
}
