// TLemStats.h
//
// T.Prokscha, 25-April-2007
//
// Lem Statistics for LEM Run Header. Define own class which
// appears as a subdirectory in the /RunInfo/TLemRunHeader
// directory of a root file.
//
// use TObjString class: this has the advantage, that
// we can "see" the header in the root browser; also, have to
// "number" the lines so that they appear in the right order
// in TBrowser.
// Anyway, this is just a test - and a final "root" header
// definition should be closer to uSR Nexus definition?
//
//
#ifndef __TLemStats__
#define __TLemStats__

//#include <iostream>
//#include "TSystem.h"
#include "TObject.h"
#include "TObjString.h"
//#include "TMath.h"
//#include "TApplication.h"
//#include "TCanvas.h"
//#include "TText.h"
//#include "TPaveText.h"

#define  NHIST 32
//using std::cout;
//using std::endl;

//------------------------------------------------
// class for LEM statistics
class TLemStats : public TObject {

private:
   TObjString fIpSumString;          //sum of Ip counts in one run
   Double_t   fIpSum;
   TObjString fScalerClockSumString; //sum of 10kHz counts in one run
   Double_t   fScalerClockSum;       //extracted from VME scaler module
   TObjString fTdcClockSumString;    //sum of 10kHz counts in one run,
   Double_t   fTdcClockSum;          //extracted from VME TDC
   TObjString fNeventsString;        //total number of slow muon events
   Double_t   fNevents;
   TObjString fTdSumString;          //total number of TD hits (muon start detector for LEM)
   Double_t   fTdSum;
   TObjString fTdCleanSumString;     //total number of TD hits without pre-pileup
   Double_t   fTdCleanSum;
   TObjString fTdGoodSumString;      //total number of TD hirs without pre- and post-pileup
   Double_t   fTdGoodSum;
   TObjString fPosSumString;         //total number of slow muon events
   Double_t   fPosSum[NHIST];        //total number of positron detector hits inLEM detectors
   Int_t      fNPosDetectors;        //number of e+ detectors/histograms

public:
   TLemStats();
   virtual ~TLemStats();
   virtual Bool_t IsFolder() const { return kTRUE; } // make object "browsable"

   virtual void SetIpSum(Double_t value);
   virtual void SetScalerClockSum(Double_t value);
   virtual void SetTdcClockSum(Double_t value);
   virtual void SetNevents(Double_t value);
   virtual void SetTdSum(Double_t value);
   virtual void SetTdCleanSum(Double_t value);
   virtual void SetTdGoodSum(Double_t value);
   virtual void SetPosSum(const Double_t *value);
   virtual void SetNPosDetectors(Int_t n) { fNPosDetectors = n; }

   virtual Double_t GetIpSum() const          { return fIpSum; }
   virtual Double_t GetScalerClockSum() const { return fScalerClockSum; }
   virtual Double_t GetTdcClockSum() const    { return fTdcClockSum; }
   virtual Double_t GetNevents() const        { return fNevents; }
   virtual Double_t GetTdSum() const          { return fTdSum; }
   virtual Double_t GetTdCleanSum() const     { return fTdCleanSum; }
   virtual Double_t GetTdGoodSum() const      { return fTdGoodSum; }
   virtual Double_t *GetPosSum()              { return fPosSum; }
   TObjString       GetIpSumString() const    { return fIpSumString; }
   virtual Int_t GetNPosDetectors() const     { return fNPosDetectors;}

   //2nd version of TLemStats
   ClassDef(TLemStats,2) //LEM Statistics
};
#endif
