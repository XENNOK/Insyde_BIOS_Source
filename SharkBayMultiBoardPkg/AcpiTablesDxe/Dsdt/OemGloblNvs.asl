//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

  // Define a OEM Global region of ACPI NVS Region that may be used for any
  // type of implementation.  The starting offset and size will be fixed
  // up by the System BIOS during POST.  Note that the Size must be a word
  // in size to be fixed up correctly.

  OperationRegion(OGNS,SystemMemory,0x55AA55AA, 0x55AA55AA)  
  Field(OGNS,AnyAcc,Lock,Preserve)
  {
    // The definitions below need to be matched OemGlobalNvsArea.h 
    // and can be modified by OEM
	OG00,	8,	//   (00) Oem GlobalNvStore Data 00
	OG01,	8,	//   (01) Oem GlobalNvStore Data 01
	OG02,	8,	//   (02) Oem GlobalNvStore Data 02
	OG03,	8,	//   (03) Oem GlobalNvStore Data 03
	OG04,	8,	//   (04) Oem GlobalNvStore Data 04
	OG05,	8,	//   (05) Oem GlobalNvStore Data 05
	OG06,	8,	//   (06) Oem GlobalNvStore Data 06
	OG07,	8,	//   (07) Oem GlobalNvStore Data 07
	OG08,	8,	//   (08) Oem GlobalNvStore Data 08
	OG09,	8,	//   (09) Oem GlobalNvStore Data 09
	OG10,	8,	//   (10) Oem GlobalNvStore Data 10
	ECON, 8,        //   (11) OemEmbeddedControllerFlag
  }

