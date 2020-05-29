//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*--
Module Name:

  H2OIpmiSelInfoProtocol.c

Abstract:

  H2O IPMI SelInfo Protocol source file

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (H2OIpmiSelInfoProtocol)

EFI_GUID  gH2OIpmiSelInfoProtocolGuid = H2O_IPMI_SEL_INFO_PROTOCOL_GUID;

EFI_GUID_STRING (&gH2OIpmiSelInfoProtocolGuid, "H2O IPMI SEL Info", "SEL Information Parser");