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
//;   Frequency Control PPI defined in EFI 2.0
//;


#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION(FrequencyControl)

EFI_GUID gPeiFrequencyControlPpiGuid = PEI_FREQUENCY_CONTROL_PPI_GUID;

EFI_GUID_STRING (&gPeiFrequencyControlPpiGuid, "FREQUENCY CONTROL", "FREQUENCY CONTROL");
