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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  TcoWatchdog.c

Abstract:

  A PEIM of features that install and publish 3 PPI services:
  (1) Enable Tco Watchdog Timer
  (2) Disable Tco Watchdog Timer
  (3) Get Abnormal Status

--*/

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION(TcoWatchdog)

EFI_GUID gPeiTcoWatchdogPpiGuid = PEI_TCOWATCHDOG_PPI_GUID;

EFI_GUID_STRING (&gPeiTcoWatchdogPpiGuid, "PeiTcoWatchdogTimer", "PeiTcoWatchdogTimer");
