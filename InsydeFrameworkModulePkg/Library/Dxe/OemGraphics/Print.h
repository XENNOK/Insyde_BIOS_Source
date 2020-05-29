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
//;   Print.h
//;

#ifndef _PRINT_H_
#define _PRINT_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "TianoCommon.h"
#include "EfiCommonLib.h"
#include "PrintWidth.h"
#include "EfiPrintLib.h"
#define LEFT_JUSTIFY  0x01
#define PREFIX_SIGN   0x02
#define PREFIX_BLANK  0x04
#define COMMA_TYPE    0x08
#define LONG_TYPE     0x10
#define PREFIX_ZERO   0x20

//
// Largest number of characters that can be printed out.
//
#define EFI_DRIVER_LIB_MAX_PRINT_BUFFER (80 * 4)

#endif
