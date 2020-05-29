/** @file
 IPMI ASL Code.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


Device(MI0)
{
  Name(_HID, EISAID("IPI0001"))
  Name(_UID, 0)

  Name(IT1, Unicode("IPMI_KCS"))
  Name(IT2, Unicode("IPMI_SMIC"))
  Name(IT3, Unicode("IPMI_BT"))
  //Name(IT4, Unicode("IPMI_SSIF"))

  Method(_STR, 0, Serialized)
  {
    Switch(ToInteger(IMIT))
    {
      Case(0x01) {return (IT1)}
      Case(0x02) {return (IT2)}
      Case(0x03) {return (IT3)}
      //Case(0x04) {return (IT4)}
      Default {return (IT1)}
    }
  }

  Method(_CRS, 0, Serialized)
  {

    Switch(ToInteger(IMIT))
    {
      Case(0x01)
      {
        Name(BUF0, ResourceTemplate ()
        {
          IO(Decode16, 0xCA2, 0xCA2, 0x01, 0x01)
          IO(Decode16, 0xCA3, 0xCA3, 0x01, 0x01)
        })

        CreateByteField (BUF0, 0x02, DP1L) // Data Port 1 Low Byte
        CreateByteField (BUF0, 0x03, DP1H) // Data Port 1 High Byte
        CreateByteField (BUF0, 0x04, DP2L) // Data Port 2 Low Byte
        CreateByteField (BUF0, 0x05, DP2H) // Data Port 2 High Byte
        CreateByteField (BUF0, 0x0A, CP1L) // Command Port 1 Low Byte
        CreateByteField (BUF0, 0x0B, CP1H) // Command Port 1 High Byte
        CreateByteField (BUF0, 0x0C, CP2L) // Command Port 2 Low Byte
        CreateByteField (BUF0, 0x0D, CP2H) // Command Port 2 High Byte

        And(IMBA, 0x00FF, DP1L)
        ShiftRight(IMBA, 0x08, DP1H)
        Store(DP1L, DP2L)
        Store(DP1H, DP2H)

        And(Add(IMBA, REOT), 0x00FF, CP1L)
        ShiftRight(Add(IMBA, REOT), 0x08, CP1H)
        Store(CP1L, CP2L)
        Store(CP1H, CP2H)

        return (BUF0)
      }

      Default
      {
        Name(BUF1, ResourceTemplate()
        {
          // Temp resource
          IO(Decode16, 0xCA2, 0xCA2, 0x01, 0x03)
        })

        CreateWordField (BUF1, 0x01, BAS1) // Base Address
        CreateWordField (BUF1, 0x02, BAS2) // Base Address

        Store(IMBA, BAS1)
        Store(IMBA, BAS2)

        return (BUF1)
      }
    }
  }

  Method(_IFT)
  {
    Return(IMIT)
  }

  Method(_SRV)
  {
    Name(REV0, 0x0000)
    Store(IMSV, REV0)

    Return(ShiftLeft(REV0, 0x04, REV0))
  }

  Method(_STA)
  {
    If(LLessEqual(OSYS, 2003))
    {
      Return (0x00)
    }

    If(LEqual(BMCS, 0x01))
    {
      Return (0x0F)
    }
    Else
    {
      Return (0x00)
    }
  }

}

