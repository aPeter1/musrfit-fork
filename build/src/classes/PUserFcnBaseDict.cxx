// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PUserFcnBaseDict
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
#include "PUserFcnBase.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void delete_PUserFcnBase(void *p);
   static void deleteArray_PUserFcnBase(void *p);
   static void destruct_PUserFcnBase(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PUserFcnBase*)
   {
      ::PUserFcnBase *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PUserFcnBase >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PUserFcnBase", ::PUserFcnBase::Class_Version(), "", 44,
                  typeid(::PUserFcnBase), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PUserFcnBase::Dictionary, isa_proxy, 4,
                  sizeof(::PUserFcnBase) );
      instance.SetDelete(&delete_PUserFcnBase);
      instance.SetDeleteArray(&deleteArray_PUserFcnBase);
      instance.SetDestructor(&destruct_PUserFcnBase);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PUserFcnBase*)
   {
      return GenerateInitInstanceLocal((::PUserFcnBase*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PUserFcnBase*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PUserFcnBase::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PUserFcnBase::Class_Name()
{
   return "PUserFcnBase";
}

//______________________________________________________________________________
const char *PUserFcnBase::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PUserFcnBase*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PUserFcnBase::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PUserFcnBase*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PUserFcnBase::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PUserFcnBase*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PUserFcnBase::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PUserFcnBase*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PUserFcnBase::Streamer(TBuffer &R__b)
{
   // Stream an object of class PUserFcnBase.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PUserFcnBase::Class(),this);
   } else {
      R__b.WriteClassBuffer(PUserFcnBase::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_PUserFcnBase(void *p) {
      delete ((::PUserFcnBase*)p);
   }
   static void deleteArray_PUserFcnBase(void *p) {
      delete [] ((::PUserFcnBase*)p);
   }
   static void destruct_PUserFcnBase(void *p) {
      typedef ::PUserFcnBase current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PUserFcnBase

namespace {
  void TriggerDictionaryInitialization_libPUserFcnBase_Impl() {
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
#line 1 "libPUserFcnBase dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PUserFcnBase;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPUserFcnBase dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PUserFcnBase.h

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

#ifndef _PUSERFCNBASE_H_
#define _PUSERFCNBASE_H_

#include <vector>

#include "TObject.h"
#include "TSAXParser.h"

//--------------------------------------------------------------------------------------------
/**
 * <p>Interface class for the user function.
 */
class PUserFcnBase : public TObject
{
  public:
    PUserFcnBase() {}
    virtual ~PUserFcnBase() {}

    virtual Bool_t NeedGlobalPart() const { return false; } ///< if a user function needs a global part this function should return true, otherwise false (default: false)
    virtual void SetGlobalPart(std::vector<void *> &globalPart, UInt_t idx) {} ///< if a user function is using a global part, this function is used to invoke and retrieve the proper global object
    virtual Bool_t GlobalPartIsValid() const { return false; } ///< if a user function is using a global part, this function returns if the global object part is valid (default: false)

    virtual Double_t operator()(Double_t t, const std::vector<Double_t> &param) const = 0;

  ClassDef(PUserFcnBase, 1)
};

//--------------------------------------------------------------------------
// This function is a replacement for the ParseFile method of TSAXParser.
//--------------------------------------------------------------------------
Int_t parseXmlFile(TSAXParser*, const Char_t*);

#endif // _PUSERFCNBASE_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PUserFcnBase", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPUserFcnBase",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPUserFcnBase_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPUserFcnBase_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPUserFcnBase() {
  TriggerDictionaryInitialization_libPUserFcnBase_Impl();
}
