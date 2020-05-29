;; @file
;  Define for Mp
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

VacantFlag                    Equ   00h
NotVacantFlag                 Equ   0ffh
BreakToRunApSignal            Equ   6E755200h

MonitorFilterSize             Equ   40h
WakeUpApCounterInit           Equ   0
WakeUpApPerHltLoop            Equ   1
WakeUpApPerMwaitLoop          Equ   2
WakeUpApPerRunLoop            Equ   3
WakeUpApPerMwaitLoop32        Equ   4
WakeUpApPerRunLoop32          Equ   5

LockLocation                  equ        1000h - 0400h
StackStartAddressLocation     equ        LockLocation + 08h
StackSizeLocation             equ        LockLocation + 10h
CProcedureLocation            equ        LockLocation + 18h
GdtrLocation                  equ        LockLocation + 20h
IdtrLocation                  equ        LockLocation + 2Ah
BufferStartLocation           equ        LockLocation + 34h
Cr3OffsetLocation             equ        LockLocation + 38h
InitFlagLocation              equ        LockLocation + 3Ch
WakeUpApManner                equ        LockLocation + 40h
BistBuffer                    equ        LockLocation + 44h

PAUSE32   MACRO
            DB      0F3h
            DB      090h
            ENDM

;-------------------------------------------------------------------------------

