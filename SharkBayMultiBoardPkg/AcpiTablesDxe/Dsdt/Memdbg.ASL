//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#define DBG_SIZE        0x1000
#define DBG_SIZE_MASK   0xFFF

Name(BufN, 0x0)                 // Buffer Number Offset
Name(MBuf,Buffer(DBG_SIZE){})        // Memory Buffer

OperationRegion(\MDBG,SystemMemory,0x55AA55AA, 0x55AA55AA)  
Field(\MDBG,AnyAcc,Lock,Preserve)
{
      Offset(0),
      MDG0, 32768         // Set total memory for debug code = 4096 * 8
}

//
// ======================   Command  ============================
//
//
// **********************   Show Number  ************************ 
//
Method(DB2H, 1, Serialized)             // Show Byte Number in Memory
{
//[-start-120816-IB05330371-modify]//
    if (MDEN) {
      SHOW(arg0)
      MDGC(0x20)
      Store(MBuf, MDG0)
    } else { 
      Return
    }
//[-end-120816-IB05330371-modify]//
}

Method(DW2H, 1, Serialized)             // Show Word Number in Memory
{
    Store(arg0, local0)
    ShiftRight(arg0, 8, local1)
    And(local0, 0xff, local0)
    And(local1, 0xff, local1)
    DB2H(local1)
    Decrement(BufN)
    DB2H(local0)
}

Method(DD2H, 1, Serialized)             // Show DWord Number in Memory
{
    Store(arg0, local0)
    ShiftRight(arg0, 16, local1)
    And(local0, 0xffff, local0)
    And(local1, 0xffff, local1)
    DW2H(local1)
    Decrement(BufN)
    DW2H(local0)
}

//
// **********************   Show String    ********************** 
//
Method(MBGS, 1, Serialized)             // Show String in Memory
{
//[-start-120816-IB05330371-modify]//
    if (MDEN) {
      Store(Sizeof(arg0), local0)         // Get String Length
      Name(BUFS, Buffer(local0){})
      Store(ARG0, BUFS)
      MDGC(0x20)
//    LINE()
      While(local0) {
          MDGC(Derefof(Index(BUFS, Subtract(Sizeof(arg0), local0))))
          Decrement(local0)    
      }
      Store(MBuf, MDG0)
    } else {
      Return
    }
//[-end-120816-IB05330371-modify]//
//    LINE()
}

//[-start-121130-IB10540021-add]//
Method(ADBG, 1, Serialized)             // Show String in Memory
{
    if (MDEN) {
      Store(Sizeof(arg0), local0)         // Get String Length
      Name(BUFS, Buffer(local0){})
      Store(ARG0, BUFS)
      MDGC(0x20)
      While(local0) {
          MDGC(Derefof(Index(BUFS, Subtract(Sizeof(arg0), local0))))
          Decrement(local0)    
      }
      Store(MBuf, MDG0)
    } else {
      Return
    }
}
//[-end-121130-IB10540021-add]//

//
// **********************  Command function  *********************
//
Method(SHOW, 1, Serialized)             // Show Number
{
    MDGC(NtoC(ShiftRight(arg0, 4)))
    MDGC(NtoC(arg0))
}

Method(LINE, 0, Serialized)             // Change line
{
    Store(BufN, local0)
    And(local0, 0x0f, local0)
    while(local0){
        MDGC(0x0)
        Increment(local0)
        And(local0, 0x0f, local0)
    }    
}

//
// **********************  Show Character  **********************
//
Method(MDGC, 1, Serialized)
{
        Store(arg0, Index(MBuf, BufN))      // Save character to buffer
        Add(BufN, 1, BufN)
        If(LGreater(BufN, DBG_SIZE_MASK)) {           
            And(BufN, DBG_SIZE_MASK, BufN)
            UP_L(1)                         // Move up 1 line
        }
}

Method(UP_L, 1, Serialized)             // Move memory up
{
    Store(arg0, local2)                 // get line number
    ShiftLeft(local2, 4, local2)
    MOVE(local2)
    Subtract(DBG_SIZE, local2, local3)
    while(local2){
        Store(0x0, Index(MBuf, local3))
        Increment(local3)
        Decrement(local2)
    }    
}

Method(MOVE, 1, Serialized)
{
    Store(arg0, local4)
    Store(0x0, BufN)

    Subtract(DBG_SIZE, local4, local5)
    while(local5){    
        Decrement(local5)
        Store(DeRefOf(Index(MBuf,local4)), Index(MBuf, BufN))
        Increment(BufN)
        Increment(local4)
    }
    
}

//
// ****************  Convert INT to ASCII character ***************
//
Method(NtoC, 1, Serialized)                 // Change Number to character
{
    And(arg0, 0x0f, local0)
    If(LLess(local0, 10)) {                 // 0 ~ 9 ASCII code
        Add(local0, 0x30, local0)
    }
    Else {                                  // A ~ F ASCII code
        Add(local0, 0x37, local0)
    }
    Return(local0)
}

