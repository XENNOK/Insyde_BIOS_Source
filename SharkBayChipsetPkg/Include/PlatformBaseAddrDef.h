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

#ifndef _PLATFORM_BASE_ADDRESS_DEFINITION_H_
#define _PLATFORM_BASE_ADDRESS_DEFINITION_H_

#ifndef PLATFORM_PCIEX_BASE_ADDRESS
#define MCH_PCI_EXPRESS_BASE_ADDRESS  0xE0000000
#else
#ifndef PLATFORM_PCIEX_BASE_ADDRESS_64MB
#define MCH_PCI_EXPRESS_BASE_ADDRESS  0xF0000000
#else
#define MCH_PCI_EXPRESS_BASE_ADDRESS  0xF0000000
#endif
#endif

//
// PCH relation  base address 
//
#ifdef  PLATFORM_PCIEX_BASE_ADDRESS
#define PLATFORM_PCICFG_MMIO_BASE_ADDRESS       EDKII_GLUE_PciExpressBaseAddress
#endif

#define PLATFORM_HDA_BASE_ADDRESS               0xF9000000
#define PLATFORM_IO_APIC_BASE_ADDRESS           0xFEC00000
#define PLATFORM_HPET_CONFIG_BASE_ADDRESS       0xFED00000
#define PCH_RCBA_ADDRESS                        0xFED1C000  // PCH Root complex base address
#define HECI_MMIO_ADDR                          0xFEDB0000  // HECI MMIO BASE Address
#define PLATFORM_LOCAL_APIC_BASE_ADDRESS        0xFEE00000

//[-start-120628-IB06460410-modify]//
#define PCH_GPIO_BASE_ADDRESS                   0x0800
//[-end-120628-IB06460410-modify]//
//[-start-120723-IB06460424-add]//
#define PLATFORM_ACPI_IO_BASE_ADDRESS           0x1800
//[-end-120723-IB06460424-add]//
#define PCH_SMBUS_BASE_ADDRESS                  0xEFA0
#define SIO_BASE_ADDRESS                        0x0600
#define SIO_BASE_ADDRESS2                       0x1640
  #define SIO_BASE_MASK                           0xFFF0
#define BMC_KCS_BASE_ADDRESS                    0x0CA0
  #define BMC_KCS_BASE_MASK                       0xFFF0

//
// MCH relation base address
//
#define DMI_BASE_ADDRESS                        0xFED18000  // DMI  Register Base Address
#define EP_BASE_ADDRESS                         0xFED19000  // EP   Register Base Address
#define TXT_PUBLIC_BASE                         0xFED30000
//[-start-120404-IB05300309-add]//
#define SA_ED_RAM_BASE_ADDRESS                  0xFED80000  // ED RAM Register Base Address
//[-end-120404-IB05300309-add]//
//[-start-120404-IB05300309-modify]//
//[-start-120320-IB06460377-add]//
#define SA_GDXC_BASE_ADDRESS                    0xFED84000
//[-end-120320-IB06460377-add]//
//[-end-120404-IB05300309-modify]//
//[-start-120911-IB10820121-remove]//
//#ifdef CRISIS_RECOVERY_SUPPORT
//[-end-120911-IB10820121-remove]//
#define PCH_UHCI_IO_BASE_ADDRESS                0x8000
#define PCH_EHCI_MEM_BASE_ADDRESS               0xD8000000
//[-start-120327-IB06460380-modify]//
//
// Temporary modify this address, due to PCH hardware issue in EHCI Bar.
// This might be removed at next stepping PCH.
//
//#define PCH_XHCI_MEM_BASE_ADDRESS               0xD8000800
#define PCH_XHCI_MEM_BASE_ADDRESS               0xD8200800
//[-end-120327-IB06460380-modify]//
//[-start-120911-IB10820121-remove]//
//#endif
//[-end-120911-IB10820121-remove]//


#endif

