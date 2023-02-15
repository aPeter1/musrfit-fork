// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PMusrT0Dict
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
#include "PMusrT0.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PMusrT0(void *p = nullptr);
   static void *newArray_PMusrT0(Long_t size, void *p);
   static void delete_PMusrT0(void *p);
   static void deleteArray_PMusrT0(void *p);
   static void destruct_PMusrT0(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMusrT0*)
   {
      ::PMusrT0 *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMusrT0 >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMusrT0", ::PMusrT0::Class_Version(), "", 121,
                  typeid(::PMusrT0), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMusrT0::Dictionary, isa_proxy, 4,
                  sizeof(::PMusrT0) );
      instance.SetNew(&new_PMusrT0);
      instance.SetNewArray(&newArray_PMusrT0);
      instance.SetDelete(&delete_PMusrT0);
      instance.SetDeleteArray(&deleteArray_PMusrT0);
      instance.SetDestructor(&destruct_PMusrT0);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMusrT0*)
   {
      return GenerateInitInstanceLocal((::PMusrT0*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMusrT0*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PMusrT0::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMusrT0::Class_Name()
{
   return "PMusrT0";
}

//______________________________________________________________________________
const char *PMusrT0::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMusrT0*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMusrT0::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMusrT0*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMusrT0::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMusrT0*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMusrT0::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMusrT0*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PMusrT0::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMusrT0.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMusrT0::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMusrT0::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMusrT0(void *p) {
      return  p ? new(p) ::PMusrT0 : new ::PMusrT0;
   }
   static void *newArray_PMusrT0(Long_t nElements, void *p) {
      return p ? new(p) ::PMusrT0[nElements] : new ::PMusrT0[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMusrT0(void *p) {
      delete ((::PMusrT0*)p);
   }
   static void deleteArray_PMusrT0(void *p) {
      delete [] ((::PMusrT0*)p);
   }
   static void destruct_PMusrT0(void *p) {
      typedef ::PMusrT0 current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMusrT0

namespace {
  void TriggerDictionaryInitialization_libPMusrT0_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/opt/homebrew/include",
"/Users/maria/Applications/musrfit/src/include",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/classes/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPMusrT0 dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PMusrT0;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPMusrT0 dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PMusrT0.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2021 by Andreas Suter                              *
 *   andreas.suter@psi.ch                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef _PMUSRT0_H_
#define _PMUSRT0_H_

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TLine.h>
#include <TLatex.h>
#include <TTimer.h>

#include "PMusr.h"
#ifndef __MAKECLING__
#include "PMsrHandler.h"
#endif // __MAKECLING__

#define PMUSRT0_FORWARD  0
#define PMUSRT0_BACKWARD 1

#define PMUSRT0_GET_T0                    0
#define PMUSRT0_GET_DATA_AND_BKG_RANGE    1
#define PMUSRT0_GET_T0_DATA_AND_BKG_RANGE 2


//--------------------------------------------------------------------------
/**
 * <p>Handles the raw muSR run data sets.
 */
class PMusrT0Data {
  public:
    PMusrT0Data();
    virtual ~PMusrT0Data();


    virtual void InitData();

    virtual Bool_t IsSingleHisto() { return fSingleHisto; }
    virtual UInt_t GetRawRunDataSize() { return fRawRunData.size(); }
    virtual PRawRunData* GetRawRunData(Int_t idx);
    virtual Int_t GetRunNo() { return fRunNo; }
    virtual Int_t GetAddRunIdx() { return fAddRunIdx; }
    virtual Int_t GetHistoNoIdx() { return fHistoNoIdx; }
    virtual UInt_t GetHistoNoSize() { return fHistoNo.size(); }
    virtual Int_t GetHistoNo(UInt_t idx);
    virtual Int_t GetDetectorTag() { return fDetectorTag; }
    virtual Int_t GetCmdTag() { return fCmdTag; }
    virtual UInt_t GetT0BinSize() { return fT0.size(); }
    virtual Int_t GetT0Bin(UInt_t idx);
    virtual UInt_t GetAddT0Entries() { return fAddT0.size(); }
    virtual UInt_t GetAddT0BinSize(UInt_t idx);
    virtual Int_t GetAddT0Bin(UInt_t addRunIdx, UInt_t idx);
    virtual Int_t GetT0BinData() { return fT0Data; }

    virtual void SetSingleHisto(const Bool_t flag) { fSingleHisto = flag; }
    virtual void SetRawRunData(const std::vector<PRawRunData*> rawRunData) { fRawRunData = rawRunData; }
    virtual void SetRunNo(const UInt_t runNo) { fRunNo = runNo; }
    virtual void SetAddRunIdx(const UInt_t addRunIdx) { fAddRunIdx = addRunIdx; }
    virtual void SetHistoNoIdx(const UInt_t histoNoIdx) { fHistoNoIdx = histoNoIdx; }
    virtual void SetHistoNo(const PIntVector histoNo) { fHistoNo = histoNo; }
    virtual void SetDetectorTag(const UInt_t detectorTag) { fDetectorTag = detectorTag; }
    virtual void SetCmdTag(const UInt_t cmdTag) { fCmdTag = cmdTag; }
    virtual void SetT0Bin(UInt_t val, UInt_t idx);
    virtual void SetAddT0Bin(UInt_t val, UInt_t addRunIdx, UInt_t idx);
    virtual void SetT0BinData(UInt_t val) { fT0Data = val; }

  private:
    Bool_t fSingleHisto;              ///< true if single histo fit, false for asymmetry fit
    std::vector<PRawRunData*> fRawRunData; ///< holds the raw data of the needed runs, idx=0 the run, idx>0 the addruns
    Int_t fRunNo;                     ///< msr-file run number
    Int_t fAddRunIdx;                 ///< msr-file addrun index
    Int_t fHistoNoIdx;                ///< msr-file histo number index
    PIntVector fHistoNo;              ///< msr-file histo numbers, i.e. idx + Red/Green offset
    Int_t fDetectorTag;               ///< detector tag. forward=0,backward=1
    Int_t fCmdTag;                    ///< command tag. 0=get t0, 1=get data-/bkg-range, 2=get t0, and data-/bkg-range
    PIntVector fT0;                   ///< holding the t0's of the run
    std::vector<PIntVector> fAddT0;   ///< holding the t0's of the addruns
    Int_t fT0Data;                    ///< holding the t0 found in the current data set
};


//--------------------------------------------------------------------------
/**
 * <p>Handles the musrt0 graphical user interface.
 * <p>The preprocessor tag __MAKECLING__ is used to hide away from rootcling
 * the overly complex spirit header files.
 */
class PMusrT0 : public TObject, public TQObject
{
  public:
    PMusrT0();
    PMusrT0(PMusrT0Data &data);

    virtual ~PMusrT0();

    virtual Bool_t IsValid() { return fValid; }

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void Quit(); // SLOT
    virtual void SetTimeout(Int_t timeout);

#ifndef __MAKECLING__
    virtual void SetMsrHandler(PMsrHandler *msrHandler);
#endif // __MAKECLING__

    virtual void InitT0();
    virtual void InitDataAndBkg();
    virtual Int_t GetStatus() { return fStatus; }

  private:
#ifndef __MAKECLING__
    PMsrHandler *fMsrHandler; ///< msr-file handler
#endif // __MAKECLING__
    Int_t  fTimeout;          ///< timeout after which the Done signal should be emited. If timeout <= 0, no timeout is taking place

    Bool_t fValid; ///< true if raw data set are available, otherwise false

    Int_t fStatus; ///< 0=quit locally, i.e. only a single musrt0 raw data canvas will terminate but not the application, 1=quit the application

    PMusrT0Data fMusrT0Data; ///< raw muSR run data sets.

    Bool_t fDataAndBkgEnabled; ///< enable/disable data and background range handling (necessary in connection with grouping and addrun)
    Bool_t fT0Enabled; ///< enable/disable t0 handling (necessary in connection with grouping and addrun)
    Int_t  fT0Estimated; ///< estimated t0 value (in bins)
    Bool_t fShowT0DataChannel;

    TTimer *fTimeoutTimer; ///< timeout timer in order to terminate if no action is taking place for too long

    // canvas related variables
    TCanvas   *fMainCanvas; ///< main canvas for the graphical user interface

    TH1F *fHisto; ///< full raw data histogram
    TH1F *fData;  ///< ranged raw data histogram (first good bin, last good bin)
    TH1F *fBkg;   ///< histogram starting from 'bkg start' up to 'bkg end'

    TLatex *fToDoInfo; ///< clear text user instruction string

    TLine *fT0Line; ///< line showing the position of t0
    TLine *fT0DataLine; ///< line showing the position of t0 found in the data file
    TLine *fFirstBkgLine; ///< line showing the start of the background
    TLine *fLastBkgLine; ///< line showing the end of the background
    TLine *fFirstDataLine; ///< line showing the start of the data (first good data bin)
    TLine *fLastDataLine; ///< line showing the end of the data (last good data bin)

    Int_t fPx; ///< x-position of the cursor
    Int_t fPy; ///< y-position of the cursor

    Int_t fDataRange[2]; ///< data range (first good bin, last good bin)
    Int_t fBkgRange[2]; ///< background range (first bkg bin, last bkg bin)

    void ShowDataFileT0Channel();
    void HideDataFileT0Channel();
    void SetT0Channel();
    void SetEstimatedT0Channel();
    void SetDataFirstChannel();
    void SetDataLastChannel();
    void SetBkgFirstChannel();
    void SetBkgLastChannel();
    void UnZoom();
    void ZoomT0();

  ClassDef(PMusrT0, 1)
};

#endif // _PMUSRT0_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PMusrT0", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPMusrT0",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPMusrT0_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPMusrT0_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPMusrT0() {
  TriggerDictionaryInitialization_libPMusrT0_Impl();
}
