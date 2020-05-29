/** @file

   The definition of BIOS Storage PPI.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _EFI_PPI_BIOS_STORAGE_H_
#define _EFI_PPI_BIOS_STORAGE_H_

#include <Guid/BiosEventLogHob.h>

typedef struct _EFI_PEI_EVENT_LOG_PPI  EFI_PEI_EVENT_LOG_PPI;

typedef enum {  
  EfiEventLogTypeReserved1                          = 0,
  EfiEventLogTypeSingleBitEccMemoryError            = 1,
  EfiEventLogTypeMultiBitEccMemoryError             = 2,
  EfiEventLogTypeParityMemoryError                  = 3,
  EfiEventLogTypeBusTimeOut                         = 4,
  EfiEventLogTypeIoChannelCheck                     = 5,
  EfiEventLogTypeSoftwareNmi                        = 6,
  EfiEventLogTypePostMemoryResize                   = 7,
  EfiEventLogTypePostError                          = 8,
  EfiEventLogTypePciParityError                     = 9,
  EfiEventLogTypePciSystemError                     = 0xA,
  EfiEventLogTypeCpuFailure                         = 0xB,
  EfiEventLogTypeEisaFailSafeTimerTimeOut           = 0xC,
  EfiEventLogTypeCorrectableMemoryLogDisabled       = 0xD,
  EfiEventLogTypeLoggingDisabled                    = 0xE,
  EfiEventLogTypeReserved2                          = 0xF,
  EfiEventLogTypeSystemLimitExceeded                = 0x10,
  EfiEventLogTypeAsynchronousHardwareTimerExpired   = 0x11,
  EfiEventLogTypeSystemConfigurationInformation     = 0x12,
  EfiEventLogTypeHardDiskInformation                = 0x13,
  EfiEventLogTypeSystemReconfigured                 = 0x14,
  EfiEventLogTypeUncorrectableCpuComplexError       = 0x15,
  EfiEventLogTypeLogAreaResetCleared                = 0x16,
  EfiEventLogTypeSystemBoot                         = 0x17,
  EfiEventLogTypeEndOfLog                           = 0xFF
} EFI_MISC_LOG_TYPE;

/**
  Write event to BIOS Storage by creating HOB.

  @param [in] PeiServices          General purpose services available to every PEIM.
  @param [in] This                 This PPI interface.
  @param [in] BiosStorageType      Event log type.
  @param [in] PostBitmap1          Post bitmap 1 which will be stored in data area of POST error type log.
  @param [in] PostBitmap2          Post bitmap 2 which will be stored in data area of POST error type log.

  @retval EFI Status            
**/
typedef
EFI_STATUS
(EFIAPI *EVENT_LOG_PPI_WRITE) (
  IN EFI_PEI_SERVICES                   **PeiServices,
  IN EFI_PEI_EVENT_LOG_PPI              *This,
  IN UINT8                              EventLogType,
  IN BIOS_EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap1,
  IN BIOS_EVENT_LOG_POST_ERROR_BITMAP_FIRST_DWORD PostBitmap2
  )
;

typedef
struct _EFI_PEI_EVENT_LOG_PPI {
  EVENT_LOG_PPI_WRITE     WriteEvent;
} EFI_PEI_EVENT_LOG_PPI;

extern EFI_GUID                   gEfiPeiBiosStorageGuid;

#endif

