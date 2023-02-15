// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME PRgeHandlerDict
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
#include "PRgeHandler.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_PXmlRgeHandler(void *p = nullptr);
   static void *newArray_PXmlRgeHandler(Long_t size, void *p);
   static void delete_PXmlRgeHandler(void *p);
   static void deleteArray_PXmlRgeHandler(void *p);
   static void destruct_PXmlRgeHandler(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PXmlRgeHandler*)
   {
      ::PXmlRgeHandler *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PXmlRgeHandler >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PXmlRgeHandler", ::PXmlRgeHandler::Class_Version(), "", 67,
                  typeid(::PXmlRgeHandler), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PXmlRgeHandler::Dictionary, isa_proxy, 4,
                  sizeof(::PXmlRgeHandler) );
      instance.SetNew(&new_PXmlRgeHandler);
      instance.SetNewArray(&newArray_PXmlRgeHandler);
      instance.SetDelete(&delete_PXmlRgeHandler);
      instance.SetDeleteArray(&deleteArray_PXmlRgeHandler);
      instance.SetDestructor(&destruct_PXmlRgeHandler);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PXmlRgeHandler*)
   {
      return GenerateInitInstanceLocal((::PXmlRgeHandler*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PXmlRgeHandler*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_PRgeHandler(void *p = nullptr);
   static void *newArray_PRgeHandler(Long_t size, void *p);
   static void delete_PRgeHandler(void *p);
   static void deleteArray_PRgeHandler(void *p);
   static void destruct_PRgeHandler(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::PRgeHandler*)
   {
      ::PRgeHandler *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::PRgeHandler >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("PRgeHandler", ::PRgeHandler::Class_Version(), "", 107,
                  typeid(::PRgeHandler), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::PRgeHandler::Dictionary, isa_proxy, 4,
                  sizeof(::PRgeHandler) );
      instance.SetNew(&new_PRgeHandler);
      instance.SetNewArray(&newArray_PRgeHandler);
      instance.SetDelete(&delete_PRgeHandler);
      instance.SetDeleteArray(&deleteArray_PRgeHandler);
      instance.SetDestructor(&destruct_PRgeHandler);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::PRgeHandler*)
   {
      return GenerateInitInstanceLocal((::PRgeHandler*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::PRgeHandler*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr PXmlRgeHandler::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PXmlRgeHandler::Class_Name()
{
   return "PXmlRgeHandler";
}

//______________________________________________________________________________
const char *PXmlRgeHandler::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PXmlRgeHandler*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PXmlRgeHandler::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PXmlRgeHandler*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PXmlRgeHandler::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PXmlRgeHandler*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PXmlRgeHandler::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PXmlRgeHandler*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr PRgeHandler::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *PRgeHandler::Class_Name()
{
   return "PRgeHandler";
}

//______________________________________________________________________________
const char *PRgeHandler::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PRgeHandler*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int PRgeHandler::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::PRgeHandler*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *PRgeHandler::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PRgeHandler*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *PRgeHandler::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::PRgeHandler*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void PXmlRgeHandler::Streamer(TBuffer &R__b)
{
   // Stream an object of class PXmlRgeHandler.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PXmlRgeHandler::Class(),this);
   } else {
      R__b.WriteClassBuffer(PXmlRgeHandler::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PXmlRgeHandler(void *p) {
      return  p ? new(p) ::PXmlRgeHandler : new ::PXmlRgeHandler;
   }
   static void *newArray_PXmlRgeHandler(Long_t nElements, void *p) {
      return p ? new(p) ::PXmlRgeHandler[nElements] : new ::PXmlRgeHandler[nElements];
   }
   // Wrapper around operator delete
   static void delete_PXmlRgeHandler(void *p) {
      delete ((::PXmlRgeHandler*)p);
   }
   static void deleteArray_PXmlRgeHandler(void *p) {
      delete [] ((::PXmlRgeHandler*)p);
   }
   static void destruct_PXmlRgeHandler(void *p) {
      typedef ::PXmlRgeHandler current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PXmlRgeHandler

//______________________________________________________________________________
void PRgeHandler::Streamer(TBuffer &R__b)
{
   // Stream an object of class PRgeHandler.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(PRgeHandler::Class(),this);
   } else {
      R__b.WriteClassBuffer(PRgeHandler::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_PRgeHandler(void *p) {
      return  p ? new(p) ::PRgeHandler : new ::PRgeHandler;
   }
   static void *newArray_PRgeHandler(Long_t nElements, void *p) {
      return p ? new(p) ::PRgeHandler[nElements] : new ::PRgeHandler[nElements];
   }
   // Wrapper around operator delete
   static void delete_PRgeHandler(void *p) {
      delete ((::PRgeHandler*)p);
   }
   static void deleteArray_PRgeHandler(void *p) {
      delete [] ((::PRgeHandler*)p);
   }
   static void destruct_PRgeHandler(void *p) {
      typedef ::PRgeHandler current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::PRgeHandler

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
  void TriggerDictionaryInitialization_libPRgeHandler_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"-I/Users/maria/Applications/musrfit/src/include",
"/opt/homebrew/include",
"/Users/maria/Applications/musrfit/src/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/include",
"/Users/maria/Applications/musrfit/src/classes",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/classes/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libPRgeHandler dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class PXmlRgeHandler;
class PRgeHandler;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libPRgeHandler dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  PRgeHandler.h

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

#ifndef _PRGEHANDLER_H_
#define _PRGEHANDLER_H_

#include <string>
#include <vector>

#include <TObject.h>
#include <TQObject.h>
#include <TSAXParser.h>

#include "PMusr.h"

//-----------------------------------------------------------------------------
/**
 * <p>Keep a single rge table from TrimSP for a given energy.
 */
typedef struct {
  Double_t energy;
  PDoubleVector depth;
  PDoubleVector amplitude;
  PDoubleVector nn; // normalized int n(z) dz = 1 amplitudes
  Double_t noOfParticles;
} PRgeData;

//-----------------------------------------------------------------------------
/**
 * <p>Keep all rge tables from TrimSP.
 */
typedef std::vector<PRgeData> PRgeDataList;

//-----------------------------------------------------------------------------
/**
 * <p>Reads the xml-startup file in order to extract all necessary information
 * about the RGE files (TrimSP) needed.
 */
class PXmlRgeHandler : public TObject, public TQObject
{
  public:
    PXmlRgeHandler() {}
    virtual ~PXmlRgeHandler() {}

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
    virtual std::string GetTrimSpDataPath() { return fTrimSpDataPath; }
    virtual std::string GetTrimSpFlnPre() { return fTrimSpFlnPre; }
    virtual const PIntVector GetTrimSpDataVectorList() const { return fTrimSpDataEnergyList; }

  private:
    enum EKeyWords {eEmpty, eDataPath, eFlnPre, eEnergy};
    EKeyWords      fKey;

    bool isTrimSp{false};
    bool fIsValid{true};

    std::string fTrimSpDataPath{""}; //< where to find the rge-files
    std::string fTrimSpFlnPre{""};   //< keeps the preface of the rge file name, e.g. LCCO_E
    PIntVector  fTrimSpDataEnergyList; //< TrimSP implantation energy list (in eV)

  ClassDef(PXmlRgeHandler, 1)
};

//-----------------------------------------------------------------------------
/**
 * @brief The PRegHandler class
 */
class PRgeHandler : public TObject
{
  public:
    PRgeHandler(std::string fln="");
    virtual ~PRgeHandler() {}

    virtual bool IsValid() { return fValid; }
    virtual UInt_t GetNoOfRgeDataSets() { return (UInt_t)fData.size(); }
    virtual PRgeDataList GetRgeData() { return fData; }
    virtual Double_t GetZmax(const Double_t energy);
    virtual Double_t GetZmax(const Int_t idx);
    virtual Double_t Get_n(const Double_t energy, const Double_t z);
    virtual Double_t Get_n(const Int_t idx, const Double_t z);
    virtual Int_t GetEnergyIndex(const Double_t energy);

  private:
    bool fValid{false};
    PRgeDataList fData;

    virtual bool ReadRgeFile(const std::string fln, PRgeData &data);

  ClassDef(PRgeHandler, 1)
};

#endif // _PRGEHANDLER_H_

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"PRgeHandler", payloadCode, "@",
"PXmlRgeHandler", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libPRgeHandler",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libPRgeHandler_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libPRgeHandler_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libPRgeHandler() {
  TriggerDictionaryInitialization_libPRgeHandler_Impl();
}
