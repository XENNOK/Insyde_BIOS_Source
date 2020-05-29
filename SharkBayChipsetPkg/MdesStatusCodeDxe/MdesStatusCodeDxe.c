/** @file

  Provides an interface to call function to send HECI message.

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


#include "MdesStatusCodeDxe.h"
#include <Library/UefiBootServicesTableLib.h>
#include <MeLib.h>
//#include "MePlatformPolicy.h"


EFI_GUID  gMdesStatusCodeProtocolGuid = MDES_STATUS_CODE_PROTOCOL_GUID;

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
EFI_STATUS
EFIAPI
MdesReportStatusCodeHandler (
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value,
  IN  UINT32                      Instance,
  IN  EFI_GUID                    *CallerId OPTIONAL,
  IN  EFI_STATUS_CODE_DATA        *Data OPTIONAL
  )
{
  EFI_STATUS                      Status;

  Status = HeciSendMdesStatusCode (Type, Value, Instance, CallerId, Data);

  return Status;
}

MDES_STATUS_CODE_PROTOCOL MdesStatusCodeProtocolInstance = {MdesReportStatusCodeHandler};


/**
  Installs MdesStatusCodeProtocolInstance protocol.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_STATUS              Driver instaled with siccess.
**/
EFI_STATUS
EFIAPI
MdesStatusCodeDrvEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  MDES_BIOS_FLAGS                 Flags;
  UINT32                          BiosEventFilters;
  DXE_ME_POLICY_PROTOCOL          *MePlatformPolicy;

  ///
  /// Get the ME platform policy.
  ///
  Status = gBS->LocateProtocol (&gDxePlatformMePolicyGuid, NULL, (VOID **) &MePlatformPolicy);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if(MePlatformPolicy->MeConfig.MdesForBiosState == TRUE) {
    ///
    /// Check if Mdes is enabled in FW
    ///
    Status = HeciGetMdesConfig(&Flags, &BiosEventFilters);
    if (EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }

    if (Flags.fl.MdesEnabled ==  0x1) {
      ///
      /// Install Mdes protocol to be consumed by platform library for ReportStatusCode core driver.
      ///
      Status = gBS->InstallProtocolInterface (
                      &ImageHandle,
                      &gMdesStatusCodeProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &MdesStatusCodeProtocolInstance
                      );
    }
  }
  return Status;
}
