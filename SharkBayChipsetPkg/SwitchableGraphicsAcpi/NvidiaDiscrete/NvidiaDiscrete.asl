/** @file
  nVIDIA discrete only mode SSDT sample.

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

#include <NvidiaDiscrete.asi>

DefinitionBlock (
    "NvidiaDiscrete.aml",
    "SSDT",
    1,
    "Insyde",
    "NvdPegTb",
    0x1000
    )
{
    External (\_SB.PCI0, DeviceObj)
    External (DGPU_BRIDGE_SCOPE, DeviceObj)
    External (DGPU2_BRIDGE_SCOPE, DeviceObj)
    External (CPU0_SCOPE._PSS, BuffObj)
    External (CPU0_SCOPE._PPC, IntObj)
    External (CPU1_SCOPE, DeviceObj)
    External (CPU2_SCOPE, DeviceObj)
    External (CPU3_SCOPE, DeviceObj)
    External (CPU4_SCOPE, DeviceObj)
    External (CPU5_SCOPE, DeviceObj)
    External (CPU6_SCOPE, DeviceObj)
    External (CPU7_SCOPE, DeviceObj)
    External (EC_SCOPE.BRTS)
    External (\_TZ.TZ01._TMP)
    External (\GPRW, MethodObj)
    External (IGDS, FieldUnitObj)
    External (BRTL, FieldUnitObj)
    External (OSYS) // Note: According to the OS type to change the base brightness levels, Win8 = 10, Win7 = 7.
    External (TCNT) // Number of Enabled Threads
    External (P8XH, MethodObj)
    External (MBGS, MethodObj)
    External (DD2H, MethodObj)
//[-start-130523-IB05160449-modify]//
//[-start-130812-IB05670190-modify]
#if !FeaturePcdGet(PcdUltFlag)
//[-end-130812-IB05670190-modify]
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

    #include <../OpRegion/VbiosOpRegion.asl>
    #include <../OpRegion/NvidiaOpRegion.asl>

    Include ("NvGps.asl")
    Include ("NvDgpu.asl")
    Include ("NvDgpu2.asl")
    Include ("NvPort.asl")
    Include ("NvWmi.asl")
}