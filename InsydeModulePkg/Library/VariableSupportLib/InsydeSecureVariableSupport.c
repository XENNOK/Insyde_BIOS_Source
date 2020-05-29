/** @file
  Common Insyde secure variable support functions

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/VariableSupportLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>

#include <Guid/AdminPassword.h>


/**
  This code uses to initialize secure variable header.
  User can use this function set variable header to set Insyde secure variable.

  @param[in]  DataSize           Input buffer size by byte.
  @param[out] Data               Buffer to save initialized variable header data.

  @retval EFI_SUCCESS            Setting secure variable header successful.
  @retval EFI_INVALID_PARAMETER  Data is NULL or DataSize is too small.
--*/
EFI_STATUS
InitSecureVariableHeader (
  IN   UINTN          DataSize,
  OUT  VOID           *Data
  )
{
  EFI_VARIABLE_AUTHENTICATION   *CertData;

  if (Data == NULL || DataSize < INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + sizeof (UINT8)) {
    return EFI_INVALID_PARAMETER;
  }
  CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;

  CertData->AuthInfo.Hdr.dwLength         = (UINT32) DataSize;
  CertData->AuthInfo.Hdr.wRevision        = 0x0200;
  CertData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  CopyMem (&CertData->AuthInfo.CertType, &gIbvCertTypeInsydeSecureGuid, sizeof (EFI_GUID));

  return EFI_SUCCESS;
}

/**
  Function to get password combined data.

  @param[in]  Data               Buffer to save input data.
  @param[in]  DataLen            Length of input data in bytes.
  @param[out] CombinedData       Output buffer to save combined data.

  @retval EFI_SUCCESS            Get Combined data successfully.
  @retval EFI_INVALID_PARAMETER  Data is NULL or CombinedData is NULL.
--*/
EFI_STATUS
GetCombinedData (
  IN  CONST UINT8      *Data,
  IN        UINTN      DataLen,
  OUT       CHAR8      *CombinedData
  )
{
                                                         //
                                                         // INSYDESECURECOVT
                                                         //
  CHAR8                  ConvertData[CERT_DATA_SIZE] =   {0x73, 0x78, 0x83, 0x89, 0x68, 0x69, 0x83, 0x69,
                                                          0x67, 0x85, 0x82, 0x69, 0x67, 0x79, 0x86, 0x84};
  UINTN                  Index;

  if (Data == NULL || CombinedData == NULL || DataLen == 0 || DataLen > CERT_DATA_SIZE) {
    return EFI_INVALID_PARAMETER;
  }

  for (Index = 0; Index < CERT_DATA_SIZE; Index++) {
    if (Index < DataLen) {
      CombinedData[Index] = (CHAR8) (ConvertData[Index] + Data[Index]);
    } else {
      CombinedData[Index] = (CHAR8) ConvertData[Index];
    }
  }

  return EFI_SUCCESS;
}