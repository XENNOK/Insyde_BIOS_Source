/** @file

  Definition of the MEFW global NVS area protocol.  This protocol
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM/DXE/PEI code and ASL code.
  @todo The format is derived from the ACPI reference code, version 0.95.

  Note:  Data structures defined in this protocol are not naturally aligned.

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

#ifndef _ME_GLOBAL_NVS_AREA_H_
#define _ME_GLOBAL_NVS_AREA_H_

///
/// Includes
///
///
/// Forward reference for pure ANSI compatability
///
typedef struct _ME_GLOBAL_NVS_AREA_PROTOCOL ME_GLOBAL_NVS_AREA_PROTOCOL;

///
/// ME Global NVS Area Protocol GUID
///
#define ME_GLOBAL_NVS_AREA_PROTOCOL_GUID \
  { 0x3BFFECFD, 0xD75F, 0x4975, { 0xB8, 0x88, 0x39, 0x02, 0xBD, 0x69, 0x00, 0x2B }}

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gMeGlobalNvsAreaProtocolGuid;

///
/// Global NVS Area definition
///
#pragma pack(1)
typedef struct {
  UINT32      MeNvsRevision;                                ///< 000 Me NVS Protocol Revision
  ///
  /// PTT Solution
  ///
  UINT8       FTpmSwitch;                                  /// (004) fTPM Solution Method Selection
  ///
  /// PTT Allocated Buffer Address
  ///
  UINT64      PTTAddress;                                  /// (005) PTT Allocated Buffer Address

} ME_GLOBAL_NVS_AREA;
#pragma pack()
///
/// ME Global NVS Area Protocol
///
typedef struct _ME_GLOBAL_NVS_AREA_PROTOCOL {
  ME_GLOBAL_NVS_AREA *Area;
} ME_GLOBAL_NVS_AREA_PROTOCOL;

#endif
