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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmmRtProtocol.h

Abstract:

  This code abstracts SMM Runtime Protocol

--*/

#ifndef _SMM_VARIABLE_PROTO_H_
#define _SMM_VARIABLE_PROTO_H_

#include "Tiano.h"

#define EFI_SMM_VARIABLE_PROTOCOL_GUID \
  { \
    0x5eb95b0a, 0xd085, 0x4a18, 0x9e, 0x41, 0x8e, 0xbd, 0xa7, 0x9f, 0x30, 0xc8 \
  }

//
// SMM RUNTIME PROTOCOL
//
typedef struct {
  EFI_GET_VARIABLE               GetVariable;
  EFI_GET_NEXT_VARIABLE_NAME     GetNextVariableName;
  EFI_SET_VARIABLE               SetVariable;

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  EFI_QUERY_VARIABLE_INFO        QueryVariableInfo;
#endif

} EFI_SMM_VARIABLE_PROTOCOL;

extern EFI_GUID gEfiSmmVariableProtocolGuid;

#endif
