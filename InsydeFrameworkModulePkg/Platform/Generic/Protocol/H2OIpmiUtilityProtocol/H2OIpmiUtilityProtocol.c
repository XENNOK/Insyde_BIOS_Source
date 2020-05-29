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

  H2OIpmiUtilityProtocol.c

Abstract:

  H2OIpmiUtilityProtocol source file.

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION(H2OIpmiUtilityProtocol)

EFI_GUID  gH2OIpmiUtilityProtocolGuid = H2O_IPMI_UTILITY_PROTOCOL_GUID;

EFI_GUID_STRING (&gH2OIpmiUtilityProtocolGuid, "H2O Ipmi Utility Protocol", "H2O Ipmi Utility Protocol GUID");

