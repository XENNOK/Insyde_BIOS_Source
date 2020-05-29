//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   InsydeSecureVariableSupport.c
//;
//; Abstract:
//;
//;  Common Insyde secure variable support functions
//;

#include "VariableSupportLib.h"
#include "EfiCommonLib.h"
#include EFI_GUID_DEFINITION (AdminPassword)

EFI_STATUS
InitSecureVariableHeader (
  IN   UINTN          DataSize,
  OUT  VOID           *Data
  )
/*++

Routine Description:

  This code uses to initialize secure variable header.
  User can use this function set variable header to set Insyde secure variable.

Arguments:

  DataSize               - Input buffer size by byte.
  Data                   - Buffer to save initialized variable header data.

Returns:

  EFI_SUCCESS            - Setting secure variable header successful.
  EFI_INVALID_PARAMETER  - Data is NULL or DataSize is too small.

--*/
{
#ifdef SECURE_BOOT_SUPPORT

  EFI_VARIABLE_AUTHENTICATION   *CertData;

  if (Data == NULL || DataSize < INSYDE_VARIABLE_PASSWORD_HEADER_SIZE + sizeof (UINT8)) {
    return EFI_INVALID_PARAMETER;
  }
  CertData = (EFI_VARIABLE_AUTHENTICATION *) Data;

  CertData->AuthInfo.Hdr.dwLength         = (UINT32) DataSize;
  CertData->AuthInfo.Hdr.wRevision        = 0x0200;
  CertData->AuthInfo.Hdr.wCertificateType = WIN_CERT_TYPE_EFI_GUID;
  EfiCommonLibCopyMem (&CertData->AuthInfo.CertType, &gIbvCertTypeInsydeSecureGuid, sizeof (EFI_GUID));

  return EFI_SUCCESS;

#else

  return EFI_UNSUPPORTED;

#endif
}

EFI_STATUS
GetCombinedData (
  IN  UINT8      *Data,
  IN  UINTN      DataLen,
  OUT CHAR8      *CombinedData
  )
/*++

Routine Description:

  Function to get password combined data.

Arguments:

  Data                   - Buffer to save input data.
  DataLen                - Length of input data by bytes.
  CombinedData           - Output buffer to save combined data.

Returns:

  EFI_SUCCESS            - Get Combined data successfully.
  EFI_INVALID_PARAMETER  - Data is NULL, CombinedData is NULL, DataLen is 0 or Datalen is large than CERT_DATA_SIZE.

--*/
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