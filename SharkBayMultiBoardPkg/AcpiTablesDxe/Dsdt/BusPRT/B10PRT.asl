Name(PR0A, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, LNKB, 0 },
Package(){ 0x0000FFFF, 1, LNKC, 0 },
Package(){ 0x0000FFFF, 2, LNKD, 0 },
Package(){ 0x0000FFFF, 3, LNKA, 0 },
//[-end-120618-IB05330349-modify]//
})
Name(AR0A, Package() {
// 
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, 0, 17 },
Package(){ 0x0000FFFF, 1, 0, 18 },
Package(){ 0x0000FFFF, 2, 0, 19 },
Package(){ 0x0000FFFF, 3, 0, 16 },
//[-end-120618-IB05330349-modify]//
})
//[-start-121101-IB10540016-remove]//
//Method(_PRT, 0) {
//    If(PICM) { Return(AR0A) } // APIC mode
//    Return (PR0A)             // PIC mode
//}
//[-end-121101-IB10540016-remove]//
