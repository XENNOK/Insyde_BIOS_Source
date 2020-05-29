//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such 
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.                 
//
/*++

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Wdtt.h

Abstract:

  Wdtt Ppi

--*/

#ifndef _PEI_WDTT_PPI_H_
#define _PEI_WDTT_PPI_H_

#define PEI_WDTT_PPI_GUID  \
  {0xb9527104, 0x5b6a, 0x4640, 0x9b, 0x50, 0xf8, 0x49, 0xf8, 0x66, 0xcc, 0xf5}

typedef enum {
  WdttRunning,
  WdttTriggered,
  WdttNormalBoot
} PEI_WDTT_STATUS;

typedef enum {
  XtuWatchdogFlagPei1,
  XtuWatchdogFlagPei2,
  XtuWatchdogFlagXmp
} PEI_WDTT_WATCHDOG_FLAG;
//[-start-120605-IB10820064-modify]//
typedef struct _PEI_WDTT_PPI PEI_WDTT_PPI;
//[-end-120605-IB10820064-modify]//

typedef
EFI_STATUS
(EFIAPI *PEI_WDTT_PPI_START_WDT) (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag,
  IN     UINT16                     TimerInitValue    OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *PEI_WDTT_PPI_STOP_WDT) (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  );

typedef
PEI_WDTT_STATUS
(EFIAPI *PEI_WDTT_PPI_CHECK_WDT_STATUS) (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  );

typedef
EFI_STATUS
(EFIAPI *PEI_WDTT_PPI_CLEAR_WDT_STATUS) (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  );
//[-start-120605-IB10820064-modify]//
struct _PEI_WDTT_PPI {
  PEI_WDTT_PPI_START_WDT            StartWDT;
  PEI_WDTT_PPI_STOP_WDT             StopWDT;
  PEI_WDTT_PPI_CHECK_WDT_STATUS     CheckWdtStatus;
  PEI_WDTT_PPI_CLEAR_WDT_STATUS     ClearWdtStatus;
};
//[-end-120605-IB10820064-modify]//

extern EFI_GUID gPeiWdttPpiGuid;

#endif
