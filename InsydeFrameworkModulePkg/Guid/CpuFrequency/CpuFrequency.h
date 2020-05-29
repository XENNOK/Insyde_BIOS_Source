//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   CPU Frequency Status GUID.
//;

#ifndef _PEI_IN_CPU_FREQUENCY_H
#define _PEI_IN_CPU_FREQUENCY_H

#define PEI_IN_CPU_FREQUENCY_GUID \
  {0xa9fb8b70, 0xda20, 0x40f5, 0xb0, 0x0b, 0x76, 0xca, 0x05, 0xe4, 0x5d, 0x38}

#define CPU_FREQUENCY_SET_RIGHT          0xE0
#define CPU_FREQUENCY_SET_ERROR          0xEE

#define CPU_EXTERN_FREQUENCY_DIFFERENT   0xCD

#define CPU_FREQUENCY_BEFORE_VALUE       0xCB

//
//
//
typedef struct {
    UINT16  CpuFrequency;            // CPU Current Use Frequency
    UINT16  OriginalCpuFrequency;    // CPU Extern Frequency
    UINT8   CurrentRatio;            // CPU Cueent Ratio
    UINT8   AbnormalStatus;
    UINT8   CpuDiffFlags;            // When use different CPU that we set this
    UINT8   CpuFrequencyFlags;       // Store Forward Frequency
} CPU_FREQUENCY_STATUS;

extern EFI_GUID gEfiCpuFrequencyGuid;

#endif
