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
//;   GifDecoder.c
//;
//; Abstract:
//;
//;   EFI Gif decoder protocol
//;

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (GifDecoder)

EFI_GUID gEfiGifDecoderProtocolGuid = EFI_GIF_DECODER_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiGifDecoderProtocolGuid, "GIF DECODER Protocol", "EFI GIF DECODER Protocol");

