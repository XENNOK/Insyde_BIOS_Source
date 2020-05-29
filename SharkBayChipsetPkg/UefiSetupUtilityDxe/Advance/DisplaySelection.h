/** @file

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

#ifndef _DISPLAY_SELECTION_H_
#define _DISPLAY_SELECTION_H_

#include <SetupUtility.h>
#include <Protocol/PlatformGopPolicy.h>
#include <Protocol/DevicePath.h>
#include <Protocol/ConsoleControl.h>

//[-start-120808-IB10820098-add]//
#define NULL_HANDLE ((VOID *) 0)
#define INVALID_VARSTORE_ID             0
#define LANGUAGE_CODE_ENGLISH    "en-US"
//[-end-120808-IB10820098-add]//
//
// the VOLATILE variable "ActiveDisplayInfo" stores all of the active video controller info to
// provide the using of SCU.
//
#define PLUG_IN_VIDEO_DISPLAY_INFO_VAR_NAME  L"PlugInVgaHandles"

#pragma pack (1)
//
// For the Plug-in device.
//
typedef struct _VIDEO_OUTPUT_PORT_INFO_ {
  ACPI_ADR_DEVICE_PATH      *PortAcpiADR;
  CHAR16                    *PortName;
} VIDEO_OUTPUT_PORT_INFO;

typedef struct _PLUG_IN_ACTIVE_VIDEO_CONTROLLER_NODE_ {
  EFI_HANDLE                       ControllerHandle;
  CHAR16                           *ControllerName;
  UINTN                            NumOfConnectedPort;
  VIDEO_OUTPUT_PORT_INFO           *ConnectedPortInfo;
  ACPI_ADR_DEVICE_PATH             *DefaultAcpiADR;
  ACPI_ADR_DEVICE_PATH             *SelectedAcpiADR;
} PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO;
//
// For the IGD device.
//
typedef struct {
  EFI_STRING_ID       StringId;
  UINT8               SCUDisplayType;
} IGD_SCU_OPTION_INFO;

//[-start-120216-IB09960004-add]//
typedef struct {
  UINT8               PrimaryType;
  UINT8               SecondaryType; 
} IGD_SCU_OPTION_MAP;
//[-end-120216-IB09960004-add]//

//[-start-130401-IB05400394-add]//
typedef struct {
  EFI_STRING_ID       StringToken;
  UINT8               Value;
  UINT8               Flags;
} OPTION_ENTRY;
//[-end-130401-IB05400394-add]//

#pragma pack ()

//[-start-111201-IB07370070-add]//
#define VIDEO_CONFIG_FORM_ID  0x25
//[-end-111201-IB07370070-add]//
//[-start-121207-IB04770253-add]//
#define IGD_FORM_ID  0x250
//[-end-121207-IB04770253-add]//

EFI_STATUS
PlugInVgaUpdateInfo (
  VOID
  );

EFI_STATUS
PlugInVgaDisplaySelectionSave (
  VOID
  );

EFI_STATUS
PlugInVideoDisplaySelectionOption (
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  EFI_IFR_TYPE_VALUE                     *Value
  );

EFI_STATUS
PlugInVideoDisplaySelectionLabel (
  IN  EFI_HII_HANDLE                      HiiHandle
  );

//[-start-120216-IB09960004-remove]//
//EFI_STATUS
//IgdVideoDisplaySelectionOption (
//  IN     EFI_HII_HANDLE                      HiiHandle,  
//  IN OUT CHIPSET_CONFIGURATION                *MyIfrNVData,
//  IN     EFI_QUESTION_ID                     QuestionId,
//  IN     UINT8                               PrimaryDisplaySelected
//  );
//[-end-120216-IB09960004-remove]//

EFI_STATUS
IgdInitialVideoDisplaySelectionLabel (
  IN  EFI_HII_HANDLE                      HiiHandle
  );

//[-start-130401-IB05400394-modify]//
EFI_STATUS
ClearFormDataFromLabel (
  IN     EFI_HII_HANDLE                      HiiHandle,
  IN     EFI_FORM_ID                         FormID,
  IN     UINT16                              Label,
  IN     UINT16                              Label2
  );
//[-end-130401-IB05400394-modify]//

EFI_STATUS
InitialDualVgaControllersLabel (
  IN  EFI_HII_HANDLE                      HiiHandle
  );

//[-start-130401-IB05400394-add]//
UINT8 *
EFIAPI
InternalHiiCreateOpCodeExtended (
  IN VOID   *OpCodeHandle,
  IN VOID   *OpCodeTemplate,
  IN UINT8  OpCode,
  IN UINTN  OpCodeSize,
  IN UINTN  ExtensionSize,
  IN UINT8  Scope
  );
//[-end-130401-IB05400394-add]//
#endif
