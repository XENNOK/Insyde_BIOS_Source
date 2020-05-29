//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PEI_FREQUENCY_CONTROL_POLICY_PPI_H
#define _PEI_FREQUENCY_CONTROL_POLICY_PPI_H
        
#define PEI_FREQUENCY_CONTROL_POLICY_PPI_GUID  \
  {0x8adb2c5c, 0x3855, 0x4dda, 0x81, 0xa6, 0x50, 0xa3, 0x40, 0x68, 0xb4, 0xaa};


typedef
EFI_STATUS
(EFIAPI *PEI_CPU_FREQUENCY_CONTROL_POLICY) (
  IN   EFI_PEI_SERVICES            **PeiServices,
  IN   UINT64                      FrequencyId,
  OUT UINT8                        *FsbMultiplier
  );

typedef
EFI_STATUS
(EFIAPI *PEI_MEMORY_FREQUENCY_CONTROL_POLICY) (
  IN      EFI_PEI_SERVICES              **PeiServices,
  IN OUT  UINT8                         *MemFreq,
     OUT  BOOLEAN                       *Status
  );
    
typedef struct PEI_FREQUENCY_CONTROL_POLICY_PPI {
  PEI_CPU_FREQUENCY_CONTROL_POLICY       CpuFrequencyPolicy;
  PEI_MEMORY_FREQUENCY_CONTROL_POLICY    MemoryFrequencyPolicy;
} PEI_FREQUENCY_CONTROL_POLICY_PPI;


extern EFI_GUID gPeiFrequencyControlPolicyPpiGuid;

#endif
