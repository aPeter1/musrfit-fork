// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PFourierCanvasDict
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
#include "PFourierCanvas.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PFourierCanvas(void *p = nullptr);
   static void *newArray_PFourierCanvas(Long_t size, void *p);
   static void delete_PFourierCanvas(void *p);
   static void deleteArray_PFourierCanvas(void *p);
   static void destruct_PFourierCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PFourierCanvas*)
   {
      ::PFourierCanvas *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PFourierCanvas >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PFourierCanvas", ::PFourierCanvas::Class_Version(), "", 91,
                  typeid(::PFourierCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PFourierCanvas::Dictionary, isa_proxy, 4,
                  sizeof(::PFourierCanvas) );
      instance.SetNew(&new_PFourierCanvas);
      instance.SetNewArray(&newArray_PFourierCanvas);
      instance.SetDelete(&delete_PFourierCanvas);
      instance.SetDeleteArray(&deleteArray_PFourierCanvas);
      instance.SetDestructor(&destruct_PFourierCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PFourierCanvas*)
   {
      return GenerateInitInstanceLocal((::PFourierCanvas*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PFourierCanvas*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PFourierCanvas::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PFourierCanvas::Class_Name()
{
   return "PFourierCanvas";
}

//______________________________________________________________________________
const char *PFourierCanvas::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PFourierCanvas*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PFourierCanvas::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PFourierCanvas*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PFourierCanvas::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PFourierCanvas*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PFourierCanvas::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PFourierCanvas*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PFourierCanvas::Streamer(TBuffer &R__b)
{
   // Stream an object of class PFourierCanvas.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PFourierCanvas::Class(),this);
   } else {
      R__b.WriteClassBuffer(PFourierCanvas::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PFourierCanvas(void *p) {
      return  p ? new(p) ::PFourierCanvas : new ::PFourierCanvas;
   }
   static void *newArray_PFourierCanvas(Long_t nElements, void *p) {
      return p ? new(p) ::PFourierCanvas[nElements] : new ::PFourierCanvas[nElements];
   }
   // Wrapper around operator delete
   static void delete_PFourierCanvas(void *p) {
      delete ((::PFourierCanvas*)p);
   }
   static void deleteArray_PFourierCanvas(void *p) {
      delete [] ((::PFourierCanvas*)p);
   }
   static void destruct_PFourierCanvas(void *p) {
      typedef ::PFourierCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PFourierCanvas

namespace ROOT {
   static TClass *vectorlEintgR_Dictionary();
   static void vectorlEintgR_TClassManip(TClass*);
   static void *new_vectorlEintgR(void *p = nullptr);
   static void *newArray_vectorlEintgR(Long_t size, void *p);
   static void delete_vectorlEintgR(void *p);
   static void deleteArray_vectorlEintgR(void *p);
   static void destruct_vectorlEintgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<int>*)
   {
      vector<int> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<int>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<int>", -2, "vector", 493,
                  typeid(vector<int>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEintgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<int>) );
      instance.SetNew(&new_vectorlEintgR);
      instance.SetNewArray(&newArray_vectorlEintgR);
      instance.SetDelete(&delete_vectorlEintgR);
      instance.SetDeleteArray(&deleteArray_vectorlEintgR);
      instance.SetDestructor(&destruct_vectorlEintgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<int> >()));

      ::ROOT::AddClassAlternate("vector<int>","std::__1::vector<int, std::__1::allocator<int> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<int>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEintgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<int>*)nullptr)->GetClass();
      vectorlEintgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEintgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEintgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<int> : new vector<int>;
   }
   static void *newArray_vectorlEintgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<int>[nElements] : new vector<int>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEintgR(void *p) {
      delete ((vector<int>*)p);
   }
   static void deleteArray_vectorlEintgR(void *p) {
      delete [] ((vector<int>*)p);
   }
   static void destruct_vectorlEintgR(void *p) {
      typedef vector<int> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<int>

namespace ROOT {
   static TClass *vectorlEPFourierCanvasDataSetgR_Dictionary();
   static void vectorlEPFourierCanvasDataSetgR_TClassManip(TClass*);
   static void *new_vectorlEPFourierCanvasDataSetgR(void *p = nullptr);
   static void *newArray_vectorlEPFourierCanvasDataSetgR(Long_t size, void *p);
   static void delete_vectorlEPFourierCanvasDataSetgR(void *p);
   static void deleteArray_vectorlEPFourierCanvasDataSetgR(void *p);
   static void destruct_vectorlEPFourierCanvasDataSetgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PFourierCanvasDataSet>*)
   {
      vector<PFourierCanvasDataSet> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PFourierCanvasDataSet>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PFourierCanvasDataSet>", -2, "vector", 493,
                  typeid(vector<PFourierCanvasDataSet>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPFourierCanvasDataSetgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PFourierCanvasDataSet>) );
      instance.SetNew(&new_vectorlEPFourierCanvasDataSetgR);
      instance.SetNewArray(&newArray_vectorlEPFourierCanvasDataSetgR);
      instance.SetDelete(&delete_vectorlEPFourierCanvasDataSetgR);
      instance.SetDeleteArray(&deleteArray_vectorlEPFourierCanvasDataSetgR);
      instance.SetDestructor(&destruct_vectorlEPFourierCanvasDataSetgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PFourierCanvasDataSet> >()));

      ::ROOT::AddClassAlternate("vector<PFourierCanvasDataSet>","std::__1::vector<PFourierCanvasDataSet, std::__1::allocator<PFourierCanvasDataSet> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PFourierCanvasDataSet>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPFourierCanvasDataSetgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PFourierCanvasDataSet>*)nullptr)->GetClass();
      vectorlEPFourierCanvasDataSetgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPFourierCanvasDataSetgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPFourierCanvasDataSetgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PFourierCanvasDataSet> : new vector<PFourierCanvasDataSet>;
   }
   static void *newArray_vectorlEPFourierCanvasDataSetgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PFourierCanvasDataSet>[nElements] : new vector<PFourierCanvasDataSet>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPFourierCanvasDataSetgR(void *p) {
      delete ((vector<PFourierCanvasDataSet>*)p);
   }
   static void deleteArray_vectorlEPFourierCanvasDataSetgR(void *p) {
      delete [] ((vector<PFourierCanvasDataSet>*)p);
   }
   static void destruct_vectorlEPFourierCanvasDataSetgR(void *p) {
      typedef vector<PFourierCanvasDataSet> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PFourierCanvasDataSet>

namespace ROOT {
   static TClass *vectorlEPFouriermUgR_Dictionary();
   static void vectorlEPFouriermUgR_TClassManip(TClass*);
   static void *new_vectorlEPFouriermUgR(void *p = nullptr);
   static void *newArray_vectorlEPFouriermUgR(Long_t size, void *p);
   static void delete_vectorlEPFouriermUgR(void *p);
   static void deleteArray_vectorlEPFouriermUgR(void *p);
   static void destruct_vectorlEPFouriermUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PFourier*>*)
   {
      vector<PFourier*> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PFourier*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PFourier*>", -2, "vector", 493,
                  typeid(vector<PFourier*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPFouriermUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PFourier*>) );
      instance.SetNew(&new_vectorlEPFouriermUgR);
      instance.SetNewArray(&newArray_vectorlEPFouriermUgR);
      instance.SetDelete(&delete_vectorlEPFouriermUgR);
      instance.SetDeleteArray(&deleteArray_vectorlEPFouriermUgR);
      instance.SetDestructor(&destruct_vectorlEPFouriermUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PFourier*> >()));

      ::ROOT::AddClassAlternate("vector<PFourier*>","std::__1::vector<PFourier*, std::__1::allocator<PFourier*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PFourier*>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPFouriermUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PFourier*>*)nullptr)->GetClass();
      vectorlEPFouriermUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPFouriermUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPFouriermUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PFourier*> : new vector<PFourier*>;
   }
   static void *newArray_vectorlEPFouriermUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PFourier*>[nElements] : new vector<PFourier*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPFouriermUgR(void *p) {
      delete ((vector<PFourier*>*)p);
   }
   static void deleteArray_vectorlEPFouriermUgR(void *p) {
      delete [] ((vector<PFourier*>*)p);
   }
   static void destruct_vectorlEPFouriermUgR(void *p) {
      typedef vector<PFourier*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PFourier*>

namespace {
  void TriggerDictionaryInitialization_libPFourierCanvas_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/include",
"/opt/homebrew/include",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/classes/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPFourierCanvas dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PFourierCanvas;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPFourierCanvas dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PFourierCanvas.h

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

#ifndef _PFOURIERCANVAS_H_
#define _PFOURIERCANVAS_H_

#include <vector>

#include <TObject.h>
#include <TQObject.h>
#include <TTimer.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TPad.h>
#include <TH1F.h>
#include <TLatex.h>

#include "PMusr.h"
#include "PFourier.h"

// Canvas menu id's
#define P_MENU_ID_FOURIER              10001
#define P_MENU_ID_AVERAGE              10002
#define P_MENU_ID_AVERAGE_PER_DATA_SET 10003
#define P_MENU_ID_EXPORT_DATA          10004

#define P_MENU_ID_FOURIER_REAL           100
#define P_MENU_ID_FOURIER_IMAG           101
#define P_MENU_ID_FOURIER_REAL_AND_IMAG  102
#define P_MENU_ID_FOURIER_PWR            103
#define P_MENU_ID_FOURIER_PHASE          104
#define P_MENU_ID_FOURIER_PHASE_OPT_REAL 105
#define P_MENU_ID_FOURIER_PHASE_PLUS     106
#define P_MENU_ID_FOURIER_PHASE_MINUS    107

//------------------------------------------------------------------------
/**
 * <p>Structure holding all necessary Fourier histograms.
 */
typedef struct {
  TH1F *dataFourierRe;           ///< real part of the Fourier transform of the data histogram
  TH1F *dataFourierIm;           ///< imaginary part of the Fourier transform of the data histogram
  TH1F *dataFourierPwr;          ///< power spectrum of the Fourier transform of the data histogram
  TH1F *dataFourierPhase;        ///< phase spectrum of the Fourier transform of the data histogram
  TH1F *dataFourierPhaseOptReal; ///< phase otpimized real Fourier transform of the data histogram
  std::vector<Double_t> optPhase;     ///< optimal phase which maximizes the real Fourier
} PFourierCanvasDataSet;

//------------------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of histogram data sets.
 */
typedef std::vector<PFourierCanvasDataSet> PFourierCanvasDataList;

//--------------------------------------------------------------------------
/**
 * <p>
 */
class PFourierCanvas : public TObject, public TQObject
{
  public:
    PFourierCanvas();
    PFourierCanvas(std::vector<PFourier*> &fourier, PIntVector dataSetTag, const Char_t* title,
                   const Bool_t showAverage, const Bool_t showAveragePerDataSet,
                   const Int_t fourierPlotOpt, Double_t fourierXrange[2], Double_t phase,
                   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, const Bool_t batch);
    PFourierCanvas(std::vector<PFourier*> &fourier, PIntVector dataSetTag, const Char_t* title,
                   const Bool_t showAverage, const Bool_t showAveragePerDataSet,
                   const Int_t fourierPlotOpt, Double_t fourierXrange[2], Double_t phase,
                   Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                   const PIntVector markerList, const PIntVector colorList, const Bool_t batch);
    virtual ~PFourierCanvas();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void HandleMenuPopup(Int_t id); // SLOT
    virtual void LastCanvasClosed(); // SLOT

    virtual void UpdateFourierPad();
    virtual void UpdateInfoPad();

    virtual Bool_t IsValid() { return fValid; }

    virtual void SetTimeout(Int_t ival);

    virtual void SaveGraphicsAndQuit(const Char_t *fileName);
    virtual void ExportData(const Char_t *pathFileName);

  private:
    Int_t  fTimeout;         ///< timeout after which the Done signal should be emited. If timeout <= 0, no timeout is taking place
    Bool_t fBatchMode;       ///< musrview in ROOT batch mode
    Bool_t fValid;           ///< if true, everything looks OK
    Bool_t fAveragedView;    ///< tag showing that the averaged view for ALL data or normal view should be presented.
    Bool_t fAveragedViewPerDataSet; ///< tag showing that the averaged view for individual data sets or normal view should be presented.
    PIntVector fDataSetTag;  ///< vector holding the data set tags
    Int_t  fCurrentPlotView; ///< tag showing what the current plot view is: real, imag, power, phase, ...
    Double_t fInitialXRange[2]; ///< keeps the initial x-range
    Double_t fInitialYRange[2]; ///< keeps the initial y-range

    TString fTitle;
    TString fXaxisTitle;
    std::vector<PFourier*> fFourier; ///< keeps all the Fourier data, ownership is with the caller
    PFourierCanvasDataList fFourierHistos; ///< keeps all the Fourier histos
    PFourierCanvasDataList fFourierAverage; ///< keeps the average of the Fourier histos
    Double_t fCurrentFourierPhase; ///< keeps the current Fourier phase (real/imag)
    TLatex *fCurrentFourierPhaseText; ///< used in Re/Im Fourier to show the current phase in the pad

    TStyle *fStyle; ///< A collection of all graphics attributes

    TTimer *fTimeoutTimer;  ///< timeout timer in order to terminate if no action is taking place for too long

    PIntVector fMarkerList; ///< list of markers
    PIntVector fColorList;  ///< list of colors

    // canvas menu related variables
    TRootCanvas *fImp;           ///< ROOT native GUI version of main window with menubar and drawing area
    TGMenuBar   *fBar;           ///< menu bar
    TGPopupMenu *fPopupMain;     ///< popup menu MusrFT in the main menu bar
    TGPopupMenu *fPopupFourier;  ///< popup menu of the MusrFT/Fourier sub menu

    // canvas related variables
    TCanvas   *fMainCanvas;        ///< main canvas
    TPaveText *fTitlePad;          ///< title pad used to display a title
    TPad      *fFourierPad;        ///< fourier pad used to display the fourier
    TLegend   *fInfoPad;           ///< info pad used to display a legend of the data plotted
    TLegend   *fLegAvgPerDataSet;  ///< legend used for averaged per data set view

    virtual void CreateXaxisTitle();
    virtual void CreateStyle();
    virtual void InitFourierDataSets();
    virtual void InitFourierCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual void CleanupAverage();
    virtual void HandleAverage();
    virtual void CalcPhaseOptReal();

    virtual void PlotFourier();
    virtual void PlotFourierPhaseValue();
    virtual void PlotAverage();
    virtual void IncrementFourierPhase();
    virtual void DecrementFourierPhase();

    virtual Double_t GetMaximum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMinimum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetInterpolatedValue(TH1F* histo, Double_t xVal);
    virtual TString GetDataSetName(TString title);

    ClassDef(PFourierCanvas, 1)
};

#endif // _PFOURIERCANVAS_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PFourierCanvas", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPFourierCanvas",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPFourierCanvas_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPFourierCanvas_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPFourierCanvas() {
  TriggerDictionaryInitialization_libPFourierCanvas_Impl();
}
