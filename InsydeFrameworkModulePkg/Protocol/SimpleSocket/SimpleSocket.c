//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Efi.h"
#include EFI_PROTOCOL_DEFINITION (SimpleSocket)


EFI_GUID gSimpleSocketProtocolGuid = SIMPLE_SOCKET_PROTOCOL_GUID;

EFI_GUID_STRING(&gSimpleSocketProtocolGuid, "Simple Socket Protocol", "Simple Socket Protocol");


