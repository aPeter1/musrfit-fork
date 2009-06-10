// TLemStats.C
//
// T. Prokscha
// 25/04/2007
//
#include "TLemStats.h"

ClassImp(TLemStats)

//----------------------------------------------------------
TLemStats::TLemStats(){
 fIpSum = fScalerClockSum = fTdcClockSum = fTdSum = 1.;
 fTdCleanSum = fTdGoodSum = fNevents = 1.;
 for (int i=0; i<NHIST; i++)
   fPosSum[i] = -1.;
}
//----------------------------------------------------------
TLemStats::~TLemStats(){
}
//----------------------------------------------------------
void TLemStats::SetIpSum(Double_t value){
 TString str;
 fIpSum = value;
 str = "20 Sum Ip             : ";
 str += value;
 str += " ";

 if (fScalerClockSum > 0. ){
  str += value/fScalerClockSum*0.1;
  str += " /mAs";
 }

 fIpSumString.SetString(str);
 //printf("String = %s\n", (const char*) str);
}
//----------------------------------------------------------
void TLemStats::SetScalerClockSum(Double_t value){
 TString str;
 fScalerClockSum = value;
 str = "21 Sum Clock (Scaler)  : ";
 str += value;
 fScalerClockSumString.SetString(str);
 //printf("String = %s\n", (const char*) str);
}
//----------------------------------------------------------
void TLemStats::SetTdcClockSum(Double_t value){
 TString str;
 fTdcClockSum = value;
 str = "22 Sum Clock (TDC)     : ";
 str += value;
 fTdcClockSumString.SetString(str);
 //printf("String = %s\n", (const char*) str);
}
//----------------------------------------------------------
void TLemStats::SetNevents(Double_t value){
 TString str;
 fNevents = value;
 str = "23 Sum Slow Muon Events: ";
 str += value;
 fNeventsString.SetString(str);
}
//----------------------------------------------------------
void TLemStats::SetTdSum(Double_t value){
 TString str;
 fTdSum = value;
 str = "24 Sum TD              : ";
 str += value;
 fTdSumString.SetString(str);
}
//----------------------------------------------------------
void TLemStats::SetTdCleanSum(Double_t value){
 TString str;
 fTdCleanSum = value;
 str = "25 Sum TD_Clean        : ";
 str += value;
 fTdCleanSumString.SetString(str);
}
//----------------------------------------------------------
void TLemStats::SetTdGoodSum(Double_t value){
 TString str;
 fTdGoodSum = value;
 str = "26 Sum TD_Good         : ";
 str += value;
 fTdGoodSumString.SetString(str);
}
//----------------------------------------------------------
void TLemStats::SetPosSum(const Double_t *value){
 TString str;
 str = "27 Sum Positrons       : ";
 for (Int_t i = 0; i < fNPosDetectors; i++){
  fPosSum[i] = value[i];
  str += value[i];
  str += "   ";
 }
 fPosSumString.SetString(str);
}
//----------------------------------------------------------
