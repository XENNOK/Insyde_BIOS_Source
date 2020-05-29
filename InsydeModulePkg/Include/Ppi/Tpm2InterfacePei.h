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

#ifndef _TPM2_INTERFACE_PEI_H_
#define _TPM2_INTERFACE_PEI_H_

#define TPM2_INTERFACE_PPI_GUID \
  {0x9d910b61, 0x85bd, 0x4473, 0x81, 0x31, 0x40, 0x95, 0x9d, 0x60, 0xc7, 0xee}

typedef struct _PEI_TPM2_INTERFACE_PPI    PEI_TPM2_INTERFACE_PPI;

/**
  Send a command to TPM for execution and return response data.

  @param[in] PeiServices                  Pointer's pointer to EFI_PEI_SERVICES
  @param[in] This                         PPI instance structure.
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
(EFIAPI *PEI_TPM2_SUMMIT_COMMAND) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  );

/**
  Get the control of TPM chip by sending requestUse command TIS_PC_ACC_RQUUSE
  to ACCESS Register in the time of default TIS_TIMEOUT_D.

  @param[in] PeiServices                  Pointer's pointer to EFI_PEI_SERVICES
  @param[in] This                         PPI instance structure.

  @retval    EFI_SUCCESS                  Get the control of TPM chip.
  @retval    EFI_NOT_FOUND                TPM chip doesn't exit.
  @retval    EFI_TIMEOUT                  Can't get the TPM control in time.
**/
typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_REQUEST_USE) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This
  );

/**
  Send a command to TPM for execution.

  @param[in] PeiServices                  Pointer's pointer to EFI_PEI_SERVICES
  @param[in] This                         PPI instance structure.
  @param[in] BufferIn                     Buffer for command data.
  @param[in] SizeIn                       Size of command data.

  @retval    EFI_SUCCESS                  Operation completed successfully.
  @retval    EFI_BUFFER_TOO_SMALL         Response data buffer is too small.
  @retval    EFI_TIMEOUT                  The register can't run into the expected status in time.
**/
typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_SEND) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn
  );

/**
  Get return response data.

  @param[in] PeiServices                  Pointer's pointer to EFI_PEI_SERVICES
  @param[in] This                         PPI instance structure.
  @param[out] BufferOut                   Buffer for response data.
  @param[out] SizeOut                     Size of response data.

  @retval    EFI_SUCCESS                  Operation completed successfully.
  @retval    EFI_BUFFER_TOO_SMALL         Response data buffer is too small.
  @retval    EFI_TIMEOUT                  The register can't run into the expected status in time.
**/
typedef
EFI_STATUS
(EFIAPI *PEI_TPM2_RECEIVE) (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  );


struct _PEI_TPM2_INTERFACE_PPI {
  PEI_TPM2_SUMMIT_COMMAND                 SummitCommand;
  PEI_TPM2_REQUEST_USE                    RequestUse;
  PEI_TPM2_SEND                           Send;
  PEI_TPM2_RECEIVE                        Receive;
};

extern EFI_GUID gPeiTpm2InterfacePpiGuid;
#endif //_TPM2_INTERFACE_PEI_H_