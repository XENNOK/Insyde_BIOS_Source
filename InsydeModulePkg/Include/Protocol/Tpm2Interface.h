/** @file

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
#ifndef _TPM2_INTERFACE_H_
#define _TPM2_INTERFACE_H_

#define TPM2_INTERFACE_GUID \
  {0x18d1324c, 0x7c8f, 0x489e, 0x91, 0x8d, 0x3b, 0x7b, 0x78, 0x60, 0x52, 0x33}

typedef struct _TPM2_INTERFACE_PROTOCOL TPM2_INTERFACE_PROTOCOL;

/**
  Send a command to TPM for execution and return response data.

  @param[in] This                         Tpm2 interface protocol.
  @param[in] BufferIn                     Buffer for command data.
  @param[in] SizeIn                       Size of command data.
  @param[in, out] BufferOut               Buffer for response data.
  @param[in, out] SizeOut                 Size of response data.

  @retval    EFI_SUCCESS                  Operation completed successfully.
  @retval    EFI_BUFFER_TOO_SMALL         Response data buffer is too small.
  @retval    EFI_TIMEOUT                  The register can't run into the expected status in time.
**/
typedef
EFI_STATUS
(EFIAPI *TPM2_SUMMIT_COMMAND) (
  IN     TPM2_INTERFACE_PROTOCOL          *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  );

/**
  Get the control of TPM chip by sending requestUse command TIS_PC_ACC_RQUUSE
  to ACCESS Register in the time of default TIS_TIMEOUT_D.

  @param[in] This                         Tpm2 interface protocol.

  @retval    EFI_SUCCESS                  Get the control of TPM chip.
  @retval    EFI_NOT_FOUND                TPM chip doesn't exit.
  @retval    EFI_TIMEOUT                  Can't get the TPM control in time.
**/
typedef
EFI_STATUS
(EFIAPI *TPM2_REQUEST_USE) (
  IN     TPM2_INTERFACE_PROTOCOL          *This
  );

/**
  Send a command to TPM for execution.

  @param[in] This                         Tpm2 interface protocol.
  @param[in] BufferIn                     Buffer for command data.
  @param[in] SizeIn                       Size of command data.

  @retval    EFI_SUCCESS                  Operation completed successfully.
  @retval    EFI_BUFFER_TOO_SMALL         Response data buffer is too small.
  @retval    EFI_TIMEOUT                  The register can't run into the expected status in time.
**/
typedef
EFI_STATUS
(EFIAPI *TPM2_SEND) (
  IN     TPM2_INTERFACE_PROTOCOL          *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn
  );

/**
  Get return response data.

  @param[in]  This                        Tpm2 interface protocol.
  @param[out] BufferOut                   Buffer for response data.
  @param[out] SizeOut                     Size of response data.

  @retval    EFI_SUCCESS                  Operation completed successfully.
  @retval    EFI_BUFFER_TOO_SMALL         Response data buffer is too small.
  @retval    EFI_TIMEOUT                  The register can't run into the expected status in time.
**/
typedef
EFI_STATUS
(EFIAPI *TPM2_RECEIVE) (
  IN     TPM2_INTERFACE_PROTOCOL          *This,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  );

struct _TPM2_INTERFACE_PROTOCOL {
  TPM2_SUMMIT_COMMAND                     SummitCommand;
  TPM2_REQUEST_USE                        RequestUse;
  TPM2_SEND                               Send;
  TPM2_RECEIVE                            Receive;
};

extern EFI_GUID gTpm2InterfaceGuid;
#endif //_TPM2_INTERFACE_H_