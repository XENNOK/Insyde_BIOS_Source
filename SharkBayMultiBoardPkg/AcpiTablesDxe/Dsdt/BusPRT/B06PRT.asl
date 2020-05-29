Name(PR06, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, LNKC, 0 },
Package(){ 0x0000FFFF, 1, LNKD, 0 },
Package(){ 0x0000FFFF, 2, LNKA, 0 },
Package(){ 0x0000FFFF, 3, LNKB, 0 },
//[-end-120618-IB05330349-modify]//
})
Name(AR06, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, 0, 18 },
Package(){ 0x0000FFFF, 1, 0, 19 },
Package(){ 0x0000FFFF, 2, 0, 16 },
Package(){ 0x0000FFFF, 3, 0, 17 },
//[-end-120618-IB05330349-modify]//
})
//[-start-130726-IB06720229-remove]//
//Method(_PRT, 0) {
//    If(PICM) { Return(AR06) } // APIC mode
//    Return (PR06)             // PIC mode
//}
//[-end-130726-IB06720229-remove]//
