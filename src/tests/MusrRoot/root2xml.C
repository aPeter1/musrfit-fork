// quick and dirty ROOT -> XML converter for MusrRoot
// needs to be rewritten as proper program.
//
// Andreas Suter
//
// $Id$

vector<string> xml_data;

enum EFolderTag {eUnkown, eDecayAnaModule, eSlowControlAnaModule};
enum ERunHeaderTag {eUnkown, eRunInfo, eSampleEnvironmentInfo, eMagneticFieldEnvironmentInfo, eBeamlineInfo, eScalerInfo};
EFolderTag folderTag = eUnkown;
ERunHeaderTag runHeaderTag = eUnkown;

void root2xml(const char *filename, const char *xmlFilename)
{
  TFile f(filename);

  if (f.IsZombie()) {
    cout << endl << "**ERROR** couldn't open file " << filename << endl;
    return;
  }

  xml_data.clear();

  xml_data.push_back("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
  xml_data.push_back("<MusrRoot xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"file:MusrRoot.xsd\">");

  TIter next = f.GetListOfKeys();
  TKey *key;
  TFolder *folder;
  TString str, tag;

  UInt_t offset = 2;

  while (key = (TKey*) next()) {
    cout << endl << "name: " << key->GetName() << ", class name: " << key->GetClassName();
    str = key->GetClassName();
    if (str == "TFolder") {      
      folder = (TFolder*)key->ReadObj();
      checkClass(folder, str, offset);
    }
  }
  cout << endl;

  f.Close();

  xml_data.push_back("</MusrRoot>");

  // the sort_histo_folders is needed since XML-Schema is not flexible enough to handle
  // histos -|
  //         |- DecayAnaModule
  //         ... (any other analyzer module sub-folder
  //         |- SCAnaModule
  // Hence SCAnaModule has artificially moved up, just to follow DecayAnaModule
  sort_histo_folders();

  ofstream fout(xmlFilename);

  for (UInt_t i=0; i<xml_data.size(); i++)
    fout << xml_data[i] << endl;
  fout.close();
}

void sort_histo_folders()
{
  vector<string> temp_xml_data;

  // first make a copy of the original xml_data
  for (unsigned int i=0; i<xml_data.size(); i++)
    temp_xml_data.push_back(xml_data[i]);

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
  temp_xml_data.insert(temp_xml_data.begin()+pos, xml_data.begin()+start, xml_data.begin()+end);

  // copy temp_xml_data back into xml_data
  xml_data.clear();
  for (unsigned int i=0; i<temp_xml_data.size(); i++)
    xml_data.push_back(temp_xml_data[i]);

  // clean up
  temp_xml_data.clear();
}

void dumpFolder(TFolder *folder, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  TIter next = folder->GetListOfFolders();
  TObject *obj;
  TString str;
  while (obj = (TObject*) next()) {
    cout << endl << offsetStr << "name: " << obj->GetName() << ", class name: " << obj->ClassName();
    str = obj->ClassName();
    checkClass(obj, str, offset);
  }

}

void dumpObjArray(TObjArray *obj, UInt_t offset)
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
    cout << endl << "debug>> xmlLablel=" << xmlLabel;
    if (xmlLabel.IsDigit())
      xmlLabel = "Detector";
    else
      xmlLabel = TString(obj->GetName());
  }

  cout << endl << offsetStr << obj->GetName() << " (# " << obj->GetEntries() << ")";

  xmlStr = offsetStr + "<" + xmlLabel + ">";
  xml_data.push_back(xmlStr.Data());

  for (UInt_t i=0; i<obj->GetEntries(); i++) {
    // check if entry is a TObjArray
    type = obj->At(i)->ClassName();
    if (type == "TObjArray") {
      dumpObjArray((TObjArray*)(obj->At(i)), offset+2);
    } else { // not a TObjArray
      tstr = (TObjString*) obj->At(i);
      str = tstr->GetString();
      str.Remove(TString::kTrailing, '\n');

      getType(str, type);
      getLabel(str, label);

      cout << endl << offsetStr << i << ": " << str;

      xmlStr = offsetStr + "  " + "<" + label + ">" + type + "</" + label + ">" ;
      xml_data.push_back(xmlStr.Data());
    }
  }

  xmlStr = offsetStr + "</" + xmlLabel + ">";
  xml_data.push_back(xmlStr.Data());
}

void dumpEntry(TObject *obj, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  TString nameTag(""), typeTag("");
  switch (folderTag) {
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

  TString str;

  str = offsetStr + "<" + nameTag + ">";
  str += obj->GetName();
  str += "</" + nameTag + ">";
  xml_data.push_back(str.Data());

  str = offsetStr + "<" + typeTag + ">";
  str += obj->ClassName();
  str += "</" + typeTag + ">";
  xml_data.push_back(str.Data());
}

void checkClass(TObject *obj, TString str, UInt_t offset)
{
  TString offsetStr="";
  for (UInt_t i=0; i<offset; i++)
    offsetStr += " ";

  if (str == "TFolder") {
    TString xmlTagName(TString(obj->GetName()));

    // set folder tag
    if (!xmlTagName.CompareTo("DecayAnaModule"))
      folderTag = eDecayAnaModule;
    else if (!xmlTagName.CompareTo("SCAnaModule"))
      folderTag = eSlowControlAnaModule;
    else if (!xmlTagName.CompareTo("SCAnaModule"))
      folderTag = eSlowControlAnaModule;
    else
      folderTag = eUnkown;

    offset += 2;    
    str = offsetStr + "<" + xmlTagName + ">";
    xml_data.push_back(str.Data());

    dumpFolder((TFolder*)obj, offset);

    str = offsetStr + "</" + xmlTagName + ">";
    xml_data.push_back(str.Data());
  } else if (str == "TObjArray") {
    offset += 2;
    dumpObjArray((TObjArray*)obj, offset);
  } else {
    // filter out the proper entry tag
    TString entryTag("");
    switch (folderTag) {
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
    xml_data.push_back(str.Data());
    dumpEntry((TObjArray*)obj, offset);
    str = offsetStr + "</" + entryTag + ">";
    xml_data.push_back(str.Data());
  }
}

void getType(TString entry, TString &type)
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

void getLabel(TString entry, TString &label)
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

  // replace spaces through underscores
  label.ReplaceAll(' ', '_');
}
