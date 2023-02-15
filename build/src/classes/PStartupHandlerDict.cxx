// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PStartupHandlerDict
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
#include "PStartupHandler.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PStartupHandler(void *p = nullptr);
   static void *newArray_PStartupHandler(Long_t size, void *p);
   static void delete_PStartupHandler(void *p);
   static void deleteArray_PStartupHandler(void *p);
   static void destruct_PStartupHandler(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PStartupHandler*)
   {
      ::PStartupHandler *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PStartupHandler >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PStartupHandler", ::PStartupHandler::Class_Version(), "", 56,
                  typeid(::PStartupHandler), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PStartupHandler::Dictionary, isa_proxy, 4,
                  sizeof(::PStartupHandler) );
      instance.SetNew(&new_PStartupHandler);
      instance.SetNewArray(&newArray_PStartupHandler);
      instance.SetDelete(&delete_PStartupHandler);
      instance.SetDeleteArray(&deleteArray_PStartupHandler);
      instance.SetDestructor(&destruct_PStartupHandler);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PStartupHandler*)
   {
      return GenerateInitInstanceLocal((::PStartupHandler*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PStartupHandler*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PStartupHandler::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PStartupHandler::Class_Name()
{
   return "PStartupHandler";
}

//______________________________________________________________________________
const char *PStartupHandler::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PStartupHandler::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PStartupHandler::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PStartupHandler::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PStartupHandler::Streamer(TBuffer &R__b)
{
   // Stream an object of class PStartupHandler.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PStartupHandler::Class(),this);
   } else {
      R__b.WriteClassBuffer(PStartupHandler::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PStartupHandler(void *p) {
      return  p ? new(p) ::PStartupHandler : new ::PStartupHandler;
   }
   static void *newArray_PStartupHandler(Long_t nElements, void *p) {
      return p ? new(p) ::PStartupHandler[nElements] : new ::PStartupHandler[nElements];
   }
   // Wrapper around operator delete
   static void delete_PStartupHandler(void *p) {
      delete ((::PStartupHandler*)p);
   }
   static void deleteArray_PStartupHandler(void *p) {
      delete [] ((::PStartupHandler*)p);
   }
   static void destruct_PStartupHandler(void *p) {
      typedef ::PStartupHandler current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PStartupHandler

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
   static TClass *vectorlETStringgR_Dictionary();
   static void vectorlETStringgR_TClassManip(TClass*);
   static void *new_vectorlETStringgR(void *p = nullptr);
   static void *newArray_vectorlETStringgR(Long_t size, void *p);
   static void delete_vectorlETStringgR(void *p);
   static void deleteArray_vectorlETStringgR(void *p);
   static void destruct_vectorlETStringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TString>*)
   {
      vector<TString> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TString>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TString>", -2, "vector", 493,
                  typeid(vector<TString>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETStringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TString>) );
      instance.SetNew(&new_vectorlETStringgR);
      instance.SetNewArray(&newArray_vectorlETStringgR);
      instance.SetDelete(&delete_vectorlETStringgR);
      instance.SetDeleteArray(&deleteArray_vectorlETStringgR);
      instance.SetDestructor(&destruct_vectorlETStringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TString> >()));

      ::ROOT::AddClassAlternate("vector<TString>","std::__1::vector<TString, std::__1::allocator<TString> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TString>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETStringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TString>*)nullptr)->GetClass();
      vectorlETStringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETStringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETStringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TString> : new vector<TString>;
   }
   static void *newArray_vectorlETStringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TString>[nElements] : new vector<TString>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETStringgR(void *p) {
      delete ((vector<TString>*)p);
   }
   static void deleteArray_vectorlETStringgR(void *p) {
      delete [] ((vector<TString>*)p);
   }
   static void destruct_vectorlETStringgR(void *p) {
      typedef vector<TString> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TString>

namespace {
  void TriggerDictionaryInitialization_libPStartupHandler_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
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
#line 1 "libPStartupHandler dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PStartupHandler;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPStartupHandler dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PStartupHandler.h

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

#ifndef _PSTARTUPHANDLER_H_
#define _PSTARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TList.h>
#include <TString.h>
#include <TSAXParser.h>

#include "PMusr.h"

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
//--------------------------------------------------------------------------
int parseXmlFile(TSAXParser*, const char*);

/**
 * <p>Handles the XML musrfit startup file (musrfit_startup.xml) in which default settings
 * are stored:
 * - list of search paths for the data files
 * - Fourier transform default settings
 * - marker list for plotting
 * - color list for plotting
 */
class PStartupHandler : public TObject, public TQObject
{
  public:
    PStartupHandler();
    virtual ~PStartupHandler();

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

    virtual Bool_t StartupFileFound() { return fStartupFileFound; } ///< true = musrfit_startup.xml found
    virtual TString GetStartupFilePath() { return fStartupFilePath; } ///< returns FULLPATH/musrfit_startup.xml, where FULLPATH=path were the musrfit_startup.xml is found

    virtual void CheckLists();

    virtual PMsrFourierStructure  GetFourierDefaults() { return fFourierDefaults; } ///< returns the Fourier defaults
    virtual const PStringVector   GetDataPathList() const { return fDataPathList; } ///< returns the search data path list
    virtual const PIntVector      GetMarkerList() const { return fMarkerList; }     ///< returns the marker list
    virtual const PIntVector      GetColorList() const { return fColorList; }       ///< returns the color list

  private:
    enum EKeyWords {eEmpty, eComment, eDataPath, eOptions,
                    eFourierSettings, eUnits, eFourierPower,
                    eApodization, ePlot, ePhase, ePhaseIncrement,
                    eRootSettings, eMarkerList, eMarker, 
                    eColorList, eColor};
    EKeyWords       fKey; ///< xml filter key

    Bool_t               fStartupFileFound; ///< startup file found flag
    TString              fStartupFilePath;  ///< full musrfit_startup.xml startup file paths
    PMsrFourierStructure fFourierDefaults;  ///< Fourier defaults
    PStringVector        fDataPathList;     ///< search data path list
    PIntVector           fMarkerList;       ///< marker list
    PIntVector           fColorList;        ///< color list

    Bool_t StartupFileExists(Char_t *fln);
    Bool_t WriteDefaultStartupFile();

  ClassDef(PStartupHandler, 1)
};

#endif // _PSTARTUPHANDLER_H_


#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PStartupHandler", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPStartupHandler",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPStartupHandler_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPStartupHandler_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPStartupHandler() {
  TriggerDictionaryInitialization_libPStartupHandler_Impl();
}
