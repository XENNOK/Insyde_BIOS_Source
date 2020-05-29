/** @file

 Ksc settings in PEI phase

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Ppi/CpuIo.h>
#include <Ppi/Stall.h>
#include <Library/PeiKscLib.h>
///
/// Function implemenations
///

/**
 Sends command to Keyboard System Controller.

 @param[in] PeiServices - PEI Services
 @param[in] CpiIo       - Pointer to CPU IO protocol
 @param[in] StallPpi    - Pointer to Stall PPI
 @param[in] Command     - Command byte to send

 @retval EFI_SUCCESS       - Command success
 @retval EFI_DEVICE_ERROR  - Command error
 @retval EFI_TIMEOUT       - Command timeout
**/
EFI_STATUS
SendKscCommand (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI    *CpuIo,
  IN       EFI_PEI_STALL_PPI     *StallPpi,
  IN       UINT8                 Command
  )
{
  UINTN   Index;
  UINT8   KscStatus;

  ///
  /// Verify library has been initialized properly
  ///
  ASSERT (CpuIo != NULL);
  ASSERT (StallPpi != NULL);

  Index = 0;
  KscStatus = 0;
  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (PeiServices, CpuIo, &KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    StallPpi->Stall (PeiServices, StallPpi, (15 * STALL_ONE_MICRO_SECOND));
    ReceiveKscStatus (PeiServices, CpuIo, &KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_TIMEOUT;
  }
  ///
  /// Send the KSC command
  ///
  CpuIo->IoWrite8 (PeiServices, CpuIo, KSC_C_PORT, Command);

  return EFI_SUCCESS;
}

/**
 Receives status from Keyboard System Controller.

 @param[in] PeiServices - PEI Services
 @param[in] CpiIo       - Pointer to CPU IO protocol
 @param[in] KscStatus   - Status byte to receive

 @retval EFI_DEVICE_ERROR  - Ksc library has not initialized yet or KSC not present
 @retval EFI_SUCCESS       - Get KSC status successfully
**/
EFI_STATUS
ReceiveKscStatus (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI    *CpuIo,
  IN       UINT8                 *KscStatus
  )
{
  ///
  /// Verify library has been initialized properly
  ///
  ASSERT (CpuIo != NULL);

  ///
  /// Read and return the status
  ///
  *KscStatus = CpuIo->IoRead8 (PeiServices, CpuIo, KSC_C_PORT);

  return EFI_SUCCESS;
}

/**
 Sends data to Keyboard System Controller.

 @param[in] PeiServices - PEI Services
 @param[in] CpiIo       - Pointer to CPU IO protocol
 @param[in] StallPpi    - Pointer to Stall PPI  
 @param[in] Data        - Data byte to send

 @retval EFI_SUCCESS       - Success
 @retval EFI_DEVICE_ERROR  - Error
 @retval EFI_TIMEOUT       - Command timeout
**/
EFI_STATUS
SendKscData (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI    *CpuIo,
  IN       EFI_PEI_STALL_PPI     *StallPpi,
  IN       UINT8                 Data
  )
{
  UINTN   Index;
  UINT8   KscStatus;

  ///
  /// Verify library has been initialized properly
  ///
  ASSERT (CpuIo != NULL);   
  ASSERT (StallPpi != NULL);

  Index = 0;
  KscStatus = 0;
  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (PeiServices, CpuIo, &KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    StallPpi->Stall (PeiServices, StallPpi, 15);
    ReceiveKscStatus (PeiServices, CpuIo, &KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_TIMEOUT;
  }
  ///
  /// Send the data and return
  ///
  CpuIo->IoWrite8 (PeiServices, CpuIo, KSC_D_PORT, Data);

  return EFI_SUCCESS;
}

/**
 Receives data from Keyboard System Controller.

 @param[in] PeiServices - PEI Services
 @param[in] CpiIo       - Pointer to CPU IO protocol
 @param[in] StallPpi    - Pointer to Stall PPI
 @param[in] Data        - Data byte received

 @retval EFI_SUCCESS       - Read success
 @retval EFI_DEVICE_ERROR  - Read error
 @retval EFI_TIMEOUT       - Command timeout
**/
EFI_STATUS
ReceiveKscData (
  IN CONST EFI_PEI_SERVICES      **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI    *CpuIo,
  IN       EFI_PEI_STALL_PPI     *StallPpi,
  IN       UINT8                 *Data
 )
{
  UINTN         Index;
  UINT8         KscStatus;  

  ///
  /// Verify library has been initialized properly
  ///
  ASSERT (CpuIo != NULL);
  ASSERT (StallPpi != NULL);

  Index = 0;
  KscStatus = 0;
  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (PeiServices, CpuIo, &KscStatus);
  while (((KscStatus & KSC_S_OBF) == 0) && (Index < KSC_TIME_OUT)) {
    StallPpi->Stall (PeiServices, StallPpi, 15);
    ReceiveKscStatus (PeiServices, CpuIo, &KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_TIMEOUT;
  }
  ///
  /// Read KSC data and return
  ///
  *Data = CpuIo->IoRead8 (PeiServices, CpuIo, KSC_D_PORT);

  return EFI_SUCCESS;
}
