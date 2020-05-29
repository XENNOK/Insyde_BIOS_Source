//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************



DefinitionBlock ("SSDT.AML", "SSDT", 0x01, "Insyde", "XTU", 0x00001000)
{
	
External(ECON, IntObj)	
External(\_SB.PCI0.LPCB.H_EC.CFSP, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.CVRT, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSD0, FieldUnitObj)
External(\_SB.PCI0.LPCB.H_EC.TSD1, FieldUnitObj)
External(\_TZ.TZ01, ThermalZoneObj)
External(\_TZ.TZ01._TMP)              // Thermal Zone 01 Temperature
External(\TSOD, IntObj)
External(\GNVS)
	
	Scope(\_SB)
	{
          Field(GNVS,AnyAcc,Lock,Preserve)
          {
             Offset(439),
             XTUB, 32,       // (439) XTU Continous structure Base Address 
             XTUS, 32,       // (443) XTU Entries size
             XMPB, 32,       // (447) XMP Base Address
          }
		Device(PTMD)
		{

			Name(_HID, 0x9433D425)
			Name(_CID, EISAID("PNP0C02"))
			Name(IVER, 0x00010000)
			Name(SIZE, 0x1C00)
			Method(GACI, 0, NotSerialized)
			{
				Name(RPKG, Package(2){})
				Store(0x00, Index(RPKG, 0x00))
				If(LNotEqual(XTUB, Zero))
				{
					OperationRegion(XNVS, SystemMemory, XTUB, SIZE)
					Field(XNVS, ByteAcc, NoLock, Preserve)
					{
						XBUF, 7168,
					}
					Name(TEMP, Buffer(XTUS){})
					Store(XBUF, TEMP)
					Store(TEMP, Index(RPKG, 0x01))
				}
				Else
				{
					Store(0x00, Index(RPKG, 0x01))
				}
				Return(RPKG)
			}
			Method(GDSV, 1, Serialized)
			{
				If(LEqual(Arg0, 0x05))
				{
					Return(Package(2)
					{
						0x00, 
						Buffer(0x40)
						{
							0x00, 0x00, 0x00, 0x00, 0x46, 0x05, 0x00, 0x00, 
							0x01, 0x00, 0x00, 0x00, 0x79, 0x05, 0x00, 0x00, 
							0x02, 0x00, 0x00, 0x00, 0xA9, 0x05, 0x00, 0x00, 
							0x03, 0x00, 0x00, 0x00, 0xDC, 0x05, 0x00, 0x00, 
							0x04, 0x00, 0x00, 0x00, 0xDF, 0x05, 0x00, 0x00, 
							0x05, 0x00, 0x00, 0x00, 0xFD, 0x05, 0x00, 0x00, 
							0x06, 0x00, 0x00, 0x00, 0x42, 0x06, 0x00, 0x00, 
							0x07, 0x00, 0x00, 0x00, 0x74, 0x06, 0x00, 0x00
						}
					})
				}
				If(LEqual(Arg0, 0x13))
				{
					Return(Package(2)
					{
						0x00, 
						Buffer(0x38)
						{
							0x04, 0x00, 0x00, 0x00, 0x2B, 0x04, 0x00, 0x00, 
							0x05, 0x00, 0x00, 0x00, 0x35, 0x05, 0x00, 0x00, 
							0x06, 0x00, 0x00, 0x00, 0x40, 0x06, 0x00, 0x00, 
							0x07, 0x00, 0x00, 0x00, 0x4B, 0x07, 0x00, 0x00, 
							0x08, 0x00, 0x00, 0x00, 0x55, 0x08, 0x00, 0x00, 
							0x09, 0x00, 0x00, 0x00, 0x60, 0x09, 0x00, 0x00, 
							0x0A, 0x00, 0x00, 0x00, 0x6B, 0x0A, 0x00, 0x00
						}
					})
				}
				Return(Package(1) {0x01})
			}
			Method(GXDV, 1, Serialized)
			{
				If(LNotEqual(XMPB, Zero))
				{
					OperationRegion(XMPN, SystemMemory, XMPB, SIZE)
					Field(XMPN, ByteAcc, NoLock, Preserve)
					{
//[-start-120521-IB03780442-modify]//
//[-start-120210-IB03780422-modify]//
						XMP1, 768,
						XMP2, 768,
//[-end-120210-IB03780422-modify]//
//[-end-120521-IB03780442-modify]//
					}
					If(LEqual(Arg0, 0x01))
					{
						Name(XP_1, Package(2){})
						Store(0x00, Index(XP_1, 0x00))
						Store(XMP1, Index(XP_1, 0x01))
						Return(XP_1)
					}
					If(LEqual(Arg0, 0x02))
					{
						Name(XP_2, Package(2){})
						Store(0x00, Index(XP_2, 0x00))
						Store(XMP2, Index(XP_2, 0x01))
						Return(XP_2)
					}
					Return(Package(1) {0x01})
				}
			}
			Method(GSCV, 0, NotSerialized)
			{
				Return(Package(1) {0x72})
			}
			Method(CDRD, 1, Serialized)
			{
				Return(Package(1) {0x01})
			}
			Method(CDWR, 2, Serialized)
			{
				Return(0x01)
			}
			Name(RPMV, Package(4) {0x01, 0x07, 0x00, 0x00})
			Name(TMP1, Package(12)
			{
				0x01, 0x02, 0x00, 0x00, 0x05, 0x04, 0x00, 0x00, 0x06, 0x05, 0x00,
				0x00
			})
			Name(TMP2, Package(8) {0x01, 0x02, 0x00, 0x00, 0x05, 0x04, 0x00, 0x00})
			Name(TMP3, Package(4) {0x01, 0x02, 0x00, 0x00})
			Method(TSDD, 0, NotSerialized)
			{
				If(\ECON)
				{
					If(\TSOD)
					{
						Store(\_TZ.TZ01._TMP, Index(TMP1, 0x02))
						Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.CVRT, 0x0A), 0x0AAC), Index(TMP1, 0x06))
						If(LGreater(\_SB.PCI0.LPCB.H_EC.TSD0, \_SB.PCI0.LPCB.H_EC.TSD1))
						{
							Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.TSD0, 0x0A), 0x0AAC), Index(TMP1, 0x0A))
						}
						Else
						{
							Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.TSD1, 0x0A), 0x0AAC), Index(TMP1, 0x0A))
						}
						Return(TMP1)
					}
					Else
					{
						Store(\_TZ.TZ01._TMP, Index(TMP2, 0x02))
						Store(Add(Multiply(\_SB.PCI0.LPCB.H_EC.CVRT, 0x0A), 0x0AAC), Index(TMP2, 0x06))
						Return(TMP2)
					}
				}
				Else
				{
					Store(\_TZ.TZ01._TMP, Index(TMP3, 0x02))
					Return(TMP3)
				}
			}
			Method(FSDD, 0, NotSerialized)
			{
				If(\ECON)
				{
					Store(\_SB.PCI0.LPCB.H_EC.CFSP, Index(RPMV, 0x02))
				}
				Return(RPMV)
			}
			Method(SDSP, 0, NotSerialized)
			{
				Return(0x0A)
			}
		}
	}
}
