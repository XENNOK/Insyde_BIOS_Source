/** @file

  The definition of Event Log PPI.

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

#ifndef _EFI_PPI_EVENT_LOG_H_
#define _EFI_PPI_EVENT_LOG_H_

#include <Guid/PeiEventLogHob.h>

typedef struct _H2O_PEI_EVENT_LOG_PPI H2O_PEI_EVENT_LOG_PPI;

typedef struct {
  UINT8                            SensorType;
  UINT8                            SensorNum;
  UINT8                            EventType;
  } EVENT_TYPE_ID;

/**
  Log event data in PEI phase by Pei Event Handler PPI.

  @param [in] PeiServices     Efi Pei services.
  @param [in] EventID         Event ID of the data.
  @param [in] DataSize        Size of event data.
  @param [in] *LogData        Event data which will be logged.
    
  @retval EFI Status            
**/ 
typedef
EFI_STATUS
(EFIAPI *PEI_EVENT_LOG_PPI_WRITE) (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *LogData
  );

struct _H2O_PEI_EVENT_LOG_PPI {
  PEI_EVENT_LOG_PPI_WRITE     WriteEvent;
};

extern EFI_GUID                   gH2OPeiEventLogPpiGuid;

#endif

