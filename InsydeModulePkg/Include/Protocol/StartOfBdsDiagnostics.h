/** @file
  This protocol will be installed  at the start of PlatformBdsDiagnostics

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _START_OF_BDS_DIAGNOSTICS_PROTOCOL_H_
#define _START_OF_BDS_DIAGNOSTICS_PROTOCOL_H_

#define EFI_START_OF_BDS_DIAGNOSTICS_PROTOCOL_GUID  \
  {0x3725e246, 0x7cab, 0x4241, 0x95, 0xa3, 0xa5, 0x60, 0xbd, 0xf4, 0x4a, 0x6c}

extern EFI_GUID gEfiStartOfBdsDiagnosticsProtocolGuid;

#endif
