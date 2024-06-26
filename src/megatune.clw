; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=userDefined
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "megatune.h"
LastPage=0
CDK=Y

ClassCount=32
Class1=CBarMeter
Class2=burstLog
Class3=Dabout
Class4=dataDialog
Class5=wueWizard
Class6=Dconplot
Class7=Dconstants
Class8=Denrichments
Class9=DlogOptions
Class10=Dreqfuel
Class11=Druntime
Class12=Dsetcomm
Class13=tuning3D
Class14=DveGen
Class15=genO2
Class16=genTherm
Class17=hexEdit
Class18=CMegatuneApp
Class19=CMegatuneDlg
Class20=scaleVe
Class21=selectList
Class22=tpgen
Class23=userDefined
Class24=userHelp
Class25=tableEditor

ResourceCount=40
Resource1=IDD_MEGATUNE_DIALOG
Resource2=IDD_HEXEDIT
Resource3=IDR_CURVE_EDITOR
Resource4=IDD_XFORM_TABLE
Resource5=IDR_GAUGE_POPUP
Resource6=IDD_BURSTING
Resource7=IDD_GENMAF
Resource8=IDD_CONSTANTS
Resource9=IDD_ABOUT
Resource10=IDD_USER_DEFINED
Resource11=IDD_INJ_SIZE
Resource12=IDD_SELECT_LIST
Resource13=IDD_PORTEDIT
Resource14=IDD_RUNTIME
Resource15=IDD_TABLE_EDITOR
Resource16=IDD_TUNING
Resource17=IDD_GENTHERM
Resource18=IDR_TABLE_EDITOR
Resource19=IDD_SETCOMM
Resource20=IDD_REQFUEL
Resource21=IDD_DATADIALOG
Resource22=IDD_TABLE_USE
Resource23=IDD_GENERATOR
Resource24=IDD_AE_WIZARD
Resource25=IDD_WUE_WIZARD
Resource26=IDD_MSQINFO
Class26=injectorSize
Resource27=IDD_LOG_OPTIONS
Class27=aeWizard
Resource28=IDD_USERHELP
Resource29=IDD_TP_CALIBRATE
Class28=triggerWizard
Resource30=IDD_ENRICHMENTS
Class29=msqInfoDlg
Resource31=IDR_TUNING_MENU
Resource32=IDD_INJCONTROL
Class30=portEdit
Resource33=IDD_SCALE_VE
Class31=miniTerm
Resource34=IDD_CONPLOT
Resource35=IDD_GENO2
Class32=curveEditor
Resource36=IDD_TRIGGER_WIZARD
Resource37=IDD_CURVE_EDITOR
Resource38=IDD_DVE_GEN
Resource39=IDD_MINITERM
Resource40=IDR_MEGATUNE

[CLS:CBarMeter]
Type=0
BaseClass=CCmdTarget
HeaderFile=BarMeter.h
ImplementationFile=BarMeter.cpp

[CLS:burstLog]
Type=0
BaseClass=dataDialog
HeaderFile=burstLog.h
ImplementationFile=burstLog.cpp

[CLS:Dabout]
Type=0
BaseClass=CDialog
HeaderFile=Dabout.h
ImplementationFile=Dabout.cpp

[CLS:dataDialog]
Type=0
BaseClass=CDialog
HeaderFile=dataDialog.h
ImplementationFile=dataDialog.cpp

[CLS:wueWizard]
Type=0
BaseClass=dataDialog
HeaderFile=wueWizard.h
ImplementationFile=wueWizard.cpp
Filter=D
VirtualFilter=dWC
LastObject=wueWizard

[CLS:Dconplot]
Type=0
BaseClass=CDialog
HeaderFile=Dconplot.h
ImplementationFile=Dconplot.cpp

[CLS:Dconstants]
Type=0
BaseClass=dataDialog
HeaderFile=Dconstants.h
ImplementationFile=Dconstants.cpp

[CLS:Denrichments]
Type=0
BaseClass=CDialog
HeaderFile=Denrichments.h
ImplementationFile=Denrichments.cpp

[CLS:DlogOptions]
Type=0
BaseClass=CDialog
HeaderFile=DlogOptions.h
ImplementationFile=DlogOptions.cpp

[CLS:Dreqfuel]
Type=0
BaseClass=dataDialog
HeaderFile=Dreqfuel.h
ImplementationFile=Dreqfuel.cpp

[CLS:Druntime]
Type=0
BaseClass=CDialog
HeaderFile=Druntime.h
ImplementationFile=Druntime.cpp
Filter=D
LastObject=Druntime

[CLS:Dsetcomm]
Type=0
BaseClass=dataDialog
HeaderFile=Dsetcomm.h
ImplementationFile=Dsetcomm.cpp
Filter=D

[CLS:tuning3D]
Type=0
BaseClass=CDialog
HeaderFile=tuning3D.h
ImplementationFile=tuning3D.cpp
Filter=D
LastObject=tuning3D

[CLS:DveGen]
Type=0
BaseClass=CDialog
HeaderFile=DveGen.h
ImplementationFile=DveGen.cpp
Filter=D

[CLS:genO2]
Type=0
BaseClass=dataDialog
HeaderFile=genO2.h
ImplementationFile=genO2.cpp
LastObject=IDC_VOLT1
Filter=D
VirtualFilter=dWC

[CLS:genTherm]
Type=0
BaseClass=dataDialog
HeaderFile=genTherm.h
ImplementationFile=genTherm.cpp

[CLS:hexEdit]
Type=0
BaseClass=CDialog
HeaderFile=hexedit.h
ImplementationFile=hexedit.cpp
Filter=D
VirtualFilter=dWC
LastObject=hexEdit

[CLS:CMegatuneApp]
Type=0
BaseClass=CWinApp
HeaderFile=megatune.h
ImplementationFile=megatune.cpp

[CLS:CMegatuneDlg]
Type=0
BaseClass=CDialog
HeaderFile=megatuneDlg.h
ImplementationFile=megatuneDlg.cpp

[CLS:transformTable]
Type=0
BaseClass=CDialog
HeaderFile=transformTable.h
ImplementationFile=transformTable.cpp

[CLS:scaleVe]
Type=0
BaseClass=CDialog
HeaderFile=scaleVe.h
ImplementationFile=scaleVe.cpp

[CLS:selectList]
Type=0
BaseClass=dataDialog
HeaderFile=selectList.h
ImplementationFile=selectList.cpp
LastObject=ID_COMMLOG

[CLS:tpgen]
Type=0
BaseClass=dataDialog
HeaderFile=tpgen.h
ImplementationFile=tpgen.cpp
Filter=D
VirtualFilter=dWC
LastObject=IDCANCEL

[CLS:userDefined]
Type=0
BaseClass=dataDialog
HeaderFile=userDefined.h
ImplementationFile=userDefined.cpp
LastObject=ID_AUTOTUNING

[CLS:userHelp]
Type=0
BaseClass=dataDialog
HeaderFile=userHelp.h
ImplementationFile=userHelp.cpp
Filter=D
VirtualFilter=dWC
LastObject=ID_COMMLOG

[CLS:tableEditor]
Type=0
BaseClass=dataDialog
HeaderFile=tableEditor.h
ImplementationFile=tableEditor.cpp

[DLG:IDD_BURSTING]
Type=1
Class=burstLog
ControlCount=3
Control1=IDCANCEL,button,1342242816
Control2=IDC_FILE_INFO,static,1342308352
Control3=IDC_MONITOR,static,1342308352

[DLG:IDD_ABOUT]
Type=1
Class=Dabout
ControlCount=2
Control1=IDC_ABOUT_TEXT,static,1342308352
Control2=IDOK,button,1342242817

[DLG:IDD_DATADIALOG]
Type=1
Class=dataDialog
ControlCount=2
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816

[DLG:IDD_WUE_WIZARD]
Type=1
Class=wueWizard
ControlCount=53
Control1=IDC_WUEBIN0,edit,1350631552
Control2=IDC_WUEBIN1,edit,1350631552
Control3=IDC_WUEBIN2,edit,1350631552
Control4=IDC_WUEBIN3,edit,1350631552
Control5=IDC_WUEBIN4,edit,1350631552
Control6=IDC_WUEBIN5,edit,1350631552
Control7=IDC_WUEBIN6,edit,1350631552
Control8=IDC_WUEBIN7,edit,1350631552
Control9=IDC_WUEBIN8,edit,1350631552
Control10=IDC_WUEBIN9,edit,1350631552
Control11=IDC_INCREMENT,button,1342242816
Control12=IDC_DECREMENT,button,1342242816
Control13=IDC_PRIMEP,edit,1350631552
Control14=IDC_CWU,edit,1350631552
Control15=IDC_CWH,edit,1350631552
Control16=IDC_FLOODCLEAR,edit,1350631552
Control17=IDC_AWEV,edit,1350631552
Control18=IDC_AWC,edit,1350631552
Control19=IDC_UP,button,1342242816
Control20=IDC_DOWN,button,1342242817
Control21=IDCANCEL,button,1342242816
Control22=IDC_STATIC,button,1342177287
Control23=IDC_PW1_LABEL,static,1342308352
Control24=IDC_PW2_LABEL,static,1342308352
Control25=IDC_FLOODCLEAR_LABEL,static,1342308352
Control26=IDC_STATIC,button,1342177287
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,button,1342177287
Control31=IDC_WUEBIN0_L,static,1342308354
Control32=IDC_WUEBIN1_L,static,1342308354
Control33=IDC_WUEBIN2_L,static,1342308354
Control34=IDC_WUEBIN3_L,static,1342308354
Control35=IDC_WUEBIN4_L,static,1342308354
Control36=IDC_WUEBIN5_L,static,1342308354
Control37=IDC_WUEBIN6_L,static,1342308354
Control38=IDC_WUEBIN7_L,static,1342308354
Control39=IDC_WUEBIN8_L,static,1342308354
Control40=IDC_WUEBIN9_L,static,1342308354
Control41=IDC_ARROW0,static,1342177294
Control42=IDC_COOLANT_LABEL,static,1342308352
Control43=IDC_STATIC,static,1342308352
Control44=IDC_EGO_BAR_LABEL,static,1342308352
Control45=IDC_STATIC,static,1342308352
Control46=IDC_CLT,edit,1342187648
Control47=IDC_WARMCOR,edit,1342187522
Control48=IDC_EGO,edit,1342187648
Control49=IDC_MAP,edit,1342187522
Control50=IDC_CLT_BAR,hysteresisBar,1350565889
Control51=IDC_WARMCOR_BAR,hysteresisBar,1350565889
Control52=IDC_EGO_BAR,hysteresisBar,1350565889
Control53=IDC_MAP_BAR,hysteresisBar,1350565889

[DLG:IDD_CONPLOT]
Type=1
Class=Dconplot
ControlCount=1
Control1=IDCANCEL,button,1342242817

[DLG:IDD_CONSTANTS]
Type=1
Class=Dconstants
ControlCount=41
Control1=IDB_REQFUEL,button,1342242816
Control2=IDC_REQFUEL,edit,1350631552
Control3=IDC_INJOPEN,edit,1350631552
Control4=IDC_BATTFAC,edit,1350631552
Control5=IDC_INJPWM,edit,1350631552
Control6=IDC_INJPWMT,edit,1350631552
Control7=IDC_FASTIDLE,edit,1350631552
Control8=IDC_BAROCORRENABLE,combobox,1342242819
Control9=IDC_ALGORITHM,combobox,1342242819
Control10=IDC_NSQUIRTS,combobox,1344339971
Control11=IDC_ALTERNATE,combobox,1342242819
Control12=IDC_TWOSTROKE,combobox,1342242819
Control13=IDC_NCYLINDERS,combobox,1344339971
Control14=IDC_INJTYPE,combobox,1342242819
Control15=IDC_NINJECTORS,combobox,1344339971
Control16=IDC_MAPTYPE,combobox,1342242819
Control17=IDC_ENGINETYPE,combobox,1342242819
Control18=IDC_UP,button,1342242816
Control19=IDC_DOWN,button,1342242816
Control20=IDCANCEL,button,1342242817
Control21=IDC_STATIC,button,1342177287
Control22=IDC_STATIC,button,1342177287
Control23=IDC_STATIC,static,1342308352
Control24=IDC_STATIC,static,1342308352
Control25=IDC_STATIC,static,1342308352
Control26=IDC_STATIC,button,1342177287
Control27=IDC_STATIC,static,1342308352
Control28=IDC_STATIC,static,1342308352
Control29=IDC_STATIC,static,1342308352
Control30=IDC_STATIC,static,1342308352
Control31=IDC_FIDLE_LABEL,static,1342308352
Control32=IDC_STATIC,button,1342177287
Control33=IDC_STATIC,static,1342308352
Control34=IDC_STATIC,static,1342308352
Control35=IDC_STATIC,static,1342308352
Control36=IDC_STATIC,static,1342308352
Control37=IDC_STATIC,static,1342308352
Control38=IDC_REQFUELDOWN,edit,1350568064
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,button,1342177287
Control41=IDC_STATIC,static,1342308352

[DLG:IDD_ENRICHMENTS]
Type=1
Class=Denrichments
ControlCount=71
Control1=IDC_PRIMEP,edit,1350631552
Control2=IDC_CWU,edit,1350631552
Control3=IDC_CWH,edit,1350631552
Control4=IDC_AWEV,edit,1350631552
Control5=IDC_AWC,edit,1350631552
Control6=IDC_EGOTYPE,combobox,1342242819
Control7=IDC_EGOSWITCHV,edit,1350631552
Control8=IDC_EGOTEMP,edit,1350631552
Control9=IDC_EGOCOUNTCMP,edit,1350631552
Control10=IDC_EGODELTA,edit,1350631552
Control11=IDC_EGOLIMIT,edit,1350631552
Control12=IDC_RPMOXLIMIT,edit,1350631552
Control13=IDC_WUEBIN0,edit,1350631552
Control14=IDC_WUEBIN1,edit,1350631552
Control15=IDC_WUEBIN2,edit,1350631552
Control16=IDC_WUEBIN3,edit,1350631552
Control17=IDC_WUEBIN4,edit,1350631552
Control18=IDC_WUEBIN5,edit,1350631552
Control19=IDC_WUEBIN6,edit,1350631552
Control20=IDC_WUEBIN7,edit,1350631552
Control21=IDC_WUEBIN8,edit,1350631552
Control22=IDC_WUEBIN9,edit,1350631552
Control23=IDC_TPSTHRESH,edit,1350631552
Control24=IDC_AETIME,edit,1350631552
Control25=IDC_TPSCOLDA,edit,1350631552
Control26=IDC_TPSCOLDM,edit,1350631552
Control27=IDC_TPSDQ,edit,1350631552
Control28=IDC_TAEBIN0,edit,1350631552
Control29=IDC_TAEBIN1,edit,1350631552
Control30=IDC_TAEBIN2,edit,1350631552
Control31=IDC_TAEBIN3,edit,1350631552
Control32=IDC_UP,button,1342242816
Control33=IDC_DOWN,button,1342242816
Control34=IDCANCEL,button,1342242817
Control35=IDC_STATIC,button,1342177287
Control36=IDC_PW1_LABEL,static,1342308352
Control37=IDC_PW2_LABEL,static,1342308352
Control38=IDC_STATIC,button,1342177287
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,button,1342177287
Control42=IDC_WUEBIN0_L,static,1342308354
Control43=IDC_WUEBIN1_L,static,1342308354
Control44=IDC_WUEBIN2_L,static,1342308354
Control45=IDC_WUEBIN3_L,static,1342308354
Control46=IDC_WUEBIN4_L,static,1342308354
Control47=IDC_WUEBIN5_L,static,1342308354
Control48=IDC_WUEBIN6_L,static,1342308354
Control49=IDC_WUEBIN7_L,static,1342308354
Control50=IDC_WUEBIN8_L,static,1342308354
Control51=IDC_WUEBIN9_L,static,1342308354
Control52=IDC_STATIC,button,1342177287
Control53=IDC_STATIC,static,1342308353
Control54=IDC_STATIC,static,1342308353
Control55=IDC_STATIC,static,1342308353
Control56=IDC_STATIC,static,1342308352
Control57=IDC_STATIC,static,1342308352
Control58=IDC_STATIC,static,1342308352
Control59=IDC_STATIC,static,1342308352
Control60=IDC_STATIC,static,1342308353
Control61=IDC_STATIC,button,1342177287
Control62=IDC_COOLANT_LABEL,static,1342308352
Control63=IDC_STATIC,static,1342308352
Control64=IDC_STATIC,static,1342308352
Control65=IDC_STATIC,static,1342308352
Control66=IDC_STATIC,static,1342308353
Control67=IDC_STATIC,static,1342308352
Control68=IDC_STATIC,static,1342308352
Control69=IDC_STATIC,static,1342308352
Control70=IDC_STATIC,static,1342308352
Control71=IDC_STATIC,static,1342308353

[DLG:IDD_LOG_OPTIONS]
Type=1
Class=DlogOptions
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_LOG_FILE_COMMENTS,edit,1352732676
Control3=IDC_LOG_DUMP_BEFORE,button,1342242819
Control4=IDC_LOG_DUMP_AFTER,button,1342242819
Control5=IDOK,button,1342242816
Control6=IDCANCEL,button,1342242816

[DLG:IDD_REQFUEL]
Type=1
Class=Dreqfuel
ControlCount=14
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_CID,edit,1350631552
Control6=IDC_DISP_UNITS_CID,button,1342373897
Control7=IDC_DISP_UNITS_CC,button,1342242825
Control8=IDC_NCYL,edit,1350568064
Control9=IDC_FLOW,edit,1350631552
Control10=IDC_INJ_UNITS_LBH,button,1342373897
Control11=IDC_INJ_UNITS_CCM,button,1342242825
Control12=IDC_AFR,edit,1350631552
Control13=IDOK,button,1342242817
Control14=IDCANCEL,button,1342242816

[DLG:IDD_RUNTIME]
Type=1
Class=Druntime
ControlCount=63
Control1=IDCANCEL,button,1342242817
Control2=IDC_SECONDS,edit,1342187648
Control3=IDC_BARO,edit,1342187648
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_MAP,edit,1342187522
Control7=IDC_STATIC,static,1342308352
Control8=IDC_MAT,edit,1342187522
Control9=IDC_MAT_LABEL,static,1342308352
Control10=IDC_CLT,edit,1342187648
Control11=IDC_COOLANT_LABEL,static,1342308352
Control12=IDC_TPS,edit,1342187522
Control13=IDC_RT_TPS,static,1342308352
Control14=IDC_BATT,edit,1342187648
Control15=IDC_STATIC,static,1342308352
Control16=IDC_RPM,edit,1342187522
Control17=IDC_STATIC,static,1342308352
Control18=IDC_PW,edit,1342187522
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,button,1342177287
Control21=IDC_EGOCOR,edit,1342187522
Control22=IDC_STATIC,static,1342177280
Control23=IDC_STATIC,button,1342177287
Control24=IDC_AIRCOR,edit,1342187522
Control25=IDC_STATIC,static,1342308352
Control26=IDC_BAROCOR,edit,1342187522
Control27=IDC_STATIC,static,1342308352
Control28=IDC_VECOR,edit,1342187522
Control29=IDC_STATIC,static,1342308352
Control30=IDC_WARMCOR,edit,1342187522
Control31=IDC_STATIC,static,1342308352
Control32=IDC_ACCELCOR,edit,1342187522
Control33=IDC_STATIC,static,1342308352
Control34=IDC_GAMMAECOR,edit,1342187648
Control35=IDC_STATIC,static,1342308352
Control36=IDC_STATIC,button,1342177287
Control37=IDC_RPM_BAR,hysteresisBar,1350565889
Control38=IDC_PW_BAR,hysteresisBar,1350565889
Control39=IDC_TPS_BAR,hysteresisBar,1350565889
Control40=IDC_MAT_BAR,hysteresisBar,1350565889
Control41=IDC_MAP_BAR,hysteresisBar,1350565889
Control42=IDC_CLT_BAR,hysteresisBar,1350565889
Control43=IDC_BATT_BAR,hysteresisBar,1350565889
Control44=IDC_GAMMA_BAR,hysteresisBar,1350565889
Control45=IDC_EGO_BAR_LABEL,static,1342308352
Control46=IDC_EGO,edit,1342187648
Control47=IDC_EGO_BAR,hysteresisBar,1350565889
Control48=IDC_CRANKING_TXT,static,1476526081
Control49=IDC_RUNNING_TXT,static,1476526081
Control50=IDC_WARMUP_TXT,static,1476526081
Control51=IDC_AFTERSTART_TXT,static,1476526081
Control52=IDC_ACCEL_TXT,static,1476526081
Control53=IDC_DECEL_TXT,static,1476526081
Control54=IDC_EGOCOR_BAR,hysteresisBar,1350565889
Control55=IDC_BAROCOR_BAR,hysteresisBar,1350565889
Control56=IDC_WARMCOR_BAR,hysteresisBar,1350565889
Control57=IDC_AIRCOR_BAR,hysteresisBar,1350565889
Control58=IDC_VECOR_BAR,hysteresisBar,1350565889
Control59=IDC_ACCELCOR_BAR,hysteresisBar,1350565889
Control60=IDC_DUTYCYCLE_BAR,hysteresisBar,1350565889
Control61=IDC_DUTYCYCLE,edit,1342187522
Control62=IDC_STATIC,static,1342308352
Control63=IDC_CONNECTED_TXT,static,1476526081

[DLG:IDD_SETCOMM]
Type=1
Class=Dsetcomm
ControlCount=12
Control1=IDC_SELECTCOMM,combobox,1344339971
Control2=IDC_TIMERINTERVAL,edit,1350631552
Control3=IDC_SELECTSPEED,combobox,1344339971
Control4=IDC_TESTCOMM,button,1342242816
Control5=IDCANCEL,button,1342242817
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_TEST_STATUS,edit,1342179456
Control11=IDC_STATIC,static,1342308352
Control12=IDC_COMM_WARNING,static,1342308353

[DLG:IDD_TUNING]
Type=1
Class=tuning3D
ControlCount=13
Control1=IDC_GAUGE_T1,static,1207959559
Control2=IDC_GAUGE_T3,static,1207959559
Control3=IDC_GAUGE_T5,static,1207959559
Control4=IDC_GAUGE_T2,static,1207959559
Control5=IDC_GAUGE_T4,static,1207959559
Control6=IDC_GAUGE_T6,static,1207959559
Control7=IDC_3DPLOT,static,1207959559
Control8=IDC_3DSTAT,static,1207959559
Control9=IDC_PAGE_A,button,1342308361
Control10=IDC_PAGE_B,button,1342177289
Control11=IDC_PAGE_C,button,1342177289
Control12=IDC_PAGE_D,button,1342177289
Control13=IDC_METEREGOBAR,static,1476395015

[DLG:IDD_DVE_GEN]
Type=1
Class=DveGen
ControlCount=24
Control1=IDC_VEG_DISP,edit,1350631552
Control2=IDC_DISP_UNITS_CID,button,1342373897
Control3=IDC_DISP_UNITS_CC,button,1342242825
Control4=IDC_VEG_IDLERPM,edit,1350631552
Control5=IDC_VEG_IDLEMAP,edit,1350631552
Control6=IDC_VEG_PTQ,edit,1350631552
Control7=IDC_VEG_PTQRPM,edit,1350631552
Control8=IDC_VEG_PTQMAP,edit,1350631552
Control9=IDC_VEG_PHP,edit,1350631552
Control10=IDC_VEG_PHPRPM,edit,1350631552
Control11=IDC_VEG_PHPMAP,edit,1350631552
Control12=IDC_VEG_REDRPM,edit,1350631552
Control13=IDC_VEG_REDMAP,edit,1350631552
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDOK,button,1342242817
Control23=IDCANCEL,button,1342242816
Control24=IDC_VEG_WARNING,static,1342308353

[DLG:IDD_GENO2]
Type=1
Class=genO2
ControlCount=14
Control1=IDC_TABLE_TYPE,combobox,1342242819
Control2=IDC_VOLT1,edit,1350631552
Control3=IDC_AFR1,edit,1350631552
Control4=IDC_VOLT2,edit,1350631552
Control5=IDC_AFR2,edit,1350631552
Control6=IDOK,button,1342242817
Control7=IDCANCEL,button,1342242816
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,button,1342177287
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_DOWNLOAD_STATUS,static,1342308352

[DLG:IDD_GENTHERM]
Type=1
Class=genTherm
ControlCount=18
Control1=IDC_TABLE_TYPE,combobox,1344339971
Control2=IDC_BIASR,edit,1350639744
Control3=IDC_TEMP_C,button,1342373897
Control4=IDC_TEMP_F,button,1342242825
Control5=IDC_TEMP1,edit,1350631552
Control6=IDC_RESI1,edit,1350639744
Control7=IDC_TEMP2,edit,1350631552
Control8=IDC_RESI2,edit,1350639744
Control9=IDC_TEMP3,edit,1350631552
Control10=IDC_RESI3,edit,1350639744
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,static,1342308352
Control16=IDC_TEMP_LABEL,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_DOWNLOAD_STATUS,static,1342308352

[DLG:IDD_HEXEDIT]
Type=1
Class=hexEdit
ControlCount=11
Control1=IDC_PAGE_NO,msctls_updown32,1342242848
Control2=IDC_PAGE_NO_TEXT,static,1342308352
Control3=IDC_BIN,button,1342308361
Control4=IDC_DEC,button,1342177289
Control5=IDC_HEX,button,1342177289
Control6=IDC_REALTIME,button,1342242819
Control7=IDC_FETCH,button,1342242816
Control8=IDCANCEL,button,1342242817
Control9=IDC_COUNTER,static,1342308352
Control10=IDC_BURN,button,1342242816
Control11=IDC_HEX_SCROLL,scrollbar,1342242817

[DLG:IDD_MEGATUNE_DIALOG]
Type=1
Class=CMegatuneDlg
ControlCount=13
Control1=IDC_FRAME_1,static,1073742080
Control2=IDC_FRAME_2,static,1073742080
Control3=IDC_FRAME_3,static,1073742080
Control4=IDC_FRAME_4,static,1073742080
Control5=IDC_FRAME_5,static,1073742080
Control6=IDC_FRAME_6,static,1073742080
Control7=IDC_FRAME_7,static,1073742080
Control8=IDC_FRAME_8,static,1073742080
Control9=IDC_STATUS_FILENAME,edit,1342179456
Control10=IDC_STATUS_SAVED,edit,1350568065
Control11=IDC_STATUS_LOGGING,edit,1350568065
Control12=IDC_STATUS_CONNECTED,edit,1350568065
Control13=IDC_LED_BAR,static,1073741831

[DLG:IDD_XFORM_TABLE]
Type=1
Class=transformTable
ControlCount=6
Control1=IDC_SCALE_TABLE,button,1342308361
Control2=IDC_XFORM_TABLE,button,1342177289
Control3=IDC_STATIC,static,1342308352
Control4=IDC_VALUE,edit,1350631552
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816

[DLG:IDD_SCALE_VE]
Type=1
Class=scaleVe
ControlCount=7
Control1=IDC_OLD_REQ_FUEL,edit,1350631552
Control2=IDC_NEW_REQ_FUEL,edit,1350631552
Control3=IDC_RESET_REQFUEL,button,1342242819
Control4=IDOK,button,1342242817
Control5=IDCANCEL,button,1342242816
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_SELECT_LIST]
Type=1
Class=selectList
ControlCount=4
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_CAPTION,static,1342308352
Control4=IDC_SEL_LIST,listbox,1352728833

[DLG:IDD_TP_CALIBRATE]
Type=1
Class=tpgen
ControlCount=8
Control1=IDC_TP_LO,edit,1350639744
Control2=IDC_GET_LO,button,1342242816
Control3=IDC_TP_HI,edit,1350639744
Control4=IDC_GET_HI,button,1342242816
Control5=IDOK,button,1342242817
Control6=IDCANCEL,button,1342242816
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352

[DLG:IDD_USER_DEFINED]
Type=1
Class=userDefined
ControlCount=64
Control1=IDC_UDL01,static,1342308352
Control2=IDC_UDL02,static,1342308352
Control3=IDC_UDL03,static,1342308352
Control4=IDC_UDL04,static,1342308352
Control5=IDC_UDL05,static,1342308352
Control6=IDC_UDL06,static,1342308352
Control7=IDC_UDL07,static,1342308352
Control8=IDC_UDL08,static,1342308352
Control9=IDC_UDL09,static,1342308352
Control10=IDC_UDL10,static,1342308352
Control11=IDC_UDL11,static,1342308352
Control12=IDC_UDL12,static,1342308352
Control13=IDC_UDL13,static,1342308352
Control14=IDC_UDL14,static,1342308352
Control15=IDC_UDL15,static,1342308352
Control16=IDC_UDL16,static,1342308352
Control17=IDC_UDL17,static,1342308352
Control18=IDC_UDL18,static,1342308352
Control19=IDC_UDL19,static,1342308352
Control20=IDC_UDL20,static,1342308352
Control21=IDC_UDV01,edit,1350631552
Control22=IDC_UDC01,combobox,1344339971
Control23=IDC_UDV02,edit,1350631552
Control24=IDC_UDC02,combobox,1344339971
Control25=IDC_UDV03,edit,1350631552
Control26=IDC_UDC03,combobox,1344339971
Control27=IDC_UDV04,edit,1350631552
Control28=IDC_UDC04,combobox,1344339971
Control29=IDC_UDV05,edit,1350631552
Control30=IDC_UDC05,combobox,1344339971
Control31=IDC_UDV06,edit,1350631552
Control32=IDC_UDC06,combobox,1344339971
Control33=IDC_UDV07,edit,1350631552
Control34=IDC_UDC07,combobox,1344339971
Control35=IDC_UDV08,edit,1350631552
Control36=IDC_UDC08,combobox,1344339971
Control37=IDC_UDV09,edit,1350631552
Control38=IDC_UDC09,combobox,1344339971
Control39=IDC_UDV10,edit,1350631552
Control40=IDC_UDC10,combobox,1344339971
Control41=IDC_UDV11,edit,1350631552
Control42=IDC_UDC11,combobox,1344339971
Control43=IDC_UDV12,edit,1350631552
Control44=IDC_UDC12,combobox,1344339971
Control45=IDC_UDV13,edit,1350631552
Control46=IDC_UDC13,combobox,1344339971
Control47=IDC_UDV14,edit,1350631552
Control48=IDC_UDC14,combobox,1344339971
Control49=IDC_UDV15,edit,1350631552
Control50=IDC_UDC15,combobox,1344339971
Control51=IDC_UDV16,edit,1350631552
Control52=IDC_UDC16,combobox,1344339971
Control53=IDC_UDV17,edit,1350631552
Control54=IDC_UDC17,combobox,1344339971
Control55=IDC_UDV18,edit,1350631552
Control56=IDC_UDC18,combobox,1344339971
Control57=IDC_UDV19,edit,1350631552
Control58=IDC_UDC19,combobox,1344339971
Control59=IDC_UDV20,edit,1350631552
Control60=IDC_UDC20,combobox,1344339971
Control61=ID_HELP_ABOUT,button,1342242816
Control62=IDC_UP,button,1342242816
Control63=IDC_DOWN,button,1342242817
Control64=IDCANCEL,button,1342242816

[DLG:IDD_USERHELP]
Type=1
Class=userHelp
ControlCount=3
Control1=IDCANCEL,button,1342242817
Control2=IDC_TEXT,edit,1352669188
Control3=IDC_WEBHELP,button,1342242816

[DLG:IDD_TABLE_EDITOR]
Type=1
Class=tableEditor
ControlCount=10
Control1=IDC_ZVAL,button,1342177287
Control2=IDC_YBIN,button,1342177287
Control3=IDC_XBIN,button,1342177287
Control4=IDC_COMMITTED,static,1342308353
Control5=IDC_BURN,static,1342308353
Control6=IDC_TE_ADD,button,1342242816
Control7=IDC_TE_SUB,button,1342242816
Control8=IDC_TE_MUL,button,1342242816
Control9=IDC_TE_SET,button,1342242816
Control10=IDC_TE_VAL,edit,1350631552

[DLG:IDD_TABLE_USE]
Type=1
Class=?
ControlCount=18
Control1=IDC_INJ1_OFF,button,1342308361
Control2=IDC_INJ1_1,button,1342177289
Control3=IDC_INJ1_2,button,1342177289
Control4=IDC_INJ1_GAMMA,button,1342242819
Control5=IDC_INJ2_OFF,button,1342308361
Control6=IDC_INJ2_1,button,1342177289
Control7=IDC_INJ2_2,button,1342177289
Control8=IDC_INJ2_GAMMA,button,1342242819
Control9=IDOK,button,1342242817
Control10=IDCANCEL,button,1342242816
Control11=IDC_STATIC,static,1342308352
Control12=IDC_STATIC,static,1342308352
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,static,1342308352
Control15=IDC_STATIC,static,1342308352
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_TABLE_MODE,button,1342242819

[MNU:IDR_MEGATUNE]
Type=1
Class=?
Command1=ID_FILE_OPEN
Command2=ID_FILE_SAVE
Command3=ID_FILE_SAVEAS
Command4=ID_MSQ_INFO
Command5=ID_OFFLINE
Command6=ID_DATALOG
Command7=ID_DATALOG_X
Command8=ID_DATALOG_OPT
Command9=ID_DATALOG_BURST
Command10=ID_VEEXPORT
Command11=ID_VEIMPORT
Command12=ID_COMMUNICATIONS_SETTINGS
Command13=ID_COMMLOG
Command14=ID_STD_MINITERM
Command15=ID_RUNTIME_TABLEEDIT
Command16=ID_FILE_DUMP
Command17=ID_TOOLS_REVIEW
Command18=ID_TOOLS_GENTP
Command19=ID_HELP_ABOUT
Command20=ID_HELP_TOPICS
CommandCount=20

[MNU:IDR_TABLE_EDITOR]
Type=1
Class=?
Command1=ID_TABLE_GET
Command2=ID_TABLE_PUT
Command3=ID_VEEXPORT
Command4=ID_VEIMPORT
Command5=ID_PLOT_VE
Command6=ID_XFORM_TABLE
Command7=ID_SCALE_VE
Command8=ID_GEN_VE
CommandCount=8

[ACL:IDR_MEGATUNE]
Type=1
Class=?
Command1=ID_DATALOG
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_ESCAPE
CommandCount=4

[DLG:IDD_INJ_SIZE]
Type=1
Class=injectorSize
ControlCount=25
Control1=IDCANCEL,button,1342242816
Control2=IDC_STATIC,static,1342308352
Control3=IDC_N_INJECTORS,edit,1350631552
Control4=IDC_STATIC,static,1342308352
Control5=IDC_OC_TIME,edit,1350631552
Control6=IDC_STATIC,static,1342308352
Control7=IDC_EVENTS_PER_CYCLE,edit,1350631552
Control8=IDC_DESIRED_DC,edit,1350631552
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,static,1342308352
Control11=IDC_STATIC,static,1342308352
Control12=IDC_RATED_FUEL_PRESSURE,edit,1350631552
Control13=IDC_ACTUAL_FUEL_PRESSURE,edit,1350631552
Control14=IDC_RESULT,static,1342308352
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_FUEL_SG,edit,1350631552
Control22=IDC_ESTIMATED_HP,edit,1350631552
Control23=IDC_BSFC,edit,1350631552
Control24=IDC_MAX_RPM,edit,1350631552
Control25=IDC_MAX_RPM_HP,edit,1350631552

[CLS:injectorSize]
Type=0
HeaderFile=injectorSize.h
ImplementationFile=injectorSize.cpp
BaseClass=dataDialog
Filter=D
LastObject=IDC_N_INJECTORS
VirtualFilter=dWC

[DLG:IDD_AE_WIZARD]
Type=1
Class=aeWizard
ControlCount=54
Control1=IDC_MAP_VAL4,edit,1350631552
Control2=IDC_MAP_VAL3,edit,1350631552
Control3=IDC_MAP_VAL2,edit,1350631552
Control4=IDC_MAP_VAL1,edit,1350631552
Control5=IDC_MAP_RATE4,edit,1350631552
Control6=IDC_MAP_RATE3,edit,1350631552
Control7=IDC_MAP_RATE2,edit,1350631552
Control8=IDC_MAP_RATE1,edit,1350631552
Control9=IDC_TPS_RATE4,edit,1350631552
Control10=IDC_TPS_RATE3,edit,1350631552
Control11=IDC_TPS_RATE2,edit,1350631552
Control12=IDC_TPS_RATE1,edit,1350631552
Control13=IDC_TPS_VAL4,edit,1350631552
Control14=IDC_TPS_VAL3,edit,1350631552
Control15=IDC_TPS_VAL2,edit,1350631552
Control16=IDC_TPS_VAL1,edit,1350631552
Control17=IDC_TPS_PROPORTION,msctls_trackbar32,1342242840
Control18=IDC_MAPTHRESH,edit,1350631552
Control19=IDC_AETIME,edit,1350631552
Control20=IDC_AETAPERTIME,edit,1350631552
Control21=IDC_ENDPULSEWIDTH,edit,1350631552
Control22=IDC_TPSTHRESH,edit,1350631552
Control23=IDC_TPSDQ,edit,1350631552
Control24=IDC_TPSCOLDA,edit,1350631552
Control25=IDC_TPSCOLDM,edit,1350631552
Control26=IDC_TABLE_GET,button,1342242816
Control27=IDC_TABLE_PUT,button,1342242816
Control28=IDCANCEL,button,1342242817
Control29=IDC_STATIC,static,1342308353
Control30=IDC_STATIC,static,1342308353
Control31=IDC_STATIC,static,1342308353
Control32=IDC_STATIC,static,1342308353
Control33=IDC_THRESHOLD_LABEL,static,1342308353
Control34=IDC_STATIC,static,1342308353
Control35=IDC_STATIC,static,1342308353
Control36=IDC_STATIC,static,1342308353
Control37=IDC_STATIC,static,1342308352
Control38=IDC_STATIC,static,1342308352
Control39=IDC_STATIC,static,1342308352
Control40=IDC_STATIC,static,1342308352
Control41=IDC_STATIC,static,1342308352
Control42=IDC_RATE_LABEL,static,1342308352
Control43=IDC_TPS_PROP_LABEL,static,1342308353
Control44=IDC_TPSDOT_RT,hysteresisBar,1350565893
Control45=IDC_MAPDOT_RT,hysteresisBar,1350565893
Control46=IDC_MAPDOT_LB,static,1342308354
Control47=IDC_TPSDOT_LB,static,1342308354
Control48=IDC_STATIC,static,1342308352
Control49=IDC_TPSDOT_UNITS,static,1342308352
Control50=IDC_MAPDOT_LO,static,1342308354
Control51=IDC_MAPDOT_HI,static,1342308354
Control52=IDC_TPSDOT_LO,static,1342308354
Control53=IDC_TPSDOT_HI,static,1342308354
Control54=IDC_LED_BAR,static,1073741831

[CLS:aeWizard]
Type=0
HeaderFile=aeWizard.h
ImplementationFile=aeWizard.cpp
BaseClass=dataDialog
Filter=D
VirtualFilter=dWC
LastObject=aeWizard

[DLG:IDD_INJCONTROL]
Type=1
Class=?
ControlCount=24
Control1=IDB_REQFUEL,button,1342242816
Control2=IDC_REQFUEL,edit,1350631552
Control3=IDC_ALGORITHM,combobox,1342242819
Control4=IDC_NSQUIRTS,combobox,1344339971
Control5=IDC_ALTERNATE,combobox,1342242819
Control6=IDC_TWOSTROKE,combobox,1342242819
Control7=IDC_NCYLINDERS,combobox,1344339971
Control8=IDC_INJTYPE,combobox,1342242819
Control9=IDC_NINJECTORS,combobox,1344339971
Control10=IDC_ENGINETYPE,combobox,1342242819
Control11=IDC_UP,button,1342242816
Control12=IDC_DOWN,button,1342242816
Control13=IDCANCEL,button,1342242817
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,button,1342177287
Control16=IDC_STATIC,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_STATIC,static,1342308352
Control19=IDC_STATIC,static,1342308352
Control20=IDC_STATIC,static,1342308352
Control21=IDC_STATIC,static,1342308352
Control22=IDC_STATIC,static,1342308352
Control23=IDC_STATIC,static,1342308352
Control24=IDC_REQFUELDOWN,edit,1350568064

[DLG:IDD_TRIGGER_WIZARD]
Type=1
Class=triggerWizard
ControlCount=7
Control1=IDC_ADVANCE,button,1342242944
Control2=IDC_RETARD,button,1342242944
Control3=IDC_TRIGGER_ANGLE,edit,1350631552
Control4=IDC_BURN,button,1342242816
Control5=IDCANCEL,button,1342242816
Control6=IDC_GAUGE,static,1073742080
Control7=IDC_STATIC,static,1342308352

[CLS:triggerWizard]
Type=0
HeaderFile=triggerWizard.h
ImplementationFile=triggerWizard.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=triggerWizard

[DLG:IDD_MSQINFO]
Type=1
Class=msqInfoDlg
ControlCount=2
Control1=IDC_INFO,edit,1353715716
Control2=IDCANCEL,button,1342242816

[CLS:msqInfoDlg]
Type=0
HeaderFile=msqInfoDlg.h
ImplementationFile=msqInfoDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=msqInfoDlg

[DLG:IDD_PORTEDIT]
Type=1
Class=portEdit
ControlCount=22
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_PEPORT,combobox,1344339971
Control8=IDC_PEENABLE,button,1342242819
Control9=IDC_PEVAR_1,combobox,1344339971
Control10=IDC_PECOND_1,combobox,1344339971
Control11=IDC_PETHRESH_1,edit,1350631552
Control12=IDC_PEHYST_1,edit,1350631552
Control13=IDC_PECONNECTOR,combobox,1344339971
Control14=IDC_PEVAR_2,combobox,1344339971
Control15=IDC_PECOND_2,combobox,1344339971
Control16=IDC_PETHRESH_2,edit,1350631552
Control17=IDC_PEHYST_2,edit,1350631552
Control18=IDC_PEINIT,combobox,1344339971
Control19=IDC_PEVALUE,combobox,1344339971
Control20=IDC_UP,button,1342242816
Control21=IDC_DOWN,button,1342242816
Control22=IDCANCEL,button,1342242817

[CLS:portEdit]
Type=0
HeaderFile=portEdit.h
ImplementationFile=portEdit.cpp
BaseClass=CDialog
Filter=D
LastObject=ID_COMMLOG

[DLG:IDD_MINITERM]
Type=1
Class=miniTerm
ControlCount=6
Control1=IDC_TEXT,edit,1352734724
Control2=IDC_STATIC,static,1342308352
Control3=IDC_SELECTCOMM,combobox,1344339971
Control4=IDC_STATIC,static,1342308352
Control5=IDC_SELECTSPEED,combobox,1344339971
Control6=IDCANCEL,button,1342242816

[CLS:miniTerm]
Type=0
HeaderFile=miniTerm.h
ImplementationFile=miniTerm.cpp
BaseClass=dataDialog
Filter=D
VirtualFilter=dWC
LastObject=ID_COMMLOG

[MNU:IDR_GAUGE_POPUP]
Type=1
Class=?
Command1=ID_POPUP_RESET_ONE
Command2=ID_POPUP_RESET_ALL
CommandCount=2

[DLG:IDD_CURVE_EDITOR]
Type=1
Class=curveEditor
ControlCount=38
Control1=IDC_X_LABEL,static,1342308865
Control2=IDC_Y_LABEL,static,1342308865
Control3=IDC_X_UNITS,static,1342308865
Control4=IDC_Y_UNITS,static,1342308865
Control5=IDC_CEX01,edit,1350631552
Control6=IDC_CEX02,edit,1350631552
Control7=IDC_CEX03,edit,1350631552
Control8=IDC_CEX04,edit,1350631552
Control9=IDC_CEX05,edit,1350631552
Control10=IDC_CEX06,edit,1350631552
Control11=IDC_CEX07,edit,1350631552
Control12=IDC_CEX08,edit,1350631552
Control13=IDC_CEX09,edit,1350631552
Control14=IDC_CEX10,edit,1350631552
Control15=IDC_CEX11,edit,1350631552
Control16=IDC_CEX12,edit,1350631552
Control17=IDC_CEX13,edit,1350631552
Control18=IDC_CEX14,edit,1350631552
Control19=IDC_CEX15,edit,1350631552
Control20=IDC_CEX16,edit,1350631552
Control21=IDC_CEY01,edit,1350631552
Control22=IDC_CEY02,edit,1350631552
Control23=IDC_CEY03,edit,1350631552
Control24=IDC_CEY04,edit,1350631552
Control25=IDC_CEY05,edit,1350631552
Control26=IDC_CEY06,edit,1350631552
Control27=IDC_CEY07,edit,1350631552
Control28=IDC_CEY08,edit,1350631552
Control29=IDC_CEY09,edit,1350631552
Control30=IDC_CEY10,edit,1350631552
Control31=IDC_CEY11,edit,1350631552
Control32=IDC_CEY12,edit,1350631552
Control33=IDC_CEY13,edit,1350631552
Control34=IDC_CEY14,edit,1350631552
Control35=IDC_CEY15,edit,1350631552
Control36=IDC_CEY16,edit,1350631552
Control37=IDC_GAUGE_FRAME,static,1207959559
Control38=IDC_CHART_FRAME,static,1207959559

[CLS:curveEditor]
Type=0
HeaderFile=curveEditor.h
ImplementationFile=curveEditor.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=curveEditor

[MNU:IDR_CURVE_EDITOR]
Type=1
Class=?
Command1=ID_TABLE_GET
Command2=ID_TABLE_PUT
Command3=ID_XFORM_TABLE
CommandCount=3

[MNU:IDR_TUNING_MENU]
Type=1
Class=?
Command1=ID_TABLE_GET
Command2=ID_TABLE_PUT
Command3=ID_VEEXPORT
Command4=ID_VEIMPORT
Command5=ID_AUTOTUNING
CommandCount=5

[DLG:IDD_GENERATOR]
Type=1
Class=?
ControlCount=18
Control1=IDC_STATIC,static,1342308864
Control2=IDC_GEN_LN,combobox,1342242819
Control3=IDC_GEN_UX,button,1342242819
Control4=IDC_STATIC,static,1342308864
Control5=IDC_X_UNITS,static,1342308864
Control6=IDC_Y_UNITS,static,1342308864
Control7=IDC_X_LABEL,static,1342308864
Control8=IDC_Y_LABEL,static,1342308864
Control9=IDC_STATIC,static,1342308864
Control10=IDC_GEN_I0,edit,1350631552
Control11=IDC_GEN_X0,edit,1350631552
Control12=IDC_GEN_Y0,edit,1350631552
Control13=IDC_GEN_I1,edit,1350631552
Control14=IDC_GEN_X1,edit,1350631552
Control15=IDC_GEN_Y1,edit,1350631552
Control16=IDC_GEN_OF,edit,1350631552
Control17=IDOK,button,1342242817
Control18=IDCANCEL,button,1342242816

[DLG:IDD_GENMAF]
Type=1
Class=?
ControlCount=18
Control1=IDC_TABLE_TYPE,combobox,1344339971
Control2=IDC_BIASR,edit,1350639744
Control3=IDC_MG_SEC,button,1342373897
Control4=IDC_GR_HR,button,1342242825
Control5=IDC_AF1,edit,1350631552
Control6=IDC_MAFVOLTS1,edit,1350639744
Control7=IDC_AF2,edit,1350631552
Control8=IDC_MAFVOLTS2,edit,1350639744
Control9=IDC_AF3,edit,1350631552
Control10=IDC_MAFVOLTS3,edit,1350639744
Control11=IDOK,button,1342242817
Control12=IDCANCEL,button,1342242816
Control13=IDC_STATIC,static,1342308352
Control14=IDC_STATIC,button,1342177287
Control15=IDC_STATIC,static,1342308352
Control16=IDC_TEMP_LABEL,static,1342308352
Control17=IDC_STATIC,static,1342308352
Control18=IDC_DOWNLOAD_STATUS,static,1342308352

