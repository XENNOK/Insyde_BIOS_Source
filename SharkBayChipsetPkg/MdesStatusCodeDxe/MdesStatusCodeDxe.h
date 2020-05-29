/** @file

  Header file to provides an interface to call function to send HECI message.

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

#ifndef _MDES_STATUS_CODE_DXE_H_
#define _MDES_STATUS_CODE_DXE_H_

#define MDES_STATUS_CODE_PROTOCOL_GUID \
  { 0xE5D0875A, 0xF647, 0x4E16, { 0xBE, 0x4D, 0x95, 0x02, 0x40, 0x29, 0xCC, 0x44 }}

/**
  This function is called in case of status code appears.
  Provides an interface to call function to send HECI message.

  @param[in] Type                 Indicates the type of status code being reported.
  @param[in] Value                Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @retval EFI_STATUS              HECI sent with success.
**/
typedef
EFI_STATUS
(EFIAPI *SEND_STATUS_CODE) (
  IN EFI_STATUS_CODE_TYPE         Type,
  IN EFI_STATUS_CODE_VALUE        Value,
  IN UINT32                       Instance,
  IN EFI_GUID                     *CallerId OPTIONAL,
  IN EFI_STATUS_CODE_DATA         *Data OPTIONAL
  );

typedef struct _MDES_STATUS_CODE_PROTOCOL {
  SEND_STATUS_CODE                SendMdesStatusCode;
} MDES_STATUS_CODE_PROTOCOL;

#endif
