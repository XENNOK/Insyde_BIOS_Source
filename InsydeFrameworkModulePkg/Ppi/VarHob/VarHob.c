//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;   Boot Mode PPI GUID as defined in PEI EAS
//;
#include "Tiano.h"
#include EFI_PPI_DEFINITION(VarHob)

EFI_GUID gPeiVarHobPpiGuid = PEI_VARHOB_PEIM_PPI;

EFI_GUID_STRING (&gPeiVarHobPpiGuid, "VarHob", "VarHob PPI");
