/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _GEN_SETUP_DEFAULT_H_
#define _GEN_SETUP_DEFAULT_H_

#define VARIABLE_NAME     L"Setup"

#if (EFI_SPECIFICATION_VERSION < 0x0002000A)
#define VFR_BIN_EXTERN_DEFINE \
        extern unsigned char AdvanceVfrBin[]; \
        extern unsigned char PowerVfrBin[]; \
        extern unsigned char ExitVfrBin[]; \
        extern unsigned char BootVfrBin[]; \
        extern unsigned char SecurityVfrBin[]; \
        extern unsigned char MainVfrBin[];

#define VFR_BIN_PACKAGE_DEFINE \
    (EFI_HII_IFR_PACK *) ExitVfrBin, \
    (EFI_HII_IFR_PACK *) BootVfrBin, \
    (EFI_HII_IFR_PACK *) PowerVfrBin, \
    (EFI_HII_IFR_PACK *) SecurityVfrBin, \
    (EFI_HII_IFR_PACK *) AdvanceVfrBin, \
    (EFI_HII_IFR_PACK *) MainVfrBin,

#else

#define VFR_BIN_EXTERN_DEFINE \
    extern UINT8 MainVfrSystemConfigDefault0000[]; \
    extern UINT8 AdvanceVfrSystemConfigDefault0000[]; \
    extern UINT8 SecurityVfrSystemConfigDefault0000[]; \
    extern UINT8 PowerVfrSystemConfigDefault0000[]; \
    extern UINT8 BootVfrSystemConfigDefault0000[];

#define VFR_BIN_PACKAGE_DEFINE \
    5, \
    BootVfrSystemConfigDefault0000, \
    PowerVfrSystemConfigDefault0000, \
    SecurityVfrSystemConfigDefault0000, \
    AdvanceVfrSystemConfigDefault0000, \
    MainVfrSystemConfigDefault0000

#endif

#endif
