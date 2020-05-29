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
//;   Frequency Control PPI defined in EFI 2.0. This code abstracts Frequency Control.
//;


#ifndef _PEI_FREQUENCY_CONTROL_PPI_H_
#define _PEI_FREQUENCY_CONTROL_PPI_H_

#define PEI_FREQUENCY_CONTROL_PPI_GUID  \
  {0xf6b70fe6, 0x7963, 0x4098, 0xa9, 0xa7, 0xd5, 0x58, 0x1e, 0x1c, 0x61, 0xa7}

EFI_FORWARD_DECLARATION (PEI_FREQUENCY_CONTROL_PPI);

//*******************************************************
// PEI_FREQUENCY_CONTROL_PPI_CHECKSUM_READ
//*******************************************************

typedef
BOOLEAN
(EFIAPI *PEI_FREQUENCY_CONTROL_PPI_CHECKSUM_READ) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_FREQUENCY_CONTROL_PPI         *This
  );

//*******************************************************
// PEI_FREQUENCY_CONTROL_PPI_CHECKSUM_WRITE
//*******************************************************

typedef
EFI_STATUS
(EFIAPI *PEI_FREQUENCY_CONTROL_PPI_CHECKSUM_WRITE) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_FREQUENCY_CONTROL_PPI         *This
  );

//*******************************************************
// PEI_FREQUENCY_CONTROL_PPI_POWER_STATUS
//*******************************************************

typedef
BOOLEAN
(EFIAPI *PEI_FREQUENCY_CONTROL_PPI_POWER_STATUS) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_FREQUENCY_CONTROL_PPI         *This
  );

//*******************************************************
// PEI_FREQUENCY_CONTROL_PPI_GET_CPU_INFO
//*******************************************************

typedef
EFI_STATUS
(EFIAPI *PEI_FREQUENCY_CONTROL_PPI_GET_CPU_INFO) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  PEI_FREQUENCY_CONTROL_PPI         *This,
  OUT UINT16                            *Frequency,
  OUT UINT8                             *CurrentRatio,
  OUT UINT16                            *CpuId
  );

//*******************************************************
// PEI_FREQUENCY_CONTROL_PPI
//*******************************************************

typedef
struct _PEI_FREQUENCY_CONTROL_PPI {
	 PEI_FREQUENCY_CONTROL_PPI_CHECKSUM_READ     CheckSumRead;
	 PEI_FREQUENCY_CONTROL_PPI_CHECKSUM_WRITE    CheckSumWrite;
	 PEI_FREQUENCY_CONTROL_PPI_POWER_STATUS      PowerStatus;
	 PEI_FREQUENCY_CONTROL_PPI_GET_CPU_INFO      GetCpuInfo;
} PEI_FREQUENCY_CONTROL_PPI;

extern EFI_GUID                   gPeiFrequencyControlPpiGuid;

#endif
