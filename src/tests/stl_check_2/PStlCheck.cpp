#include "PStlCheck.h"

PStlCheck::PStlCheck()
{
}

PStlCheck::~PStlCheck()
{
  fPpo.clear();
}

void PStlCheck::Add(unsigned int count)
{
  fPpo.push_back(new PPointObj(count));
}

void PStlCheck::CleanUp()
{
  for (unsigned int i=0; i<fPpo.size(); i++) {
    fPpo[i]->~PPointObj();
  }
}

void PStlCheck::PrintTest()
{
  for (unsigned int i=0; i<fPpo.size(); i++)
    fPpo[i]->PrintTest();
}

