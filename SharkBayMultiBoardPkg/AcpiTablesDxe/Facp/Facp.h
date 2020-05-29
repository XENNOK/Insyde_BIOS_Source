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
#ifndef _FACP_H_
#define _FACP_H_

#include <AcpiHeaderDefaultValue.h>
#include <SmiTable.h>
//[-start-120723-IB06460424-add]//
//#include <BaseAddrDef.h>
//[-end-120723-IB06460424-add]//

//
// ACPI table information used to initialize tables.
//
#define INT_MODEL        	0x01
#define RESERVED        	0x00
#define SCI_INT_VECTOR  	0x0009
#define SMI_CMD_IO_PORT 	FixedPcdGet16(PcdSoftwareSmiPort)
#define ACPI_ENABLE     	EFI_ACPI_ENABLE_SW_SMI
#define ACPI_DISABLE    	EFI_ACPI_DISABLE_SW_SMI
#define S4BIOS_REQ      	0x00
//[-start-120723-IB06460424-modify]//
#define PM1a_EVT_BLK    	FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress)
#define PM1b_EVT_BLK    	0x00000000
#define PM1a_CNT_BLK    	FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x00000004
#define PM1b_CNT_BLK    	0x00000000
#define PM2_CNT_BLK     	FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x00000050
#define PM_TMR_BLK      	FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x00000008
//[-start-120809-IB10370015-modify]//
//[-start-130523-IB05160449-modify]//
#ifdef ULT_SUPPORT
//[-end-130523-IB05160449-modify]//
#define GPE0_BLK        	FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x00000080
#else
#define GPE0_BLK        	FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress) + 0x00000020
#endif // ULT_SUPPORT
//[-end-120809-IB10370015-modify]//
#define GPE1_BLK        	0x00000000
//[-end-120723-IB06460424-modify]//
#define PM1_EVT_LEN     	0x04
#define PM1_CNT_LEN     	0x02
#define PM2_CNT_LEN     	0x01
#define PM_TM_LEN       	0x04
//[-start-120809-IB10370015-modify]//
//[-start-130523-IB05160449-modify]//
#ifdef ULT_SUPPORT
//[-end-130523-IB05160449-modify]//
#define GPE0_BLK_LEN    	0x20
#else
#define GPE0_BLK_LEN    	0x10
#endif // ULT_SUPPORT
//[-end-120809-IB10370015-modify]//
#define GPE1_BLK_LEN    	0x00
#define GPE1_BASE       	0x00
#define P_LVL2_LAT      	101
#define P_LVL3_LAT      	1001
#define FLUSH_SIZE      	0x400 //0x0000
#define FLUSH_STRIDE    	0x10  //0x0000
#define DUTY_OFFSET     	0x01
#define DUTY_WIDTH      	0x03
#define DAY_ALRM        	0x0D
#define MON_ALRM        	0x00
#define CENTURY         	0x00
#define FLAG            	EFI_ACPI_1_0_WBINVD | EFI_ACPI_1_0_PROC_C1 | EFI_ACPI_1_0_SLP_BUTTON | EFI_ACPI_1_0_RTC_S4 | EFI_ACPI_1_0_RESET_REG_SUP | EFI_USE_PLATFORM_CLOCK | EFI_ACPI_1_0_S4_RTC_STS_VALID | EFI_ACPI_1_0_REMOTE_POWER_ON_CAPABLE
#define EFI_USE_PLATFORM_CLOCK  (1 << 15)

#endif
