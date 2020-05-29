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

Copyright (c)  2004 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PciGInfoHob.h

Abstract:

  GUID used for PCIG Info HOB entries in the HOB list.

--*/

#ifndef _PCIG_INFO_HOB_GUID_H_
#define _PCIG_INFO_HOB_GUID_H_


#define EFI_PCIG_INFO_HOB_GUID \
	{ 0xacb72a8b, 0xf584, 0x47cc, 0xb3, 0xa5, 0xd4, 0xb8, 0x6d, 0x4c, 0x4, 0x4};

extern EFI_GUID gEfiPciGInfoHobGuid;


typedef struct _EFI_PCIG_INFO {
	UINT8				 PciGDisable;
	UINT8				 PciGSlotNr;

} EFI_PCIG_INFO;



#endif
