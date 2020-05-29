/** @file
  This file defines the RapidStart Policy Protocol.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _RAPID_START_PLATFORM_POLICY_H_
#define _RAPID_START_PLATFORM_POLICY_H_

///
/// GUID to locate Rapid Start Platform Policy Protocol
///
/// @see _RAPID_START_PLATFORM_POLICY_PROTOCOL
///
#define RAPID_START_PLATFORM_POLICY_PROTOCOL_GUID \
  { \
    0x42bbaca3, 0x7161, 0x4891, 0xac, 0x10, 0xc7, 0x5e, 0x2e, 0x4d, 0xf6, 0x14 \
  }

#define RAPID_START_WAKE_TIMER_MAX      (24 * 60)
#define RAPID_START_WAKE_TIMER_DEFAULT  10

///
/// Extern the GUID for protocol users.
///
/// @see _RAPID_START_PLATFORM_POLICY_PROTOCOL
///
extern EFI_GUID gRapidStartPlatformPolicyProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _RAPID_START_PLATFORM_POLICY_PROTOCOL RAPID_START_PLATFORM_POLICY_PROTOCOL;

///
/// Revision 1: Original version
///
///
/// Revision 2: Added DisplaySaveRestore
///             Added DisplayType
#define DXE_RAPID_START_PLATFORM_POLICY_PROTOCOL_REVISION   1
#define DXE_RAPID_START_PLATFORM_POLICY_PROTOCOL_REVISION_2 2

/**
  RapidStart DXE Platform Platform Policy Protocol

  This protocol provides an interface for platform code to configure Rapid Start
  behavior and usable resource without modifying reference code.

  This protocol provides interface to configure Rapid Start policy. Platform code
  should initialize this protocol before RapidStartDxe driver executing basing on
  the platform implementation. RapidStartDxePolicyInit sample code provided to
  initialize default settings which has been validated by CRB BIOS. Platform code
  can include this sample code with some porting to make it setup menu
  controllable.

  @see RAPID_START_PLATFORM_POLICY_PROTOCOL_GUID
  @see gRapidStartPlatformPolicyProtocolGuid
  @see RapidStartDxePolicyInitEntryPoint()
**/
struct _RAPID_START_PLATFORM_POLICY_PROTOCOL {
  ///
  /// Revision for the protocol structure
  ///
  UINT8   Revision;
  ///
  /// Enable or disable Rapid Start function. 0=Disable (Default) and 1=Enable
  ///
  UINT16  EnableRapidStart : 1;
  ///
  /// Enable reference code to do Rapid Start Entry flow when system waking up from S3
  /// state by RTC timer event. 0=Disable, 1=Enable (Default)
  ///
  /// @see S3WakeTimerMin
  ///
  UINT16  EntryOnS3RtcWake : 1;
  ///
  /// Enable reference code to perform Rapid Start Entry flow when system waking up
  /// from Critical low battery event (from S3 state). 0=Disable, 1=Enable (Default)
  ///
  UINT16  EntryOnS3CritBattWake : 1;
  ///
  /// Enable Active Page Threshold support in Rapid Start reference code. Once enabled
  /// with proper ActivePageThresholdSize value, Rapid Start functions are able to
  /// work on systems with Rapid Start partition size smaller than the amount of the
  /// whole system memory.
  ///
  /// @see ActivePageThresholdSize
  ///
  UINT16  ActivePageThresholdSupport : 1;
  ///
  /// Enable Hybrid Hard Disk support in Rapid Start reference code.
  ///
  /// @see AhciHybridHardDiskSupport
  ///

  UINT16  HybridHardDisk : 1;
  UINT16  DisplaySaveRestore : 1;
  UINT16  DisplayType : 1;
  ///
  /// These bits are reserved for future use
  ///
  UINT16  Reserved : 9;                   ///< Reserved
  ///
  /// Specific the minutes used for RTC timeout wakeup event. This setting only works
  /// when EntryOnS3RtcWake = 1. Default is 10 minutes which will set 10 minutes RTC
  /// Alarm to wake system up after system entering S3 state. Available settings:
  /// "Immediately=0min", 1min, 2mins, 5mins, 10mins, 15mins, 30mins, 60mins and
  /// 120mins etc.
  ///
  /// @see EntryOnS3RtcWake
  /// @see RAPID_START_WAKE_TIMER_MAX
  /// @see RAPID_START_WAKE_TIMER_DEFAULT
  ///
  UINT16  S3WakeTimerMin;
  ///
  /// This is reserved and not used by FRC. It may be used by sample code for RST
  /// Caching mode support.
  ///
  UINT8   RaidModeSataPortNumber;
  ///
  /// When ActivePageThresholdSupport is enabled, ActivePageThresholdSize will be
  /// adopted as the criteria for the Rapid Start partition size. For example, on a
  /// system with 2GB system memory and 1GB Rapid Start partition, when
  /// ActivePageThresholdSupport is disabled, partition provisioning will fail because
  /// system memory size is greater than Rapid Start partition size. When
  /// ActivePageThresholdSupport is enabled and ActivePageThresholdSize set to 1G or
  /// smaller, it passes the provisioning. When ActivePageThresholdSize is set to
  /// zero, size checking will be skipped. Any size of partition reserved for Rapid
  /// Start will pass the provisioning. If Rapid Start functions are still enabled
  /// during S3 entry, Rapid Start reference code calculates the size of Active Memory
  /// Pages, which is the amount of memory that is to be transferred to the Rapid
  /// Start partition after the Zero page filtering. If it is smaller than or equal to
  /// the provisioned Rapid Start partition size, then Rapid Start transitions will be
  /// activated normally after system entering S3. Otherwise, system will remain in S3
  /// state.
  ///
  /// @see ActivePageThresholdSupport
  ///
  UINT32  ActivePageThresholdSize;
  ///
  /// Specifies the type value of the Firmware Version Info(FVI) Structure used to report
  /// version information about Rapid Start through the FVI Interface.
  ///
  UINT8   FviSmbiosType;
};

#endif
