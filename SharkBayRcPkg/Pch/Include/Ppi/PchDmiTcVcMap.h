/** @file
  This file defines the PCH DMI TC/VC mapping PPI

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchDmiTcVcMap.h

@brief
  This file defines the PCH DMI TC/VC mapping PPI

**/
#ifndef _PCH_DMI_TC_VC_MAP_H_
#define _PCH_DMI_TC_VC_MAP_H_

///
/// Define the PCH DMI TC VC Mapping PPI GUID
///
#define PCH_DMI_TC_VC_PPI_GUID \
  { \
    0xed097352, 0x9041, 0x445a, \
    { \
      0x80, 0xb6, 0xb2, 0x9d, 0x50, 0x9e, 0x88, 0x45 \
    } \
  }

//
// Extern the GUID for PPI users.
//
extern EFI_GUID                   gPchDmiTcVcMapPpiGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PCH_DMI_TC_VC_PPI PCH_DMI_TC_VC_PPI;

typedef enum {
  DmiVcTypeVc0,
  DmiVcTypeVc1,
  DmiVcTypeVcp,
  DmiVcTypeVcm,
  DmiVcTypeMax
} PCH_DMI_VC_TYPE;

typedef struct {
  PCH_DMI_VC_TYPE Vc; ///< The Virtual Channel to which the TC is mapped
} PCH_DMI_TC_CONFIG;

typedef struct {
  BOOLEAN Enable;     ///< 0: Disable; 1: Enable
  UINT8   VcId;       ///< Vc ID Encoding for the Virtual Channel
} PCH_DMI_VC_CONFIG;

#define DmiTcTypeMax  8

///
/// PCH_DMI_TC_VC_PPI Structure Definition
/// Note: The default DMI TC/VC mapping will be used if it's not initialized
///
struct _PCH_DMI_TC_VC_PPI {
  PCH_DMI_TC_CONFIG DmiTc[DmiTcTypeMax];  ///< Configures PCH DMI Traffic class mapping.
  PCH_DMI_VC_CONFIG DmiVc[DmiVcTypeMax];  ///< Configures PCH DMI Virtual Channel setting.
};

#endif
