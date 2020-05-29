//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef   _HOT_KEYS_H_
#define   _HOT_KEYS_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "OemGraphicsLib.h"
#include "GenericBdsLib.h"
#include "OemPostKey.h"
#include "Kernel.h"
#include "SetupConfig.h"
#include "KernelSetupConfig.h"

#ifdef ENABLE_CONSOLE_EX
#include EFI_PROTOCOL_CONSUMER (SimpleTextInputEx)
#else
#include EFI_PROTOCOL_CONSUMER (SimpleTextIn)
#endif
#include EFI_PROTOCOL_CONSUMER (UgaDraw)
#include EFI_PROTOCOL_CONSUMER (ConsoleControl)
#include EFI_PROTOCOL_CONSUMER (MonitorKeyFilter)
#include EFI_PROTOCOL_CONSUMER (OemServices)
#include EFI_PROTOCOL_CONSUMER (SkipScanRemovableDev)

#define SPECIFIED_TIME       500000
#define TIMER_EVENT_ONE_SEC  10000000

#endif
