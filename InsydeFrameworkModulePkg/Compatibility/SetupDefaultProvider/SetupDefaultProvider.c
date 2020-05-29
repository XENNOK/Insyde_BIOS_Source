/** @file

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

/*++
Module Name:

  SetupDefaultProvider.c

Abstract:

--*/

#include "Tiano.h"
#include "Pei.h"
#include "PeiLib.h"
#include "EfiCommonLib.h"
#include "SetupConfig.h"
#include "UefiSetupUtilityLib.h"
#include "OemServicesSupport.h"
#include "Library/EdkIIGluePeiServicesTablePointerLib.h"

#include EFI_PPI_DEFINITION (OemServices)
#include EFI_PPI_DEPENDENCY (Variable2)
#include EFI_GUID_DEFINITION (PeiDefaultSetupProviderHob)
#include EFI_GUID_DEFINITION (SetupDefaultGuid)
#ifdef USE_FAST_CRISIS_RECOVERY
#include EFI_PPI_DEFINITION (EmuPeiPpi)
#include "FastRecoveryData.h"
#endif

#define VARIABLE_NAME    L"Setup"
#define MAX_VFR_NUMBER    6


VOID
ExtractSetupDefault (
  UINT8 *SetupDefault
  );

EFI_STATUS
EFIAPI
PeiSetupDefaultProviderGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN CONST CHAR16                          *VariableName,
  IN CONST EFI_GUID                        * VendorGuid,
  OUT UINT32                               *Attributes OPTIONAL,
  IN OUT UINTN                             *DataSize,
  OUT VOID                                 *Data
  );

EFI_STATUS
EFIAPI
PeiSetupDefaultProviderGetNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VendorGuid
  );

#pragma pack(push, 1)
typedef struct {
  UINT8  Id[7];
  UINT8  EZH2OVersion;
  UINT32 NumberOfVfrs;
  UINT64 VfrDefaultOffset[MAX_VFR_NUMBER];
} IFR_SETUP_DEFAULT;
#pragma pack(pop)

extern UINT8  BootVfrSystemConfigDefault0000[];
extern UINT8  PowerVfrSystemConfigDefault0000[];
extern UINT8  SecurityVfrSystemConfigDefault0000[];
extern UINT8  AdvanceVfrSystemConfigDefault0000[];
extern UINT8  MainVfrSystemConfigDefault0000[];

//
// IFR default signature for EzH2O application
//
#pragma pack(push, 16)
IFR_SETUP_DEFAULT IfrSetupDefault = {
                    {'$', 'I', 'F', 'R', 'D', 'F', 'T'},
                    0x02,
                    0x05,
                    { (UINT64) (UINTN) AdvanceVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) PowerVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) MainVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) SecurityVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) BootVfrSystemConfigDefault0000,
                      (UINT64) (UINTN) 0
                    }
};
#pragma pack(pop)


EFI_GUID    SetupVariableGuid = SYSTEM_CONFIGURATION_GUID;
CHAR16      *SetupVariableName = VARIABLE_NAME;

typedef struct {
  EFI_PEI_READ_ONLY_VARIABLE2_PPI    *pPeiVariablePpi;
} PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA;

static EFI_PEI_READ_ONLY_VARIABLE2_PPI SetupDefaultProviderVariablePpi = {
  PeiSetupDefaultProviderGetVariable,
  PeiSetupDefaultProviderGetNextVariableName
};

static EFI_PEI_PPI_DESCRIPTOR     SetupDefaultProviderPpiListVariable = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiReadOnlyVariable2PpiGuid,
  &SetupDefaultProviderVariablePpi
};

VOID* PeiFindHobByGuid (
  IN EFI_PEI_SERVICES     **PeiServices,
  IN EFI_GUID             *pGuid
  )
{
  EFI_STATUS              Status; 
  EFI_PEI_HOB_POINTERS    Hob;
  UINT8                   *pTmp;

  Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);

  if (EFI_SUCCESS == Status) {
    while (!END_OF_HOB_LIST (Hob)) {
      if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && CompareGuid (&Hob.Guid->Name, pGuid)) {
        pTmp = (UINT8*)Hob.Raw + sizeof (EFI_HOB_GENERIC_HEADER) + sizeof (EFI_GUID);
        return (VOID*)(pTmp);
      }
      Hob.Raw = GET_NEXT_HOB (Hob);
    }
  }
  return NULL;
}

/**
 Determine the length of null terminated char16 array.

 @param [in]   String           Nullterminated CHAR16 array pointer.

 @retval UINT32                 Number of bytes in the string, including the double NULL at the end;

**/
UINT32
EFIAPI
ArrayLength (
  IN CHAR16 *String
  )
{
  UINT32  Count;

  if (NULL == String) {
    return 0;
  }

  Count = 0;

  while (0 != String[Count]) {
    Count++;
  }

  return (Count * 2) + 2;
}

EFI_STATUS
EFIAPI
PeiSetupDefaultProviderGetNextVariableName (
  IN CONST  EFI_PEI_READ_ONLY_VARIABLE2_PPI *This,
  IN OUT UINTN                              *VariableNameSize,
  IN OUT CHAR16                             *VariableName,
  IN OUT EFI_GUID                           *VendorGuid
  )
{
  EFI_STATUS                                Status;
  PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA   *PeiDefaultSetupProviderPrivateData;

  PeiDefaultSetupProviderPrivateData = (PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA*)PeiFindHobByGuid (GetPeiServicesTablePointer (), &gPeiDefaultSetupProviderGuid);

  Status = PeiDefaultSetupProviderPrivateData->pPeiVariablePpi->GetNextVariableName (This, VariableNameSize, VariableName, VendorGuid);
  
  return Status;
}

EFI_STATUS
EFIAPI
PeiSetupDefaultProviderGetVariable (
  IN CONST EFI_PEI_READ_ONLY_VARIABLE2_PPI  *This,
  IN CONST CHAR16                           *VariableName,
  IN CONST EFI_GUID                         * VendorGuid,
  OUT UINT32                                *Attributes OPTIONAL,
  IN OUT UINTN                              *DataSize,
  OUT VOID                                  *Data
  )
{
  EFI_STATUS                                Status;
  PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA   *PeiDefaultSetupProviderPrivateData;
  UINTN                                     Index;
  SETUP_DEFAULT_HEADER                      *SetupDefaultHeader;
  UINT8                                     *DefaultSystemConfigurationPtr;
  UINT8                                     SetupNvData[sizeof(SETUP_DEFAULT_HEADER) + sizeof(VARIABLE_NAME) + sizeof(SYSTEM_CONFIGURATION)];
  EFI_PEI_SERVICES                          **PeiServices =  GetPeiServicesTablePointer ();

  Index = 0;
  Status = EFI_SUCCESS;
  PeiDefaultSetupProviderPrivateData = (PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA*)PeiFindHobByGuid (PeiServices, &gPeiDefaultSetupProviderGuid);

  Status = PeiDefaultSetupProviderPrivateData->pPeiVariablePpi->GetVariable (This, VariableName, VendorGuid, Attributes, DataSize, Data);
  
  if ((Status == EFI_NOT_FOUND) || (Status == EFI_UNSUPPORTED)) {
    if (EfiCompareGuid ((EFI_GUID*)VendorGuid, &SetupVariableGuid) && (EfiCompareMem ((EFI_GUID*)VariableName, SetupVariableName, ArrayLength ((CHAR16 *)VariableName)) == 0)) {
      if (*DataSize < sizeof (SYSTEM_CONFIGURATION)) {
        *DataSize = sizeof (SYSTEM_CONFIGURATION);
        Status = EFI_BUFFER_TOO_SMALL;
      } else {
        ExtractSetupDefault (SetupNvData);
        SetupDefaultHeader = (SETUP_DEFAULT_HEADER*)SetupNvData;
        DefaultSystemConfigurationPtr = SetupNvData + sizeof (SETUP_DEFAULT_HEADER) + SetupDefaultHeader->VariableNameSize;
        (**PeiServices).CopyMem (Data, (VOID*)(DefaultSystemConfigurationPtr), sizeof (SYSTEM_CONFIGURATION));
        *DataSize = sizeof (SYSTEM_CONFIGURATION);
        if (Attributes != NULL) {
          *Attributes = (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS);
        }
        //
        // Indicate that real Setup variable is invalid
        //
        ((SYSTEM_CONFIGURATION *) Data)->SetupVariableInvalid = 1;
        Status = EFI_SUCCESS;
      }
    }
  }
  
  return Status;
}


/**
 Provide Setup default data in PEI phase.

 @param [in]   FfsHeader
 @param [in]   PeiServices

 @retval EFI_STATUS

**/
EFI_STATUS
EFIAPI
SetupDefaultProviderEntryPoint (
  IN EFI_FFS_FILE_HEADER        *FfsHeader,
  IN EFI_PEI_SERVICES           **PeiServices
  )
{
  EFI_STATUS                                Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI           *VariablePpi;
  EFI_PEI_PPI_DESCRIPTOR                    *VariablePpiDescriptor;
  PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA   *PeiDefaultSetupProviderPrivateData;
  UINTN                                     VariableSize;
  SYSTEM_CONFIGURATION                      SystemConfiguration;
  DEFAULT_SETUP_VARIABLE_CONTENT            *DefaultSetupVariableContent;
  SETUP_DEFAULT_HEADER                      *SetupDefaultHeader;
  UINT8                                     *DefaultSystemConfigurationPtr;
  UINT8                                     SetupNvData[sizeof(SETUP_DEFAULT_HEADER) + sizeof(VARIABLE_NAME) + sizeof(SYSTEM_CONFIGURATION)];
  UINT8                                     BootTypeOrder[] = {BOOT_TYPE_ORDER_SEQUENCE};
  OEM_SERVICES_PPI                          *OemServicesPpi;
  BOOLEAN                                   IsRecovery = FALSE;  
#ifdef USE_FAST_CRISIS_RECOVERY  
  UINT8                                     *EmuPeiPpi;
#endif  
  UINT8 EZH2OVersion;

  //
  // Dummy code to avoid IfrSetupDefault data being removed due to linker optimization
  // Do not remove this line
  //
  EZH2OVersion = IfrSetupDefault.EZH2OVersion;


  Status = (**PeiServices).LocatePpi (
                                      PeiServices,
                                      &gPeiReadOnlyVariable2PpiGuid,
                                      0,
                                      &VariablePpiDescriptor,
                                      &VariablePpi
                                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Detect if in recovery mode
  // 
  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gOemServicesPpiGuid,
                            0,
                            NULL,
                            &OemServicesPpi
                            );

  if (!EFI_ERROR (Status)) {
    Status = OemServicesPpi->Services[COMMON_DETECT_RECOVERY_REQUEST] (        
                               PeiServices,
                               OemServicesPpi,
                               COMMON_DETECT_RECOVERY_REQUEST_ARG_COUNT,
                               &IsRecovery
                               );
  } else {
    return Status;
  }

  if (!EFI_ERROR (Status)){
    return Status;    
  }      

#ifdef USE_FAST_CRISIS_RECOVERY  
  Status = (*PeiServices)->LocatePpi (
                            PeiServices,
                            &gEmuPeiPpiGuid,
                            0,
                            NULL,
                            (VOID **)&EmuPeiPpi
                            ); 

  if (!EFI_ERROR (Status)) {
    IsRecovery = 1;
  }
#endif

  // Set Boot Mode
  if (IsRecovery) {
    Status = (*PeiServices)->SetBootMode (PeiServices,
                                 BOOT_IN_RECOVERY_MODE                    
                                 );
  }
  
  VariableSize = sizeof (SYSTEM_CONFIGURATION);
  
  Status = VariablePpi->GetVariable (
                                  VariablePpi,
                                  SetupVariableName,
                                  &SetupVariableGuid,
                                  NULL,
                                  &VariableSize,
                                  &SystemConfiguration
                                  );

  if (!EFI_ERROR (Status)  && !IsRecovery ) {

    return Status;
    
  }

  Status = (**PeiServices).AllocatePool (PeiServices,
                                         sizeof (PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA),
                                         &PeiDefaultSetupProviderPrivateData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (**PeiServices).AllocatePool (PeiServices,
                                         sizeof (DEFAULT_SETUP_VARIABLE_CONTENT),
                                         &DefaultSetupVariableContent);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ExtractSetupDefault (SetupNvData);
  
  SetupDefaultHeader = (SETUP_DEFAULT_HEADER*)SetupNvData;
  DefaultSystemConfigurationPtr = SetupNvData + sizeof (SETUP_DEFAULT_HEADER) + SetupDefaultHeader->VariableNameSize;
  (**PeiServices).CopyMem (&DefaultSetupVariableContent->SetupNvData, (VOID*)(DefaultSystemConfigurationPtr), sizeof (SYSTEM_CONFIGURATION));
  (**PeiServices).CopyMem (DefaultSetupVariableContent->SetupNvData.BootTypeOrder, BootTypeOrder, sizeof (BootTypeOrder));  
  DefaultSetupVariableContent->SetupNvData.SetupVariableInvalid = 1;
  Status = PeiBuildHobGuidData (PeiServices, &gSetupDefaultHobGuid, DefaultSetupVariableContent, sizeof (DEFAULT_SETUP_VARIABLE_CONTENT));
  
  PeiDefaultSetupProviderPrivateData->pPeiVariablePpi = VariablePpi;
  
  Status = PeiBuildHobGuidData (PeiServices, &gPeiDefaultSetupProviderGuid, PeiDefaultSetupProviderPrivateData, sizeof (PEI_DEFAULT_SETUP_PROVIDER_PRIVATE_DATA));

  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = (*PeiServices)->ReInstallPpi (
                               PeiServices,
                               VariablePpiDescriptor,
                               &SetupDefaultProviderPpiListVariable
                               );
  return Status;
}

