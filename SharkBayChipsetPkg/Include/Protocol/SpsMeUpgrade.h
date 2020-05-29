/** @file

 SPS ME Upgrade Support Protocol

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_ME_UPGRADE_PROTOCOL_H_
#define _SPS_ME_UPGRADE_PROTOCOL_H_

///
/// SPS ME Upgrade Support Protocol
///
/// {CA959F40-DB17-4555-8135-DCAB0A6976B7}
#define SPS_ME_UPGRADE_PROTOCOL_GUID \
  { 0xCA959F40, 0xDB17, 0x4555, { 0x81, 0x35, 0xDC, 0xAB, 0x0A, 0x69, 0x76, 0xB7 }}

///
/// SPS ME Upgrade Support Protocol for SMM
///
/// {06E29B6E-B38A-4c80-AECA-BD27C43E2724}
#define SMM_SPS_ME_UPGRADE_PROTOCOL_GUID \
  { 0x06E29B6E, 0xB38A, 0x4C80, { 0xAE, 0xCA, 0xBD, 0x27, 0xC4, 0x3E, 0x27, 0x24 }}

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gSpsDxeMeUpgradeProtocolGuid;
extern EFI_GUID gSpsSmmMeUpgradeProtocolGuid;

typedef struct _SPS_ME_UPGRADE_PROTOCOL SPS_ME_UPGRADE_PROTOCOL;

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
#define SPS_ME_UPGRADE_PROTOCOL_REVISION_1         1

#pragma pack(1)

typedef enum  {
  SPS_ME_PROTECT_ROM_NON_OVERRIDABLE = 0,
  SPS_ME_PROTECT_ROM_OVERRIDABLE,
  SPS_ME_PROTECT_ROM_UMKNOW
} SPS_ME_PROTECT_ROM_MAP_ATTRIBUTES;

typedef struct {
  UINTN                               ProtectBase;
  UINTN                               ProtectLimit;
  SPS_ME_PROTECT_ROM_MAP_ATTRIBUTES   Attribute;
} SPS_ME_PROTECT_ROM_MAP;

typedef enum  {
  SPS_ME_UPGRADE_MODE_DEFAULT            = 0x00,
  SPS_ME_UPGRADE_MODE_ONLY_ENABLE_HMRFPO,
  SPS_ME_UPGRADE_MODE_FULL_UPGRADE,
  SPS_ME_UPGRADE_MODE_MAX
} SPS_ME_UPGRADE_MODE;

typedef enum {
  SPS_ME_UPGRADE_STATUS_DEFAULT  = 0x00,
  SPS_ME_UPGRADE_STATUS_INITIAL,
  SPS_ME_UPGRADE_STATUS_INITIAL_DONE,
  SPS_ME_UPGRADE_STATUS_ENTIRE_ME_REGION,
  SPS_ME_UPGRADE_STATUS_ENTIRE_ME_REGION_DONE,
  SPS_ME_UPGRADE_STATUS_ME_OPERATIONAL_REGION,
  SPS_ME_UPGRADE_STATUS_ME_OPERATIONAL_REGION_DONE,
  SPS_ME_UPGRADE_STATUS_ONLY_ENABLE_HMRFPO,
  SPS_ME_UPGRADE_STATUS_DO_NOTHING,
  SPS_ME_UPGRADE_STATUS_OUT_OF_ME_REGION,
  SPS_ME_UPGRADE_STATUS_TEST,
  SPS_ME_UPGRADE_STATUS_TEST_IN_SPI_ERASE,
  SPS_ME_UPGRADE_STATUS_TEST_IN_SPI_WRITE,

  SPS_ME_UPGRADE_STATUS_HMRFPO_ENABLE_SUCCESS,
  SPS_ME_UPGRADE_STATUS_HMRFPO_ENABLE_FAILURE,

  SPS_ME_UPGRADE_STATUS_WRITE_NEW_FTPR_TO_OPR1,
  SPS_ME_UPGRADE_STATUS_WRITE_NEW_FTPR_TO_OPR2,
  SPS_ME_UPGRADE_STATUS_WRITE_NEW_FTPR_TO_OPR1_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_NEW_FTPR_TO_OPR2_DONE,

  SPS_ME_UPGRADE_STATUS_ERASE_FACTORY_DATA,
  SPS_ME_UPGRADE_STATUS_ERASE_FPT,
  SPS_ME_UPGRADE_STATUS_ERASE_MFSB,
  SPS_ME_UPGRADE_STATUS_ERASE_SCA,
  SPS_ME_UPGRADE_STATUS_ERASE_FTPR,
  SPS_ME_UPGRADE_STATUS_ERASE_MFS,
  SPS_ME_UPGRADE_STATUS_ERASE_OPR1,
  SPS_ME_UPGRADE_STATUS_ERASE_OPR2,
  SPS_ME_UPGRADE_STATUS_ERASE_ASAP,

  SPS_ME_UPGRADE_STATUS_ERASE_FACTORY_DATA_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_FPT_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_MFSB_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_SCA_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_FTPR_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_MFS_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_OPR1_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_OPR2_DONE,
  SPS_ME_UPGRADE_STATUS_ERASE_ASAP_DONE,

  SPS_ME_UPGRADE_STATUS_WRITE_FACTORY_DATA,
  SPS_ME_UPGRADE_STATUS_WRITE_FPT,
  SPS_ME_UPGRADE_STATUS_WRITE_MFSB,
  SPS_ME_UPGRADE_STATUS_WRITE_SCA,
  SPS_ME_UPGRADE_STATUS_WRITE_FTPR,
  SPS_ME_UPGRADE_STATUS_WRITE_MFS,
  SPS_ME_UPGRADE_STATUS_WRITE_OPR1,
  SPS_ME_UPGRADE_STATUS_WRITE_OPR2,
  SPS_ME_UPGRADE_STATUS_WRITE_ASAP,

  SPS_ME_UPGRADE_STATUS_WRITE_FACTORY_DATA_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_FPT_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_MFSB_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_SCA_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_FTPR_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_MFS_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_OPR1_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_OPR2_DONE,
  SPS_ME_UPGRADE_STATUS_WRITE_ASAP_DONE,

  SPS_ME_UPGRADE_STATUS_VERIFY_NEW_FTPR,
  SPS_ME_UPGRADE_STATUS_VERIFY_NEW_FTPR_FAIL,
  SPS_ME_UPGRADE_STATUS_VERIFY_NEW_FTPR_DONE,
  SPS_ME_UPGRADE_STATUS_VERIFY_ENTIRE_ME,
  SPS_ME_UPGRADE_STATUS_VERIFY_ENTIRE_ME_FAIL,
  SPS_ME_UPGRADE_STATUS_VERIFY_ENTIRE_ME_DONE,
  SPS_ME_UPGRADE_STATUS_VERIFY_FAIL,
  SPS_ME_UPGRADE_STATUS_VERIFY_SUCCESS,

  SPS_ME_UPGRADE_STATUS_OPR_ME_FULL_UPGRADE_DONE,
  SPS_ME_UPGRADE_STATUS_ENTIRE_ME_FULL_UPGRADE_DONE,
  SPS_ME_UPGRADE_STATUS_MAX
} SPS_ME_UPGRADE_STATUS;

#pragma pack()

/**
  Get SPS ME protect ROM map

  @param[in]      This            Pointer of SPS_ME_UPGRADE_PROTOCOL
  @param[out]     ProtectRomMap   Pointer of protect ROM map output buffer
  @param[in, out] NumberOfProtectRegions  # of protect ROM map ouput buffer as input parameter;
                                          physical # of protect ROM map as output parameter

  @retval EFI_INVALID_PARAMETER   This or NumberOfProtectRegions parameter is NULL pointer
  @retval EFI_BUFFER_TOO_SMALL    Size of protect ROM map buffer less than physical size
  @retval EFI_SUCCESS
**/
typedef
EFI_STATUS
(EFIAPI *SPS_ME_UPGRADE_GET_PROTECT_ROM_MAP) (
  IN     SPS_ME_UPGRADE_PROTOCOL       *This,
  OUT    SPS_ME_PROTECT_ROM_MAP        *ProtectRomMap, OPTIONAL
  IN OUT UINTN                         *NumberOfProtectRegions
  );

/**
  Distinguish does the target linear address within ME region. If true, then perform the
  SPS ME firmware updating procedures.
  This function must be invoked before writing data into ME region of flash device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL
  @param[in] WriteTargetAddr      Target linear address for writing flash device
  @param[in] WriteTargetSize      Target size for writing flash device

  @retval others                  Perform SPS ME firmware updating procedures failed
  @retval EFI_SUCCESS             The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *SPS_ME_UPGRADE_PROGRESS) (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN UINTN                             WriteTargetAddr,
  IN UINTN                             WriteTargetSize
  );

/**
  Perform the SPS ME firmware updating procedures after writing data to ME region of flash
  device.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.

  @retval EFI_INVALID_PARAMETER   This is NULL pointer
  @retval EFI_SUCCESS             Updating procedures successfully.
  @retval others                  Perform SPS ME firmware updating procedures failed
**/
typedef
EFI_STATUS
(EFIAPI *SPS_ME_UPGRADE_COMLETE) (
  IN SPS_ME_UPGRADE_PROTOCOL           *This
  );

/**
  Pass new image.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] NewImageSource       Pointer of New image.
  @param[in] NewImageSize         New image size.

  @retval EFI_INVALID_PARAMETER   This is NULL pointer
  @retval EFI_SUCCESS             Updating procedures successfully.
  @retval others                  Pass SPS ME firmware to driver failed.
**/
typedef
EFI_STATUS
(EFIAPI *SPS_ME_UPGRADE_PASS_IMAGE) (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN VOID                              *NewImageSource,
  IN UINTN                             NewImageSize
  );

/**
  Setting SPS ME Upgrade Mode.

  @param[in] This                 Pointer of SPS_ME_UPGRADE_PROTOCOL.
  @param[in] UpgradeMode          SPS ME Upgrade mode.

  @retval EFI_INVALID_PARAMETER   This is NULL pointer
  @retval others                  Pass SPS ME firmware to driver failed.
**/
typedef
EFI_STATUS
(EFIAPI *SPS_ME_UPGRADE_SET_MODE) (
  IN SPS_ME_UPGRADE_PROTOCOL           *This,
  IN UINTN                             UpgradeMode
  );

///
/// SPS ME FW Upgrade Support Protocol
///
typedef struct _SPS_ME_UPGRADE_PROTOCOL {
  UINT8                                       Revision;
  SPS_ME_UPGRADE_GET_PROTECT_ROM_MAP          GetProtectRomMap;
  SPS_ME_UPGRADE_PROGRESS                     UpgradeProgress;
  SPS_ME_UPGRADE_COMLETE                      UpgradeComplete;
  SPS_ME_UPGRADE_PASS_IMAGE                   PassImage;
  SPS_ME_UPGRADE_SET_MODE                     SetMode;
} SPS_ME_UPGRADE_PROTOCOL;

#endif /// End of _SPS_ME_UPGRADE_PROTOCOL_H_
