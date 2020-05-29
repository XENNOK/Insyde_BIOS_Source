//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/*++

Copyright (c) 2004 - 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  CpuRegs.h

Abstract:

--*/

#ifndef _CPU_CSR_H
#define _CPU_CSR_H

#include "SaCommonDefinitions.h"


//
// Device 0,  Function 0
//
#define CPU_D0F0_PCIE_ADDR(Bus, Offset)      PCI_EXPRESS_LIB_ADDRESS(Bus, 0, 0, Offset)

#define CSR_DESIRED_CORES                    0x0080
#define   B_CSR_DESIRED_CORES_CORE_CNT        (BIT0 | BIT1)
#define   B_CSR_DESIRED_CORES_SMT_DIS          BIT8
#define   B_CSR_DESIRED_CORES_LOCK             BIT16


//
// Device 0,  Function 1
//
#define CPU_D0F1_PCIE_ADDR(Bus, Offset)      PCI_EXPRESS_LIB_ADDRESS(Bus, 0, 1, Offset)

#define GQ1_CR_PCIEXBAR                      0x0050

#endif
