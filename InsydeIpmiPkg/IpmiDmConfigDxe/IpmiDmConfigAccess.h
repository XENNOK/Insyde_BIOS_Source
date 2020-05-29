/** @file
 The header file of HII Config Access Protocol implementation of IPMI BMC
 Config in Device Manager driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#ifndef _IPMI_DM_CONFIG_ACCESS_H_
#define _IPMI_DM_CONFIG_ACCESS_H_


#include <PiDxe.h>

#include <Library/DxeOemIpmiDmConfigLib.h>
#include <Library/DxeIpmiDmConfigVfrLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiInterfaceLib.h>
#include <Library/IpmiCommonLib.h>
#include <Library/IpmiHobLib.h>

#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/H2OIpmiInterfaceProtocol.h>
#include <Protocol/H2OIpmiSdrProtocol.h>
#include <Protocol/H2OIpmiUtilityProtocol.h>

#include <Guid/MdeModuleHii.h>


#define IPMI_CONFIG_PRIVATE_DATA_SIGNATURE     SIGNATURE_32 ('I', 'C', 'P', 'D')

#define IPMI_CONFIG_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
      IPMI_DM_CONFIG_PRIVATE_DATA, \
      ConfigAccess, \
      IPMI_CONFIG_PRIVATE_DATA_SIGNATURE \
      )


//
// HII specific Vendor Device Path definition.
//
typedef struct {
  VENDOR_DEVICE_PATH                    VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              End;
} HII_VENDOR_DEVICE_PATH;

typedef struct {
  UINTN                                 Signature;
  EFI_HII_CONFIG_ACCESS_PROTOCOL        ConfigAccess;
  EFI_HII_HANDLE                        HiiHandle;
  EFI_HANDLE                            DriverHandle;
  UINTN                                 IpmiDmConfigDataSize;
  UINT8                                 *IpmiDmConfigData;    ///< IPMI_DM_CONFIG or OEM_IPMI_DM_CONFIG
} IPMI_DM_CONFIG_PRIVATE_DATA;


extern IPMI_DM_CONFIG_PRIVATE_DATA      *mIpmiDmConfigPrivateData;

extern EFI_GUID                         mIpmiDmConfigFormSetGuid;

extern CHAR16                           *mIpmiDmConfigVariableName;
extern EFI_GUID                         mIpmiDmConfigVariableGuid;

//
// Tool generated in DxeIpmiDmConfigVfrLib IFR binary data and String data.
//
extern UINT8                            IpmiDmConfigVfrBin[];
extern CHAR16                           DxeIpmiDmConfigVfrLibStrings[];

extern UINT8                            mBmcStatus;

extern UINT8                            mBmcLanChannel;
extern UINT8                            mBmcPrevLanChannel;
extern UINT16                           mBmcLanChannelBitmap;

extern UINT8                            mBmcTempIpv4IpAddress[];
extern UINT8                            mBmcTempIpv4SubnetMask[];
extern UINT8                            mBmcTempIpv4GatewayAddress[];
extern UINT8                            mBmcTempIpv6IpAddress[];
extern UINT8                            mBmcTempIpv6GatewayAddress[];


/**
 This function allows a caller to extract the current configuration for one
 or more named elements from the target driver.
 Extract mIpmiDmConfigPrivateData->IpmiDmConfig to config when enter config menu.

 @param[in]         This                Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]         Request             A null-terminated Unicode string in <ConfigRequest> format.
 @param[OUT]        Progress            On return, points to a character in the Request string.
                                        Points to the string's null terminator if request was successful.
                                        Points to the most recent '&' before the first failing name/value
                                        pair (or the beginning of the string if the failure is in the
                                        first name/value pair) if the request was not successful.
 @param[OUT]        Results             A null-terminated Unicode string in <ConfigAltResp> format which
                                        has all values filled in for the names in the Request string.
                                        String to be allocated by the called function.

 @retval EFI_SUCCESS                    The Results is filled with the requested values.
 @retval EFI_OUT_OF_RESOURCES           Not enough memory to store the results.
 @retval EFI_INVALID_PARAMETER          Request is NULL, illegal syntax, or unknown name.
 @retval EFI_NOT_FOUND                  Routing data doesn't match any storage in this driver.
*/
EFI_STATUS
EFIAPI
ConfigAccessExtract (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Request,
  OUT      EFI_STRING                       *Progress,
  OUT      EFI_STRING                       *Results
  );


/**
 This function processes the results of changes in configuration.
 Get current config setting and save to mIpmiDmConfigPrivateData->IpmiDmConfig & "IpmiConfigData" variable.

 @param[in]         This                Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]         Configuration       A null-terminated Unicode string in <ConfigResp> format.
 @param[OUT]        Progress            A pointer to a string filled in with the offset of the most
                                        recent '&' before the first failing name/value pair (or the
                                        beginning of the string if the failure is in the first
                                        name/value pair) or the terminating NULL if all was successful.

 @retval EFI_SUCCESS                    The Results is processed successfully.
 @retval EFI_INVALID_PARAMETER          This or Configuration or Progress is NULL.
 @retval EFI_NOT_FOUND                  Routing data doesn't match any storage in this driver.
*/
EFI_STATUS
EFIAPI
ConfigAccessRoute (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Configuration,
  OUT      EFI_STRING                       *Progress
  );


/**
 This function processes the results of changes in configuration.
 And when select the object that have INTERACTIVE flags in VFR file.

 @param[in]         This                Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]         Action              Specifies the type of action taken by the browser.
 @param[in]         QuestionId          A unique value which is sent to the original exporting driver
                                        so that it can identify the type of data to expect.
 @param[in]         Type                The type of value for the question.
 @param[in]         Value               A pointer to the data being sent to the original exporting driver.
 @param[OUT]        ActionRequest       On return, points to the action requested by the callback function.

 @retval EFI_SUCCESS                    The callback successfully handled the action.
 @retval EFI_OUT_OF_RESOURCES           Not enough storage is available to hold the variable and its data.
 @retval EFI_DEVICE_ERROR               The variable could not be saved.
 @retval EFI_UNSUPPORTED                The specified Action is not supported by the callback.
*/
EFI_STATUS
EFIAPI
ConfigAccessCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN       EFI_BROWSER_ACTION               Action,
  IN       EFI_QUESTION_ID                  QuestionId,
  IN       UINT8                            Type,
  IN       EFI_IFR_TYPE_VALUE               *Value,
  OUT      EFI_BROWSER_ACTION_REQUEST       *ActionRequest
  );


/**
 Init config data and get the current configuration from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmInitConfigData (
  IN UINT8                              *IpmiDmConfig
  );


/**
 Init & Update IPMI Device Manager Config HII Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.
 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
IpmiDmInitHiiData (
  IN  UINT8                             *IpmiDmConfig,
  IN  EFI_HII_HANDLE                    HiiHandle
  );


/**
 Extract Default Config Data from VFR Formset Package Bin.

 @param[in, out]    ConfigData          A pointer to the config data buffer.
 @param[in]         VfrBin              The VFR Formset Package Bin of a single VFR Formset.
 @param[in]         ConfigDataSize      Data size in bytes of the config data.
*/
VOID
IpmiDmExtractVfrDefault (
  IN OUT UINT8                          *ConfigData,
  IN     UINT8                          *VfrBin,
  IN     UINTN                          ConfigDataSize
  );


/**
 This function is called by ConfigAccessExtract, do something necessary before extract Config Data.
 Such as handling the Config Data non-referenced by IFR.

 @param[in]         IpmiDmConfig            A pointer to IPMI Device Manager Config Data struct.

 @return EFI_SUCCESS always.
*/
EFI_STATUS
IpmiDmConfigAccessExtract (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 This function is called by ConfigAccessRoute, do something necessary in Config Access Route.
 Such as handling the Config Data non-referenced by IFR and set configuration to BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

 @return EFI_SUCCESS always.
*/
EFI_STATUS
IpmiDmConfigAccessRoute (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 This function is called by ConfigAccessCallback,
 Handling the object that have INTERACTIVE flags in VFR.

 @param[in]         QuestionId              A unique value which is sent to the original exporting driver.
 @param[in]         HiiHandle               EFI_HII_HANDLE.
 @param[in]         IpmiDmConfig            A pointer to IPMI Device Manager Config Data struct.

 @return EFI_SUCCESS always.
*/
EFI_STATUS
IpmiDmConfigAccessCallback (
  IN  EFI_QUESTION_ID                   QuestionId,
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Get some general current configuration from BMC.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmGetFromBmc (
  IN UINT8                              *IpmiDmConfig
  );


/**
 Set some general configuration to BMC according to the IPMI Config Data.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
IpmiDmSetToBmc (
  IN UINT8                              *IpmiDmConfig
  );


/**
 Get SDRs information from BMC and create OpCode in IPMI Device Manager Config.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
IpmiDmUpdateSdrList (
  IN  EFI_HII_HANDLE                    HiiHandle
  );


/**
 Update "IPv4 IP Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv4IpAddrStr (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Update "IPv4 Subnet Mask" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv4SubnetMaskStr (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Update "IPv4 Gateway Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv4GatewayAddrStr (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Update "IPv6 IP Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv6IpAddrStr (
  IN  UINT8                             *IpmiDmConfig
  );


/**
 Update "IPv6 Gateway Address" string in IPMI Device Manager Config.

 @param[in]         IpmiDmConfig        A pointer to IPMI Device Manager Config Data struct.

*/
VOID
UpdateIpv6GatewayAddrStr (
  IN  UINT8                             *IpmiDmConfig
  );


#endif

