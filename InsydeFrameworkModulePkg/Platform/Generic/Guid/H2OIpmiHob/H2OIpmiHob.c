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

  H2OIpmiHob.c

Abstract:

  H2O IPMI HOB source file

--*/

#include "Tiano.h"
#include EFI_GUID_DEFINITION (H2OIpmiHob)

EFI_GUID  gH2OIpmiHobGuid = H2O_IPMI_HOB_GUID;

EFI_GUID_STRING (&gH2OIpmiHobGuid, "H2O Ipmi Hob Info", "H2O Ipmi Hob Guid");
