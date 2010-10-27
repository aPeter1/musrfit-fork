/***************************************************************************

  PEventHandlerTest.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2010 by Andreas Suter                                   *
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

#include <TRandom.h>
#include <TAxis.h>

#include "PEventHandlerTest.h"

ClassImpQ(PEventHandlerTest)

PEventHandlerTest::PEventHandlerTest()
{
  fSwitched = false;

  fMainCanvas = new TCanvas("fMainCanvas", "Event Handler Test", 10, 10, 600, 400);

  fMainCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "PEventHandlerTest",
                       this, "HandleCmdKey(Int_t,Int_t,Int_t,TObject*)");

  fMultiGraph1 = new TMultiGraph();
  fMultiGraph2 = 0;

  // feed to random graphs
  TGraph *graph;
  TRandom rand;
  for (UInt_t i=0; i<3; i++) {
    graph = new TGraph(100);
    for (UInt_t j=0; j<100; j++) {
      graph->SetPoint(j, 0.01*(Double_t)j, 0.0001*(Double_t)i*(Double_t)(j*j)+0.01*(Double_t)j+0.1*rand.Rndm());
    }
    graph->SetMarkerStyle(20+i);
    graph->SetMarkerColor(1+i);
    fGraph.push_back(graph);
  }

  fMultiGraph1->Add(new TGraph(*(fGraph[0])), "p");
  fMultiGraph1->Add(new TGraph(*(fGraph[1])), "p");

  fMultiGraph1->Draw("a");

  fMainCanvas->Show();
}

PEventHandlerTest::~PEventHandlerTest()
{
  for (UInt_t i=0; i<fGraph.size(); i++) {
    delete fGraph[i];
  }
  fGraph.clear();

  if (fMultiGraph1) {
    delete fMultiGraph1;
    fMultiGraph1 = 0;
  }
  if (fMultiGraph2) {
    delete fMultiGraph2;
    fMultiGraph2 = 0;
  }

  if (fMainCanvas) {
    delete fMainCanvas;
    fMainCanvas = 0;
  }
}

void PEventHandlerTest::Done(Int_t status)
{
  Emit("Done(Int_t)", status);
}

void PEventHandlerTest::HandleCmdKey(Int_t event, Int_t x, Int_t y, TObject *selected)
{
  if (event != kKeyPress)
     return;

  if (x == 'q') { // quit
    Done(0);
  } else if (x == 's') { // switch
    fSwitched = !fSwitched;
    SwitchGraph();
  }
}

void PEventHandlerTest::LastCanvasClosed()
{
  if (gROOT->GetListOfCanvases()->IsEmpty()) {
    Done(0);
  }
}

void PEventHandlerTest::SwitchGraph()
{
  Double_t xmin, xmax;

  // tell the canvas that the selected object (the one under the mouse pointer) is not your object, before to actually delete it.
  fMainCanvas->SetSelected(fMainCanvas->GetPadSave());

  if (fSwitched) {
    xmin = fMultiGraph1->GetXaxis()->GetBinCenter(fMultiGraph1->GetXaxis()->GetFirst());
    xmax = fMultiGraph1->GetXaxis()->GetBinCenter(fMultiGraph1->GetXaxis()->GetLast());

    delete fMultiGraph1;
    fMultiGraph1 = 0;

    fMultiGraph2 = new TMultiGraph();
    fMultiGraph2->Add(new TGraph(*(fGraph[0])), "p");
    fMultiGraph2->Add(new TGraph(*(fGraph[2])), "p");

    fMultiGraph2->Draw("a");
    fMultiGraph2->GetXaxis()->SetRangeUser(xmin, xmax);
    fMultiGraph2->Draw("a");
  } else {
    xmin = fMultiGraph2->GetXaxis()->GetBinCenter(fMultiGraph2->GetXaxis()->GetFirst());
    xmax = fMultiGraph2->GetXaxis()->GetBinCenter(fMultiGraph2->GetXaxis()->GetLast());

    delete fMultiGraph2;
    fMultiGraph2 = 0;

    fMultiGraph1 = new TMultiGraph();
    fMultiGraph1->Add(new TGraph(*(fGraph[0])), "p");
    fMultiGraph1->Add(new TGraph(*(fGraph[1])), "p");

    fMultiGraph1->Draw("a");
    fMultiGraph1->GetXaxis()->SetRangeUser(xmin, xmax);
    fMultiGraph1->Draw("a");
  }

  fMainCanvas->Update();
}
