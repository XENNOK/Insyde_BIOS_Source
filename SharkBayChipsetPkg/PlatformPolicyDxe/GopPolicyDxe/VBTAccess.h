//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   VBTAccess.h
//;

#ifndef _VBT_ACCESS_H_
#define _VBT_ACCESS_H_

#include "VBTRegs.h"

#define VBTAddress( BaseAddr, Register ) \
  ( ( UINTN )( BaseAddr ) + ( UINTN )( Register ) )

#define VBTPtr( GRANULARITY, BaseAddr, Register ) \
  ( ( volatile GRANULARITY * )VBTAddress ( BaseAddr, Register ) )

#define VBT( GRANULARITY, BaseAddr, Register ) \
  ( *VBTPtr ( GRANULARITY, BaseAddr, Register ) )

#define VBTUpdateChecksum( BaseAddr, Register, Data ) \
  ( VBT ( UINT8, BaseAddr, R_VBT_CHECKSUM ) = ( UINT8 )( VBT ( UINT8, BaseAddr, R_VBT_CHECKSUM ) + VBT ( UINT8, BaseAddr, Register ) - ( Data ) ) )

#define VBT8UpdateChecksum( BaseAddr, Register, Data ) \
  { \
    VBTUpdateChecksum ( BaseAddr, ( Register + 0 ), ( UINT8 )( Data >> 0 ) );    \
  }

#define VBT8Read( BaseAddr, Register ) \
  ( VBT ( UINT8, BaseAddr, Register ) )

#define VBT8Write( BaseAddr, Register, Data ) \
  { \
    VBT8UpdateChecksum ( BaseAddr, Register, Data );          \
    VBT ( UINT8, BaseAddr, Register ) = ( UINT8 )( Data );    \
  }

#define VBT8And( BaseAddr, Register, AndData ) \
  VBT8Write (                                        \
    BaseAddr,                                        \
    Register,                                        \
    ( VBT8Read ( BaseAddr, Register ) & AndData )    \
    )

#define VBT8Or( BaseAddr, Register, OrData ) \
  VBT8Write (                                       \
    BaseAddr,                                       \
    Register,                                       \
    ( VBT8Read ( BaseAddr, Register ) | OrData )    \
    )

#define VBT8AndThenOr( BaseAddr, Register, AndData, OrData ) \
  VBT8Write (                                                     \
    BaseAddr,                                                     \
    Register,                                                     \
    ( ( VBT8Read ( BaseAddr, Register ) & AndData ) | OrData )    \
    )

#define VBT16UpdateChecksum( BaseAddr, Register, Data ) \
  { \
    VBTUpdateChecksum ( BaseAddr, ( Register + 0 ), ( UINT8 )( Data >> 0 ) );    \
    VBTUpdateChecksum ( BaseAddr, ( Register + 1 ), ( UINT8 )( Data >> 8 ) );    \
  }

#define VBT16Read( BaseAddr, Register ) \
  ( VBT ( UINT16, BaseAddr, Register ) )

#define VBT16Write( BaseAddr, Register, Data ) \
  { \
    VBT16UpdateChecksum ( BaseAddr, Register, Data );           \
    VBT ( UINT16, BaseAddr, Register ) = ( UINT16 )( Data );    \
  }

#define VBT16And( BaseAddr, Register, AndData ) \
  VBT16Write (                                        \
    BaseAddr,                                         \
    Register,                                         \
    ( VBT16Read ( BaseAddr, Register ) & AndData )    \
    )

#define VBT16Or( BaseAddr, Register, OrData ) \
  VBT16Write (                                       \
    BaseAddr,                                        \
    Register,                                        \
    ( VBT16Read ( BaseAddr, Register ) | OrData )    \
    )

#define VBT16AndThenOr( BaseAddr, Register, AndData, OrData ) \
  VBT16Write (                                                     \
    BaseAddr,                                                      \
    Register,                                                      \
    ( ( VBT16Read ( BaseAddr, Register ) & AndData ) | OrData )    \
    )

#define VBT32UpdateChecksum( BaseAddr, Register, Data ) \
  { \
    VBTUpdateChecksum ( BaseAddr, ( Register + 0 ), ( UINT8 )( Data >>  0 ) );    \
    VBTUpdateChecksum ( BaseAddr, ( Register + 1 ), ( UINT8 )( Data >>  8 ) );    \
    VBTUpdateChecksum ( BaseAddr, ( Register + 2 ), ( UINT8 )( Data >> 16 ) );    \
    VBTUpdateChecksum ( BaseAddr, ( Register + 3 ), ( UINT8 )( Data >> 24 ) );    \
  }

#define VBT32Read( BaseAddr, Register ) \
  ( VBT ( UINT32, BaseAddr, Register ) )

#define VBT32Write( BaseAddr, Register, Data ) \
  { \
    VBT32UpdateChecksum ( BaseAddr, Register, Data );           \
    VBT ( UINT32, BaseAddr, Register ) = ( UINT32 )( Data );    \
  }

#define VBT32And( BaseAddr, Register, AndData ) \
  VBT32Write (                                        \
    BaseAddr,                                         \
    Register,                                         \
    ( VBT32Read ( BaseAddr, Register ) & AndData )    \
    )

#define VBT32Or( BaseAddr, Register, OrData ) \
  VBT32Write (                                       \
    BaseAddr,                                        \
    Register,                                        \
    ( VBT32Read ( BaseAddr, Register ) | OrData )    \
    )

#define VBT32AndThenOr( BaseAddr, Register, AndData, OrData ) \
  VBT32Write (                                                     \
    BaseAddr,                                                      \
    Register,                                                      \
    ( ( VBT32Read ( BaseAddr, Register ) & AndData ) | OrData )    \
    )

#endif
