/** @file
  Definition of the RapidStart global NVS area protocol.  This protocol
  publishes the address and format of a global ACPI NVS buffer used as a communications
  buffer between SMM/DXE/PEI code and ASL code.
  @todo The format is derived from the ACPI reference code, version 0.95.

  Note:  Data structures defined in this protocol are not naturally aligned.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _RAPID_START_GLOBAL_NVS_AREA_H_
#define _RAPID_START_GLOBAL_NVS_AREA_H_

//
// Includes
//
//
// Forward reference for pure ANSI compatability
//
typedef struct _RAPID_START_GLOBAL_NVS_AREA_PROTOCOL RAPID_START_GLOBAL_NVS_AREA_PROTOCOL;

///
/// RapidStart Global NVS Area Protocol GUID
///
#define RAPID_START_GLOBAL_NVS_AREA_PROTOCOL_GUID \
  { \
    0xa5559f06, 0x6415, 0x4759, 0x88, 0x69, 0xde, 0x15, 0xf9, 0xcd, 0x9c, 0x9b \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gRapidStartGlobalNvsAreaProtocolGuid;

/**
  Global NVS Area definition

  To provide OS base utility controlling capability, these ACPI Global NVS fields
  provides the interface for OS base utility to control Rapid Start behavior and
  Rapid Start reference code will work accordingly.
**/
#pragma pack(1)
typedef struct {
  ///
  /// It shows what wake up event sources available for Rapid Start Entry.
  ///
  /// Used by RapidStart.asl as "RSTA"
  ///
  UINT8   EventsAvailable;
  ///
  /// This controls which wake up source can be used to trigger Rapid Start Entry tasks.
  ///
  /// Used by RapidStart.asl as "RSTS"
  ///
  UINT8   EventsEnabled;
  ///
  /// This defines how many minutes the wake up event will happen for performing Rapid
  /// Start Entry transition. Available options are "Immediately" (wake system up
  /// once system entering S3 mode), 1 Min, 2 Min, 5 Min and 10 Min etc. Default
  /// Platform Policy will set this to 10 minutes.
  ///
  /// Used by RapidStart.asl as "RSTT"
  ///
  UINT16  WakeTimerMin;
  ///
  /// This is the pointer for performance data structure.
  ///
  /// Used by RapidStart.asl as "RSTP"
  ///
  UINT32  PerfDataPtr;
} RAPID_START_GLOBAL_NVS_AREA;
#pragma pack()

/**
  RapidStart Global NVS Area Protocol

  To provide OS base utility controlling mechanism there are some items
  created in Rapid Start private ACPI Global NVS area. Note: This will be
  published by reference code as one SSDT table so no Platform code change
  required.
**/
struct _RAPID_START_GLOBAL_NVS_AREA_PROTOCOL {
  RAPID_START_GLOBAL_NVS_AREA *Area;
  VOID                        *RapidStartData;
};

#endif
