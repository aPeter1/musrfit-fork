/**************************************************************************/

/***************************************************************************
 *   Copyright (C) 2017 by Jonas A. Krieger                                *
 *   jonas.krieger@psi.ch                                                  *
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

#include "LineProfile.h"
#include <iostream> //for testing purposes

//Implement helperfunctions

Double_t GaussianShape(Double_t x, Double_t position, Double_t width) {
    if(!width){ // width=0
        if(x==position) return 1.0;
        return 0.0;}
    return exp(-2.7725887222397811*pow((x-position)/width,2));
}

Double_t LaplacianShape(Double_t x, Double_t position, Double_t width) {
    if(!width){ // width=0
        if(x==position) return 1.0;
        return 0.0;}
    return exp(-1.3862943611198906*abs((x-position)/width));
}

Double_t LorentzianShape(Double_t x, Double_t position, Double_t width) {
    if(!width){ // width=0
        if(x==position) return 1.0;
        return 0.0;}

   // return 2/PI*(width/(4*pow(x-position,2)+pow(width,2)));//constant Area
    return (1/(4*(x-position)*(x-position)/width/width+1));//constant Height
}


Double_t IAxial(Double_t x, Double_t omega_par, Double_t omega_per){
    if(omega_par==omega_per) return 0; // delta function at x==omega_per, taken care of in the userfunction
    if((x<=min(omega_par,omega_per))||(x>=max(omega_par,omega_per))) return 0;
    if (abs(x-omega_per)<0.001*abs(omega_par-omega_per)){return 1./2/pow((omega_par-omega_per)*(0.03*(omega_par-omega_per)),0.5);}
    return 1./2/pow((omega_par-omega_per)*(x-omega_per),0.5);
}


Double_t IAsym(Double_t x, Double_t omega_center, Double_t omega_min,Double_t omega_max){
    if (omega_center==omega_min&&omega_min==omega_max) return 0; // delta function at x==omega_per, taken care of in the userfunction
    Double_t cutoff=0.001;
    if (abs(omega_center-omega_min)<cutoff*(omega_max-omega_min)) return IAxial(x,omega_max,omega_center);
    if (abs(omega_max-omega_center)<cutoff*(omega_max-omega_min)) return IAxial(x,omega_min,omega_center);
    if((x<=omega_min)||(x>=omega_max)) return 0;
    if (abs(omega_center-x)<cutoff*(omega_max-omega_min)){//Linear interpolation close to the pole at omega_center
        Double_t lower=IAsym_low(omega_center-cutoff*(omega_max-omega_min),omega_center,omega_min,omega_max);
        Double_t upper=IAsym_high(omega_center+cutoff*(omega_max-omega_min),omega_center,omega_min,omega_max);
        return lower+(omega_center-x)/cutoff/(omega_max-omega_min)*(upper-lower);
    }
    if(x-omega_center<0){//omega_min<x<omega_center
        return IAsym_low(x,omega_center,omega_min,omega_max);
    }

    if(x-omega_center>0){//omega_center<x<omega_max
        return IAsym_high(x,omega_center,omega_min,omega_max);
    }
    return 0;
}

Double_t IAsym_low(Double_t omega, Double_t omega_center, Double_t omega_min,Double_t omega_max){
    Double_t m =(omega-omega_min)*(omega_max-omega_center)/((omega_max-omega)*(omega_center-omega_min));
    return 1./PI/pow(omega_max-omega,0.5)/pow(omega_center-omega_min,0.5)*boost::math::ellint_1( m );
    }
Double_t IAsym_high(Double_t omega, Double_t omega_center,  Double_t omega_min,Double_t omega_max){
    Double_t m =(omega_center-omega_min)*(omega_max-omega)/((omega_max-omega_center)*(omega-omega_min));
    return 1./PI/pow(omega-omega_min,0.5)/pow(omega_max-omega_center,0.5)*boost::math::ellint_1( m );
    }


//Implement Userfunctions
ClassImp(LineGauss)  // for the ROOT dictionary

Double_t LineGauss::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==2); // make sure the number of parameters handed to the function is correct
  Double_t position=par[0];
  Double_t width=par[1];

  return GaussianShape( x, position, width);
}

ClassImp(LineLaplace)  // for the ROOT dictionary

Double_t LineLaplace::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==2); // make sure the number of parameters handed to the function is correct
  Double_t position=par[0];
  Double_t width=par[1];
  return LaplacianShape( x, position, width);

}


ClassImp(LineLorentzian)  // for the ROOT dictionary

Double_t LineLorentzian::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==2); // make sure the number of parameters handed to the function is correct
  Double_t position=par[0];
  Double_t width=par[1];
  return LorentzianShape( x, position, width);

}

ClassImp(LineSkewLorentzian)  // for the ROOT dictionary

Double_t LineSkewLorentzian::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  Double_t position=par[0];
  Double_t width=par[1];
  Double_t a=par[2];


  //  sigmoidal variation of width, Stancik2008 dx.doi.org/10.1016/j.vibspec.2008.02.009
  Double_t skewwidth=2*width/(1+exp(a*(x-position)));

  return width/skewwidth*LorentzianShape( x, position, skewwidth);
}


ClassImp(LineSkewLorentzian2)  // for the ROOT dictionary

Double_t LineSkewLorentzian2::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  Double_t position=par[0];
  Double_t widthlow=par[1]; //width below position
  Double_t widthhigh=par[2]; //width above position

  if(x==position){return 1.0;}
  else if(x<position){
      if(!widthlow) {return 0.0;}
      return LorentzianShape( x, position, widthlow);;
  }
  else{
      if(!widthhigh) {return 0.0;}
      return LorentzianShape( x, position, widthhigh);;
  }
}


/******************************************************/
// Implementation PowderLineAxialLorGlobal

ClassImp(PowderLineAxialLorGlobal)  // for the ROOT dictionary

void PowderLineAxialLorGlobal::CalcNormalization(const vector<Double_t> &param){

assert(param.size() == 3);

// check that param are new and hence a calculation is needed (except the phase, which is not needed here)
Bool_t newParams = false;
if (fPrevParam.size() == 0) {
  for (UInt_t i=0; i<param.size(); i++)
    fPrevParam.push_back(param[i]);
  newParams = true;
} else {
  assert(param.size() == fPrevParam.size());

  for (UInt_t i=0; i<param.size(); i++)
    if (param[i] != fPrevParam[i]) {
      newParams = true;
      break;
    }
  }

if (!newParams)
  return;

// keep parameters
for (UInt_t i=0; i<param.size(); i++)
  fPrevParam[i] = param[i];


// calculate the Normalization
Double_t omega_par=param[0];
Double_t omega_per=param[1]; //middle of the line
Double_t width=param[2];//width of the Lorentzian

//symmetric lineshape is normalized by default:
if(omega_par==omega_per){ return;}

//ensure width>0
if(width<0) {width=-width;}

Double_t omin=min(omega_par,omega_per);
Double_t omax=max(omega_par,omega_per);
Double_t x=omega_per;

//Lorentzian window: maximally +- 15*width
//Lineshape Window [omin omax]
//Determine necessary convolution window
Double_t convmin=-15*width;
Double_t convmax=-convmin;
if(x-convmax>omin){convmax=x-omin;}
if(x-convmin<omax){convmin=x-omax;}

//convolution
Double_t result=0;
UInt_t steps=GetSteps();
for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
  result+=LorentzianShape(y,0,width)*IAxial(x-y,omega_par,omega_per);
}
fNormalization=result*(convmax-convmin)/steps;
if(!(fNormalization>0)) fNormalization=1;
return;
}


/******************************************************/
// Implementation PowderLineAxialLor
ClassImp(PowderLineAxialLor)  // for the ROOT dictionary

PowderLineAxialLor::PowderLineAxialLor(){
    fValid = false;
    fInvokedGlobal = false;
    fGlobalUserFcn = 0;
}


PowderLineAxialLor::~PowderLineAxialLor()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}



void PowderLineAxialLor::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new PowderLineAxialLorGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> PowderLineAxialLor::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PowderLineAxialLorGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (PowderLineAxialLorGlobal*)globalPart[fIdxGlobal];
  }
}



Double_t PowderLineAxialLor::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  Double_t omega_par=par[0];
  Double_t omega_per=par[1]; //middle of the line
  Double_t width=par[2];//width of the Lorentzian


  //ensure width>0
  if(width<0) width=-width;

  //calculate normalization globally
  fGlobalUserFcn->CalcNormalization(par);

  // extract Normalization from the global object
  Double_t Norm = fGlobalUserFcn->GetNormalization();

  //symmetric situation has a delta function:
  if(abs(omega_par-omega_per)<0.01*abs(width)){
      cout<<endl<<"Using a Lorentzian line shape";
      return LorentzianShape(x,omega_par,width);}

  Double_t omin=min(omega_par,omega_per);
  Double_t omax=max(omega_par,omega_per);

  //Lorentzian window: maximally +- 15*width
  //Lineshape Window [omin omax]
  //Determine necessary convolution window
  Double_t convmin=-15*width;
  Double_t convmax=-convmin;
  if(x-convmax>omin){convmax=x-omin;}
  if(x-convmin<omax){convmin=x-omax;}

  //convolution
  Double_t result=0;
  UInt_t steps=fGlobalUserFcn->GetSteps();
  for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
    result+=LorentzianShape(y,0,width)*IAxial(x-y,omega_par,omega_per);
  }
  result=result*(convmax-convmin)/steps;
  return result/Norm;

}


/******************************************************/
// Implementation PowderLineAxialGssGlobal

ClassImp(PowderLineAxialGssGlobal)  // for the ROOT dictionary

void PowderLineAxialGssGlobal::CalcNormalization(const vector<Double_t> &param){

assert(param.size() == 3);

// check that param are new and hence a calculation is needed (except the phase, which is not needed here)
Bool_t newParams = false;
if (fPrevParam.size() == 0) {
  for (UInt_t i=0; i<param.size(); i++)
    fPrevParam.push_back(param[i]);
  newParams = true;
} else {
  assert(param.size() == fPrevParam.size());

  for (UInt_t i=0; i<param.size(); i++)
    if (param[i] != fPrevParam[i]) {
      newParams = true;
      break;
    }
  }

if (!newParams)
  return;

// keep parameters
for (UInt_t i=0; i<param.size(); i++)
  fPrevParam[i] = param[i];


// calculate the Normalization
Double_t omega_par=param[0];
Double_t omega_per=param[1]; //middle of the line
Double_t width=param[2];//width of the Lorentzian

//symmetric lineshape is normalized by default:
if(omega_par==omega_per){ return;}


//ensure width>0
if(width<0) width=-width;


Double_t omin=min(omega_par,omega_per);
Double_t omax=max(omega_par,omega_per);
Double_t x=omega_per;

//Gaussian window: maximally +- 15*width
//Lineshape Window [omin omax]
//Determine necessary convolution window
Double_t convmin=-15*width;
Double_t convmax=-convmin;
if(x-convmax>omin){convmax=x-omin;}
if(x-convmin<omax){convmin=x-omax;}

//convolution
Double_t result=0;
UInt_t steps=GetSteps();
for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
  result+=GaussianShape(y,0,width)*IAxial(x-y,omega_par,omega_per);
}
fNormalization=result*(convmax-convmin)/steps;
if(!(fNormalization>0)) fNormalization=1;
return;
}


/******************************************************/
// Implementation PowderLineAxialGss
ClassImp(PowderLineAxialGss)  // for the ROOT dictionary

PowderLineAxialGss::PowderLineAxialGss(){
    fValid = false;
    fInvokedGlobal = false;
    fGlobalUserFcn = 0;
}


PowderLineAxialGss::~PowderLineAxialGss()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}



void PowderLineAxialGss::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new PowderLineAxialGssGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> PowderLineAxialGss::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PowderLineAxialGssGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (PowderLineAxialGssGlobal*)globalPart[fIdxGlobal];
  }
}



Double_t PowderLineAxialGss::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==3); // make sure the number of parameters handed to the function is correct
  Double_t omega_par=par[0];
  Double_t omega_per=par[1]; //middle of the line
  Double_t width=par[2];//width of the Gaussian


  //ensure width>0
  if(width<0) width=-width;

  //calculate normalization globally
  fGlobalUserFcn->CalcNormalization(par);

  // extract Normalization from the global object
  Double_t Norm = fGlobalUserFcn->GetNormalization();

  //symmetric situation has a delta function:
  if(abs(omega_par-omega_per)<0.01*abs(width)){
      cout<<endl<<"Using a Gaussian line shape";
      return GaussianShape(x,omega_par,width);}

  Double_t omin=min(omega_par,omega_per);
  Double_t omax=max(omega_par,omega_per);

  //Gaussian window: maximally +- 15*width
  //Lineshape Window [omin omax]
  //Determine necessary convolution window
  Double_t convmin=-15*width;
  Double_t convmax=-convmin;
  if(x-convmax>omin){convmax=x-omin;}
  if(x-convmin<omax){convmin=x-omax;}

  //convolution
  Double_t result=0;
  UInt_t steps=fGlobalUserFcn->GetSteps();
  for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
    result+=GaussianShape(y,0,width)*IAxial(x-y,omega_par,omega_per);
  }
  result=result*(convmax-convmin)/steps;
  return result/Norm;

}



/******************************************************/
// Implementation PowderLineAsymLorGlobal
ClassImp(PowderLineAsymLorGlobal)  // for the ROOT dictionary

void PowderLineAsymLorGlobal::UpdateGlobalPart(const vector<Double_t> &param){

assert(param.size() == 4);

// check that param are new and hence a calculation is needed (except the phase, which is not needed here)
Bool_t newParams = false;
if (fPrevParam.size() == 0) {
  for (UInt_t i=0; i<param.size(); i++)
    fPrevParam.push_back(param[i]);
  newParams = true;
} else {
  assert(param.size() == fPrevParam.size());

  for (UInt_t i=0; i<param.size(); i++)
    if (param[i] != fPrevParam[i]) {
      newParams = true;
      break;
    }
  }

if (!newParams)
  return;

// keep parameters
for (UInt_t i=0; i<param.size(); i++)
  fPrevParam[i] = param[i];



// calculate the Normalization
Double_t omega1=param[0];
Double_t omega2=param[1];
Double_t omega3=param[2];
Double_t width=param[3];//width of the Lorentzian


//ensure width>0
if(width<0) {width=-width;}

//sort omegas:
if(omega1>=omega2){
    if(omega1>=omega3){
        fOmegaMax=omega1;
        if (omega2>=omega3){
            fOmegaCenter=omega2;
            fOmegaMin=omega3;
        }else{
            fOmegaCenter=omega3;
            fOmegaMin=omega2;
        }
    }else {
        fOmegaMin=omega2;
        fOmegaCenter=omega1;
        fOmegaMax=omega3;
    }
}else if (omega2>omega3){
    fOmegaMax=omega2;
    if(omega3<omega1){
        fOmegaMin=omega3;
        fOmegaCenter=omega1;
    }else{
        fOmegaMin=omega1;
        fOmegaCenter=omega3;
    }
}else {
    fOmegaMin=omega1;
    fOmegaCenter=omega2;
    fOmegaMax=omega3;
}


//calculate normalization
Double_t x=fOmegaCenter;

//Lorentzian window: maximally +- 15*width
//Lineshape Window [fOmegaMin fOmegaMax]
//Determine necessary convolution window
Double_t convmin=-15*width;
Double_t convmax=-convmin;
if(x-convmax>fOmegaMin){convmax=x-fOmegaMin;}
if(x-convmin<fOmegaMax){convmin=x-fOmegaMax;}

//convolution
Double_t result=0;
UInt_t steps=GetSteps();
for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
  result+=LorentzianShape(y,0,width)* IAsym(x-y,fOmegaCenter,fOmegaMin,fOmegaMax);
}
fNormalization=result*(convmax-convmin)/steps;
if(!(fNormalization>0)) fNormalization=1;
return;
}


/******************************************************/
// Implementation PowderLineAxialLor
ClassImp(PowderLineAsymLor)  // for the ROOT dictionary

PowderLineAsymLor::PowderLineAsymLor(){
    fValid = false;
    fInvokedGlobal = false;
    fGlobalUserFcn = 0;
}


PowderLineAsymLor::~PowderLineAsymLor()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}



void PowderLineAsymLor::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new PowderLineAsymLorGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> PowderLineAxialLor::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PowderLineAsymLorGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (PowderLineAsymLorGlobal*)globalPart[fIdxGlobal];
  }
}



Double_t PowderLineAsymLor::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==4); // make sure the number of parameters handed to the function is correct
  Double_t width=par[3];//width of the Lorentzian


  //ensure width>0
  if(width<0) width=-width;

  //calculate normalization globally
  fGlobalUserFcn->UpdateGlobalPart(par);

  // extract Normalization from the global object
  Double_t Norm =fGlobalUserFcn->GetNorm();
  Double_t omega_center=fGlobalUserFcn->GetCenter();
  Double_t omega_min=fGlobalUserFcn->GetMin();
  Double_t omega_max=fGlobalUserFcn->GetMax();


  //symmetric situation has a delta function:
  if(abs(omega_max-omega_min)<0.01*abs(width)){
      cout<<endl<<"Using a Lorentzian line shape";
      return LorentzianShape(x,omega_center,width);}


  //Lorentzian window: maximally +- 15*width
  //Lineshape Window [omega_min omega_max]
  //Determine necessary convolution window
  Double_t convmin=-15*width;
  Double_t convmax=-convmin;
  if(x-convmax>omega_min){convmax=x-omega_min;}
  if(x-convmin<omega_max){convmin=x-omega_max;}

  //convolution
  Double_t result=0;
  UInt_t steps=fGlobalUserFcn->GetSteps();
  for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
    result+=LorentzianShape(y,0,width)* IAsym(x-y,omega_center,omega_min,omega_max);
  }
  result=result*(convmax-convmin)/steps;
  return result/Norm;

}



/******************************************************/
// Implementation PowderLineAsymGssGlobal
ClassImp(PowderLineAsymGssGlobal)  // for the ROOT dictionary

void PowderLineAsymGssGlobal::UpdateGlobalPart(const vector<Double_t> &param){

assert(param.size() == 4);

// check that param are new and hence a calculation is needed (except the phase, which is not needed here)
Bool_t newParams = false;
if (fPrevParam.size() == 0) {
  for (UInt_t i=0; i<param.size(); i++)
    fPrevParam.push_back(param[i]);
  newParams = true;
} else {
  assert(param.size() == fPrevParam.size());

  for (UInt_t i=0; i<param.size(); i++)
    if (param[i] != fPrevParam[i]) {
      newParams = true;
      break;
    }
  }

if (!newParams)
  return;

// keep parameters
for (UInt_t i=0; i<param.size(); i++)
  fPrevParam[i] = param[i];



// calculate the Normalization
Double_t omega1=param[0];
Double_t omega2=param[1];
Double_t omega3=param[2];
Double_t width=param[3];//width of the Lorentzian


//ensure width>0
if(width<0) {width=-width;}

//sort omegas:
if(omega1>=omega2){
    if(omega1>=omega3){
        fOmegaMax=omega1;
        if (omega2>=omega3){
            fOmegaCenter=omega2;
            fOmegaMin=omega3;
        }else{
            fOmegaCenter=omega3;
            fOmegaMin=omega2;
        }
    }else {
        fOmegaMin=omega2;
        fOmegaCenter=omega1;
        fOmegaMax=omega3;
    }
}else if (omega2>omega3){
    fOmegaMax=omega2;
    if(omega3<omega1){
        fOmegaMin=omega3;
        fOmegaCenter=omega1;
    }else{
        fOmegaMin=omega1;
        fOmegaCenter=omega3;
    }
}else {
    fOmegaMin=omega1;
    fOmegaCenter=omega2;
    fOmegaMax=omega3;
}


//calculate normalization
Double_t x=fOmegaCenter;

//Lorentzian window: maximally +- 15*width
//Lineshape Window [fOmegaMin fOmegaMax]
//Determine necessary convolution window
Double_t convmin=-15*width;
Double_t convmax=-convmin;
if(x-convmax>fOmegaMin){convmax=x-fOmegaMin;}
if(x-convmin<fOmegaMax){convmin=x-fOmegaMax;}

//convolution
Double_t result=0;
UInt_t steps=GetSteps();
for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
  result+=GaussianShape(y,0,width)* IAsym(x-y,fOmegaCenter,fOmegaMin,fOmegaMax);
}
fNormalization=result*(convmax-convmin)/steps;
if(!(fNormalization>0)) fNormalization=1;
return;
}


/******************************************************/
// Implementation PowderLineAsymGss
ClassImp(PowderLineAsymGss)  // for the ROOT dictionary

PowderLineAsymGss::PowderLineAsymGss(){
    fValid = false;
    fInvokedGlobal = false;
    fGlobalUserFcn = 0;
}


PowderLineAsymGss::~PowderLineAsymGss()
{
  if ((fGlobalUserFcn != 0) && fInvokedGlobal) {
    delete fGlobalUserFcn;
    fGlobalUserFcn = 0;
  }
}



void PowderLineAsymGss::SetGlobalPart(vector<void *> &globalPart, UInt_t idx)
{
  fIdxGlobal = static_cast<Int_t>(idx);

  if ((Int_t)globalPart.size() <= fIdxGlobal) { // global user function not present, invoke it
    fGlobalUserFcn = new PowderLineAsymGssGlobal();
    if (fGlobalUserFcn == 0) { // global user function object couldn't be invoked -> error
      fValid = false;
      cerr << endl << ">> PowderLineAxialLor::SetGlobalPart(): **ERROR** Couldn't invoke global user function object, sorry ..." << endl;
    } else {  // global user function object could be invoked -> resize to global user function vector and keep the pointer to the corresponding object
      globalPart.resize(fIdxGlobal+1);
      globalPart[fIdxGlobal] = dynamic_cast<PowderLineAsymGssGlobal*>(fGlobalUserFcn);
      fValid = true;
      fInvokedGlobal = true;
    }
  } else { // global user function already present hence just retrieve a pointer to it
    fValid = true;
    fGlobalUserFcn = (PowderLineAsymGssGlobal*)globalPart[fIdxGlobal];
  }
}



Double_t PowderLineAsymGss::operator()(Double_t x, const vector<Double_t> &par) const {
  assert(par.size()==4); // make sure the number of parameters handed to the function is correct
  Double_t width=par[3];//width of the Lorentzian


  //ensure width>0
  if(width<0) width=-width;

  //calculate normalization globally
  fGlobalUserFcn->UpdateGlobalPart(par);

  // extract Normalization from the global object
  Double_t Norm =fGlobalUserFcn->GetNorm();
  Double_t omega_center=fGlobalUserFcn->GetCenter();
  Double_t omega_min=fGlobalUserFcn->GetMin();
  Double_t omega_max=fGlobalUserFcn->GetMax();


  //symmetric situation has a delta function:
  if(abs(omega_max-omega_min)<0.01*abs(width)){
      cout<<endl<<"Using a Lorentzian line shape";
      return GaussianShape(x,omega_center,width);}


  //Lorentzian window: maximally +- 15*width
  //Lineshape Window [omega_min omega_max]
  //Determine necessary convolution window
  Double_t convmin=-15*width;
  Double_t convmax=-convmin;
  if(x-convmax>omega_min){convmax=x-omega_min;}
  if(x-convmin<omega_max){convmin=x-omega_max;}

  //convolution
  Double_t result=0;
  UInt_t steps=fGlobalUserFcn->GetSteps();
  for(Double_t y=convmin;y<convmax;y+=(convmax-convmin)/steps){
    result+=GaussianShape(y,0,width)* IAsym(x-y,omega_center,omega_min,omega_max);
  }
  result=result*(convmax-convmin)/steps;
  return result/Norm;

}



