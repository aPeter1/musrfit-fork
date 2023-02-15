// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PNL_StartupHandlerDict
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
#include "PNL_StartupHandler.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PNL_StartupHandler(void *p = nullptr);
   static void *newArray_PNL_StartupHandler(Long_t size, void *p);
   static void delete_PNL_StartupHandler(void *p);
   static void deleteArray_PNL_StartupHandler(void *p);
   static void destruct_PNL_StartupHandler(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PNL_StartupHandler*)
   {
      ::PNL_StartupHandler *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PNL_StartupHandler >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PNL_StartupHandler", ::PNL_StartupHandler::Class_Version(), "", 39,
                  typeid(::PNL_StartupHandler), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PNL_StartupHandler::Dictionary, isa_proxy, 4,
                  sizeof(::PNL_StartupHandler) );
      instance.SetNew(&new_PNL_StartupHandler);
      instance.SetNewArray(&newArray_PNL_StartupHandler);
      instance.SetDelete(&delete_PNL_StartupHandler);
      instance.SetDeleteArray(&deleteArray_PNL_StartupHandler);
      instance.SetDestructor(&destruct_PNL_StartupHandler);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PNL_StartupHandler*)
   {
      return GenerateInitInstanceLocal((::PNL_StartupHandler*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PNL_StartupHandler*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PNL_StartupHandler::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PNL_StartupHandler::Class_Name()
{
   return "PNL_StartupHandler";
}

//______________________________________________________________________________
const char *PNL_StartupHandler::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PNL_StartupHandler*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PNL_StartupHandler::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PNL_StartupHandler*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PNL_StartupHandler::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PNL_StartupHandler*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PNL_StartupHandler::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PNL_StartupHandler*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PNL_StartupHandler::Streamer(TBuffer &R__b)
{
   // Stream an object of class PNL_StartupHandler.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PNL_StartupHandler::Class(),this);
   } else {
      R__b.WriteClassBuffer(PNL_StartupHandler::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PNL_StartupHandler(void *p) {
      return  p ? new(p) ::PNL_StartupHandler : new ::PNL_StartupHandler;
   }
   static void *newArray_PNL_StartupHandler(Long_t nElements, void *p) {
      return p ? new(p) ::PNL_StartupHandler[nElements] : new ::PNL_StartupHandler[nElements];
   }
   // Wrapper around operator delete
   static void delete_PNL_StartupHandler(void *p) {
      delete ((::PNL_StartupHandler*)p);
   }
   static void deleteArray_PNL_StartupHandler(void *p) {
      delete [] ((::PNL_StartupHandler*)p);
   }
   static void destruct_PNL_StartupHandler(void *p) {
      typedef ::PNL_StartupHandler current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PNL_StartupHandler

namespace {
  void TriggerDictionaryInitialization_libPNL_StartupHandler_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/external/Nonlocal",
"/Users/maria/Applications/musrfit/src/external/Nonlocal",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/external/Nonlocal",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/Nonlocal/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPNL_StartupHandler dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PNL_StartupHandler;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPNL_StartupHandler dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PNL_StartupHandler.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2009-2021 by Andreas Suter                              *
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

#ifndef _PNL_STARTUPHANDLER_H_
#define _PNL_STARTUPHANDLER_H_

#include <TObject.h>
#include <TQObject.h>
#include <TString.h>

class PNL_StartupHandler : public TObject
{
  public:
    PNL_StartupHandler();
    virtual ~PNL_StartupHandler() {}

    virtual void OnStartDocument(); // SLOT
    virtual void OnEndDocument(); // SLOT
    virtual void OnStartElement(const char*, const TList*); // SLOT
    virtual void OnEndElement(const char*); // SLOT
    virtual void OnCharacters(const char*); // SLOT
    virtual void OnComment(const char*); // SLOT
    virtual void OnWarning(const char*); // SLOT
    virtual void OnError(const char*); // SLOT
    virtual void OnFatalError(const char*); // SLOT
    virtual void OnCdataBlock(const char*, Int_t); // SLOT

    virtual bool IsValid() { return fIsValid; }
    virtual TString GetStartupFilePath() { return fStartupFilePath; }
    virtual const Int_t GetFourierPoints() const { return fFourierPoints; }
    virtual bool StartupFileFound() { return fStartupFileFound; }

  private:
    enum EKeyWords {eEmpty, eFourierPoints};
    EKeyWords      fKey;

    bool fIsValid{true};

    bool    fStartupFileFound{false};
    TString fStartupFilePath{""};

    Int_t fFourierPoints{0};

  ClassDef(PNL_StartupHandler, 1)
};

#endif // _PNL_STARTUPHANDLER_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PNL_StartupHandler", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPNL_StartupHandler",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPNL_StartupHandler_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPNL_StartupHandler_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPNL_StartupHandler() {
  TriggerDictionaryInitialization_libPNL_StartupHandler_Impl();
}
