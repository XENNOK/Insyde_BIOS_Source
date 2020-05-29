Name(PR00, Package() {
//[-start-121016-IB06150252-add]//
// Audio Digital Signal Processor 
Package(){ 0x0013FFFF, 0, LNKH, 0 },
//[-end-121016-IB06150252-add]//
// XHCI
//[-start-120319-IB05330329-modify]//
Package(){ 0x0014FFFF, 0, LNKA, 0 },
Package(){ 0x0014FFFF, 1, LNKB, 0 },
//[-start-120618-IB05330349-modify]//
Package(){ 0x0014FFFF, 2, LNKC, 0 },
Package(){ 0x0014FFFF, 3, LNKD, 0 },
//[-start-120727-IB05330364-add]//
// LPSS D21
Package(){ 0x0015FFFF, 0, LNKE, 0 },
Package(){ 0x0015FFFF, 1, LNKF, 0 },
//[-end-120727-IB05330364-add]//

// 
Package(){ 0x0016FFFF, 0, LNKA, 0 },
Package(){ 0x0016FFFF, 1, LNKB, 0 },
Package(){ 0x0016FFFF, 2, LNKC, 0 },
Package(){ 0x0016FFFF, 3, LNKD, 0 },
//[-start-120727-IB05330364-add]//
// SDIO D23 
Package(){ 0x0017FFFF, 0, LNKG, 0 },
//[-end-120727-IB05330364-add]//
// 
Package(){ 0x0019FFFF, 0, LNKE, 0 },
//[-start-120705-IB05330352-remove]//
//Package(){ 0x0019FFFF, 1, LNKF, 0 },
//Package(){ 0x0019FFFF, 2, LNKG, 0 },
//Package(){ 0x0019FFFF, 3, LNKH, 0 },
//[-end-120705-IB05330352-remove]//
// 
Package(){ 0x001AFFFF, 0, LNKA, 0 },
Package(){ 0x001AFFFF, 1, LNKF, 0 },
Package(){ 0x001AFFFF, 2, LNKC, 0 },
Package(){ 0x001AFFFF, 3, LNKD, 0 },
// 
Package(){ 0x001BFFFF, 0, LNKG, 0 },
//[-start-120705-IB05330352-remove]//
//Package(){ 0x001BFFFF, 1, LNKB, 0 },
//Package(){ 0x001BFFFF, 2, LNKC, 0 },
//Package(){ 0x001BFFFF, 3, LNKD, 0 },
//[-end-120705-IB05330352-remove]//
//[-end-120618-IB05330349-modify]//
// 
Package(){ 0x001CFFFF, 0, LNKA, 0 },
Package(){ 0x001CFFFF, 1, LNKB, 0 },
Package(){ 0x001CFFFF, 2, LNKC, 0 },
Package(){ 0x001CFFFF, 3, LNKD, 0 },
// 
Package(){ 0x001DFFFF, 0, LNKH, 0 },
Package(){ 0x001DFFFF, 1, LNKD, 0 },
Package(){ 0x001DFFFF, 2, LNKA, 0 },
Package(){ 0x001DFFFF, 3, LNKC, 0 },
// 
//[-start-120326-IB05330332-modify]//
//[-start-120618-IB05330349-modify]//
Package(){ 0x001FFFFF, 0, LNKA, 0 },
//[-end-120618-IB05330349-modify]//
//[-end-120326-IB05330332-modify]//
Package(){ 0x001FFFFF, 1, LNKD, 0 },
Package(){ 0x001FFFFF, 2, LNKC, 0 },
Package(){ 0x001FFFFF, 3, LNKA, 0 },
//[-end-120319-IB05330329-modify]//
// 
Package(){ 0x0001FFFF, 0, LNKA, 0 },
Package(){ 0x0001FFFF, 1, LNKB, 0 },
Package(){ 0x0001FFFF, 2, LNKC, 0 },
Package(){ 0x0001FFFF, 3, LNKD, 0 },
//[-start-120326-IB05330332-modify]//
// Mobile IGFX
Package(){ 0x0002FFFF, 0, LNKA, 0 },
// SA Audio Device
Package(){ 0x0003FFFF, 0, LNKA, 0 },
// SA Thermal Device
//[-end-120326-IB05330332-modify]//
Package(){ 0x0004FFFF, 0, LNKA, 0 },
//[-start-120618-IB05330349-remove]//
//Package(){ 0x0004FFFF, 1, LNKB, 0 },
//Package(){ 0x0004FFFF, 2, LNKC, 0 },
//Package(){ 0x0004FFFF, 3, LNKD, 0 },
//[-end-120618-IB05330349-remove]//
// 
//[-start-120326-IB05330332-remove]//
//Package(){ 0x0006FFFF, 0, LNKD, 0 },
//Package(){ 0x0006FFFF, 1, LNKA, 0 },
//Package(){ 0x0006FFFF, 2, LNKB, 0 },
//Package(){ 0x0006FFFF, 3, LNKC, 0 },
//[-end-120326-IB05330332-remove]//
// 
Package(){ 0x0018FFFF, 0, LNKE, 0 },
})
Name(AR00, Package() {
// 
//[-start-120319-IB05330329-modify]//
//[-start-120618-IB05330349-modify]//
// xHCI Controller D20
Package(){ 0x0014FFFF, 0, 0, 16 },
Package(){ 0x0014FFFF, 1, 0, 17 },
Package(){ 0x0014FFFF, 2, 0, 18 },
Package(){ 0x0014FFFF, 3, 0, 19 },
//[-start-120727-IB05330364-add]//
// LPSS D21
Package(){ 0x0015FFFF, 0, 0, 20 },
Package(){ 0x0015FFFF, 1, 0, 21 },
//[-end-120727-IB05330364-add]//

// Intel Management Engine Subsystem D22 
Package(){ 0x0016FFFF, 0, 0, 16 },
Package(){ 0x0016FFFF, 1, 0, 17 },
Package(){ 0x0016FFFF, 2, 0, 18 },
Package(){ 0x0016FFFF, 3, 0, 19 },

//[-start-120727-IB05330364-add]//
// SDIO D23 
Package(){ 0x0017FFFF, 0, 0, 22 },
//[-end-120727-IB05330364-add]//

// Gigabit Ethernet Controller D25
Package(){ 0x0019FFFF, 0, 0, 20 },
//[-start-120705-IB05330352-remove]//
//Package(){ 0x0019FFFF, 1, 0, 21 },
//Package(){ 0x0019FFFF, 2, 0, 22 },
//Package(){ 0x0019FFFF, 3, 0, 23 },
//[-end-120705-IB05330352-remove]//
// USB EHCI Host Controllers D26
Package(){ 0x001AFFFF, 0, 0, 16 },
Package(){ 0x001AFFFF, 1, 0, 21 },
Package(){ 0x001AFFFF, 2, 0, 18 },
Package(){ 0x001AFFFF, 3, 0, 19 },
//[-start-130208-IB02382266-add]//
// Audio DSP
Package(){0x0013FFFF, 0, 0, 23 },
//[-end-130208-IB02382266-add]//
// Intel High Definition Audio D27
Package(){ 0x001BFFFF, 0, 0, 22 },
//[-start-120705-IB05330352-remove]//
//Package(){ 0x001BFFFF, 1, 0, 17 },
//Package(){ 0x001BFFFF, 2, 0, 18 },
//Package(){ 0x001BFFFF, 3, 0, 19 },
//[-end-120705-IB05330352-remove]//
// PCI Express Root Ports D28
Package(){ 0x001CFFFF, 0, 0, 16 },
Package(){ 0x001CFFFF, 1, 0, 17 },
Package(){ 0x001CFFFF, 2, 0, 18 },
Package(){ 0x001CFFFF, 3, 0, 19 },
// USB EHCI Host Controllers D29
Package(){ 0x001DFFFF, 0, 0, 23 },
Package(){ 0x001DFFFF, 1, 0, 19 },
Package(){ 0x001DFFFF, 2, 0, 16 },
Package(){ 0x001DFFFF, 3, 0, 18 },
//[-start-120326-IB05330332-modify]//
// LPC D31
Package(){ 0x001FFFFF, 0, 0, 16 },
//[-end-120618-IB05330349-modify]//
Package(){ 0x001FFFFF, 1, 0, 19 },
//[-end-120319-IB05330329-modify]//
Package(){ 0x001FFFFF, 2, 0, 18 },
Package(){ 0x001FFFFF, 3, 0, 16 },
//[-end-120326-IB05330332-modify]//
// 
Package(){ 0x0001FFFF, 0, 0, 16 },
Package(){ 0x0001FFFF, 1, 0, 17 },
Package(){ 0x0001FFFF, 2, 0, 18 },
Package(){ 0x0001FFFF, 3, 0, 19 },
//[-start-120326-IB05330332-modify]//
// Mobile IGFX
Package(){ 0x0002FFFF, 0, 0, 16 },
// SA Audio Device
Package(){ 0x0003FFFF, 0, 0, 16 },
// SA Thermal Device
//[-end-120326-IB05330332-modify]//
Package(){ 0x0004FFFF, 0, 0, 16 },
//[-start-120618-IB05330349-remove]//
//Package(){ 0x0004FFFF, 1, 0, 17 },
//Package(){ 0x0004FFFF, 2, 0, 18 },
//Package(){ 0x0004FFFF, 3, 0, 19 },
//[-end-120618-IB05330349-remove]//
 
// 
//[-start-120326-IB05330332-remove]//
//Package(){ 0x0006FFFF, 0, 0, 19 },
//Package(){ 0x0006FFFF, 1, 0, 16 },
//Package(){ 0x0006FFFF, 2, 0, 17 },
//Package(){ 0x0006FFFF, 3, 0, 18 },
//[-end-120326-IB05330332-remove]//
// 
Package(){ 0x0018FFFF, 0, 0, 20 },
})
//[-start-130726-IB06720229-remove]//
//Method(_PRT, 0) {
//    If(PICM) { Return(AR00) } // APIC mode
//    Return (PR00)             // PIC mode
//}
//[-end-130726-IB06720229-remove]//
