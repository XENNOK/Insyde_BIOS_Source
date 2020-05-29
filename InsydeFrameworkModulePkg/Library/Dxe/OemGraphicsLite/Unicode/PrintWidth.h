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
//;   PrintWidth.h
//;

#ifndef _PRINT_WIDTH_H_
#define _PRINT_WIDTH_H_

typedef CHAR16  CHAR_W;
#define STRING_W(_s)                                  L##_s

#define USPrint(Buffer, BufferSize, Format)           SPrint (Buffer, BufferSize, Format)
#define UvSPrint(Buffer, BufferSize, Format, Marker)  VSPrint (Buffer, BufferSize, Format, Marker)

#include "EfiCommonLib.h"

#endif
