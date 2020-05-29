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

  StartOfBdsDiagnostics.c

Abstract:

  This protocol will be installed at the start of PlatformBdsDiagnostics

--*/

#include <Tiano.h>

#include EFI_PROTOCOL_DEFINITION(StartOfBdsDiagnostics)

EFI_GUID gEfiStartOfBdsDiagnosticsProtocolGuid = EFI_START_OF_BDS_DIAGNOSTICS_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiStartOfBdsDiagnosticsProtocolGuid, "StartOfBdsDiagnostics", "Start Of BdsDiagnostics Protocol");
