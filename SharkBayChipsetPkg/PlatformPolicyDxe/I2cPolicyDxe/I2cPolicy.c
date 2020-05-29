//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

//[-start-130102-IB05280028-modify]//
#include <Protocol/I2cHidDevicesPolicy.h>
#include <Library/DxeOemSvcChipsetLib.h>
#include <Library/PcdLib.h>
//[-end-130102-IB05280028-modify]//
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

EFI_STATUS
I2cPolicyEntry (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                             Status;
  EFI_HANDLE                             Handle;
  EFI_I2C_HID_DEVICES_POLICY_PROTOCOL    *I2cHidPolicy;
  
  DEBUG (( EFI_D_INFO | EFI_D_ERROR, "\nI2C BusPolicy Entry\n" ));

  I2cHidPolicy = AllocateZeroPool(sizeof(EFI_I2C_HID_DEVICES_POLICY_PROTOCOL));

  //
  // Bit  0 ~  6 : Slave address
  // Bit  7      : Interrupt GPIO pin active level, 0 = low active, 1 = high active
  // Bit  8 ~ 15 : Interrupt GPIO pin number
  // Bit 16 ~ 31 : HID descriptor register number
  // Bit 32 ~ 47 : Device type, 
  //               0x0000 -> (AutoDetect)
  //               0x0d00 -> Touch panel
  //               0x0102 -> Mouse
  //               0x0106 -> Keyboard
  // Bit 48 ~ 51 : Host controller number 
  //               0x00      -> (AutoDetect)
  //               0x01~0x0f -> I2C host controller 0~14 (One based)
  // Bit 52 ~ 55 : Device Speed Override
  //               0x01      -> V_SPEED_STANDARD
  //               0x02      -> V_SPEED_FAST  
  //               0x03      -> V_SPEED_HIGH    
  // 
//[-start-130102-IB05280028-modify]//
  I2cHidPolicy->HidDevicesCfg[0] = PcdGet64 (PcdI2cHidDevice01);
  I2cHidPolicy->HidDevicesCfg[1] = PcdGet64 (PcdI2cHidDevice02);
  I2cHidPolicy->HidDevicesCfg[2] = PcdGet64 (PcdI2cHidDevice03);
  I2cHidPolicy->HidDevicesCfg[3] = PcdGet64 (PcdI2cHidDevice04);
  I2cHidPolicy->HidDevicesCfg[4] = PcdGet64 (PcdI2cHidDevice05);
  I2cHidPolicy->HidDevicesCfg[5] = PcdGet64 (PcdI2cHidDevice06);
  I2cHidPolicy->HidDevicesCfg[6] = PcdGet64 (PcdI2cHidDevice07);
  I2cHidPolicy->HidDevicesCfg[7] = PcdGet64 (PcdI2cHidDevice08);

//[-start-130524-IB05160451-modify]//
  Status = OemSvcUpdateI2cPlatformPolicy (I2cHidPolicy);
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "DexOemSvcChipsetLib OemSvcUpdateI2cPlatformPolicy, Status : %r\n", Status));
//[-end-130524-IB05160451-modify]//
  if (Status == EFI_SUCCESS) {
    return Status;
  }
//[-end-130102-IB05280028-modify]//

  //
  // Install the SMBUS interface
  //
  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiI2cHidDevicesPolicyProtocolGuid,
                  I2cHidPolicy,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}
