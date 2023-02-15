// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PNL_PippardFitterDict
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
#include "PNL_PippardFitter.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PNL_PippardFitterGlobal(void *p = nullptr);
   static void *newArray_PNL_PippardFitterGlobal(Long_t size, void *p);
   static void delete_PNL_PippardFitterGlobal(void *p);
   static void deleteArray_PNL_PippardFitterGlobal(void *p);
   static void destruct_PNL_PippardFitterGlobal(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PNL_PippardFitterGlobal*)
   {
      ::PNL_PippardFitterGlobal *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PNL_PippardFitterGlobal >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PNL_PippardFitterGlobal", ::PNL_PippardFitterGlobal::Class_Version(), "", 46,
                  typeid(::PNL_PippardFitterGlobal), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PNL_PippardFitterGlobal::Dictionary, isa_proxy, 4,
                  sizeof(::PNL_PippardFitterGlobal) );
      instance.SetNew(&new_PNL_PippardFitterGlobal);
      instance.SetNewArray(&newArray_PNL_PippardFitterGlobal);
      instance.SetDelete(&delete_PNL_PippardFitterGlobal);
      instance.SetDeleteArray(&deleteArray_PNL_PippardFitterGlobal);
      instance.SetDestructor(&destruct_PNL_PippardFitterGlobal);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PNL_PippardFitterGlobal*)
   {
      return GenerateInitInstanceLocal((::PNL_PippardFitterGlobal*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PNL_PippardFitterGlobal*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_PNL_PippardFitter(void *p = nullptr);
   static void *newArray_PNL_PippardFitter(Long_t size, void *p);
   static void delete_PNL_PippardFitter(void *p);
   static void deleteArray_PNL_PippardFitter(void *p);
   static void destruct_PNL_PippardFitter(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PNL_PippardFitter*)
   {
      ::PNL_PippardFitter *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PNL_PippardFitter >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PNL_PippardFitter", ::PNL_PippardFitter::Class_Version(), "", 88,
                  typeid(::PNL_PippardFitter), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PNL_PippardFitter::Dictionary, isa_proxy, 4,
                  sizeof(::PNL_PippardFitter) );
      instance.SetNew(&new_PNL_PippardFitter);
      instance.SetNewArray(&newArray_PNL_PippardFitter);
      instance.SetDelete(&delete_PNL_PippardFitter);
      instance.SetDeleteArray(&deleteArray_PNL_PippardFitter);
      instance.SetDestructor(&destruct_PNL_PippardFitter);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PNL_PippardFitter*)
   {
      return GenerateInitInstanceLocal((::PNL_PippardFitter*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PNL_PippardFitter*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PNL_PippardFitterGlobal::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PNL_PippardFitterGlobal::Class_Name()
{
   return "PNL_PippardFitterGlobal";
}

//______________________________________________________________________________
const char *PNL_PippardFitterGlobal::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitterGlobal*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PNL_PippardFitterGlobal::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitterGlobal*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PNL_PippardFitterGlobal::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitterGlobal*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PNL_PippardFitterGlobal::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitterGlobal*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr PNL_PippardFitter::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PNL_PippardFitter::Class_Name()
{
   return "PNL_PippardFitter";
}

//______________________________________________________________________________
const char *PNL_PippardFitter::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitter*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PNL_PippardFitter::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitter*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PNL_PippardFitter::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitter*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PNL_PippardFitter::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PNL_PippardFitter*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PNL_PippardFitterGlobal::Streamer(TBuffer &R__b)
{
   // Stream an object of class PNL_PippardFitterGlobal.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PNL_PippardFitterGlobal::Class(),this);
   } else {
      R__b.WriteClassBuffer(PNL_PippardFitterGlobal::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PNL_PippardFitterGlobal(void *p) {
      return  p ? new(p) ::PNL_PippardFitterGlobal : new ::PNL_PippardFitterGlobal;
   }
   static void *newArray_PNL_PippardFitterGlobal(Long_t nElements, void *p) {
      return p ? new(p) ::PNL_PippardFitterGlobal[nElements] : new ::PNL_PippardFitterGlobal[nElements];
   }
   // Wrapper around operator delete
   static void delete_PNL_PippardFitterGlobal(void *p) {
      delete ((::PNL_PippardFitterGlobal*)p);
   }
   static void deleteArray_PNL_PippardFitterGlobal(void *p) {
      delete [] ((::PNL_PippardFitterGlobal*)p);
   }
   static void destruct_PNL_PippardFitterGlobal(void *p) {
      typedef ::PNL_PippardFitterGlobal current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PNL_PippardFitterGlobal

//______________________________________________________________________________
void PNL_PippardFitter::Streamer(TBuffer &R__b)
{
   // Stream an object of class PNL_PippardFitter.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PNL_PippardFitter::Class(),this);
   } else {
      R__b.WriteClassBuffer(PNL_PippardFitter::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PNL_PippardFitter(void *p) {
      return  p ? new(p) ::PNL_PippardFitter : new ::PNL_PippardFitter;
   }
   static void *newArray_PNL_PippardFitter(Long_t nElements, void *p) {
      return p ? new(p) ::PNL_PippardFitter[nElements] : new ::PNL_PippardFitter[nElements];
   }
   // Wrapper around operator delete
   static void delete_PNL_PippardFitter(void *p) {
      delete ((::PNL_PippardFitter*)p);
   }
   static void deleteArray_PNL_PippardFitter(void *p) {
      delete [] ((::PNL_PippardFitter*)p);
   }
   static void destruct_PNL_PippardFitter(void *p) {
      typedef ::PNL_PippardFitter current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PNL_PippardFitter

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
  void TriggerDictionaryInitialization_libPNL_PippardFitter_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/Users/maria/Applications/musrfit/src/include",
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
#line 1 "libPNL_PippardFitter dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PNL_PippardFitterGlobal;
class PNL_PippardFitter;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPNL_PippardFitter dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PNL_PippardFitter.h

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

#ifndef _PNL_PIPPARDFITTER_H_
#define _PNL_PIPPARDFITTER_H_

// the following ifdef is needed for GCC 4.6 or higher, fftw 3.3 or higher and root 5.30.03 or lower
//#ifdef __CINT__
//typedef struct { char a[7]; } __float128; // needed since cint doesn't know it
//#endif
#include <fftw3.h>

#include "PMusr.h"
#include "PUserFcnBase.h"
#include "PNL_StartupHandler.h"
#include "PRgeHandler.h"

class PNL_PippardFitterGlobal
{
  public:
    PNL_PippardFitterGlobal();
    virtual ~PNL_PippardFitterGlobal();

    Bool_t IsValid() { return fValid; }
    virtual void SetTempExponent(const Double_t nn) { f_nn = nn; }
    virtual void CalculateField(const std::vector<Double_t> &param) const;
    virtual Int_t GetEnergyIndex(const Double_t energy) { return fRgeHandler->GetEnergyIndex(energy); }
    virtual Double_t GetMuonStoppingDensity(const Int_t energyIndex, const Double_t z) const { return fRgeHandler->Get_n(energyIndex, z); }
    virtual Double_t GetMagneticField(const Double_t z) const;    

  private:
    Bool_t fValid{true};

    PNL_StartupHandler *fStartupHandler{nullptr};
    PRgeHandler *fRgeHandler{nullptr};

    mutable std::vector<Double_t> fPreviousParam;

    Double_t f_nn;
    Double_t f_dx;           // dx = xiPT dq
    mutable Double_t f_dz;   // spatial step size

    mutable Bool_t fPlanPresent;
    mutable Int_t fFourierPoints;
    mutable fftw_plan     fPlan;
    mutable fftw_complex *fFieldq;   // (xiPT x)/(x^2 + xiPT^2 K(x,T)), x = q xiPT
    mutable fftw_complex *fFieldB;   // field calculated for specular boundary conditions
    mutable Int_t fShift; // shift needed to pick up fFieldB at the maximum for B->0

    mutable Int_t fEnergyIndex; // keeps the proper index to select n(z)


    virtual Double_t DeltaBCS(const Double_t t) const;
    virtual Double_t LambdaL_T(const Double_t lambdaL, const Double_t t) const;
    virtual Double_t XiP_T(const Double_t xi0, const Double_t meanFreePath, Double_t t) const;

  ClassDef(PNL_PippardFitterGlobal, 1)
};

class PNL_PippardFitter : public PUserFcnBase
{
  public:
    PNL_PippardFitter() {}
    virtual ~PNL_PippardFitter();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid{true};
    Bool_t fInvokedGlobal{false};
    Int_t fIdxGlobal;

    PNL_PippardFitterGlobal *fPippardFitterGlobal{nullptr};

  ClassDef(PNL_PippardFitter, 1)
};

#endif // _PNL_PIPPARDFITTER_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PNL_PippardFitter", payloadCode, "@",
"PNL_PippardFitterGlobal", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPNL_PippardFitter",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPNL_PippardFitter_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPNL_PippardFitter_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPNL_PippardFitter() {
  TriggerDictionaryInitialization_libPNL_PippardFitter_Impl();
}
