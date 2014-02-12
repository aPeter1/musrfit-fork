/***************************************************************************

  musrRootValidation.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2014 by Andreas Suter                              *
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

#include "TString.h"
#include "TFile.h"
#include "TFolder.h"
#include "TKey.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TSystemFile.h"

#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

#include "git-revision.h"
#include "PMusr.h"

//-----------------------------------------------------------------------
/**
 * <p>class which converts the structure of a ROOT file into a XML file.
 */
class PMusrRoot2Xml
{
  public:
    PMusrRoot2Xml(const char *fileName, bool quiet, bool keep);
    virtual ~PMusrRoot2Xml();

    virtual Bool_t IsValid() { return fValid; }
    virtual TString GetXmlDumpFileName() { return fXmlDumpFileName; }
    virtual UInt_t GetNoOfDecayHistos() { return fNoOfDecayHistos; }
    virtual UInt_t GetNoOfExpectedHistos() { return fNoOfRedGreenOffsets*fNoOfHistos; }
    virtual UInt_t GetNoOfHistos() { return fNoOfHistos; }
    virtual UInt_t GetNoOfRedGreenOffsets() { return fNoOfRedGreenOffsets; }
    virtual UInt_t GetNoOfDetectors() { return fNoOfDetectors; }

  private:
    enum EFolderTag {eUnkown, eDecayAnaModule, eSlowControlAnaModule};

    vector<string> fXmlData; ///< keeps the XML structure dump of the ROOT file

    Bool_t fQuiet; ///< true = suppress output while converting
    Bool_t fKeep;  ///< true = keep the XML dump file
    Bool_t fValid; ///< true if the conversion was fine
    TString fFileName; ///< file name of the ROOT file
    TString fXmlDumpFileName; ///< file name of the XML dump file
    EFolderTag fFolderTag; ///< switch indicating which kind of TFolder object is found

    UInt_t fNoOfDecayHistos; ///< number of decay histos in the DecayAnaModule
    UInt_t fNoOfHistos; ///< number of histos from run header
    UInt_t fNoOfRedGreenOffsets; ///< number of RedGreen offsets
    UInt_t fNoOfDetectors; ///< number of detector entries in the header

    virtual void SortHistoFolders();
    virtual void DumpFolder(TFolder *folder, UInt_t offset);
    virtual void DumpObjArray(TObjArray *obj, UInt_t offset);
    virtual void DumpEntry(TObject *obj, UInt_t offset);
    virtual void CheckClass(TObject *obj, TString str, UInt_t offset);
    virtual void GetType(TString entry, TString &type);
    virtual void GetLabel(TString entry, TString &label);
};

//-----------------------------------------------------------------------
/**
 * <p>Constructor. Reads the ROOT file and converts its structure to an XML dump file.
 */
PMusrRoot2Xml::PMusrRoot2Xml(const char *fileName, bool quiet, bool keep) : fQuiet(quiet), fKeep(keep), fFileName(fileName)
{
  fXmlDumpFileName = "__MusrRootXmlDump.xml";
  fFolderTag = eUnkown;
  fValid = false;
  fXmlData.clear();
  fNoOfDecayHistos = 0;
  fNoOfHistos = 0;
  fNoOfRedGreenOffsets = 0;
  fNoOfDetectors = 0;

  // read assumed MusrRoot file
  TFile f(fFileName.Data());

  if (f.IsZombie()) {
    cerr << endl << "**ERROR** couldn't open file " << fFileName << endl;
    return;
  }

  fXmlData.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  fXmlData.push_back("<MusrRoot xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"file:MusrRoot.xsd\">");

  TIter next = f.GetListOfKeys();
  TKey *key;
  TFolder *folder;
  TString str, tag;

  UInt_t offset = 2;

  while ((key = (TKey*) next()) != 0) {
    if (!fQuiet) cout << endl << "name: " << key->GetName() << ", class name: " << key->GetClassName();
    str = key->GetClassName();
    if (str == "TFolder") {
      folder = (TFolder*)key->ReadObj();
      CheckClass(folder, str, offset);
    }
  }
  if (!fQuiet) cout << endl;

  f.Close();

  fXmlData.push_back("</MusrRoot>");

  // the sort_histo_folders is needed since XML-Schema is not flexible enough to handle
  // histos -|
  //         |- DecayAnaModule
  //         ... (any other analyzer module sub-folder
  //         |- SCAnaModule
  // Hence SCAnaModule has artificially moved up, just to follow DecayAnaModule
  SortHistoFolders();

  ofstream fout(fXmlDumpFileName.Data());

  for (UInt_t i=0; i<fXmlData.size(); i++)
    fout << fXmlData[i] << endl;
  fout.close();

  fValid = true;
}

//-----------------------------------------------------------------------
/**
 * <p>Destructor.
 */
PMusrRoot2Xml::~PMusrRoot2Xml()
{
  if (!fKeep) {
    TSystemFile sf(fXmlDumpFileName.Data(), "./");
    sf.Delete();
  }
}

//-----------------------------------------------------------------------
/**
 * <p>Sorts the folders of the 'histos' TFolder in order to enforce the order
 * according to:
 * -# DecayAnaModule
 * -# SCAnaModule
 * -# all the rest
 * <p>This is needed to the limited abilities of XML-Schema validation.
 */
void PMusrRoot2Xml::SortHistoFolders()
{
  vector<string> temp_xml_data;

  // first make a copy of the original fXmlData
  for (unsigned int i=0; i<fXmlData.size(); i++)
    temp_xml_data.push_back(fXmlData[i]);

  // remove SCAnaModule from temp_xml_data
  unsigned int start = 0, end = 0;
  for (unsigned int i=0; i<temp_xml_data.size(); i++) {
    if (temp_xml_data[i].find("<SCAnaModule>") != string::npos)
      start = i;
    if (temp_xml_data[i].find("</SCAnaModule>") != string::npos)
      end = i+1;
  }
  if ((start > 0) && (end > 0))
    temp_xml_data.erase(temp_xml_data.begin()+start, temp_xml_data.begin()+end);
  else // no SCAnaModule present, hence nothing to be done
    return;

  // insert SCAnaModule just after DecayAnaModule
  // 1st find end of DecayAnaModule
  unsigned int pos = 0;
  for (unsigned int i=0; i<temp_xml_data.size(); i++) {
    if (temp_xml_data[i].find("</DecayAnaModule>") != string::npos) {
      pos = i+1;
      break;
    }
  }
  if (pos == 0) // something is wrong, hence to not do anything
    return;
  temp_xml_data.insert(temp_xml_data.begin()+pos, fXmlData.begin()+start, fXmlData.begin()+end);

  // copy temp_xml_data back into fXmlData
  fXmlData.clear();
  for (unsigned int i=0; i<temp_xml_data.size(); i++)
    fXmlData.push_back(temp_xml_data[i]);

  // clean up
  temp_xml_data.clear();
}

//-----------------------------------------------------------------------
/**
 * <p>Dump folder structure.
 *
 * \param folder TFolder object found in the ROOT file
 * \param offset needed to indent dump info
 */
void PMusrRoot2Xml::DumpFolder(TFolder *folder, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  TIter next = folder->GetListOfFolders();
  TObject *obj;
  TString str;
  while ((obj = (TObject*) next()) != 0) {
    if (!fQuiet) cout << endl << offsetStr << "name: " << obj->GetName() << ", class name: " << obj->ClassName();
    str = obj->ClassName();
    CheckClass(obj, str, offset);
  }
}

//-----------------------------------------------------------------------
/**
 * <p>Dump object array content structure.
 *
 * \param obj object array found in the ROOT file
 * \param offset needed to indent dump info
 */
void PMusrRoot2Xml::DumpObjArray(TObjArray *obj, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  TObjString *tstr;
  TString str, xmlStr, type, label, xmlLabel;

  // check if the obj name is anything like DetectorXXX, where XXX is a number
  xmlLabel = TString(obj->GetName());
  if (xmlLabel.BeginsWith("Detector")) {
    xmlLabel.Remove(0, 8); // remove 'Detector'
    if (xmlLabel.IsDigit())
      xmlLabel = "Detector";
    else
      xmlLabel = TString(obj->GetName());
  }

  if (!fQuiet) cout << endl << offsetStr << obj->GetName() << " (# " << obj->GetEntries() << ")";
  if (!strcmp(obj->GetName(), "DetectorInfo"))
    fNoOfDetectors = obj->GetEntries();

  xmlStr = offsetStr + "<" + xmlLabel + ">";
  fXmlData.push_back(xmlStr.Data());

  for (UInt_t i=0; i<(UInt_t)obj->GetEntries(); i++) {
    // check if entry is a TObjArray
    type = obj->At(i)->ClassName();
    if (type == "TObjArray") {
      DumpObjArray((TObjArray*)(obj->At(i)), offset+2);
    } else { // not a TObjArray
      tstr = (TObjString*) obj->At(i);
      str = tstr->GetString();
      str.Remove(TString::kTrailing, '\n');

      GetType(str, type);
      GetLabel(str, label);

      if (!fQuiet) cout << endl << offsetStr << i << ": " << str;

      // filter out the number of histograms according to the RunInfo
      if (str.Contains("- No of Histos: ")) {
        TString histoStr = str;
        Ssiz_t pos = histoStr.Last(':');
        histoStr.Remove(0, pos+1);
        pos = histoStr.Last('-');
        histoStr.Remove(pos);
        fNoOfHistos = histoStr.Atoi();
      }

      // filter out the number of Red/Green offsets
      if (str.Contains("- RedGreen Offsets: ")) {
        TString redGreenStr = str;
        Ssiz_t pos = redGreenStr.Last(':');
        redGreenStr.Remove(0, pos+1);
        pos = redGreenStr.Last('-');
        redGreenStr.Remove(pos);
        TObjArray *tokens = redGreenStr.Tokenize(";");
        if (tokens)
          fNoOfRedGreenOffsets = tokens->GetEntries();
        if (tokens) delete tokens;
      }

      xmlStr = offsetStr + "  " + "<" + label + ">" + type + "</" + label + ">" ;
      fXmlData.push_back(xmlStr.Data());
    }
  }

  xmlStr = offsetStr + "</" + xmlLabel + ">";
  fXmlData.push_back(xmlStr.Data());
}

//-----------------------------------------------------------------------
/**
 * <p>Dump content.
 *
 * \param obj object found in the ROOT file to be dumped
 * \param offset needed to indent dump info
 */
void PMusrRoot2Xml::DumpEntry(TObject *obj, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  TString nameTag(""), typeTag("");
  switch (fFolderTag) {
    case eDecayAnaModule:
      nameTag = "HistoName";
      typeTag = "HistoType";
      break;
    case eSlowControlAnaModule:
      nameTag = "SlowControlName";
      typeTag = "SlowControlType";
      break;
    case eUnkown:
    default:
      nameTag = "Name";
      typeTag = "Type";
      break;
  }

  if (fFolderTag == eDecayAnaModule)
    fNoOfDecayHistos++;

  TString str;

  str = offsetStr + "<" + nameTag + ">";
  str += obj->GetName();
  str += "</" + nameTag + ">";
  fXmlData.push_back(str.Data());

  str = offsetStr + "<" + typeTag + ">";
  str += obj->ClassName();
  str += "</" + typeTag + ">";
  fXmlData.push_back(str.Data());
}

//-----------------------------------------------------------------------
/**
 * <p>
 *
 * \param obj object to be checked
 * \param str tag telling what kind of object it is
 * \param offset needed to indent dump info
 */
void PMusrRoot2Xml::CheckClass(TObject *obj, TString str, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  if (str == "TFolder") {
    TString xmlTagName(TString(obj->GetName()));

    // set folder tag
    if (!xmlTagName.CompareTo("DecayAnaModule"))
      fFolderTag = eDecayAnaModule;
    else if (!xmlTagName.CompareTo("SCAnaModule"))
      fFolderTag = eSlowControlAnaModule;
    else if (!xmlTagName.CompareTo("SCAnaModule"))
      fFolderTag = eSlowControlAnaModule;
    else
      fFolderTag = eUnkown;

    offset += 2;
    str = offsetStr + "<" + xmlTagName + ">";
    fXmlData.push_back(str.Data());

    DumpFolder((TFolder*)obj, offset);

    str = offsetStr + "</" + xmlTagName + ">";
    fXmlData.push_back(str.Data());
  } else if (str == "TObjArray") {
    offset += 2;
    DumpObjArray((TObjArray*)obj, offset);
  } else {
    // filter out the proper entry tag
    TString entryTag("");
    switch (fFolderTag) {
      case eDecayAnaModule:
        entryTag = TString("DecayHistoEntry");
        break;
      case eSlowControlAnaModule:
        entryTag = TString("SlowControlHistoEntry");
        break;
      case eUnkown:
      default:
        entryTag = TString("Entry");
        break;
    }

    offset += 2;
    str = offsetStr + "<" + entryTag + ">";
    fXmlData.push_back(str.Data());
    DumpEntry((TObjArray*)obj, offset);
    str = offsetStr + "</" + entryTag + ">";
    fXmlData.push_back(str.Data());
  }
}

//-----------------------------------------------------------------------
/**
 * <p>Checks to TMusrRunHeader type of the RunHeader entries.
 *
 * \param entry to be checked.
 * \param type repesentation of the entry
 */
void PMusrRoot2Xml::GetType(TString entry, TString &type)
{
  if (entry.Contains("-@0")) {
    type = "TString";
  } else if (entry.Contains("-@1")) {
    type = "Int_t";
  } else if (entry.Contains("-@2")) {
    type = "Double_t";
  } else if (entry.Contains("-@3")) {
    type = "TMusrRunPhysicalQuantity";
  } else if (entry.Contains("-@4")) {
    type = "TStringVector";
  } else if (entry.Contains("-@5")) {
    type = "TIntVector";
  } else if (entry.Contains("-@6")) {
    type = "TDoubleVector";
  } else {
    type = "TString";
  }
}

//-----------------------------------------------------------------------
/**
 * <p>Filters from a TMusrRunHeader RunHeader entry the label.
 *
 * \param entry to be filtered
 * \param label extracted from entry
 */
void PMusrRoot2Xml::GetLabel(TString entry, TString &label)
{
  label="no_idea";

  Ssiz_t start = entry.First('-');
  Ssiz_t end   = entry.First(':');

  if ((start == -1) || (end == -1))
    return;

  if (end - start < 2)
    return;

  // check that '-@' is present in the string, otherwise it is NOT a known label
  Ssiz_t pos = entry.First('@');
  if (pos < 1)
    return;
  if (entry(pos-1) != '-')
    return;

  // cut out value
  label = entry;
  label.Remove(0, start+2);
  label.Remove(end-start-2, label.Length());

  label.ReplaceAll(' ', '_'); // replace spaces through underscores
  label.ReplaceAll('(', '_'); // replace '(' through underscores
  label.ReplaceAll(')', '_'); // replace ')' through underscores
  label.ReplaceAll('[', '_'); // replace '[' through underscores
  label.ReplaceAll(']', '_'); // replace ']' through underscores
  label.ReplaceAll('{', '_'); // replace '[' through underscores
  label.ReplaceAll('}', '_'); // replace ']' through underscores
}

//-----------------------------------------------------------------------
/**
 * <p>Dump help information.
 */
void mrv_syntax()
{
  cout << endl << "usage: musrRootValidation <musrRootFile> <musrRootSchema> [--quiet] [--keep] | --help | --version";
  cout << endl << "   --quiet: do not dump the MusrRoot file info while validating";
  cout << endl << "   --keep: do NOT delete the intermediate XML-file";
  cout << endl << "   --help: shows this help";
  cout << endl << "   --version: shows the current version";
  cout << endl << endl;
}

//-----------------------------------------------------------------------
/**
 * <p>Validates an XML file against a Schema.
 *
 * \param doc XML file object representation
 * \param schema_filename Schema file name
 */
int is_valid(const xmlDocPtr doc, const char *schema_filename)
{
    xmlDocPtr schema_doc = xmlReadFile(schema_filename, NULL, XML_PARSE_NONET);
    if (schema_doc == NULL) {
      cerr << endl << "**ERROR** the schema (" << schema_filename << ") cannot be loaded or is not well-formed" << endl << endl;
      return -1;
    }
    xmlSchemaParserCtxtPtr parser_ctxt = xmlSchemaNewDocParserCtxt(schema_doc);
    if (parser_ctxt == NULL) {
      cerr << endl << "**ERROR** unable to create a parser context for the schema." << endl << endl;
      xmlFreeDoc(schema_doc);
      return -2;
    }
    xmlSchemaPtr schema = xmlSchemaParse(parser_ctxt);
    if (schema == NULL) {
      cerr << endl << "**ERROR** the schema itself is not valid." << endl << endl;
      xmlSchemaFreeParserCtxt(parser_ctxt);
      xmlFreeDoc(schema_doc);
      return -3;
    }
    xmlSchemaValidCtxtPtr valid_ctxt = xmlSchemaNewValidCtxt(schema);
    if (valid_ctxt == NULL) {
      cerr << endl << "**ERROR** unable to create a validation context for the schema." << endl << endl;
      xmlSchemaFree(schema);
      xmlSchemaFreeParserCtxt(parser_ctxt);
      xmlFreeDoc(schema_doc);
      return -4;
    }
    int is_valid = (xmlSchemaValidateDoc(valid_ctxt, doc) == 0);
    xmlSchemaFreeValidCtxt(valid_ctxt);
    xmlSchemaFree(schema);
    xmlSchemaFreeParserCtxt(parser_ctxt);
    xmlFreeDoc(schema_doc);
    // force the return value to be non-negative on success
    return is_valid ? 1 : 0;
}

//-----------------------------------------------------------------------
/**
 * <p>
 */
int main(int argc, char *argv[])
{
  if (argc==1) {
    mrv_syntax();
    return -1;
  } else if (argc==2) {
    if (!strcmp(argv[1], "--version")) {
#ifdef HAVE_CONFIG_H
      cout << endl << "musrRootValidation version: " << PACKAGE_VERSION << ", git-rev: " << GIT_REVISION << endl << endl;
#else
      cout << endl << "musrRootValidation git-rev: " << GIT_REVISION << endl << endl;
#endif
      return 0;
    } else {
      mrv_syntax();
      return -1;
    }
  }

  // filter out possible options
  bool quiet=false, keep=false;
  for (int i=1; i<argc; i++) {
    if (!strcmp(argv[i], "--quiet"))
      quiet = true;
    if (!strcmp(argv[i], "--keep"))
      keep = true;
  }

  PMusrRoot2Xml dump(argv[1], quiet, keep);
  if (!dump.IsValid()) {
    cerr << endl << "**ERROR** " << argv[1] << " is not a valid MusrRoot file." << endl << endl;
    return -1;
  }

  xmlDocPtr doc = xmlParseFile(dump.GetXmlDumpFileName().Data());
  if (doc == 0) {
    cerr << endl << "**ERROR** couldn't get xmlDocPtr for xml-file " << argv[1] << "." << endl << endl;
    return -1;
  }

  if (is_valid(doc, argv[2])) {
    cout << endl << "xml-file " << argv[1] << " validates against xml-schema " << argv[2] << endl << endl;
  } else {
    cerr << endl << "**ERROR** xml-file " << argv[1] << " fails to validate against xml-schema " << argv[2] << endl << endl;
  }

  // do some further consistency checks
  if (dump.GetNoOfDecayHistos() != dump.GetNoOfExpectedHistos()) {
    cerr << endl << "**ERROR** number of histogram found in the DecayAnaModule is inconsistent";
    cerr << endl << "  with the header; found " << dump.GetNoOfDecayHistos() << " histograms in the DecayAnaModule,";
    cerr << endl << "  but in the header: No of Histos = " << dump.GetNoOfHistos() << "; # RedGreen Offsets = " << dump.GetNoOfRedGreenOffsets();
    cerr << endl << endl;
  }

  if (dump.GetNoOfDecayHistos() != dump.GetNoOfDetectors()) {
    cerr << endl << "**ERROR** number of histogram found in the DecayAnaModule is inconsistent";
    cerr << endl << "  with number of Detector entries in the RunHeader.";
    cerr << endl << "  Found " << dump.GetNoOfDecayHistos() << " histograms in the DecayAnaModule,";
    cerr << endl << "  but " << dump.GetNoOfDetectors() << " number of Detector entries.";
    cerr << endl << endl;
  }

  return 0;
}
