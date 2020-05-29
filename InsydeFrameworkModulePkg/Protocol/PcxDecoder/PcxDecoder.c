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
//; Abstract:
//;   PCX Decoder protocol
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (PcxDecoder)

EFI_GUID gEfiPcxDecoderProtocolGuid = EFI_PCX_DECODER_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiPcxDecoderProtocolGuid, "PCX DECODER Protocol", "EFI 1.1 PCX DECODER Protocol");

