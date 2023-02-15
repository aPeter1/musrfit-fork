// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TLemRunHeaderDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "TLemRunHeader.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_TLemRunHeader(void *p = nullptr);
   static void *newArray_TLemRunHeader(Long_t size, void *p);
   static void delete_TLemRunHeader(void *p);
   static void deleteArray_TLemRunHeader(void *p);
   static void destruct_TLemRunHeader(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TLemRunHeader*)
   {
      ::TLemRunHeader *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TLemRunHeader >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("TLemRunHeader", ::TLemRunHeader::Class_Version(), "", 35,
                  typeid(::TLemRunHeader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TLemRunHeader::Dictionary, isa_proxy, 4,
                  sizeof(::TLemRunHeader) );
      instance.SetNew(&new_TLemRunHeader);
      instance.SetNewArray(&newArray_TLemRunHeader);
      instance.SetDelete(&delete_TLemRunHeader);
      instance.SetDeleteArray(&deleteArray_TLemRunHeader);
      instance.SetDestructor(&destruct_TLemRunHeader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TLemRunHeader*)
   {
      return GenerateInitInstanceLocal((::TLemRunHeader*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TLemRunHeader*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TLemRunHeader::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *TLemRunHeader::Class_Name()
{
   return "TLemRunHeader";
}

//______________________________________________________________________________
const char *TLemRunHeader::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TLemRunHeader*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int TLemRunHeader::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TLemRunHeader*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TLemRunHeader::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TLemRunHeader*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TLemRunHeader::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TLemRunHeader*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TLemRunHeader::Streamer(TBuffer &R__b)
{
   // Stream an object of class TLemRunHeader.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TLemRunHeader::Class(),this);
   } else {
      R__b.WriteClassBuffer(TLemRunHeader::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TLemRunHeader(void *p) {
      return  p ? new(p) ::TLemRunHeader : new ::TLemRunHeader;
   }
   static void *newArray_TLemRunHeader(Long_t nElements, void *p) {
      return p ? new(p) ::TLemRunHeader[nElements] : new ::TLemRunHeader[nElements];
   }
   // Wrapper around operator delete
   static void delete_TLemRunHeader(void *p) {
      delete ((::TLemRunHeader*)p);
   }
   static void deleteArray_TLemRunHeader(void *p) {
      delete [] ((::TLemRunHeader*)p);
   }
   static void destruct_TLemRunHeader(void *p) {
      typedef ::TLemRunHeader current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TLemRunHeader

namespace {
  void TriggerDictionaryInitialization_libTLemRunHeader_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/external/TLemRunHeader",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/TLemRunHeader",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/TLemRunHeader/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libTLemRunHeader dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate(R"ATTRDUMP(LEM Run Header)ATTRDUMP"))) TLemRunHeader;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libTLemRunHeader dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
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

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"TLemRunHeader", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libTLemRunHeader",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libTLemRunHeader_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libTLemRunHeader_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libTLemRunHeader() {
  TriggerDictionaryInitialization_libTLemRunHeader_Impl();
}
