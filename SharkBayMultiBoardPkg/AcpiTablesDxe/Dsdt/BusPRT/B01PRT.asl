	Name(PR01, Package(){
// ThimblePeak 1
		Package(){0x0000FFFF, 0, LNKF, 0 },
		Package(){0x0000FFFF, 1, LNKG, 0 },
		Package(){0x0000FFFF, 2, LNKH, 0 },
		Package(){0x0000FFFF, 3, LNKE, 0 },
// ThimblePeak 2
		Package(){0x0001FFFF, 0, LNKG, 0 },
		Package(){0x0001FFFF, 1, LNKF, 0 },
		Package(){0x0001FFFF, 2, LNKE, 0 },
		Package(){0x0001FFFF, 3, LNKH, 0 },
// ThimblePeak 3
		Package(){0x0002FFFF, 0, LNKH, 0 },
		Package(){0x0002FFFF, 1, LNKE, 0 },
		Package(){0x0002FFFF, 2, LNKF, 0 },
		Package(){0x0002FFFF, 3, LNKG, 0 },
// ThimblePeak 4
		Package(){0x0003FFFF, 0, LNKD, 0 },
		Package(){0x0003FFFF, 1, LNKC, 0 },
		Package(){0x0003FFFF, 2, LNKF, 0 },
		Package(){0x0003FFFF, 3, LNKG, 0 },
// ThimblePeak 5
		Package(){0x0004FFFF, 0, LNKC, 0 },
		Package(){0x0004FFFF, 1, LNKH, 0 },
		Package(){0x0004FFFF, 2, LNKA, 0 },
		Package(){0x0004FFFF, 3, LNKE, 0 },
// ThimblePeak 6
		Package(){0x0005FFFF, 0, LNKC, 0 },
		Package(){0x0005FFFF, 1, LNKE, 0 },
		Package(){0x0005FFFF, 2, LNKG, 0 },
		Package(){0x0005FFFF, 3, LNKF, 0 },
	})
	Name(AR01, Package(){
// ThimblePeak 1
		Package(){0x0000FFFF, 0, 0, 21 },
		Package(){0x0000FFFF, 1, 0, 22 },
		Package(){0x0000FFFF, 2, 0, 23 },
		Package(){0x0000FFFF, 3, 0, 20 },
// ThimblePeak 2
		Package(){0x0001FFFF, 0, 0, 22 },
		Package(){0x0001FFFF, 1, 0, 21 },
		Package(){0x0001FFFF, 2, 0, 20 },
		Package(){0x0001FFFF, 3, 0, 23 },
// ThimblePeak 3
		Package(){0x0002FFFF, 0, 0, 23 },
		Package(){0x0002FFFF, 1, 0, 20 },
		Package(){0x0002FFFF, 2, 0, 21 },
		Package(){0x0002FFFF, 3, 0, 22 },
// ThimblePeak 4
		Package(){0x0003FFFF, 0, 0, 19 },
		Package(){0x0003FFFF, 1, 0, 18 },
		Package(){0x0003FFFF, 2, 0, 21 },
		Package(){0x0003FFFF, 3, 0, 22 },
// ThimblePeak 5
		Package(){0x0004FFFF, 0, 0, 18 },
		Package(){0x0004FFFF, 1, 0, 23 },
		Package(){0x0004FFFF, 2, 0, 16 },
		Package(){0x0004FFFF, 3, 0, 20 },
// ThimblePeak 6
		Package(){0x0005FFFF, 0, 0, 18 },
		Package(){0x0005FFFF, 1, 0, 20 },
		Package(){0x0005FFFF, 2, 0, 22 },
		Package(){0x0005FFFF, 3, 0, 21 },
	})
//[-start-130726-IB06720229-remove]//
//Method(_PRT,0) {
//	If(PICM) { Return(AR01) }// APIC mode
//	Return (PR01) // PIC mode
//}
//[-end-130726-IB06720229-remove]//
