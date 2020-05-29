/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmmGpi.c

@brief
  File to contain all the hardware specific stuff for the Smm Gpi dispatch protocol.

**/
#include "PchSmmHelpers.h"

#define LPTH_GPI_INIT_ELEMENT(num) { \
    PCH_SMM_NO_FLAGS, \
    { \
      { \
        { \
          ACPI_ADDR_TYPE, R_PCH_LPTH_ALT_GP_SMI_EN \
        }, \
        S_PCH_LPTH_ALT_GP_SMI_EN, num, \
      }, \
      NULL_BIT_DESC_INITIALIZER \
    }, \
    { \
      { \
        { \
          ACPI_ADDR_TYPE, R_PCH_LPTH_ALT_GP_SMI_STS \
        }, \
        S_PCH_LPTH_ALT_GP_SMI_STS, (num), \
      }, \
    } \
  }

#define LPTLP_GPI_INIT_ELEMENT(num) { \
    PCH_SMM_NO_FLAGS, \
    { \
      { \
        { \
          GPIO_ADDR_TYPE, R_PCH_LPTLP_ALT_GP_SMI_EN \
        }, \
        S_PCH_LPTLP_ALT_GP_SMI_EN, num, \
      }, \
      NULL_BIT_DESC_INITIALIZER \
    }, \
    { \
      { \
        { \
          GPIO_ADDR_TYPE, R_PCH_LPTLP_ALT_GP_SMI_STS \
        }, \
        S_PCH_LPTLP_ALT_GP_SMI_STS, (num), \
      }, \
    } \
  }

const PCH_SMM_SOURCE_DESC LPTH_GPI_SOURCE_DESC[NUM_SUPPORTED_GPIS] = {
  LPTH_GPI_INIT_ELEMENT(0),
  LPTH_GPI_INIT_ELEMENT(1),
  LPTH_GPI_INIT_ELEMENT(2),
  LPTH_GPI_INIT_ELEMENT(3),
  LPTH_GPI_INIT_ELEMENT(4),
  LPTH_GPI_INIT_ELEMENT(5),
  LPTH_GPI_INIT_ELEMENT(6),
  LPTH_GPI_INIT_ELEMENT(7),
  LPTH_GPI_INIT_ELEMENT(8),
  LPTH_GPI_INIT_ELEMENT(9),
  LPTH_GPI_INIT_ELEMENT(10),
  LPTH_GPI_INIT_ELEMENT(11),
  LPTH_GPI_INIT_ELEMENT(12),
  LPTH_GPI_INIT_ELEMENT(13),
  LPTH_GPI_INIT_ELEMENT(14),
  LPTH_GPI_INIT_ELEMENT(15),
};

const PCH_SMM_SOURCE_DESC LPTLP_GPI_SOURCE_DESC[NUM_SUPPORTED_GPIS] = {
  LPTLP_GPI_INIT_ELEMENT(0),
  LPTLP_GPI_INIT_ELEMENT(1),
  LPTLP_GPI_INIT_ELEMENT(2),
  LPTLP_GPI_INIT_ELEMENT(3),
  LPTLP_GPI_INIT_ELEMENT(4),
  LPTLP_GPI_INIT_ELEMENT(5),
  LPTLP_GPI_INIT_ELEMENT(6),
  LPTLP_GPI_INIT_ELEMENT(7),
  LPTLP_GPI_INIT_ELEMENT(8),
  LPTLP_GPI_INIT_ELEMENT(9),
  LPTLP_GPI_INIT_ELEMENT(10),
  LPTLP_GPI_INIT_ELEMENT(11),
  LPTLP_GPI_INIT_ELEMENT(12),
  LPTLP_GPI_INIT_ELEMENT(13),
  LPTLP_GPI_INIT_ELEMENT(14),
  LPTLP_GPI_INIT_ELEMENT(15),
};
