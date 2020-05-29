Name(PR09, Package() {
	
// PCIE Port #6 Slot
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, LNKB, 0 },
Package(){ 0x0000FFFF, 1, LNKC, 0 },
Package(){ 0x0000FFFF, 2, LNKD, 0 },
Package(){ 0x0000FFFF, 3, LNKA, 0 },
//[-end-120618-IB05330349-modify]//
})
Name(AR09, Package() {
	
// PCIE Port #6 Slot
//[-start-120618-IB05330349-modify]//
Package(){ 0x0000FFFF, 0, 0, 17 },
Package(){ 0x0000FFFF, 1, 0, 18 },
Package(){ 0x0000FFFF, 2, 0, 19 },
Package(){ 0x0000FFFF, 3, 0, 16 },
//[-end-120618-IB05330349-modify]//
})
//[-start-130726-IB06720229-remove]//
//
////[-start-130709-IB08050222-modify]//
//External(AR08, PkgObj)
//External(PR08, PkgObj)
//
//Method(_PRT, 0) {
//  If (LEqual (And (CDID, 0xF000), 0x8000)) { // LPT-H
//    If (PICM) { Return(AR09) } // APIC mode
//    Return (PR09)             // PIC mode
//  } Else { // ULT
//    If (PICM) { Return (AR08) }// APIC mode
//    Return (PR08) // PIC mode
//  }
//}
////[-end-130709-IB08050222-modify]//
//[-end-130726-IB06720229-remove]//
