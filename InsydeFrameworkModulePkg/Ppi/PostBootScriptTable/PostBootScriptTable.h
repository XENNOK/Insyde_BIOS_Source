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

Copyright (c)  1999 - 2004 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PostBootScriptTable.h

Abstract:

  Provides a common Ppi to be installed just after boot script table execution.

--*/

#ifndef _PEI_POST_BOOT_SCRIPT_TABLE_H_
#define _PEI_POST_BOOT_SCRIPT_TABLE_H_

#define PEI_POST_BOOT_SCRIPT_TABLE_PPI_GUID  \
  {0x88c9d306, 0x900, 0x4eb5, 0x82, 0x60, 0x3e, 0x2d, 0xbe, 0xda, 0x1f, 0x89};

EFI_FORWARD_DECLARATION (PEI_POST_BOOT_SCRIPT_TABLE_PPI);

extern EFI_GUID   gPeiPostScriptTablePpiGuid;

#endif
