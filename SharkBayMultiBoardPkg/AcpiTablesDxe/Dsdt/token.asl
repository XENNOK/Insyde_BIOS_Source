//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/**************************************************************************;
;*                                                                        *;
;*    Intel Confidential                                                  *;
;*                                                                        *;
;*    Intel Corporation - ACPI Reference Code for the Haswell             *;
;*    Family of Customer Reference Boards.                                *;
;*                                                                        *;
;*                                                                        *;
;*    Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved   *;
;*    This software and associated documentation (if any) is furnished    *;
;*    under a license and may only be used or copied in accordance        *;
;*    with the terms of the license. Except as permitted by such          *;
;*    license, no part of this software or documentation may be           *;
;*    reproduced, stored in a retrieval system, or transmitted in any     *;
;*    form or by any means without the express written consent of         *;
;*    Intel Corporation.                                                  *;
;*                                                                        *;
;*                                                                        *;
;**************************************************************************/
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/

//[-start-120723-IB06460424-modify]//
//#include "BaseAddrDef.h"
//[-end-120723-IB06460424-modify]//

Name(SP2O, 0x4e)        // Super I/O (SMSC 1007) Index/Data configuration port for ASL.
Name(SP1O, 0x2e)	// Super I/O (SMSC 1007) Index/Data configuration port for ASL.
Name(IO1B, 0x600)       // Super I/O (SMSC 1007) GPIO base address
Name(IO1L, 0x70)        // Super I/O (SMSC 1007) GPIO base address length
Name(IO2B, 0x600)	// Super I/O (SMSC 1007) PME base address
Name(IO2L, 0x20)        // Super I/O (SMSC 1007) PME base address length
Name(IO3B, 0x290)       // Super I/O (SMSC 1007) HHM Base Address
Name(IO3L, 0x10)        // Super I/O (SMSC 1007) HHM base address length
Name(SP3O, 0x2e)        // Super I/O (Winbond 3xx) Index/Data configuration port for ASL.
Name(IO4B, 0xa20)       // Super I/O (Winbond 3xx) GPIO base address length
Name(IO4L, 0x20)        // Super I/O (Winbond 3xx) GPIO base address length
Name(HSCS, 0x1)         // Hitachi H8S2113D Super I/O COM port status.
Name(MCHB, 0xfed10000)  // MCH BAR address
Name(MCHL, 0x8000)      // MCH BAR length
Name(EGPB, 0xfed19000)  // EP BAR address
Name(EGPL, 0x1000)      // EP BAR length
Name(DMIB, 0xfed18000)  // DMI BAR address
Name(DMIL, 0x1000)      // DMI BAR length
Name(IFPB, 0xfed14000)  // Base addrerss of 4K MMIO Window any write to which will FLUSH GMCH Global Write Buffer to let software
                        // guarantee coherency between writes from an isochronous agent and writes from the CPU.
Name(IFPL, 0x1000)      // Length of the MMIO region above.
//[-start-130812-IB05670190-modify]//
Name(PEBS, FixedPcdGet32(PcdPciExpressBaseAddress))
#if FixedPcdGet32(PcdPciExpressSize) == 256
Name(PELN, 0x10000000)
#elif FixedPcdGet32(PcdPciExpressSize) == 128
Name(PELN, 0x8000000)
#elif FixedPcdGet32(PcdPciExpressSize) == 64
Name(PELN, 0x4000000)
#else                    // 32 MB
Name(PELN, 0x2000000)
#endif
//[-end-130812-IB05670190-modify]//
Name(TTTB, 0xfed20000)  //
Name(TTTL, 0x20000) //
Name(SMBS, 0xefa0)      // SMBus I/O Registes Base Address
Name(SMBL, 0x10)        // SMBus I/O Registes Range
Name(PBLK, 0x1810)       // Boot-strap CPU Control Block register address. Use return value of PM_BASE_ADDRESS with proper offset.
//[-start-120723-IB06460424-modify]//
//Name(PMBS, ACPI_IO_BASE_ADDRESS)       // ASL alias for ACPI I/O base address.
Name(PMBS, FixedPcdGet16(PcdPerfPkgAcpiIoPortBaseAddress))       // ASL alias for ACPI I/O base address.
Name(PMLN, 0x100)  // Power Management registers block length
//[-end-120723-IB06460424-modify]//
Name(LVL2, 0x1814) // Processor LVL2 register I/O address.
Name(LVL3, 0x1815) // Processor LVL3 register I/O address.
Name(LVL4, 0x1816) // Processor LVL4 register I/O address.
Name(SMIP, 0xb2)  // I/O port to triggle SMI
Name(GPBS, FixedPcdGet32(PcdPchGpioBaseAddress))  // GPIO Register Block address
Name(GPLN, 0x400) // GPIO Register Block length
Name(APCB, 0xfec00000)  // Default I/O APIC(s) memory start address, 0x0FEC00000 - default, 0 - I/O APIC's disabled
Name(APCL, 0x1000)  // I/O APIC(s) memory decoded range, 0x1000 - default, 0 - I/O APIC's not decoded
Name(PM30, 0x1830) // SMI Control Register
Name(SRCB, 0xfed1c000)  // RCBA (Root Complex Base Address)
Name(SRCL, 0x4000)  // RCBA length
Name(HPTB, 0xfed00000)  // Same as HPET_BASE_ADDRESS for ASL use
Name(HPTC, 0xfed1f404)  // High Precision Event Timer Configuration Register
Name(ACPH, 0xde)        // North Bridge Scratchpad Data Register for patch ACPI.
Name(ASSB, 0x0)         // ACPI Sleep State Buffer for BIOS Usage.
Name(AOTB, 0x0)         // ACPI OS Type Buffer for BIOS Usage.
Name(AAXB, 0x0)         // ACPI Auxiliary Buffer for BIOS Usage.
Name(PEHP, 0x1)         // _OSC: Pci Express Native Hot Plug Control
Name(SHPC, 0x1)         // _OSC: Standard Hot Plug Controller (SHPC) Native Hot Plug control
Name(PEPM, 0x1)         // _OSC: Pci Express Native Power Management Events control
Name(PEER, 0x1)         // _OSC: Pci Express Advanced Error Reporting control
Name(PECS, 0x1)         // _OSC: Pci Express Capability Structure control
Name(ITKE, 0x0) // This will be overridden by the ITK module.
Name(DSSP, 0x0) // ON - Disable SATA Software Setting Preservation feature so that it will not preserve the state after hardware reset. i.e. After a hardware reset HDD will change security state as power-on. OFF - Do not change it in POST, leave it as default (enabled).
Name(FHPP, 0x0) // Set to 1 to always prompt for HDD password (in AHCI mode).
Name(FMBL, 0x1)         // Platform Flavor - Mobile flavor for ASL code.
Name(FDTP, 0x2)         // Platform Flavor - Desktop flavor for ASL code.
Name(FUPS, 0x3) // Platform Flavor - UP Server flavor for ASL code.
Name(FEMD, 0x4)         // Platform Flavor - Embedded flavor for ASL code.
Name(BGR, 0x1)          // Haswell - Grays Reef Board ID for ASL code.
Name(BFR, 0x2)          // Haswell - Flathead Rock Board ID for ASL code.
Name(BBR, 0x3)          // Haswell - Basking Ridge Board ID for ASL code.
Name(BWC, 0x4)          // Haswell - Walnut Canyon Board ID for ASL code.
Name(BGRP, 0x9)         // Haswell - Grays Reef PPV Board ID for ASL code.
Name(BGRC, 0xa)         // Haswell - Grays Reef CPV Board ID for ASL code.
Name(BFRP, 0xb)         // Haswell - Flathead Rock PPV Board ID for ASL code.
Name(BBRP, 0xc)         // Haswell - Basking Ridge PPV Board ID for ASL code.
Name(BRPS, 0x10)        // Haswell - STHIPPVRSVD1 Board ID for ASL code.
Name(BRCS, 0x11)        // Haswell - STHICPVRSVD2 Board ID for ASL code.
Name(BRDS, 0x12)        // Haswell - STHIDOERSVD3 Board ID for ASL code.
Name(BR4S, 0x13)        // Haswell - STHIPPVRSVD4 Board ID for ASL code.
Name(BR5S, 0x14)        // Haswell - STHIPPVRSVD5 Board ID for ASL code.
Name(BWT1, 0x20)         // Haswell - WhiteTip Mountain1  Board ID for ASL code.
Name(BW1P, 0x21)         // Haswell - WhiteTip Mountain1  PPV Board ID for ASL code.
Name(BW2C, 0x22)         // Haswell - WhiteTip Mountain2  Board ID for ASL code.
Name(BW2P, 0x23)         // Haswell - WhiteTip Mountain2  PPV Board ID for ASL code.
Name(BSPC, 0x24)         // Haswell - Sawtooth Peak  Board ID for ASL code.
Name(BSPP, 0x25)         // Haswell - Sawtooth Peak  PPV Board ID for ASL code.
Name(BTS, 0x26)          // Haswell  - TopSham Board ID for ASL code.
Name(BICO, 0x27)         // Haswell  - WhiteTip Mountain2 CDK Open Chasis Board ID for ASL code.
Name(BICC, 0x28)         // Haswell  - WhiteTip Mountain2 CDK Closed Chasis Board ID for ASL code.
Name(BHB, 0x30)          // Haswell - Harris Beach FFRD Board ID for ASL code.
Name(BFS2, 0x31)         // Haswell - FFRD SKU2 Board ID for ASL code.
Name(BFS3, 0x32)         // Haswell - FFRD SKU3 Board ID for ASL code.
Name(BFS4, 0x33)         // Haswell - FFRD SKU4 Board ID for ASL code.
Name(BRH, 0x35)          // Haswell - Reed Harbor TDV Board ID for ASL code.

Name(BFHC, 0x40)        // Haswell - Flathead Creek Board ID for ASL code
Name(BFD1, 0x41)        // Haswell - Flathead Creek DOE1 Board ID for ASL code.
Name(BFD2, 0x42)        // Haswell - Flathead Creek DOE2 Board ID for ASL code.
Name(BFCC, 0x43)        // Haswell - Flathead Creek CRB Board ID for ASL code.
Name(BHCP, 0x44)        // Haswell - Haddock Creek Board ID for ASL code.
Name(BCOC, 0x45)        // Haswell - CRB OC Board ID for ASL code.
Name(BFHP, 0x46)        // Haswell - Flathead Creek PPV Board ID for ASL code.
Name(BFCP, 0x47)         // Haswell - Flathead Creek CPV Board ID for ASL code.
Name(BTDT, 0x50)        // BidwellBar - TumaloFalls DT Board ID for ASL code.
Name(BTMB, 0x51)        // BidwellBar - TumaloFalls MB Board ID for ASL code.
Name(BBGB, 0x52)        // Big Creek MB Board ID for ASL code.
Name(BHC2, 0x53)        // Haswell - Haddock Creek Fab2 Board ID for ASL code.

Name(TCGM, 0x1)         // Flag to check TCG Module support in platform.asl
Name(TRTP, 0x1)         // PPM IO Trap flag
Name(WDTE, 0x1)         // Flag for WDT enabled/disabled
Name(TRTD, 0x2)         // DTS IO Trap flag
Name(TRTI, 0x3)         // IGD IO Trap flag
Name(PFTI, 0x4)         // PFAT IO Trap flag
Name(GCDD, 0x1) // SW_SMI_GET_CURRENT_DISPLAY_DEVICE
Name(DSTA, 0xa) // SW_SMI_DISPLAY_SWITCH_TOGGLE_ACPI
Name(DSLO, 0xc) // SW_SMI_DISPLAY_SWITCH_LID_OPEN_ACPI
Name(DSLC, 0xe) // SW_SMI_DISPLAY_SWITCH_LID_CLOSE_ACPI
Name(PITS, 0x10)  // SW_SMI_POPUP_ICON_TOGGLE
Name(SBCS, 0x12)  // SW_SMI_SET_BACKLIGHT_CONTROL
Name(SALS, 0x13)  // SW_SMI_SET_ALI_LEVEL
Name(LSSS, 0x2a)  // SW_SMI_LID_STATE_SWITCH
Name(SOOT, 0x35)  // SW_SMI_SAVE_OSB_OS_TYPE
Name(PDBR, 0x4d)  // SW_SMI_PCIE_DOCK_BRIDGE_RESOURCE_PATCH
Name(DPPB, 0xfed98000)  // Thermal Solution Resource Base Address
Name(DPPL, 0x8000)  // Length of Thermal Solution Resource Base Address
