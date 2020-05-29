/** @file
  This driver provides IHISI interface in SMM mode

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

#ifndef _IHISI_FBTS_OEM_HOOK_H_
#define _IHISI_FBTS_OEM_HOOK_H_

#include "IhisiSmm.h"

#pragma pack(1)

typedef struct {
   UINT8      SizeIndex;
   UINT32     Size;
} FBTS_FLASH_DEVICE_MAP_TABLE ;

typedef struct {
  UINT32                                Signature;
  UINT8                                 Environment;
  UINT8                                 Version[4];
  UINT8                                 AtpSignature[6];
  UINT8                                 ATpVersion[3];
  UINT8                                 End;
} FBTS_TOOLS_VERSION_BUFFER;

#pragma pack()

extern FBTS_FLASH_DEVICE * FlashDeviceTable[];
extern UINTN               FBTS_OEM_ROM_MAP_COUNT;
extern UINTN               SST_BLOCK_MAP_SIZE;
#define OEM_DEFINE_TDT_TIME_OUT_MIN        180      //Default DTimer time out is set as 3 mins

EFI_STATUS
MsrUncache(
  VOID
  );

/**
  Function to get AC status.

  @return UINT8      AC status.
**/
UINT8
GetACStatus (
  VOID
  );

/**
  Function to get battery life.

  @return UINT8      battery life status.
**/
UINT8
GetBattLife (
  VOID
  );

/**
  According version or OEM condtion to get the flash permission.

  @param[in]  VersionPtr         A point Pointer to FBTS_TOOLS_VERSION_BUFFER.
  @param[out] Permission         A point to save flash permission.

  @retval EFI_SUCEESS            Get Flash permission successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
**/
EFI_STATUS
GetPermission (
  IN    FBTS_TOOLS_VERSION_BUFFER  *VersionPtr,
  OUT   UINT16                     *Permission
  );

/**
  Function to get battery life.

  @retval EFI_SUCCESS      Get OEM flash map successful.
  @retval EFI_UNSUPPORTED  FBTS_OEM_ROM_MAP_COUNT is 0 or module rom map buffer is full.
**/
EFI_STATUS
GetOemFlashMap (
  VOID
  );

/**
  Function to do system shutdown.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
FbtsShutDown (
  VOID
  );


/**
  Function to reboot system.

  @retval EFI_SUCCESS      Successfully returns.
**/
EFI_STATUS
FbtsReboot (
  IN  BOOLEAN                      IssueGlobalReset
  );

/**
  Function to do AP check.

  @return UINT8      AP check status.
**/
UINT8
FbtsApCheck (
  VOID
  );

#endif
