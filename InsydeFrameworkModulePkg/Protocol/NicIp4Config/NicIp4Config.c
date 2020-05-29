//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (NicIp4Config)

EFI_GUID gEfiNicIp4ConfigProtocolGuid = EFI_NIC_IP4_CONFIG_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiNicIp4ConfigProtocolGuid, "NicIP4Config Protocol", "NicIP4Config Protocol");

EFI_GUID gEfiNicIp4ConfigVariableGuid  = EFI_NIC_IP4_CONFIG_VARIABLE_GUID;

EFI_GUID_STRING(&gEfiNicIp4ConfigVariableGuid, "Ip4 Static Config", "Ip4 Configuration Data");
