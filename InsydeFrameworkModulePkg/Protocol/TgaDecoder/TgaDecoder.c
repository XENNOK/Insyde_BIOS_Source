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
//;   TgaDecoder.c
//;
//; Abstract:
//;
//;   EFI TGA decoder protocol
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (TgaDecoder)

EFI_GUID gEfiTgaDecoderProtocolGuid = EFI_TGA_DECODER_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiTgaDecoderProtocolGuid, "TGA DECODER Protocol", "EFI TGA DECODER Protocol");

