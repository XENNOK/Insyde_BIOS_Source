//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   Ldm.c
//;
//; Abstract:
//;
//;   The definition of LDM partition on dynamic disk
//;

#include "EfiSpec.h"
#include EFI_GUID_DEFINITION (Ldm)

EFI_GUID  gEfiPartTypeLdmMetadataPartGuid = EFI_PART_TYPE_LDM_METADATA_PART_GUID;

EFI_GUID_STRING(&gEfiPartTypeLdmMetadataPartGuid, "ELMP", "EFI LDM Metadata Partition GUID");

EFI_GUID  gEfiPartTypeLdmDataPartGuid = EFI_PART_TYPE_LDM_DATA_PART_GUID;

EFI_GUID_STRING(&gEfiPartTypeLdmDataPartGuid, "ELDP", "EFI LDM Data Partition GUID");

