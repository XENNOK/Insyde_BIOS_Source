/** @file

  Definition of H2O POST Message protocol.
    
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2O_POST_MESSAGE_PROTOCOL_H_
#define _H2O_POST_MESSAGE_PROTOCOL_H_

/**
  Provides an interface that a software module can call to report an PostMessage status code.

  @param[in] CodeType             Indicates the type of status code being reported.
  @param[in] CodeValue            Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_DEVICE_ERROR        The function should not be completed due to a device error.
**/
typedef
EFI_STATUS
(EFIAPI *H2O_POST_MESSAGE_STATUS_CODE) (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL
);

typedef struct _H2O_POST_MESSAGE_PROTOCOL {
  H2O_POST_MESSAGE_STATUS_CODE                        PostMessageStatusCode;
} H2O_POST_MESSAGE_PROTOCOL;

extern EFI_GUID gH2OPostMessageProtocolGuid;
extern EFI_GUID gH2OSmmPostMessageProtocolGuid;
extern EFI_GUID gH2OPostMessageVariableGuid;

#endif

