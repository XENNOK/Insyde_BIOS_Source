//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include <Tiano.h>

#include EFI_PROTOCOL_DEFINITION (Tpm2Interface)

EFI_GUID gTpm2InterfaceGuid = TPM2_INTERFACE_GUID;

EFI_GUID_STRING (&gTpm2InterfaceGuid, "Tpm2 Interface", "Tpm2 Interface");
