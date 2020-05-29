Name(PR07, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, LNKD, 0 },
Package(){ 0x0000FFFF, 1, LNKA, 0 },
Package(){ 0x0000FFFF, 2, LNKB, 0 },
Package(){ 0x0000FFFF, 3, LNKC, 0 },
//[-end-120618-IB05330349-modify]//
})
Name(AR07, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, 0, 19 },
Package(){ 0x0000FFFF, 1, 0, 16 },
Package(){ 0x0000FFFF, 2, 0, 17 },
Package(){ 0x0000FFFF, 3, 0, 18 },
//[-end-120618-IB05330349-modify]//
})
//[-start-130726-IB06720229-modify]//
//Method(_PRT, 0) {
//    If(PICM) { Return(AR07) } // APIC mode
//    Return (PR07)             // PIC mode
//}
//[-end-130726-IB06720229-modify]//
