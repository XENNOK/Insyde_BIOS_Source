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
  PchSmmIchn.c

@brief
  File to contain all the hardware specific stuff for the Smm Ichn dispatch protocol.

**/
#include "PchSmmHelpers.h"

#define PCH_RCRB_BASE_NEED_FIX  0

PCH_SMM_SOURCE_DESC ICHN_H_SOURCE_DESCS[NUM_ICHN_TYPES] = {
  ///
  /// IchnMch
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_DMISMI
      }
    }
  },
  ///
  /// IchnPme
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0a_EN
        },
        S_PCH_ACPI_GPE0a_EN,
        N_PCH_ACPI_GPE0a_EN_PME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0a_STS
        },
        S_PCH_ACPI_GPE0a_STS,
        N_PCH_ACPI_GPE0a_STS_PME
      }
    }
  },
  ///
  /// IchnRtcAlarm
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_PM1_EN
        },
        S_PCH_ACPI_PM1_EN,
        N_PCH_ACPI_PM1_EN_RTC
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_PM1_STS
        },
        S_PCH_ACPI_PM1_STS,
        N_PCH_ACPI_PM1_STS_RTC
      }
    }
  },
  ///
  /// IchnRingIndicate
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0a_EN
        },
        S_PCH_ACPI_GPE0a_EN,
        N_PCH_ACPI_GPE0a_EN_RI
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0a_STS
        },
        S_PCH_ACPI_GPE0a_STS,
        N_PCH_ACPI_GPE0a_STS_RI
      }
    }
  },
  ///
  /// IchnAc97Wake
  /// ICH8M has removed AC97 but IchnAc97Wake is the enumed index reserved in framework SmmIchnDispatch protocol,
  /// we just fill in invalid initializer and not use it.
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnSerialIrq
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_SERIRQ
      }
    }
  },
  ///
  /// IchnY2KRollover
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_NEWCENTURY
      }
    }
  },
  ///
  /// IchnTcoTimeout
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_TIMEOUT
      }
    }
  },
  ///
  /// IchnOsTco
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_SW_TCO_SMI
      }
    }
  },
  ///
  /// IchnNmi
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_CNT)
        },
        S_PCH_TCO1_CNT,
        N_PCH_TCO_CNT_NMI2SMI_EN
      }
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_NMI2SMI
      }
    }
  },
  ///
  /// IchnIntruderDetect
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO2_CNT)
        },
        S_PCH_TCO2_CNT,
        N_PCH_TCO2_CNT_INTRD_SEL
      }
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO2_STS)
        },
        S_PCH_TCO2_STS,
        N_PCH_TCO2_STS_INTRD_DET
      }
    }
  },
  ///
  /// IchnBiosWp
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_LPC << 16) |
            (PCI_FUNCTION_NUMBER_PCH_LPC << 8) |
            R_PCH_LPC_BIOS_CNTL
          )
        },
        S_PCH_LPC_BIOS_CNTL,
        N_PCH_LPC_BIOS_CNTL_BLE
      }
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_BIOSWR
      }
    }
  },
  ///
  /// IchnMcSmi
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_MCSMI
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_MCSMI
      }
    }
  },
  ///
  /// IchnPmeB0
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0a_EN
        },
        S_PCH_ACPI_GPE0a_EN,
        N_PCH_ACPI_GPE0a_EN_PME_B0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0a_STS
        },
        S_PCH_ACPI_GPE0a_STS,
        N_PCH_ACPI_GPE0a_STS_PME_B0
      }
    }
  },
  ///
  /// IchnThrmSts (THRM# signal no longer existed in PCH)
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnSmBus
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_SMBUS
      }
    }
  },
  ///
  /// IchnIntelUsb2
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_INTEL_USB2
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_INTEL_USB2
      }
    }
  },
  ///
  /// IchnMonSmi7
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnMonSmi6
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnMonSmi5
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnMonSmi4
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap13
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap12, KBC_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        12
      }
    }
  },
  ///
  /// IchnDevTrap11
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap10
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap9, PIRQDH_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        9
      }
    }
  },
  ///
  /// IchnDevTrap8, PIRQCG_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        8
      }
    }
  },
  ///
  /// IchnDevTrap7, PIRQBF_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        7
      }
    }
  },
  ///
  /// IchnDevTrap6, PIRQAE_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        6
      }
    }
  },
  ///
  /// IchnDevTrap5
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap3
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap2
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap1
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap0, IDE_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        0
      }
    }
  },
  ///
  /// PCH I/O Trap register 3 monitor,
  /// The "PCH_RCRB_BASE_NEED_FIX" should be fixed since the RCRB base should get from the RCBA register filled by platform module.
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_3
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        3
      }
    }
  },
  ///
  /// PCH I/O Trap register 2 monitor
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_2
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        2
      }
    }
  },
  ///
  /// PCH I/O Trap register 1 monitor
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_1
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        1
      }
    }
  },
  ///
  /// PCH I/O Trap register 0 monitor
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_0
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        0
      }
    }
  }
};

PCH_SMM_SOURCE_DESC ICHN_LP_SOURCE_DESCS[NUM_ICHN_TYPES] = {
  ///
  /// IchnMch
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_DMISMI
      }
    }
  },
  ///
  /// IchnPme
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0_EN_127_96
        },
        S_PCH_ACPI_GPE0_EN_127_96,
        N_PCH_ACPI_GPE0_EN_127_96_PME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0_STS_127_96
        },
        S_PCH_ACPI_GPE0_STS_127_96,
        N_PCH_ACPI_GPE0_STS_127_96_PME
      }
    }
  },
  ///
  /// IchnRtcAlarm
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_PM1_EN
        },
        S_PCH_ACPI_PM1_EN,
        N_PCH_ACPI_PM1_EN_RTC
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_PM1_STS
        },
        S_PCH_ACPI_PM1_STS,
        N_PCH_ACPI_PM1_STS_RTC
      }
    }
  },
  ///
  /// IchnRingIndicate
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0_EN_127_96
        },
        S_PCH_ACPI_GPE0_EN_127_96,
        N_PCH_ACPI_GPE0_EN_127_96_RI
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0_STS_127_96
        },
        S_PCH_ACPI_GPE0_STS_127_96,
        N_PCH_ACPI_GPE0_STS_127_96_RI
      }
    }
  },
  ///
  /// IchnAc97Wake
  /// ICH8M has removed AC97 but IchnAc97Wake is the enumed index reserved in framework SmmIchnDispatch protocol,
  /// we just fill in invalid initializer and not use it.
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnSerialIrq
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_SERIRQ
      }
    }
  },
  ///
  /// IchnY2KRollover
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_NEWCENTURY
      }
    }
  },
  ///
  /// IchnTcoTimeout
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_TIMEOUT
      }
    }
  },
  ///
  /// IchnOsTco
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_SW_TCO_SMI
      }
    }
  },
  ///
  /// IchnNmi
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_CNT)
        },
        S_PCH_TCO1_CNT,
        N_PCH_TCO_CNT_NMI2SMI_EN
      }
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_NMI2SMI
      }
    }
  },
  ///
  /// IchnIntruderDetect
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO2_CNT)
        },
        S_PCH_TCO2_CNT,
        N_PCH_TCO2_CNT_INTRD_SEL
      }
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO2_STS)
        },
        S_PCH_TCO2_STS,
        N_PCH_TCO2_STS_INTRD_DET
      }
    }
  },
  ///
  /// IchnBiosWp
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_TCO
      },
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_LPC << 16) |
            (PCI_FUNCTION_NUMBER_PCH_LPC << 8) |
            R_PCH_LPC_BIOS_CNTL
          )
        },
        S_PCH_LPC_BIOS_CNTL,
        N_PCH_LPC_BIOS_CNTL_BLE
      }
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          (PCH_TCO_BASE + R_PCH_TCO1_STS)
        },
        S_PCH_TCO1_STS,
        N_PCH_TCO1_STS_BIOSWR
      }
    }
  },
  ///
  /// IchnMcSmi
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_MCSMI
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_MCSMI
      }
    }
  },
  ///
  /// IchnPmeB0
  ///
  {
    PCH_SMM_SCI_EN_DEPENDENT,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0_EN_127_96
        },
        S_PCH_ACPI_GPE0_EN_127_96,
        N_PCH_ACPI_GPE0_EN_127_96_PME_B0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_GPE0_STS_127_96
        },
        S_PCH_ACPI_GPE0_STS_127_96,
        N_PCH_ACPI_GPE0_STS_127_96_PME_B0
      }
    }
  },
  ///
  /// IchnThrmSts (THRM# signal no longer existed in PCH)
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnSmBus
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_SMBUS
      }
    }
  },
  ///
  /// IchnIntelUsb2
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_INTEL_USB2
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_INTEL_USB2
      }
    }
  },
  ///
  /// IchnMonSmi7
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnMonSmi6
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnMonSmi5
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnMonSmi4
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap13
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap12, KBC_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        12
      }
    }
  },
  ///
  /// IchnDevTrap11
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap10
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap9, PIRQDH_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        9
      }
    }
  },
  ///
  /// IchnDevTrap8, PIRQCG_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        8
      }
    }
  },
  ///
  /// IchnDevTrap7, PIRQBF_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        7
      }
    }
  },
  ///
  /// IchnDevTrap6, PIRQAE_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        6
      }
    }
  },
  ///
  /// IchnDevTrap5
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap3
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap2
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap1
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      NULL_BIT_DESC_INITIALIZER
    }
  },
  ///
  /// IchnDevTrap0, IDE_ACT_STS
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      NULL_BIT_DESC_INITIALIZER,
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_DEVACT_STS
        },
        S_PCH_DEVACT_STS,
        0
      }
    }
  },
  ///
  /// PCH I/O Trap register 3 monitor,
  /// The "PCH_RCRB_BASE_NEED_FIX" should be fixed since the RCRB base should get from the RCBA register filled by platform module.
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_3
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        3
      }
    }
  },
  ///
  /// PCH I/O Trap register 2 monitor
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_2
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        2
      }
    }
  },
  ///
  /// PCH I/O Trap register 1 monitor
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_1
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        1
      }
    }
  },
  ///
  /// PCH I/O Trap register 0 monitor
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_IO_TRAP_0
        },
        8,
        0
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          MEMORY_MAPPED_IO_ADDRESS_TYPE,
          PCH_RCRB_BASE_NEED_FIX + R_PCH_RCRB_TRSR
        },
        1,
        0
      }
    }
  }
};

PCH_SMM_SOURCE_DESC ICHN_EX_SOURCE_DESCS[IchnExTypeMAX - IchnExPciExpress] = {
  ///
  /// IchnExPciExpress
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExMonitor
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExSpi
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExQRT
  ///
  NULL_SOURCE_DESC_INITIALIZER,
  ///
  /// IchnExGpioUnlock
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_EN
        },
        S_PCH_SMI_EN,
        N_PCH_SMI_EN_GPIO_UNLOCK
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_SMI_STS
        },
        S_PCH_SMI_STS,
        N_PCH_SMI_STS_GPIO_UNLOCK
      }
    }
  },
  ///
  /// IchnExTmrOverflow
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_PM1_EN
        },
        S_PCH_ACPI_PM1_EN,
        N_PCH_ACPI_PM1_EN_TMROF
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          ACPI_ADDR_TYPE,
          R_PCH_ACPI_PM1_STS
        },
        S_PCH_ACPI_PM1_STS,
        N_PCH_ACPI_PM1_STS_TMROF
      }
    }
  },
  ///
  /// IchnExPcie0Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie1Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie2Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie3Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie4Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie5Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie6Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie7Hotplug
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPPDM
      }
    }
  },
  ///
  /// IchnExPcie0LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_1 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie1LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_2 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie2LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_3 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie3LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_4 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie4LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_5 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie5LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_6 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie6LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_7 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  },
  ///
  /// IchnExPcie7LinkActive
  ///
  {
    PCH_SMM_NO_FLAGS,
    {
      {
        {
          PCIE_ADDR_TYPE,
          (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8 << 8) |
            R_PCH_PCIE_MPC
          )
        },
        S_PCH_PCIE_MPC,
        N_PCH_PCIE_MPC_HPME
      },
      NULL_BIT_DESC_INITIALIZER
    },
    {
      {
        {
          PCIE_ADDR_TYPE,
         (
            (DEFAULT_PCI_BUS_NUMBER_PCH << 24) |
            (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS << 16) |
            (PCI_FUNCTION_NUMBER_PCH_PCIE_ROOT_PORT_8 << 8) |
            R_PCH_PCIE_SMSCS
          )
        },
        S_PCH_PCIE_SMSCS,
        N_PCH_PCIE_SMSCS_HPLAS
      }
    }
  }
};

///
/// TCO_STS bit that needs to be cleared
///
PCH_SMM_SOURCE_DESC TCO_STS = {
  PCH_SMM_NO_FLAGS,
  {
    NULL_BIT_DESC_INITIALIZER,
    NULL_BIT_DESC_INITIALIZER
  },
  {
    {
      {
        ACPI_ADDR_TYPE,
        R_PCH_SMI_STS
      },
      S_PCH_SMI_STS,
      N_PCH_SMI_STS_TCO
    }
  }
};

/**
  Clear the SMI status bit after the SMI handling is done

  @param[in] SrcDesc              Pointer to the PCH SMI source description table

  @retval None
**/
VOID
EFIAPI
PchSmmIchnClearSource (
  PCH_SMM_SOURCE_DESC   *SrcDesc
  )
{
  if ((
        (SrcDesc->Sts[0].Reg.Data.acpi == PCH_TCO_BASE + R_PCH_TCO1_STS) &&
      (SrcDesc->Sts[0].Bit == N_PCH_TCO1_STS_NEWCENTURY)
    ) ||
      (
        (SrcDesc->Sts[0].Reg.Data.acpi == PCH_TCO_BASE + R_PCH_TCO2_STS) &&
      (SrcDesc->Sts[0].Bit == N_PCH_TCO2_STS_INTRD_DET)
    )
      ) {
    ///
    /// This is the Y2K rollover bit and requires special handling
    ///
    PchSmmClearSourceAndBlock (SrcDesc);
  } else {
    PchSmmClearSource (SrcDesc);
  }
  ///
  /// Any TCO-based status bits require special handling.
  /// SMI_STS.TCO_STS must be cleared in addition to the status bit in the TCO registers
  ///
  PchSmmClearSource (&TCO_STS);
}

/**
  Fix the base address of the source regs and status regs.
  Since Base should get from register filled by platform modules already.

  @param[in] None.

  @retval None.
**/
VOID
PchSmmIchnFixSourceBase (
  VOID
  )
{
  PCH_SERIES  PchSeries;

  PchSeries       = GetPchSeries();
  ///
  /// We need to fix the IoTrap item's RCRB base,
  ///
  if (PchSeries == PchLp) {
    ICHN_LP_SOURCE_DESCS[IchnIoTrap3].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_3);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap3].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap2].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_2);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap2].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap1].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_1);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap1].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap0].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_0);
    ICHN_LP_SOURCE_DESCS[IchnIoTrap0].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
  } else if (PchSeries == PchH) {
    ICHN_H_SOURCE_DESCS[IchnIoTrap3].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_3);
    ICHN_H_SOURCE_DESCS[IchnIoTrap3].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
    ICHN_H_SOURCE_DESCS[IchnIoTrap2].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_2);
    ICHN_H_SOURCE_DESCS[IchnIoTrap2].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
    ICHN_H_SOURCE_DESCS[IchnIoTrap1].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_1);
    ICHN_H_SOURCE_DESCS[IchnIoTrap1].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
    ICHN_H_SOURCE_DESCS[IchnIoTrap0].En[0].Reg.Data.mem   = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_IO_TRAP_0);
    ICHN_H_SOURCE_DESCS[IchnIoTrap0].Sts[0].Reg.Data.mem  = (MEM_ADDR) (PCH_RCRB_BASE + R_PCH_RCRB_TRSR);
  }
}
