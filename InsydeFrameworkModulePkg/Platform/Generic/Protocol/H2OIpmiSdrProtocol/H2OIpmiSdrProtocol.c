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

  H2OIpmiSdrProtocol.c

Abstract:

  H2O IPMI SDR Protocol source file

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (H2OIpmiSdrProtocol)

EFI_GUID  gH2OIpmiSdrProtocolGuid = H2O_IPMI_SDR_PROTOCOL_GUID;

EFI_GUID_STRING (&gH2OIpmiSdrProtocolGuid, "H2O Ipmi SDR", "H2O Ipmi Sync Protocol GUID");

