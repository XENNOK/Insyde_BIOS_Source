//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TPM_POLICY_PPI_H
#define _TPM_POLICY_PPI_H
        
#define TPM_POLICY_PPI_GUID \
  { 0x4e5f7052, 0x9db6, 0x4a11, 0x96, 0xf7, 0x31, 0x14, 0x1f, 0x3b, 0x2f, 0xd5 }

typedef struct _PEI_TPM_MEASURES_NEGLECT {
  BOOLEAN                               MeasureDebugMode;               
  BOOLEAN                               MeasureCRTMVersion; 
  BOOLEAN                               MeasureMainBios;
} PEI_TPM_MEASURES_NEGLECT;

typedef struct _TPM_POLICY_PPI {  
  BOOLEAN                               SkipTpmStartup;
  PEI_TPM_MEASURES_NEGLECT              SkipMeasures;
} TPM_POLICY_PPI;

extern EFI_GUID gPeiTpmPolicyPpiGuid;

#endif
