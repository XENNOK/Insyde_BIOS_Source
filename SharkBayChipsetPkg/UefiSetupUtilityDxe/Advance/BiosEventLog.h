/** @file

  Header file of BIOS Event Log implementation.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _BIOS_EVENT_LOG_H
#define _BIOS_EVENT_LOG_H

#include "SetupUtility.h"

#define MAX_BUFFER_SIZE                         100
#define DELETE_SEL_CMD_SUPPORTED                (1 << 3)

#ifdef H2OIPMI_SUPPORT
#include <Protocol/H2OIpmiInterfaceProtocol.h>
#endif


#ifdef H2OIPMI_SUPPORT
EFI_STATUS
CheckBmcSelOverWriteEnable(
  VOID
  );

EFI_STATUS
GetBmcSelInfo (
  IN H2O_IPMI_INTERFACE_PROTOCOL  *IpmiTransport,
  IN UINT8                        *RecvBuf,
  IN UINT8                        *RecvSize
  );

#endif



#endif
