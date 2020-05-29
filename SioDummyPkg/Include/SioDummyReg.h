//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _SIO_DUMMY_H_
#define _SIO_DUMMY_H_

#include <Sio/SioCommon.h>

//
// Define Super I/O device ID for verify component chip
//
#define SIO_ID1                     0xFF   
#define SIO_ID2                     0x00    //if the value set 0x00, it's mean we just check SIO ID1

#endif
