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

  TcoWatchdog.h

Abstract:

  

--*/

#ifndef _TCOWATCHDOG_PROTOCOL_H_
#define _TCOWATCHDOG_PROTOCOL_H_

#define EFI_TCOWATCHDOG_PROTOCOL_GUID \
  { 0x2FB45145, 0x9C90, 0x4bdb, 0x9D, 0x45, 0x4D, 0xF8, 0x8A, 0x02, 0xBD, 0x33 }


#include EFI_PROTOCOL_DEFINITION(CpuIO)

#define B_ENABLE_BIT              0x02
#define B_ABNORMAL_STATUS_BIT     0x01

#define TIMER_PERIODIC_SECOND     0x0F

typedef
EFI_STATUS
(EFIAPI *EFI_TCOWATCHDOG_ENABLE) (
  IN UINT16                    Timer
  );
  
typedef
EFI_STATUS
(EFIAPI *EFI_TCOWATCHDOG_DISABLE) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCOWATCHDOG_GETABNORMALSTATUS) (
  OUT BOOLEAN                 *AbnormalStatus
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TCOWATCHDOG_ISENABLED) (
  OUT BOOLEAN                 *Enabled
  );
    
typedef struct _EFI_TCOWATCHDOG_PROTOCOL {
	EFI_TCOWATCHDOG_ENABLE                Enable;
  EFI_TCOWATCHDOG_DISABLE               Disable;
  EFI_TCOWATCHDOG_GETABNORMALSTATUS     GetAbnormalStatus;
  EFI_TCOWATCHDOG_ISENABLED             IsEnabled;           
} EFI_TCOWATCHDOG_PROTOCOL;

extern EFI_GUID gEfiTcoWatchdogProtocolGuid;

#endif
