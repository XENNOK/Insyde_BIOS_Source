//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;

#ifndef _BASE_ADDRESS_DEFINITION_H_
#define _BASE_ADDRESS_DEFINITION_H_

//#include "PlatformBaseAddrDef.h"

//For PEI Recovery, ATA Controller Bus master base address definition
#ifdef  PLATFORM_PEI_ATA_BUS_MASTER_BASE_ADDRESS
#define PEI_ATA_BUS_MASTER_BASE_ADDRESS         PLATFORM_PEI_ATA_BUS_MASTER_BASE_ADDRESS
#else
#define PEI_ATA_BUS_MASTER_BASE_ADDRESS         0xFFA0
#endif

//PCIE MMIO base address : Accese PCI Configuration register through memory.
#ifdef  PLATFORM_PCICFG_MMIO_BASE_ADDRESS
#define PCICFG_MMIO_BASE_ADDRESS                PLATFORM_PCICFG_MMIO_BASE_ADDRESS
#else
#define PCICFG_MMIO_BASE_ADDRESS                0xE0000000
#endif

//ACPI IO base address : The IO Base for ACPI IO Block
#ifdef  PLATFORM_ACPI_IO_BASE_ADDRESS
#define ACPI_IO_BASE_ADDRESS                    PLATFORM_ACPI_IO_BASE_ADDRESS
#else
#define ACPI_IO_BASE_ADDRESS                    0x400
#endif

//SMBUS IO base address : The IO Base for SMBUS control Block
#ifdef  PLATFORM_SMBUS_BASE_ADDRESS
#define SMBUS_BASE_ADDRESS                      PLATFORM_SMBUS_BASE_ADDRESS
#else
#define SMBUS_BASE_ADDRESS                      0xEFA0
#endif
#if defined(SMBUS_CONTROLLER_BUS)&&defined(SMBUS_CONTROLLER_DEV)&&defined(SMBUS_CONTROLLER_FUNC)
#define SMBUS_BUS_DEV_FUNC                     (PEI_PCI_CFG_ADDRESS(SMBUS_CONTROLLER_BUS, SMBUS_CONTROLLER_DEV, SMBUS_CONTROLLER_FUNC , 0))
#else
#define SMBUS_BUS_DEV_FUNC                      0
#endif 
#ifdef  PLATFORM_IO_APIC_BASE_ADDRESS
#define IO_APIC_BASE_ADDRESS                    PLATFORM_IO_APIC_BASE_ADDRESS
#else
#define IO_APIC_BASE_ADDRESS                    0xFEC00000
#endif

#ifdef  PLATFORM_LOCAL_APIC_BASE_ADDRESS
#define LOCAL_APIC_BASE_ADDRESS                 PLATFORM_LOCAL_APIC_BASE_ADDRESS
#else
#define LOCAL_APIC_BASE_ADDRESS                 0xFEE00000
#endif

#ifdef  PLATFORM_HDA_BASE_ADDRESS
#define HDA_BASE_ADDRESS                        PLATFORM_HDA_BASE_ADDRESS
#else
#define HDA_BASE_ADDRESS                        0xFD000000
#endif

#endif

