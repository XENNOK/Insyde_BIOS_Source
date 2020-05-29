/** @file
  Defination GUID of H2O Display Engine Type Guid

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

#ifndef _H2O_DISPLAY_ENGINE_TYPE_H_
#define _H2O_DISPLAY_ENGINE_TYPE_H_

//
// Local Text Display Engine
//
#define H2O_DISPLAY_ENGINE_LOCAL_TEXT_GUID \
  { 0x7c808617, 0x7bc1, 0x4745, 0xa4, 0x58, 0x09, 0x28, 0xf8, 0xb9, 0x5e, 0x60 }

//
// Remote Text Display Engine
//
#define H2O_DISPLAY_ENGINE_REMOTE_TEXT_GUID \
  { 0xa5d0b0b5, 0x99ca, 0x4be4, 0x80, 0x22, 0xce, 0x29, 0x77, 0x45, 0xb6, 0x1a }

//
// Local Graphics Display Engine
//
#define H2O_DISPLAY_ENGINE_LOCAL_GRAPHICS_GUID \
  { 0xc6320a42, 0x6ec0, 0x4f2b, 0xad, 0xf7, 0x56, 0x18, 0x57, 0xe5, 0xc0, 0xab }

//
// Remote Browser Display Engine
//
#define H2O_DISPLAY_ENGINE_REMOTE_BROWSER_GUID \
  { 0x6912a75b, 0x62bd, 0x487f, 0xa0, 0x25, 0x89, 0x0d, 0xb4, 0x31, 0x34, 0x13 }

//
// Local Command-Line Display Engine
//
#define H2O_DISPLAY_ENGINE_LOCAL_COMMAND_LINE_GUID \
  { 0x82a56230, 0x4fb3, 0x4d9f, 0x89, 0x81, 0xc8, 0xb8, 0xb6, 0x11, 0xdb, 0x5a }

//
// Local Metro Display Engine
//
#define H2O_DISPLAY_ENGINE_LOCAL_METRO_GUID \
  { 0xb9e329a2, 0xaba7, 0x4f41, 0x93, 0x98, 0x46, 0xde, 0xc0, 0xae, 0xc1, 0xf7 }

//
// Null Type Display Engine
//
#define H2O_DISPLAY_ENGINE_NULL_TYPE_GUID \
  { 0xeb1c4cbf, 0x2a54, 0x45d2, 0x9d, 0x6b, 0xe9, 0xea, 0xe7, 0x8e, 0x8f, 0xb6 }

//
// All Type Display Engine
//
#define H2O_DISPLAY_ENGINE_ALL_TYPE_GUID \
  { 0x5b3eca28, 0xc2c1, 0x440e, 0x9b, 0xc8, 0xcd, 0x06, 0x95, 0x39, 0xce, 0x06 }

extern EFI_GUID gH2ODisplayEngineLocalTextGuid;
extern EFI_GUID gH2ODisplayEngineRemoteTextGuid;
extern EFI_GUID gH2ODisplayEngineLocalGraphicsGuid;
extern EFI_GUID gH2ODisplayEngineRemoteBrowserGuid;
extern EFI_GUID gH2ODisplayEngineLocalCommandLineGuid;
extern EFI_GUID gH2ODisplayEngineLocalMetroGuid;
extern EFI_GUID gH2ODisplayEngineNullTypeGuid;
extern EFI_GUID gH2ODisplayEngineAllTypeGuid;

#endif
