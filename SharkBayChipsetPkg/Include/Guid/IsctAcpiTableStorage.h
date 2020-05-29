/** @file

  GUID definition for the Isct ACPI table storage file name
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;*Copyright (c)  2012 Intel Corporation. All rights reserved
;*This software and associated documentation (if any) is furnished
;*under a license and may only be used or copied in accordance
;*with the terms of the license. Except as permitted by such
;*license, no part of this software or documentation may be
;*reproduced, stored in a retrieval system, or transmitted in any
;*form or by any means without the express written consent of
;*Intel Corporation.
;******************************************************************************

*/

#ifndef _ISCT_ACPI_TABLE_STORAGE_H_
#define _ISCT_ACPI_TABLE_STORAGE_H_
//FA2DDC38-3F19-4218-B53E-D9D79D626767
#define ISCT_ACPI_TABLE_STORAGE_GUID \
  { \
    0xfa2ddc38, 0x3f19, 0x4218, 0xb5, 0x3e, 0xd9, 0xd7, 0x9d, 0x62, 0x67, 0x67 \
  }

extern EFI_GUID gIsctAcpiTableStorageGuid;

#endif