// TLemRunHeader.h
//
// T.Prokscha, 18-June-2006
//
// Lem Run header test. It inherits now from class
// TLemStats to have important scaler data as well included.
//
// use TObjString class: this has the advantage, that
// we can "see" the header in the root browser; also, have to
// "number" the lines so that they appear in the right order
// in TBrowser.
// Anyway, this is just a test - and a final "root" header
// definition should be closer to uSR Nexus definition?
//
//
#ifndef __TLemRunHeader__
#define __TLemRunHeader__
//#include <iostream>
//#include <vector>
#include "TObjString.h"
#include "TCanvas.h"
#include "TText.h"
#include "TPaveText.h"
#include "TLemStats.h"

//#define NHIST 32
//using std::cout;
//using std::endl;
//using std::vector;

//------------------------------------------------
// now the main class definition
class TLemRunHeader : public TLemStats {

private:
   TObjString fRunTitle; // LEM Run Title
   TObjString fLemSetup; // LEM Setup
   TObjString fStartTimeString; // Run Start time
   TObjString fStopTimeString;  // Run Stop time
   UInt_t     fOffsetPPCHistograms; // ID offset for post-pileup rejected histograms, default 20
   TObjString fOffsetPPCHistogramsString;
   UInt_t     fRunNumber;
   TObjString fRunNumberString;
   UInt_t     fStartTime;       // ASCII of start time
   UInt_t     fStopTime;        // ASCII of stop time
   Float_t    fModeratorHV, fModeratorHVError;
   TObjString fModeratorHVString;
   Float_t    fSampleHV, fSampleHVError;
   TObjString fSampleHVString;
   Float_t    fImpEnergy;
   TObjString fImpEnergyString;
   Float_t    fSampleTemperature, fSampleTemperatureError;
   TObjString fSampleTemperatureString;
   Float_t    fSampleBField, fSampleBFieldError;
   TObjString fSampleBFieldString;
   Float_t    fTimeResolution; //time resolution in ns
   TObjString fTimeResolutionString;
   Int_t      fNChannels; // number of channels in one histogram
   TObjString fNChannelsString;
   Int_t      fNHist;     // number of histograms
   TObjString fNHistString;
   TObjString fCuts;
   TObjString fModerator;
//   vector<Double_t> fTimeZero; // time zero's of e+ detectors
   Double_t fTimeZero[NHIST]; // time zero's of e+ detectors
   TObjString fTimeZeroString;  //

public:
   TLemRunHeader();
   virtual ~TLemRunHeader();
   virtual Bool_t IsFolder() const { return kTRUE; } // make object "browsable"
   //virtual void   Browse(TBrowser *b);
   virtual void SetRunTitle(const Char_t *title);
   virtual void SetLemSetup(const Char_t *setup);
   virtual void SetRunNumber(UInt_t runNo);
   virtual void SetStartTimeString(const Char_t *start);
   virtual void SetStopTimeString(const Char_t *stop);
   virtual void SetOffsetPPCHistograms(UInt_t value);
   virtual void SetStartTime(UInt_t value) { fStartTime = value; }
   virtual void SetStopTime(UInt_t value)  { fStopTime = value; }
   virtual void SetModeratorHV(Float_t modHV, Float_t error);
   virtual void SetSampleHV(Float_t value, Float_t error);
   virtual void SetImpEnergy(Float_t value);
   virtual void SetSampleTemperature(Float_t value, Float_t error);
   virtual void SetSampleBField(Float_t value, Float_t error);
   virtual void SetTimeResolution(Float_t value);
   virtual void SetNChannels(Int_t value);
   virtual void SetNHist(Int_t value);
   virtual void SetCuts(const Char_t *cuts);
   virtual void SetModerator(const Char_t *moderator);
   virtual void SetTimeZero(const Double_t *value);
   TObjString GetStartTimeString() const;
   TObjString GetStopTimeString() const ;
   TObjString      GetRunTitle() const;
   TObjString      GetLemSetup() const;
   virtual UInt_t  GetRunNumber() const { return fRunNumber; }
   virtual UInt_t  GetOffsetPPCHistograms() const { return fOffsetPPCHistograms; }
   virtual UInt_t  GetStartTime() const { return fStartTime; }
   virtual UInt_t  GetStopTime() const  { return fStopTime; }
   virtual Int_t   GetNChannels() const { return fNChannels; }
   virtual Int_t   GetNHist() const     { return fNHist; }
   virtual Float_t GetModeratorHV() const { return fModeratorHV; }
   virtual Float_t GetSampleHV() const  { return fSampleHV; }
   virtual Float_t GetModeratorHVError() const { return fModeratorHVError; }
   virtual Float_t GetSampleHVError() const    { return fSampleHVError; }
   virtual Float_t GetImpEnergy() const        { return fImpEnergy; }
   virtual Float_t GetSampleTemperature() const { return fSampleTemperature; }
   virtual Float_t GetSampleBField() const     { return fSampleBField; }
   virtual Float_t GetSampleTemperatureError() const { return fSampleTemperatureError; }
   virtual Float_t GetSampleBFieldError() const { return fSampleBFieldError; }
   virtual Float_t GetTimeResolution() const { return fTimeResolution; }
   TObjString      GetCuts() const;
   TObjString      GetModerator() const;
//   virtual Double_t  *GetTimeZero() { return &fTimeZero.front(); }
   virtual Double_t  *GetTimeZero() { return fTimeZero; }

   virtual void DumpHeader() const;
   virtual void DrawHeader() const;

   // 5th version including ID offset for PPC histograms
   ClassDef(TLemRunHeader,5) // LEM Run Header
};
#endif
