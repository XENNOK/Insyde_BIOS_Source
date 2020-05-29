/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <AmdPowerXpress.asi>

DefinitionBlock (
    "AmdPowerXpress.aml",
    "SSDT",
    1,
    "Insyde",
    "AmdPch",
    0x1000
    )
{
    External (DGPU_BRIDGE_SCOPE, DeviceObj)
    External (DGPU_SCOPE, DeviceObj)
    External (IGPU_SCOPE, DeviceObj)
    External (DGPU_SCOPE._ADR, MethodObj)
    External (IGPU_SCOPE._DOD, MethodObj)
    External (IGPU_SCOPE.AINT, MethodObj)
    External (IGPU_SCOPE.DD01._ADR, MethodObj)
    External (IGPU_SCOPE.DD02._ADR, MethodObj)
    External (IGPU_SCOPE.DD03._ADR, MethodObj)
    External (IGPU_SCOPE.DD04._ADR, MethodObj)
    External (IGPU_SCOPE.DD05._ADR, MethodObj)
    External (IGPU_SCOPE.DD06._ADR, MethodObj)
    External (IGPU_SCOPE.DD07._ADR, MethodObj)
    External (IGPU_SCOPE.DD08._ADR, MethodObj)
    External (IGPU_SCOPE.DD01._DGS, MethodObj)
    External (IGPU_SCOPE.DD02._DGS, MethodObj)
    External (IGPU_SCOPE.DD03._DGS, MethodObj)
    External (IGPU_SCOPE.DD04._DGS, MethodObj)
    External (IGPU_SCOPE.DD05._DGS, MethodObj)
    External (IGPU_SCOPE.DD06._DGS, MethodObj)
    External (IGPU_SCOPE.DD07._DGS, MethodObj)
    External (IGPU_SCOPE.DD08._DGS, MethodObj)
    External (IGPU_SCOPE.DD02._DCS, MethodObj)
    External (IGPU_SCOPE.DD02._BCL, MethodObj)
    External (IGPU_SCOPE.DD02._BQC, MethodObj)
    External (IGPU_SCOPE.DD02._BCM, MethodObj)
    External (\_SB.PCI0.LPCB.H_EC.LSTE)
    External (\GPRW, MethodObj)
    External (\ECON, IntObj)
    External (PNHM, IntObj)
    External (P0UB, IntObj)
    External (DSEN)
    External (NXD1)
    External (NXD2)
    External (NXD3)
    External (NXD4)
    External (NXD5)
    External (NXD6)
    External (NXD7)
    External (NXD8)
    External (PEBS)
    External (P8XH, MethodObj)
    External (MBGS, MethodObj)
    External (DD2H, MethodObj)
    External (DGPU_SCOPE.DD02, DeviceObj)
    External (DGPU_SCOPE._OFF, MethodObj)
    External (DGPU_SCOPE._ON,  MethodObj)
    External (DGPU_SCOPE.SGPO, MethodObj)
    
    #include <../OpRegion/SgOpRegion.asl>
    #include <../OpRegion/AmdOpRegion.asl>
    
    Include ("PxPort.asl")
    Include ("Atpx.asl")
    Include ("Atif.asl")
}
