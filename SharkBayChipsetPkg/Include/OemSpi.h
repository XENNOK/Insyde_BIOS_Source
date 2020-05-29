//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
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
//;   OemSpi.h
//;
#ifndef _OEM_SPI_H_
#define _OEM_SPI_H_

//
// Determine the SPI access method (1: MMIO / 0: IO Cycle)
//
#define SPI_READ_BY_MEMORY_MAPPED     1
//
// Determine the EC SPI access method (1: MMIO / 0: IO Cycle)
//
#define EC_SPI_READ_BY_MEMORY_MAPPED  1


//
// Determine the SPI read speed
// 
#define SPI_READ_COMMAND 0x03
//
// Determine the EC SPI read speed
// 
#define EC_SPI_READ_COMMAND 0x03

#endif //#ifndef _OEM_SPI_H_

