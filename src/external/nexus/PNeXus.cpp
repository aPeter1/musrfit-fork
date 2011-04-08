/***************************************************************************

  PNeXus.cpp

  Author: Andreas Suter
  e-mail: andreas.suter@psi.ch

  $Id$

***************************************************************************/

/***************************************************************************
 *   Copyright (C) 2007-2011 by Andreas Suter                              *
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

#include <cstdio>
#include <cstring>

#include "PNeXus.h"

#ifndef SIZE_FLOAT32
#    define SIZE_FLOAT32    4
#    define SIZE_FLOAT64    8
#    define SIZE_INT8       1
#    define SIZE_UINT8      1
#    define SIZE_INT16      2
#    define SIZE_UINT16     2
#    define SIZE_INT32      4
#    define SIZE_UINT32     4
#    define SIZE_INT64      8
#    define SIZE_UINT64     8
#    define SIZE_CHAR8      1
#    define SIZE_CHAR       1
#    define SIZE_UCHAR8     1
#    define SIZE_UCHAR      1
#    define SIZE_CHAR16     2
#    define SIZE_UCHAR16    2
#endif /* SIZE_FLOAT32 */

//-----------------------------------------------------------------------------------------------------
// PNeXus constructor
//-----------------------------------------------------------------------------------------------------
/**
 *
 */
PNeXus::PNeXus()
{
  Init();
}

//-----------------------------------------------------------------------------------------------------
// PNeXus constructor
//-----------------------------------------------------------------------------------------------------
/**
 *
 *
 * \param fileName
 */
PNeXus::PNeXus(const char* fileName)
{
  Init();
  fFileName = fileName;
  if (ReadFile(fileName) != NX_OK) {
    cerr << endl << fErrorMsg << " (error code=" << fErrorCode << ")" << endl << endl;
  } else {
    fIsValid = true;
  }
}

//-----------------------------------------------------------------------------------------------------
// PNeXus destructor
//-----------------------------------------------------------------------------------------------------
/**
 *
 */
PNeXus::~PNeXus()
{
  CleanUp();
}

//-----------------------------------------------------------------------------------------------------
// ReadFile (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on successfull reading
 * - NX_ERROR on error. The error code/message will give the details.
 *
 * \param fileName
 */
int PNeXus::ReadFile(const char *fileName)
{
  int status, ival;
  float fval;
  string str("");

  // open file
  status = NXopen(fileName, NXACC_READ, &fFileHandle);
  if (status != NX_OK) {
    fErrorCode = PNEXUS_FILE_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't open file "+string(fileName)+"!";
    return NX_ERROR;
  }

  // open group
  status = NXopengroup(fFileHandle, "run", "NXentry");
  if (status != NX_OK) {
    fErrorCode = PNEXUS_GROUP_OPEN_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Couldn't open NeXus group run!";
    return NX_ERROR;
  }

  // IDF
  if (NXopendata(fFileHandle, "idf_version") != NX_OK) {
    if (NXopendata(fFileHandle, "IDF_version") != NX_OK) {
      fErrorCode = PNEXUS_OPEN_DATA_ERROR;
      fErrorMsg  = "PNeXus::ReadFile() **ERROR** couldn't open 'IDF_version' nor 'idf_version' data!!";
      return NX_ERROR;
    }
  }
  if (!ErrorHandler(NXgetdata(fFileHandle, &fIDFVersion), PNEXUS_GET_DATA_ERROR, "couldn't read 'IDF_version' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'IDF_version' data")) return NX_ERROR;

  // program_name
  if (!ErrorHandler(NXopendata(fFileHandle, "program_name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'program_name' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(fProgramName), PNEXUS_GET_DATA_ERROR, "couldn't read 'program_name' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringAttr("version", fProgramVersion), PNEXUS_GET_ATTR_ERROR, "couldn't read program_name attribute!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'program_name' data")) return NX_ERROR;

  // run number
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fRunNumber), PNEXUS_GET_DATA_ERROR, "couldn't read 'number' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'number' data")) return NX_ERROR;

  // title
  if (!ErrorHandler(NXopendata(fFileHandle, "title"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'title' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(fRunTitle), PNEXUS_GET_DATA_ERROR, "couldn't read 'title' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'title' data")) return NX_ERROR;

  // notes
  if (!ErrorHandler(NXopendata(fFileHandle, "notes"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'notes' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(fNotes), PNEXUS_GET_DATA_ERROR, "couldn't read 'notes' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'notes' data")) return NX_ERROR;

  // analysis tag
  if (!ErrorHandler(NXopendata(fFileHandle, "analysis"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'analysis' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(fAnalysisTag), PNEXUS_GET_DATA_ERROR, "couldn't read 'analysis' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'analysis' data")) return NX_ERROR;

  // lab
  if (!ErrorHandler(NXopendata(fFileHandle, "lab"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'lab' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(fLab), PNEXUS_GET_DATA_ERROR, "couldn't read 'lab' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'lab' data")) return NX_ERROR;

  // beamline
  if (!ErrorHandler(NXopendata(fFileHandle, "beamline"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'beamline' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(fBeamLine), PNEXUS_GET_DATA_ERROR, "couldn't read 'beamline' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'beamline' data")) return NX_ERROR;

  // start time
  if (!ErrorHandler(NXopendata(fFileHandle, "start_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'start_time' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'start_time' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'start_time' data")) return NX_ERROR;
  size_t pos, pos1, pos2;
  pos1 = str.find("T");
  pos2 = str.find(" ");
  if ((pos1 == string::npos) && (pos2 == string::npos)) {
    fErrorCode = PNEXUS_GET_DATA_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Wrong start date-time found (" + str + ")!";
    return NX_ERROR;
  }
  if (pos1 != string::npos)
    pos = pos1;
  else
    pos = pos2;
  fStartDate = str.substr(0, pos);
  fStartTime = str.substr(pos+1, str.length());

  // stop time
  if (!ErrorHandler(NXopendata(fFileHandle, "stop_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'start_time' data!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'start_time' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'start_time' data")) return NX_ERROR;
  pos1 = str.find("T");
  pos2 = str.find(" ");
  if ((pos1 == string::npos) && (pos2 == string::npos)) {
    fErrorCode = PNEXUS_GET_DATA_ERROR;
    fErrorMsg  = "PNeXus::ReadFile() **ERROR** Wrong stop date-time found (" + str + ")!";
    return NX_ERROR;
  }
  if (pos1 != string::npos)
    pos = pos1;
  else
    pos = pos2;
  fStopDate = str.substr(0, pos);
  fStopTime = str.substr(pos+1, str.length());

  // switching state (red/green mode)
  if (!ErrorHandler(NXopendata(fFileHandle, "switching_states"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'switching_states' data!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fSwitchingState), PNEXUS_GET_DATA_ERROR, "couldn't read 'switching_states' data!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'switching_states' data")) return NX_ERROR;

  // open subgroup NXuser
  if (!ErrorHandler(NXopengroup(fFileHandle, "user", "NXuser"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup user!")) return NX_ERROR;

  // change to subgroup
  if (!ErrorHandler(NXinitgroupdir(fFileHandle), PNEXUS_INIT_GROUPDIR_ERROR, "couldn't init group directory")) return NX_ERROR;
  int numItems = 0;
  NXname groupName, className;
  if (!ErrorHandler(NXgetgroupinfo(fFileHandle, &numItems, groupName, className), PNEXUS_GET_GROUP_INFO_ERROR, "couldn't get user group info")) return NX_ERROR;

  // get all the user info
  // initialize the user data set
  fUser.fUserName = "";
  fUser.fExperimentNumber = "";
  // go through the user list and filter out the required items
  NXname nx_label;
  int    nx_dataType;
  for (int i=0; i<numItems; i++) {
    if (!ErrorHandler(NXgetnextentry(fFileHandle, nx_label, className, &nx_dataType), PNEXUS_GET_NEXT_ENTRY_ERROR, "couldn't get next entry")) return NX_ERROR;
    if (!ErrorHandler(NXopendata(fFileHandle, nx_label), PNEXUS_OPEN_DATA_ERROR, "couldn't open data")) return NX_ERROR;
    if (!strcmp(nx_label, "name")) {
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't get user name")) return NX_ERROR;
      fUser.fUserName = str;
    }
    if (!strcmp(nx_label, "experiment_number")) {
      if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't get user name")) return NX_ERROR;
      fUser.fExperimentNumber = str;
    }
    if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close data")) return NX_ERROR;
  }

  // close subgroup NXuser
  NXclosegroup(fFileHandle);

  // open subgroup NXsample
  if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXSample"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup sample!")) return NX_ERROR;

  // read sample name
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in sample group")) return NX_ERROR;
  fSample.fName = str;

  // read sample temperature
  if (!ErrorHandler(NXopendata(fFileHandle, "temperature"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'temperature' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'temperature' data in sample group!")) return NX_ERROR;
  fSample.fTemperature = (double)fval;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read temperature units!")) return NX_ERROR;
  fSample.fTemperatureUnit = str;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'temperature' data in sample group")) return NX_ERROR;

  // read sample magnetic field
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'magnetic_field' data in sample group!")) return NX_ERROR;
  fSample.fMagneticField = (double)fval;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read magnetic field units!")) return NX_ERROR;
  fSample.fMagneticFieldUnit = str;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'temperature' data in sample group")) return NX_ERROR;

  // read sample shape, e.g. powder, single crystal, etc.
  if (!ErrorHandler(NXopendata(fFileHandle, "shape"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'shape' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'shape' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'shape' data in sample group")) return NX_ERROR;
  fSample.fShape = str;

  // read magnetic field state, e.g. TF, LF, ZF
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_state"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_state' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'magnetic_field_state' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field_state' data in sample group")) return NX_ERROR;
  fSample.fMagneticFieldState = str;

  // read 'magnetic_field_vector' and 'coordinate_system' attribute
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_vector"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_vector' data in sample group!")) return NX_ERROR;
  int attLen = SIZE_INT32, attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "available", &fSample.fMagneticFieldVectorAvailable, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'magnetic_field_vector available' data in sample group!")) return NX_ERROR;
  if (fSample.fMagneticFieldVectorAvailable) {
    if (!ErrorHandler(GetDoubleVectorData(fSample.fMagneticFieldVector), PNEXUS_GET_DATA_ERROR, "couldn't get 'magnetic_field_vector' data!")) return NX_ERROR;
  }
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'magnetic_field_vector' data in sample group")) return NX_ERROR;

  // read sample environment, e.g. CCR, LowTemp-2, etc.
  if (!ErrorHandler(NXopendata(fFileHandle, "environment"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'environment' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'environment' data in sample group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'environment' data in sample group")) return NX_ERROR;
  fSample.fEnvironment = str;

  // close subgroup NXsample
  NXclosegroup(fFileHandle);

  // get required instrument information
  // open subgroup NXinstrument with subgroups detector, collimator, beam
  if (!ErrorHandler(NXopengroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup instrument!")) return NX_ERROR;

  // get instrument name
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'name' data in instrument group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'name' data in instrument group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'name' data in instrument group")) return NX_ERROR;
  fInstrument.name = str;

  // open subgroup NXdetector
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector", "NXdetector"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup detector!")) return NX_ERROR;

  // get number of detectors
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data in detector group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'number' data in detector group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'number' data in detector group")) return NX_ERROR;
  fInstrument.detector.number = ival;

  // close subgroup NXdetector
  NXclosegroup(fFileHandle);

  // open subgroup NXcollimator
  if (!ErrorHandler(NXopengroup(fFileHandle, "collimator", "NXcollimator"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup collimator!")) return NX_ERROR;

  // get collimator type
  if (!ErrorHandler(NXopendata(fFileHandle, "type"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'type' data in collimator group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringData(str), PNEXUS_GET_DATA_ERROR, "couldn't read 'type' data in collimator group!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'type' data in collimator group")) return NX_ERROR;
  fInstrument.collimator.type = str;

  // close subgroup NXcollimator
  NXclosegroup(fFileHandle);

  // open subgroup NXbeam
  if (!ErrorHandler(NXopengroup(fFileHandle, "beam", "NXbeam"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup beam!")) return NX_ERROR;

  // get the total counts
  if (!ErrorHandler(NXopendata(fFileHandle, "total_counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'total_counts' data in beam group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &fval), PNEXUS_GET_DATA_ERROR, "couldn't read 'total_counts' data in beam group!")) return NX_ERROR;
  fInstrument.beam.total_counts = (double)fval;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read total_counts units!")) return NX_ERROR;
  fInstrument.beam.total_counts_units = str;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'total_counts' data in beam group")) return NX_ERROR;

  // close subgroup NXbeam
  NXclosegroup(fFileHandle);

  // close subgroup NXinstrument
  NXclosegroup(fFileHandle);

  // open subgroup NXdata (this is for Version 1, only and is subject to change in the near future!)
  if (!ErrorHandler(NXopengroup(fFileHandle, "histogram_data_1", "NXdata"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open NeXus subgroup histogram_data_1!")) return NX_ERROR;

  // get time resolution
  if (!ErrorHandler(NXopendata(fFileHandle, "resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'resolution' data in histogram_data_1 group!")) return NX_ERROR;
  if (!ErrorHandler(NXgetdata(fFileHandle, &ival), PNEXUS_GET_DATA_ERROR, "couldn't read 'resolution' data in histogram_data_1 group!")) return NX_ERROR;
  if (!ErrorHandler(GetStringAttr("units", str), PNEXUS_GET_ATTR_ERROR, "couldn't read 'resolution'' units!")) return NX_ERROR;
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'resolution' data in histogram_data_1 group")) return NX_ERROR;
  if (!strcmp(str.data(), "picoseconds")) {
    fData.fTimeResolution = (double)ival/1000.0; // time resolution in (ns)
  }

  // get data, t0, first good bin, last good bin, data
  if (!ErrorHandler(NXopendata(fFileHandle, "counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'counts' data in histogram_data_1 group!")) return NX_ERROR;
  int histoLength=0;

  // get number of histos
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "number", &fData.fNumberOfHistos, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data in histogram_data_1 group!")) return NX_ERROR;

  // get histo length
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "length", &histoLength, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'number' data in histogram_data_1 group!")) return NX_ERROR;

  // get t0
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "t0_bin", &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 't0_bin' data in histogram_data_1 group!")) return NX_ERROR;
  fData.fT0.push_back(ival);

  // get first good bin
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "first_good_bin", &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'first_good_bin' data in histogram_data_1 group!")) return NX_ERROR;
  fData.fFirstGoodBin.push_back(ival);

  // get last good bin
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "last_good_bin", &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'last_good_bin' data in histogram_data_1 group!")) return NX_ERROR;
  fData.fLastGoodBin.push_back(ival);

  // get data

  // get information of the current nexus entity
  int rank, type, dims[32], size, noOfElements;
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get data info!")) return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  int *i_data_ptr = (int*) data_ptr;
  status = NXgetdata(fFileHandle, i_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  // check that the amount of data is consistent with the attribute information
  if (noOfElements != (int)fData.fNumberOfHistos * histoLength) {
    fErrorCode = PNEXUS_HISTO_ERROR;
    fErrorMsg = "inconsistent histogram info!";
    return NX_ERROR;
  }


  // copy the data into the vector
  fData.fHisto.clear();
  vector<unsigned int> data;
  for (int i=0; i<noOfElements; i++) {
    if ((i % histoLength == 0) && (i>0)) {
      fData.fHisto.push_back(data);
      data.clear();
      data.push_back(*(i_data_ptr+i));
    } else {
      data.push_back(*(i_data_ptr+i));
    }
  }
  fData.fHisto.push_back(data);
  data.clear();

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
  }

  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'counts' data in histogram_data_1 group")) return NX_ERROR;

  // get grouping
  if (!ErrorHandler(NXopendata(fFileHandle, "grouping"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'grouping' data in histogram_data_1 group!")) return NX_ERROR;
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "available", &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'grouping available' data in histogram_data_1 group!")) return NX_ERROR;
  if (ival) {
    vector<int> grouping;
    if (!ErrorHandler(GetIntVectorData(grouping), PNEXUS_GET_DATA_ERROR, "couldn't read 'grouping' data in histogram_data_1 group!")) return NX_ERROR;
    for (unsigned int i=0; i<grouping.size(); i++)
      fData.fGrouping.push_back(grouping[i]);
    grouping.clear();
  }
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'grouping' data in histogram_data_1 group")) return NX_ERROR;

  // get alpha
  if (!ErrorHandler(NXopendata(fFileHandle, "alpha"), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'alpha' data in histogram_data_1 group!")) return NX_ERROR;
  attLen = SIZE_INT32;
  attType = NX_INT32;
  if (!ErrorHandler(NXgetattr(fFileHandle, "available", &ival, &attLen, &attType), PNEXUS_OPEN_DATA_ERROR, "couldn't open 'alpha available' data in histogram_data_1 group!")) return NX_ERROR;
  if (ival) {
    if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get alpha info!")) return NX_ERROR;
    // calculate the needed size
    size = dims[0];
    for (int i=1; i<rank; i++)
      size *= dims[i];
    noOfElements = size;
    size *= GetDataSize(type);

    // check that noOfElements is a multiple of 3: grp_1, grp_2, alpha_12, etc.
    if ((3*(noOfElements/3)-noOfElements) != 0) {
      fErrorCode = PNEXUS_GET_META_INFO_ERROR;
      fErrorMsg  = "PNeXus::ReadFile(): **ERROR** alpha NeXus structure (grp_1, grp_2, alpha_12, etc.) violation.";
      return NX_ERROR;
    }

    // allocate locale memory to get the data
    char *data_ptr = new char[size];
    if (data_ptr == 0) {
      return NX_ERROR;
    }

    // get the data
    float *f_data_ptr = (float*) data_ptr;
    status = NXgetdata(fFileHandle, f_data_ptr);
    if (status != NX_OK) {
      return NX_ERROR;
    }

    // copy the data into the vector
    fData.fAlpha.clear();
    PNeXusAlpha alpha;
    for (int i=0; i<noOfElements; i+=3) {
      fval = *(f_data_ptr+i);
      alpha.fGroupFirst  = (unsigned int) fval;
      fval = *(f_data_ptr+i+1);
      alpha.fGroupSecond = (unsigned int) fval;
      alpha.fAlphaVal    = *(f_data_ptr+i+2);
      fData.fAlpha.push_back(alpha);
    }

    // clean up
    if (data_ptr) {
      delete [] data_ptr;
    }
  }
  if (!ErrorHandler(NXclosedata(fFileHandle), PNEXUS_CLOSE_DATA_ERROR, "couldn't close 'alpha' data in histogram_data_1 group")) return NX_ERROR;

  // close subgroup NXdata
  NXclosegroup(fFileHandle);

  // close group run
  NXclosegroup(fFileHandle);

  // close file
  NXclose(&fFileHandle);

  GroupHistoData();

  fIsValid = true;

  return true;
}

//-----------------------------------------------------------------------------------------------------
// WriteFile (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on successfull writing
 * - NX_ERROR on error. The error code/message will give the details.
 *
 * \param fileName
 * \param fileType, allowed types are: hdf4, hdf5, xml
 */
int PNeXus::WriteFile(const char *fileName, const char *fileType)
{
  char  str[256];
  int   size, idata;
  float fdata;
  NXaccess access=NXACC_CREATE4;

  if (!strcmp(fileType, "hdf4"))
    access=NXACC_CREATE4;
  else if (!strcmp(fileType, "hdf5"))
    access=NXACC_CREATE5;
  else if (!strcmp(fileType, "xml"))
    access=NXACC_CREATEXML;
  else
    access=NXACC_CREATE4;

  sprintf(str, "couldn't open file '%s' for writing", fileName);
  if (!ErrorHandler(NXopen(fileName, access, &fFileHandle), PNEXUS_FILE_OPEN_ERROR, str)) return NX_ERROR;

  // write NXfile attributes (NeXus_version, user)
  strncpy(str, fUser.fUserName.data(), sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "user", str, strlen(str), NX_CHAR), PNEXUS_SET_ATTR_ERROR, "couldn't set NXfile attributes")) return NX_ERROR;

  // make group 'run'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "run", "NXentry"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'run'.")) return NX_ERROR;
  // open group 'run'
  if (!ErrorHandler(NXopengroup(fFileHandle, "run", "NXentry"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'run' for writting.")) return NX_ERROR;

  // write IDF_version
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "IDF_version", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'IDF_version'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "IDF_version"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'IDF_version' for writting.")) return NX_ERROR;
  idata = fIDFVersion;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'IDF_version'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write program_name, and attribute version
  size = fProgramName.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "program_name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'program_name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "program_name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'program_name' for writting.")) return NX_ERROR;
  strncpy(str, fProgramName.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'program_name'.")) return NX_ERROR;
  strncpy(str, fProgramVersion.data(), sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "version", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'version' for 'program_name'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'number'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "number", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'number'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'number' for writting.")) return NX_ERROR;
  idata = fRunNumber;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'number'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'title'
  size = fRunTitle.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "title", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'title'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "title"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'title' for writting.")) return NX_ERROR;
  strncpy(str, fRunTitle.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'title'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'notes'
  size = fNotes.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "notes", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'notes'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "notes"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'notes' for writting.")) return NX_ERROR;
  strncpy(str, fNotes.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'notes'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'analysis'
  size = fAnalysisTag.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "analysis", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'analysis'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "analysis"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'analysis' for writting.")) return NX_ERROR;
  strncpy(str, fAnalysisTag.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'analysis'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'lab'
  size = fLab.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "lab", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'lab'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "lab"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'lab' for writting.")) return NX_ERROR;
  strncpy(str, fLab.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'lab'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'beamline'
  size = fBeamLine.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "beamline", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'beamline'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "beamline"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'beamline' for writting.")) return NX_ERROR;
  strncpy(str, fBeamLine.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'beamline'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'start_time'
  snprintf(str, sizeof(str), "%s %s", fStartDate.data(), fStartTime.data());
  size = strlen(str);
  if (!ErrorHandler(NXmakedata(fFileHandle, "start_time", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'start_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "start_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'start_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'start_time'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'end_time'
  snprintf(str, sizeof(str), "%s %s", fStopDate.data(), fStopTime.data());
  size = strlen(str);
  if (!ErrorHandler(NXmakedata(fFileHandle, "end_time", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'end_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "end_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'end_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'end_time'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write run 'switching_states'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "switching_states", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'switching_states'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "switching_states"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'switching_states' for writting.")) return NX_ERROR;
  idata = fSwitchingState;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'switching_states'.")) return NX_ERROR;
  NXclosedata(fFileHandle);


  // make group 'user'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "user", "NXuser"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'user'.")) return NX_ERROR;
  // open group 'user'
  if (!ErrorHandler(NXopengroup(fFileHandle, "user", "NXuser"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'user' for writting.")) return NX_ERROR;

  // write user 'name'
  size = fUser.fUserName.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' for writting.")) return NX_ERROR;
  strncpy(str, fUser.fUserName.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write user 'experiment_number'
  size = fUser.fExperimentNumber.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "experiment_number", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'experiment_number'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "experiment_number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'experiment_number' for writting.")) return NX_ERROR;
  strncpy(str, fUser.fExperimentNumber.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'experiment_number'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'user'
  NXclosegroup(fFileHandle);

  // make group 'sample'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "sample", "NXuser"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'sample'.")) return NX_ERROR;
  // open group 'sample'
  if (!ErrorHandler(NXopengroup(fFileHandle, "sample", "NXuser"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'sample' for writting.")) return NX_ERROR;

  // write sample 'name'
  size = fSample.fName.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' for writting.")) return NX_ERROR;
  strncpy(str, fSample.fName.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'temperature'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "temperature", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'temperature'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "temperature"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'temperature' for writting.")) return NX_ERROR;
  fdata = fSample.fTemperature;
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'temperature'.")) return NX_ERROR;
  strncpy(str, fSample.fTemperatureUnit.data(), sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'temperature'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'magnetic_field'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'magnetic_field'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'magnetic_field' for writting.")) return NX_ERROR;
  fdata = fSample.fMagneticField;
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'magnetic_field'.")) return NX_ERROR;
  strncpy(str, fSample.fMagneticFieldUnit.data(), sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'magnetic_field'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'shape'
  size = fSample.fShape.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "shape", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'shape'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "shape"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'shape' for writting.")) return NX_ERROR;
  strncpy(str, fSample.fShape.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'shape'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'magnetic_field_state'
  size = fSample.fMagneticFieldState.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "magnetic_field_state", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'magnetic_field_state'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "magnetic_field_state"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'magnetic_field_state' for writting.")) return NX_ERROR;
  strncpy(str, fSample.fMagneticFieldState.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'magnetic_field_state'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write sample 'magnetic_field_vector'
  // !! TO BE DONE !!

  // write sample 'environment'
  size = fSample.fEnvironment.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "environment", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'environment'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "environment"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'environment' for writting.")) return NX_ERROR;
  strncpy(str, fSample.fEnvironment.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'environment'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'sample'
  NXclosegroup(fFileHandle);

  // make group 'instrument'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'instrument'.")) return NX_ERROR;
  // open group 'instrument'
  if (!ErrorHandler(NXopengroup(fFileHandle, "instrument", "NXinstrument"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'instrument' for writting.")) return NX_ERROR;

  // write instrument 'name'
  size = fInstrument.name.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "name", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'name'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "name"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'name' for writting.")) return NX_ERROR;
  strncpy(str, fInstrument.name.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'name'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // make group 'detector'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "detector", "NXdetector"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'detector'.")) return NX_ERROR;
  // open group 'detector'
  if (!ErrorHandler(NXopengroup(fFileHandle, "detector", "NXdetector"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'detector' for writting.")) return NX_ERROR;

  // write detector 'number'
  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "number", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'number'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "number"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'number' for writting.")) return NX_ERROR;
  idata = fInstrument.detector.number;
  if (!ErrorHandler(NXputdata(fFileHandle, &idata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'number'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'detector'
  NXclosegroup(fFileHandle);

  // make group 'collimator'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "collimator", "NXcollimator"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'collimator'.")) return NX_ERROR;
  // open group 'collimator'
  if (!ErrorHandler(NXopengroup(fFileHandle, "collimator", "NXcollimator"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'collimator' for writting.")) return NX_ERROR;

  // write collimator 'type'
  size = fInstrument.collimator.type.length();
  if (!ErrorHandler(NXmakedata(fFileHandle, "type", NX_CHAR, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'type'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "type"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'type' for writting.")) return NX_ERROR;
  strncpy(str, fInstrument.collimator.type.data(), sizeof(str));
  if (!ErrorHandler(NXputdata(fFileHandle, str), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'type'.")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'collimator'
  NXclosegroup(fFileHandle);

  // make group 'beam'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "beam", "NXbeam"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'beam'.")) return NX_ERROR;
  // open group 'beam'
  if (!ErrorHandler(NXopengroup(fFileHandle, "beam", "NXbeam"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'beam' for writting.")) return NX_ERROR;

  size = 1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "total_counts", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'total_counts'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "total_counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'total_counts' for writting.")) return NX_ERROR;
  fdata = fInstrument.beam.total_counts;
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'total_counts'.")) return NX_ERROR;
  strncpy(str, fInstrument.beam.total_counts_units.data(), sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'total_counts'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // close group 'beam'
  NXclosegroup(fFileHandle);

  // close group 'instrument'
  NXclosegroup(fFileHandle);

  // make group 'histogram_data_1'
  if (!ErrorHandler(NXmakegroup(fFileHandle, "histogram_data_1", "NXdata"), PNEXUS_CREATE_GROUP_ERROR, "couldn't create group 'histogram_data_1'.")) return NX_ERROR;
  // open group 'histogram_data_1'
  if (!ErrorHandler(NXopengroup(fFileHandle, "histogram_data_1", "NXdata"), PNEXUS_GROUP_OPEN_ERROR, "couldn't open group 'histogram_data_1' for writting.")) return NX_ERROR;

  // write data 'counts'
  int *histo_data=0;
  int histo_size[2];
  if (fData.fHisto.size() == 0) {
    histo_data = new int[1];
    histo_data[0] = -1;
    histo_size[0] = 1;
    histo_size[1] = 1;
  } else {
    histo_data = new int[fData.fHisto.size()*fData.fHisto[0].size()];
    for (unsigned int i=0; i<fData.fHisto.size(); i++)
      for (unsigned int j=0; j<fData.fHisto[0].size(); j++)
        histo_data[i*fData.fHisto[0].size()+j] = fData.fHisto[i][j];
    histo_size[0] = fData.fHisto.size();
    histo_size[1] = fData.fHisto[0].size();
  }

  if (!ErrorHandler(NXmakedata(fFileHandle, "counts", NX_INT32, 2, histo_size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'counts'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "counts"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'counts' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)histo_data), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'counts'.")) return NX_ERROR;
  strncpy(str, "counts", sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'counts'")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "signal", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'signal' for 'counts'")) return NX_ERROR;
  idata = fData.fHisto.size();
  if (!ErrorHandler(NXputattr(fFileHandle, "number", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'number' for 'counts'")) return NX_ERROR;
  idata = fData.fT0[0];
  if (!ErrorHandler(NXputattr(fFileHandle, "T0_bin", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'T0_bin' for 'counts'")) return NX_ERROR;
  idata = fData.fFirstGoodBin[0];
  if (!ErrorHandler(NXputattr(fFileHandle, "first_good_bin", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'first_good_bin' for 'counts'")) return NX_ERROR;
  idata = fData.fLastGoodBin[0];
  if (!ErrorHandler(NXputattr(fFileHandle, "last_good_bin", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'last_good_bin' for 'counts'")) return NX_ERROR;
  fdata = 1.0e3*fData.fTimeResolution/2.0;
  if (!ErrorHandler(NXputattr(fFileHandle, "offset", &fdata, 1, NX_FLOAT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'offset' for 'counts'")) return NX_ERROR;

  if (histo_data) {
    delete [] histo_data;
  }
  NXclosedata(fFileHandle);

  // write data 'histogram_resolution'
  if (!ErrorHandler(NXmakedata(fFileHandle, "histogram_resolution", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'histogram_resolution'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "histogram_resolution"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'histogram_resolution' for writting.")) return NX_ERROR;
  fdata = fData.fTimeResolution * 1.0e3; // ns -> ps
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'histogram_resolution'.")) return NX_ERROR;
  strncpy(str, "picoseconds", sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'histogram_resolution'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write data 'time_zero' always set to 0 since the T0_bin attribute of counts is relevant only
  if (!ErrorHandler(NXmakedata(fFileHandle, "time_zero", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'time_zero'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "time_zero"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'time_zero' for writting.")) return NX_ERROR;
  fdata = 0.0;
  if (!ErrorHandler(NXputdata(fFileHandle, &fdata), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'time_zero'.")) return NX_ERROR;
  strncpy(str, "microseconds", sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'time_zero'")) return NX_ERROR;
  idata = 0;
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'time_zero'")) return NX_ERROR;
  NXclosedata(fFileHandle);

  // write data 'raw_time'
  if (fData.fHisto.size() == 0) {
    fErrorCode = PNEXUS_HISTO_ERROR;
    fErrorMsg  = "no data for writing present.";
    return NX_ERROR;
  }
  float *raw_time = new float[fData.fHisto[0].size()-1];
  for (unsigned int i=0; i<fData.fHisto[0].size()-1; i++) {
    raw_time[i] = fData.fTimeResolution * (float)i / 1.0e3; // raw time in (us)
  }
  size = fData.fHisto[0].size()-1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "raw_time", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'raw_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "raw_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'raw_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, raw_time), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'raw_time'.")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "axis", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'axis' for 'raw_time'")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "primary", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'primary' for 'raw_time'")) return NX_ERROR;
  strncpy(str, "microseconds", sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'raw_time'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (raw_time) {
    delete [] raw_time;
    raw_time = 0;
  }

  // write data 'corrected_time'
  float *corrected_time = new float[fData.fHisto[0].size()-1];
  for (unsigned int i=0; i<fData.fHisto[0].size()-1; i++) {
    corrected_time[i] = fData.fTimeResolution * (float)((int)i-(int)fData.fT0[0]+1) / 1.0e3; // raw time in (us)
  }
  size = fData.fHisto[0].size()-1;
  if (!ErrorHandler(NXmakedata(fFileHandle, "corrected_time", NX_FLOAT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'corrected_time'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "corrected_time"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'corrected_time' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, corrected_time), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'corrected_time'.")) return NX_ERROR;
  idata = 1;
  if (!ErrorHandler(NXputattr(fFileHandle, "axis", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'axis' for 'corrected_time'")) return NX_ERROR;
  strncpy(str, "microseconds", sizeof(str));
  if (!ErrorHandler(NXputattr(fFileHandle, "units", str, strlen(str), NX_CHAR), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'units' for 'corrected_time'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (corrected_time) {
    delete [] corrected_time;
    corrected_time = 0;
  }

  // write data 'grouping'
  int *grouping = new int[fData.fHisto.size()];
  vector<int> groupNo; // keep the number of different groupings
  if (fData.fHisto.size() == fData.fGrouping.size()) { // grouping vector seems to be properly defined
    bool found;
    groupNo.push_back(fData.fGrouping[0]);
    for (unsigned int i=0; i<fData.fHisto.size(); i++) {
      grouping[i] = fData.fGrouping[i];
      found = false;
      for (unsigned int j=0; j<groupNo.size(); j++) {
        if ((int)fData.fGrouping[i] == groupNo[j]) {
          found = true;
          break;
        }
      }
      if (!found) {
        groupNo.push_back(fData.fGrouping[i]);
      }
    }
  } else { // grouping vector not available
    for (unsigned int i=0; i<fData.fHisto.size(); i++) {
      grouping[i] = 0;
    }
  }
  size = fData.fHisto.size();
  if (!ErrorHandler(NXmakedata(fFileHandle, "grouping", NX_INT32, 1, &size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'grouping'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "grouping"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'grouping' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, grouping), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'grouping'.")) return NX_ERROR;
  idata = groupNo.size();
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'grouping'")) return NX_ERROR;
  NXclosedata(fFileHandle);
  if (grouping) {
    delete [] grouping;
    grouping = 0;
  }
  groupNo.clear();

  // write data 'alpha'
  float *alpha;
  int array_size[2];
  if (fData.fAlpha.size() == 0) {
    alpha = new float[3];
    alpha[0] = -1.0;
    alpha[1] = -1.0;
    alpha[2] = -1.0;
    array_size[0] = 1;
    array_size[1] = 3;
  } else {
    alpha = new float[fData.fAlpha.size()*3];
    for (unsigned int i=0; i<fData.fAlpha.size(); i++) {
      alpha[i] = (float)fData.fAlpha[i].fGroupFirst;
      alpha[i+1] = (float)fData.fAlpha[i].fGroupSecond;
      alpha[i+2] = (float)fData.fAlpha[i].fAlphaVal;
    }
    array_size[0] = fData.fAlpha.size();
    array_size[1] = 3;
  }
  if (!ErrorHandler(NXmakedata(fFileHandle, "alpha", NX_FLOAT32, 2, array_size), PNEXUS_MAKE_DATA_ERROR, "couldn't create data entry 'alpha'.")) return NX_ERROR;
  if (!ErrorHandler(NXopendata(fFileHandle, "alpha"), PNEXUS_OPEN_DATA_ERROR, "couldn't open data 'alpha' for writting.")) return NX_ERROR;
  if (!ErrorHandler(NXputdata(fFileHandle, (void*)alpha), PNEXUS_PUT_DATA_ERROR, "couldn't put data 'alpha'.")) return NX_ERROR;
  idata = fData.fAlpha.size();
  if (!ErrorHandler(NXputattr(fFileHandle, "available", &idata, 1, NX_INT32), PNEXUS_PUT_ATTR_ERROR, "couldn't put attribute 'available' for 'alpha'")) return NX_ERROR;
  if (alpha) {
    delete [] alpha;
  }

  // close group 'histogram_data_1'
  NXclosegroup(fFileHandle);

  // close group 'run'
  NXclosegroup(fFileHandle);

  NXclose(&fFileHandle);
  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// WriteFile (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>Dump read data to the standard output.
 */
void PNeXus::Dump()
{
  cout << endl << "-----------------------------";
  cout << endl << " NeXus Dump";
  cout << endl << "-----------------------------";
  cout << endl;
  cout << endl << "file name       : " << fFileName;
  cout << endl << "IDF version     : " << fIDFVersion;
  cout << endl << "program name    : " << fProgramName << ", version : " << fProgramVersion;
  cout << endl << "run number      : " << fRunNumber;
  cout << endl << "run title       : " << fRunTitle;
  cout << endl << "run notes       : " << fNotes;
  cout << endl << "analysis        : " << fAnalysisTag;
  cout << endl << "lab             : " << fLab;
  cout << endl << "beamline        : " << fBeamLine;
  cout << endl << "start date/time : " << fStartDate << "/" << fStartTime;
  cout << endl << "stop  date/time : " << fStopDate << "/" << fStopTime;
  cout << endl << "switching state : " << fSwitchingState << " (red/green mode)";
  cout << endl << "-----------------------------";
  cout << endl << "user info";
  cout << endl << "user            : " << fUser.fUserName;
  cout << endl << "exp. number     : " << fUser.fExperimentNumber;
  cout << endl << "-----------------------------";
  cout << endl << "sample info";
  cout << endl << "sample name     : " << fSample.fName;
  cout << endl << "temperature     : " << fSample.fTemperature << " (" << fSample.fTemperatureUnit << ")";
  cout << endl << "magnetic state  : " << fSample.fMagneticFieldState;
  cout << endl << "magnetic field  : " << fSample.fMagneticField << " (" << fSample.fMagneticFieldUnit << ")";
  cout << endl << "mag. vector     : ";
  if (fSample.fMagneticFieldVectorAvailable) {
    for (unsigned int i=0; i<fSample.fMagneticFieldVector.size(); i++) {
      cout << fSample.fMagneticFieldVector[i] << ", ";
    }
    cout << "(" << fSample.fMagneticFieldVectorUnits << "), coord.system: " << fSample.fMagneticFieldVectorCoordinateSystem;
  } else {
    cout << "not available.";
  }
  cout << endl << "shape           : " << fSample.fShape;
  cout << endl << "environment     : " << fSample.fEnvironment;
  cout << endl << "-----------------------------";
  cout << endl << "instrument info";
  cout << endl << "instrument name : " << fInstrument.name;
  cout << endl << "   detector info";
  cout << endl << "   detector number : " << fInstrument.detector.number;
  cout << endl << "   -----------------------------";
  cout << endl << "   collimator info";
  cout << endl << "   collimator type : " << fInstrument.collimator.type;
  cout << endl << "   -----------------------------";
  cout << endl << "   beam info";
  cout << endl << "   beam total number of counts : " << fInstrument.beam.total_counts << " " << fInstrument.beam.total_counts_units;
  cout << endl << "-----------------------------";
  cout << endl << "run data";
  cout << endl << "time resolution : " << fData.fTimeResolution << " (ns)";
  cout << endl << "number of histos: " << fData.fNumberOfHistos;
  if (fData.fAlpha.size() != 0) {
    cout << endl << "alpha           : ";
    for (unsigned int i=0; i<fData.fAlpha.size(); i++) {
      cout << "(" << fData.fAlpha[i].fGroupFirst << "/" << fData.fAlpha[i].fGroupSecond << "/" << fData.fAlpha[i].fAlphaVal << "), ";
    }
  } else {
    cout << endl << "alpha           : not available";
  }
  if (fData.fGrouping.size() != 0) {
    cout << endl << "grouping        : ";
    for (unsigned int i=0; i<fData.fGrouping.size(); i++) {
      cout << "(" << i << "/" << fData.fGrouping[i] << "), ";
    }
  } else {
    cout << endl << "grouping        : not available";
  }
  cout << endl << "t0              : " << fData.fT0[0];
  cout << endl << "first good bin  : " << fData.fFirstGoodBin[0];
  cout << endl << "last good bin   : " << fData.fLastGoodBin[0];
  cout << endl << "histograms      : +++++++++++";
  for (unsigned int i=0; i<fData.fHisto.size(); i++) {
    cout << endl << "histo " << i+1 << ": ";
    for (unsigned int j=0; j<15; j++) {
      cout << fData.fHisto[i][j] << ", ";
    }
    cout << "...";
  }
  cout << endl << "-----------------------------";
  cout << endl << "grouped histograms : ++++++++";
  for (unsigned int i=0; i<fGroupedData.size(); i++) {
    cout << endl << "grouped histo " << i+1 << ": ";
    for (unsigned int j=0; j<15; j++) {
      cout << fGroupedData[i][j] << ", ";
    }
    cout << "...";
  }
  cout << endl << "-----------------------------";
  cout << endl << "done.";
  cout << endl << endl;

}

//-----------------------------------------------------------------------------------------------------
// SetStartDate (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>The date should look like yyyy-mm-dd.
 *
 * <b>return:</b>
 * - true on date string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param date
 */
bool PNeXus::SetStartDate(const char *date)
{
  string strDate = date;
  return SetStartDate(strDate);
}

//-----------------------------------------------------------------------------------------------------
// SetStartDate (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>The date should look like yyyy-mm-dd.
 *
 * <b>return:</b>
 * - true on date string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param date
 */
bool PNeXus::SetStartDate(string date)
{
  bool ok=false;
  string str = TransformDate(date, ok);

  if (!ok)
    return false;

  fStartDate = str;

  return true;
}

//-----------------------------------------------------------------------------------------------------
// SetStartTime (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>The time should look like hh:mm:ss.
 *
 * <b>return:</b>
 * - true on time string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param time
 */
bool PNeXus::SetStartTime(const char *time)
{
  string strTime = time;
  return SetStartTime(strTime);
}

//-----------------------------------------------------------------------------------------------------
// SetStartTime (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>The time should look like hh:mm:ss.
 *
 * <b>return:</b>
 * - true on time string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param time
 */
bool PNeXus::SetStartTime(string time)
{
  if (time.length() != 8) {
    fErrorCode = PNEXUS_WRONG_TIME_FORMAT;
    fErrorMsg = "PNeXus::SetStartTime **ERROR** given time="+time+", is not of the required form hh:mm:ss!";
    return false;
  }

  if ((time[2] != ':') || (time[5] != ':')) {
    fErrorCode = PNEXUS_WRONG_TIME_FORMAT;
    fErrorMsg = "PNeXus::SetStartTime **ERROR** given time="+time+", is not of the required form hh:mm:ss!";
    return false;
  }

  fStartTime = time;

  return true;
}

//-----------------------------------------------------------------------------------------------------
// SetStopDate (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - true on date string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param date
 */
bool PNeXus::SetStopDate(const char *date)
{
  string strDate = date;
  return SetStopDate(strDate);
}

//-----------------------------------------------------------------------------------------------------
// SetStopDate (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - true on date string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param date
 */
bool PNeXus::SetStopDate(string date)
{
  bool ok=false;
  string str = TransformDate(date, ok);

  if (!ok)
    return false;

  fStopDate = str;

  return true;
}

//-----------------------------------------------------------------------------------------------------
// SetStopTime (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - true on time string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param time
 */
bool PNeXus::SetStopTime(const char *time)
{
  string strTime = time;
  return SetStopTime(strTime);
}

//-----------------------------------------------------------------------------------------------------
// SetStopTime (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - true on time string has proper format
 * - false on error. The error code/message will give the details.
 *
 * \param time
 */
bool PNeXus::SetStopTime(string time)
{
  if (time.length() != 8) {
    fErrorCode = PNEXUS_WRONG_TIME_FORMAT;
    fErrorMsg = "PNeXus::SetStopTime **ERROR** given time="+time+", is not of the required form hh:mm:ss!";
    return false;
  }

  if ((time[2] != ':') || (time[5] != ':')) {
    fErrorCode = PNEXUS_WRONG_TIME_FORMAT;
    fErrorMsg = "PNeXus::SetStopTime **ERROR** given time="+time+", is not of the required form hh:mm:ss!";
    return false;
  }

  fStopTime = time;

  return true;
}

//-----------------------------------------------------------------------------------------------------
// CleanUp (private)
//-----------------------------------------------------------------------------------------------------
/**
 *
 */
void PNeXus::CleanUp()
{
  fData.fTimeResolution = 0.0;
  fData.fNumberOfHistos = 0;
  fData.fAlpha.clear();
  fData.fGrouping.clear();
  fData.fFirstGoodBin.clear();
  fData.fLastGoodBin.clear();
  fData.fT0.clear();
  fData.fHistoName.clear();
  for (unsigned int i=0; i<fData.fHisto.size(); i++)
    fData.fHisto[i].clear();
  fData.fHisto.clear();
}

//-----------------------------------------------------------------------------------------------------
// Init (private)
//-----------------------------------------------------------------------------------------------------
/**
 *
 */
void PNeXus::Init()
{
  fIsValid = false;

  fFileName    = "n/a";
  fFileHandle  = 0;

  fErrorMsg    = "No Data available!";
  fErrorCode   = -1;

  fIDFVersion  = -1;
  fProgramName = "n/a";
  fProgramVersion = "n/a";

  fRunNumber   = -1;
  fRunTitle    = "n/a";
  fNotes       = "n/a";
  fAnalysisTag = "n/a";
  fLab         = "n/a";
  fBeamLine    = "n/a";
  fStartDate   = "n/a";
  fStartTime   = "n/a";
  fStopDate    = "n/a";
  fStopTime    = "n/a";

  fSwitchingState = -1;

  fSample.fEnvironment = "n/a";
  fSample.fMagneticField = 9.9e17;
  fSample.fMagneticFieldUnit = "n/a";
  fSample.fMagneticFieldState = "n/a";
  fSample.fName = "n/a";
  fSample.fShape = "n/a";
  fSample.fTemperature = 9.9e17;
  fSample.fTemperatureUnit = "n/a";

  fInstrument.name = "n/a";
  fInstrument.detector.number = 0;
  fInstrument.collimator.type = "n/a";
  fInstrument.beam.total_counts = 0;
  fInstrument.beam.total_counts_units = "n/a";

  fData.fNumberOfHistos = 0;
  fData.fTimeResolution = 0.0;
  fData.fAlpha.clear();
  fData.fFirstGoodBin.clear();
  fData.fLastGoodBin.clear();
  fData.fHistoName.clear();
  fData.fGrouping.clear();
  fData.fT0.clear();
}

//-----------------------------------------------------------------------------------------------------
// ErrorHandler (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - true of no error occurred
 * - false on error
 *
 * \param status of the calling routine
 * \param errCode will set the fErrorCode of the class
 * \param errMsg will set the fErrorMsg of the class
 */
bool PNeXus::ErrorHandler(NXstatus status, int errCode, string errMsg)
{
  if (status != NX_OK) {
    fErrorCode = errCode;
    fErrorMsg = errMsg;
    if (fFileHandle != 0) {
      CleanUp();
      NXclose(&fFileHandle);
    }
    return false;
  }
  return true;
}

//-----------------------------------------------------------------------------------------------------
// GetStringData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param str string to be fed
 */
NXstatus PNeXus::GetStringData(string &str)
{
  int i, status, rank, type, dims[32];
  char cstr[VGNAMELENMAX];
  NXname data_value;

  status = NXgetinfo(fFileHandle, &rank, dims, &type);
  if (status != NX_OK) {
    fErrorCode = PNEXUS_GET_META_INFO_ERROR;
    fErrorMsg = "PNeXus::GetStringData() **ERROR** couldn't get meta info!";
    return NX_ERROR;
  }
  if ((type != NX_CHAR) || (rank > 1) || (dims[0] >= VGNAMELENMAX)) {
    fErrorCode = PNEXUS_WRONG_META_INFO;
    fErrorMsg = "PNeXus::GetStringData() **ERROR** found wrong meta info!";
    return NX_ERROR;
  }

  status = NXgetdata(fFileHandle, data_value);
  if (status != NX_OK) {
    fErrorCode = PNEXUS_GET_DATA_ERROR;
    fErrorMsg = "PNeXus::GetStringData() **ERROR** couldn't get data!";
    return NX_ERROR;
  }

  for (i = 0; i < dims[0]; i++)
    cstr[i] = *(data_value + i);
  cstr[i] = '\0';

  str = cstr;

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// GetStringAttr (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param attr attribute tag
 * \param str string to be fed
 */
NXstatus PNeXus::GetStringAttr(string attr, string &str)
{
  int i, status, attlen, atttype;
  char cstr[VGNAMELENMAX];
  NXname data_value;

  attlen = VGNAMELENMAX - 1;
  atttype = NX_CHAR;
  status = NXgetattr(fFileHandle, (char *)attr.c_str(), data_value, &attlen, &atttype);
  if (status != NX_OK) {
    cerr << endl << "**ERROR** in routine NXMgetstringattr: couldn't get attribute!" << endl << endl;
    fErrorCode = PNEXUS_GET_ATTR_ERROR;
    fErrorMsg = "PNeXus::GetStringAttr() **ERROR** couldn't get string attribute data!";
    return NX_ERROR;
  }

  for (i = 0; i < attlen; i++)
    cstr[i] = *(data_value + i);
  cstr[i] = '\0';

  str = cstr;

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// GetDataSize (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - size in bytes of the given type
 * - 0 if the type is not recognized
 *
 * \param type
 */
int PNeXus::GetDataSize(int type)
{
  int size;

  switch (type) {
    case NX_CHAR:
      size = SIZE_CHAR8;
      break;
    case NX_FLOAT32:
      size = SIZE_FLOAT32;
      break;
    case NX_FLOAT64:
      size = SIZE_FLOAT64;
      break;
    case NX_INT8:
      size = SIZE_INT8;
      break;
    case NX_UINT8:
      size = SIZE_UINT8;
      break;
    case NX_INT16:
      size = SIZE_INT16;
      break;
    case NX_UINT16:
      size = SIZE_UINT16;
      break;
    case NX_INT32:
      size = SIZE_INT32;
      break;
    case NX_UINT32:
      size = SIZE_UINT32;
      break;
    default:
      size = 0;
      break;
  }

  return size;
}

//-----------------------------------------------------------------------------------------------------
// GetDoubleVectorData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param data
 */
NXstatus PNeXus::GetDoubleVectorData(vector<double> &data)
{
  // get information of the current nexus entity
  int rank, type, dims[32], size, noOfElements;
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get data info!"))
    return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  float *f_data_ptr = (float*) data_ptr;
  int status = NXgetdata(fFileHandle, f_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  // copy the data into the vector
  data.clear();
  for (int i=0; i<noOfElements; i++) {
    data.push_back(*(f_data_ptr+i));
  }

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
  }

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// GetIntVectorData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * <b>return:</b>
 * - NX_OK on success
 * - NX_ERROR otherwiese
 *
 * \param data
 */
NXstatus PNeXus::GetIntVectorData(vector<int> &data)
{
  // get information of the current nexus entity
  int rank, type, dims[32], size, noOfElements;
  if (!ErrorHandler(NXgetinfo(fFileHandle, &rank, dims, &type), PNEXUS_GET_META_INFO_ERROR, "couldn't get data info!"))
    return NX_ERROR;

  // calculate the needed size
  size = dims[0];
  for (int i=1; i<rank; i++)
    size *= dims[i];
  noOfElements = size;
  size *= GetDataSize(type);

  // allocate locale memory to get the data
  char *data_ptr = new char[size];
  if (data_ptr == 0) {
    return NX_ERROR;
  }

  // get the data
  int *i_data_ptr = (int*) data_ptr;
  int status = NXgetdata(fFileHandle, i_data_ptr);
  if (status != NX_OK) {
    return NX_ERROR;
  }

  // copy the data into the vector
  data.clear();
  for (int i=0; i<noOfElements; i++) {
    data.push_back(*(i_data_ptr+i));
  }

  // clean up
  if (data_ptr) {
    delete [] data_ptr;
  }

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// SetT0 (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>Sets t0. The current muSR NeXus implementation has only a single t0 for all detectors since it was
 * tailored for pulsed muon sources (ISIS). This eventually needs to be changed.
 *
 * \param t0 to be set
 * \param idx index of t0
 */
void PNeXus::SetT0(unsigned int t0, unsigned idx)
{
  if (idx >= fData.fT0.size())
    fData.fT0.resize(idx+1);

  fData.fT0[idx] = t0;
}

//-----------------------------------------------------------------------------------------------------
// SetFirstGoodBin (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>Sets first good bin. The current muSR NeXus implementation has only a single first good bin for all detectors since it was
 * tailored for pulsed muon sources (ISIS). This eventually needs to be changed.
 *
 * \param fgb first good bin to be set
 * \param idx index of t0
 */
void PNeXus::SetFirstGoodBin(unsigned int fgb, unsigned int idx)
{
  if (idx >= fData.fFirstGoodBin.size())
    fData.fFirstGoodBin.resize(idx+1);

  fData.fFirstGoodBin[idx] = fgb;
}

//-----------------------------------------------------------------------------------------------------
// SetLastGoodBin (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>Sets last good bin. The current muSR NeXus implementation has only a single last good bin for all detectors since it was
 * tailored for pulsed muon sources (ISIS). This eventually needs to be changed.
 *
 * \param lgb to be set
 * \param idx index of t0
 */
void PNeXus::SetLastGoodBin(unsigned int lgb, unsigned int idx)
{
  if (idx >= fData.fLastGoodBin.size())
    fData.fLastGoodBin.resize(idx+1);

  fData.fLastGoodBin[idx] = lgb;
}

//-----------------------------------------------------------------------------------------------------
// SetHisto (public)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>
 *
 * \param histoNo histogram number (start counting from 0)
 * \param data histogram data vector to be set
 */
void PNeXus::SetHisto(unsigned int histoNo, vector<unsigned int> &data)
{
  if (histoNo >= fData.fHisto.size())
    fData.fHisto.resize(histoNo+1);

  fData.fHisto[histoNo] = data;
}

//-----------------------------------------------------------------------------------------------------
// GroupHistoData (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>Feed the grouped histo data, based on the grouping vector and the raw histo data.
 */
NXstatus PNeXus::GroupHistoData()
{
  // check if NO grouping is whished, in which case fData.fHisto == fGroupedData
  if (fData.fGrouping.size() == 0) {
    for (unsigned int i=0; i<fData.fHisto.size(); i++)
      fGroupedData.push_back(fData.fHisto[i]);
    return NX_OK;
  }

  // check that the grouping size is equal to the number of histograms
  if (fData.fGrouping.size() != fData.fHisto.size()) {
    fErrorCode = PNEXUS_HISTO_ERROR;
    fErrorMsg = "PNeXus::GroupHistoData() **ERROR** grouping vector size is unequal to the number of histos present!";
    return NX_ERROR;
  }

  // make a vector of all grouping present
  vector<unsigned int> groupingValue;
  bool newGroup = true;
  for (unsigned int i=0; i<fData.fGrouping.size(); i++) {
    newGroup = true;
    for (unsigned int j=0; j<groupingValue.size(); j++) {
      if (groupingValue[j] == fData.fGrouping[i]) {
        newGroup = false;
        break;
      }
    }
    if (newGroup)
      groupingValue.push_back(fData.fGrouping[i]);
  }

  // check that none of the grouping values is outside of the valid range
  for (unsigned int i=0; i<groupingValue.size(); i++) {
    if (groupingValue[i]-1 > fData.fHisto.size()) {
      fErrorCode = PNEXUS_HISTO_ERROR;
      fErrorMsg = "PNeXus::GroupHistoData() **ERROR** grouping values out of range";
      return NX_ERROR;
    }
  }

  // set fGroupedData to the proper size
  fGroupedData.clear();
  fGroupedData.resize(groupingValue.size());
  for (unsigned int i=0; i<fGroupedData.size(); i++) {
    fGroupedData[i].resize(fData.fHisto[0].size());
  }

  for (unsigned int i=0; i<fData.fHisto.size(); i++) {
    for (unsigned int j=0; j<fData.fHisto[i].size(); j++) {
      fGroupedData[fData.fGrouping[i]-1][j] += fData.fHisto[i][j];
    }
  }

  // cleanup
  groupingValue.clear();

  return NX_OK;
}

//-----------------------------------------------------------------------------------------------------
// TransformDate (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>Transformed a given date to the form yyyy-mm-dd if possible. Allowed input dates are yyyy-mm-dd,
 * yy-mm-dd, yyyy-MMM-dd, or yy-MMM-dd, where mm is the month as number and MMM the month as string, e.g. APR
 *
 * <p><b>return:</b> transformed date
 *
 * \param date
 * \param ok
 */
string PNeXus::TransformDate(string date, bool &ok)
{
  string result = date;
  string str;
  char cstr[128];
  int status, yy, mm, dd;

  ok = true;

  switch(date.length()) {
  case 8: // yy-mm-dd
    if ((date[2] != '-') || (date[5] != '-')) {
      ok = false;
    } else {
      status = sscanf(date.c_str(), "%d-%d-%d", &yy, &mm, &dd);
      if (status != 3) {
        ok = false;
      } else {
        if (yy >= 70) // i.e. 1970
          result = "19"+date;
        else // i.e. 20YY
          result = "20"+date;
      }
    }
    break;
  case 9: // yy-MMM-dd
    if ((date[2] != '-') || (date[6] != '-')) {
      ok = false;
    } else {
      str = date.substr(0,2); // yy
      status = sscanf(str.c_str(), "%d", &yy);
      if (status != 1)
        ok = false;
      str = date.substr(3,3); // MMM
      mm = GetMonth(str);
      if (mm == 0)
        ok = false;
      str = date.substr(7,2); // dd
      status = sscanf(str.c_str(), "%d", &dd);
      if (status != 1)
        ok = false;
      if (ok) {
        if (yy >= 70) // i.e. 1970
          yy += 1900;
        else
          yy += 2000;
        snprintf(cstr, sizeof(cstr), "%04d-%02d-%02d", yy, mm, dd);
        result = cstr;
      }
    }
    break;
  case 10: // yyyy-mm-dd
    if ((date[4] != '-') || (date[7] != '-')) {
      ok = false;
    } else {
      status = sscanf(date.c_str(), "%04d-%02d-%02d", &yy, &mm, &dd);
      if (status != 3) {
        ok = false;
      } else {
        ok = true;
      }
    }
    break;
  case 11: // yyyy-MMM-dd
    if ((date[4] != '-') || (date[8] != '-')) {
      ok = false;
    } else {
      str = date.substr(0,4); // yyyy
      status = sscanf(str.c_str(), "%d", &yy);
      if (status != 1)
        ok = false;
      str = date.substr(5,3); // MMM
      mm = GetMonth(str);
      if (mm == 0)
        ok = false;
      str = date.substr(9,2); // dd
      status = sscanf(str.c_str(), "%d", &dd);
      if (status != 1)
        ok = false;
      if (ok) {
        snprintf(cstr, sizeof(cstr), "%04d-%02d-%02d", yy, mm, dd);
        result = cstr;
      }
    }
    break;
  default:
    ok = false;
    break;
  }

  if (!ok) {
    fErrorCode = PNEXUS_WRONG_DATE_FORMAT;
    fErrorMsg = "PNeXus::TransformDate **ERROR** given date="+date+", is not of any of the the required forms!";
  }

  return result;
}

//-----------------------------------------------------------------------------------------------------
// GetMonth (private)
//-----------------------------------------------------------------------------------------------------
/**
 * <p>For a month string MMM (e.g. APR) return the month number
 *
 * <p><b>return:</b> month number
 *
 * \param month string of the form MMM, e.g. APR
 */
int PNeXus::GetMonth(const string month)
{
  int result = 0;

  if (!month.compare("jan") || !month.compare("Jan") || !month.compare("JAN"))
    result = 1;
  else if (!month.compare("feb") || !month.compare("Feb") || !month.compare("FEB"))
    result = 2;
  else if (!month.compare("mar") || !month.compare("Mar") || !month.compare("MAR"))
    result = 3;
  else if (!month.compare("apr") || !month.compare("Apr") || !month.compare("APR"))
    result = 4;
  else if (!month.compare("may") || !month.compare("May") || !month.compare("MAY"))
    result = 5;
  else if (!month.compare("jun") || !month.compare("Jun") || !month.compare("JUN"))
    result = 6;
  else if (!month.compare("jul") || !month.compare("Jul") || !month.compare("JUL"))
    result = 7;
  else if (!month.compare("aug") || !month.compare("Aug") || !month.compare("AUG"))
    result = 8;
  else if (!month.compare("sep") || !month.compare("Sep") || !month.compare("SEP"))
    result = 9;
  else if (!month.compare("oct") || !month.compare("Oct") || !month.compare("OCT"))
    result = 10;
  else if (!month.compare("nov") || !month.compare("Nov") || !month.compare("NOV"))
    result = 11;
  else if (!month.compare("dec") || !month.compare("Dec") || !month.compare("DEC"))
    result = 12;
  else
    result = 0;

  return result;
}

//-----------------------------------------------------------------------------------------------------
// end
//-----------------------------------------------------------------------------------------------------
