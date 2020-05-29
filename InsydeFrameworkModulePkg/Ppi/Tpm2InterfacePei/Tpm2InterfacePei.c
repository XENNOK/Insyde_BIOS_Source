//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "Tiano.h"
#include "Pei.h"
#include EFI_PPI_DEFINITION (Tpm2InterfacePei)

EFI_GUID gPeiTpm2InterfacePpiGuid = TPM2_INTERFACE_PPI_GUID;

EFI_GUID_STRING (&gPeiTpm2InterfacePpiGuid, "Tpm2 Interface PPI", "Tpm2 Interface PPI");
