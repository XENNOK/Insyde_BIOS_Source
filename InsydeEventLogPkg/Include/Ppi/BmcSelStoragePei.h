/** @file

  The definition of BMC SEL Storage PPI.
  
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

#ifndef _EFI_PPI_BMC_SEL_STORAGE_H_
#define _EFI_PPI_BMC_SEL_STORAGE_H_

#include <Ppi/EventLogPei.h>

typedef struct _H2O_PEI_BMC_SEL_STORAGE_PPI H2O_PEI_BMC_SEL_STORAGE_PPI;

/**
  Log event data in PEI phase by Pei BMC SEL Storage PPI.

  @param [in] PeiServices     Efi Pei services.
  @param [in] EventID         Event ID of the data.
  @param [in] DataSize        Size of event data.
  @param [in] *LogData        Event data which will be logged.
    
  @retval EFI Status            
**/  
typedef
EFI_STATUS
(EFIAPI *PEI_BMC_SEL_STORAGE_PPI_LOG) (
  IN  CONST EFI_PEI_SERVICES                  **PeiServices,
  IN  EVENT_TYPE_ID                           EventID,
  IN  UINTN                                   DataSize,
  IN  UINT8                                   *LogData
  );

struct _H2O_PEI_BMC_SEL_STORAGE_PPI {
  PEI_BMC_SEL_STORAGE_PPI_LOG     LogEvent;
};

extern EFI_GUID                   gH2OPeiBmcSelStoragePpiGuid;

#endif

