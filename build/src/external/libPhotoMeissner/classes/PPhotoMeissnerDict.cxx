// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PPhotoMeissnerDict
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
#include "PPhotoMeissner.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PPhotoMeissner(void *p = nullptr);
   static void *newArray_PPhotoMeissner(Long_t size, void *p);
   static void delete_PPhotoMeissner(void *p);
   static void deleteArray_PPhotoMeissner(void *p);
   static void destruct_PPhotoMeissner(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PPhotoMeissner*)
   {
      ::PPhotoMeissner *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PPhotoMeissner >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PPhotoMeissner", ::PPhotoMeissner::Class_Version(), "", 40,
                  typeid(::PPhotoMeissner), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PPhotoMeissner::Dictionary, isa_proxy, 4,
                  sizeof(::PPhotoMeissner) );
      instance.SetNew(&new_PPhotoMeissner);
      instance.SetNewArray(&newArray_PPhotoMeissner);
      instance.SetDelete(&delete_PPhotoMeissner);
      instance.SetDeleteArray(&deleteArray_PPhotoMeissner);
      instance.SetDestructor(&destruct_PPhotoMeissner);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PPhotoMeissner*)
   {
      return GenerateInitInstanceLocal((::PPhotoMeissner*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PPhotoMeissner*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PPhotoMeissner::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PPhotoMeissner::Class_Name()
{
   return "PPhotoMeissner";
}

//______________________________________________________________________________
const char *PPhotoMeissner::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PPhotoMeissner*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PPhotoMeissner::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PPhotoMeissner*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PPhotoMeissner::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PPhotoMeissner*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PPhotoMeissner::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PPhotoMeissner*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PPhotoMeissner::Streamer(TBuffer &R__b)
{
   // Stream an object of class PPhotoMeissner.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PPhotoMeissner::Class(),this);
   } else {
      R__b.WriteClassBuffer(PPhotoMeissner::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PPhotoMeissner(void *p) {
      return  p ? new(p) ::PPhotoMeissner : new ::PPhotoMeissner;
   }
   static void *newArray_PPhotoMeissner(Long_t nElements, void *p) {
      return p ? new(p) ::PPhotoMeissner[nElements] : new ::PPhotoMeissner[nElements];
   }
   // Wrapper around operator delete
   static void delete_PPhotoMeissner(void *p) {
      delete ((::PPhotoMeissner*)p);
   }
   static void deleteArray_PPhotoMeissner(void *p) {
      delete [] ((::PPhotoMeissner*)p);
   }
   static void destruct_PPhotoMeissner(void *p) {
      typedef ::PPhotoMeissner current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PPhotoMeissner

namespace {
  void TriggerDictionaryInitialization_libPPhotoMeissner_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/opt/homebrew/Cellar/gsl/2.7.1/include",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/external/libPhotoMeissner/classes/../include",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/libPhotoMeissner/classes/../include",
"/Users/maria/Applications/musrfit/src/external/libPhotoMeissner/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/libPhotoMeissner/classes/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPPhotoMeissner dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PPhotoMeissner;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPPhotoMeissner dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PPhotoMeissner.h

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

#ifndef _PPHOTOMEISSNER_H_
#define _PPHOTOMEISSNER_H_

#include <vector>

#include "PUserFcnBase.h"
#include "PRgeHandler.h"

class PPhotoMeissner : public PUserFcnBase
{
  public:
    PPhotoMeissner();
    virtual ~PPhotoMeissner();

    virtual Bool_t IsValid() { return fValid; }

    // function operator
    Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    PRgeHandler *fRgeHandler{nullptr};

    Bool_t   fValid{true};      ///< flag indicating if initialization went through smoothly

    constexpr static const Double_t fDegToRad = 0.0174532925199432955;
    constexpr static const Double_t fTwoPi = 6.28318530717958623;

    Double_t InuMinus(const Double_t nu, const Double_t x) const;
    Double_t FieldFilm(const Double_t z, const std::vector<Double_t> &par) const;
    Double_t FieldHalfSpace(const Double_t z, const std::vector<Double_t> &par) const;

  // definition of the class for the ROOT dictionary
  ClassDef(PPhotoMeissner, 1)
};

#endif // _PPHOTOMEISSNER_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PPhotoMeissner", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPPhotoMeissner",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPPhotoMeissner_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPPhotoMeissner_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPPhotoMeissner() {
  TriggerDictionaryInitialization_libPPhotoMeissner_Impl();
}
