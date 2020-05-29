/** @file

  Definition of AT Host Interface (ATHI) Protocol messages between the
  AT Service and HOST Applications.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _AT_HI_H
#define _AT_HI_H

#include <MeLib.h>

#define AT_COMMAND         0
#define AT_RESPONSE        1

#define AT_ME_RULE_ID      0xd0000
#define AT_ME_RULE_GROUP   0x03
#define AT_ME_RULE_COMMAND 0x02

#pragma pack(1)
///
/// ATHI Protocol Identifier
///
/// AT Host Interface protocol GUID
///
#define ATHI_PROTOCOL_GUID \
  { \
    0x3C4852D6, 0xD47B, 0x4f46, 0xB0, 0x5E, 0xB5, 0xED, 0xC1, 0xAA, 0x43, 0x0A \
  }

#define ATHI_PROTOCOL_FIXED_GUID \
  { \
    0xfa8f55e8, 0xab22, 0x42dd, 0xb9, 0x16, 0x7d, 0xce, 0x39, 0x00, 0x25, 0x74 \
  }

///
/// AT Host Interface protocol version (for ME FW internal use)
///
#define ATHI_PROTOCOL_VERSION_ME      1

///
/// AT Host Interface protocol major version
///
#define ATHI_PROTOCOL_VERSION_MAJOR   5

///
/// AT Host Interface protocol minor version
///
#define ATHI_PROTOCOL_VERSION_MINOR   0

///
/// ATHI Framing Structures
/// Defines the ATHI header and creates a type name.
///
/// Refer to AtHiHeader.dot "ATHI Header"
///
typedef struct _ATHI_HEADER {
  ///
  /// The version number for the ATHI protocol.
  ///
  struct {
    ///
    /// The major version number. The major version number shall advance if
    /// and only if backwards compatibility is broken by a change to the
    /// protocol. If ME firmware doesn't specifically support the version
    /// number supplied in the header, an error shall be returned.
    ///
    UINT8 Major;

    ///
    /// The minor version number. All minor versions under a given major
    /// version shall be backwards compatible with prevision minor versions
    /// under that major version. If the version number supplied in the
    /// header is higher than the minor version number supported by ME
    /// firmware, an error shall be returned.

    UINT8 Minor;
  } Version;

  ///
  /// Specifies the command or response in the message.
  ///
  struct {
    ///
    /// The operation with which the command is associated.
    ///
    UINT16  Code : 7;

    ///
    /// Command/Response indicator:
    /// 0 The message contains a command.
    /// 1 The message contains a response.
    ///
    UINT16  IsResponse : 1;

    ///
    /// The functional category of the command
    ///
    /// See _ATHI_CMD_GROUP
    ///
    UINT16  Category : 8;
  } Command;

  ///
  /// The length in bytes of the message. The length applies to the message
  /// body (excludes the header and signature fields).
  ///
  UINT32  Length;
} ATHI_HEADER;

///
/// Maximum blob data size.
///
#define AT_BLOB_LENGTH_MAX 256

///
/// Structure that defines the format of the Blob to be stored and retrieved
/// for the ISV software.
///
typedef struct _AT_BLOB {
  ///
  /// The length of the data to be securely stored
  ///
  UINT32  Length;

  ///
  /// The data to be securely stored
  ///
  UINT8   Value[AT_BLOB_LENGTH_MAX];
} AT_BLOB;

#define AT_NONCE_LENGTH  16  ///< Maximum nonce length.

///
/// Type definition for a nonce. To prevent replay of commands, a nonce
/// shall be added to each command message. A nonce is a value that is used
/// once. Ideally, each nonce value should not be used more than once during
/// the lifetime of the AT laptop computer. However, a nonce generation function
/// that guarantees a very low probability of nonce values being repeated shall
/// suffice.
///

typedef UINT8 AT_NONCE[AT_NONCE_LENGTH];

///
/// Command Completion Codes. Each response message contains a completion code
/// that indicates whether the command completed successfully, and the nature
/// of the failure if it did not.
///
typedef enum _ATHI_COMPLETION_CODE
{
  ///
  /// The command completed successfully.
  ///
  ATHI_COMPCODE_SUCCESS          = 0,

  ///
  /// The command failed due to an internal error.
  ///
  ATHI_COMPCODE_INTERNAL_FAILURE,

  ///
  /// The command is not recognized, or is recognized but not supported.
  ///
  ATHI_COMPCODE_INVALID_OPERATION,

  ///
  /// A parameter value does not satisfy the requirements of the command.
  ///
  ATHI_COMPCODE_INVALID_PARAM    = 6,

  ///
  /// The value of the length field in the message header is outside the
  /// supported range for the command.
  ///
  ATHI_COMPCODE_INVALID_MSG_LEN,

  ///
  /// The command failed, but no information about the nature of the failure is available.
  ///
  ATHI_COMPCODE_GENERAL_FAILURE,

  ///
  /// Maximum value for a completion code.
  ///
  ATHI_COMPCODE_MAXIMUM
} ATHI_COMPLETION_CODE;

///
/// Useful definitions for basic commands (commands without a body)
///

///
/// Type name for an unsigned command message that has no body (header only).
///
typedef ATHI_HEADER  ATHI_BASIC_CMD;

///
/// Definition and type name for a basic response. The basic response has a
/// header and a minimal body (just the Completion Code).
///
typedef struct _ATHI_BASIC_RSP {
  ATHI_HEADER          Header;         ///< Message header

  ATHI_COMPLETION_CODE CompletionCode; ///< Completion code
} ATHI_BASIC_RSP;

///
/// AT Command Groups
///

/// Related ATHI commands are grouped into several categories. Each command
/// code contains the code of its category. See #ATHI_HEADER.
///
typedef enum _ATHI_CMD_GROUP
{
  ATHI_CMD_GROUP_RESERVED        = 0, ///< Reserved

  ///
  /// Commands related to the ME's theft protection capabilities.
  ///
  /// See _ATHI_THEFT_DETECT_GRP_CMD
  ///
  ATHI_CMD_GROUP_THEFT_DETECTION,

  ///
  /// Commands related to non-volatile data storage services provided to
  /// the FDE/FLE software present on the AT laptop computer.
  ///
  /// See _ATHI_DATA_STORE_GRP_CMD
  ///
  ATHI_CMD_GROUP_DATA_STORAGE,

  ///
  /// Commands related to securely recovering the AT laptop computer for
  /// AT disablement actions.
  ///
  /// See _ATHI_RECOVERY_GRP_CMD
  ///
  ATHI_CMD_GROUP_RECOVERY,

  ///
  /// Commands related to ATHI infrastructure.
  ///
  /// See _ATHI_GENERAL_GRP_CMD
  ///
  ATHI_CMD_GROUP_GENERAL,

  ///
  /// Event notifications serve to inform the ME AT Service when an
  /// event occurs in the PBA or AT server.
  ///
  /// See _ATHI_NOTIFICATION_GRP_CMD
  ///
  ATHI_CMD_GROUP_NOTIFICATIONS,

  ///
  /// 3G NIC commands allow BIOS to initialize and request Radio
  /// status from FW
  ///
  /// See _ATHI_3G_NIC_GRP_CMD
  ///
  ATHI_CMD_GROUP_3G_NIC,

  ///
  /// Boundary check.
  ///
  ATHI_CMD_GROUP_MAX
} ATHI_CMD_GROUP;

///
/// ATHI THEFT DETECTION Group Commands
///

/// Theft detection commands control and configure the theft protection
/// capabilities of the ME AT Service.
///
typedef enum _ATHI_THEFT_DETECT_GRP_CMD
{
  ///
  /// Returns the AT state of the monitored system.
  ///
  /// See ATHI_GET_STATE_CMD
  ///
  ATHI_THEFT_DETECT_GRP_GET_STATE_CMD,

  ///
  /// Returns the state of the specified timer.
  ///
  /// See ATHI_GET_TIMER_INFO_CMD
  ///
  ATHI_THEFT_DETECT_GRP_GET_TIMER_INFO_CMD,

  ///
  /// Resets the specified timer to its configured reset interval.
  ///
  /// See ATHI_RESET_TIMER_CMD
  ///
  ATHI_THEFT_DETECT_GRP_RST_TIMER_CMD,

  ///
  /// Configures the actions that the ME AT Service shall take if the
  /// specified event occurs.
  ///
  /// See ATHI_SET_POLICY_CMD
  ///
  ATHI_THEFT_DETECT_GRP_SET_POLICY_CMD,

  ///
  /// Returns the actions that the ME AT Service shall take if the
  /// specified event occurs.
  ///
  /// See ATHI_GET_POLICY_CMD
  ///
  ATHI_THEFT_DETECT_GRP_GET_POLICY_CMD,

  ///
  /// Sets the reset interval for the specified timer. The timer counts down
  /// and expires if it reaches 0.
  ///
  /// See ATHI_SET_TIMER_INTERVAL_CMD
  ///
  ATHI_THEFT_DETECT_GRP_SET_TIMER_INTERVAL_CMD,

  ///
  /// Asserts that the AT laptop is stolen. The ME AT Service shall proceed
  /// immediately to the Stolen state, executing all actions in the
  /// AssertStolenPolicy.
  ///
  /// See ATHI_ASSERT_STOLEN_CMD
  ///
  ATHI_THEFT_DETECT_GRP_ASSERT_STOLEN_CMD,

  ///
  /// Same as the AssertStolen command but not signed.
  ///
  /// See ATHI_ASSERT_STOLEN_CMD
  ///
  ATHI_THEFT_DETECT_GRP_UNSIGNED_ASSERT_STOLEN_CMD,

  ///
  /// Returns the AT current state in unsigned mode.
  ///
  /// See GET_STATE_UNSIGNED
  ///
  ATHI_THEFT_DETECT_GRP_GET_STATE_UNSIGNED,

  ///
  /// Boundary check.
  ///
  ATHI_THEFT_DETECT_GRP_MAX
} ATHI_THEFT_DETECT_GRP_CMD;

///
/// This flag indicates that AT is in the STOLEN state and the policies are set
/// to "do nothing". The mask must be applied to the AT state rule.
///
/// NOTE: This flag is only cleared when the FW receives an AssertStolen
/// message.
///
#define AT_PSEUDO_STOLEN_STATE_MASK  0x00000080

///
/// Enumeration of AT theft triggers IDs
///
typedef enum _AT_THEFT_TRIGGER
{
  ///
  /// None.
  ///
  AT_THEFT_TRIGGER_NA                              = 0,

  ///
  /// Theft trigger identifier for disable timer expiration
  ///
  AT_THEFT_TRIGGER_DISABLE_TIMER_EXPIRATION,

  ///
  /// Theft trigger identifier for command driven disablement.
  ///
  AT_THEFT_TRIGGER_ASSERT_STOLEN,

  ///
  /// Theft trigger identifier for PBA logon failure exceeded threshold.
  ///
  AT_THEFT_TRIGGER_THRESHOLD_EXCEEDED,

  ///
  /// Theft trigger identifier for platform attack detection.
  ///
  AT_THEFT_TRIGGER_ATTACK_DETECTED,

  ///
  /// Bounday check.
  ///
  AT_THEFT_TRIGGER_MAX
} AT_THEFT_TRIGGER;

///
/// Enumeration of AT timer types.
///
typedef enum _AT_TIMER_ID
{
  ///
  /// Reserved.
  ///
  AT_TID_RSVD                                      = 0,

  ///
  /// The disable timer is a variation on the watchdog timer. It is periodically
  /// reset by ISV software to indicate that the system is behaving as expected.
  /// If it is not reset before it expires, the ME AT Service shall execute the
  /// protective actions configured in its disable timer policy.
  /// The first line of defense when the system begins to exhibit behavior that
  /// indicates it has been stolen is the ISV software, which may invoke the
  /// ATHI SetPolicy function to protect the platform.
  /// The disable timer is designed to protect the system if the ISV software
  /// is disabled. In this case, the ISV software will be unable to reset the
  /// disable timer before it expires, and the ME AT Service will take the
  /// configured protective actions.
  ///
  AT_TID_DISABLE_TIMER,

  ///
  /// The unlock timer used when the AT laptop computer is locked. The ME AT Service
  /// sets the recovery timer when it sees that the AT state is Stolen during
  /// the host boot flow. If the user doesn't successfully complete the
  /// platform_enable_flow recovery flow before the unlock timer expires,
  /// the ME AT Service shall power down the platform immediately.
  /// The variable UnlockTimerInterval shall be configurable within a range
  /// that is long enough to facilitate a bona fide recovery attempt, but short
  /// enough to prevent exploitation of the platform by a thief that does not
  /// have the recovery credential.
  ///
  AT_TID_UNLOCK_TIMER,

  ///
  /// The grace timer allows the user of a AT laptop computer time to take
  /// actions to cause a ResetTimer command to be issued by the AT service.
  /// The grace period shall be applied if and only if the ME AT service
  /// determines that the Disable Timer logically expired while the AT
  /// laptop computer was in Sx (sleeping).
  ///
  AT_TID_GRACE_TIMER,

  ///
  /// The PBA Logon Timer shall be used to count down the allotted time for
  /// PBA Logon. The ME AT Service shall start a PBA Logon Timer upon
  /// transition from S4/S5 to S0. If the ME AT Service receives a property
  /// authenticated PBA Logon notification message  before the PBA Logon Timer
  /// expires, it shall stop the timer and allow operation to continue normally.
  /// If instead the PBA Logon Timer expires before the ME AT Service has
  /// received a properly authenticated PBA Logon notification message, the
  /// ME AT Service shall power down the system immediately, but shall not
  /// transition to the Stolen state.
  ///
  AT_TID_PBA_LOGON_TIMER,

  ///
  /// The activity timer is used to extend the period of the Disable Timer in
  /// the event that a ATHI client has recently invoked a ATHI command when
  /// the Disable Timer expires. The intent is to allow the client to complete
  /// a command flow that may include a ResetTimer command.
  /// The AT activity timer shall be reset each time a ATHI command is completed.
  /// The interval of the AT activity timer shall be 30 seconds. This interval
  /// shall not be externally configurable.
  ///
  AT_TID_ACTIVITY_TIMER,

  ///
  /// Bounday check.
  ///
  AT_TID_MAX
} AT_TIMER_ID;

///
/// A timer with an interval of -1 means that the timer is disabled. Applies to
/// AT_TIMER_CONFIG and AT_TIMER_INFO.
///
#define AT_TIMER_DISABLED  0xFFFFFFFF

///
/// Maximum timer value besides the disabled timer value. This value is due to
/// the maximum timer value allowed by the FW resources.
///
#define AT_TIMER_MAX (AT_TIMER_DISABLED / 1024)

///
/// Remaining grace timer in which the user is not assumed not to be able to
/// complete a TimerResetCmd due to OS context loading from S3 -> S0.
///
/// Value is 30 seconds.
///
#define AT_LOW_GRACE_THRESHOLD 30

///
/// Minumium DISABLE timer value (in seconds)
///
#define AT_TMR_DISABLE_MIN 60

///
/// Minumium DISABLE timer value (in seconds)
///
#define AT_TMR_DISABLE_MAX AT_TIMER_DISABLED

///
// Minumium UNLOCK timer value (in seconds)
///
#define AT_TMR_UNLOCK_MIN  60

///
/// Minumium UNLOCK timer value (in seconds)
///
#define AT_TMR_UNLOCK_MAX  AT_TIMER_MAX

///
/// Minumium GRACE timer value (in seconds)
///
#define AT_TMR_GRACE_MIN 60

///
/// Minumium GRACE timer value (in seconds)
///
#define AT_TMR_GRACE_MAX AT_TIMER_MAX

///
// Minumium PBALOGON timer value (in seconds)
///
#define AT_TMR_PBALOGON_MIN  60

///
/// Minumium PBALOGON timer value (in seconds)
///
#define AT_TMR_PBALOGON_MAX  AT_TIMER_DISABLED

///
/// Structure that contains the configuration and state information for
/// a specified timer.
///
typedef struct _AT_TIMER_INFO {
  ///
  /// Identifies the timer that shall be configured. See #AT_TIMER_ID.
  ///
  UINT8   Id;

  ///
  /// Padding for now
  ///
  UINT8   Reserved[3];

  ///
  /// The interval configured in the timer, in seconds.
  ///
  UINT32  Interval;

  ///
  /// The number of ticks remaining in the current timer countdown.
  ///
  UINT32  TimeLeft;
} AT_TIMER_INFO;

///
/// Structure defining the PBAM Configuration.
///
typedef struct _AT_POLICY_PBA_CONFIG {
  UINT8 RunAfterPost : 1;
  UINT8 IsInstalled : 1;
  UINT8 Reserved : 6;
} AT_POLICY_PBA_CONFIG;

///
/// Optional union defining the PBAM Configuration.
///
typedef union _AT_POLICY_PBA_CONFIG_U {
  UINT8                 Value;
  AT_POLICY_PBA_CONFIG  Bits;
} AT_POLICY_PBA_CONFIG_U;

///
/// Enumeration of the platform lock state
///
typedef enum _AT_PLATFORM_LOCKSTATE
{
  AT_PLATFORM_LOCKSTATE_NOTLOCKED = 0,
  AT_PLATFORM_LOCKSTATE_LOCKED,
  AT_PLATFORM_LOCKSTATE_MAX
} AT_PLATFORM_LOCKSTATE;

//
// Command definition structures
//

///
/// The GetATState command. Returns the AT state of the laptop computer.
///
/// Sender:        AT AM  | PBA
/// Signing Key:   None   | None
///
/// Refer to AtHiGetStateCmd.dot "GetState command"
///
/// See ATHI_GET_STATE_RSP
///
typedef ATHI_BASIC_CMD ATHI_GET_STATE_CMD;

///
// The GetATState response.
///
/// Refer to AtHiGetStateRsp.dot "GetState response"
///
///
typedef struct _ATHI_GET_STATE_RSP {
  ATHI_HEADER          Header;
  ATHI_COMPLETION_CODE CompletionCode;
  AT_STATE_INFO        StateInfo;
} ATHI_GET_STATE_RSP;

///
/// The GetTimerInfo command. Returns configuration and state information about
/// the specified timer.
///
/// Sender:        PBA
/// Signing Key:   PBASK
///
/// Refer to AtHiGetTimerInfoCmd.dot "GetTimerInfo command"
///
/// See ATHI_GET_TIMER_INFO_RSP
///
typedef struct _ATHI_GET_TIMER_INFO_CMD {
  ATHI_HEADER  Header;
  UINT8        TimerId;
} ATHI_GET_TIMER_INFO_CMD;

///
/// ATHI_GET_TIMER_INFO response.
///
/// Refer to AtHiGetTimerInfoRsp.dot "GetTimerInfo response"
///
typedef struct _ATHI_GET_TIMER_INFO_RSP {
  ATHI_HEADER          Header;
  ATHI_COMPLETION_CODE CompletionCode;
  AT_TIMER_INFO        TimerInfo;
} ATHI_GET_TIMER_INFO_RSP;

///
/// ATHI RECOVERY Group Commands
///
typedef enum _ATHI_RECOVERY_GRP_CMD
{
  ///
  /// SetCredential stores the recovery credential in non-volatile storage,
  /// where it is used for validation purposes when the credential is presented
  /// by a user during recovery flow.
  ///
  /// See #ATHI_SET_CREDENTIAL_CMD
  ///
  ATHI_RECOVERY_GRP_SET_CREDENTIAL_CMD,
  ///
  /// AuthenticateRecoveryCredential presents the a credential to the ME AT
  /// Service for validation during the recovery flow.
  ///
  /// See ATHI_AUTHENTICATE_CREDENTIAL_CMD
  ///
  ATHI_RECOVERY_GRP_AUTH_CREDENTIAL_CMD,
  ///
  /// The ComputeHash command computes a specified hash of input data. Maximum
  /// size of the digest value shall be 384 bits (48 bytes). This command may
  /// be used to preprocess authentication tokens if the specified hash
  /// algorithm is not supported by the requester.
  ///
  /// See ATHI_COMPUTE_HASH_CMD
  ///
  ATHI_RECOVERY_GRP_COMPUTE_HASH_CMD,
  ///
  /// The DeAssertStolen command is used to recovery the platfrom from stolen state
  /// by PBAM or AT server
  /// See ATHI_DEASSERT_STOLEN_CMD
  ///
  ATHI_RECOVERY_GRP_DEASSERT_STOLEN_CMD,
  ///
  /// The GetIsvId command exports the identity of the service provider (ISV)
  /// that activated the AT service
  /// See ATHI_GET_ISVID_CMD
  ///
  ATHI_RECOVERY_GRP_GET_ISVID_CMD,
  ///
  /// Bounday check.
  ///
  ATHI_RECOVERY_GRP_MAX
} ATHI_RECOVERY_GRP_CMD;

///
/// This enumeration defines the credential types.
///
typedef enum _AT_CREDENTIAL_TYPE
{
  ///
  // Reserved.
  ///
  AT_CREDENTIAL_TYPE_RSVD                          = 0,

  ///
  // User defined recovery passphrase
  ///
  AT_CREDENTIAL_TYPE_USER_PASSPHRASE,

  ///
  /// Server generated random token
  ///
  AT_CREDENTIAL_TYPE_SRTK,

  ///
  /// Server suspend token
  ///
  AT_CREDENTIAL_TYPE_SSTK,

  ///
  /// Base key
  ///
  AT_CREDENTIAL_TYPE_BK,

  ///
  /// Boundary check.
  ///
  AT_CREDENTIAL_TYPE_MAX
} AT_CREDENTIAL_TYPE;

///
// This enumeration defines the salt IDs.
///
typedef enum _AT_HASH_ALGO_ID
{
  ///
  /// Reserved.
  ///
  AT_HASH_ALGO_ID_RSVD                             = 0,

  ///
  /// SHA-1 algorithm (160-bit output)
  ///
  AT_HASH_ALGO_ID_SHA1,

  ///
  /// Boundary check
  ///
  AT_HASH_ALGO_ID_MAX
} AT_HASH_ALGO_ID;

///
/// Defines the maximum length of a user passpharse recovery hash (will be 32 in 2009).
///
#define AT_USR_PASS_HASH_LENGTH_MAX  20

///
/// Defines the maximum length of a SRTK recovery token
///
#define AT_SRTK_LENGTH_MAX  32

///
/// Maximium credential length.
///
#define AT_CREDENTIAL_VALUE_LENGTH_MAX AT_SRTK_LENGTH_MAX

#define AT_PASSWORD_LENGTH             64

///
/// Structure that defines a credential for storage, validation, and export operations.
///
typedef struct _AT_CREDENTIAL {
  ///
  /// The credential type
  ///
  AT_CREDENTIAL_TYPE Type;

  ///
  /// The credential length
  ///
  UINT32              Length;

  ///
  /// The credential value
  ///
  UINT8               Value[1]; /// Need a pointer but cannot make a zero length array
} AT_CREDENTIAL;

///
/// The AuthenticateCredential command. Sets the value of the specified
/// credential in the ME AT Service.
///
/// Sender:        AT AM
/// Signing Key:   None
///
/// Refer to AtHiAuthenticateCredentialCmd.dot "AuthenticateCredential command"
///
/// See ATHI_AUTHENTICATE_CREDENTIAL_RSP
///
typedef struct _ATHI_AUTHENTICATE_CREDENTIAL_CMD {
  ATHI_HEADER    Header;
  AT_CREDENTIAL  Credential;
} ATHI_AUTHENTICATE_CREDENTIAL_CMD;

///
/// The AuthenticateCredential response.
///
/// Refer to AtHiAuthenticateCredentialRsp.dot "AuthenticateCredential response"
///
typedef struct _ATHI_AUTHENTICATE_CREDENTIAL_RSP {
  ATHI_HEADER          Header;
  ATHI_COMPLETION_CODE CompletionCode;
  UINT8                Authenticated;
} ATHI_AUTHENTICATE_CREDENTIAL_RSP;

///
/// Definition for the maximum hash output size.
///
#define AT_MAX_HASH_OUTPUT_SIZE  48

///
/// The ComputeHash command. Computes a specified hash of input data. Maximum
/// size of the digest value shall be 384 bits (48 bytes).
///
/// Sender:        AT AM
/// Signing Key:   None
///
/// Refer to AtHiComputeHashCmd.dot "ComputeHash command"
///
/// See ATHI_COMPUTE_HASH_RSP
///
typedef struct _ATHI_COMPUTE_HASH_CMD {
  ATHI_HEADER  Header;

  ///
  /// see AT_HASH_ALGO_ID for values
  ///
  UINT8        Algorithm;
  UINT8        InputLength;
  UINT8        InputBuffer[AT_MAX_HASH_OUTPUT_SIZE];
} ATHI_COMPUTE_HASH_CMD;

///
/// The ComputeHash response.
///
/// Refer to AtHiComputeHashRsp.dot "ComputeHash response"
///
///
typedef struct _ATHI_COMPUTE_HASH_RSP {
  ATHI_HEADER          Header;
  ATHI_COMPLETION_CODE CompletionCode;

  ///
  /// see AT_HASH_ALGO_ID for values
  ///
  UINT8                OutputLength;
  UINT8                OutputBuffer[AT_MAX_HASH_OUTPUT_SIZE];
} ATHI_COMPUTE_HASH_RSP;

///
/// The DeassertStolen command. Recover the laptop from stolen state. The ME AT
/// Service shall proceed immediately to the NotStolen state.
///
/// Sender:        ISV SW
/// Signing Key:   PBASK, TSSK
///
/// Refer to AtHiDeassertStolenCmd.dot "DeassertStolen command"
///
/// See ATHI_DEASSERT_STOLEN_RSP
///
typedef ATHI_BASIC_CMD ATHI_DEASSERT_STOLEN_CMD;

typedef ATHI_BASIC_RSP ATHI_DEASSERT_STOLEN_RSP;

///
/// The GetIsvId command exports the identity of the service provider (ISV) that
/// activated the AT service. This allows for system recovery in cases where the user
/// may not be sure who to contact when he/she is unable to recover the system locally.
/// Note that this command is allowed only while the system in the STOLEN state
///
/// Sender:        AT AM
/// Signing Key:   None
///
/// See ATHI_GET_ISVID_RSP
///
typedef ATHI_BASIC_CMD ATHI_GET_ISVID_CMD;

typedef struct _ATHI_GET_ISVID_RSP {
  ATHI_HEADER           Header;
  ATHI_COMPLETION_CODE  CompletionCode;
  UINT32                IsvId;
} ATHI_GET_ISVID_RSP;

///
/// ATHI General Group Commands
///
typedef enum _ATHI_GENERAL_GRP_CMD
{
  ///
  /// GetNonce returns a nonce generated by the ME AT Service. The purpose of
  /// this nonce is to prevent replay of AT command messages.
  ///
  /// See ATHI_GET_NONCE_CMD
  ///
  ATHI_GENERAL_GRP_GET_NONCE_CMD,

  ///
  /// GetATCapabilities requests that the ME AT Service return the
  /// capabilities it has implemented.
  ///
  /// See ATHI_GET_AT_CAPABILITIES_CMD
  ///
  ATHI_GENERAL_GRP_GET_AT_CAPABILITIES_CMD,

  ///
  /// The SetPublicKey command saves the specified public key of an RSA
  /// key-pair for use by the ME AT Service.
  ///
  /// See ATHI_GET_PUBLIC_KEY_CMD
  ///
  ATHI_GENERAL_GRP_SET_PUBLIC_KEY_CMD,

  ///
  /// The GetPublicKey command returns the specified public key of an RSA
  /// key-pair owned by the ME AT Service.
  ///
  /// See ATHI_GET_PUBLIC_KEY_CMD
  ///
  ATHI_GENERAL_GRP_GET_PUBLIC_KEY_CMD,

  ///
  /// The GetEventHistory command retrieves entries from the AT Event History
  /// in the ME.
  ///
  /// See ATHI_GET_EVENT_HISTORY_CMD
  ///
  ATHI_GENERAL_GRP_GET_EVENT_HISTORY_CMD,

  ///
  /// The ClearEventHistory command deletes all entries from the AT Event
  /// History in the ME.
  ///
  /// See ATHI_CLEAR_EVENT_HISTORY_CMD
  ///
  ATHI_GENERAL_GRP_CLEAR_EVENT_HISTORY_CMD,

  ///
  /// The SetSysTime command synchronizes the time between the FW with the
  /// AT server.
  ///
  /// See ATHI_SET_SYS_TIME_CMD
  ///
  ATHI_GENERAL_GRP_SET_SYS_TIME_CMD,

  ///
  /// The Set Supend Mode command requests FW transition
  /// into or out of Suspend Mode.
  ///

  ATHI_GENERAL_GRP_SET_SUSPEND_CMD                 = 8,

  ///
  /// Bounday check.
  ///
  ATHI_GENERAL_GRP_MAX
} ATHI_GENERAL_GRP_CMD;

///
/// ATHI 3G WWAN Commands
///
typedef enum _ATHI_3G_NIC_GRP_CMD
{ ///
  /// 3G NIC Init Command tells FW to intialize the NIC radio
  ///
  ATHI_3G_NIC_GRP_INIT_CMD                         = 3,

  ///
  /// The 3G NIC Query Command asks FW about the status of the NIC radio
  ///
  ATHI_3G_NIC_GRP_QUERY_CMD,

  ///
  /// Bounday check.
  ///
  ATHI_3G_NIC_GRP_MAX
} ATHI_3G_NIC_GRP_CMD;

///
/// The GetNonce command. Requests that the ME AT Service generate and return
/// a nonce to be used in a subsequent ATHI command. The nonce prevents
/// replay of a command.
///
/// Sender:        ISV SW
/// Signing Key:   Any
///
/// Refer to AtHiGetNonceCmd.dot "GetNonce command"
///
/// See ATHI_GET_NONCE_RSP
///
typedef ATHI_BASIC_CMD ATHI_GET_NONCE_CMD;

///
/// The GetNonce response.
///
/// Refer to AtHiGetNonceRsp.dot "GetNonce response"
///
typedef struct _ATHI_GET_NONCE_RSP {
  ATHI_HEADER          Header;
  ATHI_COMPLETION_CODE CompletionCode;
  AT_NONCE             Nonce;
} ATHI_GET_NONCE_RSP;

///
/// ATHI NOTIFICATION Group Commands
///
typedef enum _ATHI_NOTIFICATION_GRP_CMD
{
  ///
  /// A PBA sends a PBALogon notification message to notify the ME AT service
  /// of a successful user logon.
  ///
  /// See ATHI_PBA_LOGON
  ///
  ATHI_NOTIFICATION_GRP_PBA_LOGON,

  ///
  /// Bounday check.
  ///
  ATHI_NOTIFICATION_GRP_MAX
} ATHI_NOTIFICATION_GRP_CMD;

///
/// PBALogon notification. Notifies the ME AT service of a successful user logon.
///
/// Sender:        PBA
/// Signing Key:   PBASK
///
/// Refer to AtHiPbaLogonNotify.dot "PBALogon notification"
///
/// See ATHI_GET_NONCE_RSP
///
typedef ATHI_BASIC_CMD ATHI_PBA_LOGON_NOTIFY;

///
/// The GetNonce response.
///
/// Refer to AtHiPbaLogonAck.dot "PBALogon ack"
///
typedef ATHI_BASIC_RSP ATHI_PBA_LOGON_ACK;

///
/// ATHI DATA STORAGE Group Commands
///
typedef enum _ATHI_DATA_STORE_GRP_CMD
{
  ///
  /// SetBlob stores data in the ME AT FDE/FLE ISV non-volatile storage area.
  ///
  /// See #ATHI_SET_BLOB_CMD.
  ///
  ATHI_DATA_STORE_GRP_SET_BLOB_CMD,

  ///
  /// GetBlob returns the data stored in the ME AT FDE/FLE ISV non-volatile
  /// storage area.
  ///
  /// See ATHI_GET_BLOB_CMD.
  ///
  ATHI_DATA_STORE_GRP_GET_BLOB_CMD,

  ///
  /// Stores a vendor string the ME's non-volatile storage area.
  ///
  /// See ATHI_SET_VENDOR_STRING_CMD
  ///
  ATHI_DATA_STORE_GRP_SET_VENDOR_STRING_CMD,

  ///
  /// Retrieves the vendor string from the ME's non-volatile storage area.
  ///
  /// See ATHI_GET_VENDOR_STRING_CMD
  ///
  ATHI_DATA_STORE_GRP_GET_VENDOR_STRING_CMD,

  ///
  /// Bounday check.
  ///
  ATHI_DATA_STORE_GRP_MAX
} ATHI_DATA_STORE_GRP_CMD;

///
/// This enumeration defines the vendor string identifiers.
///
typedef enum _AT_VENDOR_STRING_ID
{
  ///
  /// Reserved.
  ///
  AT_VENDOR_STRING_ID_RSVD                         = 0,

  ///
  /// User defined recovery passphrase
  ///
  AT_VENDOR_STRING_ID_RECOVERY_HELP,
  ///
  /// AT_VENDOR_STRING_ID_RECOVERY_HELP,
  ///

  ///
  /// Server generated random token
  ///
  AT_CUSTOM_RECOVERY_ID_CONFIGURATIONS,

  ///
  /// Boundary check.
  ///
  AT_VENDOR_STRING_ID_MAX
} AT_VENDOR_STRING_ID;

///
/// Maximum blob data size.
///
#define AT_VENDOR_STRING_LENGTH_MAX  256

///
/// The SetVendorString command. Stores a vendor string the ME's
/// non-volatile storage area.
///
/// Sender:        ISV SW
/// Signing Key:   TSSK
///
/// See ATHI_SET_VENDOR_STRING_RSP
///
typedef struct _ATHI_SET_VENDOR_STRING_CMD {
  ATHI_HEADER  Header;
  UINT8        Id;
  UINT8        Reserved[3];
  AT_BLOB      String;
} ATHI_SET_VENDOR_STRING_CMD;

///
/// The SetVendorString response.
///
typedef ATHI_BASIC_RSP ATHI_SET_VENDOR_STRING_RSP;

///
/// The GetVendorString command. Retrieves the vendor string data from the ME's
/// non-volatile storage area.
///
/// Sender:        ISV SW
/// Signing Key:   None
///
/// See ATHI_GET_VENDOR_STRING_RSP
///
typedef struct _ATHI_GET_VENDOR_STRING_CMD {
  ATHI_HEADER  Header;
  UINT8        Id;
} ATHI_GET_VENDOR_STRING_CMD;

///
/// The GetVendorString response.
///
/// Sender:        ISV SW
/// Signing Key:   None
///
///
typedef struct _ATHI_GET_VENDOR_STRING_RSP {
  ATHI_HEADER          Header;
  ATHI_COMPLETION_CODE CompletionCode;
  AT_BLOB              String;
} ATHI_GET_VENDOR_STRING_RSP;

///
/// The SendAssertStolen Request
///
/// Sender:        ISV SW
/// Signing Key:   None
///
///
typedef struct _ATHI_ASSERT_STOLEN_CMD {
  ATHI_HEADER  Header;
  UINT32       DelayTime;
} ATHI_ASSERT_STOLEN_CMD;

///
/// The SendAssertStolen response.
///
typedef ATHI_BASIC_RSP ATHI_ASSERT_STOLEN_RSP;

///
/// The Set Supend State Request Message
///
/// Sender:        BIOS
/// Signing Key:   SSTK One Time Token
///
///
typedef struct _ATHI_SET_SUSPEND_STATE_CMD {
  ATHI_HEADER    Header;
  UINT32         TransitionState;  /// 0=Exit, 1=Enter
  AT_CREDENTIAL  Credential;
} ATHI_SET_SUSPEND_STATE_CMD;

///
/// The Set Supend State Response Message
///
/// Sender:        FW
/// Signing Key:   None
///
///
typedef struct _ATHI_SET_SUSPEND_STATE_RSP {
  ATHI_HEADER  Header;
  UINT32       CompletionCode;
} ATHI_SET_SUSPEND_STATE_RSP;

///
/// The Initialize WWAN Recovery Request Message
///
/// Sender:        BIOS
/// Signing Key:   None
///
///
typedef struct _ATHI_INIT_WWAN_RECOV_CMD {
  ATHI_HEADER  Header;
} ATHI_INIT_WWAN_RECOV_CMD;

///
/// The Initialize WWAN Recovery Response Message
///
/// Sender:        FW
/// Signing Key:   None
///
///
typedef struct _ATHI_INIT_WWAN_RECOV_RSP {
  ATHI_HEADER  Header;
  UINT32       CompletionCode;
} ATHI_INIT_WWAN_RECOV_RSP;

///
/// The Initialize WWAN Status Request Message
///
/// Sender:        BIOS
/// Signing Key:   None
///
///
typedef struct _ATHI_WWAN_STATUS_CMD {
  ATHI_HEADER  Header;
} ATHI_WWAN_STATUS_CMD;

///
/// The Initialize WWAN Status Response Message
///
/// Sender:        FW
/// Signing Key:   None
///
///
typedef struct _ATHI_WWAN_STATUS_RSP {
  ATHI_HEADER  Header;
  UINT32       CompletionCode;
  UINT8        RadioStatus;
  UINT8        NetworkStatus;
} ATHI_WWAN_STATUS_RSP;

#pragma pack()

#endif // _AT_HI_H
