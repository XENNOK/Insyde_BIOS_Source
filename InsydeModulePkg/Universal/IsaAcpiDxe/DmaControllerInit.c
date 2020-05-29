/** @file

DMA contorller init

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

#include "DmaControllerInit.h"

UINTN NumPnp200=1;

/**
  brief-description of function. 

  extended description of function.  
  
**/
VOID DmaControllerInit (
    VOID
  )
{
  EFI_STATUS                     Status;	
  EFI_CPU_IO2_PROTOCOL           *CpuIo;
  UINTN                          Index;
  UINT8                          Value8;


  Status = gBS->LocateProtocol (&gEfiCpuIo2ProtocolGuid, NULL, (VOID **)&CpuIo);
  
  if (!EFI_ERROR (Status)) {    
    //
    // DMA controller initialize
    //
    Value8 = 0;
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_CLEAR_POINTER, 1, &Value8);
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_COMMAND_REG, 1, &Value8);
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA1_03_CLEAR_POINTER, 1, &Value8);
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA1_03_COMMAND_REG, 1, &Value8);
    
    Value8 = CASCADE_MODE | ADDR_INCREMENT_SELECT | AUTO_INITIALISATION_DISABLE | VERIFY_OPERATION | CHANNEL_4_SELECT;
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_MODE_REG, 1, &Value8);
    
    Value8 = 0;    
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_WRITE_REQUEST, 1, &Value8); 
    CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_MASK_REG, 1, &Value8); 
    
    for (Index=0; Index<=3; Index++) {
      Value8 = (UINT8)(Index + 0x40);
      CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA1_03_MODE_REG, 1, &Value8);
      Value8 = (UINT8)Index;
      CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA1_03_WRITE_REQUEST, 1, &Value8);
      CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA1_03_MASK_REG, 1, &Value8); 
    }
    
    for (Index=1; Index<=3; Index++) {
      Value8 = (UINT8)(Index + 0x40);
      CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_MODE_REG, 1, &Value8);
      Value8 = (UINT8)Index;
      CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_WRITE_REQUEST, 1, &Value8);
      CpuIo->Io.Write (CpuIo, EfiCpuIoWidthUint8, DMA2_47_MASK_REG, 1, &Value8);    
    }           
  }
}
