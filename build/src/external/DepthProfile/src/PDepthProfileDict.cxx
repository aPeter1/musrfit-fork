// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PDepthProfileDict
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
#include "PDepthProfile.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PDepthProfileGlobal(void *p = nullptr);
   static void *newArray_PDepthProfileGlobal(Long_t size, void *p);
   static void delete_PDepthProfileGlobal(void *p);
   static void deleteArray_PDepthProfileGlobal(void *p);
   static void destruct_PDepthProfileGlobal(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PDepthProfileGlobal*)
   {
      ::PDepthProfileGlobal *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PDepthProfileGlobal >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PDepthProfileGlobal", ::PDepthProfileGlobal::Class_Version(), "", 40,
                  typeid(::PDepthProfileGlobal), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PDepthProfileGlobal::Dictionary, isa_proxy, 4,
                  sizeof(::PDepthProfileGlobal) );
      instance.SetNew(&new_PDepthProfileGlobal);
      instance.SetNewArray(&newArray_PDepthProfileGlobal);
      instance.SetDelete(&delete_PDepthProfileGlobal);
      instance.SetDeleteArray(&deleteArray_PDepthProfileGlobal);
      instance.SetDestructor(&destruct_PDepthProfileGlobal);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PDepthProfileGlobal*)
   {
      return GenerateInitInstanceLocal((::PDepthProfileGlobal*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PDepthProfileGlobal*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_PDepthProfile(void *p = nullptr);
   static void *newArray_PDepthProfile(Long_t size, void *p);
   static void delete_PDepthProfile(void *p);
   static void deleteArray_PDepthProfile(void *p);
   static void destruct_PDepthProfile(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PDepthProfile*)
   {
      ::PDepthProfile *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PDepthProfile >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PDepthProfile", ::PDepthProfile::Class_Version(), "", 66,
                  typeid(::PDepthProfile), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PDepthProfile::Dictionary, isa_proxy, 4,
                  sizeof(::PDepthProfile) );
      instance.SetNew(&new_PDepthProfile);
      instance.SetNewArray(&newArray_PDepthProfile);
      instance.SetDelete(&delete_PDepthProfile);
      instance.SetDeleteArray(&deleteArray_PDepthProfile);
      instance.SetDestructor(&destruct_PDepthProfile);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PDepthProfile*)
   {
      return GenerateInitInstanceLocal((::PDepthProfile*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PDepthProfile*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PDepthProfileGlobal::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PDepthProfileGlobal::Class_Name()
{
   return "PDepthProfileGlobal";
}

//______________________________________________________________________________
const char *PDepthProfileGlobal::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfileGlobal*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PDepthProfileGlobal::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfileGlobal*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PDepthProfileGlobal::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfileGlobal*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PDepthProfileGlobal::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfileGlobal*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr PDepthProfile::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PDepthProfile::Class_Name()
{
   return "PDepthProfile";
}

//______________________________________________________________________________
const char *PDepthProfile::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfile*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PDepthProfile::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfile*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PDepthProfile::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfile*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PDepthProfile::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PDepthProfile*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PDepthProfileGlobal::Streamer(TBuffer &R__b)
{
   // Stream an object of class PDepthProfileGlobal.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PDepthProfileGlobal::Class(),this);
   } else {
      R__b.WriteClassBuffer(PDepthProfileGlobal::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PDepthProfileGlobal(void *p) {
      return  p ? new(p) ::PDepthProfileGlobal : new ::PDepthProfileGlobal;
   }
   static void *newArray_PDepthProfileGlobal(Long_t nElements, void *p) {
      return p ? new(p) ::PDepthProfileGlobal[nElements] : new ::PDepthProfileGlobal[nElements];
   }
   // Wrapper around operator delete
   static void delete_PDepthProfileGlobal(void *p) {
      delete ((::PDepthProfileGlobal*)p);
   }
   static void deleteArray_PDepthProfileGlobal(void *p) {
      delete [] ((::PDepthProfileGlobal*)p);
   }
   static void destruct_PDepthProfileGlobal(void *p) {
      typedef ::PDepthProfileGlobal current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PDepthProfileGlobal

//______________________________________________________________________________
void PDepthProfile::Streamer(TBuffer &R__b)
{
   // Stream an object of class PDepthProfile.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PDepthProfile::Class(),this);
   } else {
      R__b.WriteClassBuffer(PDepthProfile::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PDepthProfile(void *p) {
      return  p ? new(p) ::PDepthProfile : new ::PDepthProfile;
   }
   static void *newArray_PDepthProfile(Long_t nElements, void *p) {
      return p ? new(p) ::PDepthProfile[nElements] : new ::PDepthProfile[nElements];
   }
   // Wrapper around operator delete
   static void delete_PDepthProfile(void *p) {
      delete ((::PDepthProfile*)p);
   }
   static void deleteArray_PDepthProfile(void *p) {
      delete [] ((::PDepthProfile*)p);
   }
   static void destruct_PDepthProfile(void *p) {
      typedef ::PDepthProfile current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PDepthProfile

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
   static TClass *vectorlEPRgeDatagR_Dictionary();
   static void vectorlEPRgeDatagR_TClassManip(TClass*);
   static void *new_vectorlEPRgeDatagR(void *p = nullptr);
   static void *newArray_vectorlEPRgeDatagR(Long_t size, void *p);
   static void delete_vectorlEPRgeDatagR(void *p);
   static void deleteArray_vectorlEPRgeDatagR(void *p);
   static void destruct_vectorlEPRgeDatagR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<PRgeData>*)
   {
      vector<PRgeData> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<PRgeData>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<PRgeData>", -2, "vector", 493,
                  typeid(vector<PRgeData>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEPRgeDatagR_Dictionary, isa_proxy, 0,
                  sizeof(vector<PRgeData>) );
      instance.SetNew(&new_vectorlEPRgeDatagR);
      instance.SetNewArray(&newArray_vectorlEPRgeDatagR);
      instance.SetDelete(&delete_vectorlEPRgeDatagR);
      instance.SetDeleteArray(&deleteArray_vectorlEPRgeDatagR);
      instance.SetDestructor(&destruct_vectorlEPRgeDatagR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<PRgeData> >()));

      ::ROOT::AddClassAlternate("vector<PRgeData>","std::__1::vector<PRgeData, std::__1::allocator<PRgeData> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<PRgeData>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEPRgeDatagR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<PRgeData>*)nullptr)->GetClass();
      vectorlEPRgeDatagR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEPRgeDatagR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEPRgeDatagR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PRgeData> : new vector<PRgeData>;
   }
   static void *newArray_vectorlEPRgeDatagR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<PRgeData>[nElements] : new vector<PRgeData>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEPRgeDatagR(void *p) {
      delete ((vector<PRgeData>*)p);
   }
   static void deleteArray_vectorlEPRgeDatagR(void *p) {
      delete [] ((vector<PRgeData>*)p);
   }
   static void destruct_vectorlEPRgeDatagR(void *p) {
      typedef vector<PRgeData> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<PRgeData>

namespace {
  void TriggerDictionaryInitialization_libPDepthProfile_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/opt/homebrew/include",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/external/DepthProfile/src/../inc",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/DepthProfile/src/../inc",
"/Users/maria/Applications/musrfit/src/external/DepthProfile/src",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/DepthProfile/src/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPDepthProfile dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PDepthProfileGlobal;
class PDepthProfile;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPDepthProfile dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PDepthProfile.h

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2013-2023 by Andreas Suter                              *
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

#ifndef _PDEPTHPROFILE_H_
#define _PDEPTHPROFILE_H_

#include <vector>

#include "PUserFcnBase.h"
#include "PRgeHandler.h"

class PDepthProfileGlobal
{
  public:
    PDepthProfileGlobal();
    virtual ~PDepthProfileGlobal();

    Bool_t IsValid() { return fValid; }
    virtual Int_t GetEnergyIndex(const Double_t energy) { return fRgeHandler->GetEnergyIndex(energy); }
    virtual Double_t GetMuonStoppingDensity(const Int_t energyIndex, const Double_t z) const { return fRgeHandler->Get_n(energyIndex, z); }
    virtual Double_t GetStoppingProbability(double a, double b, Double_t energy) const;
    virtual Double_t GetZmax(const Double_t energy);

  private:
    Bool_t fValid{true};
    

    mutable std::vector<Double_t> fPreviousParam;

    PRgeHandler *fRgeHandler{nullptr};
    PRgeDataList fCfd;

    Int_t GetPosIdx(const double z, const Int_t energyIdx) const;

  ClassDef(PDepthProfileGlobal, 1)
};

class PDepthProfile : public PUserFcnBase
{
  public:
    PDepthProfile() {}
    virtual ~PDepthProfile();

    virtual Bool_t NeedGlobalPart() const { return true; }
    virtual void SetGlobalPart(std::vector<void*> &globalPart, UInt_t idx);
    virtual Bool_t GlobalPartIsValid() const;

    virtual Double_t operator()(Double_t energy, const std::vector<Double_t> &param) const;

  private:
    Bool_t fValid{true};
    Bool_t fInvokedGlobal{false};
    Int_t fIdxGlobal;

    mutable std::vector<Double_t> fPreviousParam;
    PDepthProfileGlobal *fDepthProfileGlobal{nullptr};

  // definition of the class for the ROOT dictionary
  ClassDef(PDepthProfile, 1)
};

#endif // _PDEPTHPROFILE_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PDepthProfile", payloadCode, "@",
"PDepthProfileGlobal", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPDepthProfile",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPDepthProfile_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPDepthProfile_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPDepthProfile() {
  TriggerDictionaryInitialization_libPDepthProfile_Impl();
}
