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

  A PEIM of features that install and publish 3 PPI services:
  (1) Enable Tco Watchdog Timer
  (2) Disable Tco Watchdog Timer
  (3) Get Abnormal Status - get the status showing whether the last boot is normal

--*/

#ifndef _PEI_TCOWATCHDOG_PPI_H
#define _PEI_TCOWATCHDOG_PPI_H
        
#define PEI_TCOWATCHDOG_PPI_GUID  \
  {0x142DACCC, 0xC5F4, 0x47ae, 0x9D, 0xFC, 0xD2, 0x5A, 0xE2, 0x88, 0x22, 0x94}


#define B_ENABLE_BIT              0x02
#define B_ABNORMAL_STATUS_BIT     0x01

#define TIMER_SECOND              0x14

typedef
EFI_STATUS
(EFIAPI *PEI_TCOWATCHDOG_ENABLE) (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CPU_IO_PPI            *CpuIo,
  IN UINT16                    Timer
  );
  
typedef
EFI_STATUS
(EFIAPI *PEI_TCOWATCHDOG_DISABLE) (
  IN EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CPU_IO_PPI            *CpuIo
  );

typedef
EFI_STATUS
(EFIAPI *PEI_TCOWATCHDOG_GETABNORMALSTATUS) (
  IN  EFI_PEI_SERVICES         **PeiServices,
  OUT BOOLEAN                  *AbnormalStatus
  );
    
typedef struct PEI_TCOWATCHDOG_PPI {
  PEI_TCOWATCHDOG_ENABLE                Enable;
  PEI_TCOWATCHDOG_DISABLE               Disable;
  PEI_TCOWATCHDOG_GETABNORMALSTATUS     GetAbnormalStatus;
} PEI_TCOWATCHDOG_PPI;


extern EFI_GUID gPeiTcoWatchdogPpiGuid;

#endif
