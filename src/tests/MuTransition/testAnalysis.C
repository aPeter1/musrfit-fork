/***************************************************************************

  testAnalysis.C

  Author: Thomas Prokscha
  Date: 26-Feb-2010

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Thomas Prokscha, Paul Scherrer Institut         *
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
{
 //gROOT->Reset();
 Int_t rebin = 5;
 Double_t t0, tbin;

 t0     = 0.;
 tbin   = 1.;
 gROOT->ProcessLine(".x defineFit.C"); //define fit function muonLife

 histos   = dynamic_cast <TFolder*>(gDirectory->Get("histos"));
 hDecay00 = dynamic_cast <TH1F*>(histos->FindObjectAny("hDecay00"));
 hDecay01 = dynamic_cast <TH1F*>(histos->FindObjectAny("hDecay01"));
 
 hForward   = hDecay00->Rebin(rebin, "hForward");
 hBackward  = hDecay01->Rebin(rebin, "hBackward");

 //hForward->Draw();
 hSum = (TH1*) hForward->Clone("hSum");
 hSum->Reset();
 hSum->Add(hForward, hBackward, 1., 1.);
 //hSum->Draw();

 hAsym = (TH1*) hForward->GetAsymmetry(hBackward, 1., 0.);
 hAsym->SetName("Asymmetry");
 hAsym->SetTitle("Asymmetry");
 hAsym->GetXaxis()->SetTitle("Time (ns)");
 hAsym->Draw();

 asymTFExpo->SetLineColor(2); //fit function red
}
