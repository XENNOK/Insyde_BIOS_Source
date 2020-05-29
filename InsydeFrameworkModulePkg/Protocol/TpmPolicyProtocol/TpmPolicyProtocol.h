//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TPM_POLICY_PROTOCOL_H_
#define _TPM_POLICY_PROTOCOL_H_

#define TPM_POLICY_PROTOCOL_GUID  \
  { 0x7c9f8551, 0x7b30, 0x4280, 0x90, 0xd2, 0xc1, 0xc, 0x42, 0x9e, 0x10, 0xfc }

typedef struct _DXE_TPM_MEASURES_NEGLECT {
  BOOLEAN                               MeasureHandoffTables;               
  BOOLEAN                               MeasureWakeEvent; 
  BOOLEAN                               MeasureAllBootVariables;
  BOOLEAN                               MeasureAction;
  BOOLEAN                               MeasureSecureBootVariables;               
  BOOLEAN                               MeasureSeparatorEvent; 
  BOOLEAN                               MeasureOsLoaderAuthority; 
  BOOLEAN                               MeasurePeImage;
  BOOLEAN                               MeasureGptTable;
} DXE_TPM_MEASURES_NEGLECT;


typedef struct _TPM_POLICY_PROTOCOL {
  BOOLEAN                               SkipTpmRevokeTrust;  
  BOOLEAN                               SkipInstallTcgAcpiTable;
  BOOLEAN                               SkipTcgDxeLogEventI;
  DXE_TPM_MEASURES_NEGLECT              SkipMeasures;
} TPM_POLICY_PROTOCOL;

extern EFI_GUID gTpmPolicyProtocolGuid;

#endif
