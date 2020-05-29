//;******************************************************************************
//;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (VariableLock)

EFI_GUID gEdkiiVariableLockProtocolGuid = EDKII_VARIABLE_LOCK_PROTOCOL_GUID;

EFI_GUID_STRING (&gEdkiiVariableLockProtocolGuid, "EDKII Variable Lock Protocol", "EDKII Variable Lock Protocol");

