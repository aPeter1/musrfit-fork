# Form implementation generated from reading ui file 'MuSRFit.ui'
#
# Created: Fri Sep 4 16:24:16 2009
#      by: The PerlQt User Interface Compiler (puic)
#
# WARNING! All changes made in this file will be lost!


use strict;
use utf8;


package MuSRFitform;
use Qt;
use Qt::isa qw(Qt::MainWindow);
use Qt::slots
    fileOpen => [],
    fileSave => [],
    fileChangeDir => [],
    filePrint => [],
    fileExit => [],
    editUndo => [],
    editRedo => [],
    editCut => [],
    editCopy => [],
    editPaste => [],
    helpIndex => [],
    helpContents => [],
    helpAbout => [],
    CreateAllInput => [],
    CallMSRCreate => [],
    UpdateMSRFileInitTable => [],
    ActivateT0Hists => [],
    ActivateShComp => [],
    InitializeTab => [],
    TabChanged => [],
    GoFit => [],
    GoPlot => [],
    ShowMuSRT0 => [],
    T0Update => [],
    RunSelectionToggle => [],
    fileBrowse => [];
use Qt::attributes qw(
    musrfit_tabs
    RUNSPage
    groupTitle
    TITLELabel
    TITLE
    FILENAMELabel
    FILENAME
    FitAsyTypeLabel
    FitAsyType
    LRBFLabel
    LRBF
    groupBox7
    FitType2
    TfsLabel
    FitType1
    BINS
    Comp3Label
    Tis
    Tfs
    Comp1Label
    BINSLabel
    TisLabel
    FitType3
    Comp2Label
    RUNSAuto
    RunNumbers
    BeamLineLabel
    BeamLine
    YEAR
    YEARLabel
    RUNSMan
    RunFiles
    Browse
    SharingPahe
    buttonGroupSharing
    SharingComp1
    ShParam_1_1
    ShParam_1_2
    ShParam_1_3
    ShParam_1_4
    ShParam_1_5
    ShParam_1_6
    ShParam_1_7
    ShParam_1_8
    ShParam_1_9
    Comp1ShLabel
    SharingComp2
    Comp2ShLabel
    ShParam_2_1
    ShParam_2_2
    ShParam_2_3
    ShParam_2_4
    ShParam_2_5
    ShParam_2_6
    ShParam_2_7
    ShParam_2_8
    ShParam_2_9
    SharingComp3
    Comp3ShLabel
    ShParam_3_1
    ShParam_3_2
    ShParam_3_3
    ShParam_3_4
    ShParam_3_5
    ShParam_3_6
    ShParam_3_7
    ShParam_3_8
    ShParam_3_9
    InitializationPage
    InitParamTable
    FittingPage
    textMSROutput
    TabPage
    Minimization
    MINIMIZE
    MIGRAD
    SIMPLEX
    ErrorCalc
    HESSE
    MINOS
    go
    PlotMSR
    FitTextOutput
    TabPage_2
    FUnitsLabel
    FApodizationLabel
    FPlotLabel
    FUnits
    FApodization
    FPlot
    textLabel1
    textLabel1_3
    textLabel1_3_5
    lineEdit28
    textLabel1_3_2
    textLabel1_3_6
    textLabel1_2
    textLabel1_3_3
    textLabel1_3_7
    lineEdit28_2
    textLabel1_3_4
    textLabel1_3_8
    TabPage_3
    groupHist0
    textLabel2
    textLabel2_2_2_3
    textLabel2_2_2
    textLabel2_2_2_2
    textLabel2_2_2_2_2
    groupHist1
    t01
    Bg11
    Bg21
    Data11
    Data21
    groupHist2
    t02
    Bg12
    Bg22
    Data12
    Data22
    groupHist3
    t03
    Bg13
    Bg23
    Data13
    Data23
    groupHist4
    t04
    Bg14
    Bg24
    Data14
    Data24
    ShowT0
    MenuBar
    fileMenu
    editMenu
    Options
    helpMenu
    toolBar
    fileNewAction
    fileOpenAction
    fileSaveAction
    fileSaveAsAction
    filePrintAction
    fileExitAction
    editUndoAction
    editRedoAction
    editCutAction
    editCopyAction
    editPasteAction
    editFindAction
    helpContentsAction
    helpIndexAction
    helpAboutAction
    Action
    separatorAction
    FileExistCheck
    ManualFile
    fileChangeDirAction
    Action_2
    Action_3
    optionsnew_itemAction
);


our $image0;
our $image0_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xd3, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xb5, 0x94, 0x51, 0x0e, 0x84,
    0x20, 0x0c, 0x44, 0xa7, 0x86, 0x4b, 0xd9, 0xfb, 0x67, 0x3c, 0x16, 0xfe,
    0x50, 0xb7, 0xa9, 0x55, 0x01, 0xdd, 0x97, 0x10, 0x49, 0x83, 0x43, 0x5b,
    0x06, 0x84, 0x24, 0x22, 0xab, 0x6a, 0xb5, 0xf9, 0x46, 0xca, 0x69, 0x41,
    0x07, 0x4b, 0x0c, 0xa8, 0x13, 0xcd, 0x36, 0xea, 0xa5, 0xf8, 0x9f, 0x05,
    0x00, 0x49, 0x81, 0x13, 0x52, 0xd5, 0x3a, 0xac, 0x8a, 0x96, 0xb1, 0x65,
    0x64, 0x02, 0x56, 0xfe, 0x46, 0x8a, 0xc5, 0x46, 0xb3, 0x3e, 0xb5, 0xc2,
    0x98, 0xed, 0xed, 0xa3, 0xf0, 0x5b, 0x16, 0x55, 0xad, 0xbe, 0xf4, 0xb8,
    0xc0, 0x62, 0xa3, 0xe9, 0x2f, 0xbc, 0x11, 0x7d, 0x83, 0x64, 0x3e, 0xbe,
    0x62, 0x75, 0xd5, 0x3d, 0xd1, 0xdd, 0x63, 0x73, 0x45, 0xaf, 0x3b, 0x3e,
    0x3b, 0xbc, 0xb8, 0xe1, 0x94, 0x70, 0x76, 0x3b, 0x23, 0xdd, 0xc2, 0xde,
    0x39, 0x24, 0xc5, 0x8b, 0x67, 0x6d, 0x2a, 0x67, 0x89, 0x7b, 0x71, 0x7f,
    0x4b, 0x25, 0xdc, 0xd8, 0x48, 0xf5, 0xa3, 0xb9, 0xe4, 0xf8, 0xc6, 0x51,
    0xdd, 0x20, 0x09, 0x92, 0xc8, 0x62, 0x05, 0x68, 0x0f, 0x4f, 0x82, 0xc5,
    0x55, 0xb5, 0x1e, 0x6b, 0x92, 0xfe, 0x5a, 0x25, 0x02, 0xc0, 0xcc, 0x3b,
    0xd4, 0x0a, 0x13, 0xb9, 0x8a, 0x13, 0x3f, 0xaf, 0x17, 0xcb, 0x28, 0x66,
    0x39, 0x83, 0x3f, 0xc4, 0xdb, 0x56, 0xbc, 0xe1, 0x7f, 0xaf, 0xdb, 0x97,
    0x62, 0xde, 0xeb, 0x43, 0x8f, 0xd0, 0x08, 0x3b, 0xbf, 0x32, 0x98, 0xdf,
    0x1b, 0xba, 0x5b, 0x87, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82;

our $image1;
our $image1_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x7f, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xed, 0x95, 0x51, 0x0a, 0x00,
    0x21, 0x08, 0x44, 0x75, 0xd9, 0xdb, 0x04, 0x1e, 0xc0, 0xe8, 0xfa, 0x5e,
    0xcb, 0xfd, 0xda, 0x85, 0x2d, 0x4b, 0x11, 0xfa, 0x6b, 0x20, 0x28, 0xa3,
    0x57, 0x0c, 0x53, 0xa1, 0x88, 0xc0, 0x0e, 0xdd, 0x7d, 0x81, 0x99, 0x35,
    0xb2, 0x50, 0x44, 0x70, 0x35, 0x7f, 0x59, 0x45, 0x55, 0x9d, 0x36, 0x00,
    0x00, 0x22, 0x72, 0x0f, 0x60, 0x82, 0x3d, 0x11, 0x91, 0x0b, 0x1f, 0xac,
    0x88, 0xa8, 0x94, 0xf2, 0xf5, 0x99, 0x59, 0x2d, 0x5b, 0x52, 0xe0, 0x5a,
    0xeb, 0x6f, 0x6c, 0xc1, 0x53, 0xe0, 0xd6, 0xda, 0x50, 0xeb, 0xe1, 0x29,
    0x30, 0xe2, 0x32, 0x10, 0x39, 0xf0, 0x9b, 0x0c, 0x6f, 0xb3, 0x54, 0x2a,
    0x22, 0x3a, 0xe0, 0x03, 0x3e, 0xe0, 0x85, 0xcc, 0x2b, 0x1d, 0x79, 0x0b,
    0x3c, 0xe1, 0xae, 0x3f, 0x6f, 0x9b, 0x15, 0x0f, 0x6d, 0x54, 0x33, 0xaf,
    0x47, 0xa4, 0xf4, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82;

our $image2;
our $image2_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xae, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xb5, 0x94, 0x51, 0x0e, 0xc3,
    0x20, 0x0c, 0x43, 0x6d, 0xb4, 0x53, 0x71, 0xb6, 0x69, 0x1f, 0xd3, 0xce,
    0xe6, 0x6b, 0xb1, 0x8f, 0x51, 0x35, 0xa2, 0x19, 0x23, 0x5a, 0xf0, 0x4f,
    0x44, 0x2b, 0x5e, 0x2c, 0x03, 0xa1, 0x24, 0xec, 0x50, 0xd9, 0x42, 0xdd,
    0x09, 0xbe, 0x45, 0x37, 0xd4, 0x5a, 0x9b, 0x5d, 0x4b, 0xe2, 0xf8, 0x5f,
    0x12, 0x43, 0xe0, 0x63, 0x93, 0xd7, 0x48, 0x12, 0x6d, 0x53, 0x66, 0x1d,
    0x9e, 0x85, 0x4a, 0x62, 0x4a, 0xc6, 0x63, 0x3c, 0x40, 0xcf, 0xf8, 0x57,
    0x6e, 0x51, 0x28, 0x00, 0x10, 0x40, 0x6b, 0x0d, 0x00, 0xda, 0xb1, 0x04,
    0xb9, 0xc6, 0x9d, 0x19, 0xe8, 0x87, 0x77, 0x42, 0x01, 0xf6, 0x4e, 0xbc,
    0x7c, 0x8f, 0x34, 0x2f, 0xc6, 0x78, 0xb0, 0xce, 0x55, 0x3c, 0xc7, 0x6b,
    0x75, 0x2e, 0x37, 0xe3, 0x68, 0xd6, 0x9e, 0xdc, 0x8c, 0x2d, 0xf4, 0xf5,
    0x8c, 0x01, 0xef, 0x8f, 0x4f, 0x75, 0x33, 0xfe, 0x17, 0x6a, 0x1e, 0x48,
    0x2e, 0xf4, 0xe2, 0x38, 0x0b, 0x6a, 0xc0, 0xb9, 0xd0, 0xd3, 0x6a, 0x57,
    0x16, 0x14, 0x30, 0x83, 0x3e, 0x13, 0x0a, 0xac, 0xde, 0xf6, 0x2f, 0x9a,
    0xcd, 0x8a, 0xb4, 0x79, 0x3c, 0xea, 0x0d, 0x05, 0x19, 0x70, 0xcd, 0xbc,
    0x4f, 0x01, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
    0x42, 0x60, 0x82;

our $image3;
our $image3_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xad, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xd5, 0x95, 0x31, 0x0e, 0x84,
    0x20, 0x10, 0x45, 0x3f, 0x86, 0x86, 0xc4, 0x82, 0xf3, 0xd0, 0xcc, 0xc5,
    0x8c, 0x85, 0xf1, 0x62, 0x34, 0x73, 0x1e, 0x0b, 0x92, 0x2d, 0x77, 0x0b,
    0x33, 0x86, 0x15, 0x0d, 0x62, 0xd0, 0x84, 0xd7, 0x10, 0x0c, 0xbe, 0x7c,
    0xc6, 0x41, 0x14, 0x33, 0xe3, 0x09, 0x34, 0x00, 0x38, 0xe7, 0xbe, 0x35,
    0xa5, 0xcc, 0xac, 0xb4, 0x4c, 0xe6, 0x69, 0x1d, 0x87, 0x11, 0xf0, 0xde,
    0x17, 0x89, 0x88, 0x68, 0x7b, 0x87, 0x88, 0x00, 0x00, 0x5d, 0xa5, 0x90,
    0x09, 0x3a, 0xbf, 0xe4, 0x18, 0x13, 0x2c, 0x3e, 0xfd, 0xb2, 0xcd, 0x25,
    0xa9, 0x90, 0x4d, 0x6c, 0x82, 0x85, 0x09, 0x36, 0x79, 0x1e, 0x4b, 0x81,
    0xb5, 0x94, 0x52, 0xce, 0x4b, 0xe2, 0xbb, 0x64, 0x4b, 0xb1, 0x4f, 0x76,
    0x95, 0xc7, 0x12, 0xb7, 0x27, 0x4e, 0x6a, 0x3c, 0x4f, 0x69, 0xeb, 0xe4,
    0x88, 0xbb, 0xe1, 0x54, 0x7c, 0xb6, 0xb0, 0x94, 0x77, 0xdb, 0x6d, 0x18,
    0xcb, 0x45, 0xfb, 0x5d, 0xde, 0x3e, 0xd2, 0xf1, 0x8f, 0xea, 0xe8, 0x9b,
    0xb4, 0xd7, 0x6e, 0xed, 0x89, 0x95, 0xdc, 0x79, 0xb5, 0xae, 0x27, 0x66,
    0x56, 0x7f, 0xe2, 0xda, 0xfc, 0x00, 0x45, 0x36, 0x2b, 0xc8, 0x71, 0x7a,
    0x5b, 0x49, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42,
    0x60, 0x82;

our $image4;
our $image4_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x02,
    0x8c, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0x8d, 0x95, 0xaf, 0x7b, 0xdb,
    0x30, 0x10, 0x86, 0xdf, 0xec, 0x09, 0x38, 0x31, 0x9b, 0xc5, 0x30, 0xb0,
    0x61, 0x35, 0x0c, 0x2c, 0xf4, 0x58, 0xcb, 0x5a, 0x18, 0xb8, 0x3f, 0x61,
    0x81, 0xa3, 0x63, 0xa3, 0x85, 0x1d, 0xeb, 0xd8, 0x02, 0x0b, 0x1d, 0xd6,
    0xb0, 0x1a, 0x0a, 0xda, 0xcc, 0x62, 0x77, 0x2c, 0x03, 0xb2, 0x1d, 0xe7,
    0xd7, 0xb3, 0x8a, 0x28, 0x96, 0x2e, 0xef, 0x7d, 0xba, 0xef, 0x2c, 0x4f,
    0xca, 0xb2, 0xe4, 0xd2, 0x68, 0xdb, 0x76, 0x2f, 0x22, 0xa4, 0x69, 0x7a,
    0xb4, 0x6e, 0x66, 0x00, 0xa8, 0x2a, 0x00, 0x21, 0x04, 0x92, 0x24, 0xc1,
    0x39, 0x37, 0x19, 0xc7, 0x4d, 0x2f, 0x52, 0x01, 0x11, 0x21, 0xcf, 0xf3,
    0x01, 0x74, 0xba, 0x37, 0x12, 0x80, 0xf7, 0xfe, 0x2c, 0xe6, 0x2a, 0x18,
    0xa0, 0xaa, 0xaa, 0x33, 0xd8, 0x69, 0x22, 0x55, 0xc5, 0x39, 0xf7, 0x79,
    0xf0, 0x25, 0x80, 0x99, 0x9d, 0x25, 0xe8, 0xe7, 0x53, 0xf8, 0x97, 0x2b,
    0xdc, 0xbd, 0x99, 0x0d, 0xc1, 0xce, 0x39, 0x9c, 0x73, 0x47, 0xf5, 0xee,
    0xeb, 0x2f, 0x22, 0x67, 0xf5, 0xbd, 0xaa, 0x58, 0x55, 0x49, 0x92, 0x84,
    0xb6, 0x6d, 0x11, 0x91, 0xc1, 0xa8, 0xf1, 0xb8, 0x74, 0xfc, 0xcf, 0x28,
    0xc6, 0xcc, 0xce, 0x3a, 0x62, 0x0c, 0xed, 0x93, 0x5d, 0x32, 0xf7, 0xaa,
    0xe2, 0x5e, 0xcd, 0x25, 0xa5, 0xe3, 0x75, 0x55, 0x3d, 0xea, 0x90, 0xff,
    0x82, 0xeb, 0xba, 0x66, 0x3e, 0x9f, 0x93, 0x65, 0xd9, 0x55, 0x78, 0x6f,
    0xe4, 0x76, 0xbb, 0x25, 0x84, 0xb0, 0x17, 0x49, 0xc8, 0xb2, 0x74, 0xa8,
    0xf5, 0x11, 0x58, 0x55, 0xf7, 0xde, 0x7b, 0xaa, 0x5d, 0x85, 0x14, 0x82,
    0x88, 0x0c, 0x80, 0xd3, 0xb9, 0x57, 0xda, 0xd6, 0x2d, 0x65, 0x59, 0xd2,
    0x84, 0x86, 0x9b, 0x9b, 0x9b, 0x7d, 0x51, 0x14, 0xa4, 0x69, 0x3a, 0x99,
    0xf4, 0x6f, 0x9e, 0xaa, 0xee, 0xab, 0xca, 0x13, 0xea, 0x06, 0x35, 0x80,
    0xcb, 0x4a, 0x01, 0x9c, 0xa4, 0x08, 0x02, 0x0e, 0x7c, 0xed, 0x79, 0x7f,
    0xdf, 0xd1, 0x34, 0x35, 0xaa, 0xc6, 0x62, 0x31, 0x67, 0xb5, 0x5a, 0x45,
    0xc5, 0xbd, 0xd2, 0xe2, 0xee, 0x0e, 0x97, 0xa6, 0xa8, 0x1a, 0x98, 0x72,
    0x66, 0x8b, 0x19, 0x20, 0x94, 0x6c, 0x79, 0xe3, 0x2f, 0x4b, 0x5b, 0xb2,
    0xfc, 0xc8, 0xa9, 0xaa, 0x1d, 0xa8, 0xe1, 0x80, 0x8f, 0xf7, 0x0f, 0x7e,
    0xe9, 0x33, 0xd3, 0x08, 0x6d, 0x78, 0xf8, 0xfa, 0x40, 0x32, 0x4b, 0xae,
    0xaa, 0xec, 0x47, 0x45, 0xc5, 0x4f, 0x7e, 0xe0, 0xf1, 0xec, 0xd8, 0xf1,
    0x22, 0x2f, 0x14, 0xfe, 0x9e, 0x97, 0xf6, 0x99, 0xba, 0x0e, 0x18, 0x46,
    0xe3, 0x3d, 0xd3, 0xd0, 0x04, 0x9e, 0x9e, 0xee, 0xaf, 0xba, 0x7b, 0x3a,
    0x52, 0x52, 0xee, 0x58, 0x52, 0x22, 0x14, 0x14, 0xdc, 0xde, 0xde, 0x92,
    0x48, 0x46, 0x6b, 0x35, 0xcf, 0x2f, 0xbf, 0x09, 0x21, 0x90, 0xce, 0x52,
    0xa6, 0x06, 0x78, 0x5f, 0xc7, 0x7e, 0x34, 0x03, 0x11, 0x62, 0x8e, 0x98,
    0x48, 0x04, 0x1c, 0x2e, 0x56, 0xdc, 0x14, 0xc4, 0xf1, 0xcd, 0xbe, 0x73,
    0x8f, 0x27, 0xb3, 0x8c, 0x1d, 0x15, 0x98, 0x91, 0x66, 0x59, 0xdc, 0xef,
    0xdc, 0x99, 0xb6, 0x75, 0x8d, 0x17, 0x89, 0xf5, 0x34, 0x63, 0xf0, 0xcd,
    0x41, 0x74, 0xb1, 0x3b, 0x89, 0x33, 0x50, 0x01, 0xe2, 0x9a, 0x00, 0xde,
    0x1a, 0x8c, 0x78, 0x87, 0xf8, 0xaa, 0x1a, 0x3c, 0x71, 0x74, 0xed, 0xd6,
    0x2f, 0x58, 0x0f, 0xc4, 0x86, 0xa6, 0x50, 0xa2, 0x29, 0xb1, 0x9d, 0x87,
    0xc8, 0xa1, 0x73, 0xa4, 0xf7, 0x74, 0x34, 0x14, 0x65, 0x8a, 0x80, 0xb5,
    0xa3, 0x3f, 0x68, 0x27, 0x76, 0x1c, 0x68, 0x80, 0x28, 0xd8, 0xe9, 0x8e,
    0xc3, 0xac, 0xa5, 0x3b, 0x67, 0x7f, 0x36, 0xf2, 0x7c, 0xc9, 0x17, 0x6c,
    0xa4, 0x72, 0x10, 0xab, 0xf1, 0xd1, 0xba, 0xf9, 0x08, 0xea, 0xfa, 0xb0,
    0x0e, 0xda, 0x01, 0xbb, 0x72, 0xce, 0x66, 0x33, 0x56, 0xab, 0xa7, 0xa8,
    0xd8, 0x7b, 0xdf, 0x5b, 0x05, 0x62, 0x10, 0x04, 0x08, 0xd1, 0x39, 0xb3,
    0x88, 0x91, 0x5e, 0x95, 0x1d, 0x60, 0x1c, 0x6a, 0x8e, 0x19, 0x22, 0xb0,
    0x5e, 0xaf, 0xc9, 0xb2, 0x6c, 0x32, 0x9d, 0xcf, 0xe7, 0xbc, 0x6d, 0xde,
    0xf8, 0xb3, 0xd9, 0xa0, 0x6a, 0xc4, 0xfb, 0x47, 0x38, 0x6a, 0x3e, 0x19,
    0x3d, 0xcb, 0x01, 0x39, 0x9c, 0xb3, 0xeb, 0xa6, 0xf5, 0x7a, 0x4d, 0x9e,
    0xe7, 0x13, 0x80, 0xe1, 0x95, 0xae, 0xeb, 0x7a, 0xff, 0xfa, 0xba, 0xa1,
    0xe9, 0xbe, 0x5f, 0x8a, 0x76, 0xa6, 0x1c, 0x9c, 0x39, 0x7c, 0x35, 0x40,
    0xb5, 0x8d, 0xbf, 0x83, 0x91, 0x2f, 0x73, 0x1e, 0x1f, 0x1f, 0x59, 0x2c,
    0x16, 0xc3, 0x25, 0xf4, 0x0f, 0x01, 0x0f, 0x7c, 0xb0, 0xf1, 0xb5, 0xc3,
    0x04, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60,
    0x82;

our $image5;
our $image5_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x9b, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xed, 0x94, 0x3b, 0x0e, 0x80,
    0x20, 0x0c, 0x86, 0x5b, 0xe3, 0xa9, 0x18, 0x89, 0xc7, 0x32, 0x0e, 0x84,
    0x63, 0x99, 0x8e, 0xbd, 0x16, 0x4e, 0x68, 0x41, 0x79, 0x49, 0x18, 0x4c,
    0xfc, 0xc7, 0xb6, 0x7c, 0x6d, 0xe8, 0x03, 0x99, 0x19, 0x46, 0x68, 0x1a,
    0x42, 0xfd, 0xc1, 0x52, 0x73, 0x4b, 0xb0, 0x52, 0xe4, 0x9e, 0xec, 0xd6,
    0x00, 0xe8, 0x45, 0xa3, 0xb4, 0xa1, 0x9f, 0x0a, 0xa5, 0xc8, 0x31, 0x87,
    0xce, 0x12, 0x30, 0x96, 0x7c, 0x1f, 0x54, 0x1c, 0xc3, 0x63, 0x60, 0x29,
    0x31, 0xed, 0xe4, 0x7c, 0xe5, 0xb7, 0x3f, 0xf6, 0x41, 0x12, 0xca, 0xac,
    0x31, 0x05, 0x95, 0x09, 0xd7, 0xed, 0xb2, 0x3d, 0x36, 0x2f, 0x86, 0xa6,
    0x80, 0x39, 0x65, 0xa7, 0xe2, 0x2d, 0xb4, 0x08, 0xae, 0x6d, 0x5a, 0x33,
    0xb8, 0x07, 0x5e, 0xb5, 0x20, 0x25, 0xb8, 0xf7, 0x5b, 0x73, 0xd9, 0xb0,
    0xe7, 0xba, 0xd1, 0x4e, 0x4e, 0x4e, 0x82, 0xec, 0x49, 0xd7, 0x4a, 0xa7,
    0xa0, 0x00, 0x9d, 0x15, 0xe7, 0xf4, 0xbd, 0xeb, 0xf6, 0x83, 0x4f, 0x1d,
    0x17, 0x25, 0x46, 0xd1, 0x7f, 0x21, 0xbe, 0x92, 0x00, 0x00, 0x00, 0x00,
    0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82;

our $image6;
our $image6_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x93, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xed, 0x94, 0x3d, 0x0e, 0x80,
    0x20, 0x0c, 0x46, 0xa9, 0xf1, 0x54, 0x8c, 0xc4, 0x63, 0x19, 0x07, 0xc2,
    0xb1, 0x48, 0xc7, 0x5e, 0x0b, 0x27, 0xb0, 0x22, 0x60, 0x83, 0xb2, 0xf1,
    0x2d, 0x24, 0xd0, 0x3e, 0xfa, 0x07, 0x40, 0x44, 0x6a, 0x84, 0x96, 0x21,
    0xd4, 0x09, 0xe6, 0x5a, 0xf3, 0x0d, 0xf4, 0x18, 0xf6, 0xa3, 0x6c, 0x4c,
    0x64, 0x40, 0x0a, 0x06, 0x3e, 0x15, 0x5a, 0x63, 0x90, 0x38, 0xd5, 0x2e,
    0xd0, 0x1a, 0x43, 0x3c, 0x4b, 0x11, 0xa3, 0xbf, 0xa0, 0x2d, 0x47, 0xbe,
    0x72, 0xbb, 0x3c, 0xa8, 0x54, 0xe3, 0x98, 0x7e, 0x2b, 0x5d, 0x22, 0x03,
    0x25, 0x58, 0x29, 0xd3, 0xae, 0xe6, 0xb5, 0x22, 0xfd, 0x04, 0xce, 0xe1,
    0xbf, 0x82, 0xdf, 0x1a, 0xdd, 0x05, 0x96, 0x4c, 0x4f, 0x02, 0x3b, 0x2b,
    0x73, 0x92, 0x8e, 0x64, 0x75, 0x8e, 0x9d, 0x55, 0xca, 0x6c, 0xf2, 0x07,
    0x91, 0xeb, 0x56, 0x0a, 0xde, 0x90, 0xda, 0xeb, 0x93, 0x0a, 0xe6, 0x7f,
    0x3c, 0xc1, 0x0f, 0x9d, 0x5b, 0xa7, 0x46, 0x08, 0x5c, 0x06, 0xe1, 0xe5,
    0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82;

our $image7;
our $image7_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0x9d, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xcd, 0xd4, 0x41, 0x0e, 0x80,
    0x20, 0x0c, 0x04, 0xc0, 0xd6, 0xf8, 0xaa, 0x9e, 0x79, 0x36, 0xe7, 0xfd,
    0x16, 0x9e, 0x9a, 0x20, 0x22, 0x2c, 0xa0, 0xd1, 0x5e, 0x4c, 0x34, 0x0e,
    0xed, 0xa6, 0x41, 0x01, 0xc8, 0x1b, 0xb5, 0xb5, 0x3e, 0x9a, 0x59, 0x1a,
    0x79, 0x4f, 0xc3, 0x2b, 0xf5, 0x1f, 0x98, 0x89, 0x81, 0x82, 0x6b, 0x10,
    0x00, 0x5d, 0x82, 0x19, 0x60, 0x0a, 0xf6, 0xf2, 0xae, 0xd9, 0x18, 0x44,
    0x44, 0x94, 0xd9, 0xe3, 0x1c, 0x64, 0xa7, 0xf8, 0x76, 0x2b, 0xbc, 0xcb,
    0x91, 0xcc, 0xc9, 0x8c, 0x63, 0xca, 0x9f, 0x8f, 0xc0, 0x8e, 0x79, 0xb7,
    0x66, 0x31, 0x31, 0x07, 0x74, 0xee, 0x8a, 0x98, 0x80, 0x70, 0x1a, 0x1f,
    0x08, 0x0a, 0x04, 0xed, 0xe1, 0xb7, 0x70, 0x0d, 0x2d, 0x0f, 0x68, 0xe1,
    0x55, 0xb8, 0x87, 0x32, 0xf8, 0x05, 0x66, 0xd1, 0x5e, 0x0d, 0xed, 0x31,
    0x93, 0xed, 0x14, 0x5c, 0x56, 0x6b, 0xba, 0x7d, 0xf4, 0xe7, 0xbc, 0xeb,
    0x56, 0x64, 0xd5, 0xbb, 0x22, 0x1f, 0x77, 0x36, 0x6f, 0xea, 0x12, 0x9a,
    0xa9, 0x03, 0xbb, 0x25, 0x61, 0xad, 0xb1, 0xd6, 0xf6, 0xe7, 0x00, 0x00,
    0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82;

our $image8;
our $image8_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xd3, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xb5, 0x95, 0xb1, 0x0e, 0x83,
    0x30, 0x0c, 0x44, 0xcf, 0x88, 0xaf, 0x8a, 0x18, 0xd9, 0xf9, 0x19, 0x36,
    0xd4, 0x85, 0xaa, 0x7f, 0xd5, 0xd9, 0x9f, 0xd5, 0xeb, 0x50, 0x99, 0x46,
    0x10, 0x93, 0x10, 0x94, 0x93, 0xbc, 0x70, 0xc9, 0xf3, 0xc9, 0x49, 0x84,
    0xa8, 0x2a, 0x5a, 0xa8, 0x6b, 0x42, 0x05, 0x20, 0x00, 0x78, 0xb6, 0x40,
    0x55, 0xa5, 0x1a, 0x4c, 0xa6, 0xd9, 0x22, 0x52, 0x0d, 0xef, 0xf7, 0x10,
    0x93, 0x35, 0x23, 0x09, 0x11, 0xe1, 0x55, 0xf8, 0x06, 0xf6, 0x52, 0xd7,
    0xc2, 0xb7, 0x51, 0xa4, 0x12, 0xff, 0xbe, 0xad, 0xa7, 0x00, 0xd5, 0x31,
    0xd9, 0xac, 0x60, 0xc6, 0x2b, 0xc8, 0x87, 0xe3, 0x3f, 0x5d, 0x78, 0xf7,
    0x5f, 0x24, 0x87, 0x8a, 0x1b, 0xee, 0x3d, 0xd3, 0x30, 0x2c, 0x08, 0xe1,
    0x7d, 0x48, 0xb6, 0x81, 0x49, 0x1e, 0x2a, 0x96, 0xe7, 0x4d, 0x53, 0x1a,
    0xee, 0xde, 0x0a, 0x83, 0x79, 0xbe, 0x79, 0xf3, 0xdc, 0x01, 0xf8, 0x00,
    0x58, 0x10, 0xc2, 0x8b, 0x36, 0x96, 0xa2, 0x5b, 0x71, 0xe6, 0xdb, 0x9c,
    0x4d, 0x21, 0xbc, 0xa9, 0x3a, 0xca, 0xad, 0xc4, 0xa9, 0x43, 0xb5, 0x46,
    0xb7, 0x13, 0x7b, 0xaa, 0x4e, 0x9c, 0x6b, 0xde, 0x2c, 0x71, 0x67, 0x69,
    0xbc, 0x8a, 0x13, 0xe7, 0x2a, 0x56, 0x9f, 0x7b, 0xff, 0x76, 0x3f, 0xab,
    0x67, 0x9c, 0x53, 0xf5, 0x8c, 0xaf, 0x6c, 0x2a, 0x51, 0x11, 0x78, 0xff,
    0x08, 0x8a, 0xf6, 0xb4, 0xfa, 0x99, 0x7e, 0x01, 0xdf, 0xa4, 0x98, 0x7b,
    0x8b, 0x94, 0x35, 0x47, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44,
    0xae, 0x42, 0x60, 0x82;

our $image9;
our $image9_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x00,
    0xec, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xb5, 0x95, 0x4d, 0x0e, 0x83,
    0x20, 0x10, 0x85, 0xdf, 0x98, 0x9e, 0x6a, 0x12, 0x37, 0xbd, 0x96, 0xc4,
    0x45, 0x83, 0xd7, 0xea, 0xa6, 0xc9, 0x5c, 0x6b, 0xba, 0x40, 0x2d, 0xc8,
    0x50, 0xd0, 0xd2, 0x97, 0x98, 0x09, 0x3f, 0xf3, 0xcd, 0x13, 0x04, 0x49,
    0x44, 0xf0, 0x0f, 0xdd, 0xbe, 0x0d, 0x32, 0xb3, 0x1e, 0xfb, 0x44, 0x84,
    0x5a, 0xc0, 0xb4, 0x39, 0xb6, 0x20, 0x00, 0xa0, 0x51, 0x2f, 0x15, 0x90,
    0x56, 0xb1, 0xa1, 0x15, 0x6a, 0xb5, 0x37, 0x59, 0xf9, 0x04, 0x40, 0x01,
    0xc0, 0x3f, 0x3c, 0x00, 0x07, 0xc0, 0xc3, 0xcd, 0x6e, 0x07, 0xc5, 0x2e,
    0xe3, 0xb6, 0x35, 0x3f, 0x76, 0x4e, 0x00, 0xf4, 0x38, 0x49, 0x5e, 0xf5,
    0x0d, 0xe5, 0x91, 0x33, 0x78, 0x0c, 0x1e, 0x42, 0x08, 0x83, 0x21, 0x9e,
    0x51, 0x39, 0x6f, 0xfd, 0x2a, 0x7c, 0x32, 0x89, 0x47, 0x6e, 0x04, 0xa7,
    0x79, 0x06, 0xf8, 0x33, 0x18, 0xbf, 0xde, 0xb9, 0x98, 0x6a, 0xb0, 0x2b,
    0x5f, 0x89, 0xa9, 0xd6, 0xcd, 0x2b, 0xc3, 0xdd, 0x9c, 0x27, 0xd9, 0x4a,
    0x37, 0xcf, 0x5c, 0xe3, 0xa3, 0x13, 0xd5, 0xa9, 0x88, 0x23, 0x5a, 0xa0,
    0x3a, 0x65, 0x87, 0x27, 0x5b, 0xe3, 0x12, 0x9c, 0x68, 0xc9, 0xa0, 0x71,
    0xc1, 0x00, 0x5f, 0x54, 0xe4, 0x4e, 0xcd, 0x8e, 0x6b, 0xae, 0x2d, 0x78,
    0x37, 0xc7, 0x47, 0x78, 0x17, 0xc7, 0x56, 0xd1, 0x9f, 0x1d, 0x5b, 0x05,
    0x89, 0x96, 0xbe, 0x6b, 0xdc, 0xcd, 0xb1, 0xa5, 0xdd, 0xc0, 0x7a, 0xd1,
    0x6b, 0xf9, 0xf1, 0x7a, 0x56, 0x80, 0x57, 0xaa, 0xfd, 0xf3, 0x98, 0x9f,
    0x85, 0xeb, 0xfd, 0xbb, 0xaa, 0xe0, 0xab, 0x7a, 0x03, 0x0d, 0x36, 0xe3,
    0xe7, 0x4e, 0x20, 0x86, 0xd1, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e,
    0x44, 0xae, 0x42, 0x60, 0x82;

our $image10;
our $image10_data = pack 'C*',
0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d,
    0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x16,
    0x08, 0x06, 0x00, 0x00, 0x00, 0xc4, 0xb4, 0x6c, 0x3b, 0x00, 0x00, 0x02,
    0x59, 0x49, 0x44, 0x41, 0x54, 0x38, 0x8d, 0xb5, 0x95, 0x21, 0x93, 0x1b,
    0x39, 0x10, 0x46, 0x9f, 0xb7, 0x02, 0x7a, 0x98, 0xc4, 0x2c, 0x28, 0x38,
    0xcb, 0x6c, 0xb6, 0x86, 0x03, 0x1d, 0x68, 0x96, 0x85, 0xf9, 0x5d, 0x86,
    0x81, 0x6b, 0x66, 0xc3, 0x85, 0x03, 0x3d, 0xcc, 0x03, 0x05, 0x35, 0x4c,
    0x62, 0xdd, 0x6c, 0x02, 0x1c, 0x57, 0x6d, 0x2e, 0x97, 0xf3, 0xa6, 0x72,
    0xfb, 0x21, 0xa9, 0x4b, 0xf5, 0xf4, 0x49, 0xad, 0x6e, 0x2d, 0xfa, 0xbe,
    0xe7, 0x23, 0xf4, 0xf0, 0x21, 0x54, 0xe0, 0xd3, 0xdb, 0x49, 0x29, 0x65,
    0x4e, 0x29, 0x91, 0x73, 0xa6, 0x9a, 0x01, 0xe0, 0x44, 0x88, 0x31, 0x12,
    0x63, 0xa4, 0x69, 0x9a, 0xc5, 0x1f, 0x83, 0x73, 0xce, 0x73, 0x7f, 0x3e,
    0xe3, 0x63, 0x60, 0xbd, 0xfb, 0x0c, 0x4e, 0xae, 0xf1, 0x94, 0x18, 0x5e,
    0xcf, 0xa4, 0x94, 0xd8, 0x6c, 0x36, 0xb3, 0xf7, 0xfe, 0x5d, 0xf0, 0x45,
    0xdf, 0xf7, 0xe4, 0x9c, 0xe7, 0x97, 0x97, 0x17, 0x36, 0x9f, 0x3b, 0x36,
    0x5f, 0x76, 0x00, 0x98, 0x19, 0x6a, 0x8a, 0x61, 0x54, 0xab, 0xf4, 0xdf,
    0x8e, 0x48, 0x36, 0xb6, 0xdb, 0xed, 0xbb, 0x9c, 0x3f, 0xa8, 0xea, 0xdc,
    0x9f, 0x7b, 0xba, 0xaf, 0xcf, 0xb4, 0xdb, 0x0e, 0x33, 0xc3, 0x92, 0xa0,
    0x69, 0x09, 0x63, 0xa0, 0xf6, 0x42, 0xc9, 0x85, 0x76, 0xdb, 0x41, 0xf0,
    0x0c, 0xc3, 0xf0, 0xae, 0xab, 0x78, 0x48, 0x29, 0xe1, 0x63, 0x64, 0xd3,
    0x75, 0x00, 0x48, 0x5e, 0x52, 0x52, 0x66, 0xb2, 0x81, 0x49, 0x46, 0xc4,
    0x3c, 0xd2, 0x47, 0xac, 0x56, 0xe2, 0xa6, 0x65, 0x4c, 0x89, 0x52, 0xca,
    0x7c, 0x1f, 0x3c, 0x4d, 0xb4, 0x9b, 0x35, 0x86, 0x61, 0x05, 0xd2, 0x98,
    0x48, 0x7e, 0x44, 0x43, 0x21, 0x93, 0x30, 0x29, 0x34, 0x08, 0x96, 0x0d,
    0x44, 0x90, 0xe0, 0xc8, 0x39, 0xdf, 0x77, 0x6c, 0xaa, 0x38, 0xef, 0x28,
    0x56, 0xc8, 0xcd, 0x44, 0x59, 0x4f, 0x88, 0x13, 0xac, 0x18, 0x8e, 0x08,
    0x78, 0x52, 0x1d, 0x51, 0xa7, 0x60, 0x20, 0xae, 0x61, 0x9a, 0xa6, 0xbb,
    0xe0, 0x4f, 0x00, 0x55, 0x0d, 0x11, 0x40, 0x15, 0x03, 0x30, 0x43, 0x72,
    0xa4, 0xb1, 0x40, 0x4e, 0x3d, 0xda, 0x26, 0xc0, 0x63, 0xa5, 0x02, 0x02,
    0xd7, 0x55, 0xff, 0xed, 0x18, 0x0c, 0xcc, 0xae, 0x49, 0xb3, 0xdb, 0x18,
    0x1a, 0x13, 0xf2, 0x78, 0xa6, 0xb4, 0x19, 0x82, 0x80, 0x29, 0x26, 0x60,
    0xb5, 0x20, 0x22, 0xf7, 0xc1, 0xc1, 0x07, 0x52, 0x4a, 0x98, 0x5e, 0x81,
    0x56, 0x0d, 0xc3, 0xc8, 0x96, 0xb0, 0x50, 0x31, 0xc0, 0xca, 0x35, 0xa6,
    0x55, 0x29, 0xa5, 0x12, 0x63, 0xbc, 0x0f, 0x8e, 0x31, 0x92, 0xfb, 0x0b,
    0x5a, 0x15, 0xb3, 0x7a, 0x05, 0x14, 0x43, 0xcc, 0x43, 0x12, 0x6a, 0xad,
    0xfc, 0x38, 0x08, 0x35, 0x4f, 0x04, 0x04, 0xe7, 0xdc, 0x3b, 0x1c, 0x87,
    0xb0, 0x08, 0xde, 0x33, 0xbc, 0xbe, 0xa2, 0x80, 0x2a, 0x80, 0x92, 0x38,
    0x53, 0xdc, 0x88, 0x13, 0xa1, 0x94, 0x4a, 0x29, 0x15, 0x1b, 0x12, 0x6d,
    0xdb, 0xe2, 0x83, 0x27, 0xe7, 0x3c, 0xab, 0xea, 0x6f, 0x9f, 0xdd, 0x03,
    0xc0, 0x6a, 0xb5, 0xc2, 0x15, 0xa8, 0xc7, 0x33, 0x8d, 0x29, 0x8d, 0x35,
    0xf8, 0x65, 0x83, 0x44, 0x87, 0x18, 0x04, 0x03, 0x5e, 0x2f, 0x3c, 0x3d,
    0x3d, 0xf1, 0xf8, 0xf8, 0xb8, 0x48, 0x63, 0xe2, 0x74, 0x3a, 0xb1, 0xdf,
    0xef, 0xc9, 0x39, 0xff, 0x2b, 0x7c, 0x71, 0x6b, 0x9b, 0xaa, 0x3a, 0x0f,
    0xc3, 0xc0, 0x98, 0x12, 0xe2, 0x04, 0x9a, 0x6b, 0x82, 0xac, 0x54, 0xc4,
    0xa0, 0xeb, 0x3a, 0x42, 0x08, 0xb7, 0x52, 0x9e, 0x8f, 0xc7, 0x23, 0x87,
    0xc3, 0x01, 0xe7, 0x1c, 0xbb, 0xdd, 0x8e, 0xd5, 0x6a, 0xf5, 0x53, 0xa9,
    0x2f, 0xfe, 0xd9, 0x8f, 0x4b, 0x29, 0x73, 0xce, 0x99, 0x5a, 0x2b, 0x22,
    0x82, 0xf7, 0xfe, 0x96, 0xac, 0x5f, 0xfa, 0x43, 0x4a, 0x69, 0xfe, 0xe1,
    0x9a, 0xae, 0xeb, 0x7e, 0xda, 0xfc, 0x17, 0xf0, 0x9f, 0x4a, 0x55, 0xe7,
    0xd3, 0xe9, 0xc4, 0xe1, 0x70, 0x60, 0xb9, 0x5c, 0xf2, 0xfc, 0xfc, 0xcc,
    0x7a, 0xbd, 0x5e, 0xfc, 0x35, 0xf8, 0xa6, 0xcb, 0xe5, 0x32, 0xef, 0xf7,
    0x7b, 0xa6, 0x69, 0x62, 0xbb, 0xdd, 0xfe, 0xbd, 0xe3, 0xb7, 0x2a, 0xa5,
    0xcc, 0x87, 0xc3, 0x01, 0x55, 0xfd, 0x7f, 0xc1, 0x6f, 0xf5, 0x61, 0x7f,
    0xde, 0x77, 0x3b, 0x42, 0x7a, 0x02, 0x0f, 0x23, 0xb1, 0x5a, 0x00, 0x00,
    0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82;


sub NEW
{
    shift->SUPER::NEW(@_[0..2]);
    statusBar();

    $image0 = Qt::Pixmap();
    $image0->loadFromData($image0_data, length ($image0_data), "PNG");
    $image1 = Qt::Pixmap();
    $image1->loadFromData($image1_data, length ($image1_data), "PNG");
    $image2 = Qt::Pixmap();
    $image2->loadFromData($image2_data, length ($image2_data), "PNG");
    $image3 = Qt::Pixmap();
    $image3->loadFromData($image3_data, length ($image3_data), "PNG");
    $image4 = Qt::Pixmap();
    $image4->loadFromData($image4_data, length ($image4_data), "PNG");
    $image5 = Qt::Pixmap();
    $image5->loadFromData($image5_data, length ($image5_data), "PNG");
    $image6 = Qt::Pixmap();
    $image6->loadFromData($image6_data, length ($image6_data), "PNG");
    $image7 = Qt::Pixmap();
    $image7->loadFromData($image7_data, length ($image7_data), "PNG");
    $image8 = Qt::Pixmap();
    $image8->loadFromData($image8_data, length ($image8_data), "PNG");
    $image9 = Qt::Pixmap();
    $image9->loadFromData($image9_data, length ($image9_data), "PNG");
    $image10 = Qt::Pixmap();
    $image10->loadFromData($image10_data, length ($image10_data), "PNG");

    if ( name() eq "unnamed" )
    {
        setName("MuSRFitform" );
    }
    setSizePolicy(Qt::SizePolicy(3, 3, 1, 1, this->sizePolicy()->hasHeightForWidth()) );
    setMinimumSize(Qt::Size(21, 227) );
    setIcon($image0 );

    setCentralWidget(Qt::Widget(this, "qt_central_widget"));

    musrfit_tabs = Qt::TabWidget(centralWidget(), "musrfit_tabs");
    musrfit_tabs->setEnabled( 1 );
    musrfit_tabs->setGeometry( Qt::Rect(10, 10, 560, 485) );
    musrfit_tabs->setSizePolicy( Qt::SizePolicy(7, 7, 1, 1, musrfit_tabs->sizePolicy()->hasHeightForWidth()) );
    musrfit_tabs->setMinimumSize( Qt::Size(560, 400) );
    musrfit_tabs->setMaximumSize( Qt::Size(95, 32767) );

    RUNSPage = Qt::Widget(musrfit_tabs, "RUNSPage");

    groupTitle = Qt::GroupBox(RUNSPage, "groupTitle");
    groupTitle->setGeometry( Qt::Rect(11, 9, 538, 113) );
    groupTitle->setMargin( int(5) );

    my $LayoutWidget = Qt::Widget(groupTitle, '$LayoutWidget');
    $LayoutWidget->setGeometry( Qt::Rect(6, 8, 530, 100) );
    my $layout25 = Qt::VBoxLayout($LayoutWidget, 11, 6, '$layout25');

    TITLELabel = Qt::Label($LayoutWidget, "TITLELabel");
    TITLELabel->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, TITLELabel->sizePolicy()->hasHeightForWidth()) );
    $layout25->addWidget(TITLELabel);

    TITLE = Qt::LineEdit($LayoutWidget, "TITLE");
    TITLE->setSizePolicy( Qt::SizePolicy(5, 0, 1, 1, TITLE->sizePolicy()->hasHeightForWidth()) );
    $layout25->addWidget(TITLE);

    my $layout17 = Qt::HBoxLayout(undef, 0, 6, '$layout17');

    FILENAMELabel = Qt::Label($LayoutWidget, "FILENAMELabel");
    $layout17->addWidget(FILENAMELabel);

    FILENAME = Qt::LineEdit($LayoutWidget, "FILENAME");
    FILENAME->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, FILENAME->sizePolicy()->hasHeightForWidth()) );
    $layout17->addWidget(FILENAME);
    $layout25->addLayout($layout17);

    my $LayoutWidget_2 = Qt::Widget(RUNSPage, '$LayoutWidget_2');
    $LayoutWidget_2->setGeometry( Qt::Rect(21, 385, 515, 55) );
    my $layout23 = Qt::HBoxLayout($LayoutWidget_2, 11, 6, '$layout23');

    FitAsyTypeLabel = Qt::Label($LayoutWidget_2, "FitAsyTypeLabel");
    FitAsyTypeLabel->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, FitAsyTypeLabel->sizePolicy()->hasHeightForWidth()) );
    $layout23->addWidget(FitAsyTypeLabel);

    FitAsyType = Qt::ComboBox(0, $LayoutWidget_2, "FitAsyType");
    FitAsyType->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, FitAsyType->sizePolicy()->hasHeightForWidth()) );
    $layout23->addWidget(FitAsyType);

    LRBFLabel = Qt::Label($LayoutWidget_2, "LRBFLabel");
    LRBFLabel->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, LRBFLabel->sizePolicy()->hasHeightForWidth()) );
    $layout23->addWidget(LRBFLabel);

    LRBF = Qt::LineEdit($LayoutWidget_2, "LRBF");
    LRBF->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, LRBF->sizePolicy()->hasHeightForWidth()) );
    $layout23->addWidget(LRBF);

    groupBox7 = Qt::GroupBox(RUNSPage, "groupBox7");
    groupBox7->setGeometry( Qt::Rect(11, 220, 538, 164) );
    groupBox7->setMargin( int(5) );

    my $LayoutWidget_3 = Qt::Widget(groupBox7, '$LayoutWidget_3');
    $LayoutWidget_3->setGeometry( Qt::Rect(8, 21, 520, 125) );
    my $layout42 = Qt::GridLayout($LayoutWidget_3, 1, 1, 5, 6, '$layout42');

    FitType2 = Qt::ComboBox(0, $LayoutWidget_3, "FitType2");
    FitType2->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, FitType2->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(FitType2, 1, 1);

    TfsLabel = Qt::Label($LayoutWidget_3, "TfsLabel");
    TfsLabel->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, TfsLabel->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(TfsLabel, 2, 1);

    FitType1 = Qt::ComboBox(0, $LayoutWidget_3, "FitType1");
    FitType1->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, FitType1->sizePolicy()->hasHeightForWidth()) );
    FitType1->setSizeLimit( int(20) );
    FitType1->setAutoCompletion( 0 );
    FitType1->setDuplicatesEnabled( 0 );

    $layout42->addWidget(FitType1, 1, 0);

    BINS = Qt::LineEdit($LayoutWidget_3, "BINS");
    BINS->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, BINS->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(BINS, 3, 2);

    Comp3Label = Qt::Label($LayoutWidget_3, "Comp3Label");
    my $pal = Qt::Palette();
    my $cg = Qt::ColorGroup();
    $cg->setColor(&Qt::ColorGroup::Foreground, &black);
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, Qt::Color(242,247,252));
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, &black);
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, &black);
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, &black);
    $cg->setColor(&Qt::ColorGroup::LinkVisited, &black);
    $pal->setActive($cg);
    $cg->setColor(&Qt::ColorGroup::Foreground, &black);
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, &white);
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, &black);
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, &black);
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, Qt::Color(0,0,238));
    $cg->setColor(&Qt::ColorGroup::LinkVisited, Qt::Color(82,24,139));
    $pal->setInactive($cg);
    $cg->setColor(&Qt::ColorGroup::Foreground, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, &white);
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, Qt::Color(0,0,238));
    $cg->setColor(&Qt::ColorGroup::LinkVisited, Qt::Color(82,24,139));
    $pal->setDisabled($cg);
    Comp3Label->setPalette( $pal );

    $layout42->addWidget(Comp3Label, 0, 2);

    Tis = Qt::LineEdit($LayoutWidget_3, "Tis");
    Tis->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, Tis->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(Tis, 3, 0);

    Tfs = Qt::LineEdit($LayoutWidget_3, "Tfs");
    Tfs->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, Tfs->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(Tfs, 3, 1);

    Comp1Label = Qt::Label($LayoutWidget_3, "Comp1Label");
    $cg->setColor(&Qt::ColorGroup::Foreground, &black);
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, Qt::Color(242,247,252));
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, &black);
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, &black);
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, &black);
    $cg->setColor(&Qt::ColorGroup::LinkVisited, &black);
    $pal->setActive($cg);
    $cg->setColor(&Qt::ColorGroup::Foreground, &black);
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, &white);
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, &black);
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, &black);
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, Qt::Color(0,0,238));
    $cg->setColor(&Qt::ColorGroup::LinkVisited, Qt::Color(82,24,139));
    $pal->setInactive($cg);
    $cg->setColor(&Qt::ColorGroup::Foreground, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, &white);
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, Qt::Color(0,0,238));
    $cg->setColor(&Qt::ColorGroup::LinkVisited, Qt::Color(82,24,139));
    $pal->setDisabled($cg);
    Comp1Label->setPalette( $pal );

    $layout42->addWidget(Comp1Label, 0, 0);

    BINSLabel = Qt::Label($LayoutWidget_3, "BINSLabel");
    BINSLabel->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, BINSLabel->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(BINSLabel, 2, 2);

    TisLabel = Qt::Label($LayoutWidget_3, "TisLabel");
    TisLabel->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, TisLabel->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(TisLabel, 2, 0);

    FitType3 = Qt::ComboBox(0, $LayoutWidget_3, "FitType3");
    FitType3->setSizePolicy( Qt::SizePolicy(5, 0, 0, 0, FitType3->sizePolicy()->hasHeightForWidth()) );

    $layout42->addWidget(FitType3, 1, 2);

    Comp2Label = Qt::Label($LayoutWidget_3, "Comp2Label");
    $cg->setColor(&Qt::ColorGroup::Foreground, &black);
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, Qt::Color(242,247,252));
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, &black);
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, &black);
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, &black);
    $cg->setColor(&Qt::ColorGroup::LinkVisited, &black);
    $pal->setActive($cg);
    $cg->setColor(&Qt::ColorGroup::Foreground, &black);
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, &white);
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, &black);
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, &black);
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, Qt::Color(0,0,238));
    $cg->setColor(&Qt::ColorGroup::LinkVisited, Qt::Color(82,24,139));
    $pal->setInactive($cg);
    $cg->setColor(&Qt::ColorGroup::Foreground, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::Button, Qt::Color(230,240,249));
    $cg->setColor(&Qt::ColorGroup::Light, &white);
    $cg->setColor(&Qt::ColorGroup::Midlight, &white);
    $cg->setColor(&Qt::ColorGroup::Dark, Qt::Color(115,120,124));
    $cg->setColor(&Qt::ColorGroup::Mid, Qt::Color(154,160,166));
    $cg->setColor(&Qt::ColorGroup::Text, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::BrightText, &white);
    $cg->setColor(&Qt::ColorGroup::ButtonText, Qt::Color(128,128,128));
    $cg->setColor(&Qt::ColorGroup::Base, &white);
    $cg->setColor(&Qt::ColorGroup::Background, Qt::Color(255,255,127));
    $cg->setColor(&Qt::ColorGroup::Shadow, &black);
    $cg->setColor(&Qt::ColorGroup::Highlight, Qt::Color(0,0,128));
    $cg->setColor(&Qt::ColorGroup::HighlightedText, &white);
    $cg->setColor(&Qt::ColorGroup::Link, Qt::Color(0,0,238));
    $cg->setColor(&Qt::ColorGroup::LinkVisited, Qt::Color(82,24,139));
    $pal->setDisabled($cg);
    Comp2Label->setPalette( $pal );

    $layout42->addWidget(Comp2Label, 0, 1);

    my $LayoutWidget_4 = Qt::Widget(RUNSPage, '$LayoutWidget_4');
    $LayoutWidget_4->setGeometry( Qt::Rect(4, 123, 550, 95) );
    my $layout43 = Qt::VBoxLayout($LayoutWidget_4, 5, 6, '$layout43');

    RUNSAuto = Qt::GroupBox($LayoutWidget_4, "RUNSAuto");
    RUNSAuto->setSizePolicy( Qt::SizePolicy(5, 7, 0, 0, RUNSAuto->sizePolicy()->hasHeightForWidth()) );
    RUNSAuto->setMargin( int(5) );

    RunNumbers = Qt::LineEdit(RUNSAuto, "RunNumbers");
    RunNumbers->setGeometry( Qt::Rect(15, 25, 510, 26) );
    RunNumbers->setSizePolicy( Qt::SizePolicy(0, 0, 0, 0, RunNumbers->sizePolicy()->hasHeightForWidth()) );
    RunNumbers->setMinimumSize( Qt::Size(0, 23) );

    BeamLineLabel = Qt::Label(RUNSAuto, "BeamLineLabel");
    BeamLineLabel->setGeometry( Qt::Rect(22, 55, 99, 26) );
    BeamLineLabel->setMinimumSize( Qt::Size(0, 20) );

    BeamLine = Qt::ComboBox(0, RUNSAuto, "BeamLine");
    BeamLine->setGeometry( Qt::Rect(142, 55, 100, 26) );
    BeamLine->setSizePolicy( Qt::SizePolicy(0, 0, 0, 0, BeamLine->sizePolicy()->hasHeightForWidth()) );
    BeamLine->setMinimumSize( Qt::Size(0, 20) );

    YEAR = Qt::ComboBox(0, RUNSAuto, "YEAR");
    YEAR->setGeometry( Qt::Rect(425, 55, 100, 26) );
    YEAR->setSizePolicy( Qt::SizePolicy(0, 0, 0, 0, YEAR->sizePolicy()->hasHeightForWidth()) );
    YEAR->setMinimumSize( Qt::Size(0, 20) );

    YEARLabel = Qt::Label(RUNSAuto, "YEARLabel");
    YEARLabel->setGeometry( Qt::Rect(361, 55, 55, 26) );
    YEARLabel->setMinimumSize( Qt::Size(0, 20) );
    $layout43->addWidget(RUNSAuto);

    RUNSMan = Qt::GroupBox($LayoutWidget_4, "RUNSMan");
    RUNSMan->setEnabled( 1 );
    RUNSMan->setMargin( int(5) );

    RunFiles = Qt::LineEdit(RUNSMan, "RunFiles");
    RunFiles->setEnabled( 1 );
    RunFiles->setGeometry( Qt::Rect(20, 25, 505, 26) );
    RunFiles->setSizePolicy( Qt::SizePolicy(0, 0, 0, 0, RunFiles->sizePolicy()->hasHeightForWidth()) );
    RunFiles->setMinimumSize( Qt::Size(0, 23) );

    Browse = Qt::PushButton(RUNSMan, "Browse");
    Browse->setEnabled( 1 );
    Browse->setGeometry( Qt::Rect(425, 55, 100, 26) );
    Browse->setSizePolicy( Qt::SizePolicy(0, 0, 0, 0, Browse->sizePolicy()->hasHeightForWidth()) );
    Browse->setMinimumSize( Qt::Size(0, 20) );
    $layout43->addWidget(RUNSMan);
    musrfit_tabs->insertTab( RUNSPage, "" );

    SharingPahe = Qt::Widget(musrfit_tabs, "SharingPahe");

    buttonGroupSharing = Qt::ButtonGroup(SharingPahe, "buttonGroupSharing");
    buttonGroupSharing->setGeometry( Qt::Rect(5, 5, 545, 435) );
    buttonGroupSharing->setSizePolicy( Qt::SizePolicy(7, 7, 0, 0, buttonGroupSharing->sizePolicy()->hasHeightForWidth()) );
    buttonGroupSharing->setAlignment( int(&Qt::ButtonGroup::AlignLeft) );
    buttonGroupSharing->setCheckable( 0 );

    SharingComp1 = Qt::ButtonGroup(buttonGroupSharing, "SharingComp1");
    SharingComp1->setEnabled( 0 );
    SharingComp1->setGeometry( Qt::Rect(31, 20, 162, 330) );
    SharingComp1->setSizePolicy( Qt::SizePolicy(5, 3, 0, 0, SharingComp1->sizePolicy()->hasHeightForWidth()) );
    SharingComp1->setAlignment( int(&Qt::ButtonGroup::AlignLeft) );

    my $LayoutWidget_5 = Qt::Widget(SharingComp1, '$LayoutWidget_5');
    $LayoutWidget_5->setGeometry( Qt::Rect(40, 60, 81, 266) );
    my $layout14 = Qt::VBoxLayout($LayoutWidget_5, 0, 0, '$layout14');

    ShParam_1_1 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_1");
    $layout14->addWidget(ShParam_1_1);

    ShParam_1_2 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_2");
    $layout14->addWidget(ShParam_1_2);

    ShParam_1_3 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_3");
    $layout14->addWidget(ShParam_1_3);

    ShParam_1_4 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_4");
    $layout14->addWidget(ShParam_1_4);

    ShParam_1_5 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_5");
    ShParam_1_5->setEnabled( 0 );
    ShParam_1_5->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_1_5->setTristate( 0 );
    $layout14->addWidget(ShParam_1_5);

    ShParam_1_6 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_6");
    ShParam_1_6->setEnabled( 0 );
    ShParam_1_6->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_1_6->setTristate( 0 );
    $layout14->addWidget(ShParam_1_6);

    ShParam_1_7 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_7");
    ShParam_1_7->setEnabled( 0 );
    ShParam_1_7->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_1_7->setTristate( 0 );
    $layout14->addWidget(ShParam_1_7);

    ShParam_1_8 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_8");
    ShParam_1_8->setEnabled( 0 );
    ShParam_1_8->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_1_8->setTristate( 0 );
    $layout14->addWidget(ShParam_1_8);

    ShParam_1_9 = Qt::CheckBox($LayoutWidget_5, "ShParam_1_9");
    ShParam_1_9->setEnabled( 0 );
    ShParam_1_9->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_1_9->setTristate( 0 );
    $layout14->addWidget(ShParam_1_9);

    Comp1ShLabel = Qt::Label(SharingComp1, "Comp1ShLabel");
    Comp1ShLabel->setGeometry( Qt::Rect(2, 30, 185, 20) );
    Comp1ShLabel->setPaletteBackgroundColor( Qt::Color(255, 255, 127) );

    SharingComp2 = Qt::ButtonGroup(buttonGroupSharing, "SharingComp2");
    SharingComp2->setEnabled( 0 );
    SharingComp2->setGeometry( Qt::Rect(199, 20, 162, 330) );
    SharingComp2->setSizePolicy( Qt::SizePolicy(5, 3, 0, 0, SharingComp2->sizePolicy()->hasHeightForWidth()) );

    Comp2ShLabel = Qt::Label(SharingComp2, "Comp2ShLabel");
    Comp2ShLabel->setGeometry( Qt::Rect(2, 30, 185, 20) );
    Comp2ShLabel->setPaletteBackgroundColor( Qt::Color(255, 255, 127) );

    my $LayoutWidget_6 = Qt::Widget(SharingComp2, '$LayoutWidget_6');
    $LayoutWidget_6->setGeometry( Qt::Rect(40, 60, 81, 266) );
    my $layout13 = Qt::VBoxLayout($LayoutWidget_6, 0, 0, '$layout13');

    ShParam_2_1 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_1");
    $layout13->addWidget(ShParam_2_1);

    ShParam_2_2 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_2");
    $layout13->addWidget(ShParam_2_2);

    ShParam_2_3 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_3");
    $layout13->addWidget(ShParam_2_3);

    ShParam_2_4 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_4");
    $layout13->addWidget(ShParam_2_4);

    ShParam_2_5 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_5");
    $layout13->addWidget(ShParam_2_5);

    ShParam_2_6 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_6");
    ShParam_2_6->setEnabled( 0 );
    ShParam_2_6->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_2_6->setTristate( 0 );
    $layout13->addWidget(ShParam_2_6);

    ShParam_2_7 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_7");
    ShParam_2_7->setEnabled( 0 );
    ShParam_2_7->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_2_7->setTristate( 0 );
    $layout13->addWidget(ShParam_2_7);

    ShParam_2_8 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_8");
    ShParam_2_8->setEnabled( 0 );
    ShParam_2_8->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_2_8->setTristate( 0 );
    $layout13->addWidget(ShParam_2_8);

    ShParam_2_9 = Qt::CheckBox($LayoutWidget_6, "ShParam_2_9");
    ShParam_2_9->setEnabled( 0 );
    ShParam_2_9->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_2_9->setTristate( 0 );
    $layout13->addWidget(ShParam_2_9);

    SharingComp3 = Qt::ButtonGroup(buttonGroupSharing, "SharingComp3");
    SharingComp3->setEnabled( 0 );
    SharingComp3->setGeometry( Qt::Rect(367, 20, 162, 330) );
    SharingComp3->setSizePolicy( Qt::SizePolicy(5, 3, 0, 0, SharingComp3->sizePolicy()->hasHeightForWidth()) );
    SharingComp3->setAlignment( int(&Qt::ButtonGroup::AlignJustify | &Qt::ButtonGroup::AlignVCenter) );

    Comp3ShLabel = Qt::Label(SharingComp3, "Comp3ShLabel");
    Comp3ShLabel->setGeometry( Qt::Rect(2, 30, 185, 20) );
    Comp3ShLabel->setPaletteBackgroundColor( Qt::Color(255, 255, 127) );

    my $LayoutWidget_7 = Qt::Widget(SharingComp3, '$LayoutWidget_7');
    $LayoutWidget_7->setGeometry( Qt::Rect(41, 58, 81, 266) );
    my $layout15 = Qt::VBoxLayout($LayoutWidget_7, 11, 6, '$layout15');

    ShParam_3_1 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_1");
    $layout15->addWidget(ShParam_3_1);

    ShParam_3_2 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_2");
    $layout15->addWidget(ShParam_3_2);

    ShParam_3_3 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_3");
    $layout15->addWidget(ShParam_3_3);

    ShParam_3_4 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_4");
    $layout15->addWidget(ShParam_3_4);

    ShParam_3_5 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_5");
    $layout15->addWidget(ShParam_3_5);

    ShParam_3_6 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_6");
    ShParam_3_6->setEnabled( 0 );
    ShParam_3_6->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_3_6->setTristate( 0 );
    $layout15->addWidget(ShParam_3_6);

    ShParam_3_7 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_7");
    ShParam_3_7->setEnabled( 0 );
    ShParam_3_7->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_3_7->setTristate( 0 );
    $layout15->addWidget(ShParam_3_7);

    ShParam_3_8 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_8");
    ShParam_3_8->setEnabled( 0 );
    ShParam_3_8->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_3_8->setTristate( 0 );
    $layout15->addWidget(ShParam_3_8);

    ShParam_3_9 = Qt::CheckBox($LayoutWidget_7, "ShParam_3_9");
    ShParam_3_9->setEnabled( 0 );
    ShParam_3_9->setPaletteBackgroundColor( Qt::Color(234, 233, 232) );
    ShParam_3_9->setTristate( 0 );
    $layout15->addWidget(ShParam_3_9);
    musrfit_tabs->insertTab( SharingPahe, "" );

    InitializationPage = Qt::Widget(musrfit_tabs, "InitializationPage");

    InitParamTable = Qt::Table(InitializationPage, "InitParamTable");
    InitParamTable->setNumCols(InitParamTable->numCols() + 1);
    InitParamTable->horizontalHeader()->setLabel(InitParamTable->numCols() - 1, trUtf8("Value"));
    InitParamTable->setNumCols(InitParamTable->numCols() + 1);
    InitParamTable->horizontalHeader()->setLabel(InitParamTable->numCols() - 1, trUtf8("Error"));
    InitParamTable->setNumCols(InitParamTable->numCols() + 1);
    InitParamTable->horizontalHeader()->setLabel(InitParamTable->numCols() - 1, trUtf8("Min"));
    InitParamTable->setNumCols(InitParamTable->numCols() + 1);
    InitParamTable->horizontalHeader()->setLabel(InitParamTable->numCols() - 1, trUtf8("Max"));
    InitParamTable->setGeometry( Qt::Rect(5, 5, 545, 440) );
    InitParamTable->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, InitParamTable->sizePolicy()->hasHeightForWidth()) );
    InitParamTable->setMaximumSize( Qt::Size(32767, 32767) );
    InitParamTable->setFocusPolicy( &Qt::Table::TabFocus() );
    InitParamTable->setFrameShape( &Qt::Table::PopupPanel() );
    InitParamTable->setLineWidth( int(2) );
    InitParamTable->setMargin( int(0) );
    InitParamTable->setMidLineWidth( int(0) );
    InitParamTable->setResizePolicy( &Qt::Table::Default() );
    InitParamTable->setVScrollBarMode( &Qt::Table::Auto() );
    InitParamTable->setDragAutoScroll( 0 );
    InitParamTable->setNumRows( int(0) );
    InitParamTable->setNumCols( int(4) );
    InitParamTable->setShowGrid( 1 );
    InitParamTable->setRowMovingEnabled( 0 );
    InitParamTable->setFocusStyle( &Qt::Table::SpreadSheet() );
    musrfit_tabs->insertTab( InitializationPage, "" );

    FittingPage = Qt::Widget(musrfit_tabs, "FittingPage");

    textMSROutput = Qt::TextEdit(FittingPage, "textMSROutput");
    textMSROutput->setGeometry( Qt::Rect(5, 5, 545, 435) );
    textMSROutput->setOverwriteMode( 1 );
    musrfit_tabs->insertTab( FittingPage, "" );

    TabPage = Qt::Widget(musrfit_tabs, "TabPage");

    Minimization = Qt::ButtonGroup(TabPage, "Minimization");
    Minimization->setGeometry( Qt::Rect(6, 1, 200, 125) );
    Minimization->setExclusive( 1 );
    Minimization->setRadioButtonExclusive( 1 );
    Minimization->setProperty( "selectedId", Qt::Variant(int(0) ) );

    MINIMIZE = Qt::RadioButton(Minimization, "MINIMIZE");
    MINIMIZE->setGeometry( Qt::Rect(16, 30, 83, 22) );
    MINIMIZE->setChecked( 1 );

    MIGRAD = Qt::RadioButton(Minimization, "MIGRAD");
    MIGRAD->setGeometry( Qt::Rect(16, 59, 83, 22) );

    SIMPLEX = Qt::RadioButton(Minimization, "SIMPLEX");
    SIMPLEX->setGeometry( Qt::Rect(16, 88, 83, 22) );

    ErrorCalc = Qt::ButtonGroup(TabPage, "ErrorCalc");
    ErrorCalc->setGeometry( Qt::Rect(208, 1, 200, 125) );
    ErrorCalc->setAlignment( int(&Qt::ButtonGroup::AlignTop) );
    ErrorCalc->setExclusive( 1 );
    ErrorCalc->setRadioButtonExclusive( 1 );
    ErrorCalc->setProperty( "selectedId", Qt::Variant(int(0) ) );

    HESSE = Qt::RadioButton(ErrorCalc, "HESSE");
    HESSE->setGeometry( Qt::Rect(19, 30, 69, 22) );
    HESSE->setChecked( 1 );

    MINOS = Qt::RadioButton(ErrorCalc, "MINOS");
    MINOS->setGeometry( Qt::Rect(19, 59, 69, 22) );

    my $LayoutWidget_8 = Qt::Widget(TabPage, '$LayoutWidget_8');
    $LayoutWidget_8->setGeometry( Qt::Rect(418, 5, 125, 120) );
    my $layout28 = Qt::VBoxLayout($LayoutWidget_8, 11, 6, '$layout28');

    go = Qt::PushButton($LayoutWidget_8, "go");
    go->setSizePolicy( Qt::SizePolicy(7, 0, 1, 0, go->sizePolicy()->hasHeightForWidth()) );
    go->setMinimumSize( Qt::Size(95, 30) );
    go->setMaximumSize( Qt::Size(95, 30) );
    go->setAutoDefault( 0 );
    go->setDefault( 1 );
    $layout28->addWidget(go);

    PlotMSR = Qt::PushButton($LayoutWidget_8, "PlotMSR");
    PlotMSR->setSizePolicy( Qt::SizePolicy(7, 0, 1, 0, PlotMSR->sizePolicy()->hasHeightForWidth()) );
    PlotMSR->setMinimumSize( Qt::Size(95, 30) );
    PlotMSR->setMaximumSize( Qt::Size(95, 30) );
    PlotMSR->setAutoDefault( 0 );
    PlotMSR->setDefault( 1 );
    $layout28->addWidget(PlotMSR);

    FitTextOutput = Qt::TextEdit(TabPage, "FitTextOutput");
    FitTextOutput->setGeometry( Qt::Rect(5, 135, 545, 305) );
    FitTextOutput->setOverwriteMode( 1 );
    musrfit_tabs->insertTab( TabPage, "" );

    TabPage_2 = Qt::Widget(musrfit_tabs, "TabPage_2");

    my $LayoutWidget_9 = Qt::Widget(TabPage_2, '$LayoutWidget_9');
    $LayoutWidget_9->setGeometry( Qt::Rect(5, 5, 545, 94) );
    my $layout33 = Qt::HBoxLayout($LayoutWidget_9, 11, 6, '$layout33');

    my $layout26 = Qt::VBoxLayout(undef, 0, 6, '$layout26');

    FUnitsLabel = Qt::Label($LayoutWidget_9, "FUnitsLabel");
    $layout26->addWidget(FUnitsLabel);

    FApodizationLabel = Qt::Label($LayoutWidget_9, "FApodizationLabel");
    $layout26->addWidget(FApodizationLabel);

    FPlotLabel = Qt::Label($LayoutWidget_9, "FPlotLabel");
    $layout26->addWidget(FPlotLabel);
    $layout33->addLayout($layout26);

    my $layout27 = Qt::VBoxLayout(undef, 0, 6, '$layout27');

    FUnits = Qt::ComboBox(0, $LayoutWidget_9, "FUnits");
    FUnits->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, FUnits->sizePolicy()->hasHeightForWidth()) );
    $layout27->addWidget(FUnits);

    FApodization = Qt::ComboBox(0, $LayoutWidget_9, "FApodization");
    FApodization->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, FApodization->sizePolicy()->hasHeightForWidth()) );
    $layout27->addWidget(FApodization);

    FPlot = Qt::ComboBox(0, $LayoutWidget_9, "FPlot");
    FPlot->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, FPlot->sizePolicy()->hasHeightForWidth()) );
    $layout27->addWidget(FPlot);
    $layout33->addLayout($layout27);

    my $layout28_2 = Qt::VBoxLayout(undef, 0, 6, '$layout28_2');

    textLabel1 = Qt::Label($LayoutWidget_9, "textLabel1");
    $layout28_2->addWidget(textLabel1);

    textLabel1_3 = Qt::Label($LayoutWidget_9, "textLabel1_3");
    $layout28_2->addWidget(textLabel1_3);

    textLabel1_3_5 = Qt::Label($LayoutWidget_9, "textLabel1_3_5");
    $layout28_2->addWidget(textLabel1_3_5);
    $layout33->addLayout($layout28_2);

    my $layout29 = Qt::VBoxLayout(undef, 0, 6, '$layout29');

    lineEdit28 = Qt::LineEdit($LayoutWidget_9, "lineEdit28");
    lineEdit28->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, lineEdit28->sizePolicy()->hasHeightForWidth()) );
    $layout29->addWidget(lineEdit28);

    textLabel1_3_2 = Qt::Label($LayoutWidget_9, "textLabel1_3_2");
    $layout29->addWidget(textLabel1_3_2);

    textLabel1_3_6 = Qt::Label($LayoutWidget_9, "textLabel1_3_6");
    $layout29->addWidget(textLabel1_3_6);
    $layout33->addLayout($layout29);

    my $layout31 = Qt::VBoxLayout(undef, 0, 6, '$layout31');

    textLabel1_2 = Qt::Label($LayoutWidget_9, "textLabel1_2");
    $layout31->addWidget(textLabel1_2);

    textLabel1_3_3 = Qt::Label($LayoutWidget_9, "textLabel1_3_3");
    $layout31->addWidget(textLabel1_3_3);

    textLabel1_3_7 = Qt::Label($LayoutWidget_9, "textLabel1_3_7");
    $layout31->addWidget(textLabel1_3_7);
    $layout33->addLayout($layout31);

    my $layout32 = Qt::VBoxLayout(undef, 0, 6, '$layout32');

    lineEdit28_2 = Qt::LineEdit($LayoutWidget_9, "lineEdit28_2");
    lineEdit28_2->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, lineEdit28_2->sizePolicy()->hasHeightForWidth()) );
    $layout32->addWidget(lineEdit28_2);

    textLabel1_3_4 = Qt::Label($LayoutWidget_9, "textLabel1_3_4");
    $layout32->addWidget(textLabel1_3_4);

    textLabel1_3_8 = Qt::Label($LayoutWidget_9, "textLabel1_3_8");
    $layout32->addWidget(textLabel1_3_8);
    $layout33->addLayout($layout32);
    musrfit_tabs->insertTab( TabPage_2, "" );

    TabPage_3 = Qt::Widget(musrfit_tabs, "TabPage_3");

    my $LayoutWidget_10 = Qt::Widget(TabPage_3, '$LayoutWidget_10');
    $LayoutWidget_10->setGeometry( Qt::Rect(7, 5, 540, 180) );
    my $layout27_2 = Qt::HBoxLayout($LayoutWidget_10, 0, 6, '$layout27_2');

    groupHist0 = Qt::GroupBox($LayoutWidget_10, "groupHist0");
    groupHist0->setSizePolicy( Qt::SizePolicy(5, 5, 0, 0, groupHist0->sizePolicy()->hasHeightForWidth()) );
    groupHist0->setMinimumSize( Qt::Size(0, 0) );

    my $LayoutWidget_11 = Qt::Widget(groupHist0, '$LayoutWidget_11');
    $LayoutWidget_11->setGeometry( Qt::Rect(5, 18, 100, 150) );
    my $layout16_2 = Qt::VBoxLayout($LayoutWidget_11, 11, 6, '$layout16_2');

    textLabel2 = Qt::Label($LayoutWidget_11, "textLabel2");
    $layout16_2->addWidget(textLabel2);

    textLabel2_2_2_3 = Qt::Label($LayoutWidget_11, "textLabel2_2_2_3");
    $layout16_2->addWidget(textLabel2_2_2_3);

    textLabel2_2_2 = Qt::Label($LayoutWidget_11, "textLabel2_2_2");
    $layout16_2->addWidget(textLabel2_2_2);

    textLabel2_2_2_2 = Qt::Label($LayoutWidget_11, "textLabel2_2_2_2");
    $layout16_2->addWidget(textLabel2_2_2_2);

    textLabel2_2_2_2_2 = Qt::Label($LayoutWidget_11, "textLabel2_2_2_2_2");
    $layout16_2->addWidget(textLabel2_2_2_2_2);
    $layout27_2->addWidget(groupHist0);

    groupHist1 = Qt::GroupBox($LayoutWidget_10, "groupHist1");
    groupHist1->setFrameShadow( &Qt::GroupBox::Plain() );

    my $LayoutWidget_12 = Qt::Widget(groupHist1, '$LayoutWidget_12');
    $LayoutWidget_12->setGeometry( Qt::Rect(8, 17, 75, 150) );
    my $layout18 = Qt::VBoxLayout($LayoutWidget_12, 0, 0, '$layout18');

    t01 = Qt::LineEdit($LayoutWidget_12, "t01");
    $layout18->addWidget(t01);

    Bg11 = Qt::LineEdit($LayoutWidget_12, "Bg11");
    $layout18->addWidget(Bg11);

    Bg21 = Qt::LineEdit($LayoutWidget_12, "Bg21");
    $layout18->addWidget(Bg21);

    Data11 = Qt::LineEdit($LayoutWidget_12, "Data11");
    $layout18->addWidget(Data11);

    Data21 = Qt::LineEdit($LayoutWidget_12, "Data21");
    $layout18->addWidget(Data21);
    $layout27_2->addWidget(groupHist1);

    groupHist2 = Qt::GroupBox($LayoutWidget_10, "groupHist2");
    groupHist2->setEnabled( 1 );
    groupHist2->setFrameShadow( &Qt::GroupBox::Plain() );

    my $LayoutWidget_13 = Qt::Widget(groupHist2, '$LayoutWidget_13');
    $LayoutWidget_13->setGeometry( Qt::Rect(8, 17, 75, 150) );
    my $layout18_2 = Qt::VBoxLayout($LayoutWidget_13, 0, 0, '$layout18_2');

    t02 = Qt::LineEdit($LayoutWidget_13, "t02");
    $layout18_2->addWidget(t02);

    Bg12 = Qt::LineEdit($LayoutWidget_13, "Bg12");
    $layout18_2->addWidget(Bg12);

    Bg22 = Qt::LineEdit($LayoutWidget_13, "Bg22");
    $layout18_2->addWidget(Bg22);

    Data12 = Qt::LineEdit($LayoutWidget_13, "Data12");
    $layout18_2->addWidget(Data12);

    Data22 = Qt::LineEdit($LayoutWidget_13, "Data22");
    $layout18_2->addWidget(Data22);
    $layout27_2->addWidget(groupHist2);

    groupHist3 = Qt::GroupBox($LayoutWidget_10, "groupHist3");
    groupHist3->setEnabled( 1 );
    groupHist3->setFrameShadow( &Qt::GroupBox::Plain() );

    my $LayoutWidget_14 = Qt::Widget(groupHist3, '$LayoutWidget_14');
    $LayoutWidget_14->setGeometry( Qt::Rect(8, 17, 75, 150) );
    my $layout18_3 = Qt::VBoxLayout($LayoutWidget_14, 0, 0, '$layout18_3');

    t03 = Qt::LineEdit($LayoutWidget_14, "t03");
    $layout18_3->addWidget(t03);

    Bg13 = Qt::LineEdit($LayoutWidget_14, "Bg13");
    $layout18_3->addWidget(Bg13);

    Bg23 = Qt::LineEdit($LayoutWidget_14, "Bg23");
    $layout18_3->addWidget(Bg23);

    Data13 = Qt::LineEdit($LayoutWidget_14, "Data13");
    $layout18_3->addWidget(Data13);

    Data23 = Qt::LineEdit($LayoutWidget_14, "Data23");
    $layout18_3->addWidget(Data23);
    $layout27_2->addWidget(groupHist3);

    groupHist4 = Qt::GroupBox($LayoutWidget_10, "groupHist4");
    groupHist4->setEnabled( 1 );
    groupHist4->setFrameShadow( &Qt::GroupBox::Plain() );

    my $LayoutWidget_15 = Qt::Widget(groupHist4, '$LayoutWidget_15');
    $LayoutWidget_15->setGeometry( Qt::Rect(8, 17, 75, 150) );
    my $layout18_4 = Qt::VBoxLayout($LayoutWidget_15, 0, 0, '$layout18_4');

    t04 = Qt::LineEdit($LayoutWidget_15, "t04");
    $layout18_4->addWidget(t04);

    Bg14 = Qt::LineEdit($LayoutWidget_15, "Bg14");
    $layout18_4->addWidget(Bg14);

    Bg24 = Qt::LineEdit($LayoutWidget_15, "Bg24");
    $layout18_4->addWidget(Bg24);

    Data14 = Qt::LineEdit($LayoutWidget_15, "Data14");
    $layout18_4->addWidget(Data14);

    Data24 = Qt::LineEdit($LayoutWidget_15, "Data24");
    $layout18_4->addWidget(Data24);
    $layout27_2->addWidget(groupHist4);

    ShowT0 = Qt::PushButton(TabPage_3, "ShowT0");
    ShowT0->setGeometry( Qt::Rect(451, 190, 95, 30) );
    ShowT0->setMinimumSize( Qt::Size(95, 30) );
    ShowT0->setMaximumSize( Qt::Size(95, 30) );
    ShowT0->setAutoDefault( 0 );
    ShowT0->setDefault( 1 );
    musrfit_tabs->insertTab( TabPage_3, "" );

    fileNewAction= Qt::Action(this, "fileNewAction");
    fileNewAction->setIconSet( Qt::IconSet($image1) );
    fileOpenAction= Qt::Action(this, "fileOpenAction");
    fileOpenAction->setEnabled( 0 );
    fileOpenAction->setIconSet( Qt::IconSet($image2) );
    fileSaveAction= Qt::Action(this, "fileSaveAction");
    fileSaveAction->setIconSet( Qt::IconSet($image3) );
    fileSaveAsAction= Qt::Action(this, "fileSaveAsAction");
    filePrintAction= Qt::Action(this, "filePrintAction");
    filePrintAction->setEnabled( 0 );
    filePrintAction->setIconSet( Qt::IconSet($image4) );
    fileExitAction= Qt::Action(this, "fileExitAction");
    editUndoAction= Qt::Action(this, "editUndoAction");
    editUndoAction->setEnabled( 0 );
    editUndoAction->setIconSet( Qt::IconSet($image5) );
    editRedoAction= Qt::Action(this, "editRedoAction");
    editRedoAction->setEnabled( 0 );
    editRedoAction->setIconSet( Qt::IconSet($image6) );
    editCutAction= Qt::Action(this, "editCutAction");
    editCutAction->setEnabled( 0 );
    editCutAction->setIconSet( Qt::IconSet($image7) );
    editCopyAction= Qt::Action(this, "editCopyAction");
    editCopyAction->setEnabled( 0 );
    editCopyAction->setIconSet( Qt::IconSet($image8) );
    editPasteAction= Qt::Action(this, "editPasteAction");
    editPasteAction->setEnabled( 0 );
    editPasteAction->setIconSet( Qt::IconSet($image9) );
    editFindAction= Qt::Action(this, "editFindAction");
    editFindAction->setIconSet( Qt::IconSet($image10) );
    helpContentsAction= Qt::Action(this, "helpContentsAction");
    helpContentsAction->setEnabled( 0 );
    helpIndexAction= Qt::Action(this, "helpIndexAction");
    helpIndexAction->setEnabled( 0 );
    helpAboutAction= Qt::Action(this, "helpAboutAction");
    Action= Qt::Action(this, "Action");
    separatorAction= Qt::Action(this, "separatorAction");
    FileExistCheck= Qt::Action(this, "FileExistCheck");
    FileExistCheck->setToggleAction( 1 );
    FileExistCheck->setOn( 1 );
    ManualFile= Qt::Action(this, "ManualFile");
    ManualFile->setToggleAction( 1 );
    fileChangeDirAction= Qt::Action(this, "fileChangeDirAction");
    Action_2= Qt::Action(this, "Action_2");
    Action_3= Qt::Action(this, "Action_3");
    optionsnew_itemAction= Qt::Action(this, "optionsnew_itemAction");


    toolBar = Qt::ToolBar("", this, &DockTop);

    fileOpenAction->addTo(toolBar);
    fileSaveAction->addTo(toolBar);
    fileSaveAsAction->addTo(toolBar);
    filePrintAction->addTo(toolBar);
    fileExitAction->addTo(toolBar);


    MenuBar= Qt::MenuBar( this, "MenuBar");

    MenuBar->setEnabled( 1 );

    fileMenu = Qt::PopupMenu( this );
    fileOpenAction->addTo( fileMenu );
    fileSaveAction->addTo( fileMenu );
    fileChangeDirAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    filePrintAction->addTo( fileMenu );
    fileMenu->insertSeparator();
    fileExitAction->addTo( fileMenu );
    MenuBar->insertItem( "", fileMenu, 2 );

    editMenu = Qt::PopupMenu( this );
    editUndoAction->addTo( editMenu );
    editRedoAction->addTo( editMenu );
    editMenu->insertSeparator();
    editCutAction->addTo( editMenu );
    editCopyAction->addTo( editMenu );
    editPasteAction->addTo( editMenu );
    MenuBar->insertItem( "", editMenu, 3 );

    Options = Qt::PopupMenu( this );
    FileExistCheck->addTo( Options );
    ManualFile->addTo( Options );
    MenuBar->insertItem( "", Options, 4 );

    helpMenu = Qt::PopupMenu( this );
    helpContentsAction->addTo( helpMenu );
    helpIndexAction->addTo( helpMenu );
    helpMenu->insertSeparator();
    helpAboutAction->addTo( helpMenu );
    MenuBar->insertItem( "", helpMenu, 5 );

    MenuBar->insertSeparator( 6 );

    languageChange();
    my $resize = Qt::Size(579, 578);
    $resize = $resize->expandedTo(minimumSizeHint());
    resize( $resize );
    clearWState( &Qt::WState_Polished );

    Qt::Object::connect(helpAboutAction, SIGNAL "activated()", this, SLOT "helpAbout()");
    Qt::Object::connect(helpContentsAction, SIGNAL "activated()", this, SLOT "helpContents()");
    Qt::Object::connect(helpIndexAction, SIGNAL "activated()", this, SLOT "helpIndex()");
    Qt::Object::connect(editPasteAction, SIGNAL "activated()", this, SLOT "editPaste()");
    Qt::Object::connect(editCopyAction, SIGNAL "activated()", this, SLOT "editCopy()");
    Qt::Object::connect(editCutAction, SIGNAL "activated()", this, SLOT "editCut()");
    Qt::Object::connect(editRedoAction, SIGNAL "activated()", this, SLOT "editRedo()");
    Qt::Object::connect(editUndoAction, SIGNAL "activated()", this, SLOT "editUndo()");
    Qt::Object::connect(fileExitAction, SIGNAL "activated()", this, SLOT "fileExit()");
    Qt::Object::connect(filePrintAction, SIGNAL "activated()", this, SLOT "filePrint()");
    Qt::Object::connect(fileSaveAction, SIGNAL "activated()", this, SLOT "fileSave()");
    Qt::Object::connect(fileOpenAction, SIGNAL "activated()", this, SLOT "fileOpen()");
    Qt::Object::connect(fileChangeDirAction, SIGNAL "activated()", this, SLOT "fileChangeDir()");
    Qt::Object::connect(musrfit_tabs, SIGNAL "selected(const QString&)", this, SLOT "TabChanged()");
    Qt::Object::connect(go, SIGNAL "clicked()", this, SLOT "GoFit()");
    Qt::Object::connect(ShowT0, SIGNAL "clicked()", this, SLOT "ShowMuSRT0()");
    Qt::Object::connect(PlotMSR, SIGNAL "clicked()", this, SLOT "GoPlot()");
    Qt::Object::connect(InitParamTable, SIGNAL "valueChanged(int,int)", this, SLOT "CallMSRCreate()");
    Qt::Object::connect(ManualFile, SIGNAL "toggled(bool)", this, SLOT "RunSelectionToggle()");
    Qt::Object::connect(Browse, SIGNAL "clicked()", this, SLOT "fileBrowse()");
    Qt::Object::connect(BeamLine, SIGNAL "activated(int)", this, SLOT "T0Update()");

    setTabOrder(musrfit_tabs, TITLE);
    setTabOrder(TITLE, FILENAME);
    setTabOrder(FILENAME, RunFiles);
    setTabOrder(RunFiles, Browse);
    setTabOrder(Browse, RunNumbers);
    setTabOrder(RunNumbers, BeamLine);
    setTabOrder(BeamLine, YEAR);
    setTabOrder(YEAR, FitType1);
    setTabOrder(FitType1, FitType2);
    setTabOrder(FitType2, FitType3);
    setTabOrder(FitType3, Tis);
    setTabOrder(Tis, Tfs);
    setTabOrder(Tfs, BINS);
    setTabOrder(BINS, FitAsyType);
    setTabOrder(FitAsyType, LRBF);
    setTabOrder(LRBF, ShParam_1_1);
    setTabOrder(ShParam_1_1, ShParam_1_2);
    setTabOrder(ShParam_1_2, ShParam_1_3);
    setTabOrder(ShParam_1_3, ShParam_1_4);
    setTabOrder(ShParam_1_4, ShParam_1_5);
    setTabOrder(ShParam_1_5, ShParam_1_6);
    setTabOrder(ShParam_1_6, ShParam_1_7);
    setTabOrder(ShParam_1_7, ShParam_1_8);
    setTabOrder(ShParam_1_8, ShParam_1_9);
    setTabOrder(ShParam_1_9, ShParam_2_1);
    setTabOrder(ShParam_2_1, ShParam_2_2);
    setTabOrder(ShParam_2_2, ShParam_2_3);
    setTabOrder(ShParam_2_3, ShParam_2_4);
    setTabOrder(ShParam_2_4, ShParam_2_5);
    setTabOrder(ShParam_2_5, ShParam_2_6);
    setTabOrder(ShParam_2_6, ShParam_2_7);
    setTabOrder(ShParam_2_7, ShParam_2_8);
    setTabOrder(ShParam_2_8, ShParam_2_9);
    setTabOrder(ShParam_2_9, ShParam_3_1);
    setTabOrder(ShParam_3_1, ShParam_3_2);
    setTabOrder(ShParam_3_2, ShParam_3_3);
    setTabOrder(ShParam_3_3, ShParam_3_4);
    setTabOrder(ShParam_3_4, ShParam_3_5);
    setTabOrder(ShParam_3_5, ShParam_3_6);
    setTabOrder(ShParam_3_6, ShParam_3_7);
    setTabOrder(ShParam_3_7, ShParam_3_8);
    setTabOrder(ShParam_3_8, ShParam_3_9);
    setTabOrder(ShParam_3_9, InitParamTable);
    setTabOrder(InitParamTable, textMSROutput);
    setTabOrder(textMSROutput, MINIMIZE);
    setTabOrder(MINIMIZE, HESSE);
    setTabOrder(HESSE, go);
    setTabOrder(go, PlotMSR);
    setTabOrder(PlotMSR, FitTextOutput);
    setTabOrder(FitTextOutput, FUnits);
    setTabOrder(FUnits, FApodization);
    setTabOrder(FApodization, FPlot);
    setTabOrder(FPlot, lineEdit28);
    setTabOrder(lineEdit28, lineEdit28_2);
    setTabOrder(lineEdit28_2, t01);
    setTabOrder(t01, Bg11);
    setTabOrder(Bg11, Bg21);
    setTabOrder(Bg21, Data11);
    setTabOrder(Data11, Data21);
    setTabOrder(Data21, t02);
    setTabOrder(t02, Bg12);
    setTabOrder(Bg12, Bg22);
    setTabOrder(Bg22, Data12);
    setTabOrder(Data12, Data22);
    setTabOrder(Data22, t03);
    setTabOrder(t03, Bg13);
    setTabOrder(Bg13, Bg23);
    setTabOrder(Bg23, Data13);
    setTabOrder(Data13, Data23);
    setTabOrder(Data23, t04);
    setTabOrder(t04, Bg14);
    setTabOrder(Bg14, Bg24);
    setTabOrder(Bg24, Data14);
    setTabOrder(Data14, Data24);
    setTabOrder(Data24, ShowT0);

    TITLELabel->setBuddy(this->TITLE);
    FILENAMELabel->setBuddy(this->FILENAME);
    TfsLabel->setBuddy(this->Tfs);
    Comp1Label->setBuddy(this->FitType1);
    BINSLabel->setBuddy(this->BINS);
    TisLabel->setBuddy(this->Tis);
}


#  Sets the strings of the subwidgets using the current
#  language.

sub languageChange
{
    setCaption(trUtf8("MuSRFit GUI") );
    setIconText(trUtf8("MuSRFitGUI") );
    groupTitle->setTitle( undef );
    TITLELabel->setText( trUtf8("Enter the label (defaul is run title from the first run)") );
    TITLE->setText( trUtf8("","Title line for MSR file (optional)") );
    Qt::ToolTip::add(TITLE, trUtf8("Title line for MSR file (optional)"));
    Qt::WhatsThis::add(TITLE, trUtf8("Title line for MSR file (optional)"));
    FILENAMELabel->setText( trUtf8("Enter [name] for output [name].msr file (optional)") );
    Qt::ToolTip::add(FILENAME, trUtf8("Name of the produced MSR file (optional)"));
    Qt::WhatsThis::add(FILENAME, trUtf8("Name of the produced MSR file (optional)"));
    FitAsyTypeLabel->setText( trUtf8("Fit type") );
    FitAsyType->clear();
    FitAsyType->insertItem( trUtf8("Asymmetry") );
    FitAsyType->insertItem( trUtf8("SingleHist") );
    LRBFLabel->setText( trUtf8("Histograms list") );
    LRBF->setText( trUtf8("1,3") );
    groupBox7->setTitle( trUtf8("Theory Function") );
    FitType2->clear();
    FitType2->insertItem( trUtf8("Exponential") );
    FitType2->insertItem( trUtf8("Gaussian") );
    FitType2->insertItem( trUtf8("Stretch Exp.") );
    FitType2->insertItem( trUtf8("Exponential Cos") );
    FitType2->insertItem( trUtf8("Gaussian Cos") );
    FitType2->insertItem( trUtf8("Stretch Cos") );
    FitType2->insertItem( trUtf8("Lorentzian Dynamic KT") );
    FitType2->insertItem( trUtf8("Gaussian Dynamic KT") );
    FitType2->insertItem( trUtf8("Background") );
    FitType2->insertItem( trUtf8("Lorentzian Kubo-Toyabe LF x Exp") );
    FitType2->insertItem( trUtf8("Gaussian Kubo-Toyabe LF x Exp") );
    FitType2->insertItem( trUtf8("Lorentzian Kubo-Toyabe LF x Str Exp") );
    FitType2->insertItem( trUtf8("Gaussian Kubo-Toyabe LF x Str Exp") );
    FitType2->insertItem( trUtf8("Meissner State Model") );
    FitType2->insertItem( trUtf8("None") );
    FitType2->setCurrentItem( int(14) );
    TfsLabel->setText( trUtf8("Final Time") );
    FitType1->clear();
    FitType1->insertItem( trUtf8("Exponential") );
    FitType1->insertItem( trUtf8("Gaussian") );
    FitType1->insertItem( trUtf8("Stretch Exp.") );
    FitType1->insertItem( trUtf8("Exponential Cos") );
    FitType1->insertItem( trUtf8("Gaussian Cos") );
    FitType1->insertItem( trUtf8("Stretch Cos") );
    FitType1->insertItem( trUtf8("Lorentzian Dynamic KT") );
    FitType1->insertItem( trUtf8("Gaussian Dynamic KT") );
    FitType1->insertItem( trUtf8("Background") );
    FitType1->insertItem( trUtf8("Lorentzian Kubo-Toyabe LF x Exp") );
    FitType1->insertItem( trUtf8("Gaussian Kubo-Toyabe LF x Exp") );
    FitType1->insertItem( trUtf8("Lorentzian Kubo-Toyabe LF x Str Exp") );
    FitType1->insertItem( trUtf8("Gaussian Kubo-Toyabe LF x Str Exp") );
    FitType1->insertItem( trUtf8("Meissner State Model") );
    FitType1->insertItem( trUtf8("None") );
    BINS->setText( trUtf8("100") );
    Comp3Label->setText( trUtf8("Third Component") );
    Tis->setText( trUtf8("0") );
    Tfs->setText( trUtf8("8") );
    Comp1Label->setText( trUtf8("First Component") );
    BINSLabel->setText( trUtf8("Binning Factor") );
    TisLabel->setText( trUtf8("Initial Time") );
    FitType3->clear();
    FitType3->insertItem( trUtf8("Exponential") );
    FitType3->insertItem( trUtf8("Gaussian") );
    FitType3->insertItem( trUtf8("Stretch Exp.") );
    FitType3->insertItem( trUtf8("Exponential Cos") );
    FitType3->insertItem( trUtf8("Gaussian Cos") );
    FitType3->insertItem( trUtf8("Stretch Cos") );
    FitType3->insertItem( trUtf8("Lorentzian Dynamic KT") );
    FitType3->insertItem( trUtf8("Gaussian Dynamic KT") );
    FitType3->insertItem( trUtf8("Background") );
    FitType3->insertItem( trUtf8("Lorentzian Kubo-Toyabe LF x Exp") );
    FitType3->insertItem( trUtf8("Gaussian Kubo-Toyabe LF x Exp") );
    FitType3->insertItem( trUtf8("Lorentzian Kubo-Toyabe LF x Str Exp") );
    FitType3->insertItem( trUtf8("Gaussian Kubo-Toyabe LF x Str Exp") );
    FitType3->insertItem( trUtf8("Meissner State Model") );
    FitType3->insertItem( trUtf8("None") );
    FitType3->setCurrentItem( int(14) );
    Comp2Label->setText( trUtf8("Second Component") );
    RUNSAuto->setTitle( trUtf8("RUN Numbers") );
    Qt::ToolTip::add(RunNumbers, trUtf8("Numbers of RUNs to fit. Multiple runs are comma separated."));
    Qt::WhatsThis::add(RunNumbers, trUtf8("Numbers of RUNs to fit. Multiple runs are comma separated."));
    BeamLineLabel->setText( trUtf8("On beam line") );
    BeamLine->clear();
    BeamLine->insertItem( trUtf8("LEM") );
    BeamLine->insertItem( trUtf8("GPS") );
    BeamLine->insertItem( trUtf8("Dolly") );
    BeamLine->insertItem( trUtf8("LTF") );
    YEAR->clear();
    YEAR->insertItem( trUtf8("2009") );
    YEAR->insertItem( trUtf8("2008") );
    YEAR->insertItem( trUtf8("2007") );
    YEAR->insertItem( trUtf8("2006") );
    YEAR->insertItem( trUtf8("2005") );
    YEAR->insertItem( trUtf8("2004") );
    YEARLabel->setText( trUtf8("Year") );
    RUNSMan->setTitle( trUtf8("RUN Files") );
    Qt::ToolTip::add(RunFiles, trUtf8("Names of data files to be fit. Multiple data files are comma separated."));
    Qt::WhatsThis::add(RunFiles, trUtf8("Names of data files to be fit. Multiple data files are comma separated."));
    Browse->setText( trUtf8("Browse") );
    Qt::ToolTip::add(Browse, trUtf8("Browse to select data files for fitting."));
    Qt::WhatsThis::add(Browse, trUtf8("Browse to select data files for fitting."));
    musrfit_tabs->changeTab( RUNSPage, trUtf8("RUNS") );
    buttonGroupSharing->setTitle( trUtf8("Shared Parameters") );
    SharingComp1->setTitle( trUtf8("1st Component") );
    ShParam_1_1->setText( trUtf8("Param1") );
    ShParam_1_2->setText( trUtf8("Param2") );
    ShParam_1_3->setText( trUtf8("Param3") );
    ShParam_1_4->setText( trUtf8("Param4") );
    ShParam_1_5->setText( trUtf8("Param5") );
    ShParam_1_6->setText( trUtf8("Param6") );
    ShParam_1_7->setText( trUtf8("Param7") );
    ShParam_1_8->setText( trUtf8("Param8") );
    ShParam_1_9->setText( trUtf8("Param9") );
    Comp1ShLabel->setText( trUtf8("FitType1") );
    SharingComp2->setTitle( trUtf8("2nd Component") );
    Comp2ShLabel->setText( trUtf8("FitType2") );
    ShParam_2_1->setText( trUtf8("Param1") );
    ShParam_2_2->setText( trUtf8("Param2") );
    ShParam_2_3->setText( trUtf8("Param3") );
    ShParam_2_4->setText( trUtf8("Param4") );
    ShParam_2_5->setText( trUtf8("Param5") );
    ShParam_2_6->setText( trUtf8("Param6") );
    ShParam_2_7->setText( trUtf8("Param7") );
    ShParam_2_8->setText( trUtf8("Param8") );
    ShParam_2_9->setText( trUtf8("Param9") );
    SharingComp3->setTitle( trUtf8("3rd Component") );
    Comp3ShLabel->setText( trUtf8("FitType3") );
    ShParam_3_1->setText( trUtf8("Param1") );
    ShParam_3_2->setText( trUtf8("Param2") );
    ShParam_3_3->setText( trUtf8("Param3") );
    ShParam_3_4->setText( trUtf8("Param4") );
    ShParam_3_5->setText( trUtf8("Param5") );
    ShParam_3_6->setText( trUtf8("Param6") );
    ShParam_3_7->setText( trUtf8("Param7") );
    ShParam_3_8->setText( trUtf8("Param8") );
    ShParam_3_9->setText( trUtf8("Param9") );
    musrfit_tabs->changeTab( SharingPahe, trUtf8("Sharing") );
    InitParamTable->horizontalHeader()->setLabel( 0, trUtf8("Value") );
    InitParamTable->horizontalHeader()->setLabel( 1, trUtf8("Error") );
    InitParamTable->horizontalHeader()->setLabel( 2, trUtf8("Min") );
    InitParamTable->horizontalHeader()->setLabel( 3, trUtf8("Max") );
    musrfit_tabs->changeTab( InitializationPage, trUtf8("Initialization") );
    musrfit_tabs->changeTab( FittingPage, trUtf8("MSR File") );
    Minimization->setTitle( trUtf8("Minimization") );
    MINIMIZE->setText( trUtf8("Minimize") );
    MIGRAD->setText( trUtf8("Migrad") );
    SIMPLEX->setText( trUtf8("Simplex") );
    ErrorCalc->setTitle( trUtf8("Error Calculation") );
    HESSE->setText( trUtf8("Hesse") );
    MINOS->setText( trUtf8("Minos") );
    go->setText( trUtf8("Fit") );
    PlotMSR->setText( trUtf8("Plot") );
    musrfit_tabs->changeTab( TabPage, trUtf8("Fitting") );
    FUnitsLabel->setText( trUtf8("Units") );
    FApodizationLabel->setText( trUtf8("Apodization") );
    FPlotLabel->setText( trUtf8("Plot") );
    FUnits->clear();
    FUnits->insertItem( trUtf8("MHz") );
    FUnits->insertItem( trUtf8("Gauss") );
    FUnits->insertItem( trUtf8("Mc/s") );
    FApodization->clear();
    FApodization->insertItem( trUtf8("STRONG") );
    FApodization->insertItem( trUtf8("MEDIUM") );
    FApodization->insertItem( trUtf8("WEAK") );
    FApodization->insertItem( trUtf8("NONE") );
    FPlot->clear();
    FPlot->insertItem( trUtf8("power") );
    FPlot->insertItem( trUtf8("real") );
    FPlot->insertItem( trUtf8("imag") );
    FPlot->insertItem( trUtf8("real_and_imag") );
    FPlot->insertItem( trUtf8("phase") );
    textLabel1->setText( trUtf8("Range: from") );
    textLabel1_3->setText( undef );
    textLabel1_3_5->setText( undef );
    textLabel1_3_2->setText( undef );
    textLabel1_3_6->setText( undef );
    textLabel1_2->setText( trUtf8("to") );
    textLabel1_3_3->setText( undef );
    textLabel1_3_7->setText( undef );
    textLabel1_3_4->setText( undef );
    textLabel1_3_8->setText( undef );
    musrfit_tabs->changeTab( TabPage_2, trUtf8("Fourier") );
    groupHist0->setTitle( undef );
    textLabel2->setText( trUtf8("t0") );
    textLabel2_2_2_3->setText( trUtf8("Bg1") );
    textLabel2_2_2->setText( trUtf8("Bg2") );
    textLabel2_2_2_2->setText( trUtf8("Data 1") );
    textLabel2_2_2_2_2->setText( trUtf8("Data 2") );
    groupHist1->setTitle( trUtf8("Hist1") );
    groupHist2->setTitle( trUtf8("Hist2") );
    groupHist3->setTitle( trUtf8("Hist3") );
    groupHist4->setTitle( trUtf8("Hist4") );
    ShowT0->setText( trUtf8("Show") );
    musrfit_tabs->changeTab( TabPage_3, trUtf8("t0/Bg Bins") );
    fileNewAction->setText( trUtf8("&New") );
    fileNewAction->setMenuText( trUtf8("&New") );
    fileNewAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+N") ) );
    fileOpenAction->setText( trUtf8("&Open MSR...") );
    fileOpenAction->setMenuText( trUtf8("&Open MSR...") );
    fileOpenAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+O") ) );
    fileSaveAction->setText( trUtf8("&Save MSR") );
    fileSaveAction->setMenuText( trUtf8("&Save MSR") );
    fileSaveAction->setStatusTip( trUtf8("&Save MSR") );
    fileSaveAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+S") ) );
    fileSaveAsAction->setText( trUtf8("Save MSR &As...") );
    fileSaveAsAction->setMenuText( trUtf8("Save MSR &As...") );
    fileSaveAsAction->setAccel( Qt::KeySequence( undef ) );
    filePrintAction->setText( trUtf8("Print") );
    filePrintAction->setMenuText( trUtf8("&Print...") );
    filePrintAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+P") ) );
    fileExitAction->setText( trUtf8("Exit") );
    fileExitAction->setMenuText( trUtf8("E&xit") );
    fileExitAction->setAccel( Qt::KeySequence( undef ) );
    editUndoAction->setText( trUtf8("Undo") );
    editUndoAction->setMenuText( trUtf8("&Undo") );
    editUndoAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+Z") ) );
    editRedoAction->setText( trUtf8("Redo") );
    editRedoAction->setMenuText( trUtf8("&Redo") );
    editRedoAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+Y") ) );
    editCutAction->setText( trUtf8("Cut") );
    editCutAction->setMenuText( trUtf8("Cu&t") );
    editCutAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+X") ) );
    editCopyAction->setText( trUtf8("Copy") );
    editCopyAction->setMenuText( trUtf8("&Copy") );
    editCopyAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+C") ) );
    editPasteAction->setText( trUtf8("Paste") );
    editPasteAction->setMenuText( trUtf8("&Paste") );
    editPasteAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+V") ) );
    editFindAction->setText( trUtf8("Find") );
    editFindAction->setMenuText( trUtf8("&Find...") );
    editFindAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+F") ) );
    helpContentsAction->setText( trUtf8("Contents") );
    helpContentsAction->setMenuText( trUtf8("&Contents...") );
    helpContentsAction->setAccel( Qt::KeySequence( undef ) );
    helpIndexAction->setText( trUtf8("Index") );
    helpIndexAction->setMenuText( trUtf8("&Index...") );
    helpIndexAction->setAccel( Qt::KeySequence( undef ) );
    helpAboutAction->setText( trUtf8("About") );
    helpAboutAction->setMenuText( trUtf8("&About") );
    helpAboutAction->setAccel( Qt::KeySequence( undef ) );
    Action->setText( trUtf8("Unnamed") );
    Action->setMenuText( trUtf8("Unnamed") );
    separatorAction->setText( undef );
    separatorAction->setMenuText( undef );
    FileExistCheck->setText( trUtf8("File exist check") );
    FileExistCheck->setMenuText( trUtf8("File exist check") );
    FileExistCheck->setWhatsThis( trUtf8("Enable/Disable checking for MSR files.") );
    ManualFile->setText( trUtf8("Maual file selection") );
    ManualFile->setMenuText( trUtf8("Maual file selection") );
    fileChangeDirAction->setText( trUtf8("&Change dir") );
    fileChangeDirAction->setMenuText( trUtf8("&Change dir") );
    fileChangeDirAction->setAccel( Qt::KeySequence( trUtf8("Ctrl+C") ) );
    Action_2->setText( trUtf8("Unnamed") );
    Action_3->setText( trUtf8("Unnamed") );
    optionsnew_itemAction->setText( trUtf8("new item") );
    optionsnew_itemAction->setMenuText( trUtf8("new item") );
    toolBar->setLabel( trUtf8("Tools") );
    MenuBar->findItem( 2 )->setText( trUtf8("&File") );
    MenuBar->findItem( 3 )->setText( trUtf8("&Edit") );
    MenuBar->findItem( 4 )->setText( trUtf8("Options") );
    MenuBar->findItem( 5 )->setText( trUtf8("&Help") );
}


sub fileOpen
{

    my $file=Qt::FileDialog::getOpenFileName(
	    ".",
	    "MSR Files (*.msr *.mlog)",
	    this,
	    "open file dialog",
	    "Choose a MSR file");
    print "Selected file: $file\n";
# TODO: Possibly decipher the MSR file and setup the GUI accordingly
# Find run numbers, beamlines etc.
# Get theory block and understand it
# Get parameters list and update table
    

}

sub fileSave
{

    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"}.".msr";
    my $file=Qt::FileDialog::getSaveFileName(
	    "$FILENAME",
	    "MSR Files (*.msr *.mlog)",
	    this,
	    "save file dialog",
	    "Choose a filename to save under");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
# TODO: check if the extension is correct, or add it.
	if (-e $FILENAME) {
	    my $cmd="cp $FILENAME $file";
	    my $pid=system($cmd);
	} else {
	    if ($file ne "") {
		my $Warning = "Warning: No MSR file found yet!";
		my $WarningWindow = Qt::MessageBox::information( this, "Warning",$Warning);
	    }
	}
    }

}

sub fileChangeDir
{

    my $newdir=Qt::FileDialog::getExistingDirectory(
	    "",
	    this,
	    "get existing directory",
	    "Choose a directory",
	    1);
    chdir ("$newdir");

}

sub filePrint
{
    print "MuSRFitform->filePrint(): Not implemented yet.\n";
}

sub fileExit
{

    Qt::Application::exit( 0 );

}

sub editUndo
{
    print "MuSRFitform->editUndo(): Not implemented yet.\n";
}

sub editRedo
{
    print "MuSRFitform->editRedo(): Not implemented yet.\n";
}

sub editCut
{
    print "MuSRFitform->editCut(): Not implemented yet.\n";
}

sub editCopy
{
    print "MuSRFitform->editCopy(): Not implemented yet.\n";
}

sub editPaste
{
    print "MuSRFitform->editPaste(): Not implemented yet.\n";
}

sub helpIndex
{
    print "MuSRFitform->helpIndex(): Not implemented yet.\n";
}

sub helpContents
{
    print "MuSRFitform->helpContents(): Not implemented yet.\n";
}

sub helpAbout
{

    my $AboutText="
		  This is a GUI that uses the musrfit binary, developed by Andreas Suter,
		  to fit muSR spectra. 
		  
		  MuSRFitGUI is free software: you can redistribute it and/or modify
		  it under the terms of the GNU General Public License as published by
		  the Free Software Foundation, either version 3 of the License, or
		  (at your option) any later version.
		  
		  MuSRFitGUI is distributed in the hope that it will be useful,
		  but WITHOUT ANY WARRANTY; without even the implied warranty of
		  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		  GNU General Public License for more details.
		  
		  You should have received a copy of the GNU General Public License
		  along with MuSRFitGUI.  If not, see <http://www.gnu.org/licenses/>.
		  
		  Copyright 2009 by Zaher Salman and the LEM Group.
		  <zaher.salman\@psi.ch>
		  ";
    my $AboutWindow = Qt::MessageBox::information( this, "About MuSRFit GUI",$AboutText);

}

sub CreateAllInput
{

# TODO: Need to deliver shared parameters also
    my %All=();
# From RUNS Tab    
    $All{"TITLE"}= TITLE->text;
    $All{"FILENAME"}= FILENAME->text;
    $All{"RunNumbers"} = RunNumbers->text;
    $All{"RunFiles"} = RunFiles->text;
    $All{"BeamLine"} = BeamLine->currentText;
    $All{"RUNSType"} = ManualFile->isOn();
    $All{"YEAR"} =YEAR->currentText;
    $All{"Tis"} = Tis->text;
    $All{"Tfs"} = Tfs->text;
    $All{"BINS"} = BINS->text;
    $All{"FitAsyType"} = FitAsyType->currentText;
    $All{"LRBF"} = LRBF->text;
    
    RunSelectionToggle();
    my @RUNS = ();
    if ($All{"RUNSType"} ) {
	@RUNS = split( /,/, $All{"RunFiles"});
    } else {
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	@RUNS = split( /,/, $All{"RunNumbers"} );	
    }
    
    my @Hists = split(/,/, $All{"LRBF"} );
    
# From Fourier Tab
    $All{"FUNITS"}= FUnits->currentText;
    $All{"FAPODIZATION"}= FApodization->currentText;
    $All{"FPLOT"}= FPlot->currentText;
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","Bg1","Bg2","Data1","Data2") {
	    my $Name = "$_$NHist";
	    $All{$Name}=child($Name)->text;
# TODO: If empty fill with defaults
	    if ($All{$Name} eq "") {
		$All{$Name}=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
		child($Name)->setText($All{$Name});
	    }
	}
	$NHist++;
    }
    
# Construct fittypes that can be understood by MSR.pm
    my %FTs=(0,"Exponential",
	     1,"Gaussian",
	     2,"Stretch",
	     3,"ExponentialCos",
	     4,"GaussianCos",
	     5,"StretchCos",
	     6,"LDKTLF",
	     7,"GDKTLF",
	     8,"Background",
	     9,"LLFExp",
	     10,"GLFExp",
	     11,"LLFSExp",
	     12,"GLFSExp",
	     13,"Meissner",
	     14,"None"
	     );
    
    my $FT1=FitType1->currentItem;
    my $FT2=FitType2->currentItem;
    my $FT3=FitType3->currentItem;
    $All{"FitType1"} = $FTs{$FT1};
    $All{"FitType2"} = $FTs{$FT2};
    $All{"FitType3"} = $FTs{$FT3};
    my @FitTypes =();
    my $FitType="";
    foreach $FitType ($All{"FitType1"}, $All{"FitType2"}, $All{"FitType3"}) {
	if ( $FitType ne "None" ) {
	    push( @FitTypes, $FitType );	    
	}
    }
    
# Also theory block and paramets list
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
    $All{"Full_T_Block"}=$Full_T_Block;    
    $All{"Paramcomp_ref"}=$Paramcomp_ref;
    my @Paramcomp = @$Paramcomp_ref;
    
# TODO: Read initial values of paramets from tabel
    my $erradd = "d";
    my $minadd = "_min";
    my $maxadd = "_max";
    my $NRows = InitParamTable->numRows();
    my $Header=InitParamTable->verticalHeader();
    if ($NRows > 0) {
	for (my $i=0;$i<$NRows;$i++) {
# Take label of row, i.e. name of parameter
	    my $Param=$Header->label($i);
# Then take the value, error, max and min (as numbers)
	    $All{"$Param"}=1.0*InitParamTable->text($i,0);
	    $All{"$erradd$Param"}=1.0*InitParamTable->text($i,1);
	    $All{"$Param$minadd"}=1.0*InitParamTable->text($i,2);
	    $All{"$Param$maxadd"}=1.0*InitParamTable->text($i,3);
	}
    }
    
    
# Shared settings are detected here
    my $Shared = 0; 
    my $PCount =0;
    my $Component=1;
    foreach $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );
	
	if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
	    unshift( @Params, "Alpha" );
	}	
	elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    unshift( @Params, ( "N0", "NBg" ) );
	}
	
# This is the counter for parameters of this component
	my $NP=1;
	$Shared = 0;
# Change state/label of parameters
	foreach my $Param (@Params) {
	    my $Param_ORG = $Param;
# TODO: I need to take care of single hist fits here
	    if ( $All{"FitAsyType"} eq "SingleHist" ) {
		$Param=$Param.$Hists[0];	    
	    }
	    if ( $#FitTypes != 0 && (   $Param ne "Alpha" && $Param ne "N0" && $Param ne "NBg" ) ){
		$Param = join( "", $Param, "_", $Component);
	    }
	    
# Is there any point of sharing, multiple runs?
	    if ( $#RUNS == 0 && $All{"FitAsyType"} eq "Asymmetry") {
		$Shared = 1;
	    }
	    elsif ( $#RUNS == 0 && $#Hists == 0 &&  $All{"FitAsyType"} eq "SingleHist" )  {
		$Shared = 1;
	    } else {	
# Check if shared or not, construct name of checkbox, find its handle and then 
# check if it is checked		    
		my $ChkName="ShParam_".$Component."_".$NP;
		my $ChkBx = child($ChkName);
		$Shared = $ChkBx->isChecked();
	    }
	    $All{"Sh_$Param"}=$Shared;
	    $NP++;
	}
#Loop on parameters
	$Component++;
    }
# Loop on components
# Done with shared parameters detecting
    
# Construct a default filename if empty
    if ( $All{"FILENAME"} eq ""  && !$All{"RUNSType"}) {
	$All{"FILENAME"}=$RUNS[0]."_".$All{"BeamLine"}."_".$All{"YEAR"};
    } else {
	$All{"FILENAME"}="TMP";
    }
    
    if ( $All{"go"} eq "" ) {
	$All{"go"}="PLOT";
    }
    
# Get minimization process
    my $Min = Minimization->selectedId();
    if ($Min==0) {
	$All{"Minimization"}= "MINIMIZE";
	$All{"go"}="MIGRAD";
    }
    elsif ($Min==1) {
	$All{"Minimization"}= "MIGRAD";
	$All{"go"}="MIGRAD";
    }
    elsif ($Min==2) {
	$All{"Minimization"}= "SIMPLEX";
	$All{"go"}="SIMPLAEX";
    }
    
# Get Error calculation process
    my $Err = ErrorCalc->selectedId();
    if ($Err==0) {
	$All{"ErrorCalc"}= "HESSE";
#	$All{"go"}="MIGRAD";	
    }
    elsif ($Err==1) {
	$All{"ErrorCalc"}= "MINOS";
	$All{"go"}="MINOS";
    }
    
# Return Hash with all important values
    return %All;  

}

sub CallMSRCreate
{

    use MSR;
    my %All=CreateAllInput();
    if ($All{"RunNumbers"} ne ""  || $All{"RunFiles"} ne "") {
	if ( $All{"FitAsyType"} eq "Asymmetry" ) {
	    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateMSR(\%All);
	}
	elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
	    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateMSRSingleHist(\%All);
	}
	UpdateMSRFileInitTable();
    }

}

sub UpdateMSRFileInitTable
{

    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"};
    open (MSRF,q{<},"$FILENAME.msr" );
    my @lines = <MSRF>;
    close(IFILE);
    textMSROutput->setText("");
    foreach my $line (@lines) {
	textMSROutput->append("$line");
    }
    
    my $FPBlock_ref=MSR::ExtractParamBlk(@lines);
    my @FPBloc = @$FPBlock_ref;
    
    my $PCount=0;
    foreach my $line (@FPBloc) {
	$PCount++;
	my @Param=split(/\s+/,$line);
	
# Depending on home many elements in @Param determine what they mean
# 0th element is empty (always)
# 1st element is the order (always)	
# 2nd element is the name (always)
# 3rd element is the value (always)
# 4th element can be taken as step/error (always)
# 5th element can be
#  if it is last element or there are two more = positive error, check $#Param=5/7
#  if there is only one more                            = minimum, check $#Param=6
	
# To summarize, check the value of $#Param
	my $value=1.0*$Param[3];
	my $error = 1.0*$Param[4];
	my $minvalue=0.0;
	my $maxvalue=0.0;
	if ($#Param == 4) {
	    $minvalue=0.0;
	    $maxvalue=0.0;
	}
	elsif ($#Param == 6) {
	    $minvalue=1.0*$Param[5];
	    $maxvalue=1.0*$Param[6];
	}
	elsif ($#Param == 5 || $#Param == 7) {
	    $minvalue=1.0*$Param[6];
	    $maxvalue=1.0*$Param[7];
	}	    
# Now update the initialization tabel	
	InitParamTable->setText($PCount-1,0,$value);
	InitParamTable->setText($PCount-1,1,$error);
	InitParamTable->setText($PCount-1,2,$minvalue);
	InitParamTable->setText($PCount-1,3,$maxvalue);
    }
    return;

}

sub ActivateT0Hists
{

    my %All=CreateAllInput();
    my @Hists = split(/,/, $All{"LRBF"} );
    my $HistBox = "";
    for (my $iHist=1; $iHist<=4; $iHist++) {
	$HistBox="groupHist$iHist";
	my $HistBoxHandle = child($HistBox);
	if ($iHist<=$#Hists+1) {
# Activate this histogram box
	    $HistBoxHandle->setHidden(0);
	    $HistBoxHandle->setEnabled(1);
	    $HistBoxHandle->setTitle("Hist # $Hists[$iHist-1]");
	} else {
# Deactivate this histogram box
	    $HistBoxHandle->setHidden(1);
	    $HistBoxHandle->setEnabled(0);
	}
    }
    
# TODO: Set default values
    

}

sub ActivateShComp
{

    my %All=CreateAllInput();
    my @RUNS = split( /,/, $All{"RunNumbers"} );
    
# Hide all sharing components
    SharingComp1->setHidden(1);
    SharingComp2->setHidden(1);
    SharingComp3->setHidden(1);
    SharingComp1->setEnabled(0);
    SharingComp2->setEnabled(0);
    SharingComp3->setEnabled(0);
    
    my @FitTypes =();
    foreach my $FitType ($All{"FitType1"}, $All{"FitType2"}, $All{"FitType3"}) {
	if ( $FitType ne "None" ) {
	    push( @FitTypes, $FitType );	    
	}
    }
    
# Get number of parameters to determine the size of the table 
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
# For now the line below does not work. Why?    
#    my $Paramcomp_ref=$All{"Paramcomp_ref"};
    my @Paramcomp = @$Paramcomp_ref;
    my $Full_T_Block= $All{"Full_T_Block"};
    
    my $Component=1;
    foreach my $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );
	
	if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
	    unshift( @Params, "Alpha" );
	}
	elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    unshift( @Params, ( "N0", "NBg" ) );
	}
	
	
# Make the component appear first (only if we have multiple runs)
	my $ShCompG="SharingComp".$Component;
	my $ShCG = child($ShCompG);
	if ($#RUNS>0) {
	    $ShCG->setHidden(0);
	    $ShCG->setEnabled(1);
	}
	my $CompShLabel = "Comp".$Component."ShLabel";
	my $CompShL = child($CompShLabel);
	$CompShL->setText($All{"FitType$Component"});
	
# Change state/label of parameters
	for (my $i=1; $i<=9;$i++) {		
	    my $ParamChkBx="ShParam_".$Component."_".$i;
	    my $ChkBx = child($ParamChkBx);
	    if ($Params[$i-1] ne "") {
		$ChkBx->setHidden(0);
		$ChkBx->setEnabled(1);
		$ChkBx ->setText($Params[$i-1]);
	    } else {
		$ChkBx->setHidden(1);
	    }
	}
	$Component++;
    }  

}

sub InitializeTab
{

    my %All=CreateAllInput();
    InitParamTable->setLeftMargin(100);
    my $NRows = InitParamTable->numRows();
    
# Remove any rows in table
    if ($NRows > 0) {
	for (my $i=0;$i<$NRows;$i++) {
# TODO: Better remove the row rather than hide it.
	    InitParamTable->hideRow($i);
	}
    }
    
    my %PTable=MSR::PrepParamTable(\%All);
    
# Setup the table with the right size    
    my $NParam=scalar keys( %PTable );
    if ($NParam>$NRows) {	
	InitParamTable->setNumRows($NParam);
    }
    
# Fill the table with labels and values of parametr 
    for (my $PCount=0;$PCount<$NParam;$PCount++) {
	my ($Param,$value,$error,$minvalue,$maxvalue) = split(/,/,$PTable{$PCount});
	InitParamTable->verticalHeader()->setLabel( $PCount,$Param);
	InitParamTable->showRow($PCount);
	InitParamTable->setText($PCount,0,$value);
	InitParamTable->setText($PCount,1,$error);
	InitParamTable->setText($PCount,2,$minvalue);
	InitParamTable->setText($PCount,3,$maxvalue);
    }

}

sub TabChanged
{

# TODO: First check if there are some runs given, otherwise disbale
# TODO: Check if the MSR file exists and decide whether to use it or not
    my %All=CreateAllInput();
    
    my $SlectedTab = musrfit_tabs->currentPageIndex;
# Check if the option for checking for existing files is selected    
    my $FileExistCheck= FileExistCheck->isOn();
    my $FILENAME=$All{"FILENAME"}.".msr";
    if ($All{"RunNumbers"} ne "" && $SlectedTab==4 && $FileExistCheck==1) {
	if (-e $FILENAME) {
# Warning: MSR file exists
	    my $Warning = "Warning: MSR file $FILENAME Already exists!\nIf you continue it will overwriten.";
	    my $WarningWindow = Qt::MessageBox::information( this, "Warning",$Warning);
#	    my $Answer= Qt::MessageBox::warning( this, "Warning",$Warning, "&No", "&Yes", undef, 1,1);
# $Answer =1,0 for yes and no
#	    print "Answer=$Answer\n";
	}
    }
    
# First make sure we have sharing initialized    
    ActivateShComp();
    InitializeTab();
    UpdateMSRFileInitTable();
# And also setup T0 and Bg bins
    ActivateT0Hists();

}

sub GoFit
{

    my %All=CreateAllInput();
    CallMSRCreate();
    my $FILENAME=$All{"FILENAME"}.".msr";
    if (-e $FILENAME) {
	my $cmd="musrfit -t $FILENAME";
	my $pid = open(FTO,"$cmd 2>&1 |");
	while (<FTO>) {
	    FitTextOutput->append("$_");
#		print "line= ".$_;
	}
	close(FTO);
	$cmd="musrview $FILENAME &";
	$pid = system($cmd);
    } else {
	FitTextOutput->append("Cannot find MSR file!");
    }
    FitTextOutput->append("-----------------------------------------------------------------------------------------------------------------------------");
# update MSR File tab and initialization table
    UpdateMSRFileInitTable();
    return;

}

sub GoPlot
{

    my %All=CreateAllInput();
    CallMSRCreate();
    my $FILENAME=$All{"FILENAME"}.".msr";
    if (-e $FILENAME) {
	my $cmd="musrview $FILENAME &";
	my $pid = system($cmd);
    } else {
	FitTextOutput->append("Cannot find MSR file!");
	FitTextOutput->append("-----------------------------------------------------------------------------------------------------------------------------");
    }
    return;

}

sub ShowMuSRT0
{

    my %All=CreateAllInput();
# Create MSR file and then run musrt0
    CallMSRCreate();
    my $FILENAME=$All{"FILENAME"}.".msr";
    if (-e $FILENAME) {
	my $cmd="musrt0 $FILENAME &";
	my $pid = system($cmd);
    } else {
	print STDERR "Cannot find MSR file!\n";
    }
    return;

}

sub T0Update
{

    my %All = CreateAllInput();
    my @Hists = split(/,/, $All{"LRBF"} );
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","Bg1","Bg2","Data1","Data2") {
	    my $Name = "$_$NHist";
	    my $tmp=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
	    child($Name)->setText($tmp);
	}
	$NHist++
	    }
    

}

sub RunSelectionToggle
{

    my $ManualFile= ManualFile->isOn();
    if ($ManualFile) {
# Manual RUN selection
	RUNSMan->setEnabled(1);
	RUNSMan->setHidden(0);
	RunNumbers->setText("");
	RUNSAuto->setEnabled(0);
	RUNSAuto->setHidden(1);
    } else {
# Auto RUN selection
	RUNSMan->setEnabled(0);
	RUNSMan->setHidden(1);
	RunFiles->setText("");
	RUNSAuto->setEnabled(1);
	RUNSAuto->setHidden(0);
    }

}

sub fileBrowse
{

    my $files_ref=Qt::FileDialog::getOpenFileNames(
	    "Data files (*.root *.bin)",
	    "./",
	    this,
	    "open files dialog",
	    "Select one or more files to fit");
    my @files = @$files_ref;
    my $RunFiles=join(",",@files);
    RunFiles->setText($RunFiles);

}

1;


package main;

use Qt;
use MuSRFitform;

my $a = Qt::Application(\@ARGV);
my $w = MuSRFitform;
$a->setMainWidget($w);
$w->show;
exit $a->exec;
