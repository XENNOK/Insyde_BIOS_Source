/** @file
  Common Definitions for PchUfsPei module

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_PCH_UFS_H
#define _PEI_PCH_UFS_H

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <IndustryStandard/Pci.h>
//
// Driver Produced PPI Prototypes
//
#include <Ppi/UfsController.h>

//
// Driver Consumed PPI Prototypes
//
#include <Ppi/PciCfg2.h>
#include <Ppi/EndOfPeiPhase.h>

#include <PchRegs.h>

#define PEI_UFS_SIGNATURE                  SIGNATURE_32 ('p', 'u', 'f', 's')

#define PEI_UFS_DEVICE_FROM_THIS(a)        CR(a, PEI_UFS_DEVICE, UfsControllerPpi, PEI_UFS_SIGNATURE)

typedef struct _PEI_UFS_DEVICE PEI_UFS_DEVICE;

#define UFS_BUS_NUMBER                     0x01
#define UFS_FUNCTION_NUMBER                0x00

#define UFS_MEM_BASE_ADDRESS               0xC4000000

#define ICH_NUMBER_OF_PCIE_BRIDGES         6
#define ICH_PCIE_BRIDGE_ADDRESS            0x010000

#define COMMAND_REGISTER                   0x04
#define CLASSC_REGISTER                    0x08
#define BASE_ADDRESS_REGISTER_0            0x10
#define BASE_ADDRESS_REGISTER_1            0x14

#define ROOT_BRIDGE_BUS_REGISTER           0x18
#define ROOT_BRIDGE_ADDRESS_REGISTER       0x20

typedef struct _PEI_UFS_DEVICE {
  UINTN                    Signature;
  PEI_UFS_CONTROLLER_PPI   UfsControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR   PpiList;
  EFI_PEI_PCI_CFG2_PPI     *PciCfgPpi;
  UINT32                   TotalUfsControllers;
  EFI_PHYSICAL_ADDRESS     MemBase;
};

#endif
