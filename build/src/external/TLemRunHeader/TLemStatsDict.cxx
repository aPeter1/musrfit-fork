// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TLemStatsDict
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
#include "TLemStats.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_TLemStats(void *p = nullptr);
   static void *newArray_TLemStats(Long_t size, void *p);
   static void delete_TLemStats(void *p);
   static void deleteArray_TLemStats(void *p);
   static void destruct_TLemStats(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TLemStats*)
   {
      ::TLemStats *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TLemStats >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("TLemStats", ::TLemStats::Class_Version(), "", 38,
                  typeid(::TLemStats), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TLemStats::Dictionary, isa_proxy, 4,
                  sizeof(::TLemStats) );
      instance.SetNew(&new_TLemStats);
      instance.SetNewArray(&newArray_TLemStats);
      instance.SetDelete(&delete_TLemStats);
      instance.SetDeleteArray(&deleteArray_TLemStats);
      instance.SetDestructor(&destruct_TLemStats);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TLemStats*)
   {
      return GenerateInitInstanceLocal((::TLemStats*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TLemStats*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TLemStats::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *TLemStats::Class_Name()
{
   return "TLemStats";
}

//______________________________________________________________________________
const char *TLemStats::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TLemStats*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int TLemStats::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TLemStats*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TLemStats::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TLemStats*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TLemStats::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TLemStats*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TLemStats::Streamer(TBuffer &R__b)
{
   // Stream an object of class TLemStats.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TLemStats::Class(),this);
   } else {
      R__b.WriteClassBuffer(TLemStats::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TLemStats(void *p) {
      return  p ? new(p) ::TLemStats : new ::TLemStats;
   }
   static void *newArray_TLemStats(Long_t nElements, void *p) {
      return p ? new(p) ::TLemStats[nElements] : new ::TLemStats[nElements];
   }
   // Wrapper around operator delete
   static void delete_TLemStats(void *p) {
      delete ((::TLemStats*)p);
   }
   static void deleteArray_TLemStats(void *p) {
      delete [] ((::TLemStats*)p);
   }
   static void destruct_TLemStats(void *p) {
      typedef ::TLemStats current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TLemStats

namespace {
  void TriggerDictionaryInitialization_libTLemStats_Impl() {
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
#line 1 "libTLemStats dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate(R"ATTRDUMP(LEM Statistics)ATTRDUMP"))) TLemStats;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libTLemStats dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
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

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"TLemStats", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libTLemStats",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libTLemStats_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libTLemStats_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libTLemStats() {
  TriggerDictionaryInitialization_libTLemStats_Impl();
}
