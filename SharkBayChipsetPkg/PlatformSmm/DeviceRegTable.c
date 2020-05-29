/** @file

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

#include <DeviceRegTable.h>
//[-start-121022-IB10820142-remove]//
//#include <DeviceRegOemTable.h>
//[-end-121022-IB10820142-remove]//
#include <PchAccess.h>
//[-start-121212-IB10820191-modify]//
#include <CpuRegs.h>
#include <Cpu.h>
//[-end-121212-IB10820191-modify]//
// PIC Buffer
UINT8 I8259Mask, I8259AMask;
UINT8 PIC1_ICW2, PIC2_ICW2;


UINT8 DRAM_PCI [] = {
  0x2C, 
  0x80, 0x84
};

UINT32 DRAM_PCI_SAVE [sizeof (DRAM_PCI)];

//
// IGD
//
UINT8 OB_GFX [] = {
// #ifdef SWITCHABLE_GRAPHICS_SUPPORT
//   0x2C,
// #endif
//   0x10, 0x14, 0x18, 0x20, 0x3c, 0xE8, 0xFC, 0x04
  0x3c, 0xE8, 0xFC
};

UINT32 VIDEO_PCI_SAVE [sizeof (OB_GFX)];
UINT32 VIDEO_PCI_F1_SAVE [FILL_PCI_REG_NUM];
UINT8  PEG_GFX [] = { 
//   0x18, 0x1C, 
//   0x20, 0x24, 0x2C, 
  0x3C,
  0x8C,
  0xAC,
//   0x04  
};

UINT32 PEG_PCIE_SAVE [sizeof (PEG_GFX)];

// Bus 0, Dev 4, Fun 0
UINT8 B0_D4_F0_PCI [] = {
//   0x10, 0x3c, 0x04
  0x3c
};

UINT32 B0_D4_F0_PCI_SAVE [sizeof (B0_D4_F0_PCI)];

//
// Lpc Save Reg Table
//
UINT8 PCH_LPC_PCI [] = {
  0x40,
  0x44,
  0x4C,
  0x60,
  0x64,
  0x68,
  0x80,
  0xA0,
  0xA8,
  0xD8,
  0xDC,
  0xB8,
  0xF0
};

UINT32 PCH_LPC_PCI_SAVE [sizeof (PCH_LPC_PCI)];

UINT8
CFD_PPL_PCIE_PCI [] = {
  0x40
};

UINT32 CFD_PPL_PCIE_SAVE [sizeof (CFD_PPL_PCIE_PCI)];

UINT8
CFD_PPL_PCI [] = {
  0xC4
};

UINT32 CFD_PPL_PCI_SAVE [sizeof (CFD_PPL_PCI)];

//
// SATA Controller
//
UINT8 PCH_SATA_PCI [] = {
  0x90,       // 0x90 must be first  for Combined Mode
  0x08,
//   0x10,
//   0x14,
//   0x18,
//   0x1C,
//   0x20,
//   0x24,  
//   0x28,
//   0x2c,
  0x3c,
  0x40,
  0x44,
  0x48,
  0x54,
//   0x04
};

UINT32 PCH_SATA_PCI_SAVE  [sizeof (PCH_SATA_PCI)];
UINT32 PCH_SATA2_PCI_SAVE [sizeof (PCH_SATA_PCI)];


//
// UHCI Controller
//
UINT8 PCH_UHCI_PCI [] = {
//   0x20,
//   0x2C,
  0x3C,
  0xC8,
//   0x04
};
UINT32 PCH_UHCI0_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI1_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI2_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI3_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI4_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI5_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI6_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI7_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI8_PCI_SAVE [sizeof (PCH_UHCI_PCI)];
UINT32 PCH_UHCI9_PCI_SAVE [sizeof (PCH_UHCI_PCI)];

//
// EHCI Controller
//
UINT8 PCH_EHCI_PCI [] = {

//   0x10,
//   0x20,
//   0x2C,
  0x3C,
  0xFC,
//   0x04,
};

UINT32 PCH_EHCI0_PCI_SAVE [sizeof (PCH_EHCI_PCI)];
UINT32 PCH_EHCI1_PCI_SAVE [sizeof (PCH_EHCI_PCI)];



//
// SMBus
//
UINT8 PCH_SMB_PCI [] = {
//   0x10,
//   0x20,
//   0x2C,
  0x3C,
  0x40,
//   0x04

};
UINT32 PCH_SMB_PCI_SAVE [sizeof (PCH_SMB_PCI)];

UINT8
PCH_AZ_PCI [] = {
//   0x10,
//   0x14,
  0x3C,
//   0x04
};

UINT32 PCH_AZ_PCI_SAVE [sizeof (PCH_AZ_PCI)];

//
// P2P Controller
//
UINT8 PCH_HUB_PCI [] = {
//   0x18, 0x1C,
//   0x20, 0x24, 0x28,
  0x3C,
  0x40,
  0x54,
//   0x04  
};

UINT32 PCH_HUB_PCI_SAVE [sizeof (PCH_HUB_PCI)];

//
// Pci Express Port 0-5
//
UINT8
PCH_PCIE_PCI [] = {
//   0x18, 0x1C,
//   0x20, 0x24,
  0x3C,
  0x40,
  0x54,
  0x58,
  0xD8,
//   0x04  
};

UINT32 PCH_PCIE0_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE1_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE2_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE3_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE4_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE5_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE6_SAVE [sizeof (PCH_PCIE_PCI)];
UINT32 PCH_PCIE7_SAVE [sizeof (PCH_PCIE_PCI)];

/*
//
// DMI 
//
UINT8 DMI_PCI [] = {

};
UINT32 DMI_PCI_SAVE [sizeof (DMI_PCI)];
*/

//
// PCI Express Root Port
//
UINT8 PCIE_ROOT_PCI [] = {
//   0x18, 0x1C,
//   0x20, 0x24,
  0x3C,
//   0x04
};
UINT32 PCIE_ROOT_SAVE [sizeof (PCIE_ROOT_PCI)];

/*
//
// Virtualization Engine Controller Interface 
//
UINT8 PCH_VECI_PCI [] = {

};
UINT32 PCH_VECI_SAVE [sizeof (PCH_VECI_PCI)];
*/

//
// Host Manageability Engine Communication Interface 0-1
//
UINT8 PCH_HECI1_PCI [] = {
//   0x10,
  0x3C,
//   0x04
};
UINT32 PCH_HECI1_SAVE [sizeof (PCH_HECI1_PCI)];

INT8 PCH_HECI2_PCI [] = {
//   0x10,
  0x3C,
//   0x04
};
UINT32 PCH_HECI2_SAVE [sizeof (PCH_HECI2_PCI)];

INT8 PCH_KT_PCI [] = {
//   0x10,
//   0x14,
  0x3C,
//   0x04
};
UINT32 PCH_KT_SAVE [sizeof (PCH_KT_PCI)];

INT8 PCH_GBE_PCI [] = {
//   0x10, 
//   0x14,
//   0x18,
  0x3C,
  0xCC,
//   0x04
};

UINT32 PCH_GBE_SAVE [sizeof (PCH_GBE_PCI)];

INT8 PCH_THERMAL_PCI [] = {
//   0x10,
  0x40,
  0x3C,
//   0x04
};
UINT32 PCH_THERMAL_SAVE [sizeof (PCH_THERMAL_PCI)];


UINT8 SYSTEM_ADDRESS_DECODER_PCI [] = {
  0x40, 0x44, 0x48, 0x4C 
};

UINT32 SYSTEM_ADDRESS_DECODER_PCI_SAVE [sizeof (SYSTEM_ADDRESS_DECODER_PCI)];

//[-start-121120-IB05280016-modify]//
//
// Pci device table list
//
SR_DEVICE PciDeviceSubResList [] = {
//=====================================================================================================
//          Register Number                  | Bus | Dev | Fun | Reg Table      | Reg Table Save Area
//=====================================================================================================
  { (sizeof (DRAM_PCI)       /sizeof (UINT8)), 0x00, 0x00, 0x00, DRAM_PCI       , DRAM_PCI_SAVE      },
  { (sizeof (PEG_GFX)        /sizeof (UINT8)), 0x00, 0x01, 0x00, PEG_GFX        , PEG_PCIE_SAVE      },
  { (sizeof (OB_GFX)         /sizeof (UINT8)), 0x00, 0x02, 0x00, OB_GFX         , VIDEO_PCI_SAVE     },
  { (FILL_PCI_REG_NUM)                       , 0x00, 0x02, 0x01, NULL           , VIDEO_PCI_F1_SAVE  }, 
  { (sizeof (B0_D4_F0_PCI)   /sizeof (UINT8)), 0x00, 0x04, 0x00, B0_D4_F0_PCI   , B0_D4_F0_PCI_SAVE  }, 
  { (sizeof (PCH_LPC_PCI)    /sizeof (UINT8)), 0x00, 0x1F, 0x00, PCH_LPC_PCI    , PCH_LPC_PCI_SAVE   },
  { (sizeof (PCH_SATA_PCI)   /sizeof (UINT8)), 0x00, 0x1F, 0x02, PCH_SATA_PCI   , PCH_SATA_PCI_SAVE  },
  { (sizeof (PCH_SMB_PCI)    /sizeof (UINT8)), 0x00, 0x1F, 0x03, PCH_SMB_PCI    , PCH_SMB_PCI_SAVE   },
//[-start-120718-IB05330360-modify]//
  { (sizeof (PCH_SATA_PCI)   /sizeof (UINT8)), 0x00, 0x1F, 0x05, PCH_SATA_PCI   , PCH_SATA2_PCI_SAVE },
  { (sizeof (PCH_EHCI_PCI)   /sizeof (UINT8)), 0x00, 0x1D, 0x00, PCH_EHCI_PCI   , PCH_EHCI0_PCI_SAVE },
  { (sizeof (PCH_EHCI_PCI)   /sizeof (UINT8)), 0x00, 0x1A, 0x00, PCH_EHCI_PCI   , PCH_EHCI1_PCI_SAVE },
  { (sizeof (PCH_AZ_PCI)     /sizeof (UINT8)), 0x00, 0x1B, 0x00, PCH_AZ_PCI     , PCH_AZ_PCI_SAVE    },
  { (sizeof (PCH_HUB_PCI)    /sizeof (UINT8)), 0x00, 0x1E, 0x00, PCH_HUB_PCI    , PCH_HUB_PCI_SAVE   },
//[-end-120718-IB05330360-modify]//
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x00, PCH_PCIE_PCI   , PCH_PCIE0_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x01, PCH_PCIE_PCI   , PCH_PCIE1_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x02, PCH_PCIE_PCI   , PCH_PCIE2_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x03, PCH_PCIE_PCI   , PCH_PCIE3_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x04, PCH_PCIE_PCI   , PCH_PCIE4_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x05, PCH_PCIE_PCI   , PCH_PCIE5_SAVE     },
//[-start-120718-IB05330360-modify]//
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x06, PCH_PCIE_PCI   , PCH_PCIE6_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x07, PCH_PCIE_PCI   , PCH_PCIE7_SAVE     },
//[-end-120718-IB05330360-modify]//
  { (sizeof (PCH_HECI1_PCI)  /sizeof (UINT8)), 0x00, 0x16, 0x00, PCH_HECI1_PCI  , PCH_HECI1_SAVE     },
  { (sizeof (PCH_HECI2_PCI)  /sizeof (UINT8)), 0x00, 0x16, 0x01, PCH_HECI2_PCI  , PCH_HECI2_SAVE     },
  { (sizeof (PCH_KT_PCI)     /sizeof (UINT8)), 0x00, 0x16, 0x03, PCH_KT_PCI     , PCH_KT_SAVE        },
  { (sizeof (PCH_GBE_PCI)    /sizeof (UINT8)), 0x00, 0x19, 0x00, PCH_GBE_PCI    , PCH_GBE_SAVE       },
  { (sizeof (PCH_THERMAL_PCI)/sizeof (UINT8)), 0x00, 0x1F, 0x06, PCH_THERMAL_PCI, PCH_THERMAL_SAVE   },
  { 0, 0, 0, 0, 0, 0 },
};
//[-end-121120-IB05280016-modify]//

//[-start-121120-IB05280016-add]//
//
// Pci device table list for ULT
//
SR_DEVICE PciDeviceSubResListUlt [] = {
//=====================================================================================================
//          Register Number                  | Bus | Dev | Fun | Reg Table      | Reg Table Save Area
//=====================================================================================================
  { (sizeof (DRAM_PCI)       /sizeof (UINT8)), 0x00, 0x00, 0x00, DRAM_PCI       , DRAM_PCI_SAVE      },
  { (sizeof (PEG_GFX)        /sizeof (UINT8)), 0x00, 0x01, 0x00, PEG_GFX        , PEG_PCIE_SAVE      },
  { (sizeof (OB_GFX)         /sizeof (UINT8)), 0x00, 0x02, 0x00, OB_GFX         , VIDEO_PCI_SAVE     },
  { (FILL_PCI_REG_NUM)                       , 0x00, 0x02, 0x01, NULL           , VIDEO_PCI_F1_SAVE  }, 
  { (sizeof (B0_D4_F0_PCI)   /sizeof (UINT8)), 0x00, 0x04, 0x00, B0_D4_F0_PCI   , B0_D4_F0_PCI_SAVE  }, 
  { (sizeof (PCH_LPC_PCI)    /sizeof (UINT8)), 0x00, 0x1F, 0x00, PCH_LPC_PCI    , PCH_LPC_PCI_SAVE   },
  { (sizeof (PCH_SATA_PCI)   /sizeof (UINT8)), 0x00, 0x1F, 0x02, PCH_SATA_PCI   , PCH_SATA_PCI_SAVE  },
  { (sizeof (PCH_SMB_PCI)    /sizeof (UINT8)), 0x00, 0x1F, 0x03, PCH_SMB_PCI    , PCH_SMB_PCI_SAVE   },
  { (sizeof (PCH_EHCI_PCI)   /sizeof (UINT8)), 0x00, 0x1D, 0x00, PCH_EHCI_PCI   , PCH_EHCI0_PCI_SAVE },
  { (sizeof (PCH_AZ_PCI)     /sizeof (UINT8)), 0x00, 0x1B, 0x00, PCH_AZ_PCI     , PCH_AZ_PCI_SAVE    },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x00, PCH_PCIE_PCI   , PCH_PCIE0_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x01, PCH_PCIE_PCI   , PCH_PCIE1_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x02, PCH_PCIE_PCI   , PCH_PCIE2_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x03, PCH_PCIE_PCI   , PCH_PCIE3_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x04, PCH_PCIE_PCI   , PCH_PCIE4_SAVE     },
  { (sizeof (PCH_PCIE_PCI)   /sizeof (UINT8)), 0x00, 0x1C, 0x05, PCH_PCIE_PCI   , PCH_PCIE5_SAVE     },
  { (sizeof (PCH_HECI1_PCI)  /sizeof (UINT8)), 0x00, 0x16, 0x00, PCH_HECI1_PCI  , PCH_HECI1_SAVE     },
  { (sizeof (PCH_HECI2_PCI)  /sizeof (UINT8)), 0x00, 0x16, 0x01, PCH_HECI2_PCI  , PCH_HECI2_SAVE     },
  { (sizeof (PCH_KT_PCI)     /sizeof (UINT8)), 0x00, 0x16, 0x03, PCH_KT_PCI     , PCH_KT_SAVE        },
  { (sizeof (PCH_GBE_PCI)    /sizeof (UINT8)), 0x00, 0x19, 0x00, PCH_GBE_PCI    , PCH_GBE_SAVE       },
  { (sizeof (PCH_THERMAL_PCI)/sizeof (UINT8)), 0x00, 0x1F, 0x06, PCH_THERMAL_PCI, PCH_THERMAL_SAVE   },
  { 0, 0, 0, 0, 0, 0 },
};
//[-end-121120-IB05280016-add]//

//[-start-121022-IB10820142-add]//
//[-start-121029-IB10820145-modify]//
SR_OEM_DEVICE
PciDeviceOemSubResList [] = {
//==============================================================================================
//          Register Number               | Bus | Dev | Fun | Reg Table   | Reg Table Save Area
//==============================================================================================
  {{ (FILL_PCI_REG_NUM),                     0x00, 0x00, 0x00, NULL,         PEGPCI_Fun0   }, PCIE_PEG_BRIDGE},
  {{ (FILL_PCI_REG_NUM),                     0x00, 0x00, 0x01, NULL,         PEGPCI_Fun1   }, PCIE_PEG_BRIDGE},
  {{ 0, 0, 0, 0, 0}, 0},
};
//[-end-121029-IB10820145-modify]//
//[-end-121022-IB10820142-add]//

SR_TABLE PciResTable = {PciDeviceSubResList, PciDeviceOemSubResList};

//[-start-121120-IB05280016-add]//
SR_TABLE PciResTableUlt = {PciDeviceSubResListUlt, PciDeviceOemSubResList};
//[-end-121120-IB05280016-add]//


UINT8 EDP_PCI_SAVE [0x09];

//[-start-121211-IB05300367-modify]//
//[-start-121212-IB05300368-modify]//
//*******************************************************************************************
//
// GPIO Register
//
UINT8
GPIO_REG [] = {
  R_PCH_GPIO_USE_SEL,  R_PCH_GPIO_IO_SEL,  R_PCH_GPIO_LVL,  R_PCH_GP_RST_SEL,
  R_PCH_GPIO_USE_SEL2, R_PCH_GPIO_IO_SEL2, R_PCH_GPIO_LVL2, R_PCH_GP_RST_SEL2,
  R_PCH_GPIO_USE_SEL3, R_PCH_GPIO_IO_SEL3, R_PCH_GPIO_LVL3, R_PCH_GP_RST_SEL3,
  R_PCH_GPIO_BLINK,
  R_PCH_GPIO_NMI_EN,
  R_PCH_GPIO_GPI_INV,
  0xFF
};

UINT32 GPIO_REG_SAVE [sizeof (GPIO_REG)];

UINT16
GPIO_REG_ULT [] = {
  R_PCH_GPIO_OWN0 , (R_PCH_GPIO_OWN0  + 4), (R_PCH_GPIO_OWN0  + 8),
  R_PCH_GPIO_ROUT0, (R_PCH_GPIO_ROUT0 + 4), (R_PCH_GPIO_ROUT0 + 8),
  R_PCH_GPI_IE0   , (R_PCH_GPI_IE0    + 4), (R_PCH_GPI_IE0    + 8),
  (R_PCH_GP_X_CONFIG0 ( 0)), (R_PCH_GP_X_CONFIG0 ( 0) + 4),
  (R_PCH_GP_X_CONFIG0 ( 1)), (R_PCH_GP_X_CONFIG0 ( 1) + 4),
  (R_PCH_GP_X_CONFIG0 ( 2)), (R_PCH_GP_X_CONFIG0 ( 2) + 4),
  (R_PCH_GP_X_CONFIG0 ( 3)), (R_PCH_GP_X_CONFIG0 ( 3) + 4),
  (R_PCH_GP_X_CONFIG0 ( 4)), (R_PCH_GP_X_CONFIG0 ( 4) + 4),
  (R_PCH_GP_X_CONFIG0 ( 5)), (R_PCH_GP_X_CONFIG0 ( 5) + 4),
  (R_PCH_GP_X_CONFIG0 ( 6)), (R_PCH_GP_X_CONFIG0 ( 6) + 4),
  (R_PCH_GP_X_CONFIG0 ( 7)), (R_PCH_GP_X_CONFIG0 ( 7) + 4),
  (R_PCH_GP_X_CONFIG0 ( 8)), (R_PCH_GP_X_CONFIG0 ( 8) + 4),
  (R_PCH_GP_X_CONFIG0 ( 9)), (R_PCH_GP_X_CONFIG0 ( 9) + 4),
  (R_PCH_GP_X_CONFIG0 (10)), (R_PCH_GP_X_CONFIG0 (10) + 4),
  (R_PCH_GP_X_CONFIG0 (11)), (R_PCH_GP_X_CONFIG0 (11) + 4),
  (R_PCH_GP_X_CONFIG0 (12)), (R_PCH_GP_X_CONFIG0 (12) + 4),
  (R_PCH_GP_X_CONFIG0 (13)), (R_PCH_GP_X_CONFIG0 (13) + 4),
  (R_PCH_GP_X_CONFIG0 (14)), (R_PCH_GP_X_CONFIG0 (14) + 4),
  (R_PCH_GP_X_CONFIG0 (15)), (R_PCH_GP_X_CONFIG0 (15) + 4),
  (R_PCH_GP_X_CONFIG0 (16)), (R_PCH_GP_X_CONFIG0 (16) + 4),
  (R_PCH_GP_X_CONFIG0 (17)), (R_PCH_GP_X_CONFIG0 (17) + 4),
  (R_PCH_GP_X_CONFIG0 (18)), (R_PCH_GP_X_CONFIG0 (18) + 4),
  (R_PCH_GP_X_CONFIG0 (19)), (R_PCH_GP_X_CONFIG0 (19) + 4),
  (R_PCH_GP_X_CONFIG0 (20)), (R_PCH_GP_X_CONFIG0 (20) + 4),
  (R_PCH_GP_X_CONFIG0 (21)), (R_PCH_GP_X_CONFIG0 (21) + 4),
  (R_PCH_GP_X_CONFIG0 (22)), (R_PCH_GP_X_CONFIG0 (22) + 4),
  (R_PCH_GP_X_CONFIG0 (23)), (R_PCH_GP_X_CONFIG0 (23) + 4),
  (R_PCH_GP_X_CONFIG0 (24)), (R_PCH_GP_X_CONFIG0 (24) + 4),
  (R_PCH_GP_X_CONFIG0 (25)), (R_PCH_GP_X_CONFIG0 (25) + 4),
  (R_PCH_GP_X_CONFIG0 (26)), (R_PCH_GP_X_CONFIG0 (26) + 4),
  (R_PCH_GP_X_CONFIG0 (27)), (R_PCH_GP_X_CONFIG0 (27) + 4),
  (R_PCH_GP_X_CONFIG0 (28)), (R_PCH_GP_X_CONFIG0 (28) + 4),
  (R_PCH_GP_X_CONFIG0 (29)), (R_PCH_GP_X_CONFIG0 (29) + 4),
  (R_PCH_GP_X_CONFIG0 (30)), (R_PCH_GP_X_CONFIG0 (30) + 4),
  (R_PCH_GP_X_CONFIG0 (31)), (R_PCH_GP_X_CONFIG0 (31) + 4),
  (R_PCH_GP_X_CONFIG0 (32)), (R_PCH_GP_X_CONFIG0 (32) + 4),
  (R_PCH_GP_X_CONFIG0 (33)), (R_PCH_GP_X_CONFIG0 (33) + 4),
  (R_PCH_GP_X_CONFIG0 (34)), (R_PCH_GP_X_CONFIG0 (34) + 4),
  (R_PCH_GP_X_CONFIG0 (35)), (R_PCH_GP_X_CONFIG0 (35) + 4),
  (R_PCH_GP_X_CONFIG0 (36)), (R_PCH_GP_X_CONFIG0 (36) + 4),
  (R_PCH_GP_X_CONFIG0 (37)), (R_PCH_GP_X_CONFIG0 (37) + 4),
  (R_PCH_GP_X_CONFIG0 (38)), (R_PCH_GP_X_CONFIG0 (38) + 4),
  (R_PCH_GP_X_CONFIG0 (39)), (R_PCH_GP_X_CONFIG0 (39) + 4),
  (R_PCH_GP_X_CONFIG0 (40)), (R_PCH_GP_X_CONFIG0 (40) + 4),
  (R_PCH_GP_X_CONFIG0 (41)), (R_PCH_GP_X_CONFIG0 (41) + 4),
  (R_PCH_GP_X_CONFIG0 (42)), (R_PCH_GP_X_CONFIG0 (42) + 4),
  (R_PCH_GP_X_CONFIG0 (43)), (R_PCH_GP_X_CONFIG0 (43) + 4),
  (R_PCH_GP_X_CONFIG0 (44)), (R_PCH_GP_X_CONFIG0 (44) + 4),
  (R_PCH_GP_X_CONFIG0 (45)), (R_PCH_GP_X_CONFIG0 (45) + 4),
  (R_PCH_GP_X_CONFIG0 (46)), (R_PCH_GP_X_CONFIG0 (46) + 4),
  (R_PCH_GP_X_CONFIG0 (47)), (R_PCH_GP_X_CONFIG0 (47) + 4),
  (R_PCH_GP_X_CONFIG0 (48)), (R_PCH_GP_X_CONFIG0 (48) + 4),
  (R_PCH_GP_X_CONFIG0 (49)), (R_PCH_GP_X_CONFIG0 (49) + 4),
  (R_PCH_GP_X_CONFIG0 (50)), (R_PCH_GP_X_CONFIG0 (50) + 4),
  (R_PCH_GP_X_CONFIG0 (51)), (R_PCH_GP_X_CONFIG0 (51) + 4),
  (R_PCH_GP_X_CONFIG0 (52)), (R_PCH_GP_X_CONFIG0 (52) + 4),
  (R_PCH_GP_X_CONFIG0 (53)), (R_PCH_GP_X_CONFIG0 (53) + 4),
  (R_PCH_GP_X_CONFIG0 (54)), (R_PCH_GP_X_CONFIG0 (54) + 4),
  (R_PCH_GP_X_CONFIG0 (55)), (R_PCH_GP_X_CONFIG0 (55) + 4),
  (R_PCH_GP_X_CONFIG0 (56)), (R_PCH_GP_X_CONFIG0 (56) + 4),
  (R_PCH_GP_X_CONFIG0 (57)), (R_PCH_GP_X_CONFIG0 (57) + 4),
  (R_PCH_GP_X_CONFIG0 (58)), (R_PCH_GP_X_CONFIG0 (58) + 4),
  (R_PCH_GP_X_CONFIG0 (59)), (R_PCH_GP_X_CONFIG0 (59) + 4),
  (R_PCH_GP_X_CONFIG0 (60)), (R_PCH_GP_X_CONFIG0 (60) + 4),
  (R_PCH_GP_X_CONFIG0 (61)), (R_PCH_GP_X_CONFIG0 (61) + 4),
  (R_PCH_GP_X_CONFIG0 (62)), (R_PCH_GP_X_CONFIG0 (62) + 4),
  (R_PCH_GP_X_CONFIG0 (63)), (R_PCH_GP_X_CONFIG0 (63) + 4),
  (R_PCH_GP_X_CONFIG0 (64)), (R_PCH_GP_X_CONFIG0 (64) + 4),
  (R_PCH_GP_X_CONFIG0 (65)), (R_PCH_GP_X_CONFIG0 (65) + 4),
  (R_PCH_GP_X_CONFIG0 (66)), (R_PCH_GP_X_CONFIG0 (66) + 4),
  (R_PCH_GP_X_CONFIG0 (67)), (R_PCH_GP_X_CONFIG0 (67) + 4),
  (R_PCH_GP_X_CONFIG0 (68)), (R_PCH_GP_X_CONFIG0 (68) + 4),
  (R_PCH_GP_X_CONFIG0 (69)), (R_PCH_GP_X_CONFIG0 (69) + 4),
  (R_PCH_GP_X_CONFIG0 (70)), (R_PCH_GP_X_CONFIG0 (70) + 4),
  (R_PCH_GP_X_CONFIG0 (71)), (R_PCH_GP_X_CONFIG0 (71) + 4),
  (R_PCH_GP_X_CONFIG0 (72)), (R_PCH_GP_X_CONFIG0 (72) + 4),
  (R_PCH_GP_X_CONFIG0 (73)), (R_PCH_GP_X_CONFIG0 (73) + 4),
  (R_PCH_GP_X_CONFIG0 (74)), (R_PCH_GP_X_CONFIG0 (74) + 4),
  (R_PCH_GP_X_CONFIG0 (75)), (R_PCH_GP_X_CONFIG0 (75) + 4),
  (R_PCH_GP_X_CONFIG0 (76)), (R_PCH_GP_X_CONFIG0 (76) + 4),
  (R_PCH_GP_X_CONFIG0 (77)), (R_PCH_GP_X_CONFIG0 (77) + 4),
  (R_PCH_GP_X_CONFIG0 (78)), (R_PCH_GP_X_CONFIG0 (78) + 4),
  (R_PCH_GP_X_CONFIG0 (79)), (R_PCH_GP_X_CONFIG0 (79) + 4),
  (R_PCH_GP_X_CONFIG0 (80)), (R_PCH_GP_X_CONFIG0 (80) + 4),
  (R_PCH_GP_X_CONFIG0 (81)), (R_PCH_GP_X_CONFIG0 (81) + 4),
  (R_PCH_GP_X_CONFIG0 (82)), (R_PCH_GP_X_CONFIG0 (82) + 4),
  (R_PCH_GP_X_CONFIG0 (83)), (R_PCH_GP_X_CONFIG0 (83) + 4),
  (R_PCH_GP_X_CONFIG0 (84)), (R_PCH_GP_X_CONFIG0 (84) + 4),
  (R_PCH_GP_X_CONFIG0 (85)), (R_PCH_GP_X_CONFIG0 (85) + 4),
  (R_PCH_GP_X_CONFIG0 (86)), (R_PCH_GP_X_CONFIG0 (86) + 4),
  (R_PCH_GP_X_CONFIG0 (87)), (R_PCH_GP_X_CONFIG0 (87) + 4),
  (R_PCH_GP_X_CONFIG0 (88)), (R_PCH_GP_X_CONFIG0 (88) + 4),
  (R_PCH_GP_X_CONFIG0 (89)), (R_PCH_GP_X_CONFIG0 (89) + 4),
  (R_PCH_GP_X_CONFIG0 (90)), (R_PCH_GP_X_CONFIG0 (90) + 4),
  (R_PCH_GP_X_CONFIG0 (91)), (R_PCH_GP_X_CONFIG0 (91) + 4),
  (R_PCH_GP_X_CONFIG0 (92)), (R_PCH_GP_X_CONFIG0 (92) + 4),
  (R_PCH_GP_X_CONFIG0 (93)), (R_PCH_GP_X_CONFIG0 (93) + 4),
  (R_PCH_GP_X_CONFIG0 (94)), (R_PCH_GP_X_CONFIG0 (94) + 4),
  0xFFFF
};

UINT32 GPIO_REG_SAVE_ULT [sizeof (GPIO_REG_ULT) / sizeof (UINT16)];
//[-end-121212-IB05300368-modify]//
//[-end-121211-IB05300367-modify]//

//*******************************************************************************************
//
//  Edge/Level Control Registers
//
UINT8  ELCR[2];

//*******************************************************************************************
//
// PM Base IO Register
//
UINT8
PMIO_REG [] = {
  R_PCH_ACPI_PM1_EN, (R_PCH_ACPI_PM1_EN + 1), R_PCH_ACPI_PM1_CNT,
  R_PCH_SMI_EN, (R_PCH_SMI_EN + 1), (R_PCH_SMI_EN + 2), (R_PCH_SMI_EN + 3),
  (PCH_TCO_BASE + R_PCH_TCO1_CNT + 1),
  0xFF
};

UINT8 PMIO_REG_SAVE [sizeof (PMIO_REG)];

//*******************************************************************************************
//
//  CPU State
//
UINT16
CPU_MSR_REG [] = {
  0x13C,         // EFI_FEATURE_CONFIG
  0x01B,        // MSR_IA32_APIC_BASE
  0x02E,        // MSR_PIC_MSG_CONTROL
  0x0E2,        // MSR_PMG_CST_CONTROL_CONTROL
  0x0E4,        // MSR_IO_CAPT_BASE
  0x174,        // MSR_IA32_SYSENTER_CS
  0x175,        // MSR_IA32_SYSENTER_ESP
  0x176,        // MSR_IA32_SYSENTER_EIP
  0x194,        // MSR_FLEX_RATIO
  0x199,        // MSR_IA32_PERF_CTL
  0x19A,        // MSR_IA32_CLOCK_MODULATION
  0x1A0,        // MSR_IA32_MISC_ENABLES  
  0x1A2,        // MSR_TEMPERATURE_TARGET
  0x1A4,        // MSR_MISC_FEATURE_CONTROL
  0x1AA,        // MSR_MISC_PWR_MGMT
  0x1FC,        // MSR_POWER_CTL
//[-start-120705-IB05330352-remove]//
//  0x1B0,        // MSR_IA32_ENERGY_PERFORMANCE_BIAS , Note: This MSR must restore after MSR_POWER_CTL.
//[-end-120705-IB05330352-remove]//
  0x277,        // MSR_IA32_CR_PAT
  0x400,        // MSR_IA32_MCi_CTL
  0x610,        // TURBO_POWER_LIMIT
//  0x03A,        // MSR_IA32_FEATURE_CONTROL
  0x2E7,        // MSR_TXT_CTRL_STS
  0x1F2,        // SMRR Base Register
  0x1F3,        // SMRR Mask Register
  0x601,        // MSR_PRIMARY_PLANE_CURRENT_CONFIG
//  0x602,        // MSR_SECONDARY_PLANE_CURRENT_CONFIG
  0x60A,        // MSR_PKGC3_IRTL
  0x60B,        // MSR_PKGC6_IRTL
  0x60C,        // MSR_PKGC7_IRTL
  0x610,        // MSR_TURBO_POWER_LIMIT
  0x638,        // MSR_PRIMARY_PLANE_TURBO_POWER_LIMIT
  0x640,        // MSR_SECONDARY_PLANE_TURBO_POWER_LIMIT
  0xFFFF
};

UINT64 BSP_MSR_REG_SAVE [(sizeof (CPU_MSR_REG) / 2)];
UINT64 AP_MSR_REG_SAVE [(sizeof (CPU_MSR_REG) / 2)][CPU_NUM];

UINT64 MSR_IA32_FEATURE_CONTROL_SAVE;          
UINT64 MSR_SMRR_PHYS_BASE_SAVE;                
UINT64 MSR_SMRR_PHYS_MASK_SAVE;                
UINT64 MSR_EMRR_PHYS_BASE_SAVE;
UINT64 MSR_EMRR_PHYS_MASK_SAVE;
UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_LOWER_SAVE;
UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_UPPER_SAVE;
UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_LOWER_SAVE;
UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_UPPER_SAVE;


UINT32 BSP_CR0_Register;
UINT32 AP_CR0_Register[CPU_NUM];

UINT64 CTL3_SAVE;
UINT32 CAP_Register;
UINT8  PI_Register;

//*******************************************************************************************
//
//  MTRR registers
//
UINT32
FixedMtrr[] = {
        0x250,                           // EFI_IA32_MTRR_FIX64K_00000,
        0x258,                           // EFI_IA32_MTRR_FIX16K_80000,
        0x259,                           // EFI_IA32_MTRR_FIX16K_A0000,
        0x268,                           // EFI_IA32_MTRR_FIX4K_C0000,
        0x269,                           // EFI_IA32_MTRR_FIX4K_C8000,
        0x26A,                           // EFI_IA32_MTRR_FIX4K_D0000,
        0x26B,                           // EFI_IA32_MTRR_FIX4K_D8000,
        0x26C,                           // EFI_IA32_MTRR_FIX4K_E0000,
        0x26D,                           // EFI_IA32_MTRR_FIX4K_E8000,
        0x26E,                           // EFI_IA32_MTRR_FIX4K_F0000,
        0x26F,                           // EFI_IA32_MTRR_FIX4K_F8000,
        0xFFFFFFFF
};

UINT64 FixedMtrrSave [sizeof (FixedMtrr) / 4];
UINT64 VariableMtrrSave [EFI_CACHE_VARIABLE_MTRR_END - CACHE_VARIABLE_MTRR_BASE + 1];

//*******************************************************************************************
UINT32
DMIBAR_IO [] = {
  0x0024, 0x002C, 0x0050,
  0x0060,
  0x031C, 
  0x0ED0,
  0x0204, 0x0EB4,
  0x0014, 0x0058, 0x0068, 0x00F0, 0x00FC, 0x0200,
  0x0208,
  0xffffffff
};

UINT32 DMIBAR_IO_SAVE [sizeof (DMIBAR_IO) / 4];

//[-start-121218-IB08050189-remove]//
//UINT8 RCBABAR_IO_SAVE [0x4000];
//[-end-121218-IB08050189-remove]//

UINT32
MCHBAR_IO [] = {
  0x0F00, 0x0F04, 0x0F08, 0x0F0C,  
  0x0F10, 0x0F60, 0x0F80, 0x0FB0, 0x0FB8, 0x0FC0, 0x0FFC,
  0xffffffff
};

UINT32 MCHBAR_IO_SAVE  [sizeof (MCHBAR_IO) / 4];

UINT32
AHCIBAR_IO [] = {
  //
  // Generic Host Controller Register
  //
  0x0000,    // [0000h-0003h] Host Capabilities
  0x0004,    // [0004h-0007h] Global PCH Control
  //
  // Port 0 port control registers
  //
  0x0100,    // [0100h-0103h] Port 0 Command List Base Address
  0x0108,    // [0108h-010Bh] Port 0 FIS Base Address
  0x0118,    // [0118h-011Bh] Port 0 Command
  0x012C,    // [012Ch-012Fh] Port 0 Serial ATA Control
  //
  // Port 1 port control registers
  //
  0x0180,    // [0180h-0183h] Port 1 Command List Base Address
  0x0188,    // [0188h-018Bh] Port 1 FIS Base Address
  0x0198,    // [0198h-019Bh] Port 1 Command
  0x01AC,    // [01ACh-01AFh] Port 1 Serial ATA Control
  //
  // Port 2 port control registers
  //
  0x0200,    // [0200h-0203h] Port 2 Command List Base Address
  0x0208,    // [0208h-020Bh] Port 2 FIS Base Address
  0x0218,    // [0218h-021Bh] Port 2 Command
  0x022C,    // [022Ch-022Fh] Port 2 Serial ATA Control
  //
  // Port 3 port control registers
  //
  0x0280,    // [0280h-0283h] Port 3 Command List Base Address
  0x0288,    // [0288h-028Bh] Port 3 FIS Base Address
  0x0298,    // [0298h-029Bh] Port 3 Command
  0x02AC,    // [02ACh-02AFh] Port 3 Serial ATA Control
  //
  // Port 4 port control registers
  //
//[-start-121120-IB05280016-remove]//
////[-start-120718-IB05330360-add]//
//#ifndef ULT_SUPPORT
////[-end-120718-IB05330360-add]//
//[-end-121120-IB05280016-remove]//
  0x0300,    // [0300h-0303h] Port 4 Command List Base Address
  0x0308,    // [0308h-030Bh] Port 4 FIS Base Address
  0x0318,    // [0318h-031Bh] Port 4 Command
  0x032C,    // [032Ch-032Fh] Port 4 Serial ATA Control
  //
  // Port 5 port control registers
  //
  0x0380,    // [0380h-0383h] Port 5 Command List Base Address
  0x0388,    // [0388h-038Bh] Port 5 FIS Base Address
  0x0398,    // [0398h-039Bh] Port 5 Command
  0x03AC,    // [03ACh-03AFh] Port 5 Serial ATA Control
//[-start-121120-IB05280016-remove]//
////[-start-120718-IB05330360-add]//
//#endif  
////[-end-120718-IB05330360-add]//
//[-end-121120-IB05280016-remove]//
  0xffffffff
};
UINT32 AHCIBAR_IO_SAVE  [sizeof (AHCIBAR_IO) / 4];

//[-start-121120-IB05280016-add]//
UINT32
AHCIBAR_IO_ULT [] = {
  //
  // Generic Host Controller Register
  //
  0x0000,    // [0000h-0003h] Host Capabilities
  0x0004,    // [0004h-0007h] Global PCH Control
  //
  // Port 0 port control registers
  //
  0x0100,    // [0100h-0103h] Port 0 Command List Base Address
  0x0108,    // [0108h-010Bh] Port 0 FIS Base Address
  0x0118,    // [0118h-011Bh] Port 0 Command
  0x012C,    // [012Ch-012Fh] Port 0 Serial ATA Control
  //
  // Port 1 port control registers
  //
  0x0180,    // [0180h-0183h] Port 1 Command List Base Address
  0x0188,    // [0188h-018Bh] Port 1 FIS Base Address
  0x0198,    // [0198h-019Bh] Port 1 Command
  0x01AC,    // [01ACh-01AFh] Port 1 Serial ATA Control
  //
  // Port 2 port control registers
  //
  0x0200,    // [0200h-0203h] Port 2 Command List Base Address
  0x0208,    // [0208h-020Bh] Port 2 FIS Base Address
  0x0218,    // [0218h-021Bh] Port 2 Command
  0x022C,    // [022Ch-022Fh] Port 2 Serial ATA Control
  //
  // Port 3 port control registers
  //
  0x0280,    // [0280h-0283h] Port 3 Command List Base Address
  0x0288,    // [0288h-028Bh] Port 3 FIS Base Address
  0x0298,    // [0298h-029Bh] Port 3 Command
  0x02AC,    // [02ACh-02AFh] Port 3 Serial ATA Control
  0xffffffff
};
UINT32 AHCIBAR_IO_SAVE_ULT  [sizeof (AHCIBAR_IO_ULT) / 4];
//[-end-121120-IB05280016-add]//


UINT32
EPBAR_IO [] = {
  0x0014, 0x001C, 0x0020, 0x0028, 0x002C, 0x0038,
  0x003C, 0x0044, 0x0050, 0x0058, 0x0100, 0x0104,
  0x0108, 0x010C, 0x0110, 0x0114, 0x0118, 0x011C,
  0xffffffff
};
UINT32 EPBAR_IO_SAVE  [sizeof (EPBAR_IO) / 4];

UINT32 AZALIA_MMIO [] = {
  0x0044, 0x0114, 0x0120,
  0xFFFFFFFF
};
UINT32 AZALIA_MMIO_SAVE [sizeof (AZALIA_MMIO)/4];

UINT16
MsrIndex [] = {
  0x15F, 0x19B, 0xFFFF
};

UINT64 *MsrValue [2];

//[-start-121205-IB08050187-add]//
UINT32  SPI_MMIO [] = {
          R_PCH_SPI_VSCC1,
          R_PCH_SPI_VSCC0,
          R_PCH_SPI_OPMENU + 4,
          R_PCH_SPI_OPMENU,
          R_PCH_SPI_PREOP,
          R_PCH_SPI_SSFS,
          R_PCH_SPI_HSFS,
          0xFFFFFFFF
          };

UINT32 SPI_MMIO_SAVE [sizeof (SPI_MMIO) / 4];
//[-end-121205-IB08050187-add]//

