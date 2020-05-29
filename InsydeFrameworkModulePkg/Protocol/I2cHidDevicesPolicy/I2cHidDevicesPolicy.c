//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    I2cHidPolicy.c

Abstract:

    The GUID definition of the protocol

--*/

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (I2cHidDevicesPolicy)

EFI_GUID gEfiI2cHidDevicesPolicyProtocolGuid = EFI_I2C_HID_DEVICES_POLICY_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiI2cHidDevicesPolicyProtocolGuid, "EFI I2C HID devices policy protocol", "EFI I2C HID DEVICES POLICY Protocol");

