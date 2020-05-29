/** @file

  Definitions for Tpm 2.0

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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

#ifndef    _TPM20_H
#define    _TPM20_H

///
/// The start of TPM return codes
///
#define TPM_BASE                  0
#include <IndustryStandard/Tpm12.h>

#pragma pack (push, 1)

typedef UINT8                    BYTE;
typedef UINT8                    BOOL;

typedef struct {
    UINT16        size;
    BYTE          buffer[1];
} TPM2B;

#include <IndustryStandard/Tpm20Implementation.h>

#define    MAX_CAP_DATA                   (MAX_CAP_BUFFER-sizeof(TPM_CAP)-sizeof(UINT32))
#define    MAX_CAP_ALGS                   (MAX_CAP_DATA/sizeof(TPMS_ALG_PROPERTY))
#define    MAX_CAP_HANDLES                (MAX_CAP_DATA/sizeof(TPM_HANDLE))
#define    MAX_CAP_CC                     (MAX_CAP_DATA/sizeof(TPM_CC))
#define    MAX_TPM_PROPERTIES             (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PROPERTY))
#define    MAX_PCR_PROPERTIES             (MAX_CAP_DATA/sizeof(TPMS_TAGGED_PCR_SELECT))
#define    MAX_ECC_CURVES                 (MAX_CAP_DATA/sizeof(TPM_ECC_CURVE))

// Table 2 -- BaseTypes BaseTypes <I/O>

// Table 3 -- DocumentationClarity Types <I/O>
typedef UINT32    TPM_ALGORITHM_ID;
typedef UINT32    TPM_MODIFIER_INDICATOR;
typedef UINT32    TPM_SESSION_OFFSET;
typedef UINT16    TPM_KEY_SIZE;
typedef UINT16    TPM_KEY_BITS;
typedef UINT64    TPM_SYSTEM_ADDRESS;
typedef UINT32 TPM_SPEC;

#define    TPM_SPEC_FAMILY         (TPM_SPEC)(0x322E3000)
#define    TPM_SPEC_LEVEL          (TPM_SPEC)(00)
#define    TPM_SPEC_VERSION        (TPM_SPEC)(88)
#define    TPM_SPEC_YEAR           (TPM_SPEC)(2012)
#define    TPM_SPEC_DAY_OF_YEAR    (TPM_SPEC)(65)

// Table 5 -- TPM_GENERATED Constants <O,S>
typedef UINT32 TPM_GENERATED;

#define    TPM_GENERATED_VALUE    (TPM_GENERATED)(0xff544347)

// Table 10 -- TPM_CC Constants <I/O,S>
typedef UINT32 TPM_CC;

#define    TPM_CC_FIRST                         (TPM_CC)(0x0000011F)
#define    TPM_CC_PP_FIRST                      (TPM_CC)(0x0000011F)
#define    TPM_CC_NV_UndefineSpaceSpecial       (TPM_CC)(0x0000011F)
#define    CC_NV_UndefineSpaceSpecial       YES
#define    TPM_CC_EvictControl                  (TPM_CC)(0x00000120)
#define    CC_EvictControl                  YES
#define    TPM_CC_HierarchyControl              (TPM_CC)(0x00000121)
#define    CC_HierarchyControl              YES
#define    TPM_CC_NV_UndefineSpace              (TPM_CC)(0x00000122)
#define    CC_NV_UndefineSpace              YES
#define    TPM_CC_ChangeEPS                     (TPM_CC)(0x00000124)
#define    CC_ChangeEPS                     YES
#define    TPM_CC_ChangePPS                     (TPM_CC)(0x00000125)
#define    CC_ChangePPS                     YES
#define    TPM_CC_Clear                         (TPM_CC)(0x00000126)
#define    CC_Clear                         YES
#define    TPM_CC_ClearControl                  (TPM_CC)(0x00000127)
#define    CC_ClearControl                  YES
#define    TPM_CC_ClockSet                      (TPM_CC)(0x00000128)
#define    CC_ClockSet                      YES
#define    TPM_CC_HierarchyChangeAuth           (TPM_CC)(0x00000129)
#define    CC_HierarchyChangeAuth           YES
#define    TPM_CC_NV_DefineSpace                (TPM_CC)(0x0000012A)
#define    CC_NV_DefineSpace                YES
#define    TPM_CC_PCR_Allocate                  (TPM_CC)(0x0000012B)
#define    CC_PCR_Allocate                  YES
#define    TPM_CC_PCR_SetAuthPolicy             (TPM_CC)(0x0000012C)
#define    CC_PCR_SetAuthPolicy             YES
#define    TPM_CC_PP_Commands                   (TPM_CC)(0x0000012D)
#define    CC_PP_Commands                   YES
#define    TPM_CC_SetPrimaryPolicy              (TPM_CC)(0x0000012E)
#define    CC_SetPrimaryPolicy              YES
#define    TPM_CC_FieldUpgradeStart             (TPM_CC)(0x0000012F)
#define    CC_FieldUpgradeStart             NO
#define    TPM_CC_ClockRateAdjust               (TPM_CC)(0x00000130)
#define    CC_ClockRateAdjust               YES
#define    TPM_CC_CreatePrimary                 (TPM_CC)(0x00000131)
#define    CC_CreatePrimary                 YES
#define    TPM_CC_NV_GlobalWriteLock            (TPM_CC)(0x00000132)
#define    CC_NV_GlobalWriteLock            YES
#define    TPM_CC_PP_LAST                       (TPM_CC)(0x00000132)
#define    TPM_CC_GetCommandAuditDigest         (TPM_CC)(0x00000133)
#define    CC_GetCommandAuditDigest         YES
#define    TPM_CC_NV_Increment                  (TPM_CC)(0x00000134)
#define    CC_NV_Increment                  YES
#define    TPM_CC_NV_SetBits                    (TPM_CC)(0x00000135)
#define    CC_NV_SetBits                    YES
#define    TPM_CC_NV_Extend                     (TPM_CC)(0x00000136)
#define    CC_NV_Extend                     YES
#define    TPM_CC_NV_Write                      (TPM_CC)(0x00000137)
#define    CC_NV_Write                      YES
#define    TPM_CC_NV_WriteLock                  (TPM_CC)(0x00000138)
#define    CC_NV_WriteLock                  YES
#define    TPM_CC_DictionaryAttackLockReset     (TPM_CC)(0x00000139)
#define    CC_DictionaryAttackLockReset     YES
#define    TPM_CC_DictionaryAttackParameters    (TPM_CC)(0x0000013A)
#define    CC_DictionaryAttackParameters    YES
#define    TPM_CC_NV_ChangeAuth                 (TPM_CC)(0x0000013B)
#define    CC_NV_ChangeAuth                 YES
#define    TPM_CC_PCR_Event                     (TPM_CC)(0x0000013C)
#define    CC_PCR_Event                     YES
#define    TPM_CC_PCR_Reset                     (TPM_CC)(0x0000013D)
#define    CC_PCR_Reset                     YES
#define    TPM_CC_SequenceComplete              (TPM_CC)(0x0000013E)
#define    CC_SequenceComplete              YES
#define    TPM_CC_SetAlgorithmSet               (TPM_CC)(0x0000013F)
#define    CC_SetAlgorithmSet               YES
#define    TPM_CC_SetCommandCodeAuditStatus     (TPM_CC)(0x00000140)
#define    CC_SetCommandCodeAuditStatus     YES
#define    TPM_CC_FieldUpgradeData              (TPM_CC)(0x00000141)
#define    CC_FieldUpgradeData              NO
#define    TPM_CC_IncrementalSelfTest           (TPM_CC)(0x00000142)
#define    CC_IncrementalSelfTest           YES
#define    TPM_CC_SelfTest                      (TPM_CC)(0x00000143)
#define    CC_SelfTest                      YES
#define    TPM_CC_Startup                       (TPM_CC)(0x00000144)
#define    CC_Startup                       YES
#define    TPM_CC_Shutdown                      (TPM_CC)(0x00000145)
#define    CC_Shutdown                      YES
#define    TPM_CC_StirRandom                    (TPM_CC)(0x00000146)
#define    CC_StirRandom                    YES
#define    TPM_CC_ActivateCredential            (TPM_CC)(0x00000147)
#define    CC_ActivateCredential            YES
#define    TPM_CC_Certify                       (TPM_CC)(0x00000148)
#define    CC_Certify                       YES
#define    TPM_CC_PolicyNV                      (TPM_CC)(0x00000149)
#define    CC_PolicyNV                      YES
#define    TPM_CC_CertifyCreation               (TPM_CC)(0x0000014A)
#define    CC_CertifyCreation               YES
#define    TPM_CC_Duplicate                     (TPM_CC)(0x0000014B)
#define    CC_Duplicate                     YES
#define    TPM_CC_GetTime                       (TPM_CC)(0x0000014C)
#define    CC_GetTime                       YES
#define    TPM_CC_GetSessionAuditDigest         (TPM_CC)(0x0000014D)
#define    CC_GetSessionAuditDigest         YES
#define    TPM_CC_NV_Read                       (TPM_CC)(0x0000014E)
#define    CC_NV_Read                       YES
#define    TPM_CC_NV_ReadLock                   (TPM_CC)(0x0000014F)
#define    CC_NV_ReadLock                   YES
#define    TPM_CC_ObjectChangeAuth              (TPM_CC)(0x00000150)
#define    CC_ObjectChangeAuth              YES
#define    TPM_CC_PolicySecret                  (TPM_CC)(0x00000151)
#define    CC_PolicySecret                  YES
#define    TPM_CC_Rewrap                        (TPM_CC)(0x00000152)
#define    CC_Rewrap                        YES
#define    TPM_CC_Create                        (TPM_CC)(0x00000153)
#define    CC_Create                        YES
#define    TPM_CC_ECDH_ZGen                     (TPM_CC)(0x00000154)
#define    CC_ECDH_ZGen                     YES
#define    TPM_CC_HMAC                          (TPM_CC)(0x00000155)
#define    CC_HMAC                          YES
#define    TPM_CC_Import                        (TPM_CC)(0x00000156)
#define    CC_Import                        YES
#define    TPM_CC_Load                          (TPM_CC)(0x00000157)
#define    CC_Load                          YES
#define    TPM_CC_Quote                         (TPM_CC)(0x00000158)
#define    CC_Quote                         YES
#define    TPM_CC_RSA_Decrypt                   (TPM_CC)(0x00000159)
#define    CC_RSA_Decrypt                   YES
#define    TPM_CC_HMAC_Start                    (TPM_CC)(0x0000015B)
#define    CC_HMAC_Start                    YES
#define    TPM_CC_SequenceUpdate                (TPM_CC)(0x0000015C)
#define    CC_SequenceUpdate                YES
#define    TPM_CC_Sign                          (TPM_CC)(0x0000015D)
#define    CC_Sign                          YES
#define    TPM_CC_Unseal                        (TPM_CC)(0x0000015E)
#define    CC_Unseal                        YES
#define    TPM_CC_PolicySigned                  (TPM_CC)(0x00000160)
#define    CC_PolicySigned                  YES
#define    TPM_CC_ContextLoad                   (TPM_CC)(0x00000161)
#define    CC_ContextLoad                   YES
#define    TPM_CC_ContextSave                   (TPM_CC)(0x00000162)
#define    CC_ContextSave                   YES
#define    TPM_CC_ECDH_KeyGen                   (TPM_CC)(0x00000163)
#define    CC_ECDH_KeyGen                   YES
#define    TPM_CC_EncryptDecrypt                (TPM_CC)(0x00000164)
#define    CC_EncryptDecrypt                YES
#define    TPM_CC_FlushContext                  (TPM_CC)(0x00000165)
#define    CC_FlushContext                  YES
#define    TPM_CC_LoadExternal                  (TPM_CC)(0x00000167)
#define    CC_LoadExternal                  YES
#define    TPM_CC_MakeCredential                (TPM_CC)(0x00000168)
#define    CC_MakeCredential                YES
#define    TPM_CC_NV_ReadPublic                 (TPM_CC)(0x00000169)
#define    CC_NV_ReadPublic                 YES
#define    TPM_CC_PolicyAuthorize               (TPM_CC)(0x0000016A)
#define    CC_PolicyAuthorize               YES
#define    TPM_CC_PolicyAuthValue               (TPM_CC)(0x0000016B)
#define    CC_PolicyAuthValue               YES
#define    TPM_CC_PolicyCommandCode             (TPM_CC)(0x0000016C)
#define    CC_PolicyCommandCode             YES
#define    TPM_CC_PolicyCounterTimer            (TPM_CC)(0x0000016D)
#define    CC_PolicyCounterTimer            YES
#define    TPM_CC_PolicyCpHash                  (TPM_CC)(0x0000016E)
#define    CC_PolicyCpHash                  YES
#define    TPM_CC_PolicyLocality                (TPM_CC)(0x0000016F)
#define    CC_PolicyLocality                YES
#define    TPM_CC_PolicyNameHash                (TPM_CC)(0x00000170)
#define    CC_PolicyNameHash                YES
#define    TPM_CC_PolicyOR                      (TPM_CC)(0x00000171)
#define    CC_PolicyOR                      YES
#define    TPM_CC_PolicyTicket                  (TPM_CC)(0x00000172)
#define    CC_PolicyTicket                  YES
#define    TPM_CC_ReadPublic                    (TPM_CC)(0x00000173)
#define    CC_ReadPublic                    YES
#define    TPM_CC_RSA_Encrypt                   (TPM_CC)(0x00000174)
#define    CC_RSA_Encrypt                   YES
#define    TPM_CC_StartAuthSession              (TPM_CC)(0x00000176)
#define    CC_StartAuthSession              YES
#define    TPM_CC_VerifySignature               (TPM_CC)(0x00000177)
#define    CC_VerifySignature               YES
#define    TPM_CC_ECC_Parameters                (TPM_CC)(0x00000178)
#define    CC_ECC_Parameters                YES
#define    TPM_CC_FirmwareRead                  (TPM_CC)(0x00000179)
#define    CC_FirmwareRead                  NO
#define    TPM_CC_GetCapability                 (TPM_CC)(0x0000017A)
#define    CC_GetCapability                 YES
#define    TPM_CC_GetRandom                     (TPM_CC)(0x0000017B)
#define    CC_GetRandom                     YES
#define    TPM_CC_GetTestResult                 (TPM_CC)(0x0000017C)
#define    CC_GetTestResult                 YES
#define    TPM_CC_Hash                          (TPM_CC)(0x0000017D)
#define    CC_Hash                          YES
#define    TPM_CC_PCR_Read                      (TPM_CC)(0x0000017E)
#define    CC_PCR_Read                      YES
#define    TPM_CC_PolicyPCR                     (TPM_CC)(0x0000017F)
#define    CC_PolicyPCR                     YES
#define    TPM_CC_PolicyRestart                 (TPM_CC)(0x00000180)
#define    CC_PolicyRestart                 YES
#define    TPM_CC_ReadClock                     (TPM_CC)(0x00000181)
#define    CC_ReadClock                     YES
#define    TPM_CC_PCR_Extend                    (TPM_CC)(0x00000182)
#define    CC_PCR_Extend                    YES
#define    TPM_CC_PCR_SetAuthValue              (TPM_CC)(0x00000183)
#define    CC_PCR_SetAuthValue              YES
#define    TPM_CC_NV_Certify                    (TPM_CC)(0x00000184)
#define    CC_NV_Certify                    YES
#define    TPM_CC_EventSequenceComplete         (TPM_CC)(0x00000185)
#define    CC_EventSequenceComplete         YES
#define    TPM_CC_HashSequenceStart             (TPM_CC)(0x00000186)
#define    CC_HashSequenceStart             YES
#define    TPM_CC_PolicyPhysicalPresence        (TPM_CC)(0x00000187)
#define    CC_PolicyPhysicalPresence        YES
#define    TPM_CC_PolicyDuplicationSelect       (TPM_CC)(0x00000188)
#define    CC_PolicyDuplicationSelect       YES
#define    TPM_CC_PolicyGetDigest               (TPM_CC)(0x00000189)
#define    CC_PolicyGetDigest               YES
#define    TPM_CC_TestParms                     (TPM_CC)(0x0000018A)
#define    CC_TestParms                     YES
#define    TPM_CC_Commit                        (TPM_CC)(0x0000018B)
#define    CC_Commit                        YES
#define    TPM_CC_PolicyPassword                (TPM_CC)(0x0000018C)
#define    CC_PolicyPassword                YES
#define    TPM_CC_LAST                          (TPM_CC)(0x0000018C)

// Table 14 -- TPM_RC Constants <O,S>
typedef UINT32 TPM_RC;

#define    TPM_RC_SUCCESS              (TPM_RC)(0x000)
#define    TPM_RC_BAD_TAG              (TPM_RC)(0x030)
#define    RC_VER1                     (TPM_RC)(0x100)
#define    TPM_RC_INITIALIZE           (TPM_RC)(RC_VER1 + 0x000)
#define    TPM_RC_FAILURE              (TPM_RC)(RC_VER1 + 0x001)
#define    TPM_RC_SEQUENCE             (TPM_RC)(RC_VER1 + 0x003)
#define    TPM_RC_PRIVATE              (TPM_RC)(RC_VER1 + 0x00B)
#define    TPM_RC_HMAC                 (TPM_RC)(RC_VER1 + 0x019)
#define    TPM_RC_DISABLED             (TPM_RC)(RC_VER1 + 0x020)
#define    TPM_RC_EXCLUSIVE            (TPM_RC)(RC_VER1 + 0x021)
#define    TPM_RC_ECC_CURVE            (TPM_RC)(RC_VER1 + 0x023)
#define    TPM_RC_AUTH_TYPE            (TPM_RC)(RC_VER1 + 0x024)
#define    TPM_RC_AUTH_MISSING         (TPM_RC)(RC_VER1 + 0x025)
#define    TPM_RC_POLICY               (TPM_RC)(RC_VER1 + 0x026)
#define    TPM_RC_PCR                  (TPM_RC)(RC_VER1 + 0x027)
#define    TPM_RC_PCR_CHANGED          (TPM_RC)(RC_VER1 + 0x028)
#define    TPM_RC_ECC_POINT            (TPM_RC)(RC_VER1 + 0x02C)
#define    TPM_RC_UPGRADE              (TPM_RC)(RC_VER1 + 0x02D)
#define    TPM_RC_TOO_MANY_CONTEXTS    (TPM_RC)(RC_VER1 + 0x02E)
#define    TPM_RC_AUTH_UNAVAILABLE     (TPM_RC)(RC_VER1 + 0x02F)
#define    TPM_RC_REBOOT               (TPM_RC)(RC_VER1 + 0x030)
#define    TPM_RC_UNBALANCED           (TPM_RC)(RC_VER1 + 0x031)
#define    TPM_RC_COMMAND_SIZE         (TPM_RC)(RC_VER1 + 0x042)
#define    TPM_RC_COMMAND_CODE         (TPM_RC)(RC_VER1 + 0x043)
#define    TPM_RC_AUTHSIZE             (TPM_RC)(RC_VER1 + 0x044)
#define    TPM_RC_AUTH_CONTEXT         (TPM_RC)(RC_VER1 + 0x045)
#define    TPM_RC_NV_RANGE             (TPM_RC)(RC_VER1 + 0x046)
#define    TPM_RC_NV_SIZE              (TPM_RC)(RC_VER1 + 0x047)
#define    TPM_RC_NV_LOCKED            (TPM_RC)(RC_VER1 + 0x048)
#define    TPM_RC_NV_AUTHORIZATION     (TPM_RC)(RC_VER1 + 0x049)
#define    TPM_RC_NV_UNINITIALIZED     (TPM_RC)(RC_VER1 + 0x04A)
#define    TPM_RC_NV_SPACE             (TPM_RC)(RC_VER1 + 0x04B)
#define    TPM_RC_NV_DEFINED           (TPM_RC)(RC_VER1 + 0x04C)
#define    TPM_RC_BAD_CONTEXT          (TPM_RC)(RC_VER1 + 0x050)
#define    TPM_RC_CPHASH               (TPM_RC)(RC_VER1 + 0x051)
#define    TPM_RC_PARENT               (TPM_RC)(RC_VER1 + 0x052)
#define    TPM_RC_NEEDS_TEST           (TPM_RC)(RC_VER1 + 0x053)
#define    TPM_RC_NO_RESULT            (TPM_RC)(RC_VER1 + 0x054)
#define    TPM_RC_SENSITIVE            (TPM_RC)(RC_VER1 + 0x055)
#define    RC_MAX_FM0                  (TPM_RC)(RC_VER1 + 0x07F)
#define    RC_FMT1                     (TPM_RC)(0x080)
#define    TPM_RC_ASYMMETRIC           (TPM_RC)(RC_FMT1 + 0x001)
#define    TPM_RC_ATTRIBUTES           (TPM_RC)(RC_FMT1 + 0x002)
#define    TPM_RC_HASH                 (TPM_RC)(RC_FMT1 + 0x003)
#define    TPM_RC_VALUE                (TPM_RC)(RC_FMT1 + 0x004)
#define    TPM_RC_HIERARCHY            (TPM_RC)(RC_FMT1 + 0x005)
#define    TPM_RC_KEY_SIZE             (TPM_RC)(RC_FMT1 + 0x007)
#define    TPM_RC_MGF                  (TPM_RC)(RC_FMT1 + 0x008)
#define    TPM_RC_MODE                 (TPM_RC)(RC_FMT1 + 0x009)
#define    TPM_RC_TYPE                 (TPM_RC)(RC_FMT1 + 0x00A)
#define    TPM_RC_HANDLE               (TPM_RC)(RC_FMT1 + 0x00B)
#define    TPM_RC_KDF                  (TPM_RC)(RC_FMT1 + 0x00C)
#define    TPM_RC_RANGE                (TPM_RC)(RC_FMT1 + 0x00D)
#define    TPM_RC_AUTH_FAIL            (TPM_RC)(RC_FMT1 + 0x00E)
#define    TPM_RC_NONCE                (TPM_RC)(RC_FMT1 + 0x00F)
#define    TPM_RC_PP                   (TPM_RC)(RC_FMT1 + 0x010)
#define    TPM_RC_SCHEME               (TPM_RC)(RC_FMT1 + 0x012)
#define    TPM_RC_SIZE                 (TPM_RC)(RC_FMT1 + 0x015)
#define    TPM_RC_SYMMETRIC            (TPM_RC)(RC_FMT1 + 0x016)
#define    TPM_RC_TAG                  (TPM_RC)(RC_FMT1 + 0x017)
#define    TPM_RC_SELECTOR             (TPM_RC)(RC_FMT1 + 0x018)
#define    TPM_RC_INSUFFICIENT         (TPM_RC)(RC_FMT1 + 0x01A)
#define    TPM_RC_SIGNATURE            (TPM_RC)(RC_FMT1 + 0x01B)
#define    TPM_RC_KEY                  (TPM_RC)(RC_FMT1 + 0x01C)
#define    TPM_RC_POLICY_FAIL          (TPM_RC)(RC_FMT1 + 0x01D)
#define    TPM_RC_INTEGRITY            (TPM_RC)(RC_FMT1 + 0x01F)
#define    TPM_RC_TICKET               (TPM_RC)(RC_FMT1 + 0x020)
#define    TPM_RC_RESERVED_BITS        (TPM_RC)(RC_FMT1 + 0x021)
#define    TPM_RC_BAD_AUTH             (TPM_RC)(RC_FMT1 + 0x022)
#define    TPM_RC_EXPIRED              (TPM_RC)(RC_FMT1 + 0x023)
#define    TPM_RC_POLICY_CC            (TPM_RC)(RC_FMT1 + 0x024 )
#define    TPM_RC_BINDING              (TPM_RC)(RC_FMT1 + 0x025)
#define    TPM_RC_CURVE                (TPM_RC)(RC_FMT1 + 0x026)
#define    RC_WARN                     (TPM_RC)(0x900)
#define    TPM_RC_CONTEXT_GAP          (TPM_RC)(RC_WARN + 0x001)
#define    TPM_RC_OBJECT_MEMORY        (TPM_RC)(RC_WARN + 0x002)
#define    TPM_RC_SESSION_MEMORY       (TPM_RC)(RC_WARN + 0x003)
#define    TPM_RC_MEMORY               (TPM_RC)(RC_WARN + 0x004)
#define    TPM_RC_SESSION_HANDLES      (TPM_RC)(RC_WARN + 0x005)
#define    TPM_RC_OBJECT_HANDLES       (TPM_RC)(RC_WARN + 0x006)
#define    TPM_RC_LOCALITY             (TPM_RC)(RC_WARN + 0x007)
#define    TPM_RC_YIELDED              (TPM_RC)(RC_WARN + 0x008)
#define    TPM_RC_CANCELLED            (TPM_RC)(RC_WARN + 0x009)
#define    TPM_RC_TESTING              (TPM_RC)(RC_WARN + 0x00A)
#define    TPM_RC_REFERENCE_H0         (TPM_RC)(RC_WARN + 0x010)
#define    TPM_RC_REFERENCE_H1         (TPM_RC)(RC_WARN + 0x011)
#define    TPM_RC_REFERENCE_H2         (TPM_RC)(RC_WARN + 0x012)
#define    TPM_RC_REFERENCE_H3         (TPM_RC)(RC_WARN + 0x013)
#define    TPM_RC_REFERENCE_H4         (TPM_RC)(RC_WARN + 0x014)
#define    TPM_RC_REFERENCE_H5         (TPM_RC)(RC_WARN + 0x015)
#define    TPM_RC_REFERENCE_H6         (TPM_RC)(RC_WARN + 0x016)
#define    TPM_RC_REFERENCE_S0         (TPM_RC)(RC_WARN + 0x018)
#define    TPM_RC_REFERENCE_S1         (TPM_RC)(RC_WARN + 0x019)
#define    TPM_RC_REFERENCE_S2         (TPM_RC)(RC_WARN + 0x01A)
#define    TPM_RC_REFERENCE_S3         (TPM_RC)(RC_WARN + 0x01B)
#define    TPM_RC_REFERENCE_S4         (TPM_RC)(RC_WARN + 0x01C)
#define    TPM_RC_REFERENCE_S5         (TPM_RC)(RC_WARN + 0x01D)
#define    TPM_RC_REFERENCE_S6         (TPM_RC)(RC_WARN + 0x01E)
#define    TPM_RC_NV_RATE              (TPM_RC)(RC_WARN + 0x020)
#define    TPM_RC_LOCKOUT              (TPM_RC)(RC_WARN + 0x021)
#define    TPM_RC_RETRY                (TPM_RC)(RC_WARN + 0x022)
#define    TPM_RC_NV_UNAVAILABLE       (TPM_RC)(RC_WARN + 0x023)
#define    TPM_RC_NOT_USED             (TPM_RC)(RC_WARN + 0x7F)
#define    TPM_RC_H                    (TPM_RC)(0x000)
#define    TPM_RC_P                    (TPM_RC)(0x040)
#define    TPM_RC_S                    (TPM_RC)(0x800)
#define    TPM_RC_1                    (TPM_RC)(0x100)
#define    TPM_RC_2                    (TPM_RC)(0x200)
#define    TPM_RC_3                    (TPM_RC)(0x300)
#define    TPM_RC_4                    (TPM_RC)(0x400)
#define    TPM_RC_5                    (TPM_RC)(0x500)
#define    TPM_RC_6                    (TPM_RC)(0x600)
#define    TPM_RC_7                    (TPM_RC)(0x700)
#define    TPM_RC_8                    (TPM_RC)(0x800)
#define    TPM_RC_9                    (TPM_RC)(0x900)
#define    TPM_RC_A                    (TPM_RC)(0xA00)
#define    TPM_RC_B                    (TPM_RC)(0xB00)
#define    TPM_RC_C                    (TPM_RC)(0xC00)
#define    TPM_RC_D                    (TPM_RC)(0xD00)
#define    TPM_RC_E                    (TPM_RC)(0xE00)
#define    TPM_RC_F                    (TPM_RC)(0xF00)
#define    TPM_RC_N_MASK               (TPM_RC)(0xF00)

// Table 15 -- TPM_CLOCK_ADJUST Constants <I>
typedef INT8 TPM_CLOCK_ADJUST;

#define    TPM_CLOCK_COARSE_SLOWER    (TPM_CLOCK_ADJUST)(-3)
#define    TPM_CLOCK_MEDIUM_SLOWER    (TPM_CLOCK_ADJUST)(-2)
#define    TPM_CLOCK_FINE_SLOWER      (TPM_CLOCK_ADJUST)(-1)
#define    TPM_CLOCK_NO_CHANGE        (TPM_CLOCK_ADJUST)(0)
#define    TPM_CLOCK_FINE_FASTER      (TPM_CLOCK_ADJUST)(1)
#define    TPM_CLOCK_MEDIUM_FASTER    (TPM_CLOCK_ADJUST)(2)
#define    TPM_CLOCK_COARSE_FASTER    (TPM_CLOCK_ADJUST)(3)

// Table 16 -- TPM_EO Constants <I/O>
typedef UINT16 TPM_EO;

#define    TPM_EO_EQ             (TPM_EO)(0x0000)
#define    TPM_EO_NEQ            (TPM_EO)(0x0001)
#define    TPM_EO_SIGNED_GT      (TPM_EO)(0x0002)
#define    TPM_EO_UNSIGNED_GT    (TPM_EO)(0x0003)
#define    TPM_EO_SIGNED_LT      (TPM_EO)(0x0004)
#define    TPM_EO_UNSIGNED_LT    (TPM_EO)(0x0005)
#define    TPM_EO_SIGNED_GE      (TPM_EO)(0x0006)
#define    TPM_EO_UNSIGNED_GE    (TPM_EO)(0x0007)
#define    TPM_EO_SIGNED_LE      (TPM_EO)(0x0008)
#define    TPM_EO_UNSIGNED_LE    (TPM_EO)(0x0009)
#define    TPM_EO_BITSET         (TPM_EO)(0x000A)
#define    TPM_EO_BITCLEAR       (TPM_EO)(0x000B)

// Table 17 -- TPM_ST Constants <I/O,S>
typedef UINT16 TPM_ST;

#define    TPM_ST_RSP_COMMAND             (TPM_ST)(0x00C4)
#define    TPM_ST_NULL                    (TPM_ST)(0X8000)
#define    TPM_ST_NO_SESSIONS             (TPM_ST)(0x8001)
#define    TPM_ST_SESSIONS                (TPM_ST)(0x8002)
#define    TPM_ST_ATTEST_COMMAND_AUDIT    (TPM_ST)(0x8015)
#define    TPM_ST_ATTEST_SESSION_AUDIT    (TPM_ST)(0x8016)
#define    TPM_ST_ATTEST_CERTIFY          (TPM_ST)(0x8017)
#define    TPM_ST_ATTEST_QUOTE            (TPM_ST)(0x8018)
#define    TPM_ST_ATTEST_TIME             (TPM_ST)(0x8019)
#define    TPM_ST_ATTEST_CREATION         (TPM_ST)(0x801A)
#define    TPM_ST_ATTEST_NV               (TPM_ST)(0x801B)
#define    TPM_ST_CREATION                (TPM_ST)(0x8021)
#define    TPM_ST_VERIFIED                (TPM_ST)(0x8022)
#define    TPM_ST_AUTH_SECRET             (TPM_ST)(0x8023)
#define    TPM_ST_HASHCHECK               (TPM_ST)(0x8024)
#define    TPM_ST_AUTH_SIGNED             (TPM_ST)(0x8025)
#define    TPM_ST_FU_MANIFEST             (TPM_ST)(0x8029)

// Table 18 -- TPM_SU Constants <I>
typedef UINT16 TPM_SU;

#define    TPM_SU_CLEAR     (TPM_SU)(0x0000)
#define    TPM_SU_STATE     (TPM_SU)(0x0001)

// Table 19 -- TPM_SE Constants <I>
typedef UINT8 TPM_SE;

#define    TPM_SE_HMAC      (TPM_SE)(0x00)
#define    TPM_SE_POLICY    (TPM_SE)(0x01)
#define    TPM_SE_TRIAL     (TPM_SE)(0x03)

// Table 20 -- TPM_CAP Constants <I/O,S>
typedef UINT32 TPM_CAP;

#define    TPM_CAP_FIRST              (TPM_CAP)(0x00000000)
#define    TPM_CAP_ALGS               (TPM_CAP)(0x00000000)
#define    TPM_CAP_HANDLES            (TPM_CAP)(0x00000001)
#define    TPM_CAP_COMMANDS           (TPM_CAP)(0x00000002)
#define    TPM_CAP_PP_COMMANDS        (TPM_CAP)(0x00000003)
#define    TPM_CAP_AUDIT_COMMANDS     (TPM_CAP)(0x00000004)
#define    TPM_CAP_PCRS               (TPM_CAP)(0x00000005)
#define    TPM_CAP_TPM_PROPERTIES     (TPM_CAP)(0x00000006)
#define    TPM_CAP_PCR_PROPERTIES     (TPM_CAP)(0x00000007)
#define    TPM_CAP_ECC_CURVES         (TPM_CAP)(0x00000008)
#define    TPM_CAP_LAST               (TPM_CAP)(0x00000008)
#define    TPM_CAP_VENDOR_PROPERTY    (TPM_CAP)(0x00000100)

// Table 21 -- TPM_PT Constants <I/O,S>
typedef UINT32 TPM_PT;

#define    TPM_PT_NONE                   (TPM_PT)(0x00000000)
#define    PT_GROUP                      (TPM_PT)(0x00000100)
#define    PT_FIXED                      (TPM_PT)(PT_GROUP * 1)
#define    TPM_PT_FAMILY_INDICATOR       (TPM_PT)(PT_FIXED + 0)
#define    TPM_PT_LEVEL                  (TPM_PT)(PT_FIXED + 1)
#define    TPM_PT_REVISION               (TPM_PT)(PT_FIXED + 2)
#define    TPM_PT_DAY_OF_YEAR            (TPM_PT)(PT_FIXED + 3)
#define    TPM_PT_YEAR                   (TPM_PT)(PT_FIXED + 4)
#define    TPM_PT_MANUFACTURER           (TPM_PT)(PT_FIXED + 5)
#define    TPM_PT_VENDOR_STRING_1        (TPM_PT)(PT_FIXED + 6)
#define    TPM_PT_VENDOR_STRING_2        (TPM_PT)(PT_FIXED + 7)
#define    TPM_PT_VENDOR_STRING_3        (TPM_PT)(PT_FIXED + 8)
#define    TPM_PT_VENDOR_STRING_4        (TPM_PT)(PT_FIXED + 9)
#define    TPM_PT_VENDOR_TPM_TYPE        (TPM_PT)(PT_FIXED + 10)
#define    TPM_PT_FIRMWARE_VERSION_1     (TPM_PT)(PT_FIXED + 11)
#define    TPM_PT_FIRMWARE_VERSION_2     (TPM_PT)(PT_FIXED + 12)
#define    TPM_PT_INPUT_BUFFER           (TPM_PT)(PT_FIXED + 13)
#define    TPM_PT_HR_TRANSIENT_MIN       (TPM_PT)(PT_FIXED + 14)
#define    TPM_PT_HR_PERSISTENT_MIN      (TPM_PT)(PT_FIXED + 15)
#define    TPM_PT_HR_LOADED_MIN          (TPM_PT)(PT_FIXED + 16)
#define    TPM_PT_ACTIVE_SESSIONS_MAX    (TPM_PT)(PT_FIXED + 17)
#define    TPM_PT_PCR_COUNT              (TPM_PT)(PT_FIXED + 18)
#define    TPM_PT_PCR_SELECT_MIN         (TPM_PT)(PT_FIXED + 19)
#define    TPM_PT_CONTEXT_GAP_MAX        (TPM_PT)(PT_FIXED + 20)
#define    TPM_PT_NV_COUNTERS_MAX        (TPM_PT)(PT_FIXED + 22)
#define    TPM_PT_NV_INDEX_MAX           (TPM_PT)(PT_FIXED + 23)
#define    TPM_PT_MEMORY                 (TPM_PT)(PT_FIXED + 24)
#define    TPM_PT_CLOCK_UPDATE           (TPM_PT)(PT_FIXED + 25)
#define    TPM_PT_CONTEXT_HASH           (TPM_PT)(PT_FIXED + 26)
#define    TPM_PT_CONTEXT_SYM            (TPM_PT)(PT_FIXED + 27)
#define    TPM_PT_CONTEXT_SYM_SIZE       (TPM_PT)(PT_FIXED + 28)
#define    TPM_PT_ORDERLY_COUNT          (TPM_PT)(PT_FIXED + 29)
#define    TPM_PT_MAX_COMMAND_SIZE       (TPM_PT)(PT_FIXED + 30)
#define    TPM_PT_MAX_RESPONSE_SIZE      (TPM_PT)(PT_FIXED + 31)
#define    TPM_PT_MAX_DIGEST             (TPM_PT)(PT_FIXED + 32)
#define    TPM_PT_MAX_OBJECT_CONTEXT     (TPM_PT)(PT_FIXED + 33)
#define    TPM_PT_MAX_SESSION_CONTEXT    (TPM_PT)(PT_FIXED + 34)
#define    TPM_PT_PS_FAMILY_INDICATOR    (TPM_PT)(PT_FIXED + 35)
#define    TPM_PT_PS_LEVEL               (TPM_PT)(PT_FIXED + 36)
#define    TPM_PT_PS_REVISION            (TPM_PT)(PT_FIXED + 37)
#define    TPM_PT_PS_DAY_OF_YEAR         (TPM_PT)(PT_FIXED + 38)
#define    TPM_PT_PS_YEAR                (TPM_PT)(PT_FIXED + 39)
#define    TPM_PT_SPLIT_MAX              (TPM_PT)(PT_FIXED + 40)
#define    TPM_PT_TOTAL_COMMANDS         (TPM_PT)(PT_FIXED + 41)
#define    TPM_PT_LIBRARY_COMMANDS       (TPM_PT)(PT_FIXED + 42)
#define    TPM_PT_VENDOR_COMMANDS        (TPM_PT)(PT_FIXED + 43)
#define    PT_VAR                        (TPM_PT)(PT_GROUP * 2)
#define    TPM_PT_PERMANENT              (TPM_PT)(PT_VAR + 0)
#define    TPM_PT_STARTUP_CLEAR          (TPM_PT)(PT_VAR + 1)
#define    TPM_PT_HR_NV_INDEX            (TPM_PT)(PT_VAR + 2)
#define    TPM_PT_HR_LOADED              (TPM_PT)(PT_VAR + 3)
#define    TPM_PT_HR_LOADED_AVAIL        (TPM_PT)(PT_VAR + 4)
#define    TPM_PT_HR_ACTIVE              (TPM_PT)(PT_VAR + 5)
#define    TPM_PT_HR_ACTIVE_AVAIL        (TPM_PT)(PT_VAR + 6)
#define    TPM_PT_HR_TRANSIENT_AVAIL     (TPM_PT)(PT_VAR + 7)
#define    TPM_PT_HR_PERSISTENT          (TPM_PT)(PT_VAR + 8)
#define    TPM_PT_HR_PERSISTENT_AVAIL    (TPM_PT)(PT_VAR + 9)
#define    TPM_PT_NV_COUNTERS            (TPM_PT)(PT_VAR + 10)
#define    TPM_PT_NV_COUNTERS_AVAIL      (TPM_PT)(PT_VAR + 11)
#define    TPM_PT_ALGORITHM_SET          (TPM_PT)(PT_VAR + 12)
#define    TPM_PT_LOADED_CURVES          (TPM_PT)(PT_VAR + 13)
#define    TPM_PT_LOCKOUT_COUNTER        (TPM_PT)(PT_VAR + 14)
#define    TPM_PT_MAX_AUTH_FAIL          (TPM_PT)(PT_VAR + 15)
#define    TPM_PT_LOCKOUT_INTERVAL       (TPM_PT)(PT_VAR + 16)
#define    TPM_PT_LOCKOUT_RECOVERY       (TPM_PT)(PT_VAR + 17)
#define    TPM_PT_NV_WRITE_RECOVERY      (TPM_PT)(PT_VAR + 18)
#define    TPM_PT_AUDIT_COUNTER_0        (TPM_PT)(PT_VAR + 19)
#define    TPM_PT_AUDIT_COUNTER_1        (TPM_PT)(PT_VAR + 20)

// Table 22 -- TPM_PT_PCR Constants <I/O,S>
typedef UINT32 TPM_PT_PCR;

#define    TPM_PT_PCR_FIRST         (TPM_PT_PCR)(0x00000000)
#define    TPM_PT_PCR_SAVE          (TPM_PT_PCR)(0x00000000)
#define    TPM_PT_PCR_EXTEND_L0     (TPM_PT_PCR)(0x00000001)
#define    TPM_PT_PCR_RESET_L0      (TPM_PT_PCR)(0x00000002)
#define    TPM_PT_PCR_EXTEND_L1     (TPM_PT_PCR)(0x00000003)
#define    TPM_PT_PCR_RESET_L1      (TPM_PT_PCR)(0x00000004)
#define    TPM_PT_PCR_EXTEND_L2     (TPM_PT_PCR)(0x00000005)
#define    TPM_PT_PCR_RESET_L2      (TPM_PT_PCR)(0x00000006)
#define    TPM_PT_PCR_EXTEND_L3     (TPM_PT_PCR)(0x00000007)
#define    TPM_PT_PCR_RESET_L3      (TPM_PT_PCR)(0x00000008)
#define    TPM_PT_PCR_EXTEND_L4     (TPM_PT_PCR)(0x00000009)
#define    TPM_PT_PCR_RESET_L4      (TPM_PT_PCR)(0x0000000A)
#define    TPM_PT_PCR_DRTM_RESET    (TPM_PT_PCR)(0x0000000B)
#define    TPM_PT_PCR_POLICY        (TPM_PT_PCR)(0x0000000C)
#define    TPM_PT_PCR_AUTH          (TPM_PT_PCR)(0x0000000D)
#define    TPM_PT_PCR_LAST          (TPM_PT_PCR)(0x0000000D)

// Table 23 -- TPM_PS Constants <O,S>
typedef UINT32 TPM_PS;

#define    TPM_PS_MAIN              (TPM_PS)(0x00000000)
#define    TPM_PS_PC                (TPM_PS)(0x00000001)
#define    TPM_PS_PDA               (TPM_PS)(0x00000002)
#define    TPM_PS_CELL_PHONE        (TPM_PS)(0x00000003)
#define    TPM_PS_SERVER            (TPM_PS)(0x00000004)
#define    TPM_PS_PERIPHERAL        (TPM_PS)(0x00000005)
#define    TPM_PS_TSS               (TPM_PS)(0x00000006)
#define    TPM_PS_STORAGE           (TPM_PS)(0x00000007)
#define    TPM_PS_AUTHENTICATION    (TPM_PS)(0x00000008)
#define    TPM_PS_EMBEDDED          (TPM_PS)(0x00000009)
#define    TPM_PS_HARDCOPY          (TPM_PS)(0x0000000A)
#define    TPM_PS_INFRASTRUCTURE    (TPM_PS)(0x0000000B)
#define    TPM_PS_VIRTUALIZATION    (TPM_PS)(0x0000000C)
#define    TPM_PS_TNC               (TPM_PS)(0x0000000D)
#define    TPM_PS_MULTI_TENANT      (TPM_PS)(0x0000000E)
#define    TPM_PS_TC                (TPM_PS)(0x0000000F)

// Table 24 -- Handles Types <I/O>
typedef UINT32    TPM_HANDLE;
typedef UINT8 TPM_HT;

#define    TPM_HT_PCR               (TPM_HT)(0x00)
#define    TPM_HT_NV_INDEX          (TPM_HT)(0x01)
#define    TPM_HT_HMAC_SESSION      (TPM_HT)(0x02)
#define    TPM_HT_LOADED_SESSION    (TPM_HT)(0x02)
#define    TPM_HT_POLICY_SESSION    (TPM_HT)(0x03)
#define    TPM_HT_ACTIVE_SESSION    (TPM_HT)(0x03)
#define    TPM_HT_PERMANENT         (TPM_HT)(0x40)
#define    TPM_HT_TRANSIENT         (TPM_HT)(0x80)
#define    TPM_HT_PERSISTENT        (TPM_HT)(0x81)

// Table 26 -- TPM_RH Constants <I,S>
typedef UINT32 TPM_RH;

#define    TPM_RH_FIRST          (TPM_RH)(0x40000000)
#define    TPM_RH_SRK            (TPM_RH)(0x40000000)
#define    TPM_RH_OWNER          (TPM_RH)(0x40000001)
#define    TPM_RH_REVOKE         (TPM_RH)(0x40000002)
#define    TPM_RH_TRANSPORT      (TPM_RH)(0x40000003)
#define    TPM_RH_OPERATOR       (TPM_RH)(0x40000004)
#define    TPM_RH_ADMIN          (TPM_RH)(0x40000005)
#define    TPM_RH_EK             (TPM_RH)(0x40000006)
#define    TPM_RH_NULL           (TPM_RH)(0x40000007)
#define    TPM_RH_UNASSIGNED     (TPM_RH)(0x40000008)
#define    TPM_RH_PW             (TPM_RH)(0x40000009)
#define    TPM_RS_PW             (TPM_RH)(0x40000009)
#define    TPM_RH_LOCKOUT        (TPM_RH)(0x4000000A)
#define    TPM_RH_ENDORSEMENT    (TPM_RH)(0x4000000B)
#define    TPM_RH_PLATFORM       (TPM_RH)(0x4000000C)
#define    TPM_RH_LAST           (TPM_RH)(0x4000000C)
#define    TPM_RH_PCR0           (TPM_RH)(0x00000000)

// Table 27 -- TPM_HC Constants <I,S>
typedef TPM_HANDLE TPM_HC;

#define    HR_HANDLE_MASK          (TPM_HC)(0x00FFFFFF)
#define    HR_RANGE_MASK           (TPM_HC)(0xFF000000)
#define    HR_SHIFT                (TPM_HC)(24)
#define    HR_PCR                  (TPM_HC)((TPM_HC)TPM_HT_PCR << HR_SHIFT)
#define    HR_HMAC_SESSION         (TPM_HC)((TPM_HC)TPM_HT_HMAC_SESSION << HR_SHIFT)
#define    HR_POLICY_SESSION       (TPM_HC)((TPM_HC)TPM_HT_POLICY_SESSION << HR_SHIFT)
#define    HR_TRANSIENT            (TPM_HC)((TPM_HC)TPM_HT_TRANSIENT << HR_SHIFT)
#define    HR_PERSISTENT           (TPM_HC)((TPM_HC)TPM_HT_PERSISTENT << HR_SHIFT)
#define    HR_NV_INDEX             (TPM_HC)((TPM_HC)TPM_HT_NV_INDEX << HR_SHIFT)
#define    HR_PERMANENT            (TPM_HC)((TPM_HC)TPM_HT_PERMANENT << HR_SHIFT)
#define    PCR_FIRST               (TPM_HC)(TPM_RH_PCR0)
#define    PCR_LAST                (TPM_HC)(PCR_FIRST + IMPLEMENTATION_PCR-1)
#define    HMAC_SESSION_FIRST      (TPM_HC)(HR_HMAC_SESSION + 0)
#define    HMAC_SESSION_LAST       (TPM_HC)(HMAC_SESSION_FIRST+MAX_ACTIVE_SESSIONS-1)
#define    LOADED_SESSION_FIRST    (TPM_HC)(HMAC_SESSION_FIRST)
#define    LOADED_SESSION_LAST     (TPM_HC)(HMAC_SESSION_LAST)
#define    POLICY_SESSION_FIRST    (TPM_HC)(HR_POLICY_SESSION + 0)
#define    POLICY_SESSION_LAST     (TPM_HC)(POLICY_SESSION_FIRST + MAX_ACTIVE_SESSIONS-1)
#define    TRANSIENT_FIRST         (TPM_HC)(HR_TRANSIENT +  0)
#define    ACTIVE_SESSION_FIRST    (TPM_HC)(POLICY_SESSION_FIRST)
#define    ACTIVE_SESSION_LAST     (TPM_HC)(POLICY_SESSION_LAST)
#define    TRANSIENT_LAST          (TPM_HC)(TRANSIENT_FIRST+MAX_LOADED_OBJECTS-1)
#define    PERSISTENT_FIRST        (TPM_HC)(HR_PERSISTENT  +  0)
#define    PERSISTENT_LAST         (TPM_HC)(PERSISTENT_FIRST + 0x00FFFFFF)
#define    PLATFORM_PERSISTENT     (TPM_HC)(PERSISTENT_FIRST + 0x00800000)
#define    NV_INDEX_FIRST          (TPM_HC)(HR_NV_INDEX + 0)
#define    NV_INDEX_LAST           (TPM_HC)(NV_INDEX_FIRST + 0x00FFFFFF)
#define    PERMANENT_FIRST         (TPM_HC)(TPM_RH_FIRST)
#define    PERMANENT_LAST          (TPM_HC)(TPM_RH_LAST)

// Table 28 -- TPMA_ALGORITHM Bits <I/O>
typedef struct {
    unsigned int asymmetric : 1;
    unsigned int symmetric  : 1;
    unsigned int hash       : 1;
    unsigned int object     : 1;
    unsigned int reserved5  : 4;
    unsigned int signing    : 1;
    unsigned int encrypting : 1;
    unsigned int method     : 1;
    unsigned int reserved9  : 21;
} TPMA_ALGORITHM ;

// Table 29 -- TPMA_OBJECT Bits <I/O>
typedef struct {
    unsigned int reserved1           : 1;
    unsigned int fixedTPM            : 1;
    unsigned int stClear             : 1;
    unsigned int reserved4           : 1;
    unsigned int fixedParent         : 1;
    unsigned int sensitiveDataOrigin : 1;
    unsigned int userWithAuth        : 1;
    unsigned int adminWithPolicy     : 1;
    unsigned int Pad9                : 1;    //Inserted extra pad
    unsigned int reserved9           : 1;
    unsigned int noDA                : 1;
    unsigned int reserved11          : 5;
    unsigned int restricted          : 1;
    unsigned int decrypt             : 1;
    unsigned int sign                : 1;
    unsigned int Pad15               : 9;    //Inserted extra pad
    unsigned int softwareUse         : 4;
} TPMA_OBJECT ;

// Table 30 -- TPMA_SESSION Bits <I/O>
typedef struct {
    unsigned int continueSession : 1;
    unsigned int auditExclusive  : 1;
    unsigned int auditReset      : 1;
    unsigned int reserved4       : 2;
    unsigned int decrypt         : 1;
    unsigned int encrypt         : 1;
    unsigned int audit           : 1;
} TPMA_SESSION ;

// Table 31 -- TPMA_LOCALITY Bits <I/O>
//
// BUGBUG: Use low case here to resolve conflict
//
typedef struct {
    unsigned int locZero  : 1;
    unsigned int locOne   : 1;
    unsigned int locTwo   : 1;
    unsigned int locThree : 1;
    unsigned int locFour  : 1;
    unsigned int reserved6     : 3;
} TPMA_LOCALITY ;

// Table 32 -- TPMA_PERMANENT Bits <O,S>
typedef struct {
    unsigned int ownerAuthSet       : 1;
    unsigned int endorsementAuthSet : 1;
    unsigned int lockoutAuthSet     : 1;
    unsigned int reserved4          : 5;
    unsigned int disableClear       : 1;
    unsigned int inLockout          : 1;
    unsigned int tpmGeneratedEPS    : 1;
    unsigned int reserved8          : 21;
} TPMA_PERMANENT ;

// Table 33 -- TPMA_STARTUP_CLEAR Bits <O,S>
typedef struct {
    unsigned int phEnable  : 1;
    unsigned int shEnable  : 1;
    unsigned int ehEnable  : 1;
    unsigned int reserved4 : 28;
    unsigned int orderly   : 1;
} TPMA_STARTUP_CLEAR ;

// Table 34 -- TPMA_MEMORY Bits <O,S>
typedef struct {
    unsigned int sharedRAM         : 1;
    unsigned int sharedNV          : 1;
    unsigned int objectCopiedToRam : 1;
    unsigned int reserved4         : 29;
} TPMA_MEMORY ;

// Table 35 -- TPMA_CC Bits <O,S>
typedef struct {
    unsigned int commandIndex : 16;
    unsigned int reserved2    : 6;
    unsigned int nv           : 1;
    unsigned int extensive    : 1;
    unsigned int flushed      : 1;
    unsigned int cHandles     : 3;
    unsigned int rHandle      : 1;
    unsigned int V            : 1;
    unsigned int reserved9    : 2;
} TPMA_CC ;

// Table 36 -- TPMI_YES_NO Type <I/O>
typedef BYTE TPMI_YES_NO;

// Table 37 -- TPMI_DH_OBJECT Type <I/O>
typedef TPM_HANDLE TPMI_DH_OBJECT;

// Table 38 -- TPMI_DH_PERSISTENT Type <I/O>
typedef TPM_HANDLE TPMI_DH_PERSISTENT;

// Table 39 -- TPMI_DH_ENTITY Type <I>
typedef TPM_HANDLE TPMI_DH_ENTITY;

// Table 40 -- TPMI_DH_PCR Type <I>
typedef TPM_HANDLE TPMI_DH_PCR;

// Table 41 -- TPMI_SH_AUTH_SESSION Type <I/O>
typedef TPM_HANDLE TPMI_SH_AUTH_SESSION;

// Table 42 -- TPMI_SH_HMAC Type <I/O>
typedef TPM_HANDLE TPMI_SH_HMAC;

// Table 43 -- TPMI_SH_POLICY Type <I/O>
typedef TPM_HANDLE TPMI_SH_POLICY;

// Table 44 -- TPMI_DH_CONTEXT Type <I/O>
typedef TPM_HANDLE TPMI_DH_CONTEXT;

// Table 45 -- TPMI_RH_HIERARCHY Type <I/O>
typedef TPM_HANDLE TPMI_RH_HIERARCHY;

// Table 46 -- TPMI_RH_HIERARCHY_AUTH Type <I>
typedef TPM_HANDLE TPMI_RH_HIERARCHY_AUTH;

// Table 47 -- TPMI_RH_PLATFORM Type <I>
typedef TPM_HANDLE TPMI_RH_PLATFORM;

// Table 48 -- TPMI_RH_OWNER Type <I>
typedef TPM_HANDLE TPMI_RH_OWNER;

// Table 49 -- TPMI_RH_ENDORSEMENT Type <I>
typedef TPM_HANDLE TPMI_RH_ENDORSEMENT;

// Table 50 -- TPMI_RH_PROVISION Type <I>
typedef TPM_HANDLE TPMI_RH_PROVISION;

// Table 51 -- TPMI_RH_CLEAR Type <I>
typedef TPM_HANDLE TPMI_RH_CLEAR;

// Table 52 -- TPMI_RH_NV_AUTH Type <I>
typedef TPM_HANDLE TPMI_RH_NV_AUTH;

// Table 53 -- TPMI_RH_LOCKOUT Type <I>
typedef TPM_HANDLE TPMI_RH_LOCKOUT;

// Table 54 -- TPMI_RH_NV_INDEX Type <I/O>
typedef TPM_HANDLE TPMI_RH_NV_INDEX;

// Table 55 -- TPMI_ALG_HASH Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_HASH;

// Table 56 -- TPMI_ALG_ASYM Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_ASYM;

// Table 57 -- TPMI_ALG_SYM Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_SYM;

// Table 58 -- TPMI_ALG_SYM_OBJECT Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_SYM_OBJECT;

// Table 59 -- TPMI_ALG_SYM_MODE Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_SYM_MODE;

// Table 60 -- TPMI_ALG_KDF Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_KDF;

// Table 61 -- TPMI_ALG_SIG_SCHEME Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_SIG_SCHEME;

// Table 62 -- TPMI_ST_COMMAND_TAG Type <I/O>
typedef TPM_ST TPMI_ST_COMMAND_TAG;

// Table 63 -- TPMS_ALGORITHM_DESCRIPTION Structure <O,S>
typedef struct {
    TPM_ALG_ID        alg;
    TPMA_ALGORITHM    attributes;
} TPMS_ALGORITHM_DESCRIPTION;

// Table 64 -- TPMU_HA Union <I/O,S>
typedef union {
    BYTE  sha1[SHA1_DIGEST_SIZE];
    BYTE  sha256[SHA256_DIGEST_SIZE];
    BYTE  sm3_256[SM3_256_DIGEST_SIZE];
    BYTE  sha384[SHA384_DIGEST_SIZE];
    BYTE  sha512[SHA512_DIGEST_SIZE];
    BYTE  whirlpool[WHIRLPOOL512_DIGEST_SIZE];
} TPMU_HA ;

// Table 65 -- TPMT_HA Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
    TPMU_HA          digest;
} TPMT_HA;

// Table 66 -- TPM2B_DIGEST Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[sizeof(TPMU_HA)];
} DIGEST_2B;

typedef union {
    DIGEST_2B    t;
    TPM2B        b;
} TPM2B_DIGEST;

// Table 67 -- TPM2B_DATA Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[sizeof(TPMT_HA)];
} DATA_2B;

typedef union {
    DATA_2B    t;
    TPM2B      b;
} TPM2B_DATA;

// Table 68 -- TPM2B_NONCE Types <I/O>
typedef TPM2B_DIGEST    TPM2B_NONCE;

// Table 69 -- TPM2B_AUTH Types <I/O>
typedef TPM2B_DIGEST    TPM2B_AUTH;

// Table 70 -- TPM2B_OPERAND Types <I/O>
typedef TPM2B_DIGEST    TPM2B_OPERAND;

// Table 71 -- TPM2B_EVENT Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[1024];
} EVENT_2B;

typedef union {
    EVENT_2B    t;
    TPM2B       b;
} TPM2B_EVENT;

// Table 72 -- TPM2B_MAX_BUFFER Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_DIGEST_BUFFER];
} MAX_BUFFER_2B;

typedef union {
    MAX_BUFFER_2B    t;
    TPM2B            b;
} TPM2B_MAX_BUFFER;

// Table 73 -- TPM2B_TIMEOUT Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[sizeof(UINT64)];
} TIMEOUT_2B;

typedef union {
    TIMEOUT_2B    t;
    TPM2B         b;
} TPM2B_TIMEOUT;

// Table 74 -- TPM2B_IV Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_SYM_BLOCK_SIZE];
} IV_2B;

typedef union {
    IV_2B    t;
    TPM2B    b;
} TPM2B_IV;
typedef union {
    TPMT_HA  digest;
    TPM_HANDLE  handle;

} TPMU_NAME ;

// Table 76 -- TPM2B_NAME Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      name[sizeof(TPMU_NAME)];
} NAME_2B;

typedef union {
    NAME_2B    t;
    TPM2B      b;
} TPM2B_NAME;

// Table 77 -- TPMS_PCR_SELECT Structure <I/O>
typedef struct {
    UINT8    sizeofSelect;
    BYTE     pcrSelect[PCR_SELECT_MAX];
} TPMS_PCR_SELECT;

// Table 78 -- TPMS_PCR_SELECTION Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hash;
    UINT8            sizeofSelect;
    BYTE             pcrSelect[PCR_SELECT_MAX];
} TPMS_PCR_SELECTION;

// Table 82 -- TPMT_TK_CREATION Structure <I/O>
typedef struct {
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_CREATION;

// Table 83 -- TPMT_TK_VERIFIED Structure <I/O>
typedef struct {
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_VERIFIED;

// Table 84 -- TPMT_TK_AUTH Structure <I/O>
typedef struct {
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_AUTH;

// Table 85 -- TPMT_TK_HASHCHECK Structure <I/O>
typedef struct {
    TPM_ST               tag;
    TPMI_RH_HIERARCHY    hierarchy;
    TPM2B_DIGEST         digest;
} TPMT_TK_HASHCHECK;

// Table 86 -- TPMS_ALG_PROPERTY Structure <O,S>
typedef struct {
    TPM_ALG_ID        alg;
    TPMA_ALGORITHM    algProperties;
} TPMS_ALG_PROPERTY;

// Table 87 -- TPMS_TAGGED_PROPERTY Structure <O,S>
typedef struct {
    TPM_PT    property;
    UINT32    value;
} TPMS_TAGGED_PROPERTY;

// Table 88 -- TPMS_TAGGED_PCR_SELECT Structure <O,S>
typedef struct {
    TPM_PT    tag;
    UINT8     sizeofSelect;
    BYTE      pcrSelect[PCR_SELECT_MAX];
} TPMS_TAGGED_PCR_SELECT;

// Table 89 -- TPML_CC Structure <I/O>
typedef struct {
    UINT32    count;
    TPM_CC    commandCodes[MAX_CAP_CC];
} TPML_CC;

// Table 90 -- TPML_CCA Structure <O,S>
typedef struct {
    UINT32     count;
    TPMA_CC    commandAttributes[MAX_CAP_CC];
} TPML_CCA;

// Table 91 -- TPML_ALG Structure <I/O>
typedef struct {
    UINT32        count;
    TPM_ALG_ID    algorithms[MAX_ALG_LIST_SIZE];
} TPML_ALG;

// Table 92 -- TPML_HANDLE Structure <O,S>
typedef struct {
    UINT32        count;
    TPM_HANDLE    handle[MAX_CAP_HANDLES];
} TPML_HANDLE;

// Table 93 -- TPML_DIGEST Structure <I/O>
typedef struct {
    UINT32          count;
    TPM2B_DIGEST    digests[8];
} TPML_DIGEST;

// Table 94 -- TPML_DIGEST_VALUES Structure <I/O>
typedef struct {
    UINT32     count;
    TPMT_HA    digests[HASH_COUNT];
} TPML_DIGEST_VALUES;

// Table 95 -- TPM2B_DIGEST_VALUES Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[sizeof(TPML_DIGEST_VALUES)];
} DIGEST_VALUES_2B;

typedef union {
    DIGEST_VALUES_2B    t;
    TPM2B               b;
} TPM2B_DIGEST_VALUES;

// Table 96 -- TPML_PCR_SELECTION Structure <I/O>
typedef struct {
    UINT32                count;
    TPMS_PCR_SELECTION    pcrSelections[HASH_COUNT];
} TPML_PCR_SELECTION;

// Table 97 -- TPML_ALG_PROPERTY Structure <O,S>
typedef struct {
    UINT32               count;
    TPMS_ALG_PROPERTY    algProperties[MAX_CAP_ALGS];
} TPML_ALG_PROPERTY;

// Table 98 -- TPML_TAGGED_TPM_PROPERTY Structure <O,S>
typedef struct {
    UINT32                  count;
    TPMS_TAGGED_PROPERTY    tpmProperty[MAX_TPM_PROPERTIES];
} TPML_TAGGED_TPM_PROPERTY;

// Table 99 -- TPML_TAGGED_PCR_PROPERTY Structure <O,S>
typedef struct {
    UINT32                    count;
    TPMS_TAGGED_PCR_SELECT    pcrProperty[MAX_PCR_PROPERTIES];
} TPML_TAGGED_PCR_PROPERTY;

// Table 100 -- TPML_ECC_CURVE Structure <O,S>
typedef struct {
    UINT32           count;
    TPM_ECC_CURVE    eccCurves[MAX_ECC_CURVES];
} TPML_ECC_CURVE;

// Table 101 -- TPMU_CAPABILITIES Union <O,S>
typedef union {
    TPML_ALG_PROPERTY  algorithms;
    TPML_HANDLE  handles;
    TPML_CCA  command;
    TPML_CC  ppCommands;
    TPML_CC  auditCommands;
    TPML_PCR_SELECTION  assignedPCR;
    TPML_TAGGED_TPM_PROPERTY  tpmProperties;
    TPML_TAGGED_PCR_PROPERTY  pcrProperties;
    TPML_ECC_CURVE  eccCurves;

} TPMU_CAPABILITIES ;

// Table 102 -- TPMS_CAPABILITY_DATA Structure <O,S>
typedef struct {
    TPM_CAP              capability;
    TPMU_CAPABILITIES    data;
} TPMS_CAPABILITY_DATA;

// Table 103 -- TPMS_CLOCK_INFO Structure <I/O>
typedef struct {
    UINT64         clock;
    UINT32         resetCount;
    UINT32         restartCount;
    TPMI_YES_NO    safe;
} TPMS_CLOCK_INFO;

// Table 104 -- TPMS_TIME_INFO Structure <I/O>
typedef struct {
    UINT64             time;
    TPMS_CLOCK_INFO    clockInfo;
} TPMS_TIME_INFO;

// Table 105 -- TPMS_TIME_ATTEST_INFO Structure <O,S>
typedef struct {
    TPMS_TIME_INFO    time;
    UINT64            firmwareVersion;
} TPMS_TIME_ATTEST_INFO;

// Table 106 -- TPMS_CERTIFY_INFO Structure <O,S>
typedef struct {
    TPM2B_NAME    name;
    TPM2B_NAME    qualifiedName;
} TPMS_CERTIFY_INFO;

// Table 107 -- TPMS_QUOTE_INFO Structure <O,S>
typedef struct {
    TPML_PCR_SELECTION    pcrSelect;
    TPM2B_DIGEST          pcrDigest;
} TPMS_QUOTE_INFO;

// Table 108 -- TPMS_COMMAND_AUDIT_INFO Structure <O,S>
typedef struct {
    UINT64          auditCounter;
    TPM_ALG_ID      digestAlg;
    TPM2B_DIGEST    auditDigest;
    TPM2B_DIGEST    commandDigest;
} TPMS_COMMAND_AUDIT_INFO;

// Table 109 -- TPMS_SESSION_AUDIT_INFO Structure <O,S>
typedef struct {
    TPMI_YES_NO     exclusiveSession;
    TPM2B_DIGEST    sessionDigest;
} TPMS_SESSION_AUDIT_INFO;

// Table 110 -- TPMS_CREATION_INFO Structure <O,S>
typedef struct {
    TPM2B_NAME      objectName;
    TPM2B_DIGEST    creationHash;
} TPMS_CREATION_INFO;

// Table 111 -- TPMS_NV_CERTIFY_INFO Structure <O,S>
typedef struct {
    TPM2B_MAX_BUFFER    nvContents;
} TPMS_NV_CERTIFY_INFO;

// Table 112 -- TPMI_ST_ATTEST Type <O,S>
typedef TPM_ST TPMI_ST_ATTEST;

// Table 113 -- TPMU_ATTEST Union <O,S>
typedef union {
    TPMS_CERTIFY_INFO  certify;
    TPMS_CREATION_INFO  creation;
    TPMS_QUOTE_INFO  quote;
    TPMS_COMMAND_AUDIT_INFO  commandAudit;
    TPMS_SESSION_AUDIT_INFO  sessionAudit;
    TPMS_TIME_ATTEST_INFO  time;
    TPMS_NV_CERTIFY_INFO  nv;

} TPMU_ATTEST ;

// Table 114 -- TPMS_ATTEST Structure <O,S>
typedef struct {
    TPM_GENERATED      magic;
    TPMI_ST_ATTEST     type;
    TPM2B_NAME         qualifiedSigner;
    TPM2B_DATA         extraData;
    TPMS_CLOCK_INFO    clockInfo;
    UINT64             firmwareVersion;
    TPMU_ATTEST        attested;
} TPMS_ATTEST;

// Table 115 -- TPM2B_ATTEST Structure <O,S>
typedef struct {
    UINT16    size;
    BYTE      attestationData[sizeof(TPMS_ATTEST)];
} ATTEST_2B;

typedef union {
    ATTEST_2B    t;
    TPM2B        b;
} TPM2B_ATTEST;

// Table 116 -- TPMS_AUTH_SESSION_COMMAND Structure <I>
typedef struct {
    TPMI_SH_AUTH_SESSION    sessionHandle;
    TPM2B_NONCE             nonce;
    TPMA_SESSION            sessionAttributes;
    TPM2B_AUTH              auth;
} TPMS_AUTH_SESSION_COMMAND;

// Table 117 -- TPMS_AUTH_SESSION_RESPONSE Structure <O,S>
typedef struct {
    TPM2B_NONCE     nonce;
    TPMA_SESSION    sessionAttributes;
    TPM2B_AUTH      auth;
} TPMS_AUTH_SESSION_RESPONSE;
typedef struct {
    TPM2B_AUTH      sessionKey;
    TPM2B_AUTH      authValue;
    TPM2B_DIGEST    pHash;
    TPM2B_NONCE     nonceNewer;
    TPM2B_NONCE     nonceOlder;
    TPMA_SESSION    sessionFlags;
} TPMS_AUTH_COMPUTE_NOT_BOUND;
typedef struct {
    TPM2B_DIGEST    sessionKey;
    TPM2B_DIGEST    pHash;
    TPM2B_NONCE     nonceNewer;
    TPM2B_NONCE     nonceOlder;
    TPMA_SESSION    sessionFlags;
} TPMS_AUTH_COMPUTE_BOUND;

// Table 120 -- TPMI_AES_KEY_BITS Type <I/O>
typedef TPM_KEY_BITS TPMI_AES_KEY_BITS;

// Table 121 -- TPMI_SMS4_KEY_BITS Type <I/O>
typedef TPM_KEY_BITS TPMI_SMS4_KEY_BITS;

// Table 122 -- TPMU_SYM_KEY_BITS Union <I/O>
typedef union {
    TPMI_AES_KEY_BITS  aes;
    TPMI_SMS4_KEY_BITS  sms4;
    TPM_KEY_BITS  sym;
    TPMI_ALG_HASH  xor;

} TPMU_SYM_KEY_BITS ;

// Table 123 -- TPMU_SYM_MODE Union <I/O>
typedef union {
    TPMI_ALG_SYM_MODE  aes;
    TPMI_ALG_SYM_MODE  sms4;
    TPMI_ALG_SYM_MODE  sym;

} TPMU_SYM_MODE ;

// Table 125 -- TPMT_SYM_DEF Structure <I/O>
typedef struct {
    TPMI_ALG_SYM         algorithm;
    TPMU_SYM_KEY_BITS    keyBits;
    TPMU_SYM_MODE        mode;
} TPMT_SYM_DEF;

// Table 126 -- TPMT_SYM_DEF_OBJECT Structure <I/O>
typedef struct {
    TPMI_ALG_SYM_OBJECT    algorithm;
    TPMU_SYM_KEY_BITS      keyBits;
    TPMU_SYM_MODE          mode;
} TPMT_SYM_DEF_OBJECT;

// Table 127 -- TPM2B_SYM_KEY Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_SYM_KEY_BYTES];
} SYM_KEY_2B;

typedef union {
    SYM_KEY_2B    t;
    TPM2B         b;
} TPM2B_SYM_KEY;

// Table 128 -- TPMS_SYMCIPHER_PARMS Structure <I/O>
typedef struct {
    TPMT_SYM_DEF_OBJECT    sym;
} TPMS_SYMCIPHER_PARMS;

// Table 129 -- TPM2B_SENSITIVE_DATA Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_SYM_DATA];
} SENSITIVE_DATA_2B;

typedef union {
    SENSITIVE_DATA_2B    t;
    TPM2B                b;
} TPM2B_SENSITIVE_DATA;

// Table 130 -- TPMS_SENSITIVE_CREATE Structure <I>
typedef struct {
    TPM2B_AUTH              userAuth;
    TPM2B_SENSITIVE_DATA    data;
} TPMS_SENSITIVE_CREATE;

// Table 131 -- TPM2B_SENSITIVE_CREATE Structure <I,S>
typedef struct {
    UINT16                   size;
    TPMS_SENSITIVE_CREATE    sensitive;
} SENSITIVE_CREATE_2B;

typedef union {
    SENSITIVE_CREATE_2B    t;
    TPM2B                  b;
} TPM2B_SENSITIVE_CREATE;

// Table 132 -- TPMS_SCHEME_SIGHASH Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_SIGHASH;

// Table 133 -- TPMI_ALG_KEYEDHASH_SCHEME Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_KEYEDHASH_SCHEME;

// Table 134 -- HMAC_SIG_SCHEME Types <I/O>
typedef TPMS_SCHEME_SIGHASH    TPMS_SCHEME_HMAC;

// Table 135 -- TPMS_SCHEME_XOR Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
    TPMI_ALG_KDF     kdf;
} TPMS_SCHEME_XOR;

// Table 136 -- TPMU_SCHEME_KEYEDHASH Union <I/O,S>
typedef union {
    TPMS_SCHEME_HMAC  hmac;
    TPMS_SCHEME_XOR  xor;

} TPMU_SCHEME_KEYEDHASH ;

// Table 137 -- TPMT_KEYEDHASH_SCHEME Structure <I/O>
typedef struct {
    TPMI_ALG_KEYEDHASH_SCHEME    scheme;
    TPMU_SCHEME_KEYEDHASH        details;
} TPMT_KEYEDHASH_SCHEME;

// Table 138 -- RSA_SIG_SCHEMES Types <I/O>
typedef TPMS_SCHEME_SIGHASH    TPMS_SCHEME_RSASSA;
typedef TPMS_SCHEME_SIGHASH    TPMS_SCHEME_RSAPSS;

// Table 139 -- ECC_SIG_SCHEMES Types <I/O>
typedef TPMS_SCHEME_SIGHASH    TPMS_SCHEME_ECDSA;

// Table 140 -- TPMS_SCHEME_ECDAA Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_ECDAA;

// Table 141 -- TPMS_SCHEME_ECSCHNORR Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
    UINT16           count;
} TPMS_SCHEME_ECSCHNORR;

// Table 142 -- TPMU_SIG_SCHEME Union <I/O,S>
typedef union {
    TPMS_SCHEME_RSASSA  rsassa;
    TPMS_SCHEME_RSAPSS  rsapss;
    TPMS_SCHEME_ECDSA  ecdsa;
    TPMS_SCHEME_ECDAA  ecdaa;
    TPMS_SCHEME_ECSCHNORR  ecSchnorr;
    TPMS_SCHEME_HMAC  hmac;
    TPMS_SCHEME_SIGHASH  any;

} TPMU_SIG_SCHEME ;

// Table 143 -- TPMT_SIG_SCHEME Structure <I/O>
typedef struct {
    TPMI_ALG_SIG_SCHEME    scheme;
    TPMU_SIG_SCHEME        details;
} TPMT_SIG_SCHEME;

// Table 144 -- TPMS_SCHEME_OAEP Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_OAEP;

// Table 145 -- TPMS_SCHEME_ECDH Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_ECDH;

// Table 146 -- TPMS_SCHEME_MGF1 Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_MGF1;

// Table 147 -- TPMS_SCHEME_KDF1_SP800_56a Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_KDF1_SP800_56a;

// Table 148 -- TPMS_SCHEME_KDF2 Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_KDF2;

// Table 149 -- TPMS_SCHEME_KDF1_SP800_108 Structure <I/O>
typedef struct {
    TPMI_ALG_HASH    hashAlg;
} TPMS_SCHEME_KDF1_SP800_108;

// Table 150 -- TPMU_KDF_SCHEME Union <I/O,S>
typedef union {
    TPMS_SCHEME_MGF1  mgf1;
    TPMS_SCHEME_KDF1_SP800_56a  kdf1_SP800_56a;
    TPMS_SCHEME_KDF2  kdf2;
    TPMS_SCHEME_KDF1_SP800_108  kdf1_sp800_108;
} TPMU_KDF_SCHEME ;

// Table 151 -- TPMT_KDF_SCHEME Structure <I/O>
typedef struct {
    TPMI_ALG_KDF       scheme;
    TPMU_KDF_SCHEME    details;
} TPMT_KDF_SCHEME;
typedef TPM_ALG_ID TPMI_ALG_ASYM_SCHEME;

// Table 153 -- TPMU_ASYM_SCHEME Union <I/O>
typedef union {
    TPMS_SCHEME_RSASSA  rsassa;
    TPMS_SCHEME_RSAPSS  rsapss;
    TPMS_SCHEME_OAEP  oaep;
    TPMS_SCHEME_ECDSA  ecdsa;
    TPMS_SCHEME_ECDAA  ecdaa;
    TPMS_SCHEME_ECSCHNORR  ecSchnorr;
    TPMS_SCHEME_SIGHASH  anySig;

} TPMU_ASYM_SCHEME ;

typedef struct {
    TPMI_ALG_ASYM_SCHEME    scheme;
    TPMU_ASYM_SCHEME        details;
} TPMT_ASYM_SCHEME;

// Table 155 -- TPMI_ALG_RSA_SCHEME Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_RSA_SCHEME;

// Table 156 -- TPMT_RSA_SCHEME Structure <I/O>
typedef struct {
    TPMI_ALG_RSA_SCHEME    scheme;
    TPMU_ASYM_SCHEME       details;
} TPMT_RSA_SCHEME;

// Table 157 -- TPMI_ALG_RSA_DECRYPT Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_RSA_DECRYPT;

// Table 158 -- TPMT_RSA_DECRYPT Structure <I/O>
typedef struct {
    TPMI_ALG_RSA_DECRYPT    scheme;
    TPMU_ASYM_SCHEME        details;
} TPMT_RSA_DECRYPT;

// Table 159 -- TPM2B_PUBLIC_KEY_RSA Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_RSA_KEY_BYTES];
} PUBLIC_KEY_RSA_2B;

typedef union {
    PUBLIC_KEY_RSA_2B    t;
    TPM2B                b;
} TPM2B_PUBLIC_KEY_RSA;

// Table 160 -- TPMI_RSA_KEY_BITS Type <I/O>
typedef TPM_KEY_BITS TPMI_RSA_KEY_BITS;

// Table 161 -- TPM2B_PRIVATE_KEY_RSA Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_RSA_KEY_BYTES/2];
} PRIVATE_KEY_RSA_2B;

typedef union {
    PRIVATE_KEY_RSA_2B    t;
    TPM2B                 b;
} TPM2B_PRIVATE_KEY_RSA;

// Table 162 -- TPM2B_ECC_PARAMETER Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      value[MAX_ECC_KEY_BYTES];
} ECC_PARAMETER_2B;

typedef union {
    ECC_PARAMETER_2B    t;
    TPM2B               b;
} TPM2B_ECC_PARAMETER;

// Table 163 -- TPMS_ECC_POINT Structure <I/O>
typedef struct {
    TPM2B_ECC_PARAMETER    pointX;
    TPM2B_ECC_PARAMETER    pointY;
} TPMS_ECC_POINT;

// Table 164 -- TPM2B_ECC_POINT Structure <I/O>
typedef struct {
    UINT16            size;
    TPMS_ECC_POINT    point;
} ECC_POINT_2B;

typedef union {
    ECC_POINT_2B    t;
    TPM2B           b;
} TPM2B_ECC_POINT;

// Table 165 -- TPMI_ALG_ECC_SCHEME Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_ECC_SCHEME;

// Table 166 -- TPMI_ECC_CURVE Type <I/O>
typedef TPM_ECC_CURVE TPMI_ECC_CURVE;

// Table 167 -- TPMT_ECC_SCHEME Structure <I/O>
typedef struct {
    TPMI_ALG_ECC_SCHEME    scheme;
    TPMU_SIG_SCHEME        details;
} TPMT_ECC_SCHEME;

// Table 168 -- TPMS_ALGORITHM_DETAIL_ECC Structure <O,S>
typedef struct {
    TPM_ECC_CURVE          curveID;
    UINT16                 keySize;
    TPMT_KDF_SCHEME        kdf;
    TPMT_ECC_SCHEME        sign;
    TPM2B_ECC_PARAMETER    p;
    TPM2B_ECC_PARAMETER    a;
    TPM2B_ECC_PARAMETER    b;
    TPM2B_ECC_PARAMETER    gX;
    TPM2B_ECC_PARAMETER    gY;
    TPM2B_ECC_PARAMETER    n;
    TPM2B_ECC_PARAMETER    h;
} TPMS_ALGORITHM_DETAIL_ECC;

// Table 169 -- TPMS_SIGNATURE_RSASSA Structure <I/O>
typedef struct {
    TPMI_ALG_HASH           hash;
    TPM2B_PUBLIC_KEY_RSA    sig;
} TPMS_SIGNATURE_RSASSA;

// Table 170 -- TPMS_SIGNATURE_RSAPSS Structure <I/O>
typedef struct {
    TPMI_ALG_HASH           hash;
    TPM2B_PUBLIC_KEY_RSA    sig;
} TPMS_SIGNATURE_RSAPSS;

// Table 171 -- TPMS_SIGNATURE_ECDSA Structure <I/O>
typedef struct {
    TPMI_ALG_HASH          hash;
    TPM2B_ECC_PARAMETER    signatureR;
    TPM2B_ECC_PARAMETER    signatureS;
} TPMS_SIGNATURE_ECDSA;

// Table 172 -- TPMU_SIGNATURE Union <I/O,S>
typedef union {
    TPMS_SIGNATURE_RSASSA  rsassa;
    TPMS_SIGNATURE_RSAPSS  rsapss;
    TPMS_SIGNATURE_ECDSA  ecdsa;
    TPMT_HA  hmac;
} TPMU_SIGNATURE ;

// Table 173 -- TPMT_SIGNATURE Structure <I/O>
typedef struct {
    TPMI_ALG_SIG_SCHEME    sigAlg;
    TPMU_SIGNATURE         signature;
} TPMT_SIGNATURE;
typedef union {
    BYTE  ecc[sizeof(TPMS_ECC_POINT)];
    BYTE  rsa[MAX_RSA_KEY_BYTES];
    BYTE  symmetric[sizeof(TPM2B_DIGEST)];
    BYTE  keyedHash[sizeof(TPM2B_DIGEST)];
} TPMU_ENCRYPTED_SECRET ;

// Table 175 -- TPM2B_ENCRYPTED_SECRET Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      secret[sizeof(TPMU_ENCRYPTED_SECRET)];
} ENCRYPTED_SECRET_2B;

typedef union {
    ENCRYPTED_SECRET_2B    t;
    TPM2B                  b;
} TPM2B_ENCRYPTED_SECRET;

// Table 176 -- TPMI_ALG_PUBLIC Type <I/O>
typedef TPM_ALG_ID TPMI_ALG_PUBLIC;

// Table 177 -- TPMU_PUBLIC_ID Union <I/O,S>
typedef union {
    TPM2B_DIGEST  keyedHash;
    TPM2B_DIGEST  sym;
    TPM2B_PUBLIC_KEY_RSA  rsa;
    TPMS_ECC_POINT  ecc;

} TPMU_PUBLIC_ID ;

// Table 178 -- TPMS_KEYEDHASH_PARMS Structure <I/O>
typedef struct {
    TPMT_KEYEDHASH_SCHEME    scheme;
} TPMS_KEYEDHASH_PARMS;
typedef struct {
    TPMT_SYM_DEF_OBJECT    symmetric;
    TPMT_ASYM_SCHEME       scheme;
} TPMS_ASYM_PARMS;

// Table 180 -- TPMS_RSA_PARMS Structure <I/O>
typedef struct {
    TPMT_SYM_DEF_OBJECT    symmetric;
    TPMT_RSA_SCHEME        scheme;
    TPMI_RSA_KEY_BITS      keyBits;
    UINT32                 exponent;
} TPMS_RSA_PARMS;

// Table 181 -- TPMS_ECC_PARMS Structure <I/O>
typedef struct {
    TPMT_SYM_DEF_OBJECT    symmetric;
    TPMT_ECC_SCHEME        scheme;
    TPMI_ECC_CURVE         curveID;
    TPMT_KDF_SCHEME        kdf;
} TPMS_ECC_PARMS;

// Table 182 -- TPMU_PUBLIC_PARMS Union <I/O,S>
typedef union {
    TPMS_KEYEDHASH_PARMS  keyedHashDetail;
    TPMT_SYM_DEF_OBJECT  symDetail;
    TPMS_RSA_PARMS  rsaDetail;
    TPMS_ECC_PARMS  eccDetail;
    TPMS_ASYM_PARMS  asymDetail;

} TPMU_PUBLIC_PARMS ;

// Table 183 -- TPMT_PUBLIC_PARMS Structure <I/O>
typedef struct {
    TPMI_ALG_PUBLIC      type;
    TPMU_PUBLIC_PARMS    parameters;
} TPMT_PUBLIC_PARMS;

// Table 184 -- TPMT_PUBLIC Structure <I/O>
typedef struct {
    TPMI_ALG_PUBLIC      type;
    TPMI_ALG_HASH        nameAlg;
    TPMA_OBJECT          objectAttributes;
    TPM2B_DIGEST         authPolicy;
    TPMU_PUBLIC_PARMS    parameters;
    TPMU_PUBLIC_ID       unique;
} TPMT_PUBLIC;

// Table 185 -- TPM2B_PUBLIC Structure <I/O>
typedef struct {
    UINT16         size;
    TPMT_PUBLIC    publicArea;
} PUBLIC_2B;

typedef union {
    PUBLIC_2B    t;
    TPM2B        b;
} TPM2B_PUBLIC;

// Table 186 -- TPMU_SENSITIVE_COMPOSITE Union <I/O,S>
typedef union {
    TPM2B_PRIVATE_KEY_RSA  rsa;
    TPM2B_ECC_PARAMETER  ecc;
    TPM2B_SENSITIVE_DATA  bits;
    TPM2B_SYM_KEY  sym;
    TPM2B_SENSITIVE_DATA  any;

} TPMU_SENSITIVE_COMPOSITE ;

// Table 187 -- TPMT_SENSITIVE Structure <I/O>
typedef struct {
    TPMI_ALG_PUBLIC             sensitiveType;
    TPM2B_AUTH                  authValue;
    TPM2B_DIGEST                seedValue;
    TPMU_SENSITIVE_COMPOSITE    sensitive;
} TPMT_SENSITIVE;

// Table 188 -- TPM2B_SENSITIVE Structure <I/O>
typedef struct {
    UINT16            size;
    TPMT_SENSITIVE    sensitiveArea;
} SENSITIVE_2B;

typedef union {
    SENSITIVE_2B    t;
    TPM2B           b;
} TPM2B_SENSITIVE;
typedef struct {
    TPM2B_DIGEST      integrityOuter;
    TPM2B_DIGEST      integrityInner;
    TPMT_SENSITIVE    sensitive;
} _PRIVATE;

// Table 190 -- TPM2B_PRIVATE Structure <I/O,S>
typedef struct {
    UINT16    size;
    BYTE      buffer[sizeof(_PRIVATE)];
} PRIVATE_2B;

typedef union {
    PRIVATE_2B    t;
    TPM2B         b;
} TPM2B_PRIVATE;
typedef struct {
    TPM2B_DIGEST    integrityHMAC;
    TPM2B_DIGEST    encIdentity;
} _ID_OBJECT;

// Table 192 -- TPM2B_ID_OBJECT Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      credential[sizeof(_ID_OBJECT)];
} ID_OBJECT_2B;

typedef union {
    ID_OBJECT_2B    t;
    TPM2B           b;
} TPM2B_ID_OBJECT;
//
// BUGBUG: Comment here to resolve conflict
//
//typedef struct {
//    unsigned int index : 22;
//    unsigned int space : 2;
//    unsigned int RH_NV : 8;
//} TPM_NV_INDEX ;

// Table 195 -- TPMA_NV Bits <I/O>
typedef struct {
    unsigned int TPMA_NV_PPWRITE        : 1;
    unsigned int TPMA_NV_OWNERWRITE     : 1;
    unsigned int TPMA_NV_AUTHWRITE      : 1;
    unsigned int TPMA_NV_POLICYWRITE    : 1;
    unsigned int TPMA_NV_COUNTER        : 1;
    unsigned int TPMA_NV_BITS           : 1;
    unsigned int TPMA_NV_EXTEND         : 1;
    unsigned int reserved8              : 3;
    unsigned int TPMA_NV_POLICY_DELETE  : 1;
    unsigned int TPMA_NV_WRITELOCKED    : 1;
    unsigned int TPMA_NV_WRITEALL       : 1;
    unsigned int TPMA_NV_WRITEDEFINE    : 1;
    unsigned int TPMA_NV_WRITE_STCLEAR  : 1;
    unsigned int TPMA_NV_GLOBALLOCK     : 1;
    unsigned int TPMA_NV_PPREAD         : 1;
    unsigned int TPMA_NV_OWNERREAD      : 1;
    unsigned int TPMA_NV_AUTHREAD       : 1;
    unsigned int TPMA_NV_POLICYREAD     : 1;
    unsigned int reserved19             : 5;
    unsigned int TPMA_NV_NO_DA          : 1;
    unsigned int TPMA_NV_ORDERLY        : 1;
    unsigned int TPMA_NV_CLEAR_STCLEAR  : 1;
    unsigned int TPMA_NV_READLOCKED     : 1;
    unsigned int TPMA_NV_WRITTEN        : 1;
    unsigned int TPMA_NV_PLATFORMCREATE : 1;
    unsigned int TPMA_NV_READ_STCLEAR   : 1;
} TPMA_NV ;

// Table 196 -- TPMS_NV_PUBLIC Structure <I/O>
typedef struct {
    TPMI_RH_NV_INDEX    nvIndex;
    TPMI_ALG_HASH       nameAlg;
    TPMA_NV             attributes;
    TPM2B_DIGEST        authPolicy;
    UINT16              dataSize;
} TPMS_NV_PUBLIC;

// Table 197 -- TPM2B_NV_PUBLIC Structure <I/O>
typedef struct {
    UINT16            size;
    TPMS_NV_PUBLIC    nvPublic;
} NV_PUBLIC_2B;

typedef union {
    NV_PUBLIC_2B    t;
    TPM2B           b;
} TPM2B_NV_PUBLIC;

// Table 198 -- TPM2B_CONTEXT_SENSITIVE Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[MAX_CONTEXT_SIZE];
} CONTEXT_SENSITIVE_2B;

typedef union {
    CONTEXT_SENSITIVE_2B    t;
    TPM2B                   b;
} TPM2B_CONTEXT_SENSITIVE;

// Table 199 -- TPMS_CONTEXT_DATA Structure <I/O,S>
typedef struct {
    TPM2B_DIGEST               integrity;
    TPM2B_CONTEXT_SENSITIVE    encrypted;
} TPMS_CONTEXT_DATA;

// Table 200 -- TPM2B_CONTEXT_DATA Structure <I/O>
typedef struct {
    UINT16    size;
    BYTE      buffer[sizeof(TPMS_CONTEXT_DATA)];
} CONTEXT_DATA_2B;

typedef union {
    CONTEXT_DATA_2B    t;
    TPM2B              b;
} TPM2B_CONTEXT_DATA;

// Table 201 -- TPMS_CONTEXT Structure <I/O>
typedef struct {
    UINT64                sequence;
    TPMI_DH_CONTEXT       savedHandle;
    TPMI_RH_HIERARCHY     hierarchy;
    TPM2B_CONTEXT_DATA    contextBlob;
} TPMS_CONTEXT;

// Table 203 -- TPMS_CREATION_DATA Structure <O,S>
typedef struct {
    TPML_PCR_SELECTION    pcrSelect;
    TPM2B_DIGEST          pcrDigest;
    TPMA_LOCALITY         locality;
    TPM_ALG_ID            parentNameAlg;
    TPM2B_NAME            parentName;
    TPM2B_NAME            parentQualifiedName;
    TPM2B_DATA            outsideInfo;
} TPMS_CREATION_DATA;

// Table 204 -- TPM2B_CREATION_DATA Structure <O,S>
typedef struct {
    UINT16                size;
    TPMS_CREATION_DATA    creationData;
} CREATION_DATA_2B;

typedef union {
    CREATION_DATA_2B    t;
    TPM2B               b;
} TPM2B_CREATION_DATA;

//
// Command Header
//
typedef struct {
  TPM_ST tag;
  UINT32 paramSize;
  TPM_CC commandCode;
} TPM2_COMMAND_HEADER;

typedef struct {
  TPM_ST tag;
  UINT32 paramSize;
  TPM_RC responseCode;
} TPM2_RESPONSE_HEADER;

#pragma pack (pop)

#endif
