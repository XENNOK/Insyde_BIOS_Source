/** @file
  When user enable PxeToLan.
  Control network stack behavior via callback.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PXE_OPTION_ROM_HEADER_H
#define _PXE_OPTION_ROM_HEADER_H

#pragma pack(1)

#define UNDI_SIGNATURE                 SIGNATURE_32 ('U', 'N', 'D', 'I')

///
/// Legacy PXE ROM Header Extensions
/// Section 4.3, Table 4-1, Preboot Execution Environment Specification V2.1
///
typedef struct {
  UINT16  Signature;    ///< 0xaa55
  UINT8   Size512;
  UINT8   InitEntryPoint[3];
  UINT8   Reserved[0x10];
  UINT16  PxeRomIdOffset;
  UINT16  PcirOffset;
  UINT16  PnpHeaderOffset;
} LEGACY_PXE_EXPANSION_ROM_HEADER;

///
/// Legacy PXE ROM Header Extensions
/// Section 4.5.1.1, Table 4-13, Preboot Execution Environment Specification V2.1///
typedef struct {
  UINT32  Signature;    ///< 'UNDI'
  UINT8   StructLength;
  UINT8   StructCheckSum;
  UINT8   StructRev;
  UINT8   UndiRev[3];
  UINT16  UndiLoader;
  UINT16  StackSize;
  UINT16  DataSize;
  UINT16  CodeSize;
  UINT8   BusType[1];
} UNDI_ROM_ID_STRUCTURE;

#pragma pack()

#endif //_PXE_OPTION_ROM_HEADER_H
