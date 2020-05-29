/** @file
  Definition for EM64T processor

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/

#ifndef _PROCESSOR_DEF_H
#define _PROCESSOR_DEF_H

#pragma pack(1)

typedef struct {
  UINT16  Offset15To0;
  UINT16  SegmentSelector;
  UINT16  Attributes;
  UINT16  Offset31To16;
  UINT32  Offset63To32;
  UINT32  Reserved;
} INTERRUPT_GATE_DESCRIPTOR;

#pragma pack()

typedef struct {
  UINT8 *RendezvousFunnelAddress;
  UINTN PModeEntryOffset;
  UINTN FlatJumpOffset;
  UINTN LModeEntryOffset;
  UINTN LongJumpOffset;
  UINTN Size;
} MP_ASSEMBLY_ADDRESS_MAP;

/**

  Get address map of RendezvousFunnelProc.

  @param[in] AddressMap  - Output buffer for address map information


**/
VOID
AsmGetAddressMap (
  OUT MP_ASSEMBLY_ADDRESS_MAP    *AddressMap
  )
;

#endif
