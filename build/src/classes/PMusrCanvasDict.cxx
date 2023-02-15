// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PMusrCanvasDict
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
#include "PMusrCanvas.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PMusrCanvasPlotRange(void *p = nullptr);
   static void *newArray_PMusrCanvasPlotRange(Long_t size, void *p);
   static void delete_PMusrCanvasPlotRange(void *p);
   static void deleteArray_PMusrCanvasPlotRange(void *p);
   static void destruct_PMusrCanvasPlotRange(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMusrCanvasPlotRange*)
   {
      ::PMusrCanvasPlotRange *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMusrCanvasPlotRange >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMusrCanvasPlotRange", ::PMusrCanvasPlotRange::Class_Version(), "", 90,
                  typeid(::PMusrCanvasPlotRange), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMusrCanvasPlotRange::Dictionary, isa_proxy, 4,
                  sizeof(::PMusrCanvasPlotRange) );
      instance.SetNew(&new_PMusrCanvasPlotRange);
      instance.SetNewArray(&newArray_PMusrCanvasPlotRange);
      instance.SetDelete(&delete_PMusrCanvasPlotRange);
      instance.SetDeleteArray(&deleteArray_PMusrCanvasPlotRange);
      instance.SetDestructor(&destruct_PMusrCanvasPlotRange);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMusrCanvasPlotRange*)
   {
      return GenerateInitInstanceLocal((::PMusrCanvasPlotRange*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMusrCanvasPlotRange*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_PMusrCanvas(void *p = nullptr);
   static void *newArray_PMusrCanvas(Long_t size, void *p);
   static void delete_PMusrCanvas(void *p);
   static void deleteArray_PMusrCanvas(void *p);
   static void destruct_PMusrCanvas(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMusrCanvas*)
   {
      ::PMusrCanvas *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMusrCanvas >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMusrCanvas", ::PMusrCanvas::Class_Version(), "", 205,
                  typeid(::PMusrCanvas), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMusrCanvas::Dictionary, isa_proxy, 4,
                  sizeof(::PMusrCanvas) );
      instance.SetNew(&new_PMusrCanvas);
      instance.SetNewArray(&newArray_PMusrCanvas);
      instance.SetDelete(&delete_PMusrCanvas);
      instance.SetDeleteArray(&deleteArray_PMusrCanvas);
      instance.SetDestructor(&destruct_PMusrCanvas);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMusrCanvas*)
   {
      return GenerateInitInstanceLocal((::PMusrCanvas*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMusrCanvas*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PMusrCanvasPlotRange::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMusrCanvasPlotRange::Class_Name()
{
   return "PMusrCanvasPlotRange";
}

//______________________________________________________________________________
const char *PMusrCanvasPlotRange::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvasPlotRange*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMusrCanvasPlotRange::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvasPlotRange*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMusrCanvasPlotRange::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvasPlotRange*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMusrCanvasPlotRange::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvasPlotRange*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr PMusrCanvas::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMusrCanvas::Class_Name()
{
   return "PMusrCanvas";
}

//______________________________________________________________________________
const char *PMusrCanvas::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvas*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMusrCanvas::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvas*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMusrCanvas::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvas*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMusrCanvas::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMusrCanvas*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PMusrCanvasPlotRange::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMusrCanvasPlotRange.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMusrCanvasPlotRange::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMusrCanvasPlotRange::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMusrCanvasPlotRange(void *p) {
      return  p ? new(p) ::PMusrCanvasPlotRange : new ::PMusrCanvasPlotRange;
   }
   static void *newArray_PMusrCanvasPlotRange(Long_t nElements, void *p) {
      return p ? new(p) ::PMusrCanvasPlotRange[nElements] : new ::PMusrCanvasPlotRange[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMusrCanvasPlotRange(void *p) {
      delete ((::PMusrCanvasPlotRange*)p);
   }
   static void deleteArray_PMusrCanvasPlotRange(void *p) {
      delete [] ((::PMusrCanvasPlotRange*)p);
   }
   static void destruct_PMusrCanvasPlotRange(void *p) {
      typedef ::PMusrCanvasPlotRange current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMusrCanvasPlotRange

//______________________________________________________________________________
void PMusrCanvas::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMusrCanvas.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMusrCanvas::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMusrCanvas::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMusrCanvas(void *p) {
      return  p ? new(p) ::PMusrCanvas : new ::PMusrCanvas;
   }
   static void *newArray_PMusrCanvas(Long_t nElements, void *p) {
      return p ? new(p) ::PMusrCanvas[nElements] : new ::PMusrCanvas[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMusrCanvas(void *p) {
      delete ((::PMusrCanvas*)p);
   }
   static void deleteArray_PMusrCanvas(void *p) {
      delete [] ((::PMusrCanvas*)p);
   }
   static void destruct_PMusrCanvas(void *p) {
      typedef ::PMusrCanvas current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMusrCanvas

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
   static TClass *vectorlEPMusrCanvasNonMusrDataSetgR_Dictionary();
   static void vectorlEPMusrCanvasNonMusrDataSetgR_TClassManip(TClass*);
   static void *new_vectorlEPMusrCanvasNonMusrDataSetgR(void *p = nullptr);
   static void *newArray_vectorlEPMusrCanvasNonMusrDataSetgR(Long_t size, void *p);
   static void delete_vectorlEPMusrCanvasNonMusrDataSetgR(void *p);
   static void deleteArray_vectorlEPMusrCanvasNonMusrDataSetgR(void *p);
   static void destruct_vectorlEPMusrCanvasNonMusrDataSetgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PMusrCanvasNonMusrDataSet>*)
   {
      vector<PMusrCanvasNonMusrDataSet> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PMusrCanvasNonMusrDataSet>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PMusrCanvasNonMusrDataSet>", -2, "vector", 493,
                  typeid(vector<PMusrCanvasNonMusrDataSet>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPMusrCanvasNonMusrDataSetgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PMusrCanvasNonMusrDataSet>) );
      instance.SetNew(&new_vectorlEPMusrCanvasNonMusrDataSetgR);
      instance.SetNewArray(&newArray_vectorlEPMusrCanvasNonMusrDataSetgR);
      instance.SetDelete(&delete_vectorlEPMusrCanvasNonMusrDataSetgR);
      instance.SetDeleteArray(&deleteArray_vectorlEPMusrCanvasNonMusrDataSetgR);
      instance.SetDestructor(&destruct_vectorlEPMusrCanvasNonMusrDataSetgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PMusrCanvasNonMusrDataSet> >()));

      ::ROOT::AddClassAlternate("vector<PMusrCanvasNonMusrDataSet>","std::__1::vector<PMusrCanvasNonMusrDataSet, std::__1::allocator<PMusrCanvasNonMusrDataSet> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PMusrCanvasNonMusrDataSet>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPMusrCanvasNonMusrDataSetgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PMusrCanvasNonMusrDataSet>*)nullptr)->GetClass();
      vectorlEPMusrCanvasNonMusrDataSetgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPMusrCanvasNonMusrDataSetgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPMusrCanvasNonMusrDataSetgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PMusrCanvasNonMusrDataSet> : new vector<PMusrCanvasNonMusrDataSet>;
   }
   static void *newArray_vectorlEPMusrCanvasNonMusrDataSetgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PMusrCanvasNonMusrDataSet>[nElements] : new vector<PMusrCanvasNonMusrDataSet>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPMusrCanvasNonMusrDataSetgR(void *p) {
      delete ((vector<PMusrCanvasNonMusrDataSet>*)p);
   }
   static void deleteArray_vectorlEPMusrCanvasNonMusrDataSetgR(void *p) {
      delete [] ((vector<PMusrCanvasNonMusrDataSet>*)p);
   }
   static void destruct_vectorlEPMusrCanvasNonMusrDataSetgR(void *p) {
      typedef vector<PMusrCanvasNonMusrDataSet> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PMusrCanvasNonMusrDataSet>

namespace ROOT {
   static TClass *vectorlEPMusrCanvasDataSetgR_Dictionary();
   static void vectorlEPMusrCanvasDataSetgR_TClassManip(TClass*);
   static void *new_vectorlEPMusrCanvasDataSetgR(void *p = nullptr);
   static void *newArray_vectorlEPMusrCanvasDataSetgR(Long_t size, void *p);
   static void delete_vectorlEPMusrCanvasDataSetgR(void *p);
   static void deleteArray_vectorlEPMusrCanvasDataSetgR(void *p);
   static void destruct_vectorlEPMusrCanvasDataSetgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PMusrCanvasDataSet>*)
   {
      vector<PMusrCanvasDataSet> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PMusrCanvasDataSet>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PMusrCanvasDataSet>", -2, "vector", 493,
                  typeid(vector<PMusrCanvasDataSet>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPMusrCanvasDataSetgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PMusrCanvasDataSet>) );
      instance.SetNew(&new_vectorlEPMusrCanvasDataSetgR);
      instance.SetNewArray(&newArray_vectorlEPMusrCanvasDataSetgR);
      instance.SetDelete(&delete_vectorlEPMusrCanvasDataSetgR);
      instance.SetDeleteArray(&deleteArray_vectorlEPMusrCanvasDataSetgR);
      instance.SetDestructor(&destruct_vectorlEPMusrCanvasDataSetgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PMusrCanvasDataSet> >()));

      ::ROOT::AddClassAlternate("vector<PMusrCanvasDataSet>","std::__1::vector<PMusrCanvasDataSet, std::__1::allocator<PMusrCanvasDataSet> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PMusrCanvasDataSet>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPMusrCanvasDataSetgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PMusrCanvasDataSet>*)nullptr)->GetClass();
      vectorlEPMusrCanvasDataSetgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPMusrCanvasDataSetgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPMusrCanvasDataSetgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PMusrCanvasDataSet> : new vector<PMusrCanvasDataSet>;
   }
   static void *newArray_vectorlEPMusrCanvasDataSetgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PMusrCanvasDataSet>[nElements] : new vector<PMusrCanvasDataSet>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPMusrCanvasDataSetgR(void *p) {
      delete ((vector<PMusrCanvasDataSet>*)p);
   }
   static void deleteArray_vectorlEPMusrCanvasDataSetgR(void *p) {
      delete [] ((vector<PMusrCanvasDataSet>*)p);
   }
   static void destruct_vectorlEPMusrCanvasDataSetgR(void *p) {
      typedef vector<PMusrCanvasDataSet> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PMusrCanvasDataSet>

namespace {
  void TriggerDictionaryInitialization_libPMusrCanvas_Impl() {
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
#line 1 "libPMusrCanvas dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PMusrCanvasPlotRange;
class PMusrCanvas;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPMusrCanvas dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PMusrCanvas.h

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

#ifndef _PMUSRCANVAS_H_
#define _PMUSRCANVAS_H_

#include <TObject.h>
#include <TQObject.h>
#include <TStyle.h>
#include <TRootCanvas.h>
#include <TGMenu.h>
#include <TCanvas.h>
#include <TPaveText.h>
#include <TLegend.h>
#include <TPad.h>
#include <TH1F.h>
#include <TGraphErrors.h>
#include <TMultiGraph.h>
#include <TLatex.h>

#include "PMusr.h"
#ifndef __MAKECLING__
#include "PMsrHandler.h"
#include "PRunListCollection.h"
#endif // __MAKECLING__

#define YINFO  0.1
#define YTITLE 0.95
#define XTHEO  0.75

// Current Plot Views
#define PV_DATA                   1
#define PV_FOURIER_REAL           2
#define PV_FOURIER_IMAG           3
#define PV_FOURIER_REAL_AND_IMAG  4
#define PV_FOURIER_PWR            5
#define PV_FOURIER_PHASE          6
#define PV_FOURIER_PHASE_OPT_REAL 7

// Canvas menu id's
#define P_MENU_ID_DATA          10001
#define P_MENU_ID_FOURIER       10002
#define P_MENU_ID_DIFFERENCE    10003
#define P_MENU_ID_AVERAGE       10004
#define P_MENU_ID_EXPORT_DATA   10005

#define P_MENU_PLOT_OFFSET      1000

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
 * <p>
 */
class PMusrCanvasPlotRange : public TObject
{
  public:
    PMusrCanvasPlotRange();
    virtual ~PMusrCanvasPlotRange() {}

    virtual void SetXRange(Double_t xmin, Double_t xmax);
    virtual void SetYRange(Double_t ymin, Double_t ymax);

    virtual Bool_t IsXRangePresent() { return fXRangePresent; }
    virtual Bool_t IsYRangePresent() { return fYRangePresent; }

    virtual Double_t GetXmin() { return fXmin; }
    virtual Double_t GetXmax() { return fXmax; }
    virtual Double_t GetYmin() { return fYmin; }
    virtual Double_t GetYmax() { return fYmax; }

  private:
    Bool_t fXRangePresent;
    Bool_t fYRangePresent;
    Double_t fXmin;
    Double_t fXmax;
    Double_t fYmin;
    Double_t fYmax;

  ClassDef(PMusrCanvasPlotRange, 1)
};

//------------------------------------------------------------------------
/**
 * <p>Structure holding all necessary histograms for a single plot block entry for
 * fit types: asymmetry fit and single histogram fit.
 */
typedef struct {
  TH1F *data;                ///< data histogram
  TH1F *dataFourierRe;       ///< real part of the Fourier transform of the data histogram
  TH1F *dataFourierIm;       ///< imaginary part of the Fourier transform of the data histogram
  TH1F *dataFourierPwr;      ///< power spectrum of the Fourier transform of the data histogram
  TH1F *dataFourierPhase;    ///< phase spectrum of the Fourier transform of the data histogram
  TH1F *dataFourierPhaseOptReal; ///< phase optimized real part spectrum Fourier transform of the data histogram
  TH1F *theory;              ///< theory histogram belonging to the data histogram
  TH1F *theoryFourierRe;     ///< real part of the Fourier transform of the theory histogram
  TH1F *theoryFourierIm;     ///< imaginary part of the Fourier transform of the theory histogram
  TH1F *theoryFourierPwr;    ///< power spectrum of the Fourier transform of the theory histogram
  TH1F *theoryFourierPhase;  ///< phase spectrum of the Fourier transform of the theory histogram
  TH1F *theoryFourierPhaseOptReal; ///< phase optimized real part spectrum Fourier transform of the theory histogram
  TH1F *diff;                ///< difference histogram, i.e. data-theory
  TH1F *diffFourierRe;       ///< real part of the Fourier transform of the diff histogram
  TH1F *diffFourierIm;       ///< imaginary part of the Fourier transform of the diff histogram
  TH1F *diffFourierPwr;      ///< power spectrum of the Fourier transform of the diff histogram
  TH1F *diffFourierPhase;    ///< phase spectrum of the Fourier transform of the diff histogram
  TH1F *diffFourierPhaseOptReal; ///< phase optimized real part spectrum Fourier transform of the diff histogram
  PMusrCanvasPlotRange *dataRange;    ///< keep the msr-file plot data range
  UInt_t diffFourierTag; ///< 0=not relevant, 1=d-f (Fourier of difference time spectra), 2=f-d (difference of Fourier spectra)
} PMusrCanvasDataSet;

//------------------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of histogram data sets.
 */
typedef std::vector<PMusrCanvasDataSet> PMusrCanvasDataList;

//------------------------------------------------------------------------
/**
 * <p>Structure holding all necessary error graphs for a single plot block entry for
 * fit types: non-muSR fit.
 */
typedef struct {
  TGraphErrors *data;               ///< data error graph
  TGraphErrors *dataFourierRe;      ///< real part of the Fourier transform of the data error graph
  TGraphErrors *dataFourierIm;      ///< imaginary part of the Fourier transform of the data error graph
  TGraphErrors *dataFourierPwr;     ///< power spectrum of the Fourier transform of the data error graph
  TGraphErrors *dataFourierPhase;   ///< phase spectrum of the Fourier transform of the data error graph
  TGraphErrors *theory;             ///< theory histogram belonging to the data error graph
  TGraphErrors *theoryFourierRe;    ///< real part of the Fourier transform of the theory error graph
  TGraphErrors *theoryFourierIm;    ///< imaginary part of the Fourier transform of the theory error graph
  TGraphErrors *theoryFourierPwr;   ///< power spectrum of the Fourier transform of the theory error graph
  TGraphErrors *theoryFourierPhase; ///< phase spectrum of the Fourier transform of the theory error graph
  TGraphErrors *diff;               ///< difference error graph, i.e. data-theory
  TGraphErrors *diffFourierRe;      ///< real part of the Fourier transform of the diff error graph
  TGraphErrors *diffFourierIm;      ///< imaginary part of the Fourier transform of the diff error graph
  TGraphErrors *diffFourierPwr;     ///< power spectrum of the Fourier transform of the diff error graph
  TGraphErrors *diffFourierPhase;   ///< phase spectrum of the Fourier transform of the diff error graph
  PMusrCanvasPlotRange *dataRange;    ///< keep the msr-file plot data range
  UInt_t diffFourierTag; ///< 0=not relevant, 1=d-f (Fourier of difference time spectra), 2=f-d (difference of Fourier spectra)
} PMusrCanvasNonMusrDataSet;

//------------------------------------------------------------------------
/**
 * <p>typedef to make to code more readable: list of error graph data sets.
 */
typedef std::vector<PMusrCanvasNonMusrDataSet> PMusrCanvasNonMusrDataList;

//------------------------------------------------------------------------
/**
 * <p> data structure needed for ascii dump within musrview.
 */
typedef struct {
  PDoubleVector dataX;   ///< x-axis data set
  PDoubleVector data;    ///< y-axis data set
  PDoubleVector dataErr; ///< error of the y-axis data set
} PMusrCanvasAsciiDump;

//------------------------------------------------------------------------
/**
 * <p> typedef to make to code more readable: vector of the above data structure.
 * Used if there are multiple histogramms to be dumped.
 */
typedef std::vector<PMusrCanvasAsciiDump> PMusrCanvasAsciiDumpVector;

//--------------------------------------------------------------------------
/**
 * <p>The preprocessor tag __MAKECLING__ is used to hide away from rootcling
 * the overly complex spirit header files. 
 */
class PMusrCanvas : public TObject, public TQObject
{
  public:
    PMusrCanvas();
    PMusrCanvas(const Int_t number, const Char_t* title,
                Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh, const Bool_t batch,
                const Bool_t fourier=false, const Bool_t avg=false, const Bool_t theoAsData=false);
    PMusrCanvas(const Int_t number, const Char_t* title,
                Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh,
                PMsrFourierStructure fourierDefault,
                const PIntVector markerList, const PIntVector colorList, const Bool_t batch,
                const Bool_t fourier=false, const Bool_t avg=false, const Bool_t theoAsData=false);
    virtual ~PMusrCanvas();

    virtual Bool_t IsValid() { return fValid; }

#ifndef __MAKECLING__
    virtual void SetMsrHandler(PMsrHandler *msrHandler);
    virtual void SetRunListCollection(PRunListCollection *runList) { fRunList = runList; }
#endif // __MAKECLING__

    virtual void SetTimeout(Int_t ival);
    virtual void UpdateParamTheoryPad();
    virtual void UpdateDataTheoryPad();
    virtual void UpdateInfoPad();

    virtual void Done(Int_t status=0); // *SIGNAL*
    virtual void HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected); // SLOT
    virtual void HandleMenuPopup(Int_t id); // SLOT
    virtual void LastCanvasClosed(); // SLOT
    virtual void WindowClosed(); // SLOT

    virtual void SaveGraphicsAndQuit(Char_t *fileName, Char_t *graphicsFormat);
    virtual void ExportData(const Char_t *fileName);

  private:
    Bool_t fTheoAsData;       ///< flag if true, calculate theory points only at the data points
    Bool_t fStartWithFourier; ///< flag if true, the Fourier transform will be presented bypassing the time domain representation
    Bool_t fStartWithAvg;     ///< flag if true, the averaged data/Fourier will be presented
    Int_t  fTimeout;          ///< timeout after which the Done signal should be emited. If timeout <= 0, no timeout is taking place
    Bool_t fScaleN0AndBkg;    ///< true=N0 and background is scaled to (1/ns), otherwise (1/bin) for the single histogram case
    Bool_t fBatchMode;        ///< musrview in ROOT batch mode
    Bool_t fValid;            ///< if true, everything looks OK
    Bool_t fAveragedView;     ///< tag showing that the averaged view or normal view should be presented.
    Bool_t fDifferenceView;   ///< tag showing that the shown data, fourier, are the difference between data and theory
    Bool_t fToggleColor;      ///< tag showing if a single histo theory is color toggled
    Int_t  fCurrentPlotView;  ///< tag showing what the current plot view is: data, fourier, ...
    Int_t  fPreviousPlotView; ///< tag showing the previous plot view
    Int_t  fPlotType;         ///< plot type tag: -1 == undefined, MSR_PLOT_SINGLE_HISTO == single histogram, MSR_PLOT_ASYM == asymmetry, MSR_PLOT_BNMR == beta-NMR asymmetry, MSR_PLOT_MU_MINUS == mu minus (not yet implemented), MSR_PLOT_NON_MUSR == non-muSR
    Int_t  fPlotNumber;       ///< plot number

    Bool_t fXRangePresent, fYRangePresent; ///< flag indicating if x-/y-range is present
    Double_t fXmin, fXmax, fYmin, fYmax;   ///< data/theory frame range

    PDoubleVector fCurrentFourierPhase; ///< holds the current Fourier phase(s)
    TLatex *fCurrentFourierPhaseText; ///< used in Re/Im Fourier to show the current phase in the pad
    TString *fRRFText;      ///< RRF information
    TLatex  *fRRFLatexText; ///< used to display RRF info

    TStyle    *fStyle; ///< A collection of all graphics attributes

    TTimer *fTimeoutTimer; ///< timeout timer in order to terminate if no action is taking place for too long

    // canvas menu related variables
    TRootCanvas *fImp;           ///< ROOT native GUI version of main window with menubar and drawing area
    TGMenuBar   *fBar;           ///< menu bar
    TGPopupMenu *fPopupMain;     ///< popup menu Musrfit in the main menu bar
    TGPopupMenu *fPopupFourier;  ///< popup menu of the Musrfit/Fourier sub menu

    // canvas related variables
    TCanvas   *fMainCanvas;        ///< main canvas
    TPaveText *fTitlePad;          ///< title pad used to display a title
    TPad      *fDataTheoryPad;     ///< data/theory pad used to display the data/theory
    TPaveText *fParameterPad;      ///< parameter pad used to display the fitting parameters
    TPaveText *fTheoryPad;         ///< theory pad used to display the theory and functions
    TLegend   *fInfoPad;           ///< info pad used to display a legend of the data plotted
    TLegend   *fMultiGraphLegend;  ///< used for non-muSR plots to display a legend

    TH1F *fHistoFrame; ///< fHistoFrame is a 'global' frame needed in order to plot histograms with (potentially) different x-frames

#ifndef __MAKECLING__
    PMsrHandler        *fMsrHandler; ///< msr-file handler
    PRunListCollection *fRunList;    ///< data handler
#endif // __MAKECLING__

    PMusrCanvasDataSet fDataAvg; ///< set of all averaged data to be plotted (asymmetry/single histogram)

    PMusrCanvasDataList fData; ///< list of all histogram data to be plotted (asymmetry/single histogram)
    PMusrCanvasNonMusrDataList fNonMusrData; ///< list of all error graphs to be plotted (non-muSR)
    TMultiGraph *fMultiGraphData; ///< fMultiGraphData is a 'global' graph needed in order to plot error graphs (data) with (potentially) different x-frames
    TMultiGraph *fMultiGraphDiff; ///< fMultiGraphDiff is a 'global' graph needed in order to plot error graphs (data-theory) with (potentially) different x-frames

    PMsrFourierStructure fFourier; ///< structure holding all the information necessary to perform the Fourier transform
    PIntVector fMarkerList; ///< list of markers
    PIntVector fColorList;  ///< list of colors

    virtual void CreateStyle();
    virtual void InitFourier();
    virtual void InitAverage();
    virtual void InitMusrCanvas(const Char_t* title, Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
    virtual void InitDataSet(PMusrCanvasDataSet &dataSet);
    virtual void InitDataSet(PMusrCanvasNonMusrDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasDataSet &dataSet);
    virtual void CleanupDataSet(PMusrCanvasNonMusrDataSet &dataSet);
    virtual void HandleDataSet(UInt_t plotNo, UInt_t runNo, PRunData *data);
    virtual void HandleNonMusrDataSet(UInt_t plotNo, UInt_t runNo, PRunData *data);
    virtual void HandleDifference();
    virtual void HandleFourier();
    virtual void HandleDifferenceFourier();
    virtual void HandleFourierDifference();
    virtual void HandleAverage();
    virtual void CleanupDifference();
    virtual void CleanupFourier();
    virtual void CleanupFourierDifference();
    virtual void CleanupAverage();

    virtual void CalcPhaseOptReFT();
    virtual Double_t CalculateDiff(const Double_t x, const Double_t y, TH1F *theo);
    virtual Double_t CalculateDiff(const Double_t x, const Double_t y, TGraphErrors *theo);
    virtual Int_t  FindBin(const Double_t x, TGraphErrors *graph);

    virtual Double_t GetMaximum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMinimum(TH1F* histo, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMaximum(TGraphErrors* graph, Double_t xmin=-1.0, Double_t xmax=-1.0);
    virtual Double_t GetMinimum(TGraphErrors* graph, Double_t xmin=-1.0, Double_t xmax=-1.0);

    virtual void PlotData(Bool_t unzoom=false);
    virtual void PlotDifference(Bool_t unzoom=false);
    virtual void PlotFourier(Bool_t unzoom=false);
    virtual void PlotFourierDifference(Bool_t unzoom=false);
    virtual void PlotFourierPhaseValue(Bool_t unzoom=false);
    virtual void PlotAverage(Bool_t unzoom=false);
    virtual void IncrementFourierPhase();
    virtual void DecrementFourierPhase();

    virtual Bool_t IsScaleN0AndBkg();
    virtual UInt_t GetNeededAccuracy(PMsrParamStructure param);

    virtual Double_t GetInterpolatedValue(TH1F* histo, Double_t xVal);

    virtual void GetExportDataSet(const TH1F *data, const Double_t xmin, const Double_t xmax,
                                  PMusrCanvasAsciiDumpVector &dumpData, const Bool_t hasError=true);

  ClassDef(PMusrCanvas, 1)
};

#endif // _PMUSRCANVAS_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PMusrCanvas", payloadCode, "@",
"PMusrCanvasPlotRange", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPMusrCanvas",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPMusrCanvas_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPMusrCanvas_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPMusrCanvas() {
  TriggerDictionaryInitialization_libPMusrCanvas_Impl();
}
