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
//;
//;   CPU Frequency Status GUID.
//;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (CpuFrequency)

EFI_GUID gEfiCpuFrequencyGuid = PEI_IN_CPU_FREQUENCY_GUID;

EFI_GUID_STRING (&gPeiInMemoryGuid, "PeiInCpuFrequency", "PEIM IN CPU Frequency");