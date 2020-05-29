;; @file
;  This is the equates file for HT (Hyper-threading) support
;
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

VacantFlag             Equ   00h
NotVacantFlag          Equ   0ffh

LockLocation                  equ        1000h - 0200h
StackStartAddressLocation     equ        LockLocation + 04h
StackSizeLocation             equ        LockLocation + 08h
CProcedureLocation            equ        LockLocation + 0Ch
GdtrLocation                  equ        LockLocation + 10h
IdtrLocation                  equ        LockLocation + 16h
BufferStartLocation           equ        LockLocation + 1Ch
PmodeOffsetLocation           equ        LockLocation + 20h
AcpiCpuDataAddressLocation    equ        LockLocation + 24h
MtrrValuesAddressLocation     equ        LockLocation + 28h
FinishedCountAddressLocation  equ        LockLocation + 2Ch
WakeupCountAddressLocation    equ        LockLocation + 30h
SerializeLockAddressLocation  equ        LockLocation + 34h
MicrocodeAddressLocation      equ        LockLocation + 38h
BootScriptAddressLocation     equ        LockLocation + 3Ch
StartStateLocation            equ        LockLocation + 40h
VirtualWireMode               equ        LockLocation + 44h
SemaphoreCheck                equ        LockLocation + 48h
PeiServices                   equ        LockLocation + 4Ch
PeiStall                      equ        LockLocation + 50h
CpuPerfCtrlValue              equ        LockLocation + 54h
CpuPlatformPolicyPpi          equ        LockLocation + 5Ch

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM

;-------------------------------------------------------------------------------
