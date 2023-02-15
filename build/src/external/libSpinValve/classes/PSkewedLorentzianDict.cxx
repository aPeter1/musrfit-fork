// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PSkewedLorentzianDict
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
#include "PSkewedLorentzian.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PSkewedLorentzian(void *p = nullptr);
   static void *newArray_PSkewedLorentzian(Long_t size, void *p);
   static void delete_PSkewedLorentzian(void *p);
   static void deleteArray_PSkewedLorentzian(void *p);
   static void destruct_PSkewedLorentzian(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PSkewedLorentzian*)
   {
      ::PSkewedLorentzian *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PSkewedLorentzian >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PSkewedLorentzian", ::PSkewedLorentzian::Class_Version(), "", 40,
                  typeid(::PSkewedLorentzian), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PSkewedLorentzian::Dictionary, isa_proxy, 4,
                  sizeof(::PSkewedLorentzian) );
      instance.SetNew(&new_PSkewedLorentzian);
      instance.SetNewArray(&newArray_PSkewedLorentzian);
      instance.SetDelete(&delete_PSkewedLorentzian);
      instance.SetDeleteArray(&deleteArray_PSkewedLorentzian);
      instance.SetDestructor(&destruct_PSkewedLorentzian);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PSkewedLorentzian*)
   {
      return GenerateInitInstanceLocal((::PSkewedLorentzian*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PSkewedLorentzian*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PSkewedLorentzian::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PSkewedLorentzian::Class_Name()
{
   return "PSkewedLorentzian";
}

//______________________________________________________________________________
const char *PSkewedLorentzian::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PSkewedLorentzian*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PSkewedLorentzian::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PSkewedLorentzian*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PSkewedLorentzian::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PSkewedLorentzian*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PSkewedLorentzian::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PSkewedLorentzian*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PSkewedLorentzian::Streamer(TBuffer &R__b)
{
   // Stream an object of class PSkewedLorentzian.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PSkewedLorentzian::Class(),this);
   } else {
      R__b.WriteClassBuffer(PSkewedLorentzian::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PSkewedLorentzian(void *p) {
      return  p ? new(p) ::PSkewedLorentzian : new ::PSkewedLorentzian;
   }
   static void *newArray_PSkewedLorentzian(Long_t nElements, void *p) {
      return p ? new(p) ::PSkewedLorentzian[nElements] : new ::PSkewedLorentzian[nElements];
   }
   // Wrapper around operator delete
   static void delete_PSkewedLorentzian(void *p) {
      delete ((::PSkewedLorentzian*)p);
   }
   static void deleteArray_PSkewedLorentzian(void *p) {
      delete [] ((::PSkewedLorentzian*)p);
   }
   static void destruct_PSkewedLorentzian(void *p) {
      typedef ::PSkewedLorentzian current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PSkewedLorentzian

namespace {
  void TriggerDictionaryInitialization_libPSkewedLorentzian_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/external/libSpinValve/classes/../include",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/libSpinValve/classes/../include",
"/Users/maria/Applications/musrfit/src/external/libSpinValve/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/libSpinValve/classes/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPSkewedLorentzian dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PSkewedLorentzian;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPSkewedLorentzian dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PSkewedLorentzian.h

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

#ifndef _PSKEWEDLORENTZIAN_H_
#define _PSKEWEDLORENTZIAN_H_

#include <vector>

#include "PUserFcnBase.h"
#include "PStartupHandler_SV.h"

class PSkewedLorentzian : public PUserFcnBase
{
  public:
    PSkewedLorentzian();
    virtual ~PSkewedLorentzian();

    virtual Bool_t IsValid() { return fValid; }

    // function operator
    Double_t operator()(Double_t, const std::vector<Double_t>&) const;

  private:
    PStartupHandler_SV *fStartupHandler;

    Bool_t   fValid;      ///< flag indicating if initialization went through smoothly
    UInt_t   fNoOfFields; ///< number of sampling points in field around the Lorentzian peak
    Double_t fRange;      ///< range in which the sampling points are placed, given in units of \beta(1\pm\Delta)

    constexpr static const Double_t fDegToRad = 0.0174532925199432955;
    constexpr static const Double_t fTwoPi = 6.28318530717958623;

  // definition of the class for the ROOT dictionary
  ClassDef(PSkewedLorentzian, 1)
};

#endif // _PSKEWEDLORENTZIAN_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PSkewedLorentzian", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPSkewedLorentzian",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPSkewedLorentzian_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPSkewedLorentzian_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPSkewedLorentzian() {
  TriggerDictionaryInitialization_libPSkewedLorentzian_Impl();
}
