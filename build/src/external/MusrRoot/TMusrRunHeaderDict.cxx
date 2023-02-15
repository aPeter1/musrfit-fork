// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME TMusrRunHeaderDict
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
#include "TMusrRunHeader.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static void *new_TMusrRunPhysicalQuantity(void *p = nullptr);
   static void *newArray_TMusrRunPhysicalQuantity(Long_t size, void *p);
   static void delete_TMusrRunPhysicalQuantity(void *p);
   static void deleteArray_TMusrRunPhysicalQuantity(void *p);
   static void destruct_TMusrRunPhysicalQuantity(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TMusrRunPhysicalQuantity*)
   {
      ::TMusrRunPhysicalQuantity *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TMusrRunPhysicalQuantity >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("TMusrRunPhysicalQuantity", ::TMusrRunPhysicalQuantity::Class_Version(), "", 83,
                  typeid(::TMusrRunPhysicalQuantity), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TMusrRunPhysicalQuantity::Dictionary, isa_proxy, 4,
                  sizeof(::TMusrRunPhysicalQuantity) );
      instance.SetNew(&new_TMusrRunPhysicalQuantity);
      instance.SetNewArray(&newArray_TMusrRunPhysicalQuantity);
      instance.SetDelete(&delete_TMusrRunPhysicalQuantity);
      instance.SetDeleteArray(&deleteArray_TMusrRunPhysicalQuantity);
      instance.SetDestructor(&destruct_TMusrRunPhysicalQuantity);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TMusrRunPhysicalQuantity*)
   {
      return GenerateInitInstanceLocal((::TMusrRunPhysicalQuantity*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TMusrRunPhysicalQuantity*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TMusrRunHeader(void *p = nullptr);
   static void *newArray_TMusrRunHeader(Long_t size, void *p);
   static void delete_TMusrRunHeader(void *p);
   static void deleteArray_TMusrRunHeader(void *p);
   static void destruct_TMusrRunHeader(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TMusrRunHeader*)
   {
      ::TMusrRunHeader *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TMusrRunHeader >(nullptr);
      static ::ROOT::TGenericClassInfo 
         instance("TMusrRunHeader", ::TMusrRunHeader::Class_Version(), "", 124,
                  typeid(::TMusrRunHeader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TMusrRunHeader::Dictionary, isa_proxy, 4,
                  sizeof(::TMusrRunHeader) );
      instance.SetNew(&new_TMusrRunHeader);
      instance.SetNewArray(&newArray_TMusrRunHeader);
      instance.SetDelete(&delete_TMusrRunHeader);
      instance.SetDeleteArray(&deleteArray_TMusrRunHeader);
      instance.SetDestructor(&destruct_TMusrRunHeader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TMusrRunHeader*)
   {
      return GenerateInitInstanceLocal((::TMusrRunHeader*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TMusrRunHeader*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TMusrRunPhysicalQuantity::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *TMusrRunPhysicalQuantity::Class_Name()
{
   return "TMusrRunPhysicalQuantity";
}

//______________________________________________________________________________
const char *TMusrRunPhysicalQuantity::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunPhysicalQuantity*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int TMusrRunPhysicalQuantity::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunPhysicalQuantity*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TMusrRunPhysicalQuantity::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunPhysicalQuantity*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TMusrRunPhysicalQuantity::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunPhysicalQuantity*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TMusrRunHeader::fgIsA(nullptr);  // static to hold class pointer

//______________________________________________________________________________
const char *TMusrRunHeader::Class_Name()
{
   return "TMusrRunHeader";
}

//______________________________________________________________________________
const char *TMusrRunHeader::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunHeader*)nullptr)->GetImplFileName();
}

//______________________________________________________________________________
int TMusrRunHeader::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunHeader*)nullptr)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TMusrRunHeader::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunHeader*)nullptr)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TMusrRunHeader::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TMusrRunHeader*)nullptr)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TMusrRunPhysicalQuantity::Streamer(TBuffer &R__b)
{
   // Stream an object of class TMusrRunPhysicalQuantity.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TMusrRunPhysicalQuantity::Class(),this);
   } else {
      R__b.WriteClassBuffer(TMusrRunPhysicalQuantity::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TMusrRunPhysicalQuantity(void *p) {
      return  p ? new(p) ::TMusrRunPhysicalQuantity : new ::TMusrRunPhysicalQuantity;
   }
   static void *newArray_TMusrRunPhysicalQuantity(Long_t nElements, void *p) {
      return p ? new(p) ::TMusrRunPhysicalQuantity[nElements] : new ::TMusrRunPhysicalQuantity[nElements];
   }
   // Wrapper around operator delete
   static void delete_TMusrRunPhysicalQuantity(void *p) {
      delete ((::TMusrRunPhysicalQuantity*)p);
   }
   static void deleteArray_TMusrRunPhysicalQuantity(void *p) {
      delete [] ((::TMusrRunPhysicalQuantity*)p);
   }
   static void destruct_TMusrRunPhysicalQuantity(void *p) {
      typedef ::TMusrRunPhysicalQuantity current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TMusrRunPhysicalQuantity

//______________________________________________________________________________
void TMusrRunHeader::Streamer(TBuffer &R__b)
{
   // Stream an object of class TMusrRunHeader.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TMusrRunHeader::Class(),this);
   } else {
      R__b.WriteClassBuffer(TMusrRunHeader::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TMusrRunHeader(void *p) {
      return  p ? new(p) ::TMusrRunHeader : new ::TMusrRunHeader;
   }
   static void *newArray_TMusrRunHeader(Long_t nElements, void *p) {
      return p ? new(p) ::TMusrRunHeader[nElements] : new ::TMusrRunHeader[nElements];
   }
   // Wrapper around operator delete
   static void delete_TMusrRunHeader(void *p) {
      delete ((::TMusrRunHeader*)p);
   }
   static void deleteArray_TMusrRunHeader(void *p) {
      delete [] ((::TMusrRunHeader*)p);
   }
   static void destruct_TMusrRunHeader(void *p) {
      typedef ::TMusrRunHeader current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TMusrRunHeader

namespace ROOT {
   static TClass *vectorlETStringgR_Dictionary();
   static void vectorlETStringgR_TClassManip(TClass*);
   static void *new_vectorlETStringgR(void *p = nullptr);
   static void *newArray_vectorlETStringgR(Long_t size, void *p);
   static void delete_vectorlETStringgR(void *p);
   static void deleteArray_vectorlETStringgR(void *p);
   static void destruct_vectorlETStringgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TString>*)
   {
      vector<TString> *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TString>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TString>", -2, "vector", 493,
                  typeid(vector<TString>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETStringgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TString>) );
      instance.SetNew(&new_vectorlETStringgR);
      instance.SetNewArray(&newArray_vectorlETStringgR);
      instance.SetDelete(&delete_vectorlETStringgR);
      instance.SetDeleteArray(&deleteArray_vectorlETStringgR);
      instance.SetDestructor(&destruct_vectorlETStringgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TString> >()));

      ::ROOT::AddClassAlternate("vector<TString>","std::__1::vector<TString, std::__1::allocator<TString> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TString>*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETStringgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TString>*)nullptr)->GetClass();
      vectorlETStringgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETStringgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETStringgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TString> : new vector<TString>;
   }
   static void *newArray_vectorlETStringgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TString>[nElements] : new vector<TString>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETStringgR(void *p) {
      delete ((vector<TString>*)p);
   }
   static void deleteArray_vectorlETStringgR(void *p) {
      delete [] ((vector<TString>*)p);
   }
   static void destruct_vectorlETStringgR(void *p) {
      typedef vector<TString> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TString>

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<vector<int> > >*)
   {
      vector<TMusrRunObject<vector<int> > > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<vector<int> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<vector<int> > >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<vector<int> > >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<vector<int> > >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<vector<int> > > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<vector<int> > >","std::__1::vector<TMusrRunObject<std::__1::vector<int, std::__1::allocator<int> > >, std::__1::allocator<TMusrRunObject<std::__1::vector<int, std::__1::allocator<int> > > > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<vector<int> > >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<vector<int> > >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<vector<int> > > : new vector<TMusrRunObject<vector<int> > >;
   }
   static void *newArray_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<vector<int> > >[nElements] : new vector<TMusrRunObject<vector<int> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p) {
      delete ((vector<TMusrRunObject<vector<int> > >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<vector<int> > >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlEvectorlEintgRsPgRsPgR(void *p) {
      typedef vector<TMusrRunObject<vector<int> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<vector<int> > >

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<vector<double> > >*)
   {
      vector<TMusrRunObject<vector<double> > > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<vector<double> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<vector<double> > >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<vector<double> > >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<vector<double> > >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<vector<double> > > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<vector<double> > >","std::__1::vector<TMusrRunObject<std::__1::vector<double, std::__1::allocator<double> > >, std::__1::allocator<TMusrRunObject<std::__1::vector<double, std::__1::allocator<double> > > > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<vector<double> > >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<vector<double> > >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<vector<double> > > : new vector<TMusrRunObject<vector<double> > >;
   }
   static void *newArray_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<vector<double> > >[nElements] : new vector<TMusrRunObject<vector<double> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p) {
      delete ((vector<TMusrRunObject<vector<double> > >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<vector<double> > >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlEvectorlEdoublegRsPgRsPgR(void *p) {
      typedef vector<TMusrRunObject<vector<double> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<vector<double> > >

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<vector<TString> > >*)
   {
      vector<TMusrRunObject<vector<TString> > > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<vector<TString> > >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<vector<TString> > >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<vector<TString> > >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<vector<TString> > >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<vector<TString> > > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<vector<TString> > >","std::__1::vector<TMusrRunObject<std::__1::vector<TString, std::__1::allocator<TString> > >, std::__1::allocator<TMusrRunObject<std::__1::vector<TString, std::__1::allocator<TString> > > > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<vector<TString> > >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<vector<TString> > >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<vector<TString> > > : new vector<TMusrRunObject<vector<TString> > >;
   }
   static void *newArray_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<vector<TString> > >[nElements] : new vector<TMusrRunObject<vector<TString> > >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p) {
      delete ((vector<TMusrRunObject<vector<TString> > >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<vector<TString> > >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlEvectorlETStringgRsPgRsPgR(void *p) {
      typedef vector<TMusrRunObject<vector<TString> > > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<vector<TString> > >

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlEintgRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlEintgRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlEintgRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlEintgRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlEintgRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlEintgRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlEintgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<int> >*)
   {
      vector<TMusrRunObject<int> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<int> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<int> >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<int> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlEintgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<int> >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlEintgRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlEintgRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlEintgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlEintgRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlEintgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<int> > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<int> >","std::__1::vector<TMusrRunObject<int>, std::__1::allocator<TMusrRunObject<int> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<int> >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlEintgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<int> >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlEintgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlEintgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlEintgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<int> > : new vector<TMusrRunObject<int> >;
   }
   static void *newArray_vectorlETMusrRunObjectlEintgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<int> >[nElements] : new vector<TMusrRunObject<int> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlEintgRsPgR(void *p) {
      delete ((vector<TMusrRunObject<int> >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlEintgRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<int> >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlEintgRsPgR(void *p) {
      typedef vector<TMusrRunObject<int> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<int> >

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlEdoublegRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlEdoublegRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlEdoublegRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlEdoublegRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlEdoublegRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlEdoublegRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlEdoublegRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<double> >*)
   {
      vector<TMusrRunObject<double> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<double> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<double> >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<double> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlEdoublegRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<double> >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlEdoublegRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlEdoublegRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlEdoublegRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlEdoublegRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlEdoublegRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<double> > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<double> >","std::__1::vector<TMusrRunObject<double>, std::__1::allocator<TMusrRunObject<double> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<double> >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlEdoublegRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<double> >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlEdoublegRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlEdoublegRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlEdoublegRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<double> > : new vector<TMusrRunObject<double> >;
   }
   static void *newArray_vectorlETMusrRunObjectlEdoublegRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<double> >[nElements] : new vector<TMusrRunObject<double> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlEdoublegRsPgR(void *p) {
      delete ((vector<TMusrRunObject<double> >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlEdoublegRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<double> >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlEdoublegRsPgR(void *p) {
      typedef vector<TMusrRunObject<double> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<double> >

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlETStringgRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlETStringgRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlETStringgRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlETStringgRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlETStringgRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlETStringgRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlETStringgRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<TString> >*)
   {
      vector<TMusrRunObject<TString> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<TString> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<TString> >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<TString> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlETStringgRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<TString> >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlETStringgRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlETStringgRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlETStringgRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlETStringgRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlETStringgRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<TString> > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<TString> >","std::__1::vector<TMusrRunObject<TString>, std::__1::allocator<TMusrRunObject<TString> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<TString> >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlETStringgRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<TString> >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlETStringgRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlETStringgRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlETStringgRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<TString> > : new vector<TMusrRunObject<TString> >;
   }
   static void *newArray_vectorlETMusrRunObjectlETStringgRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<TString> >[nElements] : new vector<TMusrRunObject<TString> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlETStringgRsPgR(void *p) {
      delete ((vector<TMusrRunObject<TString> >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlETStringgRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<TString> >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlETStringgRsPgR(void *p) {
      typedef vector<TMusrRunObject<TString> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<TString> >

namespace ROOT {
   static TClass *vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR_Dictionary();
   static void vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR_TClassManip(TClass*);
   static void *new_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p = nullptr);
   static void *newArray_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(Long_t size, void *p);
   static void delete_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p);
   static void deleteArray_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p);
   static void destruct_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<TMusrRunObject<TMusrRunPhysicalQuantity> >*)
   {
      vector<TMusrRunObject<TMusrRunPhysicalQuantity> > *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<TMusrRunObject<TMusrRunPhysicalQuantity> >));
      static ::ROOT::TGenericClassInfo 
         instance("vector<TMusrRunObject<TMusrRunPhysicalQuantity> >", -2, "vector", 493,
                  typeid(vector<TMusrRunObject<TMusrRunPhysicalQuantity> >), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<TMusrRunObject<TMusrRunPhysicalQuantity> >) );
      instance.SetNew(&new_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR);
      instance.SetNewArray(&newArray_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR);
      instance.SetDelete(&delete_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR);
      instance.SetDeleteArray(&deleteArray_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR);
      instance.SetDestructor(&destruct_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<TMusrRunObject<TMusrRunPhysicalQuantity> > >()));

      ::ROOT::AddClassAlternate("vector<TMusrRunObject<TMusrRunPhysicalQuantity> >","std::__1::vector<TMusrRunObject<TMusrRunPhysicalQuantity>, std::__1::allocator<TMusrRunObject<TMusrRunPhysicalQuantity> > >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<TMusrRunObject<TMusrRunPhysicalQuantity> >*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<TMusrRunObject<TMusrRunPhysicalQuantity> >*)nullptr)->GetClass();
      vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<TMusrRunPhysicalQuantity> > : new vector<TMusrRunObject<TMusrRunPhysicalQuantity> >;
   }
   static void *newArray_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<TMusrRunObject<TMusrRunPhysicalQuantity> >[nElements] : new vector<TMusrRunObject<TMusrRunPhysicalQuantity> >[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p) {
      delete ((vector<TMusrRunObject<TMusrRunPhysicalQuantity> >*)p);
   }
   static void deleteArray_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p) {
      delete [] ((vector<TMusrRunObject<TMusrRunPhysicalQuantity> >*)p);
   }
   static void destruct_vectorlETMusrRunObjectlETMusrRunPhysicalQuantitygRsPgR(void *p) {
      typedef vector<TMusrRunObject<TMusrRunPhysicalQuantity> > current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<TMusrRunObject<TMusrRunPhysicalQuantity> >

namespace {
  void TriggerDictionaryInitialization_libTMusrRunHeader_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
"/Users/maria/Applications/musrfit/src/external/MusrRoot",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/src/external/MusrRoot",
"/opt/homebrew/Cellar/root/6.26.06_2/include/root",
"/Users/maria/Applications/musrfit/build/src/external/MusrRoot/",
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "libTMusrRunHeader dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class TMusrRunPhysicalQuantity;
class TMusrRunHeader;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "libTMusrRunHeader dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
/***************************************************************************

  TMusrRunHeader.h

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

#ifndef TMUSRRUNHEADER_H
#define TMUSRRUNHEADER_H

#include <vector>

#include <TDatime.h>
#include <TObject.h>
#include <TQObject.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TFolder.h>

#define MRH_UNDEFINED -9.99e99

#define MRH_DOUBLE_PREC 6

#define MRH_TSTRING                     0
#define MRH_INT                         1
#define MRH_DOUBLE                      2
#define MRH_TMUSR_RUN_PHYSICAL_QUANTITY 3
#define MRH_TSTRING_VECTOR              4
#define MRH_INT_VECTOR                  5
#define MRH_DOUBLE_VECTOR               6

typedef std::vector<Int_t> TIntVector;
typedef std::vector<Double_t> TDoubleVector;
typedef std::vector<TString> TStringVector;

//-------------------------------------------------------------------------
template <class T> class TMusrRunObject : public TObject
{
public:
  TMusrRunObject() : TObject() { fPathName = "n/a"; fType = "n/a"; }
  TMusrRunObject(TString pathName, TString type, T value) : TObject(), fPathName(pathName), fType(type), fValue(value) {}
  virtual ~TMusrRunObject() {}

  virtual TString GetPathName() { return fPathName; }
  virtual TString GetType() { return fType; }
  virtual T GetValue() { return fValue; }

  virtual void SetPathName(TString pathName) { fPathName = pathName; }
  virtual void SetType(TString type) { fType = type; }
  virtual void SetValue(T value) { fValue = value; }

private:
  TString fPathName; ///< path name of the variable, e.g. 'RunInfo/Run Number'
  TString fType;     ///< type of value, e.g. TString, or Int_t, etc.
  T fValue;          ///< value itself
};

//-------------------------------------------------------------------------
class TMusrRunPhysicalQuantity : public TObject
{
public:
  TMusrRunPhysicalQuantity();
  TMusrRunPhysicalQuantity(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description = TString("n/a"));
  TMusrRunPhysicalQuantity(TString label, Double_t demand, Double_t value, TString unit, TString description = TString("n/a"));
  TMusrRunPhysicalQuantity(TString label, Double_t value, TString unit, TString description = TString("n/a"));
  virtual ~TMusrRunPhysicalQuantity() {}

  virtual TString  GetLabel() const { return fLabel; }
  virtual Double_t GetDemand() const { return fDemand; }
  virtual Double_t GetValue() const { return fValue; }
  virtual Double_t GetError() const { return fError; }
  virtual TString  GetUnit() const { return fUnit; }
  virtual TString  GetDescription() const { return fDescription; }

  virtual void Set(TString label, Double_t demand, Double_t value, Double_t error, TString unit, TString description = TString("n/a"));
  virtual void Set(TString label, Double_t demand, Double_t value, TString unit, TString description = TString("n/a"));
  virtual void Set(TString label, Double_t value, TString unit, TString description = TString("n/a"));
  virtual void SetLabel(TString &label) { fLabel = label; }
  virtual void SetLabel(const char *label) { fLabel = label; }
  virtual void SetDemand(Double_t val) { fDemand = val; }
  virtual void SetValue(Double_t val) { fValue = val; }
  virtual void SetError(Double_t err) { fError = err; }
  virtual void SetUnit(TString &unit) { fUnit = unit; }
  virtual void SetUnit(const char *unit) { fUnit = unit; }
  virtual void SetDescription(TString &str) { fDescription = str; }
  virtual void SetDescription(const char *str) { fDescription = str; }

private:
  TString  fLabel; ///<  property label, like ’Sample Temperature’ etc.
  Double_t fDemand; ///< demand value of the physical quantity, e.g. temperature setpoint
  Double_t fValue; ///<  measured value of the physical quantity
  Double_t fError; ///<  estimated error (standard deviation) of the measured value
  TString  fUnit;   ///< unit of the physical quantity
  TString  fDescription; ///< a more detailed description of the physical quantity

  ClassDef(TMusrRunPhysicalQuantity, 1)
};

//-------------------------------------------------------------------------
class TMusrRunHeader : public TObject
{
public:
  TMusrRunHeader(bool quiet=false);
  TMusrRunHeader(const char *fileName, bool quiet=false);
  virtual ~TMusrRunHeader();

  virtual TString GetFileName() { return fFileName; }

  virtual Bool_t FillFolder(TFolder *folder);

  virtual Bool_t ExtractAll(TFolder *folder);
  virtual Bool_t ExtractHeaderInformation(TObjArray *headerInfo, TString path);

  virtual TString GetTypeOfPath(TString pathName);

  virtual void Get(TString pathName, TString &value, Bool_t &ok);
  virtual void Get(TString pathName, Int_t &value, Bool_t &ok);
  virtual void Get(TString pathName, Double_t &value, Bool_t &ok);
  virtual void Get(TString pathName, TMusrRunPhysicalQuantity &value, Bool_t &ok);
  virtual void Get(TString pathName, TStringVector &value, Bool_t &ok);
  virtual void Get(TString pathName, TIntVector &value, Bool_t &ok);
  virtual void Get(TString pathName, TDoubleVector &value, Bool_t &ok);

  virtual void SetFileName(TString fln) { fFileName = fln; }

  virtual void Set(TString pathName, TString value);
  virtual void Set(TString pathName, Int_t value);
  virtual void Set(TString pathName, Double_t value);
  virtual void Set(TString pathName, TMusrRunPhysicalQuantity value);
  virtual void Set(TString pathName, TStringVector value);
  virtual void Set(TString pathName, TIntVector value);
  virtual void Set(TString pathName, TDoubleVector value);

  virtual void DumpHeader();
  virtual void DrawHeader();

private:
  bool fQuiet;
  TString fFileName;
  TString fVersion;

  std::vector< TMusrRunObject<TString> >  fStringObj;
  std::vector< TMusrRunObject<Int_t> >    fIntObj;
  std::vector< TMusrRunObject<Double_t> > fDoubleObj;
  std::vector< TMusrRunObject<TMusrRunPhysicalQuantity> > fMusrRunPhysQuantityObj;
  std::vector< TMusrRunObject<TStringVector> > fStringVectorObj;
  std::vector< TMusrRunObject<TIntVector> > fIntVectorObj;
  std::vector< TMusrRunObject<TDoubleVector> > fDoubleVectorObj;

  std::vector< TString > fPathNameOrder; ///< keeps the path-name as they were created in ordered to keep ordering

  virtual void Init(TString str="n/a");
  virtual void CleanUp();

  virtual UInt_t GetDecimalPlace(Double_t val);
  virtual UInt_t GetLeastSignificantDigit(Double_t val) const;
  virtual void SplitPathName(TString pathName, TString &path, TString &name);

  virtual TString GetLabel(TString str);
  virtual TString GetStrValue(TString str);
  virtual TString GetType(TString str);

  virtual bool UpdateFolder(TObject *treeObj, TString path);
  virtual TObject* FindObject(TObject *treeObj, TString path);
  virtual TObjString GetHeaderString(UInt_t idx);

  virtual bool RemoveFirst(TString &str, const char splitter);
  virtual TString GetFirst(TString &str, const char splitter);

  ClassDef(TMusrRunHeader, 1)
};

#endif // TMUSRRUNHEADER_H

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"TMusrRunHeader", payloadCode, "@",
"TMusrRunPhysicalQuantity", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("libTMusrRunHeader",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_libTMusrRunHeader_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_libTMusrRunHeader_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_libTMusrRunHeader() {
  TriggerDictionaryInitialization_libTMusrRunHeader_Impl();
}
