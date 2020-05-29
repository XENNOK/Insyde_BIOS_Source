Name(PR02, Package() {
//[-start-120327-IB05330332-modify]//
// 
Package(){ 0x0000FFFF, 0, LNKA, 0 },
Package(){ 0x0000FFFF, 1, LNKB, 0 },
Package(){ 0x0000FFFF, 2, LNKC, 0 },
Package(){ 0x0000FFFF, 3, LNKD, 0 },
})
Name(AR02, Package() {
// 
Package(){ 0x0000FFFF, 0, 0, 16 },
Package(){ 0x0000FFFF, 1, 0, 17 },
Package(){ 0x0000FFFF, 2, 0, 18 },
Package(){ 0x0000FFFF, 3, 0, 19 },
})
//[-end-120327-IB05330332-modify]//
//[-start-121101-IB10540016-remove]//
//Method(_PRT, 0) {
//    If(PICM) { Return(AR02) } // APIC mode
//    Return (PR02)             // PIC mode
//}
//[-end-121101-IB10540016-remove]//
