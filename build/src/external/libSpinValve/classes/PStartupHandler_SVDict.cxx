// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PStartupHandler_SVDict
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
#include "PStartupHandler_SV.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PStartupHandler_SV(void *p = nullptr);
   static void *newArray_PStartupHandler_SV(Long_t size, void *p);
   static void delete_PStartupHandler_SV(void *p);
   static void deleteArray_PStartupHandler_SV(void *p);
   static void destruct_PStartupHandler_SV(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PStartupHandler_SV*)
   {
      ::PStartupHandler_SV *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PStartupHandler_SV >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PStartupHandler_SV", ::PStartupHandler_SV::Class_Version(), "", 39,
                  typeid(::PStartupHandler_SV), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PStartupHandler_SV::Dictionary, isa_proxy, 4,
                  sizeof(::PStartupHandler_SV) );
      instance.SetNew(&new_PStartupHandler_SV);
      instance.SetNewArray(&newArray_PStartupHandler_SV);
      instance.SetDelete(&delete_PStartupHandler_SV);
      instance.SetDeleteArray(&deleteArray_PStartupHandler_SV);
      instance.SetDestructor(&destruct_PStartupHandler_SV);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PStartupHandler_SV*)
   {
      return GenerateInitInstanceLocal((::PStartupHandler_SV*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PStartupHandler_SV*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PStartupHandler_SV::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PStartupHandler_SV::Class_Name()
{
   return "PStartupHandler_SV";
}

//______________________________________________________________________________
const char *PStartupHandler_SV::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler_SV*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PStartupHandler_SV::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler_SV*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PStartupHandler_SV::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler_SV*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PStartupHandler_SV::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PStartupHandler_SV*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PStartupHandler_SV::Streamer(TBuffer &R__b)
{
   // Stream an object of class PStartupHandler_SV.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PStartupHandler_SV::Class(),this);
   } else {
      R__b.WriteClassBuffer(PStartupHandler_SV::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PStartupHandler_SV(void *p) {
      return  p ? new(p) ::PStartupHandler_SV : new ::PStartupHandler_SV;
   }
   static void *newArray_PStartupHandler_SV(Long_t nElements, void *p) {
      return p ? new(p) ::PStartupHandler_SV[nElements] : new ::PStartupHandler_SV[nElements];
   }
   // Wrapper around operator delete
   static void delete_PStartupHandler_SV(void *p) {
      delete ((::PStartupHandler_SV*)p);
   }
   static void deleteArray_PStartupHandler_SV(void *p) {
      delete [] ((::PStartupHandler_SV*)p);
   }
   static void destruct_PStartupHandler_SV(void *p) {
      typedef ::PStartupHandler_SV current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PStartupHandler_SV

namespace {
  void TriggerDictionaryInitialization_libPStartupHandler_SV_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/external/libSpinValve/classes/../include",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/libSpinValve/classes/../include",
"/Users/maria/Applications/musrfit/src/external/libSpinValve/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/libSpinValve/classes/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPStartupHandler_SV dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PStartupHandler_SV;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPStartupHandler_SV dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PStartupHandler_SV.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2021 by Andreas Suter                              *
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

#ifndef _PSTARTUPHANDLER_SV_H_
#define _PSTARTUPHANDLER_SV_H_

#include <TObject.h>
#include <TQObject.h>
#include <TString.h>

class PStartupHandler_SV : public TObject
{
  public:
    PStartupHandler_SV();
    virtual ~PStartupHandler_SV() {}

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
    virtual UInt_t GetNoOfFields() { return fNoOfFields; }
    virtual Double_t GetRange() { return fRange; }

    virtual bool StartupFileFound() { return fStartupFileFound; }

  private:
    enum EKeyWords {eEmpty, eComment, eNoOfFields, eRange};
    EKeyWords      fKey;

    bool fIsValid;

    bool    fStartupFileFound;
    TString fStartupFilePath;

    UInt_t   fNoOfFields;
    Double_t fRange;

    bool StartupFileExists(char *fln);

  ClassDef(PStartupHandler_SV, 1)
};

#endif // _PSTARTUPHANDLER_SV_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PStartupHandler_SV", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPStartupHandler_SV",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPStartupHandler_SV_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPStartupHandler_SV_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPStartupHandler_SV() {
  TriggerDictionaryInitialization_libPStartupHandler_SV_Impl();
}
