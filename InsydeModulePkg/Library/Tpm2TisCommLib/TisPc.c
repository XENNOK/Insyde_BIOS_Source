/** @file
  Basic TIS (TPM Interface Specification) functions.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>

#include <IndustryStandard/Tpm20.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2TisCommLib.h>

/**
  Check whether TPM chip exist.

  @param[in] TisReg  Pointer to TIS register.

  @retval    TRUE    TPM chip exists.
  @retval    FALSE   TPM chip is not found.
**/
BOOLEAN
TisPcPresenceCheck (
  IN      TIS_PC_REGISTERS_PTR      TisReg
  )
{
  UINT8                             RegRead;

  RegRead = MmioRead8 ((UINTN)&TisReg->Access);
  return (BOOLEAN)(RegRead != (UINT8)-1);
}

/**
  Check whether the value of a TPM chip register satisfies the input BIT setting.

  @param[in]  Register     Address port of register to be checked.
  @param[in]  BitSet       Check these data bits are set.
  @param[in]  BitClear     Check these data bits are clear.
  @param[in]  TimeOut      The max wait time (unit MicroSecond) when checking register.

  @retval     EFI_SUCCESS  The register satisfies the check bit.
  @retval     EFI_TIMEOUT  The register can't run into the expected status in time.
**/
EFI_STATUS
EFIAPI
TisPcWaitRegisterBits (
  IN      UINT8                     *Register,
  IN      UINT8                     BitSet,
  IN      UINT8                     BitClear,
  IN      UINT32                    TimeOut
  )
{
  UINT8                             RegRead;
  UINT32                            WaitTime;

  for (WaitTime = 0; WaitTime < TimeOut; WaitTime += 30){
    RegRead = MmioRead8 ((UINTN)Register);
    if ((RegRead & BitSet) == BitSet && (RegRead & BitClear) == 0)
      return EFI_SUCCESS;
    MicroSecondDelay (30);
  }
  return EFI_TIMEOUT;
}

/**
  Get BurstCount by reading the burstCount field of a TIS regiger
  in the time of default TIS_TIMEOUT_D.

  @param[in]  TisReg                Pointer to TIS register.
  @param[out] BurstCount            Pointer to a buffer to store the got BurstConut.

  @retval     EFI_SUCCESS           Get BurstCount.
  @retval     EFI_INVALID_PARAMETER TisReg is NULL or BurstCount is NULL.
  @retval     EFI_TIMEOUT           BurstCount can't be got in time.
**/
EFI_STATUS
EFIAPI
TisPcReadBurstCount (
  IN      TIS_PC_REGISTERS_PTR      TisReg,
     OUT  UINT16                    *BurstCount
  )
{
  UINT32                            WaitTime;
  UINT8                             DataByte0;
  UINT8                             DataByte1;

  if (BurstCount == NULL || TisReg == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  WaitTime = 0;
  do {
    //
    // TIS_PC_REGISTERS_PTR->burstCount is UINT16, but it is not 2bytes aligned,
    // so it needs to use MmioRead8 to read two times
    //
    DataByte0   = MmioRead8 ((UINTN)&TisReg->BurstCount);
    DataByte1   = MmioRead8 ((UINTN)&TisReg->BurstCount + 1);
    *BurstCount = (UINT16)((DataByte1 << 8) + DataByte0);
    if (*BurstCount != 0) {
      return EFI_SUCCESS;
    }
    MicroSecondDelay (30);
    WaitTime += 30;
  } while (WaitTime < TIS_TIMEOUT_D);

  return EFI_TIMEOUT;
}

/**
  Set TPM chip to ready state by sending ready command TIS_PC_STS_READY
  to Status Register in time.

  @param[in] TisReg                Pointer to TIS register.

  @retval    EFI_SUCCESS           TPM chip enters into ready state.
  @retval    EFI_INVALID_PARAMETER TisReg is NULL.
  @retval    EFI_TIMEOUT           TPM chip can't be set to ready state in time.
**/
EFI_STATUS
EFIAPI
TisPcPrepareCommand (
  IN      TIS_PC_REGISTERS_PTR      TisReg
  )
{
  EFI_STATUS                        Status;

  if (TisReg == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MmioWrite8 ((UINTN)&TisReg->Status, TIS_PC_STS_READY);
  Status = TisPcWaitRegisterBits (
             &TisReg->Status,
             TIS_PC_STS_READY,
             0,
             TIS_TIMEOUT_B
             );
  return Status;
}

/**
  Get the control of TPM chip by sending requestUse command TIS_PC_ACC_RQUUSE
  to ACCESS Register in the time of default TIS_TIMEOUT_A.

  @param[in] TisReg                Pointer to TIS register.

  @retval    EFI_SUCCESS           Get the control of TPM chip.
  @retval    EFI_INVALID_PARAMETER TisReg is NULL.
  @retval    EFI_NOT_FOUND         TPM chip doesn't exit.
  @retval    EFI_TIMEOUT           Can't get the TPM control in time.
**/
EFI_STATUS
EFIAPI
TisPcRequestUseTpm (
  IN      TIS_PC_REGISTERS_PTR      TisReg
  )
{
  EFI_STATUS                        Status;

  if (TisReg == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!TisPcPresenceCheck (TisReg)) {
    return EFI_NOT_FOUND;
  }

  MmioWrite8 ((UINTN)&TisReg->Access, TIS_PC_ACC_RQUUSE);
  //
  // No locality set before, ACCESS_X.activeLocality MUST be valid within TIMEOUT_A
  //
  Status = TisPcWaitRegisterBits (
             &TisReg->Access,
             (UINT8)(TIS_PC_ACC_ACTIVE |TIS_PC_VALID),
             0,
             TIS_TIMEOUT_A
             );
  return Status;
}

EFI_STATUS
TisSend (
  IN  TIS_PC_REGISTERS_PTR          TisReg,
  IN  UINT8                         *BufferIn,
  IN  UINT32                        SizeIn
  )
{
  EFI_STATUS                        Status;
  UINT16                            BurstCount;
  UINT32                            Index;

  {
    DEBUG ((EFI_D_INFO, "TisTpmCommand Send - "));
    for (Index = 0; Index < SizeIn; Index++) {
      DEBUG ((EFI_D_INFO, "%02x ", BufferIn[Index]));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  }

  Status = TisPcPrepareCommand (TisReg);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Tpm is not ready for command!\n"));
    return EFI_DEVICE_ERROR;
  }
  //
  // Send the command data to Tpm
  //
  Index = 0;
  while (Index < SizeIn) {
    Status = TisPcReadBurstCount (TisReg, &BurstCount);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
    for (; BurstCount > 0 && Index < SizeIn; BurstCount--) {
      MmioWrite8 ((UINTN)&TisReg->DataFifo, *(BufferIn + Index));
      Index++;
    }
  }
  //
  // Check the Tpm status STS_EXPECT change from 1 to 0
  //
  Status = TisPcWaitRegisterBits (
             &TisReg->Status,
             (UINT8) TIS_PC_VALID,
             TIS_PC_STS_EXPECT,
             TIS_TIMEOUT_C
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "The send buffer too small!\n"));
    Status = EFI_BUFFER_TOO_SMALL;
    goto Exit;
  }
  //
  // Executed the TPM command and waiting for the response data ready
  //
  MmioWrite8 ((UINTN)&TisReg->Status, TIS_PC_STS_GO);

Exit:
  if (EFI_ERROR (Status)) {
    MmioWrite8 ((UINTN)&TisReg->Status, TIS_PC_STS_READY);
  }
  return Status;
}

EFI_STATUS
TisReceive (
  IN     TIS_PC_REGISTERS_PTR       TisReg,
  IN     UINT32                     Timeout,
  IN OUT UINT8                      *BufferOut,
  IN OUT UINT32                     *SizeOut
  )
{
  UINT16                            Data16;
  UINT32                            Data32;
  UINT32                            TpmOutSize;
  EFI_STATUS                        Status;
  UINT32                            Index;
  UINT16                            BurstCount;

  TpmOutSize = 0;
  Status = TisPcWaitRegisterBits (
             &TisReg->Status,
             (UINT8)(TIS_PC_VALID | TIS_PC_STS_DATA),
             0,
             Timeout
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "Wait for Tpm response data time out!!\n"));
    Status = EFI_TIMEOUT;
    goto Exit;
  }
  //
  // Get response data header
  //
  Index = 0;
  BurstCount = 0;
  while (Index < sizeof (TPM2_RESPONSE_HEADER)) {
    Status = TisPcReadBurstCount (TisReg, &BurstCount);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
    for (; BurstCount > 0; BurstCount--) {
      *(BufferOut + Index) = MmioRead8 ((UINTN)&TisReg->DataFifo);
      Index++;
      if (Index == sizeof (TPM2_RESPONSE_HEADER)) break;
    }
  }
  {
    DEBUG ((EFI_D_INFO, "TisTpmCommand ReceiveHeader - "));
    for (Index = 0; Index < sizeof (TPM2_RESPONSE_HEADER); Index++) {
      DEBUG ((EFI_D_INFO, "%02x ", BufferOut[Index]));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  }
  //
  // Check the reponse data header (tag,parasize and returncode )
  //
  CopyMem (&Data16, BufferOut, sizeof (UINT16));
  if (SwapBytes16 (Data16) != TPM_ST_NO_SESSIONS &&
      SwapBytes16 (Data16) != TPM_ST_SESSIONS) {
    Status = EFI_DEVICE_ERROR;
    goto Exit;
  }

  CopyMem (&Data32, (BufferOut + 2), sizeof (UINT32));
  TpmOutSize  = SwapBytes32 (Data32);
  if (*SizeOut < TpmOutSize) {
    Status = EFI_BUFFER_TOO_SMALL;
    goto Exit;
  }
  *SizeOut = TpmOutSize;
  //
  // Continue reading the remaining data
  //
  while ( Index < TpmOutSize ) {
    for (; BurstCount > 0; BurstCount--) {
      *(BufferOut + Index) = MmioRead8 ((UINTN)&TisReg->DataFifo);
      Index++;
      if (Index == TpmOutSize) {
        Status = EFI_SUCCESS;
        goto Exit;
      }
    }
    Status = TisPcReadBurstCount (TisReg, &BurstCount);
    if (EFI_ERROR (Status)) {
      Status = EFI_DEVICE_ERROR;
      goto Exit;
    }
  }

Exit:
  {
    DEBUG ((EFI_D_INFO, "TisTpmCommand Receive - "));
    for (Index = 0; Index < TpmOutSize; Index++) {
      DEBUG ((EFI_D_INFO, "%02x ", BufferOut[Index]));
    }
    DEBUG ((EFI_D_INFO, "\n"));
  }
  if (Status  != EFI_TIMEOUT ||
      Timeout != TIS_SMM_TIMEOUT) {
    MmioWrite8 ((UINTN)&TisReg->Status, TIS_PC_STS_READY);
  }
  return Status;
}

/**
  Send a command to TPM for execution and return response data.

  @param[in]      TisReg        TPM register space base address.
  @param[in]      BufferIn      Buffer for command data.
  @param[in]      SizeIn        Size of command data.
  @param[in, out] BufferOut     Buffer for response data.
  @param[in, out] SizeOut       Size of response data.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TisTpmCommand (
  IN     TIS_PC_REGISTERS_PTR       TisReg,
  IN     UINT32                     Timeout,
  IN     UINT8                      *BufferIn,
  IN     UINT32                     SizeIn,
  IN OUT UINT8                      *BufferOut,
  IN OUT UINT32                     *SizeOut
  )
{
  EFI_STATUS                        Status;

  Status = TisSend (TisReg, BufferIn, SizeIn);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = TisReceive (TisReg, Timeout, BufferOut, SizeOut);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
