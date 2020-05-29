/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "KeyboardInit.h"


EFI_PEI_NOTIFY_DESCRIPTOR BootModeNotifyList[] = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  BootModePpiNotifyCallback
};

EFI_PEI_NOTIFY_DESCRIPTOR EndOfPeiSignalNotifyList[] =
{
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  EndOfPeiPpiNotifyCallback
};

/**
 Register notify ppi to do keyboard initialize in PEI phase

 @param [in]   FileHandle
 @param [in]   PeiServices      The PEI core services table.

 @retval EFI_SUCCESS            Routine register notify ppi successfully.
 @return Other        Error occurs while notifying PPI.

**/
EFI_STATUS
EFIAPI
KeyboardInitEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  return (**PeiServices).NotifyPpi (PeiServices, &BootModeNotifyList[0]);
}


/**
 Wait for input buffer empty

 @param [in]   CommandState     the Io to read.

 @retval EFI_SUCCESS            input buffer full.

**/
EFI_STATUS
WaitKbcIbe (
  IN UINT16              CommandState
  )
{
  UINT8                 KbdCmdState = 0;
  UINTN                 Index;

  for (Index = 0; Index < KBC_TIME_OUT; Index++) {
    KbdCmdState = IoRead8 (CommandState);
    if (!(KbdCmdState & KEY_IBF)) {
      return EFI_SUCCESS;
    } else{
      MicroSecondDelay(15);
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
 Wait for output buffer full

 @param [in]   CommandState     the Io to read.

 @retval EFI_SUCCESS            input buffer full.

**/
EFI_STATUS
WaitKbcObf (
  IN UINT16              CommandState
)
{
  UINT8                 KbdCmdState = 0;
  UINTN                 Index;

  for (Index = 0; Index < KBC_TIME_OUT; Index++) {
    KbdCmdState = IoRead8 (CommandState);
    if (KbdCmdState & KEY_OBF) {
      return EFI_SUCCESS;
    } else{
      MicroSecondDelay(15);
    }
  }

  return EFI_DEVICE_ERROR;
}

/**
 once boot mode has been updated, send KBC rest command

 @param [in]   PeiServices      General purpose services available to every PEIM.
 @param [in]   NotifyDescriptor
 @param [in]   Ppi

 @retval EFI_SUCCESS            Send KBC rest command successfully.
 @retval EFI_UNSUPPORTED        KBC isn't exist or bootmode is

**/
EFI_STATUS
EFIAPI
BootModePpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  UINT8                         Data;
  EFI_STATUS                    Status;
  EFI_BOOT_MODE                 BootMode;

  (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES**)PeiServices, &BootMode);

  Status = EFI_UNSUPPORTED;
  if (BootMode != BOOT_ON_S3_RESUME) {
    Data = IoRead8 (KEY_CMD_STATE);
    //
    //Check KBC is exist
    //
    if (Data != 0xFF) {
      Status = WaitKbcIbe (KEY_CMD_STATE);
      //
      //Send keyboard reset command then read ACK
      //
      if (!EFI_ERROR (Status)) {
        IoWrite8 (KEY_DATA, 0xFF);
        (*PeiServices)->NotifyPpi ((CONST EFI_PEI_SERVICES**)PeiServices, &EndOfPeiSignalNotifyList[0]);
      }
    }
  }
  return Status;
}

/**
 Register notify ppi to do keyboard initialize in PEI phase

 @param [in]   PeiServices      General purpose services available to every PEIM.
 @param [in]   NotifyDescriptor
 @param [in]   Ppi

 @retval EFI_SUCCESS            BAT test OK.
 @return other        BAT test fail

**/
EFI_STATUS
EFIAPI
EndOfPeiPpiNotifyCallback (
  IN EFI_PEI_SERVICES                     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR            *NotifyDescriptor,
  IN VOID                                 *Ppi
  )
{
  UINT8                       Data;
  EFI_STATUS                  Status;

  Status = WaitKbcObf (KEY_CMD_STATE);
  //
  //Wait for ACK
  //
  if (!EFI_ERROR (Status)) {
    Data = IoRead8 (KEY_DATA);
    BuildGuidDataHob ((CONST EFI_GUID *)&gKbcInitializedGuid, NULL, 0);
  }

  return Status;
}