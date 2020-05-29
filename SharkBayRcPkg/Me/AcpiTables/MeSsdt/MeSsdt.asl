/** @file

  Me SSDT Table ASL code

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


DefinitionBlock (
  "MeSsdt.aml",
  "SSDT",
  0x01,
  "MeSsdt",
  "MeSsdt ",
  0x3000
  )
{

  OperationRegion(MENV,SystemMemory,0xFFFF0000,0xAA55)
  Field(MENV,AnyAcc,Lock,Preserve)
  {
  MERV, 32,     ///   (000) ME NVS Protocol Revision
  ///
  /// PTT Solution
  ///
  PTTS, 8,       ///   (004) PTT Solution Method Selection
  ///
  /// PTT Allocated Buffer Address
  ///
  PTTB, 64,      ///   (005) PTT Allocated Buffer Address

  }
}