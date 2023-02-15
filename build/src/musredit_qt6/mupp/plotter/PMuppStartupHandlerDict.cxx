// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PMuppStartupHandlerDict
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
#include "PMuppStartupHandler.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PMuppStartupHandler(void *p = nullptr);
   static void *newArray_PMuppStartupHandler(Long_t size, void *p);
   static void delete_PMuppStartupHandler(void *p);
   static void deleteArray_PMuppStartupHandler(void *p);
   static void destruct_PMuppStartupHandler(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMuppStartupHandler*)
   {
      ::PMuppStartupHandler *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMuppStartupHandler >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMuppStartupHandler", ::PMuppStartupHandler::Class_Version(), "", 51,
                  typeid(::PMuppStartupHandler), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMuppStartupHandler::Dictionary, isa_proxy, 4,
                  sizeof(::PMuppStartupHandler) );
      instance.SetNew(&new_PMuppStartupHandler);
      instance.SetNewArray(&newArray_PMuppStartupHandler);
      instance.SetDelete(&delete_PMuppStartupHandler);
      instance.SetDeleteArray(&deleteArray_PMuppStartupHandler);
      instance.SetDestructor(&destruct_PMuppStartupHandler);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMuppStartupHandler*)
   {
      return GenerateInitInstanceLocal((::PMuppStartupHandler*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMuppStartupHandler*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PMuppStartupHandler::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMuppStartupHandler::Class_Name()
{
   return "PMuppStartupHandler";
}

//______________________________________________________________________________
const char *PMuppStartupHandler::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMuppStartupHandler*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMuppStartupHandler::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMuppStartupHandler*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMuppStartupHandler::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMuppStartupHandler*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMuppStartupHandler::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMuppStartupHandler*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PMuppStartupHandler::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMuppStartupHandler.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMuppStartupHandler::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMuppStartupHandler::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMuppStartupHandler(void *p) {
      return  p ? new(p) ::PMuppStartupHandler : new ::PMuppStartupHandler;
   }
   static void *newArray_PMuppStartupHandler(Long_t nElements, void *p) {
      return p ? new(p) ::PMuppStartupHandler[nElements] : new ::PMuppStartupHandler[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMuppStartupHandler(void *p) {
      delete ((::PMuppStartupHandler*)p);
   }
   static void deleteArray_PMuppStartupHandler(void *p) {
      delete [] ((::PMuppStartupHandler*)p);
   }
   static void destruct_PMuppStartupHandler(void *p) {
      typedef ::PMuppStartupHandler current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMuppStartupHandler

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

namespace {
  void TriggerDictionaryInitialization_libPMuppStartupHandler_Impl() {
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
#line 1 "libPMuppStartupHandler dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PMuppStartupHandler;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPMuppStartupHandler dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PMuppStartupHandler.h

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

#ifndef _PMUPPSTARTUPHANDLER_H_
#define _PMUPPSTARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TList.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TColor.h>
#include <TSAXParser.h>

#include "mupp_plot.h"

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
//--------------------------------------------------------------------------
int parseXmlFile(TSAXParser*, const char*);

class PMuppStartupHandler : public TObject, public TQObject
{
public:
  PMuppStartupHandler();
  virtual ~PMuppStartupHandler();

  virtual void OnStartDocument(); // SLOT
  virtual void OnEndDocument(); // SLOT
  virtual void OnStartElement(const Char_t*, const TList*); // SLOT
  virtual void OnEndElement(const Char_t*); // SLOT
  virtual void OnCharacters(const Char_t*); // SLOT
  virtual void OnComment(const Char_t*); // SLOT
  virtual void OnWarning(const Char_t*); // SLOT
  virtual void OnError(const Char_t*); // SLOT
  virtual void OnFatalError(const Char_t*); // SLOT
  virtual void OnCdataBlock(const Char_t*, Int_t); // SLOT

  virtual Bool_t StartupFileFound() { return fStartupFileFound; } ///< true = mupp_startup.xml found
  virtual TString GetStartupFilePath() { return fStartupFilePath; } ///< returns FULLPATH/mupp_startup.xml, where FULLPATH=path were the mupp_startup.xml is found

  virtual void CheckLists();

  virtual const PIntVector GetMarkerStyleList() const { return fMarkerStyleList; }  ///< returns the marker style list
  virtual const PDoubleVector GetMarkerSizeList() const { return fMarkerSizeList; } ///< returns the marker size list
  virtual const PIntVector GetColorList() const { return fColorList; }              ///< returns the color list

private:
  enum EKeyWords {eEmpty, eMarker, eColor};
  EKeyWords       fKey; ///< xml filter key

  Bool_t               fStartupFileFound; ///< startup file found flag
  TString              fStartupFilePath;  ///< full mupp_startup.xml startup file paths
  PIntVector           fMarkerStyleList;  ///< marker style list
  PDoubleVector        fMarkerSizeList;   ///< marker size list
  PIntVector           fColorList;        ///< color list

  Bool_t StartupFileExists(Char_t *fln);

  ClassDef(PMuppStartupHandler, 1)
};

#endif // _PMUPPSTARTUPHANDLER_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PMuppStartupHandler", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPMuppStartupHandler",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPMuppStartupHandler_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPMuppStartupHandler_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPMuppStartupHandler() {
  TriggerDictionaryInitialization_libPMuppStartupHandler_Impl();
}
