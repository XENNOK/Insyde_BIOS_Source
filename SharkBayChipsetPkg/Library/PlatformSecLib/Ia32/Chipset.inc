;;******************************************************************************
;;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
;;*
;;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;;* transmit, broadcast, present, recite, release, license or otherwise exploit
;;* any part of this publication in any form, by any means, without the prior
;;* written permission of Insyde Software Corporation.
;;*
;;******************************************************************************
;
; This file contains a 'Sample Driver' and is licensed as such  
; under the terms of your license agreement with Intel or your  
; vendor.  This file may be modified by the user, subject to    
; the additional terms of the license agreement 
;
;------------------------------------------------------------------------------
;
; Copyright (c) 1999 - 2012, Intel Corporation. All rights reserved.<BR>
; This software and associated documentation (if any) is furnished
; under a license and may only be used or copied in accordance
; with the terms of the license. Except as permitted by such
; license, no part of this software or documentation may be
; reproduced, stored in a retrieval system, or transmitted in any
; form or by any means without the express written consent of
; Intel Corporation.
; 
; Module Name:
;
;   Chipset.inc
;
; Abstract:
;
;   Chipset constants and macros
;
;------------------------------------------------------------------------------

;
; APIC register
;
APICID                        EQU 0FEE00020h

;
; Power Management I/O Registers
;
;[-start-120723-IB06460424-modify];
;[-start-120628-IB04770237-modify];
PCH_ACPI_BASE_ADDRESS         EQU 1800h
;PCH_ACPI_BASE_ADDRESS         EQU 0500h
;[-end-120628-IB04770237-modify];
;[-end-120723-IB06460424-modify];
ACPI_PM1_STS                  EQU 000h
ACPI_PM1_CNT                  EQU 004h

;
; PCH RCBA base address
;
PCH_RCRB_BASE                 EQU 0FED1C000h
PCH_RCRB_BASE_REG             EQU 8000F8F0h       ; PCH Register B0:D31:RF0
PCH_RCRB_GCS                  EQU 03410h
PCH_RCRB_RTC_CONF             EQU 03400h
PCH_RCRB_RTC_CONF_UCMOS_EN    EQU 04h
PCH_RCRB_HPET                 EQU 03404h
PCH_RCRB_HPET_DECODE          EQU 080h

;
; HPET compare register
;
HPET_COMP_1                   EQU 0FED00108h
HPET_COMP_2                   EQU 0FED0010Ch
HPET_COMP_3                   EQU 0FED00128h
HPET_COMP_4                   EQU 0FED00148h

;
; MCH PCIe base address
;
;Need to match PcdPciExpressBaseAddress or PCIEX_BASE_ADDRESS
;[-start-120330-IB05300303-modify];
IFNDEF PLATFORM_PCIEX_BASE_ADDRESS
CPU_HEC_BASE                  EQU 0E0000000h      ; (4GB-512MB)
CPU_HEC_SIZE                  EQU PCIEXBAR_256MB
ELSE
IFNDEF PLATFORM_PCIEX_BASE_ADDRESS_64MB
CPU_HEC_BASE                  EQU 0F0000000h      ; (4GB-256MB)
CPU_HEC_SIZE                  EQU PCIEXBAR_128MB
ELSE
CPU_HEC_BASE                  EQU 0F0000000h      ; (4GB-256MB)
CPU_HEC_SIZE                  EQU PCIEXBAR_64MB
ENDIF
ENDIF
;[-end-120330-IB05300303-modify];
CPU_HEC_EN                    EQU 000000001h      ; Enable
CPU0_HEC_PCI_ADDR             EQU 080FF0150h
CPU1_HEC_PCI_ADDR             EQU 080FE0150h

PCI_LPC_BASE                  EQU 08000F800h

;[-start-120628-IB06460410-modify];
GPIO_BASE_ADDRESS             EQU 0800h
;[-end-120628-IB06460410-modify];
R_GPIO_USE_SEL2               EQU 030h
R_GPIO_IO_SEL2                EQU 034h
R_GPIO_LVL2                   EQU 038h

;
; PCI registers
;
PCH_LPC_PMBASE_PCI_ADDR       EQU ((1Fh * 8 + 00h) * 1000h + 0040h + CPU_HEC_BASE)
PCH_LPC_ACPICNTL_PCI_ADDR     EQU ((1Fh * 8 + 00h) * 1000h + 0044h + CPU_HEC_BASE)
PCH_LPC_GEN_PMCON_3_ADDR      EQU ((1Fh * 8 + 00h) * 1000h + 00A4h + CPU_HEC_BASE)
PCH_LPC_RCRB_PCI_ADDR         EQU ((1Fh * 8 + 00h) * 1000h + 00F0h + CPU_HEC_BASE)
PCH_LPC_BIOS_CNTL_PCI_ADDR    EQU ((1Fh * 8 + 00h) * 1000h + 00DCh + CPU_HEC_BASE)
MCH_UNCERRMASK_PCI_ADDR       EQU ((00h * 8 + 00h) * 1000h + 0108h + CPU_HEC_BASE)
MCH_SYRE_PCI_ADDR             EQU ((10h * 8 + 00h) * 1000h + 0040h + CPU_HEC_BASE)

SYRE_CPURST                   EQU 14

;
; PCIEXBAR constants for enable in bit [0]
;
ENABLE                        EQU 1

;
; PCIEXBAR constants for size in bit [2:1]
;
PCIEXBAR_64MB                 EQU 010b
PCIEXBAR_128MB                EQU 001b
PCIEXBAR_256MB                EQU 000b

MMCFG_BASE                    EQU CPU_HEC_BASE

DMIBAR_REG                    EQU (068h + CPU_HEC_BASE)
DMI_BASE_ADDRESS              EQU 0FED18000h

MCHBAR_REG                    EQU (048h + CPU_HEC_BASE)
MCH_BASE_ADDRESS              EQU 0FED10000h

