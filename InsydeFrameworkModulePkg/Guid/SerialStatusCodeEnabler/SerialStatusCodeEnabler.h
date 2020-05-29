//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
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

  SerialStatusCodeEnabler.h

Abstract:

  GUID used for Status Code Output Enabler HOB entrie in the HOB list.

--*/

#ifndef _STATUS_CODE_ENABLER_H_
#define _STATUS_CODE_ENABLER_H_

#ifdef EFI_DEBUG

#define EFI_STATUS_CODE_ENABLER_HOB_GUID \
  { \
    0x5ffc6cf3, 0x71ad, 0x46f5, 0xbd, 0x8b, 0x7e, 0x8f, 0xfe, 0x19, 0x7, 0xd7 \
  }

extern EFI_GUID gEfiSerialStatusCodeEnablerHobGuid;

typedef struct _EFI_STATUS_CODE_INFO {
  BOOLEAN    StatusCodeDisable;
} EFI_STATUS_CODE_INFO;

#endif
#endif
