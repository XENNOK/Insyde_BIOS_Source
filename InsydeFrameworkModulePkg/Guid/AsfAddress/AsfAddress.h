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

Copyright (c)  2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

   AsfAddress.h

Abstract:

   GUIDs used for ASF Address variables.

--*/

#ifndef _EFI_ASF_ADDRESS_GUID_H_
#define _EFI_ASF_ADDRESS_GUID_H_

#define EFI_ASF_ADDRESS_GUID  \
  { 0xdb3a018f, 0xd567, 0x4e98, 0x80, 0x67, 0xb6, 0xcc, 0xcb, 0x4f, 0x89, 0x3b}

#define EFI_ASF_ADDRESS_VARIABLE_NAME (L"AsfAddress")
#define EFI_ASF_ADDRESS_FORMAT_VARIABLE_NAME ("AsfAddress")

extern CHAR16   gEfiAsfAddressName[];

extern EFI_GUID gEfiAsfAddressGuid;

#endif
