/** @file
  Library functions that can be called in both PEI and DXE phase

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
#ifndef _CPU_LIB_H_
#define _CPU_LIB_H_

#if 0
UINTN
CpuReadCr0 (
  VOID
  )
/**
@brief
  Get CR0 value

  @param[in] None

  @retval CR0 value
**/
;

VOID
CpuWriteCr0 (
  UINTN   Value
  )
/**
@brief
  Write CR0 register

  @param[in] Value - Value that will be written into CR0 register
**/
;

UINTN
CpuReadCr3 (
  VOID
  )
/**
@brief
  Get CR3 register value

  @param[in] None

  @retval CR3 register value
**/
;

VOID
CpuWriteCr3 (
  UINTN   Value
  )
/**
@brief
  Write CR3 register

  @param[in] Value - Value that will be written to CR3 register
**/
;

UINTN
CpuSetPower2 (
  IN  UINTN   Input
  )
/**
@brief
  Calculate the power 2 value from the Input value

  @param[in] Input - The number that will be calculated

  @retval Power 2 value after calculated
**/
;

UINT64
CpuReadTsc (
  VOID
  )
/**
@brief
  Read CPU TSC value

  @param[in] None

  @retval TSC value
**/
;

VOID
CpuBreak (
  VOID
  )
/**
@brief
  Break by INT3

  @param[in] None
**/
;

VOID
CpuInitSelectors (
  VOID
  )
/**
@brief
  Initialize selectors by calling INT 68

  @param[in] None
**/
;

#endif

UINT16
CpuCodeSegment (
  VOID
  )
/**
@brief
  Return code segment address - CS

  @param[in] None

  @retval Code segment address
**/
;

VOID
CpuLoadGlobalDescriptorTable (
  VOID  *Table16ByteAligned
  )
/**
@brief
  Get current GDT descriptor

  @param[in] Table16ByteAligned  - the table buffer that will store current GDT table descriptor
**/
;

VOID
CpuLoadInterruptDescriptorTable (
  VOID  *Table16ByteAligned
  )
/**
@brief
  Get current IDT descriptor

  @param[in] Table16ByteAligned  - the table buffer that will store current GDT table descriptor
**/
;

#endif
