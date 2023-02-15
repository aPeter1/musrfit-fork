// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PMagProximityFitterDict
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
#include "PMagProximityFitter.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PMagProximityFitterGlobal(void *p = nullptr);
   static void *newArray_PMagProximityFitterGlobal(Long_t size, void *p);
   static void delete_PMagProximityFitterGlobal(void *p);
   static void deleteArray_PMagProximityFitterGlobal(void *p);
   static void destruct_PMagProximityFitterGlobal(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMagProximityFitterGlobal*)
   {
      ::PMagProximityFitterGlobal *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMagProximityFitterGlobal >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMagProximityFitterGlobal", ::PMagProximityFitterGlobal::Class_Version(), "", 39,
                  typeid(::PMagProximityFitterGlobal), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMagProximityFitterGlobal::Dictionary, isa_proxy, 4,
                  sizeof(::PMagProximityFitterGlobal) );
      instance.SetNew(&new_PMagProximityFitterGlobal);
      instance.SetNewArray(&newArray_PMagProximityFitterGlobal);
      instance.SetDelete(&delete_PMagProximityFitterGlobal);
      instance.SetDeleteArray(&deleteArray_PMagProximityFitterGlobal);
      instance.SetDestructor(&destruct_PMagProximityFitterGlobal);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMagProximityFitterGlobal*)
   {
      return GenerateInitInstanceLocal((::PMagProximityFitterGlobal*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMagProximityFitterGlobal*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_PMagProximityFitter(void *p = nullptr);
   static void *newArray_PMagProximityFitter(Long_t size, void *p);
   static void delete_PMagProximityFitter(void *p);
   static void deleteArray_PMagProximityFitter(void *p);
   static void destruct_PMagProximityFitter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PMagProximityFitter*)
   {
      ::PMagProximityFitter *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PMagProximityFitter >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PMagProximityFitter", ::PMagProximityFitter::Class_Version(), "", 67,
                  typeid(::PMagProximityFitter), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PMagProximityFitter::Dictionary, isa_proxy, 4,
                  sizeof(::PMagProximityFitter) );
      instance.SetNew(&new_PMagProximityFitter);
      instance.SetNewArray(&newArray_PMagProximityFitter);
      instance.SetDelete(&delete_PMagProximityFitter);
      instance.SetDeleteArray(&deleteArray_PMagProximityFitter);
      instance.SetDestructor(&destruct_PMagProximityFitter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PMagProximityFitter*)
   {
      return GenerateInitInstanceLocal((::PMagProximityFitter*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PMagProximityFitter*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PMagProximityFitterGlobal::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMagProximityFitterGlobal::Class_Name()
{
   return "PMagProximityFitterGlobal";
}

//______________________________________________________________________________
const char *PMagProximityFitterGlobal::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitterGlobal*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMagProximityFitterGlobal::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitterGlobal*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMagProximityFitterGlobal::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitterGlobal*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMagProximityFitterGlobal::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitterGlobal*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr PMagProximityFitter::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PMagProximityFitter::Class_Name()
{
   return "PMagProximityFitter";
}

//______________________________________________________________________________
const char *PMagProximityFitter::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitter*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PMagProximityFitter::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitter*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PMagProximityFitter::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitter*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PMagProximityFitter::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PMagProximityFitter*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PMagProximityFitterGlobal::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMagProximityFitterGlobal.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMagProximityFitterGlobal::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMagProximityFitterGlobal::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMagProximityFitterGlobal(void *p) {
      return  p ? new(p) ::PMagProximityFitterGlobal : new ::PMagProximityFitterGlobal;
   }
   static void *newArray_PMagProximityFitterGlobal(Long_t nElements, void *p) {
      return p ? new(p) ::PMagProximityFitterGlobal[nElements] : new ::PMagProximityFitterGlobal[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMagProximityFitterGlobal(void *p) {
      delete ((::PMagProximityFitterGlobal*)p);
   }
   static void deleteArray_PMagProximityFitterGlobal(void *p) {
      delete [] ((::PMagProximityFitterGlobal*)p);
   }
   static void destruct_PMagProximityFitterGlobal(void *p) {
      typedef ::PMagProximityFitterGlobal current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMagProximityFitterGlobal

//______________________________________________________________________________
void PMagProximityFitter::Streamer(TBuffer &R__b)
{
   // Stream an object of class PMagProximityFitter.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PMagProximityFitter::Class(),this);
   } else {
      R__b.WriteClassBuffer(PMagProximityFitter::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PMagProximityFitter(void *p) {
      return  p ? new(p) ::PMagProximityFitter : new ::PMagProximityFitter;
   }
   static void *newArray_PMagProximityFitter(Long_t nElements, void *p) {
      return p ? new(p) ::PMagProximityFitter[nElements] : new ::PMagProximityFitter[nElements];
   }
   // Wrapper around operator delete
   static void delete_PMagProximityFitter(void *p) {
      delete ((::PMagProximityFitter*)p);
   }
   static void deleteArray_PMagProximityFitter(void *p) {
      delete [] ((::PMagProximityFitter*)p);
   }
   static void destruct_PMagProximityFitter(void *p) {
      typedef ::PMagProximityFitter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PMagProximityFitter

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
  void TriggerDictionaryInitialization_libPMagProximityFitter_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/external/MagProximity",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/MagProximity",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/MagProximity/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPMagProximityFitter dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PMagProximityFitterGlobal;
class PMagProximityFitter;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPMagProximityFitter dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PMagProximity.h

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

#ifndef _PMAGPROXIMITYFITTER_H_
#define _PMAGPROXIMITYFITTER_H_

#include "PUserFcnBase.h"
#include "PMPStartupHandler.h"
#include "PRgeHandler.h"

class PMagProximityFitterGlobal
{
  public:
    PMagProximityFitterGlobal();
    virtual ~PMagProximityFitterGlobal();

    Bool_t IsValid() { return fValid; }
    virtual void CalculateField(const std::vector<Double_t> &param) const;
    virtual Int_t GetEnergyIndex(const Double_t energy) { return fRgeHandler->GetEnergyIndex(energy); }
    virtual Double_t GetMuonStoppingDensity(const Int_t energyIndex, const Double_t z) const { return fRgeHandler->Get_n(energyIndex, z); }
    virtual Double_t GetMagneticField(const Double_t z) const;

  private:
    Bool_t fValid;

    PMPStartupHandler *fStartupHandler;
    PRgeHandler *fRgeHandler;

    mutable std::vector<Double_t> fPreviousParam;

    mutable Int_t fEnergyIndex; // keeps the proper index to select n(z)

    mutable Double_t fDz;
    mutable std::vector<Double_t> fField;

  ClassDef(PMagProximityFitterGlobal, 1)
};

class PMagProximityFitter : public PUserFcnBase
{
  public:
    PMagProximityFitter();
    virtual ~PMagProximityFitter();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid;
    Bool_t fInvokedGlobal;
    Int_t fIdxGlobal;

    PMagProximityFitterGlobal *fMagProximityFitterGlobal;

  ClassDef(PMagProximityFitter, 1)
};

#endif // _PMAGPROXIMITYFITTER_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PMagProximityFitter", payloadCode, "@",
"PMagProximityFitterGlobal", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPMagProximityFitter",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPMagProximityFitter_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPMagProximityFitter_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPMagProximityFitter() {
  TriggerDictionaryInitialization_libPMagProximityFitter_Impl();
}
