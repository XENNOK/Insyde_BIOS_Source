Name(PR05, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, LNKB, 0 },
Package(){ 0x0000FFFF, 1, LNKC, 0 },
Package(){ 0x0000FFFF, 2, LNKD, 0 },
Package(){ 0x0000FFFF, 3, LNKA, 0 },
//[-end-120618-IB05330349-modify]//
})
Name(AR05, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, 0, 17 },
Package(){ 0x0000FFFF, 1, 0, 18 },
Package(){ 0x0000FFFF, 2, 0, 19 },
Package(){ 0x0000FFFF, 3, 0, 16 },
//[-end-120618-IB05330349-modify]//
})
//[-start-130726-IB06720229-remove]//
//Method(_PRT, 0) {
//    If(PICM) { Return(AR05) } // APIC mode
//    Return (PR05)             // PIC mode
//}
//[-end-130726-IB06720229-remove]//
