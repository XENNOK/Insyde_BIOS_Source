/** @file
  IRSI feature GUID and structure definitions

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IRSI_FEATURE_GUID_H_
#define _IRSI_FEATURE_GUID_H_

#include <Uefi.h>

//
// Insyde Runtime Software Interface related GUIDs
// IRSI: Insyde Generic Runtime Services
// ISB:  Insyde Runtime PNP Services
// IRSA: Insyde Runtime WMI Services
//
#define IRSI_FEATURE_GUID \
  { 0x5bce4c83, 0x6a97, 0x444b, 0x63, 0xb4, 0x67, 0x2c, 0x01, 0x47, 0x42, 0xff}
#define IRSI_SERVICES_GUID \
  { 0xFB7DDDD1, 0xB22F, 0x4B55, 0xAE, 0x3E, 0x48, 0xDA, 0x29, 0xF9, 0xB1, 0x8B}
#define ISB_SERVICES_GUID \
  { 0x59894B3E, 0xD150, 0x4530, 0xB0, 0x06, 0x7C, 0xC3, 0x5D, 0x70, 0xB4, 0x87}
#define IRSA_FEATURE_GUID \
  { 0x54DC0985, 0x7972, 0x416F, 0x8C, 0xA1, 0x3E, 0xA0, 0x96, 0xFA, 0xDE, 0x26}

#define IRSI_REGISTRATION_SIGNATURE  SIGNATURE_32 ('I','R','S','I')

//
// Image GUID definitions for IRSI Read/Write Image functions
//
#define IRSI_BIOS_IMAGE_GUID \
  { 0x210CDF71, 0xB6B8, 0x4899, 0x8F, 0xAF, 0x46, 0x24, 0x7A, 0x47, 0x15, 0xDB }
#define IRSI_PEI_IMAGE_GUID \
  { 0xbfdbfc17, 0x7da7, 0x4632, 0x99, 0x67, 0xec, 0x94, 0x43, 0x92, 0xb0, 0x98 }
#define IRSI_DXE_IMAGE_GUID \
  { 0x861b5814, 0xecaa, 0x4e0f, 0x8c, 0x20, 0x6c, 0x97, 0x58, 0xda, 0xea, 0xfb }
#define IRSI_EC_IMAGE_GUID \
  { 0x6ff0985b, 0x9486, 0x4ddd, 0xbe, 0x91, 0x07, 0x5b, 0xbe, 0x26, 0x62, 0x7d }
#define IRSI_VARIABLE_IMAGE_GUID \
  { 0x74ce7b6d, 0xe98f, 0x4b02, 0x9e, 0x54, 0x24, 0x80, 0x24, 0x28, 0x0a, 0xe4 }
#define IRSI_FACTORY_COPY_IMAGE_GUID \
  { 0xec9266f9, 0xd860, 0x4fe7, 0xb2, 0x6e, 0x82, 0x94, 0x3f, 0xe5, 0x26, 0x19 }
#define IRSI_MICROCODE_IMAGE_GUID \
  { 0x9b72be3a, 0xa1c0, 0x48df, 0x91, 0xe4, 0x27, 0xbc, 0x59, 0xd9, 0x81, 0xa7 }
#define IRSI_BVDT_IMAGE_GUID   \
  { 0x96b7051f, 0xda19, 0x4f36, 0x95, 0xd9, 0x59, 0xe2, 0x35, 0xe0, 0xba, 0xd1 }
#define IRSI_DMI_IMAGE_GUID  \
  { 0x679000a3, 0x18ea, 0x4714, 0x9b, 0xd7, 0x25, 0x5f, 0x7c, 0x5f, 0x91, 0xa9 }
#define IRSI_NULL_IMAGE_GUID \
  { 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

#pragma pack(1)
typedef struct {
  UINT32                        FunctionNumber;
  UINT32                        StructureSize;
  UINT32                        ReturnStatus;
  UINT32                        HeaderChecksum;
  UINT8                         Reserved[16];
} IRSI_HEADER;

typedef struct {
  UINT32                        Signature;
  UINT32                        NumberOfTableEntries;
  UINT64                        ConfigurationTable;
} IRSI_INFO;

#pragma pack()

#define IRSI_STATUS(Status)     (UINT32)((Status)&0x7fffffff)

typedef
EFI_STATUS
(EFIAPI *IRSI_FUNCTION) (
  VOID   *CommunicationBuf
  );


extern EFI_GUID gIrsiFeatureGuid;
extern EFI_GUID gIrsiServicesGuid;
extern EFI_GUID gIrsaServicesGuid;
extern EFI_GUID gIsbServicesGuid;

extern EFI_GUID gIrsiBiosImageGuid;
extern EFI_GUID gIrsiDxeImageGuid;
extern EFI_GUID gIrsiPeiImageGuid;
extern EFI_GUID gIrsiEcImageGuid;
extern EFI_GUID gIrsiVariableImageGuid;
extern EFI_GUID gIrsiFactoryCopyImageGuid;
extern EFI_GUID gIrsiMicrocodeImageGuid;
extern EFI_GUID gIrsiBvdtImageGuid;
extern EFI_GUID gIrsiDmiImageGuid;

#endif
