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
//; Module Name:
//;
//;   DataHubRecordPolicy.c
//;
//; Abstract:
//;   Protocol used for SMBIOS Policy definition.
//;
//;
#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION(DataHubRecordPolicy)

EFI_GUID gEfiDataHubRecordPolicyGuid = EFI_DATAHUB_RECORD_POLICY_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiDataHubRecordPolicyGuid, "DataHubRecordPolicy Protocol", "by SMBIOS Type Option Policy");
