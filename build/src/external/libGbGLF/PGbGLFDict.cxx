// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PGbGLFDict
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
#include "PGbGLF.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PGbGLF(void *p = nullptr);
   static void *newArray_PGbGLF(Long_t size, void *p);
   static void delete_PGbGLF(void *p);
   static void deleteArray_PGbGLF(void *p);
   static void destruct_PGbGLF(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PGbGLF*)
   {
      ::PGbGLF *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PGbGLF >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PGbGLF", ::PGbGLF::Class_Version(), "", 43,
                  typeid(::PGbGLF), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PGbGLF::Dictionary, isa_proxy, 4,
                  sizeof(::PGbGLF) );
      instance.SetNew(&new_PGbGLF);
      instance.SetNewArray(&newArray_PGbGLF);
      instance.SetDelete(&delete_PGbGLF);
      instance.SetDeleteArray(&deleteArray_PGbGLF);
      instance.SetDestructor(&destruct_PGbGLF);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PGbGLF*)
   {
      return GenerateInitInstanceLocal((::PGbGLF*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PGbGLF*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PGbGLF::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PGbGLF::Class_Name()
{
   return "PGbGLF";
}

//______________________________________________________________________________
const char *PGbGLF::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PGbGLF*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PGbGLF::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PGbGLF*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PGbGLF::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PGbGLF*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PGbGLF::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PGbGLF*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PGbGLF::Streamer(TBuffer &R__b)
{
   // Stream an object of class PGbGLF.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PGbGLF::Class(),this);
   } else {
      R__b.WriteClassBuffer(PGbGLF::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PGbGLF(void *p) {
      return  p ? new(p) ::PGbGLF : new ::PGbGLF;
   }
   static void *newArray_PGbGLF(Long_t nElements, void *p) {
      return p ? new(p) ::PGbGLF[nElements] : new ::PGbGLF[nElements];
   }
   // Wrapper around operator delete
   static void delete_PGbGLF(void *p) {
      delete ((::PGbGLF*)p);
   }
   static void deleteArray_PGbGLF(void *p) {
      delete [] ((::PGbGLF*)p);
   }
   static void destruct_PGbGLF(void *p) {
      typedef ::PGbGLF current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PGbGLF

namespace {
  void TriggerDictionaryInitialization_libPGbGLF_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/external/libGbGLF",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/libGbGLF",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/libGbGLF/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPGbGLF dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PGbGLF;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPGbGLF dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PGbGLF.h

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

#ifndef _PGBGLF_H_
#define _PGBGLF_H_

#include <vector>

#include "PUserFcnBase.h"

//--------------------------------------------------------------------------------------------
/**
 * <p>Interface class for the user function.
 */
class PGbGLF : public PUserFcnBase
{
  public:
    PGbGLF() {}
    virtual ~PGbGLF() {}

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Double_t pz_GbG_2(Double_t t, const std::vector<Double_t> &par) const;

    ClassDef(PGbGLF, 1)
};

#endif // _PGBGLF_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PGbGLF", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPGbGLF",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPGbGLF_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPGbGLF_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPGbGLF() {
  TriggerDictionaryInitialization_libPGbGLF_Impl();
}
