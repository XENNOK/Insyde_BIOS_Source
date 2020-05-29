/** @file

  Header file of BMC SEL Storage PEI implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _BMC_SEL_STORAGE_PEI_H_
#define _BMC_SEL_STORAGE_PEI_H_

//
// Statements that include other files.
//
#include <Ppi/H2OIpmiInterfacePpi.h>
#include <Ppi/BmcSelStoragePei.h>

#define MAX_BUFFER_SIZE                    100
#define BIOS_SOFTWARE_ID                   0x01
#define EVENT_REV                          0x04

#define DELETE_SEL_CMD_SUPPORTED           0x08
#define MAX_BMC_SEL_LOGGED_DATA_SIZE       0x03
#define EVENT_LOG_FULL_ADJUST_EVENT_NUM    0x02  
#define EVENT_LOG_DATA_SIZE                0x08

#define ADJUST_STORAGE_EVENT_ID1           0x16
#define ADJUST_STORAGE_EVENT_ID2           0xFF
#define ADJUST_STORAGE_EVENT_ID3           0xFF
#define ADJUST_STORAGE_DATA1               0x30
#define ADJUST_STORAGE_DATA2               0x14
#define ADJUST_STORAGE_DATA3               0x28

EFI_STATUS
EFIAPI
PeiBmcSelStorageInit ( 
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
EFIAPI
LogEvent (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *LogData
  );

EFI_STATUS
EFIAPI
CheckBmcSelEventLogFull (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN OUT BOOLEAN                    *LogFull,
  IN OUT BOOLEAN                    *DelEntrySupport
  );

EFI_STATUS
EFIAPI
GetBmcSelInfo (
  IN CONST EFI_PEI_SERVICES       **PeiServices,
  IN H2O_IPMI_INTERFACE_PPI       *IpmiPpi,
  IN UINT8                        *RecvBuf,
  IN UINT8                        *RecvSize
  );

EFI_STATUS
EFIAPI
ShiftDataOfBmcSel (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
EFIAPI
AddEventAfterArrangeEventStorage (
  IN CONST EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
EFIAPI
LogDataToBmcSel (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *Data
  );

#endif
