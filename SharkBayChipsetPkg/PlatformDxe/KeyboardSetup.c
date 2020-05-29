/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file:

  KeyboardSetup.c

@brief:

  Platform Initialization for keyboard setup.


--*/

#include <Platform.h>
#include <Protocol/IsaIo.h>
#include <Guid/GlobalVariable.h>

#define KBC_DATA_PORT       0x60
#define KBC_CMD_STS_PORT    0x64


BOOLEAN           Ps2PortSwapDone = FALSE;


/**

  Read Keyboard Data Register

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    

  @retval Data Register 
  
**/
UINT8
KeyReadDataRegister (
  IN  EFI_ISA_IO_PROTOCOL    *IsaIo
  )
{
  UINT8  Data;

  IsaIo->Io.Read (
              IsaIo,
              EfiIsaIoWidthUint8,
              KBC_DATA_PORT,
              1,
              &Data
              );

  return Data;
}

/**

  Read Keyboard Status Register

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    

  @retval Status Register 
  
**/
UINT8
KeyReadStatusRegister (
  IN EFI_ISA_IO_PROTOCOL    *IsaIo
  )
{
  UINT8  Data;

  IsaIo->Io.Read (
              IsaIo,
              EfiIsaIoWidthUint8,
              KBC_CMD_STS_PORT,
              1,
              &Data
              );

  return  Data;
}

/**

  Write Command Register

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    
  @param  Data                  Data to write

  @retval None 
  
**/
VOID
KeyWriteCommandRegister (
  IN EFI_ISA_IO_PROTOCOL    *IsaIo,
  IN UINT8                  Data
  )
{
  IsaIo->Io.Write (
              IsaIo,
              EfiIsaIoWidthUint8,
              KBC_CMD_STS_PORT,
              1,
              &Data
              );
  return;
}

/**

  Write Data Register

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    
  @param  Data                  Data to write

  @retval None 
  
**/
VOID
KeyWriteDataRegister (
  IN EFI_ISA_IO_PROTOCOL    *IsaIo,
  IN UINT8                  Data
  )
{
  IsaIo->Io.Write (
              IsaIo,
              EfiIsaIoWidthUint8,
              KBC_DATA_PORT,
              1,
              &Data
              );
  return;
}

/**

  Read Keyboard Data

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    
  @param  Data                  Pointer to where the data to read into
  
  @retval                       Put the data to inside the pointer
  
**/
VOID
KeyboardRead (
  IN  EFI_ISA_IO_PROTOCOL    *IsaIo,
  OUT UINT8                  *Data
  )
{
  UINT32   TimeOut;

  TimeOut = 0;

  //
  //wait till output buffer full then perform the read
  //
  for ( TimeOut = 0; TimeOut < 1000000; TimeOut += 30) {
    if (KeyReadStatusRegister (IsaIo) & 0x01) {
      *Data = KeyReadDataRegister (IsaIo);
      break;
    }
    gBS->Stall (30);
  }

  return;
}

/**

  Write Keyboard Data

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    
  @param  Data                  Data to write
  
  @retval None
  
**/
VOID
KeyboardWrite (
  IN EFI_ISA_IO_PROTOCOL    *IsaIo,
  IN UINT8                  Data
  )
{
  UINT32      TimeOut;
  UINT32      RegEmptied;

  TimeOut = 0;
  RegEmptied = 0;

  //
  //wait for input buffer empty
  //
  for ( TimeOut = 0; TimeOut < 65536; TimeOut += 30) {
    if (!(KeyReadStatusRegister (IsaIo) & 0x02)) {
      break;
    }
    gBS->Stall (30);
  }

  //
  //Write it
  //
  KeyWriteDataRegister (IsaIo, Data);

  return;
}

/**

  Write Keyboard Command

  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    
  @param  Data                  Data to write
  
  @retval None
  
**/
VOID
KeyboardCommand  (
  IN EFI_ISA_IO_PROTOCOL     *IsaIo,
  IN UINT8                   Data
  )
{
  UINT32      TimeOut;

  TimeOut = 0;

  //
  //Wait For Input Buffer Empty
  //
  for ( TimeOut = 0; TimeOut < 65536; TimeOut += 30) {
    if (!(KeyReadStatusRegister (IsaIo) & 0x02)) {
      break;
    }
    gBS->Stall (30);
  }

  //
  //issue the command
  //
  KeyWriteCommandRegister (IsaIo, Data);

  //
  //Wait For Input Buffer Empty again
  //
  for ( TimeOut = 0; TimeOut < 65536; TimeOut += 30) {
    if (!(KeyReadStatusRegister (IsaIo) & 0x02)) {
      break;
    }
    gBS->Stall (30);
  }

  return;
}

/**

  Swap Ps2 devices
   
  @param  IsaIo                 EFI_ISA_IO_PROTOCOL pointer    
  
  @retval None
  
**/
VOID
SwapPs2Command(
  IN  EFI_ISA_IO_PROTOCOL    *IsaIo
  )
{
  UINT8             Data;
  //
  // Swap it and then reset
  //
  KeyboardCommand (IsaIo ,0xc9);
  KeyboardCommand (IsaIo, 0x60);
  KeyboardWrite (IsaIo, 0xff);
  KeyboardRead (IsaIo, &Data);

  return;
}

/**

  Detect which device is connected to which port.

  @param  Handle    
  
  @retval EFI_SUCCESS
  
**/
EFI_STATUS
Ps2PortSwap (
  IN  EFI_HANDLE  Handle
  )
{
  UINT8                  Data;
  UINT8                  Index;
  EFI_STATUS             Status;
  UINTN                  Size;
  BOOLEAN                Ps2PortSwap;
  BOOLEAN                Ps2PortSwapVariable;
  EFI_ISA_IO_PROTOCOL    *IsaIo;

  if (Ps2PortSwapDone) {
    return  EFI_SUCCESS;
  }



  IsaIo = NULL;
  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiIsaIoProtocolGuid,
                  (VOID **)&IsaIo,
                  NULL,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //Keyboard reset
  //
  Ps2PortSwap = FALSE;

  KeyboardWrite (IsaIo, 0xff);
  KeyboardRead (IsaIo, &Data);
  if (Data == 0xfa) {
    //
    // Read rest of the bytes
    //
    for (Index = 0; Index < 3; Index++) {
      KeyboardRead (IsaIo, &Data);
    }

    //
    // Check if mouse
    //
    KeyboardWrite (IsaIo ,0xe6);
    KeyboardRead (IsaIo, &Data);

    if (Data == 0xfa) {
      Ps2PortSwap = TRUE;
      SwapPs2Command (IsaIo);
    }
  } else {
    //
    // Send Reset Aux
    //
    KeyboardCommand (IsaIo, 0xd4);
    KeyboardWrite (IsaIo, 0xff);
    KeyboardRead (IsaIo, &Data);
    if (Data != 0xfa) {
      //
      // No PS/2 device
      //
    } else {
      //
      // Read rest of the bytes
      //
      for (Index = 0; Index < 3; Index++) {
        KeyboardRead (IsaIo, &Data);
      }

      //
      // Check if Mouse but send to AUX
      //
      KeyboardCommand (IsaIo, 0xd4);
      KeyboardWrite (IsaIo, 0xe6);
      KeyboardRead (IsaIo, &Data);
      if (Data != 0xfa) {
        Ps2PortSwap = TRUE;
        SwapPs2Command (IsaIo);
      }
    }
  }

  Size = sizeof (BOOLEAN);
  Status = gRT->GetVariable (
                  L"Ps2Swap",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &Size,
                  &Ps2PortSwapVariable
                  );

  if (Status == EFI_NOT_FOUND) {
      Size = sizeof (BOOLEAN);
      gRT->SetVariable (
             L"Ps2Swap",
             &gEfiGlobalVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_NON_VOLATILE,
             Size,
             &Ps2PortSwap
             );
  } else {
    if (Ps2PortSwap != Ps2PortSwapVariable) {
      Size = sizeof (BOOLEAN);
      gRT->SetVariable (
             L"Ps2Swap",
             &gEfiGlobalVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_NON_VOLATILE,
             Size,
             &Ps2PortSwap
             );
    }
}

  //
  // Do the PS2 port swapping code only once
  //
  Ps2PortSwapDone = TRUE;

  return  EFI_SUCCESS;
}

/**

  Detect which device is connected to which port.

  @param  Handle    
  
  @retval EFI_SUCCESS
  
**/
EFI_STATUS
Ps2SkipHWInit (
  IN  EFI_HANDLE  Handle
  )
{
  return EFI_SUCCESS;
}
