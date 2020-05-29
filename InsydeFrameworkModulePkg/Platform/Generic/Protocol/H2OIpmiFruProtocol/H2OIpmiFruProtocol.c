//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

    H2OIpmiFruProtocol.c

Abstract:

  H2OIpmiFru Protocol

--*/


#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (H2OIpmiFruProtocol)

EFI_GUID  gH2OIpmiFruProtocolGuid = H2O_IPMI_FRU_PROTOCOL_GUID;

EFI_GUID_STRING(&gH2OIpmiFruProtocolGuid, "H2OIpmiFruProtocol", "H2O IPMI FRU Protocol");


