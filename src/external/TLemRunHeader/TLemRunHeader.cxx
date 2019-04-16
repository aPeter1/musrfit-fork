// TLemRunHeader.C
//
// T. Prokscha
// 18/06/2006
//
//
#include "TLemRunHeader.h"

ClassImp(TLemRunHeader)

//----------------------------------------------------------
TLemRunHeader::TLemRunHeader(){
 fRunTitle.SetString("");
 fLemSetup.SetString("");
 fStartTimeString.SetString("");
 fStopTimeString.SetString("");
 fOffsetPPCHistograms = 20;
 fRunNumber = 0; // -1 not possible since UInt_t
 fRunNumberString.SetString("");
 fStartTime = 0; // -1 not possible since UInt_t
 fStopTime  = 0; // -1 not possible since UInt_t
 fModeratorHV       = -999.0;
 fModeratorHVError  = -999.0;
 fModeratorHVString.SetString("");
 fSampleHV          = -999.0;
 fSampleHVError     = -999.0;
 fSampleHVString.SetString("");
 fImpEnergy         = -999.0;
 fImpEnergyString.SetString("");
 fSampleTemperature = -999.0;
 fSampleTemperatureError = -999.0;
 fSampleTemperatureString.SetString("");
 fSampleBField      = -999.0;
 fSampleBFieldError = -999.0;
 fSampleBFieldString.SetString("");
 fTimeResolution    = -999.0;
 fTimeResolutionString.SetString("");
 fNChannels = -1;
 fNChannelsString.SetString("");
 fNHist     = -1;
 fNHistString.SetString("");
 fOffsetPPCHistograms = 20;
 fOffsetPPCHistogramsString.SetString("");
 fCuts.SetString("");
 fModerator.SetString("");
 for (int i=0; i<NHIST; i++)
   fTimeZero[i] = -1.;
 fTimeZeroString.SetString("");
}
//----------------------------------------------------------
TLemRunHeader::~TLemRunHeader(){
 //fTimeZero.clear();
}
//----------------------------------------------------------
void TLemRunHeader::SetRunTitle(const Char_t *title){
 TString str;
 str = "00 Title: ";
 str += title;
 fRunTitle.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetLemSetup(const Char_t *setup){
 TString str;
 str = "01 Setup: ";
 str += setup;
 fLemSetup.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetRunNumber(UInt_t runNo){
 TString str;
 fRunNumber = runNo;
 str = "02 Run Number: ";
 str += runNo;
 fRunNumberString.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetStartTimeString(const Char_t *start){
 TString str;
 str = "03 Run Start Time: ";
 str += start;
 fStartTimeString.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetStopTimeString(const Char_t *stop){
 TString str;
 str = "04 Run Stop Time: ";
 str += stop;
 fStopTimeString.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetModeratorHV(Float_t modHV, Float_t error){
 TString str;
 char s[80];
 sprintf(s, "05 Moderator HV: %8.2f(%5.2f)", modHV, error);
 str = s;
 fModeratorHVString.SetString(str);
 fModeratorHV = modHV;
 fModeratorHVError = error;
}
//----------------------------------------------------------
void TLemRunHeader::SetSampleHV(Float_t value, Float_t error){
 TString str;
 char s[80];
 sprintf(s,"06 Sample HV: %8.2f(%5.2f)", value, error);
 str = s;
 fSampleHVString.SetString(str);
 fSampleHV = value;
 fSampleHVError = error;
}
//----------------------------------------------------------
void TLemRunHeader::SetImpEnergy(Float_t value){
 TString str;
 char s[80];
 sprintf(s,"07 Impl. Energy: %8.2f", value);
 str = s;
 fImpEnergyString.SetString(str);
 fImpEnergy = value;
}
//----------------------------------------------------------
void TLemRunHeader::SetSampleTemperature(Float_t value, Float_t error){
 TString str;
 char s[80];
 sprintf(s,"08 Sample T: %8.2f(%5.2f)", value, error);
 str = s;
 fSampleTemperatureString.SetString(str);
 fSampleTemperature      = value;
 fSampleTemperatureError = error;
}
//----------------------------------------------------------
void TLemRunHeader::SetSampleBField(Float_t value, Float_t error){
 TString str;
 char s[80];
 sprintf(s,"09 Sample B: %8.2f(%5.2f)", value, error);
 str = s;
 fSampleBFieldString.SetString(str);
 fSampleBField      = value;
 fSampleBFieldError = error;
}
//----------------------------------------------------------
void TLemRunHeader::SetTimeResolution(Float_t value){
 TString str;
 char s[80];
 sprintf(s,"10 Time Res.: %10.7f", value);
 str = s;
 fTimeResolutionString.SetString(str);
 fTimeResolution = value;
}
//----------------------------------------------------------
void TLemRunHeader::SetNChannels(Int_t value){
 TString str;
 char s[80];
 sprintf(s,"11 N Channels: %8d", value);
 str = s;
 fNChannelsString.SetString(str);
 fNChannels = value;
}
//----------------------------------------------------------
void TLemRunHeader::SetNHist(Int_t value){
 TString str;
 char s[80];
 sprintf(s,"12 N Histograms: %6d", value);
 str = s;
 fNHistString.SetString(str);
 fNHist = value;
 SetNPosDetectors(fNHist);
}
//----------------------------------------------------------
void TLemRunHeader::SetOffsetPPCHistograms(UInt_t value){
 TString str;
 char s[80];
 sprintf(s,"13 Offset PPC Histograms: %6d", value);
 str = s;
 fOffsetPPCHistogramsString.SetString(str);
 fOffsetPPCHistograms = value;
}
//----------------------------------------------------------
void TLemRunHeader::SetCuts(const Char_t *cuts){
 TString str;
 str = "14 Cuts: ";
 str += cuts;
 fCuts.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetModerator(const Char_t *moderator){
 TString str;
 str = "15 Moderator: ";
 str += moderator;
 fModerator.SetString(str);
}
//----------------------------------------------------------
void TLemRunHeader::SetTimeZero(const Double_t *value){
 Int_t noT0s = fNHist;
 if (noT0s > NHIST)
   noT0s = NHIST;
 TString str;
 str = "16 t0: ";
 for (Int_t i = 0; i < noT0s; i++){
   //fTimeZero.push_back(value[i]);
  fTimeZero[i] = value[i];
  str   += value[i];
  str   += "   ";
 }
 fTimeZeroString.SetString(str);
}
//----------------------------------------------------------
TObjString TLemRunHeader::GetRunTitle() const {
 TObjString ostr;
 TString    str;
 Int_t      delIndex;

 str = fRunTitle.GetString();
 delIndex = str.First(":") + 2;
 str.Remove(0, delIndex);
 ostr.SetString(str);
 return ostr;
}
//----------------------------------------------------------
TObjString TLemRunHeader::GetLemSetup() const {
 TObjString ostr;
 TString    str;
 Int_t      delIndex;

 str = fLemSetup.GetString();
 delIndex = str.First(":") + 2;
 str.Remove(0, delIndex);
 ostr.SetString(str);
 return ostr;
}
//----------------------------------------------------------
//Int_t TLemRunHeader::GetRunNumber() const {
// TString str;
// Int_t   delIndex;
//
// str      = fRunNumberString.GetString();
// delIndex = str.First(":") + 1;
// str.Remove(0, delIndex);
// return str.Atoi();
//}
//----------------------------------------------------------
TObjString TLemRunHeader::GetStartTimeString() const {
 TObjString ostr;
 TString    str;
 Int_t      delIndex;

 str = fStartTimeString.GetString();
 delIndex = str.First(":") + 2;
 str.Remove(0, delIndex);
 ostr.SetString(str);
 return ostr;
}
//----------------------------------------------------------
TObjString TLemRunHeader::GetStopTimeString() const {
 TObjString ostr;
 TString    str;
 Int_t      delIndex;

 str = fStopTimeString.GetString();
 delIndex = str.First(":") + 2;
 str.Remove(0, delIndex);
 ostr.SetString(str);
 return ostr;
}
//----------------------------------------------------------
TObjString TLemRunHeader::GetCuts() const {
 TObjString ostr;
 TString    str;
 Int_t      delIndex;

 str = fCuts.GetString();
 delIndex = str.First(":") + 2;
 str.Remove(0, delIndex);
 ostr.SetString(str);
 return ostr;
}
//----------------------------------------------------------
TObjString TLemRunHeader::GetModerator() const {
 TObjString ostr;
 TString    str;
 Int_t      delIndex;

 str = fModerator.GetString();
 delIndex = str.First(":") + 2;
 str.Remove(0, delIndex);
 ostr.SetString(str);
 return ostr;
}
//----------------------------------------------------------
void TLemRunHeader::DumpHeader() const {
 TObjString  oTitle, oSetup, oStart, oStop, oCuts, oMod;
 const char  *title, *setup, *runStart, *runStop, *cuts, *mod;

 oTitle   = GetRunTitle();
 title    = oTitle.GetName();
 oSetup   = GetLemSetup();
 setup    = oSetup.GetName();
 oStart   = GetStartTimeString();
 runStart = oStart.GetName();
 oStop    = GetStopTimeString();
 runStop  = oStop.GetName();
 oCuts    = GetCuts();
 cuts     = oCuts.GetName();
 oMod     = GetModerator();
 mod      = oMod.GetName();

  printf("Title:         %s\n",title);
  printf("Setup:         %s\n",setup);
  printf("Run Number:    %04d\n", GetRunNumber());
  printf("Start Time:    %s\n", runStart);
  printf("Stop Time:     %s\n", runStop);
  printf("Moderator HV: %10.2f kV\n",  fModeratorHV);
  printf("Sample HV:    %10.2f kV\n",  fSampleHV);
  printf("Impl. Energy: %10.2f keV\n", fImpEnergy);
  printf("Sample T:     %10.2f K\n",   fSampleTemperature);
  printf("Sample B:     %10.2f G\n",   fSampleBField);
  printf("Time Res.:    %10.7f ns\n",  fTimeResolution);
  printf("N Channels:   %10d\n", fNChannels);
  printf("N Histograms: %10d\n", fNHist);
  printf("Offset PPC Histograms: %10d\n", fOffsetPPCHistograms);
  printf("Cuts:         %s\n", cuts);
  printf("Moderator:    %s\n", mod);
  printf("t0: ");
  for (Int_t i=0; i<fNHist; i++)
     printf(" %7.2f ", fTimeZero[i]);
  printf("\n");
}
//----------------------------------------------------------
//void TLemRunHeader::Browse(TBrowser *b){
void TLemRunHeader::DrawHeader() const {
 TPaveText *pt;
 TText *text;
 TCanvas *ca;
 Char_t  str[256], helpstr[256];
 TObjString  oTitle, oSetup, oStart, oStop, oCuts, oMod;
 const char  *title, *setup, *runStart, *runStop, *cuts, *mod;

 oTitle   = GetRunTitle();
 title    = oTitle.GetName();
 oSetup   = GetLemSetup();
 setup    = oSetup.GetName();
 oStart   = GetStartTimeString();
 runStart = oStart.GetName();
 oStop    = GetStopTimeString();
 runStop  = oStop.GetName();
 oCuts    = GetCuts();
 cuts     = oCuts.GetName();
 oMod     = GetModerator();
 mod      = oMod.GetName();

 ca = new TCanvas("LEM RunHeader","LEM RunHeader", 147,37,699,527);
 ca->Range(0., 0., 100., 100.);

 pt = new TPaveText(10.,10.,90.,90.,"br");
 pt->SetFillColor(19);
 pt->SetTextAlign(12);
 strcpy(str, "Title: ");
 strcat(str, title);
 text = pt->AddText(str);
 strcpy(str, "Setup: ");
 strcat(str, setup);
 text = pt->AddText(str);
 strcpy(str, "Run Start: ");
 strcat(str, runStart);
 text = pt->AddText(str);
 strcpy(str, "Run Stop: ");
 strcat(str, runStop);
 text = pt->AddText(str);
 sprintf(str, "Run Number:   %10d", GetRunNumber());
 text = pt->AddText(str);
 sprintf(str, "Moderator HV: %10.2f kV", fModeratorHV);
 text = pt->AddText(str);
 sprintf(str, "Sample HV:    %10.2f kV", fSampleHV);
 text = pt->AddText(str);
 sprintf(str, "Impl. Energy: %10.2f keV", fImpEnergy);
 text = pt->AddText(str);
 sprintf(str, "Sample T:     %10.2f K", fSampleTemperature);
 text = pt->AddText(str);
 sprintf(str, "Sample B:     %10.2f G", fSampleBField);
 text = pt->AddText(str);
 sprintf(str, "Time Res.:    %10.7f ns", fTimeResolution);
 text = pt->AddText(str);
 sprintf(str, "N Channels:   %10d", fNChannels);
 text = pt->AddText(str);
 sprintf(str, "N Histograms: %10d", fNHist);
 text = pt->AddText(str);
 sprintf(str, "Offset PPC Histograms: %10d", fOffsetPPCHistograms);
 text = pt->AddText(str);
 strcpy(str, "Cuts: ");
 strcat(str, cuts);
 text = pt->AddText(str);
 strcpy(str, "Moderator: ");
 strcat(str, mod);
 text = pt->AddText(str);

 strcpy(str,"t0: ");
 for (Int_t i=0; i<fNHist; i++){
   sprintf(helpstr, " %7.2f ", fTimeZero[i]);
   strcat(str, helpstr);
 }
 text = pt->AddText(str);

 pt->Draw();

 ca->Modified(kTRUE);

}
//----------------------------------------------------------
