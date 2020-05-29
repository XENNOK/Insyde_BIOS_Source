/** @file
  GenericBdsLib

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

/** 
  BDS Lib functions which contain all the code to connect console device

Copyright (c) 2004 - 2011, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include "InternalBdsLib.h"

/**
  Check if we need to save the EFI variable with "ConVarName" as name
  as NV type
  If ConVarName is NULL, then ASSERT().

  @param ConVarName The name of the EFI variable.

  @retval TRUE    Set the EFI variable as NV type.
  @retval FALSE   EFI variable as NV type can be set NonNV.
**/
BOOLEAN
IsNvNeed (
  IN CHAR16 *ConVarName
  )
{
  CHAR16 *Ptr;

  ASSERT (ConVarName != NULL);

  Ptr = ConVarName;

  //
  // If the variable includes "Dev" at last, we consider
  // it does not support NV attribute.
  //
  while (*Ptr != L'\0') {
    Ptr++;
  }

  if (((INTN)((UINTN)Ptr - (UINTN)ConVarName) / sizeof (CHAR16)) <= 3) {
    return TRUE;
  }

  if ((*(Ptr - 3) == 'D') && (*(Ptr - 2) == 'e') && (*(Ptr - 1) == 'v')) {
    return FALSE;
  } else {
    return TRUE;
  }
}

STATIC
BOOLEAN
IsConVariable (
  IN  CHAR16                    *ConVarName
  )
{
  CHAR16           *ConVarNames[6] = { L"ConIn",    L"ConOut",    L"ErrOut",
                                       L"ConInDev", L"ConOutDev", L"ErrOutDev"
                                     };
  UINTN            Index;
  BOOLEAN          VariableFound;


  VariableFound = FALSE;
  if (ConVarName != NULL) {
    for (Index = 0; Index < 6; Index++) {
      if (StrCmp (ConVarName, ConVarNames[Index]) == 0) {
       VariableFound = TRUE;
       break;
      }
    }
  }
  return VariableFound;
}


/**
  Fill console handle in System Table if there are no valid console handle in.

  Firstly, check the validation of console handle in System Table. If it is invalid,
  update it by the first console device handle from EFI console variable.

  @param  VarName            The name of the EFI console variable.
  @param  ConsoleGuid        Specified Console protocol GUID.
  @param  ConsoleHandle      On IN,  console handle in System Table to be checked.
                             On OUT, new console hanlde in system table.
  @param  ProtocolInterface  On IN,  console protocol on console handle in System Table to be checked.
                             On OUT, new console protocol on new console hanlde in system table.

  @retval TRUE               System Table has been updated.
  @retval FALSE              System Table hasn't been updated.

**/
BOOLEAN
UpdateSystemTableConsole (
  IN     CHAR16                          *VarName,
  IN     EFI_GUID                        *ConsoleGuid,
  IN OUT EFI_HANDLE                      *ConsoleHandle,
  IN OUT VOID                            **ProtocolInterface
  )
{
  EFI_STATUS                Status;
  UINTN                     DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *FullDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  VOID                      *Interface;
  EFI_HANDLE                NewHandle;

  ASSERT (VarName != NULL);
  ASSERT (ConsoleHandle != NULL);
  ASSERT (ConsoleGuid != NULL);
  ASSERT (ProtocolInterface != NULL);

  if (*ConsoleHandle != NULL) {
    Status = gBS->HandleProtocol (
                   *ConsoleHandle,
                   ConsoleGuid,
                   (VOID **)&Interface
                   );
    if (Status == EFI_SUCCESS && Interface == *ProtocolInterface) {
      //
      // If ConsoleHandle is valid and console protocol on this handle also
      // also matched, just return.
      //
      return FALSE;
    }
  }

  //
  // Get all possible consoles device path from EFI variable
  //
  VarConsole = BdsLibGetVariableAndSize (
                VarName,
                &gEfiGlobalVariableGuid,
                &DevicePathSize
                );
  if (VarConsole == NULL) {
    //
    // If there is no any console device, just return.
    //
    return FALSE;
  }

  FullDevicePath = VarConsole;

  do {
    //
    // Check every instance of the console variable
    //
    Instance  = GetNextDevicePathInstance (&VarConsole, &DevicePathSize);
    if (Instance == NULL) {
      FreePool (FullDevicePath);
      ASSERT (FALSE);
    }

    //
    // Find console device handle by device path instance
    //
    Status = gBS->LocateDevicePath (
                   ConsoleGuid,
                   &Instance,
                   &NewHandle
                   );
    if (!EFI_ERROR (Status)) {
      //
      // Get the console protocol on this console device handle
      //
      Status = gBS->HandleProtocol (
                     NewHandle,
                     ConsoleGuid,
                     (VOID **)&Interface
                     );
      if (!EFI_ERROR (Status)) {
        //
        // Update new console handle in System Table.
        //
        *ConsoleHandle     = NewHandle;
        *ProtocolInterface = Interface;
        return TRUE;
      }
    }

  } while (Instance != NULL);

  //
  // No any available console devcie found.
  //
  return FALSE;
}

/**
  This function update console variable based on ConVarName, it can
  add or remove one specific console device path from the variable

  @param  ConVarName               Console related variable name, ConIn, ConOut,
                                   ErrOut.
  @param  CustomizedConDevicePath  The console device path which will be added to
                                   the console variable ConVarName, this parameter
                                   can not be multi-instance.
  @param  ExclusiveDevicePath      The console device path which will be removed
                                   from the console variable ConVarName, this
                                   parameter can not be multi-instance.

  @retval EFI_UNSUPPORTED          The added device path is same to the removed one.
  @retval EFI_SUCCESS              Success add or remove the device path from  the
                                   console variable.

**/
EFI_STATUS
EFIAPI
BdsLibUpdateConsoleVariable (
  IN  CHAR16                    *ConVarName,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ExclusiveDevicePath
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole;
  UINTN                     DevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  UINT32                    Attributes;
  BOOLEAN                   IsConsoleVariable;

  VarConsole      = NULL;
  DevicePathSize  = 0;

  //
  // Notes: check the device path point, here should check
  // with compare memory
  //
  if (CustomizedConDevicePath == ExclusiveDevicePath) {
    return EFI_UNSUPPORTED;
  }

  IsConsoleVariable = IsConVariable (ConVarName);
  
  //
  // Delete the ExclusiveDevicePath from current default console
  //
  VarConsole = BdsLibGetVariableAndSize (
                ConVarName,
                IsConsoleVariable ? &gEfiGlobalVariableGuid : &gEfiGenericVariableGuid,
                &DevicePathSize
                );

  //
  // Initialize NewDevicePath
  //
  NewDevicePath  = VarConsole;

  //
  // If ExclusiveDevicePath is even the part of the instance in VarConsole, delete it.
  // In the end, NewDevicePath is the final device path.
  //
  if (ExclusiveDevicePath != NULL && VarConsole != NULL) {
    NewDevicePath = BdsLibDelPartMatchInstance (VarConsole, ExclusiveDevicePath);
  }
  //
  // Try to append customized device path to NewDevicePath.
  //
  if (CustomizedConDevicePath != NULL) {
    if (!BdsLibMatchDevicePaths (NewDevicePath, CustomizedConDevicePath)) {
      //
      // Check if there is part of CustomizedConDevicePath in NewDevicePath, delete it.
      //
      NewDevicePath = BdsLibDelPartMatchInstance (NewDevicePath, CustomizedConDevicePath);
      //
      // In the first check, the default console variable will be _ModuleEntryPoint,
      // just append current customized device path
      //
      TempNewDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (NewDevicePath, CustomizedConDevicePath);
      if (TempNewDevicePath != NULL) {
        FreePool(TempNewDevicePath);
      }
    }
  }

  //
  // The attribute for ConInDev, ConOutDev and ErrOutDev does not include NV.
  //
  if (IsNvNeed(ConVarName)) {
    //
    // ConVarName has NV attribute.
    //
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE;
  } else {
    //
    // ConVarName does not have NV attribute.
    //
    Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  }

  //
  // Finally, Update the variable of the default console by NewDevicePath
  //
  DevicePathSize = GetDevicePathSize (NewDevicePath);
  Status = gRT->SetVariable (
                  ConVarName,
                  IsConsoleVariable ? &gEfiGlobalVariableGuid : &gEfiGenericVariableGuid,
                  Attributes,
                  DevicePathSize,
                  NewDevicePath
                  );
  if ((DevicePathSize == 0) && (Status == EFI_NOT_FOUND)) {
    Status = EFI_SUCCESS;
  }
  ASSERT_EFI_ERROR (Status);

  if (VarConsole == NewDevicePath) {
    if (VarConsole != NULL) {
      FreePool(VarConsole);
    }
  } else {
    if (VarConsole != NULL) {
      FreePool(VarConsole);
    }
    if (NewDevicePath != NULL) {
      FreePool(NewDevicePath);
    }
  }

  return Status;
}


/**
  Connect the console device base on the variable ConVarName, if
  device path of the ConVarName is multi-instance device path, if
  anyone of the instances is connected success, then this function
  will return success.

  @param  ConVarName               Console related variable name, ConIn, ConOut,
                                   ErrOut.

  @retval EFI_NOT_FOUND            There is not any console devices connected
                                   success
  @retval EFI_SUCCESS              Success connect any one instance of the console
                                   device path base on the variable ConVarName.

**/
EFI_STATUS
EFIAPI
BdsLibConnectConsoleVariable (
  IN  CHAR16                 *ConVarName
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *StartDevicePath;
  UINTN                     VariableSize;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  EFI_DEVICE_PATH_PROTOCOL  *CopyOfDevicePath;
  UINTN                     Size;
  BOOLEAN                   DeviceExist;

  Status      = EFI_SUCCESS;
  DeviceExist = FALSE;

  //
  // Check if the console variable exist
  //
  StartDevicePath = BdsLibGetVariableAndSize (
                      ConVarName,
                      &gEfiGlobalVariableGuid,
                      &VariableSize
                      );
  if (StartDevicePath == NULL) {
    return EFI_UNSUPPORTED;
  }

  CopyOfDevicePath = StartDevicePath;
  do {
    //
    // Check every instance of the console variable
    //
    Instance  = GetNextDevicePathInstance (&CopyOfDevicePath, &Size);
    if (Instance == NULL) {
      FreePool (StartDevicePath);
      return EFI_UNSUPPORTED;
    }

    Next      = Instance;
    while (!IsDevicePathEndType (Next)) {
      Next = NextDevicePathNode (Next);
    }

    SetDevicePathEndNode (Next);

    //
    // Connect the USB console
    // USB console device path is a short-form device path that starts with the first element being a USB WWID or a USB Class device path
    //
    if ((DevicePathType (Instance) == MESSAGING_DEVICE_PATH) &&
        ((DevicePathSubType (Instance) == MSG_USB_CLASS_DP) || 
         (DevicePathSubType (Instance) == MSG_USB_WWID_DP))) {
      Status = BdsLibConnectUsbDevByShortFormDP (0xFF, Instance);
      if (!EFI_ERROR (Status)) {
        DeviceExist = TRUE;
      }
    } else {
      //
      // Connect the instance device path
      //
      Status = BdsLibConnectDevicePath (Instance);
      if (EFI_ERROR (Status)) {
        //
        // Delete the instance from the console varialbe
        //
        BdsLibUpdateConsoleVariable (ConVarName, NULL, Instance);
      } else {
        DeviceExist = TRUE;
      }
    }

    FreePool(Instance);
  } while (CopyOfDevicePath != NULL);

  FreePool (StartDevicePath);

  if (!DeviceExist) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  This function will search every simpletext device in current system,
  and make every simpletext device as pertantial console device.

**/
VOID
EFIAPI
BdsLibConnectAllConsoles (
  VOID
  )
{
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *ConDevicePath;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;

  Index         = 0;
  HandleCount   = 0;
  HandleBuffer  = NULL;
  ConDevicePath = NULL;

  //
  // Update all the console variables
  //
  gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiSimpleTextInProtocolGuid,
          NULL,
          &HandleCount,
          &HandleBuffer
          );

  for (Index = 0; Index < HandleCount; Index++) {
    gBS->HandleProtocol (
            HandleBuffer[Index],
            &gEfiDevicePathProtocolGuid,
            (VOID **) &ConDevicePath
            );
    BdsLibUpdateConsoleVariable (L"ConIn", ConDevicePath, NULL);
  }

  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
    HandleBuffer = NULL;
  }

  gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiSimpleTextOutProtocolGuid,
          NULL,
          &HandleCount,
          &HandleBuffer
          );
  for (Index = 0; Index < HandleCount; Index++) {
    gBS->HandleProtocol (
            HandleBuffer[Index],
            &gEfiDevicePathProtocolGuid,
            (VOID **) &ConDevicePath
            );
    BdsLibUpdateConsoleVariable (L"ConOut", ConDevicePath, NULL);
    BdsLibUpdateConsoleVariable (L"ErrOut", ConDevicePath, NULL);
  }

  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }

  //
  // Connect all console variables
  //
  BdsLibConnectAllDefaultConsoles ();
}

/**
  This function will connect console device base on the console
  device variable ConIn, ConOut and ErrOut.

  @retval EFI_SUCCESS              At least one of the ConIn and ConOut device have
                                   been connected success.
  @retval EFI_STATUS               Return the status of BdsLibConnectConsoleVariable ().

**/
EFI_STATUS
EFIAPI
BdsLibConnectAllDefaultConsoles (
  VOID
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   SystemTableUpdated;

#ifdef Q2LSERVICE_SUPPORT
  //
  // PostCode = 0x17, Video device initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_OUT);
  Status = BdsLibConnectConsoleVariable (L"ConOut");
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // PostCode = 0x16, Keyboard Controller, Keyboard and Moust initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_IN);
  Status = BdsLibConnectConsoleVariable (L"ConIn");
  if (EFI_ERROR (Status)) {
    return Status;
  }
#else
  //
  // PostCode = 0x16, Keyboard Controller, Keyboard and Moust initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_IN);
  //
  // Because possibly the platform is legacy free, in such case,
  // ConIn devices (Serial Port and PS2 Keyboard ) does not exist, so we need not check the status.
  //
  BdsLibConnectConsoleVariable (L"ConIn");

  //
  // PostCode = 0x17, Video device initial
  //
  POST_CODE (BDS_CONNECT_CONSOLE_OUT);
  Status = BdsLibConnectConsoleVariable (L"ConOut");
  if (EFI_ERROR (Status)) {
    return Status;
  }
#endif

  //
  // Special treat the err out device, becaues the null err out var is legal.
  //
  //
  // PostCode = 0x18, Error report device initial
  //
  POST_CODE (BDS_CONNECT_STD_ERR);
  BdsLibConnectConsoleVariable (L"ErrOut");

  SystemTableUpdated = FALSE;
  //
  // Fill console handles in System Table if no console device assignd.
  //
  if (UpdateSystemTableConsole (L"ConIn", &gEfiSimpleTextInProtocolGuid, &gST->ConsoleInHandle, (VOID **) &gST->ConIn)) {
    SystemTableUpdated = TRUE;
  }
  if (UpdateSystemTableConsole (L"ConOut", &gEfiSimpleTextOutProtocolGuid, &gST->ConsoleOutHandle, (VOID **) &gST->ConOut)) {
    SystemTableUpdated = TRUE;
  }
  if (UpdateSystemTableConsole (L"ErrOut", &gEfiSimpleTextOutProtocolGuid, &gST->StandardErrorHandle, (VOID **) &gST->StdErr)) {
    SystemTableUpdated = TRUE;
  }

  if (SystemTableUpdated) {
    //
    // Update the CRC32 in the EFI System Table header
    //
    gST->Hdr.CRC32 = 0;
    gBS->CalculateCrc32 (
          (UINT8 *) &gST->Hdr,
          gST->Hdr.HeaderSize,
          &gST->Hdr.CRC32
          );
  }

  return EFI_SUCCESS;
}
  
/**
  This function will change video resolution and text mode
  according to defined setup mode or defined boot mode  

  @param  IsSetupMode   Indicate mode is changed to setup mode or boot mode. 

  @retval  EFI_SUCCESS  Mode is changed successfully.
  @retval  Others             Mode failed to be changed.

**/
EFI_STATUS
EFIAPI
BdsLibSetConsoleMode (
  BOOLEAN  IsSetupMode
  )
{
   return EFI_UNSUPPORTED;
}

