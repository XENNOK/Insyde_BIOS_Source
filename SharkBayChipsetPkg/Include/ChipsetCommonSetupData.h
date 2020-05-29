/** @file
  Chipset Setup Configuration Data

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifdef _IMPORT_CHIPSET_COMMON_SETUP_

//==========================================================================================================
//==========================================================================================================
//----------------------------------------------------------------------------------------------------------
// The following area is used by Chipset team to modify.
// The total size of variables in this part(Chipset_Start ~ Chipset_End) are fixed(300bytes). That means if
// you need to add or remove variables, please modify the ChipsetRSV buffer size as well.
//----------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------
// Please setup up the Chipset dependent variables here.
//----------------------------------------------------------------------------------------------------------
//Chipset_Start
//Offset(230),

  //
  // Peripheral Configuration
  // Offset(230:3)
  //
  UINT8         Lan;                                   //Offset 230
  UINT8         SlpLanLowDc;                           //Offset 231
  UINT8         AzaliaPme;                             //Offset 232

  //
  // Usb Controller
  // Offset(233:5)
  //
  UINT8         XHCIMode;                              //Offset 233
  UINT8         XhciPreBootSupport;                    //Offset 234
  UINT8         XhciStreams;                           //Offset 235
  UINT8         Ehci1Enable;                           //Offset 236
  UINT8         Ehci2Enable;                           //Offset 237

  //
  // Usb Port
  // Offset(238:15)
  //
  UINT8         UsbPerPortCtl;                         //Offset 238
  UINT8         UsbPort0;                              //Offset 239
  UINT8         UsbPort1;                              //Offset 240
  UINT8         UsbPort2;                              //Offset 241
  UINT8         UsbPort3;                              //Offset 242
  UINT8         UsbPort4;                              //Offset 243
  UINT8         UsbPort5;                              //Offset 244
  UINT8         UsbPort6;                              //Offset 245
  UINT8         UsbPort7;                              //Offset 246
  UINT8         UsbPort8;                              //Offset 247
  UINT8         UsbPort9;                              //Offset 248
  UINT8         UsbPort10;                             //Offset 249
  UINT8         UsbPort11;                             //Offset 250
  UINT8         UsbPort12;                             //Offset 251
  UINT8         UsbPort13;                             //Offset 252

  //
  // WatchDog
  // Offset(253:7)
  //
  UINT8         WatchDog;                              //Offset 253
  UINT16        WatchDogTimerOs;                       //Offset 254 - 255
  UINT16        WatchDogTimerBios;                     //Offset 256 - 257
  UINT8         TCOWatchDog;                           //Offset 258
  UINT8         WatchDogAcpiTable;                     //Offset 259

  //
  // Intel AMT
  // Offset(260:15)
  //
  UINT8         CiraRequest;                           //Offset 260
  UINT8         CiraTimeout;                           //Offset 261
  UINT8         Asf;                                   //Offset 262
  UINT8         Amt;                                   //Offset 263
  UINT8         AmtbxPasswordWrite;                    //Offset 264
  UINT8         UnConfigureME;                         //Offset 265
  UINT8         MebxDebugMsg;                          //Offset 266
  UINT8         FWProgress;                            //Offset 267
  UINT8         MeFwImageType;                         //Offset 268
  UINT8         iAmtbxSelectionScreen;                 //Offset 269
  UINT8         ChapDeviceEnable;                      //Offset 270
  UINT8         Device4Enable;                         //Offset 271
  UINT8         ScramblerEnable;                       //Offset 272
  UINT8         HideUnConfigureMeConfirm;              //Offset 273
  UINT8         AmtUsbConfigure;                       //Offset 274

  //
  // PCI_EXPRESS_CONFIG, 8 ROOT PORTS
  // Offset(275:115)
  //
  UINT8         GbePeiEPortSelect;                     //Offset 275
  UINT8         PchDmiAspm;                            //Offset 276
  UINT8         PchPcieClockGating;                    //Offset 277
  UINT8         PcieRootPortEn0;                       //Offset 278
  UINT8         PcieRootPortEn1;                       //Offset 279
  UINT8         PcieRootPortEn2;                       //Offset 280
  UINT8         PcieRootPortEn3;                       //Offset 281
  UINT8         PcieRootPortEn4;                       //Offset 282
  UINT8         PcieRootPortEn5;                       //Offset 283
  UINT8         PcieRootPortEn6;                       //Offset 284
  UINT8         PcieRootPortEn7;                       //Offset 285
  UINT8         PcieRootPortAspm0;                     //Offset 286
  UINT8         PcieRootPortAspm1;                     //Offset 287
  UINT8         PcieRootPortAspm2;                     //Offset 288
  UINT8         PcieRootPortAspm3;                     //Offset 289
  UINT8         PcieRootPortAspm4;                     //Offset 290
  UINT8         PcieRootPortAspm5;                     //Offset 291
  UINT8         PcieRootPortAspm6;                     //Offset 292
  UINT8         PcieRootPortAspm7;                     //Offset 293
  UINT8         PcieRootPortURR0;                      //Offset 294
  UINT8         PcieRootPortURR1;                      //Offset 295
  UINT8         PcieRootPortURR2;                      //Offset 296
  UINT8         PcieRootPortURR3;                      //Offset 297
  UINT8         PcieRootPortURR4;                      //Offset 298
  UINT8         PcieRootPortURR5;                      //Offset 299
  UINT8         PcieRootPortURR6;                      //Offset 300
  UINT8         PcieRootPortURR7;                      //Offset 301
  UINT8         PcieRootPortFER0;                      //Offset 302
  UINT8         PcieRootPortFER1;                      //Offset 303
  UINT8         PcieRootPortFER2;                      //Offset 304
  UINT8         PcieRootPortFER3;                      //Offset 305
  UINT8         PcieRootPortFER4;                      //Offset 306
  UINT8         PcieRootPortFER5;                      //Offset 307
  UINT8         PcieRootPortFER6;                      //Offset 308
  UINT8         PcieRootPortFER7;                      //Offset 309
  UINT8         PcieRootPortNFER0;                     //Offset 310
  UINT8         PcieRootPortNFER1;                     //Offset 311
  UINT8         PcieRootPortNFER2;                     //Offset 312
  UINT8         PcieRootPortNFER3;                     //Offset 313
  UINT8         PcieRootPortNFER4;                     //Offset 314
  UINT8         PcieRootPortNFER5;                     //Offset 315
  UINT8         PcieRootPortNFER6;                     //Offset 316
  UINT8         PcieRootPortNFER7;                     //Offset 317
  UINT8         PcieRootPortCER0;                      //Offset 318
  UINT8         PcieRootPortCER1;                      //Offset 319
  UINT8         PcieRootPortCER2;                      //Offset 320
  UINT8         PcieRootPortCER3;                      //Offset 321
  UINT8         PcieRootPortCER4;                      //Offset 322
  UINT8         PcieRootPortCER5;                      //Offset 323
  UINT8         PcieRootPortCER6;                      //Offset 324
  UINT8         PcieRootPortCER7;                      //Offset 325
  UINT8         PcieRootPortCTO0;                      //Offset 326
  UINT8         PcieRootPortCTO1;                      //Offset 327
  UINT8         PcieRootPortCTO2;                      //Offset 328
  UINT8         PcieRootPortCTO3;                      //Offset 329
  UINT8         PcieRootPortCTO4;                      //Offset 330
  UINT8         PcieRootPortCTO5;                      //Offset 331
  UINT8         PcieRootPortCTO6;                      //Offset 332
  UINT8         PcieRootPortCTO7;                      //Offset 333
  UINT8         PcieRootPortPmeInt0;                   //Offset 334
  UINT8         PcieRootPortPmeInt1;                   //Offset 335
  UINT8         PcieRootPortPmeInt2;                   //Offset 336
  UINT8         PcieRootPortPmeInt3;                   //Offset 337
  UINT8         PcieRootPortPmeInt4;                   //Offset 338
  UINT8         PcieRootPortPmeInt5;                   //Offset 339
  UINT8         PcieRootPortPmeInt6;                   //Offset 340
  UINT8         PcieRootPortPmeInt7;                   //Offset 341
  UINT8         PcieRootPortSEFE0;                     //Offset 342
  UINT8         PcieRootPortSEFE1;                     //Offset 343
  UINT8         PcieRootPortSEFE2;                     //Offset 344
  UINT8         PcieRootPortSEFE3;                     //Offset 345
  UINT8         PcieRootPortSEFE4;                     //Offset 346
  UINT8         PcieRootPortSEFE5;                     //Offset 347
  UINT8         PcieRootPortSEFE6;                     //Offset 348
  UINT8         PcieRootPortSEFE7;                     //Offset 349
  UINT8         PcieRootPortSENFE0;                    //Offset 350
  UINT8         PcieRootPortSENFE1;                    //Offset 351
  UINT8         PcieRootPortSENFE2;                    //Offset 352
  UINT8         PcieRootPortSENFE3;                    //Offset 353
  UINT8         PcieRootPortSENFE4;                    //Offset 354
  UINT8         PcieRootPortSENFE5;                    //Offset 355
  UINT8         PcieRootPortSENFE6;                    //Offset 356
  UINT8         PcieRootPortSENFE7;                    //Offset 357
  UINT8         PcieRootPortSECE0;                     //Offset 358
  UINT8         PcieRootPortSECE1;                     //Offset 359
  UINT8         PcieRootPortSECE2;                     //Offset 360
  UINT8         PcieRootPortSECE3;                     //Offset 361
  UINT8         PcieRootPortSECE4;                     //Offset 362
  UINT8         PcieRootPortSECE5;                     //Offset 363
  UINT8         PcieRootPortSECE6;                     //Offset 364
  UINT8         PcieRootPortSECE7;                     //Offset 365
  UINT8         PcieRootPortPmeSci0;                   //Offset 366
  UINT8         PcieRootPortPmeSci1;                   //Offset 367
  UINT8         PcieRootPortPmeSci2;                   //Offset 368
  UINT8         PcieRootPortPmeSci3;                   //Offset 369
  UINT8         PcieRootPortPmeSci4;                   //Offset 370
  UINT8         PcieRootPortPmeSci5;                   //Offset 371
  UINT8         PcieRootPortPmeSci6;                   //Offset 372
  UINT8         PcieRootPortPmeSci7;                   //Offset 373
  UINT8         PcieRootPortHotPlug0;                  //Offset 374
  UINT8         PcieRootPortHotPlug1;                  //Offset 375
  UINT8         PcieRootPortHotPlug2;                  //Offset 376
  UINT8         PcieRootPortHotPlug3;                  //Offset 377
  UINT8         PcieRootPortHotPlug4;                  //Offset 378
  UINT8         PcieRootPortHotPlug5;                  //Offset 379
  UINT8         PcieRootPortHotPlug6;                  //Offset 380
  UINT8         PcieRootPortHotPlug7;                  //Offset 381
  UINT8         PcieSpeed0;                            //Offset 382
  UINT8         PcieSpeed1;                            //Offset 383
  UINT8         PcieSpeed2;                            //Offset 384
  UINT8         PcieSpeed3;                            //Offset 385
  UINT8         PcieSpeed4;                            //Offset 386
  UINT8         PcieSpeed5;                            //Offset 387
  UINT8         PcieSpeed6;                            //Offset 388
  UINT8         PcieSpeed7;                            //Offset 389
  //
  // GFX relevant Variables
  // Offset(390:15)
  //
  UINT8         PrimaryDisplay;                        //Offset 390  
  UINT8         IGDControl;                            //Offset 391
  UINT8         EnableRc6;                             //Offset 392     // Enable Graphic Render C-State (Render Standby)
  UINT8         GttSize;                               //Offset 393
  UINT8         AlwaysEnablePeg;                       //Offset 394
  UINT8         ApertureSize;                          //Offset 395
  UINT8         DvmtPreAllocated;                      //Offset 396
  UINT8         DvmtTotalGfxMemSize;                   //Offset 397
  UINT8         IGDBootType;                           //Offset 398
  UINT8         IGDBootTypeSecondary;                  //Offset 399
  UINT8         PanelType;                             //Offset 400
  UINT8         BacklightControl;                      //Offset 401
  UINT8         LFPConfiguration;                      //Offset 402
  UINT8         PanelColorDepth;                       //Offset 403
  UINT8         DeepRenderStandby;                     //Offset 404

  //
  // PEG ASPM
  // Offset(405:3)
  //
  UINT8         PegAspm;                               //Offset 405
  UINT8         PegAspmL0s;                            //Offset 406
  UINT8         PcieResetDelay;                        //Offset 407

  //
  // Platform Thermal
  // Offset(408:11)
  //
  UINT8         CriticalThermalTripPoint;              //Offset 408
  UINT8         Ac0FanSpeed;                           //Offset 409  // _AC0 Fan Speed
  UINT8         ActiveTripPointLowFanSpeed;            //Offset 410
  UINT8         Ac1FanSpeed;                           //Offset 411  // _AC1 Fan Speed
  UINT8         ActiveTripPointHighFanSpeed;           //Offset 412
  UINT8         PassiveThermalTripPoint;               //Offset 413
  UINT8         PassiveTc1Value;                       //Offset 414
  UINT8         PassiveTc2Value;                       //Offset 415
  UINT8         PassiveTspValue;                       //Offset 416
  UINT8         TrTSOnDimm;                            //Offset 417
  UINT8         ThermalDevice;                         //Offset 418  // D31F6

  //
  // Advanced CPU Control
  // Offset(419:34)
  //
  UINT8         ISTConfig;                             //Offset 419
  UINT8         CoreActive;                            //Offset 420
  UINT8         HTSupport;                             //Offset 421
  UINT8         ExecuteDisableBit;                     //Offset 422
  UINT8         VTSupport;                             //Offset 423
  UINT8         LimitCpuidMaximumValue;                //Offset 424
  UINT8         EnableTurboMode;                       //Offset 425
  UINT8         TStatesEnable;                         //Offset 426
  UINT8         DtsEnable;                             //Offset 427
  UINT8         EnableProcHot;                         //Offset 428
  UINT8         TXT;                                   //Offset 429      // TXT support
  UINT8         TXTSupport;                            //Offset 430      // Cpu support TXT
  UINT8         xAPICMode;                             //Offset 431
  UINT8         EnergyEfficientPState;                 //Offset 432
  UINT8         TurboPowerLimitMsrLock;                //Offset 433
  UINT8         LongTermPowerLimitOverride;            //Offset 434
  UINT16        LongDurationPowerLimit;                //Offset 435 - 436
  UINT16        LongDurationTimeWindow;                //Offset 437 - 438
  UINT8         ShortTermPowerLimitOverride;           //Offset 439
  UINT16        ShortDurationPowerLimit;               //Offset 440 - 441
  UINT16        PrimaryPlaneCurrentLimit;              //Offset 442 - 443
  UINT16        SecondaryPlaneCurrentLimit;            //Offset 444 - 445
  UINT8         TurboCap;                              //Offset 446
  UINT8         OcCapXe;                               //Offset 447
  UINT8         ExtremeEdition;                        //Offset 448
  UINT8         RatioLimit1Core;                       //Offset 449
  UINT8         RatioLimit2Core;                       //Offset 450
  UINT8         RatioLimit3Core;                       //Offset 451
  UINT8         RatioLimit4Core;                       //Offset 452

  //
  // C-State
  // Offset(453:22)
  //
  UINT8         CStates;                               //Offset 453
  UINT8         EnCStates;                             //Offset 454
  UINT8         EnableC3;                              //Offset 455
  UINT8         EnableC6;                              //Offset 456
  UINT8         EnableC7;                              //Offset 457
  UINT8         CStateAutoDemotion;                    //Offset 458
  UINT8         CStateAutoUnDemotion;                  //Offset 459
  UINT8         C6Latency;                             //Offset 460
  UINT8         C7Latency;                             //Offset 461
  UINT8         PkgCStateDemotion;                     //Offset 462
  UINT8         PkgCStateUnDemotion;                   //Offset 463
  UINT8         CStatePreWake;                         //Offset 464
  UINT8         PkgCStateLimit;                        //Offset 465
  UINT8         CstateLatencyControl0TimeUnit;         //Offset 466
  UINT8         CstateLatencyControl1TimeUnit;         //Offset 467
  UINT8         CstateLatencyControl2TimeUnit;         //Offset 468
  UINT16        CstateLatencyControl0Irtl;             //Offset 469 - 470
  UINT16        CstateLatencyControl1Irtl;             //Offset 471 - 472
  UINT16        CstateLatencyControl2Irtl;             //Offset 473 - 474

  //
  // Virtualization Engine Controller Interface
  // Offset(475:1)
  UINT8         ReclaimLongrunTest;                    //Offset 475

  //
  // Switchable graphics
  // Offset(476:13)
  //
  UINT64        Mxm30BinAddr;                         //Offset 476 - 483
  UINT16        Mxm30BinSize;                         //Offset 484 - 485
  UINT8         PannelScaling;                        //Offset 486
  UINT8         EnSGFunction;                         //Offset 487
  UINT8         PowerXpress;                          //Offset 488

  //
  // Misc
  // Offset(489:22)
  //
  UINT8         CRIDEnable;                           //Offset 489
  UINT8         Port80Route;                          //Offset 490          // Send debug port to LPC or PCI bus
  UINT8         VTD;                                  //Offset 491
  UINT8         MemoryFrequency;                      //Offset 492
  UINT8         NModeSupport;                         //Offset 493
  UINT8         MEReFlash;                            //Offset 494
  UINT8         StateAfterG3;                         //Offset 495
  UINT8         BoardCapability;                      //Offset 496
  UINT8         DeepSlpSx;                            //Offset 497
  UINT8         WakeOnLan;                            //Offset 498                      // For Integrated LAN wake actions
  UINT8         PciClockRatRun;                       //Offset 499
  UINT8         AESSupport;                           //Offset 500
  UINT8         AESEnable;                            //Offset 501
//[-start-140521-IB05400527-modify]//
  UINT8         PciMem64DecodeSupport;                //Offset 502
//[-end-140521-IB05400527-modify]//
  UINT16        HostClockFreqBackup;                  //Offset 503 - 504
  UINT8         DDR3Voltage;                          //Offset 505
  UINT8         EcTurboCtrlMode;                      //Offset 506
  UINT8         EcBrickCap;                           //Offset 507
  UINT8         EcPollingPeriod;                      //Offset 508
  UINT8         EcGuardBandVal;                       //Offset 509
  UINT8         EcAlgorithmSel;                       //Offset 510

  //
  // DMI Config
  // Offset(511:7)
  //
  UINT8         DmiVc1;                               //Offset 511
  UINT8         DmiVcp;                               //Offset 512
  UINT8         DmiVcm;                               //Offset 513
  UINT8         SaDmiAspm;                            //Offset 514
  UINT8         SaDmiExtSync;                         //Offset 515
  UINT8         PchDmiExtSync;                        //Offset 516
  UINT8         DmiGen2;                              //Offset 517

  //
  // PEG
  // Offset(518:4)
  //
  UINT8         PegGenx0;                             //Offset 518
  UINT8         PegGenx1;                             //Offset 519
  UINT8         PegGenx2;                             //Offset 520
  UINT8         AtPBASupport;                         //Offset 521
  
  //
  // Thunderbolt
  // Offset(522:2)
  //
  UINT8         TBTSmiEnable;                         //Offset 522
  UINT8         TBTCacheLineSize;                     //Offset 523              // Thunderbolt PCI offset 0x0C Cache Line Size
  
  //
  // Misc
  // Offset(524:1)
  //
  UINT8         SetupVariableInvalid;                 //Offset 524

  //
  // Automatical Thermal Reporting, add to sync demo bios's behavior.
  //
  UINT8         AutoThermalReporting;                 //Offset 525             // Only 1 bit is used.
  UINT8         TBTSecurityLevel;                     //Offset 526
  UINT8         TBTGpio3;                             //Offset 527
  //
  // Reserved for Chipset Area
  // Offset(528:2)
  //

  UINT8         ChipsetRSV1;                           //Offset 528
  UINT8         AzaliaDockSupport;                     //Offset 529

//Chipset_End
//----------------------------------------------------------------------------------------------------------
// End of area for Chipset team use.
//----------------------------------------------------------------------------------------------------------

#endif