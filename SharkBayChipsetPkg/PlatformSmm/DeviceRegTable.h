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

#ifndef _DEVICE_REGISTER_TABLE_H
#define _DEVICE_REGISTER_TABLE_H

#include <Platform.h>

#define I8259_CTRL                        0x20
#define I8259A_CTRL                       0xA0
#define I8259_MASK                        0x21
#define I8259A_MASK                       0xA1


// PIC Buffer
extern UINT8 I8259Mask, I8259AMask;
extern UINT8 PIC1_ICW2, PIC2_ICW2;

typedef struct {
  UINTN               RegNum;
  UINT8               PciBus;
  UINT8               PciDev;
  UINT8               PciFun;
  UINT8               *PciRegTable;
  UINT32              *PciRegTableSave;
} SR_DEVICE;

typedef struct {
  UINT8                               Bus;
  UINT8                               Dev;
  UINT8                               Fun;
} P2P_BRIDGE;

typedef struct {
  SR_DEVICE                        Device;
  P2P_BRIDGE                       P2PB;
} SR_OEM_DEVICE;

typedef struct {
  SR_DEVICE                       *Chipset;
  SR_OEM_DEVICE                   *Oem;
} SR_TABLE;

typedef struct {
  UINT8               PciBus;
  UINT8               PciDev;
  UINT8               PciFun;
  UINT8               PciReg;
  UINT32              SaveResSVID;
} SAVE_RES_SVID;


//
// Pci device table list
//
extern SR_DEVICE PciDeviceSubResList [];

extern SR_TABLE  PciResTable;

//[-start-121120-IB05280016-add]//
extern SR_DEVICE PciDeviceSubResListUlt [];
extern SR_TABLE  PciResTableUlt;
//[-end-121120-IB05280016-add]//

//*******************************************************************************************

//[-start-121211-IB05300367-modify]//
//
// GPIO Register
//
extern UINT8  GPIO_REG      [];
extern UINT32 GPIO_REG_SAVE [];

extern UINT16 GPIO_REG_ULT      [];
extern UINT32 GPIO_REG_SAVE_ULT [];
//[-end-121211-IB05300367-modify]//
//*******************************************************************************************

//
//  Edge/Level Control Registers
//
#define ELCR_PORT                  0x4D0
extern UINT8  ELCR [];

//*******************************************************************************************

//
// PM Base IO Register
//
extern UINT8 PMIO_REG      [];
extern UINT8 PMIO_REG_SAVE [];

//*******************************************************************************************

//
//  CPU State
//
#define SMM_HANDLER_CR0                  0xAFFFC
#define BBL_CR_CTL3                      0x11E

extern UINT16 CPU_MSR_REG [];

extern UINT64 BSP_MSR_REG_SAVE [];
extern UINT64 AP_MSR_REG_SAVE  [][CPU_NUM];

extern UINT64 MSR_IA32_FEATURE_CONTROL_SAVE;
extern UINT64 MSR_SMRR_PHYS_BASE_SAVE;
extern UINT64 MSR_SMRR_PHYS_MASK_SAVE;
extern UINT64 MSR_EMRR_PHYS_BASE_SAVE;
extern UINT64 MSR_EMRR_PHYS_MASK_SAVE;
extern UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_LOWER_SAVE;
extern UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_BASE_UPPER_SAVE;
extern UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_LOWER_SAVE;
extern UINT32 SYSTEM_ADDRESS_DECODER_PCI_MCSEG_MASK_UPPER_SAVE;

extern UINT32 BSP_CR0_Register;
extern UINT32 AP_CR0_Register[];

extern UINT32 CAP_Register;
extern UINT8  PI_Register;
extern UINT64 CTL3_SAVE;

//*******************************************************************************************

//
//  MTRR registers
//
#define L2_ENABLE                       0x101
#define EFI_MTRR_DEF_TYPE_ENABLE        0xC00
//#define EFI_CACHE_VARIABLE_MTRR_END     0x213
//#define EFI_CACHE_VARIABLE_MTRR_BASE    0x200
//#define EFI_CACHE_IA32_MTRR_DEF_TYPE    0x2FF
#define EFI_MSR_IA32_APIC_BASE          0x1B
#define EFI_APIC_GLOBAL_ENABLE          0x800
//#define EFI_CACHE_MTRR_VALID            0x800

extern UINT32 FixedMtrr        [];

extern UINT64 FixedMtrrSave    [];
extern UINT64 VariableMtrrSave [];

//*******************************************************************************************

//
//  DMA Init
//
#define DMA1_BASE                   0
#define DMA2_BASE                   0xC0
#define DMA_CMD                     8
#define DMA_REQ                     9
#define DMA_MASK                    0xA
#define DMA_MODE                    0xB

#define FILL_PCI_REG_NUM            0x100

//*******************************************************************************************

extern UINT32 DMIBAR_IO        [];
extern UINT32 DMIBAR_IO_SAVE   [];

//[-start-121218-IB08050189-remove]//
//extern UINT8  RCBABAR_IO_SAVE  [];
//[-end-121218-IB08050189-remove]//

extern UINT32 MCHBAR_IO        [];
extern UINT32 MCHBAR_IO_SAVE   [];
extern UINT32 AHCIBAR_IO       [];
extern UINT32 AHCIBAR_IO_SAVE  [];
//[-start-121120-IB05280016-add]//
extern UINT32 AHCIBAR_IO_ULT   [];
extern UINT32 AHCIBAR_IO_SAVE_ULT [];
//[-end-121120-IB05280016-add]//
extern UINT32 EPBAR_IO         [];
extern UINT32 EPBAR_IO_SAVE    [];

extern UINT32 AZALIA_MMIO      [];
extern UINT32 AZALIA_MMIO_SAVE [];
extern UINT16 MsrIndex         [];
extern UINT64 *MsrValue        [];

extern UINT8  EDP_PCI_SAVE     [];

//[-start-121205-IB08050187-add]//
extern UINT32 SPI_MMIO         [];
extern UINT32 SPI_MMIO_SAVE    [];
//[-end-121205-IB08050187-add]//

//[-start-121029-IB10820145-add]//
#define FILL_PCI_REG_NUM            0x100
#define PCIE_PEG_BRIDGE {0x00, 0x01, 0x00}

UINT32 PEGPCI_Fun0 [FILL_PCI_REG_NUM];
UINT32 PEGPCI_Fun1 [FILL_PCI_REG_NUM];
//[-end-121029-IB10820145-add]//
#endif



