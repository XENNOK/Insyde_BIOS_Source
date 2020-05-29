/** @file

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


#ifndef _AUTHSERVICE_H_
#define _AUTHSERVICE_H_


#define EFI_CERT_TYPE_RSA2048_SHA256_SIZE 256
#define EFI_CERT_TYPE_RSA2048_SIZE        256
#define EFI_CERT_TYPE_SHA256_SIZE         256


//
// Size of AuthInfo prior to the data payload
//
#define AUTHINFO_SIZE (((UINTN)(((EFI_VARIABLE_AUTHENTICATION *) 0)->AuthInfo.CertData)) + sizeof (EFI_CERT_BLOCK_RSA_2048_SHA256))

#define AUTHINFO2_SIZE(VarAuth2) (((UINTN) &(((EFI_VARIABLE_AUTHENTICATION_2 *) 0)->AuthInfo)) + \
                                  (UINTN) ((EFI_VARIABLE_AUTHENTICATION_2 *) (VarAuth2))->AuthInfo.Hdr.dwLength)
//
// "AuthVarKeyDatabase" variable for the Public Key store.
//
#define AUTHVAR_KEYDB_NAME      L"AuthVarKeyDatabase"
#define AUTHVAR_KEYDB_NAME_SIZE 38

//
// Max size of public key database, restricted by max individal EFI varible size, exclude variable header and name size.
//
#define MAX_KEYDB_SIZE  (0x2000 - sizeof (VARIABLE_HEADER) - AUTHVAR_KEYDB_NAME_SIZE)
#define MAX_KEY_NUM     (MAX_KEYDB_SIZE / EFI_CERT_TYPE_RSA2048_SIZE)

//
// Item number of support signature types.
//
#define SIGSUPPORT_NUM 2


typedef struct {
  EFI_GUID        SignatureType;
  UINT32          SignatureSize;
  BOOLEAN         FixedSigSize;
  BOOLEAN         OnlyForSecurityImageDatabase;
} SIGNATURE_SUPPORT_INFO;


//
// "certdb" variable stores the signer's certificates for non PK/KEK/DB/DBX
// variables with EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set.
//
#define EFI_CERT_DB_NAME        L"certdb"

typedef enum {
  AuthVarTypePk,
  AuthVarTypeKek,
  AuthVarTypePriv
} AUTHVAR_TYPE;

#pragma pack(1)
typedef struct {
  EFI_GUID    VendorGuid;
  UINT32      CertNodeSize;
  UINT32      NameSize;
  UINT32      CertDataSize;
  /// CHAR16  VariableName[NameSize];
  /// UINT8   CertData[CertDataSize];
} AUTH_CERT_DB_DATA;
#pragma pack()


/**
  Convert all of module authenticated service relative pointers to virtual address.
**/
VOID
AuthVariableClassAddressChange (
  VOID
);

/**
  This funciton uses to clear all of secure settings. These variable
  includes PK, KEK, db, dbx.

  @param  Global        Pointer to VARIABLE_GLOBAL instance

  @retval EFI_SUCCESS   Clear secure settings successful

--*/
EFI_STATUS
ClearSecureSettings (
  IN  VARIABLE_GLOBAL  *Global
  );

/**
  Initializes for authenticated varibale service.

  @param[in] Global            Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS           Function successfully executed.
  @retval EFI_OUT_OF_RESOURCES  Fail to allocate enough memory resources.
**/
EFI_STATUS
AutenticatedVariableServiceInitialize (
  IN  VARIABLE_GLOBAL         *Global
  );

/**
  Add public key to database and return its index.

  @param[in]  PubKey             Input pointer to Public Key database.
  @param[in]  Global             Pointer to VARIABLE_GLOBAL instance.
  @param[out] KeyIndex           Pointer to output key index.

  @retval EFI_SUCCESS            Add public key to database successfully
  @retval EFI_OUT_OF_RESOURCES   Database is full.
  @retval EFI_NOT_FOUND          Cannot find AUTHVAR_KEYDB_NAME variable.
  @retval EFI_INVALID_PARAMETER  PubKey is NULL, Global is NULL or KeyIndex is NULL.
**/
EFI_STATUS
AddPubKeyInStore (
  IN  UINT8               *PubKey,
  IN  VARIABLE_GLOBAL     *Global,
  OUT UINT32              *KeyIndex
  );

/**
  Process variable with platform key for verification.

  @param[in] VariableName         Name of Variable to be found.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Data pointer.
  @param[in] DataSize             Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable
  @param[in] IsPk                 Indicate whether it is to process pk.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Variable passed validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation.
                                  check carried out by the firmware.
**/
EFI_STATUS
ProcessVarWithPk (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid,
  IN  VOID                      *Data,
  IN  UINTN                     DataSize,
  IN  VARIABLE_POINTER_TRACK    *Variable,
  IN  UINT32                    Attributes OPTIONAL,
  IN  BOOLEAN                   IsPk,
  IN  VARIABLE_GLOBAL           *Global
  );


/**
  Process variable with key exchange key for verification.

  @param[in] VariableName         Name of Variable to be found.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Data pointer.
  @param[in] DataSize             Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Variable pass validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_SECURITY_VIOLATION  The variable does NOT pass the validation
                                  check carried out by the firmware.
**/
EFI_STATUS
ProcessVarWithKek (
  IN  CHAR16                    *VariableName,
  IN  EFI_GUID                  *VendorGuid,
  IN  VOID                      *Data,
  IN  UINTN                     DataSize,
  IN  VARIABLE_POINTER_TRACK    *Variable,
  IN  UINT32                    Attributes OPTIONAL,
  IN  VARIABLE_GLOBAL           *Global
  );

/**
  Process variable with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS/EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS set

  @param[in] VariableName         Name of Variable to be found.
  @param[in] VendorGuid           Variable vendor GUID.
  @param[in] Data                 Data pointer.
  @param[in] DataSize             Size of Data found. If size is less than the
                                  data, this value contains the required size.
  @param[in] Variable             The variable information which is used to keep track of variable usage.
  @param[in] Attributes           Attribute value of the variable.
  @param[in] Global               Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS             Variable is not write-protected or pass validation successfully.
  @retval EFI_INVALID_PARAMETER   Invalid parameter.
  @retval EFI_WRITE_PROTECTED     Variable is write-protected and needs authentication with
                                  EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS set.
  @retval EFI_SECURITY_VIOLATION  The variable is with EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                  set, but the AuthInfo does NOT pass the validation
                                  check carried out by the firmware.
**/
EFI_STATUS
ProcessVariable (
  IN     CHAR16                             *VariableName,
  IN     EFI_GUID                           *VendorGuid,
  IN     VOID                               *Data,
  IN     UINTN                              DataSize,
  IN     VARIABLE_POINTER_TRACK             *Variable,
  IN     UINT32                             Attributes,
  IN     VARIABLE_GLOBAL                    *Global
  );

/**
  Perform a sanity check on the data to be written to secure boot variables.

  @param[in] VariableName       Name of variable.
  @param[in] VendorGuid         Variable vendor GUID.
  @param[in] Data               Variable data.
  @param[in] DataSize           Size of data. 0 means delete.

  @retval EFI_SUCCESS           The sanity check operation is success.
  @retval EFI_INVALID_PARAMETER Variable data has some data structure problem.
**/
EFI_STATUS
CheckSecureBootVarData (
  IN CONST  CHAR16          *VariableName,
  IN CONST  EFI_GUID        *VendorGuid,
  IN CONST  VOID            *Data,
  IN        UINTN           DataSize
  );

/**
  Merge two buffers which formatted as EFI_SIGNATURE_LIST. Only the new EFI_SIGNATURE_DATA+
  will be appended to the original EFI_SIGNATURE_LIST, duplicate EFI_SIGNATURE_DATA
  will be ignored.

  @param[in]      AppendDataSize       Size of NewData buffer.
  @param[in]      AppendDataBuffer     Pointer to new EFI_SIGNATURE_LIST to be appended.
  @param[in]      TotalDataBufferSize  Total size of data buffer which can be used.
  @param[in, out] UsedDataBufferSize   [in]  Used data size by byte before invoking this function.
                                       [out] Used data size by byte after invoking this function.
  @param[in, out] DataBuffer           [in]  Buffer to save original variable data.
                                       [out] Buffer to save merged variable data.

  @retval EFI_SUCCESS                  Append data to singnature list successful.
  @retval EFI_INVALID_PARAMETER        Any input parameter is invalid.
  @retval EFI_OUT_OF_RESOURCES         Total variable data size is large than maximum buffer size.
**/
EFI_STATUS
AppendSignatureList (
  IN       UINTN               AppendDataSize,
  IN CONST UINT8               *AppendDataBuffer,
  IN       UINTN               TotalDataBufferSize,
  IN OUT   UINTN               *UsedDataBufferSize,
  IN OUT   UINT8               *DataBuffer
  );

/**
  Compare two EFI_TIME data.

  @param[in] FirstTime   A pointer to the first EFI_TIME data.
  @param[in] SecondTime  A pointer to the second EFI_TIME data.

  @retval TRUE           The FirstTime is not later than the SecondTime.
  @retval FALSE          The FirstTime is later than the SecondTime.
**/
BOOLEAN
CompareTimeStamp (
  IN EFI_TIME               *FirstTime,
  IN EFI_TIME               *SecondTime
  );

/**
  This function uses to synchronize authenticated variable data and state between
  runtime and SMM.

  @param[in] Global    Pointer to VARIABLE_GLOBAL instance.

  @retval EFI_SUCCESS  Synchronize authenticated data and state successful.
**/
EFI_STATUS
SyncAuthData (
  IN  VARIABLE_GLOBAL         *Global
  );


/**
  This function uses to find secure boot database default variable.

  @param[in]  VariableName       Name of Variable to be found.
  @param[in]  VendorGuid         Variable vendor GUID.
  @param[out] DataSize           Size of variable data by byte.
  @param[out] Data               Data buffer to save variable data.

  @retval EFI_SUCCESS            Find input secure boot database default variable successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Cannot find secure boot database default variable.
**/
EFI_STATUS
FindSecureDatabaseDefaultVariables (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINTN             *DataSize    OPTIONAL,
  OUT     VOID              **Data       OPTIONAL
  );

/**
  This function uses to get secure boot database default variable.

  @param[in]      VariableName   Name of Variable to be found.
  @param[in]      VendorGuid     Variable vendor GUID.
  @param[out]     Attributes     Attribute value of the variable found.
  @param[in, out] DataSize       Size of Data found. If size is less than the
                                 data, this value contains the required size.
  @param[out]     Data           Data buffer to save variable data.

  @retval EFI_SUCCESS            Get secure boot database default variable successful.
  @retval EFI_INVALID_PARAMETER  Any input parameter is invalid.
  @retval EFI_NOT_FOUND          Cannot find secure boot database default variable.
  @retval EFI_BUFFER_TOO_SMALL   Find Secure boot database default variable but input buffer size
                                 is too small.
**/
EFI_STATUS
GetSecureDatabaseDefaultVariables (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  );

/**
  Update platform boot mode.

  @param[in] Mode      SECURE_BOOT_MODE_ENABLE or SECURE_BOOT_MODE_DISABLE.
  @param[in] Global    Pointer to VARIABLE_GLOBAL instance.
**/
VOID
UpdatePlatformBootMode (
  IN  UINT32              Mode,
  IN  VARIABLE_GLOBAL     *Global
  );



extern CRYPTO_SERVICES_PROTOCOL          *mCryptoService;
extern EFI_HASH_PROTOCOL                 *mHash;
extern VOID                              *mSha256Hash;
#endif
