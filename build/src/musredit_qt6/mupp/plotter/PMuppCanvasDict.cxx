// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PMuppCanvasDict
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
#include "PMuppCanvas.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PMuppCanvas(void *p = nullptr);
   static void *newArray_PMuppCanvas(Long_t size, void *p);
   static void delete_PMuppCanvas(void *p);
   static void deleteArray_PMuppCanvas(void *p);
   static void destruct_PMuppCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMuppCanvas*)
   {
      ::PMuppCanvas *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMuppCanvas >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMuppCanvas", ::PMuppCanvas::Class_Version(), "", 71,
                  typeid(::PMuppCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMuppCanvas::Dictionary, isa_proxy, 4,
                  sizeof(::PMuppCanvas) );
      instance.SetNew(&new_PMuppCanvas);
      instance.SetNewArray(&newArray_PMuppCanvas);
      instance.SetDelete(&delete_PMuppCanvas);
      instance.SetDeleteArray(&deleteArray_PMuppCanvas);
      instance.SetDestructor(&destruct_PMuppCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMuppCanvas*)
   {
      return GenerateInitInstanceLocal((::PMuppCanvas*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMuppCanvas*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PMuppCanvas::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMuppCanvas::Class_Name()
{
   return "PMuppCanvas";
}

//______________________________________________________________________________
const char *PMuppCanvas::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMuppCanvas*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMuppCanvas::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMuppCanvas*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMuppCanvas::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMuppCanvas*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMuppCanvas::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMuppCanvas*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PMuppCanvas::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMuppCanvas.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMuppCanvas::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMuppCanvas::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMuppCanvas(void *p) {
      return  p ? new(p) ::PMuppCanvas : new ::PMuppCanvas;
   }
   static void *newArray_PMuppCanvas(Long_t nElements, void *p) {
      return p ? new(p) ::PMuppCanvas[nElements] : new ::PMuppCanvas[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMuppCanvas(void *p) {
      delete ((::PMuppCanvas*)p);
   }
   static void deleteArray_PMuppCanvas(void *p) {
      delete [] ((::PMuppCanvas*)p);
   }
   static void destruct_PMuppCanvas(void *p) {
      typedef ::PMuppCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMuppCanvas

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
   static TClass *vectorlEdoublegR_Dictionary();
   static void vectorlEdoublegR_TClassManip(TClass*);
   static void *new_vectorlEdoublegR(void *p = nullptr);
   static void *newArray_vectorlEdoublegR(Long_t size, void *p);
   static void delete_vectorlEdoublegR(void *p);
   static void deleteArray_vectorlEdoublegR(void *p);
   static void destruct_vectorlEdoublegR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<double>*)
   {
      vector<double> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<double>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<double>", -2, "vector", 493,
                  typeid(vector<double>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEdoublegR_Dictionary, isa_proxy, 0,
                  sizeof(vector<double>) );
      instance.SetNew(&new_vectorlEdoublegR);
      instance.SetNewArray(&newArray_vectorlEdoublegR);
      instance.SetDelete(&delete_vectorlEdoublegR);
      instance.SetDeleteArray(&deleteArray_vectorlEdoublegR);
      instance.SetDestructor(&destruct_vectorlEdoublegR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<double> >()));

      ::ROOT::AddClassAlternate("vector<double>","std::__1::vector<double, std::__1::allocator<double> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<double>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEdoublegR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<double>*)nullptr)->GetClass();
      vectorlEdoublegR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEdoublegR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEdoublegR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double> : new vector<double>;
   }
   static void *newArray_vectorlEdoublegR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<double>[nElements] : new vector<double>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEdoublegR(void *p) {
      delete ((vector<double>*)p);
   }
   static void deleteArray_vectorlEdoublegR(void *p) {
      delete [] ((vector<double>*)p);
   }
   static void destruct_vectorlEdoublegR(void *p) {
      typedef vector<double> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<double>

namespace ROOT {
   static TClass *vectorlETGraphAsymmErrorsmUgR_Dictionary();
   static void vectorlETGraphAsymmErrorsmUgR_TClassManip(TClass*);
   static void *new_vectorlETGraphAsymmErrorsmUgR(void *p = nullptr);
   static void *newArray_vectorlETGraphAsymmErrorsmUgR(Long_t size, void *p);
   static void delete_vectorlETGraphAsymmErrorsmUgR(void *p);
   static void deleteArray_vectorlETGraphAsymmErrorsmUgR(void *p);
   static void destruct_vectorlETGraphAsymmErrorsmUgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TGraphAsymmErrors*>*)
   {
      vector<TGraphAsymmErrors*> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TGraphAsymmErrors*>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TGraphAsymmErrors*>", -2, "vector", 493,
                  typeid(vector<TGraphAsymmErrors*>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETGraphAsymmErrorsmUgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TGraphAsymmErrors*>) );
      instance.SetNew(&new_vectorlETGraphAsymmErrorsmUgR);
      instance.SetNewArray(&newArray_vectorlETGraphAsymmErrorsmUgR);
      instance.SetDelete(&delete_vectorlETGraphAsymmErrorsmUgR);
      instance.SetDeleteArray(&deleteArray_vectorlETGraphAsymmErrorsmUgR);
      instance.SetDestructor(&destruct_vectorlETGraphAsymmErrorsmUgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TGraphAsymmErrors*> >()));

      ::ROOT::AddClassAlternate("vector<TGraphAsymmErrors*>","std::__1::vector<TGraphAsymmErrors*, std::__1::allocator<TGraphAsymmErrors*> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TGraphAsymmErrors*>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETGraphAsymmErrorsmUgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TGraphAsymmErrors*>*)nullptr)->GetClass();
      vectorlETGraphAsymmErrorsmUgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETGraphAsymmErrorsmUgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETGraphAsymmErrorsmUgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TGraphAsymmErrors*> : new vector<TGraphAsymmErrors*>;
   }
   static void *newArray_vectorlETGraphAsymmErrorsmUgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TGraphAsymmErrors*>[nElements] : new vector<TGraphAsymmErrors*>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETGraphAsymmErrorsmUgR(void *p) {
      delete ((vector<TGraphAsymmErrors*>*)p);
   }
   static void deleteArray_vectorlETGraphAsymmErrorsmUgR(void *p) {
      delete [] ((vector<TGraphAsymmErrors*>*)p);
   }
   static void destruct_vectorlETGraphAsymmErrorsmUgR(void *p) {
      typedef vector<TGraphAsymmErrors*> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TGraphAsymmErrors*>

namespace ROOT {
   static TClass *vectorlEPDataCollectiongR_Dictionary();
   static void vectorlEPDataCollectiongR_TClassManip(TClass*);
   static void *new_vectorlEPDataCollectiongR(void *p = nullptr);
   static void *newArray_vectorlEPDataCollectiongR(Long_t size, void *p);
   static void delete_vectorlEPDataCollectiongR(void *p);
   static void deleteArray_vectorlEPDataCollectiongR(void *p);
   static void destruct_vectorlEPDataCollectiongR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PDataCollection>*)
   {
      vector<PDataCollection> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PDataCollection>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PDataCollection>", -2, "vector", 493,
                  typeid(vector<PDataCollection>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPDataCollectiongR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PDataCollection>) );
      instance.SetNew(&new_vectorlEPDataCollectiongR);
      instance.SetNewArray(&newArray_vectorlEPDataCollectiongR);
      instance.SetDelete(&delete_vectorlEPDataCollectiongR);
      instance.SetDeleteArray(&deleteArray_vectorlEPDataCollectiongR);
      instance.SetDestructor(&destruct_vectorlEPDataCollectiongR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PDataCollection> >()));

      ::ROOT::AddClassAlternate("vector<PDataCollection>","std::__1::vector<PDataCollection, std::__1::allocator<PDataCollection> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PDataCollection>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPDataCollectiongR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PDataCollection>*)nullptr)->GetClass();
      vectorlEPDataCollectiongR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPDataCollectiongR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPDataCollectiongR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PDataCollection> : new vector<PDataCollection>;
   }
   static void *newArray_vectorlEPDataCollectiongR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PDataCollection>[nElements] : new vector<PDataCollection>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPDataCollectiongR(void *p) {
      delete ((vector<PDataCollection>*)p);
   }
   static void deleteArray_vectorlEPDataCollectiongR(void *p) {
      delete [] ((vector<PDataCollection>*)p);
   }
   static void destruct_vectorlEPDataCollectiongR(void *p) {
      typedef vector<PDataCollection> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PDataCollection>

namespace {
  void TriggerDictionaryInitialization_libPMuppCanvas_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter",
"/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter/..",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter",
"/Users/maria/Applications/musrfit/src/musredit_qt6/mupp/plotter",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/musredit_qt6/mupp/plotter/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPMuppCanvas dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PMuppCanvas;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPMuppCanvas dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PMuppCanvas.h

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

#ifndef _PMUPPCANVAS_H_
#define _PMUPPCANVAS_H_

#include <vector>

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraphAsymmErrors.h>
#include <TTimer.h>

#include "mupp_plot.h"
#include "mupp.h"

// Canvas menu id's
#define P_MENU_ID_EXPORT 10001
#define P_MENU_ID_ABOUT  10002

//--------------------------------------------------------------------------
typedef struct {
  Double_t y;
  Double_t eYpos;
  Double_t eYneg;
} PDataPoint;

//--------------------------------------------------------------------------
typedef struct {
  TString name; // collection name
  TString xLabel;
  PStringVector yLabel;
  PDoubleVector xValue;
  std::vector< std::vector<PDataPoint> > yValue;
} PDataCollection;

//--------------------------------------------------------------------------
class PMuppCanvas : public TObject, public  TQObject
{
public:
  PMuppCanvas();
  PMuppCanvas(const Char_t* title,
              Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
              const PIntVector markerSytleList, const PDoubleVector markerSizeList,
              const PIntVector colorList,
              const int mupp_instance);
  virtual ~PMuppCanvas();

  virtual Bool_t IsValid() { return fValid; }

  virtual void Done(Int_t status=0); // *SIGNAL*
  virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
  virtual void HandleMenuPopup(Int_t id); // SLOT
  virtual void LastCanvasClosed(); // SLOT
  virtual void WindowClosed(); // SLOT

  virtual void CheckIPCMsgQueue();

private:
  Bool_t fValid;
  Int_t fMuppInstance;

  TString fFtokName;
  TTimer *fCheckMsgQueue; ///< timer needed to check if a message in the IPC message queue is pending

  std::vector<PDataCollection> fPlotData;

  TStyle    *fStyle; ///< A collection of all graphics attributes

  // canvas menu related variables
  TRootCanvas *fImp;           ///< ROOT native GUI version of main window with menubar and drawing area
  TGMenuBar   *fBar;           ///< menu bar
  TGPopupMenu *fPopupMain;     ///< popup menu mupp in the main menu bar

  // canvas related variables
  TCanvas   *fMainCanvas;            ///< main canvas
  TMultiGraph *fMultiGraph;          ///< main multi graph
  std::vector<TGraphAsymmErrors*> fGraphE; ///< all error graphs

  // perdefined markers, colors
  PIntVector    fMarkerStyleList;
  PDoubleVector fMarkerSizeList;
  PIntVector    fColorList;

  virtual void CreateStyle();
  virtual void InitMuppCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);

  virtual int ReadPlotData(const Char_t *fln);
  virtual void InitDataCollection(PDataCollection &coll);
  virtual void UpdateGraphs();
  virtual void ExportData();

  ClassDef(PMuppCanvas, 1)
};

#endif // _PMUPPCANVAS_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PMuppCanvas", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPMuppCanvas",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPMuppCanvas_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPMuppCanvas_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPMuppCanvas() {
  TriggerDictionaryInitialization_libPMuppCanvas_Impl();
}
