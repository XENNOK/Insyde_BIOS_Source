/** @file
  AMD discrete only mode SSDT sample.

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

#include <AmdDiscrete.asi>

DefinitionBlock (
    "AmdDiscrete.aml",
    "SSDT",
    1,
    "Insyde",
    "AmdPegTb",
    0x1000
    )
{
    External (DGPU_BRIDGE_SCOPE, DeviceObj)
    External (DGPU2_BRIDGE_SCOPE, DeviceObj)
    External (EC_SCOPE.LIDP)
    External (EC_SCOPE.BRTS)
    External (\GPRW, MethodObj)
    External (IGDS, FieldUnitObj)
    External (BRTL, FieldUnitObj)
    External (OSYS) // Note: According to the OS type to change the base brightness levels, Win8 = 10, Win7 = 7.
    External (P8XH, MethodObj)
    External (MBGS, MethodObj)
    External (DD2H, MethodObj)
//[-start-130523-IB05160449-modify]//
//[-start-130812-IB05670190-modify]//
#if !FeaturePcdGet(PcdUltFlag)
//[-end-130812-IB05670190-modify]//
//[-end-130523-IB05160449-modify]//
    External (DGPU_SCOPE, DeviceObj)
#else
    Scope (DGPU_BRIDGE_SCOPE)
    {
        Device (DGPU_DEVICE)
        {
            Name (_ADR, 0x00000000)
            Method (_PRW, 0)
            {
                Return (GPRW (0x09, 4)) // can wakeup from S4 state
            }
        }
    }
#endif

    #include <../OpRegion/AmdOpRegion.asl>

    Include ("AmdDgpu.asl")
    Include ("AmdDgpu2.asl")
    Include ("AmdPort.asl")
    Include ("AmdAtif.asl")
}
