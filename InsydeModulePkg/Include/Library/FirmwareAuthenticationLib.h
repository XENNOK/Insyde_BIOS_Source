//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   FirmwareAuthenticationLib.h
//;
//; Abstract:
//;
//;   Firmware authentication common routine
//;
#ifndef _FIRMWARE_AUTHENTICATION_LIB_H_
#define _FIRMWARE_AUTHENTICATION_LIB_H_

#include <Uefi.h>

/**
  Firmware file authentication

  @param[in] FirmwareFileData    Firmware binary data 
  @param[in] FirmwareFileSize    Size in bytes of firmware binary file

  @retval EFI_SUCCESS            Firmware file is successfuly authentication
  @retval EFI_SECURITY_VIOLATION Firmware authentication process failed

**/
EFI_STATUS 
VerifyFirmware (
  IN UINT8                      *FirmwareFileData,
  IN UINTN                      FirmwareFileSize
  );
#endif

