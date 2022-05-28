/********************************************************************************************

  MuSR_td_PSI_bin.cpp

  implementation of the class 'MuSR_td_PSI_bin'

  Main class to read mdu and td_bin PSI MuSR data.

*********************************************************************************************

    begin                : Alex Amato, October 2005
    modified             : Andrea Raselli, October 2009
                         : Andreas Suter, May 2020
    copyright            : (C) 2005 by
    email                : alex.amato@psi.ch

********************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>

#include "MuSR_td_PSI_bin.h"

//*******************************
//Implementation constructor
//*******************************

/*! \brief Simple Constructor setting some pointers and variables
 */

MuSR_td_PSI_bin::MuSR_td_PSI_bin()
{
  Clear();
}


//*******************************
//Implementation destructor
//*******************************

/*! \brief Simple Destructor clearing some pointers and variables
 */

 MuSR_td_PSI_bin::~MuSR_td_PSI_bin()
 {
   Clear();
 }


//*******************************
//Implementation Read (generic read)
//*******************************

/*! \brief Method to read a PSI-bin or an MDU file
 *
 *  This method gives back:
 *    - 0 for succesful reading
 *    - 1 if the open file action or the reading of the header failed
 *    - 2 for an unsupported version of the data
 *    - 3 for an error when allocating data buffer
 *    - 4 if number of histograms per record not equals 1
 *    - 5 if the number of histograms is less than 1
 *    - 6 if reading data failed
 *
 *  The parameter of the method is a const char * representing the name of the file to 
 *  be opened.
 */

 int MuSR_td_PSI_bin::Read(const char * fileName)
 {
   std::ifstream  file_name;

   Clear();

   fFilename    = fileName;

   file_name.open(fileName, std::ios_base::binary);  // open file
   if (file_name.fail())
   {
     fReadStatus  = "ERROR Open "+fFilename+" failed!";
     return 1;            // ERROR open failed
   }

   char *buffer_file = new char[3];
   if (!buffer_file)
   {
     fReadStatus = "ERROR Allocating data buffer";
     return 3;                 // ERROR allocating data buffer
   }

   file_name.read(buffer_file, 2);             // read format identifier of header
   // into buffer
   if (file_name.fail())
   {
     file_name.close();
     delete [] buffer_file;
     fReadStatus  = "ERROR Reading "+fFilename+" header failed!";
     return 1;                                  // ERROR reading header failed
   }

   strncpy(fFormatId,buffer_file,2);
   fFormatId[2] = '\0';

   file_name.close();
   delete [] buffer_file;

   if (fFormatId[0] == '1') {
     if (fFormatId[1] != 'N') {
       std::cout << "**WARNING** found '" << fFormatId << "'. Will change it to '1N'" << std::endl;
       fFormatId[1] = 'N';
     }
   }

   // file may either be PSI binary format
   if (strncmp(fFormatId,"1N",2) == 0)
   {
     return ReadBin(fileName);  // then read it as PSI bin
   }

   // or MDU format (pTA, TDC or 32 channel TDC)
   else if ((strncmp(fFormatId,"M3",2) == 0) ||(strncmp(fFormatId,"T4",2) == 0) ||
            (strncmp(fFormatId,"T5",2) == 0))
   {
     return ReadMdu(fileName); // else read it as MDU
   }
   else
   {
     fReadStatus  = "ERROR Unknown file format in "+fFilename+"!";
     return 2;                                 // ERROR unsupported version
   }

 }

 //*******************************
 //Implementation Write (generic write)
 //*******************************

 /*! \brief Method to write a PSI-bin or an MDU file
  *
  *  This method gives back:
  *    - 0 for succesful writing
  *    - 1 if the open file action or the writing of the header failed
  *    - 2 for an unsupported version of the data
  *    - 3 for an error when allocating data buffer
  *    - 4 if number of histograms per record not equals 1
  *    - 5 if the number of histograms is less than 1
  *    - 6 if writing data failed
  *
  *  The parameter of the method is a const char * representing the name of the file to
  *  be opened.
  */

int MuSR_td_PSI_bin::Write(const char *fileName)
{
   std::string fln = fileName;
   size_t found = fln.find_last_of(".");
   if (found == fln.npos) {
     return 1; // no extension found
   }
   std::string ext = fln.substr(found+1);
   int status = 0;
   if (ext == "bin")
     status = WriteBin(fileName);
   else if (ext == "mdu")
     status = WriteMdu(fileName);
   else
     return 2;

   return status;
}

//*******************************
//Implementation readbin
//*******************************

/* -- type definitions taken from tydefs.h -- */

#if ((defined(__DECC) || defined(__VAXC)) && !defined(unix) && !defined(OS_OSF1))

#if defined (__ALPHA)
typedef           short int      Int16;
typedef                 int      Int32;
#else
typedef                 int      Int16;
typedef            long int      Int32;
#endif


#elif defined (__osf__)            /* --- DEC UNIX or OFS/1 (AXP or else) --- */

#if defined (__alpha)
typedef           short int      Int16;
typedef                 int      Int32;
#else
typedef                 int      Int16;
typedef            long int      Int32;
#endif

#else /* other operating system */

/* 32 bit word length */
#if (defined(_WIN32) || defined(__linux__) || defined(_Darwin_) || defined(_WIN32GCC))
typedef                 short    Int16;
typedef                 int      Int32;
#else
typedef                 int      Int16;
typedef            long int      Int32;
#endif

#endif

typedef                 float   Float32;

//*******************************
//Implementation ReadBin
//*******************************

/*! \brief Method to read a PSI-bin file
 *
 *  This method gives back:
 *    - 0 for succesful reading
 *    - 1 if the open file action or the reading of the header failed
 *    - 2 for an unsupported version of the data
 *    - 3 for an error when allocating data buffer
 *    - 4 if number of histograms per record not equals 1
 *    - 5 if the number of histograms is less than 1
 *    - 6 if reading data failed
 *
 *  The parameter of the method is a const char * representing the name of the file to 
 *  be opened.
 */

int MuSR_td_PSI_bin::ReadBin(const char * fileName)
{
  std::ifstream  file_name;
  Int16     *dum_Int16;
  Int32     *dum_Int32;
  Float32   *dum_Float32;
  int       i;

  Int16    tdc_resolution;
  Int16    tdc_overflow ;

  Float32  mon_low[4];
  Float32  mon_high[4];
  Int32    mon_num_events;
  char     mon_dev[13];

  Int16    num_data_records_file;
  Int16    length_data_records_bins;
  Int16    num_data_records_histo;

  Int32    period_save;
  Int32    period_mon;

  Clear();

  if (sizeof(Int16) != 2)
  {
    fReadStatus  = "ERROR Size of Int16 data type is not 2 bytes!";
    return 1;            // ERROR open failed
  }

  if (sizeof(Int32) != 4)
  {
    fReadStatus  = "ERROR Sizeof Int32 data type is not 4 bytes";
    return 1;            // ERROR open failed
  }

  if (sizeof(Float32) != 4)
  {
    fReadStatus  = "ERROR Sizeof Float32 data type is not 4 bytes";
    return 1;            // ERROR open failed
  }

  fFilename    = fileName;

  file_name.open(fileName, std::ios_base::binary);  // open PSI bin file
  if (file_name.fail())
  {
    fReadStatus  = "ERROR Open "+fFilename+" failed!";
    return 1;            // ERROR open failed
  }

  char *buffer_file = new char[1024];
  if (!buffer_file)
  {
    fReadStatus = "ERROR Allocating buffer to read header failed!";
    return 3;                 // ERROR allocating data buffer
  }

  file_name.read(buffer_file, 1024);          // read header into buffer
  if (file_name.fail())
  {
    file_name.close();
    delete [] buffer_file;
    fReadStatus  = "ERROR Reading "+fFilename+" header failed!";
    return 1;                                  // ERROR reading header failed
  }
  // fill header data into member variables
  strncpy(fFormatId,buffer_file,2);
  fFormatId[2] = '\0';

  if (fFormatId[1] != 'N') // the warning is already issued in read()
    fFormatId[1] = 'N';

  if (strcmp(fFormatId,"1N") != 0)
  {
    file_name.close();
    delete [] buffer_file;
    fReadStatus  = "ERROR Unknown file format in "+fFilename+"!";
    return 2;                                 // ERROR unsupported version
  }

  dum_Int16 = (Int16 *) &buffer_file[2];
  tdc_resolution = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[4];
  tdc_overflow   = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[6];
  fNumRun        = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[28];
  fLengthHisto   = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[30];
  fNumberHisto   = *dum_Int16;

  strncpy(fSample,buffer_file+138,10);
  fSample[10] = '\0';

  strncpy(fTemp,buffer_file+148,10);
  fTemp[10] = '\0';

  strncpy(fField,buffer_file+158,10);
  fField[10] = '\0';

  strncpy(fOrient,buffer_file+168,10);
  fOrient[10] = '\0';

  strncpy(fSetup, buffer_file+178, 10);
  fSetup[10] = '\0';

  strncpy(fComment,buffer_file+860,62);
  fComment[62] = '\0';

  strncpy(fDateStart,buffer_file+218,9);
  fDateStart[9] = '\0';

  strncpy(fDateStop,buffer_file+227,9);
  fDateStop[9] = '\0';

  strncpy(fTimeStart,buffer_file+236,8);
  fTimeStart[8] = '\0';

  strncpy(fTimeStop,buffer_file+244,8);
  fTimeStop[8] = '\0';

  dum_Int32 = (Int32 *) &buffer_file[424];
  fTotalEvents   = *dum_Int32;

  for (i=0; i<=15; i++) {
    strncpy(fLabelsHisto[i],buffer_file+948+i*4,4);
    fLabelsHisto[i][4] = '\0';

    dum_Int32 = (Int32 *) &buffer_file[296+i*4];
    fEventsPerHisto[i] = *dum_Int32;

    dum_Int16 = (Int16 *) &buffer_file[458+i*2];
    fIntegerT0[i]       = *dum_Int16;

    dum_Int16 = (Int16 *) &buffer_file[490+i*2];
    fFirstGood[i]       = *dum_Int16;

    dum_Int16 = (Int16 *) &buffer_file[522+i*2];
    fLastGood[i]        = *dum_Int16;
  }

  for (i=0; i<=15; i++) {
    dum_Float32 = (Float32 *) &buffer_file[792+i*4];
    fRealT0[i]          = *dum_Float32;
  }

  fNumberScaler = 18;

  for (i=0; i<=5; i++) {
    dum_Int32 = (Int32 *) &buffer_file[670+i*4];
    fScalers[i]          = *dum_Int32;

    strncpy(fLabelsScalers[i],buffer_file+924+i*4,4);
    fLabelsScalers[i][4] = '\0';
  }

  for (i=6; i<fNumberScaler; i++) {
    dum_Int32 = (Int32 *) &buffer_file[360+(i-6)*4];
    fScalers[i]          = *dum_Int32;

    strncpy(fLabelsScalers[i],buffer_file+554+(i-6)*4,4);
    fLabelsScalers[i][4] = '\0';
  }

  dum_Float32 = (Float32 *) &buffer_file[1012];
  fBinWidth             = static_cast<double>(*dum_Float32);

  if (fBinWidth == 0.)
  {
    fBinWidth=0.125*(625.E-6)*pow(static_cast<double>(2.0),static_cast<double>(tdc_resolution));
  }

  fDefaultBinning = 1;

  fNumberTemper = 4;
  for (i=0; i< fNumberTemper; i++) {
    dum_Float32 = (Float32 *) &buffer_file[716+i*4];
    fTemper[i]           = *dum_Float32;

    dum_Float32 = (Float32 *) &buffer_file[738+i*4];
    fTempDeviation[i]   = *dum_Float32;

    dum_Float32 = (Float32 *) &buffer_file[72+i*4];
    mon_low[i]          = *dum_Float32;

    dum_Float32 = (Float32 *) &buffer_file[88+i*4];
    mon_high[i]         = *dum_Float32;
  }

  dum_Int32 = (Int32 *) &buffer_file[712];
  mon_num_events        = *dum_Int32;
  strncpy(mon_dev,buffer_file+60,12);
  mon_dev[12] = '\0';

  dum_Int16 = (Int16 *) &buffer_file[128]; // numdaf
  num_data_records_file    = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[130]; // lendaf
  length_data_records_bins = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[132]; // kdafhi
  num_data_records_histo   = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[134]; // khidaf
  if (*dum_Int16 != 1) {
    std::cout << "ERROR number of histograms/record not equals 1!"
        << " Required algorithm is not implemented!" << std::endl;
    delete [] buffer_file;
    file_name.close();
    fReadStatus  = "ERROR Algorithm to read multiple histograms in one block -"
                  " necessary to read " + fFilename + " - is not implemented!";
    return 4;                                // ERROR algorithm not implemented
  }

  dum_Int32 = (Int32 *) &buffer_file[654];
  period_save              = *dum_Int32;

  dum_Int32 = (Int32 *) &buffer_file[658];
  period_mon               = *dum_Int32;

  if (buffer_file) delete [] buffer_file;

  if (fNumberHisto <= 0)
  {
    file_name.close();
    fReadStatus  = "ERROR Less than 1 histogram in "  + fFilename;
    return 5;                                // ERROR number of histograms < 1
  }

  // allocate histograms
  fHisto.resize(fNumberHisto);

  for (i=0; i<fNumberHisto; i++) {
    fHisto[i].resize(fLengthHisto);
  }

  char *buffer_file_histo = new char[Int32(num_data_records_file)
                                     *Int32(length_data_records_bins)*4];
  if (!buffer_file_histo) {
    Clear();
    file_name.close();
    fReadStatus = "ERROR Allocating buffer to read histogram failed!";
    return 3;                                // ERROR allocating histogram buffer
  }
  file_name.seekg(1024, std::ios_base::beg);     // beginning of histogram data

  file_name.read(buffer_file_histo, Int32(num_data_records_file)
                 *Int32(length_data_records_bins)*4);
  if (file_name.fail()) {
    Clear();
    delete [] buffer_file_histo;
    file_name.close();
    fReadStatus  = "ERROR Reading data in "+fFilename+" failed!";
    return 6;                                // ERROR reading data failed
  }
  file_name.close();

  // process histograms
  std::vector<double> dummy_vector;

  fHistosVector.clear();
  for (i=0; i<fNumberHisto; i++) {
    dummy_vector.clear();
    for (int j=0; j<fLengthHisto; j++) {
      dum_Int32 = (Int32 *) &buffer_file_histo[(i*Int32(num_data_records_histo)*
                                                Int32(length_data_records_bins)+j)*sizeof(Int32)];
      fHisto[i][j]= *dum_Int32;
      dummy_vector.push_back(double(fHisto[i][j]));
    }
    fHistosVector.push_back(dummy_vector);
  }

  if (buffer_file_histo) delete [] buffer_file_histo;

  fReadStatus = "SUCCESS";
  fReadingOk = true;

  return 0;
}

//*******************************
//Implementation WriteBin
//*******************************

 /*! \brief Method to write a PSI-bin file
  *
  *  This method gives back:
  *    - 0 for succesful writing
  *    - 1 if the open file action or the writing of the header failed
  *    - 2 for an unsupported version of the data
  *    - 3 for an error when allocating data buffer
  *    - 4 if given data set / header information is inconsistent to the PSI-BIN format
  *
  *  The parameter of the method is a const char * representing the name of the file to
  *  be opened.
  */

int MuSR_td_PSI_bin::WriteBin(const char *fileName)
{
  if (!CheckDataConsistency(2)) {
    fWriteStatus = "ERROR Given data set is incompatible with the PSI-BIN format!";
    return 4;
  }

  std::ofstream  fout;

  // prepare buffer
  char *buffer = new char[1024];
  if (buffer == nullptr) {
    fWriteStatus = "ERROR Allocating buffer to write header failed!";
    return 3; // ERROR allocating data buffer
  }
  // initialize buffer
  memset(buffer, 0, 1024);

  // fill header info into the buffer

  fout.open(fileName, std::ios_base::binary);  // open PSI bin file
  if (fout.fail()) {
    fWriteStatus  = "ERROR Open " + std::string(fileName) + " failed!";
    return 1; // ERROR open failed
  }

  Int16   dum_Int16=0;
  Int32   dum_Int32=0;
  Float32 dum_Float32=0;
  // handle header -----------------------------------------------------------------------
  // psi-bin identifier
  strncpy(buffer, "1N", 2);
  // write tdc resolution code
  dum_Int16 = -1;
  memcpy(buffer+2, &dum_Int16, 2);
  // write run number
  dum_Int16 = (Int16)fNumRun;
  memcpy(buffer+6, &dum_Int16, 2);
  // write length of histogram: LENHIS
  dum_Int16 = (Int16)fLengthHisto;
  memcpy(buffer+28, &dum_Int16, 2);
  // write number of histogram (maximum == 16): NUMHIS
  dum_Int16 = (Int16)fNumberHisto;
  memcpy(buffer+30, &dum_Int16, 2);
  // write number of data records: NUMDAF = NUMHIS*KDAFHI
  dum_Int16 = (Int16)(fNumberHisto * (int)((fLengthHisto + MAXREC - 1) / MAXREC));
  memcpy(buffer+128, &dum_Int16, 2);
  // write record size: LENDAF
  dum_Int16 = MAXREC;
  memcpy(buffer+130, (const char*)&dum_Int16, 2);
  // write number of records per histogram: KDAFHI
  dum_Int16 = (Int16)(fLengthHisto + MAXREC - 1) / MAXREC;
  memcpy(buffer+132, &dum_Int16, 2);
  // write number of histograms per record: KHIDAF
  dum_Int16 = 1;
  memcpy(buffer+134, &dum_Int16, 2);
  // write fSample info
  strncpy(buffer+138, fSample, 10);
  // write temperature info
  strncpy(buffer+148, fTemp, 10);
  // write field info
  strncpy(buffer+158, fField, 10);
  // write orientation info
  strncpy(buffer+168, fOrient, 10);
  // write setup info
  strncpy(buffer+178, fSetup, 10);
  // write run start date
  strncpy(buffer+218, fDateStart, 9);
  // write run stop date
  strncpy(buffer+227, fDateStop, 9);
  // write run start time
  strncpy(buffer+236, fTimeStart, 8);
  // write run stop time
  strncpy(buffer+244, fTimeStop, 8);

  // write number of events of each histogram
  Int32 noEvents = 0;
  Int32 totalEvents = 0;
  for (int i=0; i<fNumberHisto; i++) {
    noEvents = 0;
    for (int j=0; j<fLengthHisto; j++) {
      noEvents += fHisto[i][j];
    }
    totalEvents += noEvents;
    memcpy(buffer+296+4*i, &noEvents, 4);
  }
  // write scaler contents 7 to 18
  for (int i=0; i<12; i++) {
    dum_Int32 = (Int32)fScalers[i+6];
    memcpy(buffer+360+4*i, &dum_Int32, 4);
  }
  // write total number of events
  memcpy(buffer+424, &totalEvents, 4);

  for (int i=0; i<fNumberHisto; i++) {
    // write t0 for each histogram
    dum_Int16 = (Int16)fIntegerT0[i];
    memcpy(buffer+458+2*i, (const char *)&dum_Int16, 2);
    // write first good bin for each histogram
    dum_Int16 = (Int16)fFirstGood[i];
    memcpy(buffer+490+2*i, (const char *)&dum_Int16, 2);
    // write last good bin for each histogram
    dum_Int16 = (Int16)fLastGood[i];
    memcpy(buffer+522+2*i, (const char *)&dum_Int16, 2);
  }
  // write labels for scalers 7-18
  for (int i=6; i<fNumberScaler; i++) {
    strncpy(buffer+554+i*4, fLabelsScalers[i], 4);
  }
  // write scaler contents 1 to 6
  for (int i=0; i<6; i++) {
    dum_Int32 = (Int32)fScalers[i];
    memcpy(buffer+670+i*4, &dum_Int32, 4);
  }
  // write number of measurements used for temperature
  dum_Int32 = (Int32)fNumberTemper;
  memcpy(buffer+712, (const char *)&dum_Int32, 4);
  for (int i=0; i<fNumberTemper; i++) {
    // write temperature means
    dum_Float32 = (Float32)fTemper[i];
    memcpy(buffer+716+i*4, (const char *)&dum_Float32, 4);
    // write temperature std dev
    dum_Float32 = (Float32)fTempDeviation[i];
    memcpy(buffer+738+i*4, (const char *)&dum_Float32, 4);
  }
  // write run comment
  strncpy(buffer+860, fComment, 62);
  // write labels of scalers 1-6
  for (int i=0; i<6; i++) {
    strncpy(buffer+924+i*4, fLabelsScalers[i], 4);
  }
  // write labels of histograms
  for (int i=0; i<fNumberHisto; i++) {
    strncpy(buffer+948+i*4, fLabelsHisto[i], 4);
  }
  // write TDS time resolution
  dum_Float32 = static_cast<Float32>(fBinWidth);
  memcpy(buffer+1012, (const char*)&dum_Float32, 4);

  // write header information
  fout.write(buffer, 1024);

  // handle histograms -----------------------------------------------------------------

  // prepare write buffer
  if (buffer) { // Get rid of the header buffer
    delete [] buffer;
    buffer = nullptr;
  }
  buffer = new char[4*MAXREC];
  if (!buffer) {
    fWriteStatus = "ERROR Allocating buffer to write data failed!";
    return 3; // ERROR allocating data buffer
  }
  // initialize buffer
  memset(buffer, 0, 4*MAXREC);

  bool buffer_empty = false;
  for (int i=0; i<fNumberHisto; i++) {
    for (int j=0; j<fLengthHisto; j++) {
      dum_Int32 = (Int32)fHisto[i][j];
      memcpy(buffer+(4*j)%(4*MAXREC), &dum_Int32, 4);
      buffer_empty = false;
      if ((j > 0) && (j%MAXREC == 0)) {
        fout.write(buffer, 4*MAXREC);
        // reinizialize buffer
        memset(buffer, 0, 4*MAXREC);
        buffer_empty = true;
      }
    }
    // check if there is still a record to be written
    if (!buffer_empty) {
      fout.write(buffer, 4*MAXREC);
      // reinizialize buffer
      memset(buffer, 0, 4*MAXREC);
    }
  }

  fout.close();

  // clean up
  if (buffer) {
    delete [] buffer;
    buffer = nullptr;
  }

  return 0;
}


//*******************************
//Implementation readmdu
//*******************************

#define DATESTR    12     /* Length of date string 01-NOV-1999 */
#define TIMESTR     9     /* Length of time string 08:45:30 */

/* automatic data conversion */
#define TITLESTR      40
#define SUBTITLESTR   62
#define DATAFORMATSTR 20
#define DETECTLISTSTR 200
#define TEMPLISTSTR   50

  /* - event types and event evaluation mode                                             */
#define PTAMODE_NONE   0  /* not initialised                                */
#define PTAMODE_NORMAL 1  /* "normal" events M-P..                          */
#define PTAMODE_CLOCK  2  /* additional clock generated events to prevent
                             overflow of pTA*/
#define PTAMODE_ECHO   4  /* echo mode M-P .. Echo (delayed M signal)       */

  /* - tag types */
#define PTATAGC_NONE       'N'
#define PTATAGC_MUON       'M'
#define PTATAGC_POSITRON   'P'
#define PTATAGC_CLOCK      'C'
#define PTATAGC_ECHO       'E'
#define PTATAGC_VETO       'V'
#define PTATAGC_UNKNOWN    'U'

  /* - number of tags and tag name string length */
#define PTAMAXTAGS    16  /* max number of pTA tags for pTA MDU M3 */
#define TDCMAXTAGS16  16  /* max number of pTA tags for TDC MDU T4 */
#define TDCMAXTAGS32  32  /* max number of pTA tags for TDC MDU T5 */

#define MAXTAGSTR     12  /* max length of pTA tag strings                  */

/* ---------------------------------------------------------------------- */

/*   basic structure of a MidasDUmp file witten by pTA front end

      // write header information
      fwrite(&gpTAfhead, gpTAfhead.NumBytesHeader, 1, fp);

      // write settings information
      fwrite(&gpTAset, gpTAfhead.NumBytesSettings, 1, fp);

      // write statistic
      fwrite(&gpTAstattot, gpTAfhead.NumBytesStatistics, 1, fp);

      for (i = 0; i < PTAMAXTAGS; i++) {
        // write tag record of histogram
        fwrite(&gpTAset.tag[i], gpTAfhead.NumBytesTag, 1, fp);

        // write histogram data
        if ((gpTAset.tag[i].Type == PTATAGC_POSITRON) &&
            ((nbins =(gpTAset.tag[i].Histomaxb - gpTAset.tag[i].Histominb + 1)) > 1) &&
            (gpHistogram[i] != NULL))
            fwrite(gpHistogram[i], sizeof(unsigned int), nbins, fp);
      }


 */
    /* - general file header part used to save runs */
typedef struct _FeFileHeaderRec {
  char   FmtId;
  char   FmtVersion;
  char   StartDate[DATESTR];
  char   StartTime[TIMESTR];
  char   EndDate[DATESTR];
  char   EndTime[TIMESTR];
  Int32  RunNumber;
  Int32  FileVersion;

  /* information for automatic data conversion */
  char   RunTitle[TITLESTR+1];
  char   RunSubTitle[SUBTITLESTR+1];
  char   DataFormat[DATAFORMATSTR];// data format (automatically converted to)
  Int32  HistoResolution;          // TDC resolution factor for tarGet format
                                   // or pTA timespan
  Int32  BinOffset;
  Int32  BinsPerHistogram;
  Int32  NumberOfDetectors;
  char DetectorNumberList[DETECTLISTSTR]; // list of detectors to be converted
                                          // to the tarGet data format
  /* additional information */
  char MeanTemp[TEMPLISTSTR];
  char TempDev[TEMPLISTSTR];
} FeFileHeaderRec, *FeFileHeaderPtr;

  /* - pTA file header */
typedef struct _pTAFileHeaderRec {
  FeFileHeaderRec Header;
  Int32  BinSize;
  Int32  NumBytesHeader;
  Int32  NumBytesSettings;
  Int32  NumBytesTag;
  Int32  NumBytesStatistics;
} pTAFileHeaderRec, *pTAFileHeaderPtr;

  /* - pTA tag information */
typedef struct _pTATagRec {
  char  Label[MAXTAGSTR];
  char  Type;

  /* original pTA list mode (raw) time difference */
  Int32   Rawminps;
  Int32   Rawmaxps;
  Int32   Rawminb; /* bin range may be 0-262143 (or larger if PTAMODE_CLOCK) */
  Int32   Rawmaxb;

  /* a modified time difference (binning) may be stored in histo */
  Int32   Histominps;
  Int32   Histomaxps;
  Int32   Histominb;
  Int32   Histomaxb;
  Int32   t0b;   /* t0, tfirst tlast in [bins] for automatic data conversion */
  Int32   tfb;   /* NOTE: t0b, tfb, tlb are in bin units of the tarGet format!! */
  Int32   tlb;
} pTATagRec, *pTATagPtr;

  /* - pTA settings relevant for td_musr for pTA M3 format*/
typedef struct _pTASettingsRec {
  Int32 mode;     /* PTAMODE_NORMAL[+PTAMODE_CLOCK] or PTAMODE_ECHO          */
  Int32 preps;    /* pre  pile up [ps] (nearest integer)        ; info only */
  Int32 posps;    /* post pile up [ps]                          ; info only */
  Int32 preb;     /* pre  pile up [bins]                                     */
  Int32 posb;     /* post pile up [bins]                                     */
  Int32 ecsps;    /* muon echo signal delay (PTAMODE_ECHO) [ps] ; info only */
  Int32 ectps;    /* muon echo tolerance    (PTAMODE_ECHO) [ps] ; info only */
  Int32 ecsb;     /* muon echo signal delay (PTAMODE_ECHO) [bins]            */
  Int32 ectb;     /* muon echo tolerance    (PTAMODE_ECHO) [bins]            */
  Int32 timespan; /* pTA timespan                                            */
  Int32 minrate;  /* minimum event rate                                      */
  Int32 eortag;   /* end of run tag number                                   */
  Int32 eorlim;   /* end of run limit                                        */
  pTATagRec tag[PTAMAXTAGS];
} pTASettingsRec, *pTASettingsPtr;

  /* - pTA settings relevant for td_musr for TDC T4 format */
typedef struct _pTATDCSettingsRec {
  Int32 mode;     /* PTAMODE_NORMAL[+PTAMODE_CLOCK] or PTAMODE_ECHO          */
  Int32 preps;    /* pre  pile up [ps] (nearest integer)        ; info only */
  Int32 posps;    /* post pile up [ps]                          ; info only */
  Int32 preb;     /* pre  pile up [bins]                                     */
  Int32 posb;     /* post pile up [bins]                                     */
  Int32 ecsps;    /* muon echo signal delay (PTAMODE_ECHO) [ps] ; info only */
  Int32 ectps;    /* muon echo tolerance    (PTAMODE_ECHO) [ps] ; info only */
  Int32 ecsb;     /* muon echo signal delay (PTAMODE_ECHO) [bins]            */
  Int32 ectb;     /* muon echo tolerance    (PTAMODE_ECHO) [bins]            */
  Int32 resolutioncode; /* type specific TDC resolution code 25 ps, 200ps    */
  Int32 minrate;  /* minimum event rate                                      */
  Int32 eortag;   /* end of run tag number                                   */
  Int32 eorlim;   /* end of run limit                                        */
  pTATagRec tag[TDCMAXTAGS16];
} pTATDCSettingsRec, *pTATDCSettingsPtr;

  /* - pTA settings relevant for td_musr for TDC T5 format */
typedef struct _pTATDC32SettingsRec {
  Int32 mode;     /* PTAMODE_NORMAL[+PTAMODE_CLOCK] or PTAMODE_ECHO          */
  Int32 preps;    /* pre  pile up [ps] (nearest integer)        ; info only */
  Int32 posps;    /* post pile up [ps]                          ; info only */
  Int32 preb;     /* pre  pile up [bins]                                     */
  Int32 posb;     /* post pile up [bins]                                     */
  Int32 ecsps;    /* muon echo signal delay (PTAMODE_ECHO) [ps] ; info only */
  Int32 ectps;    /* muon echo tolerance    (PTAMODE_ECHO) [ps] ; info only */
  Int32 ecsb;     /* muon echo signal delay (PTAMODE_ECHO) [bins]            */
  Int32 ectb;     /* muon echo tolerance    (PTAMODE_ECHO) [bins]            */
  Int32 resolutioncode; /* type specific TDC resolution code 25 ps, 200ps    */
  Int32 minrate;  /* minimum event rate                                      */
  Int32 eortag;   /* end of run tag number                                   */
  Int32 eorlim;   /* end of run limit                                        */
  pTATagRec tag[TDCMAXTAGS32];
} pTATDC32SettingsRec, *pTATDC32SettingsPtr;

  /* - pTA td_musr statistic for pTA M3 format */
typedef struct _pTAStatisticRec {
  Int32 time;
  Int32 ESum;
  Int32 EMuon;
  Int32 EPositron;
  Int32 EClock;
  Int32 EEcho;
  Int32 EVeto;
  Int32 EUnknown;
  Int32 EMMPileup;
  Int32 EPrePileup;                         /* M-P-M Pileup */
  Int32 EPostPileup;                        /* M-M-P Pileup */
  Int32 EHOverflow;
  Int32 EDoublePositron;
  Int32 EAccepted;
  Int32 TagScaler[PTAMAXTAGS];
  Int32 HistogramScaler[PTAMAXTAGS];
  Int32 EOverFlowBits;  /* overflow flag bits for time and event counter overflow*/
  Int32 TSOverFlowBits; /* overflow flag bits for tag scaler overflow            */
  Int32 HSOverFlowBits; /* overflow flag bits for histogram scaler overflow      */
  Int32 HOverFlowBits;  /* overflow flag bits for histogram overflow             */
} pTAStatisticRec, *pTAStatisticPtr;

  /* - pTA td_musr statistic for TDC T4 format */
typedef struct _pTATDCStatisticRec {
  Int32 time;
  Int32 ESum;
  Int32 EMuon;
  Int32 EPositron;
  Int32 EClock;
  Int32 EEcho;
  Int32 EVeto;
  Int32 EUnknown;
  Int32 EMMPileup;
  Int32 EPrePileup;                         /* M-P-M Pileup */
  Int32 EPostPileup;                        /* M-M-P Pileup */
  Int32 EHOverflow;
  Int32 EDoublePositron;
  Int32 EAccepted;
  Int32 TagScaler[TDCMAXTAGS16];
  Int32 HistogramScaler[TDCMAXTAGS16];
  Int32 EOverFlowBits;  /* overflow flag bits for time and event counter overflow*/
  Int32 TSOverFlowBits; /* overflow flag bits for tag scaler overflow            */
  Int32 HSOverFlowBits; /* overflow flag bits for histogram scaler overflow      */
  Int32 HOverFlowBits;  /* overflow flag bits for histogram overflow             */
} pTATDCStatisticRec, *pTATDCStatisticPtr;

  /* - pTA td_musr statistic for TDC T5 format*/
typedef struct _pTATDC32StatisticRec {
  Int32 time;
  Int32 ESum;
  Int32 EMuon;
  Int32 EPositron;
  Int32 EClock;
  Int32 EEcho;
  Int32 EVeto;
  Int32 EUnknown;
  Int32 EMMPileup;
  Int32 EPrePileup;                         /* M-P-M Pileup */
  Int32 EPostPileup;                        /* M-M-P Pileup */
  Int32 EHOverflow;
  Int32 EDoublePositron;
  Int32 EAccepted;
  Int32 TagScaler[TDCMAXTAGS32];
  Int32 HistogramScaler[TDCMAXTAGS32];
  Int32 EOverFlowBits;  /* overflow flag bits for time and event counter overflow*/
  Int32 TSOverFlowBits; /* overflow flag bits for tag scaler overflow            */
  Int32 HSOverFlowBits; /* overflow flag bits for histogram scaler overflow      */
  Int32 HOverFlowBits;  /* overflow flag bits for histogram overflow             */
} pTATDC32StatisticRec, *pTATDC32StatisticPtr;

/* ---------------------------------------------------------------------- */

//*******************************
//Implementation ReadMdu
//*******************************

/*! \brief Method to read a MuSR MDU file
 *
 *  This method gives back:
 *    - 0 for succesful reading
 *    - 1 if the open file action or the reading of the header failed
 *    - 2 for an unsupported version of the data
 *    - 3 for an error when allocating data buffer
 *    - 5 if the number of histograms is less than 1
 *    - 6 if reading data failed
 *
 *  The parameter of the method is a const char * representing the name of the 
 *  file to be opened.
 */

int MuSR_td_PSI_bin::ReadMdu(const char * fileName)
{
  std::ifstream  file_name;
  int       i, j;

  Clear();

  if (sizeof(Int32) != 4)
  {
    fReadStatus  = "ERROR Sizeof( Int32 ) data type is not 4 bytes";
    return 1;            // ERROR open failed
  }

  fFilename    = fileName;

  file_name.open(fileName, std::ios_base::binary);  // open PSI bin file
  if (file_name.fail())
  {
    fReadStatus  = "ERROR Open "+fFilename+" failed!";
    return 1;            // ERROR open failed
  }

  pTAFileHeaderRec      gpTAfhead;
  //FeFileHeaderPtr     gpFehead  = &gpTAfhead.Header;

  file_name.read((char *)&gpTAfhead, sizeof gpTAfhead);    // read header into buffer
  if (file_name.fail())
  {
    file_name.close();
    fReadStatus  = "ERROR Reading "+fFilename+" header failed!";
    return 1;                                  // ERROR reading header failed
  }
  // fill header data into member variables
  fFormatId[0] = gpTAfhead.Header.FmtId;
  fFormatId[1] = gpTAfhead.Header.FmtVersion;
  fFormatId[2] = '\0';

  if ((strcmp(fFormatId,"M3") != 0) && (strcmp(fFormatId,"T4") != 0) &&
      (strcmp(fFormatId,"T5") != 0))
  {
    file_name.close();
    fReadStatus  = "ERROR Unknown file format in "+fFilename+"!";
    return 2;                                 // ERROR unsupported version
  }

  if (sizeof(pTAFileHeaderRec) != gpTAfhead.NumBytesHeader)
  {
    file_name.close();
    fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTAFileHeaderRec size";
    return 1;                                  // ERROR reading header failed
  }

  // header size OK read header information
  strncpy(fSample,&gpTAfhead.Header.RunTitle[0],10);
  fSample[10] = '\0';

  strncpy(fTemp,  &gpTAfhead.Header.RunTitle[10],10);
  fTemp[10] = '\0';

  strncpy(fField, &gpTAfhead.Header.RunTitle[20],10);
  fField[10] = '\0';

  strncpy(fOrient,&gpTAfhead.Header.RunTitle[30],10);
  fOrient[10] = '\0';

  strncpy(fComment,&gpTAfhead.Header.RunSubTitle[0],62);
  fComment[62] = '\0';

  strncpy(&fDateStart[0],&gpTAfhead.Header.StartDate[0],7);
  strncpy(&fDateStart[7],&gpTAfhead.Header.StartDate[9],2);
  fDateStart[9] = '\0';

  strncpy(&fDateStop[0],&gpTAfhead.Header.EndDate[0],7);
  strncpy(&fDateStop[7],&gpTAfhead.Header.EndDate[9],2);
  fDateStop[9] = '\0';

  strncpy(fTimeStart,&gpTAfhead.Header.StartTime[0],8);
  fTimeStart[8] = '\0';

  strncpy(fTimeStop,&gpTAfhead.Header.EndTime[0],8);
  fTimeStop[8] = '\0';

  fNumRun = gpTAfhead.Header.RunNumber;

  if (sizeof(pTATagRec) != gpTAfhead.NumBytesTag) {
    file_name.close();
    fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTATagRec size";
    return 1;                                  // ERROR reading header failed
  }

#ifdef MIDEBUG1
  cout << "Header.MeanTemp = " << gpTAfhead.Header.MeanTemp << endl;
  cout << "Header.TempDev  = " << gpTAfhead.Header.TempDev << endl;
#endif

  // read temperature deviation from header string (td0 [td1 [td2 [td3]]])
  fNumberTemper = sscanf(gpTAfhead.Header.TempDev,"%f %f %f %f",
                         &fTempDeviation[0], &fTempDeviation[1], &fTempDeviation[2],
                         &fTempDeviation[3]);

  // fill unused
  for (i=fNumberTemper; i<MAXTEMPER; i++)
    fTempDeviation[i] = 0.f;

  // read temperature from header string (t0 [t1 [t2 [t3]]])
  fNumberTemper = sscanf(gpTAfhead.Header.MeanTemp,"%f %f %f %f",
                         &fTemper[0], &fTemper[1], &fTemper[2], &fTemper[3]);

  // fill unused
  for (i=fNumberTemper; i<MAXTEMPER; i++) {
    fTemper[i] = 0.f;
    fTempDeviation[i] = 0.f;
  }

#ifdef MIDEBUG1
  cout << "Header.DataFormat = " << gpTAfhead.Header.DataFormat << endl;
  cout << "Header.HistoResolution = " << gpTAfhead.Header.HistoResolution << endl;
  cout << "Header.BinOffset = " << gpTAfhead.Header.BinOffset << endl;
  cout << "Header.BinsPerHistogram = " << gpTAfhead.Header.BinsPerHistogram << endl;
  cout << "Header.NumberOfDetectors = " << gpTAfhead.Header.NumberOfDetectors << endl;
  cout << "Header.DetectorNumberList = " << gpTAfhead.Header.DetectorNumberList << endl;
#endif

  // process detector list in gpTAfhead.Header.NumberOfDetectors
  // for pTA only histograms of selected detectors are valid
  bool selected[MAXHISTO];

  for (i=0; i < MAXHISTO; i++)
    selected[i] = false;

  for (i=0,j=0; i <= (int)strlen(gpTAfhead.Header.DetectorNumberList); i++) {
    if ((gpTAfhead.Header.DetectorNumberList[i] == ' ') ||
        (gpTAfhead.Header.DetectorNumberList[i] == '\0')) {
      int it;
      if (sscanf(&gpTAfhead.Header.DetectorNumberList[j],"%d",&it) == 1) {
        j = i+1; // assume next char is start of next number
        if ((it >= 0) && (it < MAXHISTO)) {
          selected[it] = true;
#ifdef MIDEBUG1
          cout << "Histogram " << it << " is selected " << endl;
#endif
        } else {
          std::cout << "error " << it << " is out of range |0 - " << MAXHISTO-1 << "|"
              << std::endl;
        }
      } else {
        std::cout << "error reading " << &gpTAfhead.Header.DetectorNumberList[j] << std::endl;
      }
    }
  }

  int tothist = 0;
  int resolutionfactor = 1;

  // ---- process version specific settings and total statistics
  if        (strcmp(fFormatId,"M3") == 0) {

    if (sizeof(pTASettingsRec) != gpTAfhead.NumBytesSettings) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTASettingsRec size";
      return 1;                                  // ERROR reading header failed
    }

    if (sizeof(pTAStatisticRec) != gpTAfhead.NumBytesStatistics) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTAStatisticRec size";
      return 1;                                  // ERROR reading header failed
    }

    pTASettingsRec   gpTAsetpta;
    pTAStatisticRec  gpTAstattotpta;

    tothist = PTAMAXTAGS;

    file_name.read((char *)&gpTAsetpta, sizeof gpTAsetpta);//read settings into buffer
    if (file_name.fail()) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" settings failed!";
      return 1;                                  // ERROR reading settings failed
    }

    // read stat into buffer
    file_name.read((char *)&gpTAstattotpta, sizeof gpTAstattotpta);
    if (file_name.fail()) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" statistics failed!";
      return 1;                                  // ERROR reading statistics failed
    }

    fNumberScaler = PTAMAXTAGS;
    for (i=0; i < fNumberScaler; i++) {
      strncpy(fLabelsScalers[i],gpTAsetpta.tag[i].Label,MAXLABELSIZE);
      fLabelsScalers[i][MAXLABELSIZE-1] = '\0';

      fScalers[i] = gpTAstattotpta.TagScaler[i];
    }

    int timespan;

    resolutionfactor = 1;
    timespan = gpTAfhead.Header.HistoResolution; // tarGet timespan (PSIBIN)
    // t0, fg, lg
    if      (gpTAsetpta.timespan == 11) // pta timespan
      fBinWidth = 0.000625;
    else if (gpTAsetpta.timespan == 10)
      fBinWidth = 0.0003125;
    else if (gpTAsetpta.timespan ==  9)
      fBinWidth = 0.00015625;
    else if (gpTAsetpta.timespan ==  8)
      fBinWidth = 0.000078125;
    else if (gpTAsetpta.timespan ==  7)
      fBinWidth = 0.0000390625;
    else if (gpTAsetpta.timespan ==  6)
      fBinWidth = 0.00001953125;
    else {
      file_name.close();
      fReadStatus  = "ERROR "+fFilename+" settings resolution code failed!";
      return 1;                                   // ERROR reading settings failed
    }

    if (timespan+8-gpTAsetpta.timespan < 0) {
      // NIY error
    } else {
      /* PSI resolution    pTA timespan    resolution [usec]
         *    -2              6               0.00001953125
         *    -1              7               0.0000390625
         *     0              8               0.000078125
         *     1              9               0.00015625
         *     2             10               0.0003125
         *     3             11               0.000625
         *     4              -               0.00125
         *     5              -               0.0025
         *     6              -               0.005
         */
      // resolution factor for binning
      for (i=0; i < timespan+8-gpTAsetpta.timespan; i++)
        resolutionfactor *= 2;
    }

    fLengthHisto   = 0;
    fNumberHisto   = 0;
    for (i=0; i<tothist; i++) {
      /* read histogram data */
      if (gpTAsetpta.tag[i].Type == PTATAGC_POSITRON) {
        int nbins;

#ifdef MIDEBUG1
        cout << "Tag[" << i << "] Histomin = " << gpTAsetpta.tag[i].Histominb
            << " Histomax = " << gpTAsetpta.tag[i].Histomaxb << endl;
#endif
        // is a histogram there
        if ((nbins=(gpTAsetpta.tag[i].Histomaxb-gpTAsetpta.tag[i].Histominb + 1))>1) {

          // for pTA only: read histogram only if histogram was selected
          if (selected[i]) {
            // first histo -> take histogram length
            if (fNumberHisto == 0)
              fLengthHisto = nbins+gpTAsetpta.tag[i].Histominb;
            // different histogram length?
            else if (fLengthHisto != nbins+gpTAsetpta.tag[i].Histominb) {
              std::cout << "Different histogram lengths!" << std::endl;
            }
            fNumberHisto++;
          }
        }
      }
    }

    // check gpTAfhead.Header.NumberOfDetectors
    if (fNumberHisto != gpTAfhead.Header.NumberOfDetectors)
      std::cout << "Number of found histos " << fNumberHisto << " and number in header "
          << gpTAfhead.Header.NumberOfDetectors << " differ!" << std::endl;

    // special case: subtract 1 from stored histogram to Get desired histogram length
    if (fLengthHisto > 0) fLengthHisto -= 1;

  } else if (strcmp(fFormatId,"T4") == 0) {

    if (sizeof(pTATDCSettingsRec) != gpTAfhead.NumBytesSettings) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTATDCSettingsRec size";
      return 1;                                  // ERROR reading header failed
    }

    if (sizeof(pTATDCStatisticRec) != gpTAfhead.NumBytesStatistics) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTATDCStatisticRec size";
      return 1;                                  // ERROR reading header failed
    }

    pTATDCSettingsRec   gpTAsettdc;
    pTATDCStatisticRec  gpTAstattottdc;

    tothist = TDCMAXTAGS16;

    file_name.read((char *)&gpTAsettdc, sizeof gpTAsettdc);//read settings into buffer
    if (file_name.fail()) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" settings failed!";
      return 1;                                  // ERROR reading settings failed
    }

    file_name.read((char *)&gpTAstattottdc, sizeof gpTAstattottdc); // read stat into buffer
    if (file_name.fail()) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" statistics failed!";
      return 1;                                  // ERROR reading statistics failed
    }

    fNumberScaler = TDCMAXTAGS16;
    for (i=0; i < fNumberScaler; i++) {
      strncpy(fLabelsScalers[i],gpTAsettdc.tag[i].Label,MAXLABELSIZE);
      fLabelsScalers[i][MAXLABELSIZE-1] = '\0';

      fScalers[i] = gpTAstattottdc.TagScaler[i];
    }

    resolutionfactor = gpTAfhead.Header.HistoResolution;
    if      (gpTAsettdc.resolutioncode ==  25)
      fBinWidth = 0.0000244140625;
    else if (gpTAsettdc.resolutioncode == 100)
      fBinWidth = 0.00009765625;
    else if (gpTAsettdc.resolutioncode == 200)
      fBinWidth = 0.0001953125;
    else if (gpTAsettdc.resolutioncode == 800)
      fBinWidth = 0.0007812500;
    else {
      file_name.close();
      fReadStatus  = "ERROR "+fFilename+" settings resolution code failed!";
      return 1;                                  // ERROR reading settings failed
    }

    fLengthHisto   = 0;
    fNumberHisto   = 0;
    for (i=0; i<tothist; i++) {
      /* read histogram data */
      if (gpTAsettdc.tag[i].Type == PTATAGC_POSITRON) {
        int nbins;

#ifdef MIDEBUG1
        cout << "Tag[" << i << "] Histomin = " << gpTAsettdc.tag[i].Histominb
            << " Histomax = " << gpTAsettdc.tag[i].Histomaxb << endl;
#endif
        // is a histogram there
        if ((nbins=(gpTAsettdc.tag[i].Histomaxb-gpTAsettdc.tag[i].Histominb + 1))>1) {
          // first histo -> take histogram length
          if (fNumberHisto == 0)
            fLengthHisto = nbins+gpTAsettdc.tag[i].Histominb;
          // different histogram length?
          else if (fLengthHisto != nbins+gpTAsettdc.tag[i].Histominb) {
            std::cout << "Different histogram lengths!" << std::endl;
          }
          fNumberHisto++;
        }
      }
    }

  } else if (strcmp(fFormatId,"T5") == 0) {

    if (sizeof(pTATDC32SettingsRec) != gpTAfhead.NumBytesSettings) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTATDC32SettingsRec size";
      return 1;                                  // ERROR reading header failed
    }

    if (sizeof(pTATDC32StatisticRec) != gpTAfhead.NumBytesStatistics) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" incorrect pTATDC32StatisticRec size";
      return 1;                                  // ERROR reading header failed
    }

    pTATDC32SettingsRec   gpTAsettdc32;
    pTATDC32StatisticRec  gpTAstattottdc32;

    tothist = TDCMAXTAGS32;

    // read settings into buffer
    file_name.read((char *)&gpTAsettdc32, sizeof gpTAsettdc32);
    if (file_name.fail()) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" settings failed!";
      return 1;                                  // ERROR reading settings failed
    }

    // read stat into buffer
    file_name.read((char *)&gpTAstattottdc32, sizeof gpTAstattottdc32);
    if (file_name.fail()) {
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" statistics failed!";
      return 1;                                  // ERROR reading statistics failed
    }

    fNumberScaler = TDCMAXTAGS32;
    for (i=0; i < fNumberScaler; i++) {
      strncpy(fLabelsScalers[i],gpTAsettdc32.tag[i].Label,MAXLABELSIZE);
      fLabelsScalers[i][MAXLABELSIZE-1] = '\0';

      fScalers[i] = gpTAstattottdc32.TagScaler[i];
    }

    resolutionfactor = gpTAfhead.Header.HistoResolution;
    if      (gpTAsettdc32.resolutioncode ==  25)
      fBinWidth = 0.0000244140625;
    else if (gpTAsettdc32.resolutioncode == 100)
      fBinWidth = 0.00009765625;
    else if (gpTAsettdc32.resolutioncode == 200)
      fBinWidth = 0.0001953125;
    else if (gpTAsettdc32.resolutioncode == 800)
      fBinWidth = 0.0007812500;
    else
    {
      file_name.close();
      fReadStatus  = "ERROR "+fFilename+" settings resolution code failed!";
      return 1;                                  // ERROR reading settings failed
    }

    fLengthHisto   = 0;
    fNumberHisto   = 0;
    for (i=0; i<tothist; i++) {
      /* read histogram data */
      if (gpTAsettdc32.tag[i].Type == PTATAGC_POSITRON) {
        int nbins;

#ifdef MIDEBUG1
        cout << "Tag[" << i << "] Histomin = " << gpTAsettdc32.tag[i].Histominb
            << " Histomax = " << gpTAsettdc32.tag[i].Histomaxb << endl;
#endif
        // is a histogram there?
        if ((nbins=(gpTAsettdc32.tag[i].Histomaxb-gpTAsettdc32.tag[i].Histominb + 1))>1) {

          // first histo -> take histogram length
          if (fNumberHisto == 0)
            fLengthHisto = nbins+gpTAsettdc32.tag[i].Histominb;
          // different histogram length?
          else if (fLengthHisto != nbins+gpTAsettdc32.tag[i].Histominb) {
            std::cout << "Different histogram lengths!" << std::endl;
          }
          fNumberHisto++;
        }
      }
    }

  } else {
    tothist = 0;
  }

  // no histograms to process?
  if (tothist <= 0) {
    Clear();
    file_name.close();
    fReadStatus  = "ERROR Less than 1 histogram in "  + fFilename;
    return 5;                                // ERROR number of histograms < 1
  }

  if (tothist > MAXHISTO) {
    std::cout << "ERROR number of histograms " << tothist << " exceedes maximum "
        << MAXHISTO << "! - Setting maximum number " << std::endl;
    tothist = MAXHISTO;
  }

#ifdef MIDEBUG1
  cout << "Number of histograms is " << fNumberHisto << endl;
  cout << "Histogram length is "     << fLengthHisto << endl;
  cout << "Resolutionfactor for t0, fg, lg is " << resolutionfactor << endl;
#endif

  fDefaultBinning = resolutionfactor;

  // allocate histograms
  fHisto.resize(int(fNumberHisto));

  for (i=0; i<fNumberHisto; i++) {
    fHisto[i].resize(fLengthHisto);
  }

  pTATagRec tag;

  fTotalEvents   = 0;

  for (i=0; i<fNumberHisto; i++)
    fEventsPerHisto[i] = 0;

  int ihist = 0;
  Int32 *thist = nullptr;
  std::vector<double> dummy_vector;

  fHistosVector.clear();
  for (i=0,ihist=0; i< tothist; i++) {
    file_name.read((char *)&tag, sizeof tag); // read tag into buffer
    if (file_name.fail()) {
      dummy_vector.clear();
      Clear();
      if (thist != nullptr) {
          delete [] thist;
          thist = nullptr;
      }
      file_name.close();
      fReadStatus  = "ERROR Reading "+fFilename+" tag failed!";
      return 6;                                  // ERROR reading tag failed
    }
    /* read histogram data */
    if (tag.Type == PTATAGC_POSITRON) {
      int nbins;

#ifdef MIDEBUG1
      cout << "Tag[" << i << "] " << tag.Label << " : Histomin = " << tag.Histominb
          << " Histomax = " << tag.Histomaxb << endl;
#endif
      // is a histogram there?
      if ((nbins=(tag.Histomaxb-tag.Histominb + 1))>1) {
        if (thist == nullptr)
          thist = new Int32[nbins];
        if (thist == nullptr) {
          Clear();
          file_name.close();
          fReadStatus = "ERROR Allocating histogram buffer failed!";
          return 3;                                // ERROR allocating histogram buffer
        }

        file_name.read((char *)thist, sizeof(Int32)*nbins);// read hist into buffer
        if (file_name.fail()) {
          Clear();
          if (thist != nullptr) {
            delete [] thist;
            thist = nullptr;
          }
          file_name.close();
          fReadStatus  = "ERROR Reading "+fFilename+" hist failed!";
          return 6;                                  // ERROR reading hist failed
        }

        // for pTA only: use histogram only, if histogram was selected
        // else take all histos but mark not selected
        if (selected[i] || (strcmp(fFormatId,"M3") != 0)) {

          if (ihist < MAXHISTO) { // max number of histos not yet reached?
            dummy_vector.clear();

            strncpy(fLabelsHisto[ihist],tag.Label,MAXLABELSIZE);
            fLabelsHisto[ihist][MAXLABELSIZE-1] = '\0';

            // mark with ** when not selected
            if (!selected[i] && (strlen(fLabelsHisto[ihist])<MAXLABELSIZE-2))
              strcat(fLabelsHisto[ihist],"**");

            // calculate t0, fg, lg for "raw" TDC /pTA actually specified for binned
            // histograms
            // taking largest possible bin value for t0 and fg
            fIntegerT0[ihist] = (tag.t0b+1)*resolutionfactor -1;
            fFirstGood[ihist] = (tag.tfb+1)*resolutionfactor -1;
            fLastGood[ihist]  =  tag.tlb*resolutionfactor;

            // store histogram
            // in case of non zero offset init
            for (j=0; j<tag.Histominb; j++) {
              fHisto[ihist][j]= 0;
              dummy_vector.push_back(double(fHisto[ihist][j]));
            }
            // fill histogram
            for (j=tag.Histominb; j<fLengthHisto; j++) {
              fHisto[ihist][j]= *(thist+j-tag.Histominb);
              dummy_vector.push_back(double(fHisto[ihist][j]));

              // do summation of events between fg and lg
              if ((j >= fFirstGood[ihist]) && (j <= fLastGood[ihist]))
                fEventsPerHisto[ihist] += *(thist+j-tag.Histominb);
            }
            fHistosVector.push_back(dummy_vector);

            // only add selected histo(s) to total events
            if (selected[i])
              fTotalEvents += fEventsPerHisto[ihist];
          }
          ihist++;
        }
      }
    }
  }

  if (thist != nullptr) {
    delete [] thist;
    thist = nullptr;
  }

  file_name.close();

  fReadStatus = "SUCCESS";
  fReadingOk = true;

  return 0;
}

//*******************************
//Implementation WriteMdu
//*******************************

 /*! \brief Method to write a MuSR MDU file
  *
  *  This method gives back:
  *    - 0 for succesful writing
  *    - 1 if the open file action or the writing of the header failed
  *    - 2 for an unsupported version of the data
  *    - 3 for an error when allocating data buffer
  *    - 5 if the number of histograms is less than 1
  *    - 6 if writing data failed
  *
  *  The parameter of the method is a const char * representing the name of the
  *  file to be opened.
  */

int MuSR_td_PSI_bin::WriteMdu(const char * fileName)
{
  std::cerr << std::endl << "MuSR_td_PSI_bin::WriteMdu - not yet implemented" << std::endl;
  return 0;
}


//*******************************
//Implementation ReadingOK
//*******************************

/*! \brief Method to obtain if reading and processing of the data file was OK.
 *
 *  This method gives back:
 *    - true if reading was OK
 *    - false if reading was NOT OK
 */
bool MuSR_td_PSI_bin::ReadingOK() const
{
   return fReadingOk;
}

//*******************************
//Implementation WritingOK
//*******************************

/*! \brief Method to obtain if writing and processing of the data file was OK.
 *
 *  This method gives back:
 *    - true if writing was OK
 *    - false if writing was NOT OK
 */
bool MuSR_td_PSI_bin::WritingOK() const
{
   return fWritingOk;
}

//*******************************
//Implementation CheckDataConsistency
//*******************************

/*! \brief Check if a given set of data is consistent with the PSI-BIN limitations.
 *  If false, the error message can be obtained via ConsistencyStatus().
 *  Comment to the tag: the following restriction apply (in parenthesize the level of checking)
 *  - total number of bins is restricted to 65536 (true for reasonable, and strict)
 *  - 32767 is the maximum number of bins being stored in an integer*2 (true for reasonable, and strict)
 *  - the number of bins must be a multiple of 256 (false for reasonable, true for strict)
 *  - all histograms must have the same number of bins (true for reasonable, and strict)
 *
 * return:
 *  - true if everything is within the PSI-BIN limitations
 *  - false otherwise
 *
 * \param tag tag provided to tell how strict the tests should be. 0=reasonable, 1=strict, 2=loose
 */

bool MuSR_td_PSI_bin::CheckDataConsistency(int tag)
{
  if (fNumberHisto <= 0) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** number histograms is zero or less!";
    return fConsistencyOk;
  }

  if (fNumberHisto > MAXHISTO) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** number of histograms requested: ";
    fConsistencyStatus += fNumberHisto;
    fConsistencyStatus += ", which is larger than the possible maximum of 32.";
    return fConsistencyOk;
  }

  if (fLengthHisto <= 0) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** histogram length is zero or less!";
    return fConsistencyOk;
  }

  if (fLengthHisto > 32767) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** histogram length is too large (maximum being 32767)!";
    return fConsistencyOk;
  }

  if ((fLengthHisto % 256 != 0) && (tag == 1)) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** histogram length is not a multiple of 256!";
    return fConsistencyOk;
  }

  if ((fNumberHisto * fLengthHisto > 65536) && (tag != 2)) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** fNumberHisto * fLengthHisto > 65536!";
    return fConsistencyOk;
  }

  if (fHisto.size() == 0) {
    fConsistencyOk = false;
    fConsistencyStatus  = "**ERROR** no histograms present!";
    return fConsistencyOk;
  }

  fConsistencyOk = true;
  fConsistencyStatus = "SUCCESS";

  return fConsistencyOk;
}


//*******************************
//Implementation ReadStatus
//*******************************

/*! \brief Method to obtain error/success information after reading.
 *
 *  This method gives back:
 *    - "SUCCESS"         if reading was OK
 *    - "ERROR <message>" if reading was NOT OK
 */
std::string MuSR_td_PSI_bin::ReadStatus() const
{
   return fReadStatus;
}

//*******************************
//Implementation WriteStatus
//*******************************

/*! \brief Method to obtain error/success information after writing.
 *
 *  This method gives back:
 *    - "SUCCESS"         if writing was OK
 *    - "ERROR <message>" if writing was NOT OK
 */
std::string MuSR_td_PSI_bin::WriteStatus() const
{
   return fWriteStatus;
}

//*******************************
//Implementation ConsistencyStatus
//*******************************

/*! \brief Method to obtain error/success information on data consistency check.
 *
 *  This method gives back:
 *    - "SUCCESS"         if data are consistent OK
 *    - "ERROR <message>" otherwise
 */
std::string MuSR_td_PSI_bin::ConsistencyStatus() const
{
  return fConsistencyStatus;
}

//*******************************
//Implementation Filename
//*******************************

/*! \brief Method to obtain the file name.
 *
 *  This method gives back:
 *    - <fFilename>
 */
std::string MuSR_td_PSI_bin::Filename() const
{
   return fFilename;
}


//*******************************
//Implementation GetHistoInt
//*******************************

/*! \brief Method to return the value of a single bin as integer.
 *
 *  This method gives back:
 *    - bin value as int
 *    - 0 if an invalid histogram number or bin is choosen
 */
int MuSR_td_PSI_bin::GetHistoInt(int histo_num, int j)
{
    if (!fReadingOk) return 0;

    if (( histo_num < 0) || (histo_num >= int(fNumberHisto)) ||
       (j < 0 ) || (j >= fLengthHisto))
      return 0;
#ifdef MIDEBUG
    cout << "fHistosVector[0][0] = " << fHistosVector[0][0] << endl;
#endif
    return fHisto[histo_num][j];
}

//*******************************
//Implementation GetHisto
//*******************************

/*! \brief Method to return the value of a single bin as double.
 *
 *  This method gives back:
 *    - bin value as double
 *    - 0 if an invalid histogram number or bin is choosen
 */
double MuSR_td_PSI_bin::GetHisto(int histo_num, int j)
{
    if (!fReadingOk) return 0.;

    if (( histo_num < 0) || (histo_num >= int(fNumberHisto)) ||
       (j < 0 ) || (j >= fLengthHisto))
      return 0.;
#ifdef MIDEBUG
    cout << "fHistosVector[0][0] = " << fHistosVector[0][0] << endl;
#endif
    return static_cast<double>(fHisto[histo_num][j]);
}

//*******************************
//Implementation GetHistoArray
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
    histogram \<histo_num\> with binning \<binning\>
 *
 *  This method gives back:
 *    - a double vector
 *    - an empty vector if an invalid histogram number or binning is choosen or
 *      allocation failed
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
    representing the desired histogram number and binning.
 */

std::vector<double> MuSR_td_PSI_bin::GetHistoArray(int histo_num, int binning)
{
  std::vector<double> histo_array;

  if (!fReadingOk) return histo_array;

  if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
    return histo_array;

  histo_array.resize((int)(fLengthHisto/binning));

  if (histo_array.size() == 0) return histo_array;

  for (int i=0; i<int(int(fLengthHisto)/binning); i++) {
    histo_array[i] = 0;
    for (int j = 0; j < binning; j++)
      histo_array[i] += double(fHisto[histo_num][i*binning+j]);
  }

  return histo_array;
}

 //*******************************
 //Implementation PutHistoArrayInt
 //*******************************

 /*! \brief Method to set the histograms which is a vector of vector of int's (histogram).
  * There are two different ways to Get the data conform to PSI-BIN limitations:
  * -# rebin and zero pad the given data. In this case also a couple of other parameters
  *    need to be adopted: T0, first good bin, last good bin, and the time resolution. The
  *    values for these properties needed to be set *before* calling this routine!
  * -# truncate the data. In this it is only checked that T0, first good bin, and last good bin
  *    are staying within proper boundaries.
  *
  *  This method gives back:
  *    - 0 on success
  *    - -1 if number of histograms is out of range
  *    - -2 if not all the histograms given have the same length
  *    - -3 if unsupported tag is found
  *    - -4 failed to allocate memory for the histos
  *
  *  \param histoData vector of the histograms
  *  \param tag 0: rebin and zero pad at need, 1: truncate at need, 2: only make sure that histo length < 32512
  */

 int MuSR_td_PSI_bin::PutHistoArrayInt(std::vector<std::vector<int> > &histoData, int tag)
 {
   // check that the number of histograms are within allowed boundaries
   if ((histoData.size() == 0) || (histoData.size() > 32)) {
     fConsistencyOk = false;
     fConsistencyStatus = "**ERROR** number of histograms out of range! Must be > 0 and < 32.";
     return -1;
   }

   // check that all the given histograms have the same length
   unsigned int size = histoData[0].size();
   bool ok = true;
   for (unsigned int i=1; i<histoData.size(); i++) {
     if (histoData[i].size() != size) {
       ok = false;
       break;
     }
   }
   if (!ok) {
     fConsistencyOk = false;
     fConsistencyStatus = "**ERROR** not all histograms have the same length!";
     return -2;
   }

   // overwrite fNumberHisto
   fNumberHisto = static_cast<int>(histoData.size());

   // calculate the allowed histo length
   int lengthHisto = (65536 / histoData.size()) - ((65536 / histoData.size()) % 256);
   if (lengthHisto > 32512)
     lengthHisto = 32512;
   if (tag == 2) {
     if (lengthHisto > 32512)
       lengthHisto = 32512;
   }

   // calculate the needed data length
   int dataLength = ((int)histoData[0].size());
   int rebin = (int)histoData[0].size() / lengthHisto;
   if (tag == 0) {
     // calculate what rebinning is needed
     if ((int)histoData[0].size() % lengthHisto != 0)
       rebin++;
     if ((((int)histoData[0].size()/rebin) % 256) == 0)
       dataLength = (int)histoData[0].size()/rebin;
     else
       dataLength = (((int)histoData[0].size()/rebin/256)+1)*256;
   } else if ((tag == 1) || (tag == 2)) {
     if (((int)histoData[0].size() % 256) != 0)
       dataLength = (((int)histoData[0].size()/256)+1)*256;
   }

   // overwrite fLengthHisto
   fLengthHisto = dataLength;

   // allocate the necessary memory
   int noOfHistos = histoData.size();
   fHisto.resize(noOfHistos);

   for (unsigned int i=0; i<noOfHistos; i++) {
     fHisto[i].resize(fLengthHisto);
   }

   // check how the data shall be treated
   if (tag == 0) { // rebin and zero pad at need (strict)
     // rebin data such that it is compatible with PSI-BIN
     std::vector< std::vector<int> > data;
     data.resize(fNumberHisto);

     int val = 0;
     for (int i=0; i<fNumberHisto; i++) {
       for (unsigned int j=0; j<histoData[i].size(); j++) {
         if ((j>0) && (j%rebin == 0)) {
           data[i].push_back(val);
           val = 0;
         }
         val += histoData[i][j];
       }
     }

     // fill the histograms
     for (int i=0; i<fNumberHisto; i++) {
       for (int j=0; j<fLengthHisto; j++) {
         if (j<(int)data[i].size())
           fHisto[i][j] = data[i][j];
         else
           fHisto[i][j] = 0;
       }
     }

     // rescale T0, fgb, lgb, and time resolution
     for (int i=0; i<fNumberHisto; i++) {
       fRealT0[i] = fBinWidth*fIntegerT0[i];
       fIntegerT0[i] /= rebin;
       fFirstGood[i] /= rebin;
       fLastGood[i]  /= rebin;
     }
     fBinWidth *= rebin;
   } else if ((tag == 1) || (tag == 2)) { // truncate at need tag=1 (strict), or just fill tag=2 (loose)
     // feed the histograms
     for (int i=0; i<fNumberHisto; i++) {
       for (int j=0; j<fLengthHisto; j++) {
         if (j < histoData[i].size())
           fHisto[i][j] = histoData[i][j];
         else
           fHisto[i][j] = 0;
       }
     }

     // check if T0, fgb, and lgb are still within proper boundaries
     for (int i=0; i<fNumberHisto; i++) {
       if (fIntegerT0[i] > fLengthHisto)
         fIntegerT0[i] = 0;
       fRealT0[i] = fBinWidth*fIntegerT0[i];
       if (fFirstGood[i] > fLengthHisto)
         fFirstGood[i] = 0;
       if (fLastGood[i] > fLengthHisto)
         fLastGood[i] = fLengthHisto-1;
     }
   } else {
     fConsistencyOk = false;
     fConsistencyStatus = "**ERROR** found unsupported tag!";
     return -3;
   }

   return 0;
 }

//*******************************
//Implementation GetHistoVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram 
      \<histo_num\> with binning \<binning\>
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
    representing the desired histogram number and binning.
 */

 std::vector<double> MuSR_td_PSI_bin::GetHistoVector(int histo_num, int binning)
 {
   std::vector<double> histo_vector; //(int(fLengthHisto/binning))

   if (!fReadingOk) return histo_vector;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return histo_vector;

   for (int i = 0; i < int(fLengthHisto/binning); i++)
     histo_vector.push_back(0.);

   for (int i = 0; i < int(fLengthHisto/binning); i++) {
     for (int j = 0; j < binning; j++)
       histo_vector[i] += double(fHisto[histo_num][i*binning+j]);
   }

   return histo_vector;
 }

//*******************************
//Implementation GetHistoVectorNo0
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> but where the bins with 
 *   zero counts are replaced by a count 0.1
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 std::vector<double> MuSR_td_PSI_bin::GetHistoVectorNo0(int histo_num, int binning)
 {
   std::vector<double> histo_vector; //(int(fLengthHisto/binning));

   if (!fReadingOk) return histo_vector;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return histo_vector;

   for (int i = 0; i < int(fLengthHisto/binning); i++)
     histo_vector.push_back(0.);

   for (int i = 0; i < int(fLengthHisto/binning); i++) {
     for (int j = 0; j < binning; j++)
       histo_vector[i] += double(fHisto[histo_num][i*binning+j]);

     if (histo_vector[i] < 0.5 ) {
       histo_vector[i] = 0.1;
     }
   }

   return histo_vector;
 }


//**********************************
//Implementation GetHistoArrayInt
//**********************************

/*! \brief Method to obtain an array of type integer containing the values of the 
 *   histogram \<histo_num\>
 *
 *  This method gives back:
 *    - integer vectors
 *    - an vector of size 0 if an invalid histogram number is choosen or allocate failed
 *
 *  The parameter of the method is the integer \<histo_num\> representing the desired 
 *  histogram number.
 */

 std::vector<int> MuSR_td_PSI_bin::GetHistoArrayInt(int histo_num)
 {
   std::vector<int> histo_array;

   if (!fReadingOk) return histo_array;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto))
     return histo_array;

   histo_array.resize(fLengthHisto);

   if (histo_array.size() == 0)
     return histo_array;

   histo_array = fHisto[histo_num];

   return histo_array;
 }


//*******************************
//Implementation GetHistoFromT0Array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point t0. An \<offset\>
 *   can also be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - a pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen or
 *      allocation failed
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 double* MuSR_td_PSI_bin::GetHistoFromT0Array(int histo_num, int binning, int offset)
 {
   if (!fReadingOk) return nullptr;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   double *histo_fromt0_array =
       new double[int((int(fLengthHisto)-GetT0Int(histo_num)-offset+1)/binning)];

   if (!histo_fromt0_array) return nullptr;

   for (int i = 0; i < int((int(fLengthHisto)-GetT0Int(histo_num)-offset)/binning); i++) {
     histo_fromt0_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_fromt0_array[i] +=
           double(fHisto[histo_num][i*binning+j+GetT0Int(histo_num)+offset]);
   }

   return histo_fromt0_array;
 }


//*******************************
//Implementation GetHistoFromT0Vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram 
 *   \<histo_num\> with binning \<binning\> from the point t0.  An \<offset\> can also 
 *   be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 std::vector<double> MuSR_td_PSI_bin::GetHistoFromT0Vector(int histo_num, int binning, int offset)
 {
   std::vector<double> histo_fromt0_vector; // (int((int(fLengthHisto)-GetT0Int(histo_num)+1)/binning));

   if (!fReadingOk) return histo_fromt0_vector;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return histo_fromt0_vector;

   for (int i = 0; i < int((int(fLengthHisto)-GetT0Int(histo_num)-offset)/binning); i++)
     histo_fromt0_vector.push_back(0.);

   for (int i = 0; i < int((int(fLengthHisto)-GetT0Int(histo_num)-offset)/binning); i++) {
     for (int j = 0; j < binning; j++)
       histo_fromt0_vector[i] +=
           double(fHisto[histo_num][i*binning+j+GetT0Int(histo_num)+offset]);
   }

   return histo_fromt0_vector;
 }


//*******************************
//Implementation GetHistoGoodBinsArray
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point fFirstGood until
 *   fLastGood
 *
 *  This method gives back:
 *    - an pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen or
 *      allocate failed
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 double * MuSR_td_PSI_bin::GetHistoGoodBinsArray(int histo_num, int binning)
 {
   if (!fReadingOk) return nullptr;
   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   double *histo_goodBins_array =
       new double[int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num)+1)/binning)];

   if (!histo_goodBins_array) return nullptr;

   for (int i = 0; i < int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num))/binning); i++) {
     histo_goodBins_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_goodBins_array[i] +=
           double(fHisto[histo_num][i*binning+j+GetFirstGoodInt(histo_num)]);
   }

   return histo_goodBins_array;
 }


//*******************************
//Implementation GetHistoGoodBinsVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point fFirstGood until
 *   fLastGood
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 std::vector<double> MuSR_td_PSI_bin::GetHistoGoodBinsVector(int histo_num, int binning)
 {
   std::vector<double> histo_goodBins_vector;

   if (!fReadingOk) return histo_goodBins_vector;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return histo_goodBins_vector;

   for (int i = 0; i < int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num))/binning); i++)
     histo_goodBins_vector.push_back(0.);

   for (int i = 0; i < int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num))/binning); i++) {
     for (int j = 0; j < binning; j++)
       histo_goodBins_vector[i] +=
           double(fHisto[histo_num][i*binning+j+GetFirstGoodInt(histo_num)]);
   }

   return histo_goodBins_vector;
 }


//*******************************
//Implementation GetHistoFromT0MinusBkgArray
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point t0. A background 
 *   calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> is subtracted. 
 *   An \<offset\> can also be specified (otherwise = 0. 
 *
 *  This method gives back: 
 *    - a pointer of a double array
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *       - allocate failed
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits 
 *  between which the background is calculated.
 */

 double * MuSR_td_PSI_bin::GetHistoFromT0MinusBkgArray(int histo_num,
                                                       int lower_bckgrd, int higher_bckgrd, int binning, int offset)
 {
   if (!fReadingOk) return nullptr;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(fLengthHisto) || lower_bckgrd > higher_bckgrd )
     return nullptr;

   double bckgrd = 0;

   for (int k = lower_bckgrd; k <= higher_bckgrd; k++) {
     bckgrd += double(fHisto[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   double *histo_fromt0_minus_bckgrd_array =
       new double[int((int(fLengthHisto)-GetT0Int(histo_num)-offset+1)/binning)];

   if (!histo_fromt0_minus_bckgrd_array) return NULL;

   for (int i = 0; i < int((int(fLengthHisto)-GetT0Int(histo_num)-offset)/binning); i++) {
     histo_fromt0_minus_bckgrd_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_fromt0_minus_bckgrd_array[i] +=
           double(fHisto[histo_num][i*binning+j+GetT0Int(histo_num)+offset]) - bckgrd;
   }

   return histo_fromt0_minus_bckgrd_array;
 }

//*******************************
//Implementation GetHistoFromT0MinusBkgVector
//*******************************
 
/*! \brief Method to obtain a vector of double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point t0. A background 
 *   calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> is subtracted. 
 *   An \<offset\> can also be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits 
 *  between which the background is calculated.
 */

 std::vector<double> MuSR_td_PSI_bin::GetHistoFromT0MinusBkgVector(int histo_num, int lower_bckgrd,
                                                                   int higher_bckgrd, int binning, int offset)
 {
   std::vector<double> histo_fromt0_minus_bckgrd_vector; // (int((int(fLengthHisto)-GetT0Int(histo_num)+1)/binning));

   if (!fReadingOk) return histo_fromt0_minus_bckgrd_vector;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return histo_fromt0_minus_bckgrd_vector;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(fLengthHisto) || lower_bckgrd > higher_bckgrd )
     return histo_fromt0_minus_bckgrd_vector;

   double bckgrd = 0;
   for (int k = lower_bckgrd; k <= higher_bckgrd; k++) {
     bckgrd += double(fHisto[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   for (int i = 0; i < int((int(fLengthHisto)-GetT0Int(histo_num)-offset)/binning); i++)
     histo_fromt0_minus_bckgrd_vector.push_back(0.);

   for (int i = 0; i < int((int(fLengthHisto)-GetT0Int(histo_num)-offset)/binning); i++) {
     for (int j = 0; j < binning; j++)
       histo_fromt0_minus_bckgrd_vector[i] +=
           double(fHisto[histo_num][i*binning+j+GetT0Int(histo_num)+offset]) - bckgrd;
   }

   return histo_fromt0_minus_bckgrd_vector;
 }


//*******************************
//Implementation GetHistoGoodBinsMinusBkgArray
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point fFirstGood until
 *   the point fLastGood. A background calculated from the points \<lower_bckgrd\> and
 *   \<higher_bckgrd\> is subtracted
 *
 *  This method gives back:
 *    - a pointer of a double array
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
         - allocate failed
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits 
 *  between which the background is calculated.
 */

 double * MuSR_td_PSI_bin::GetHistoGoodBinsMinusBkgArray(int histo_num, int lower_bckgrd,
                                                         int higher_bckgrd, int binning)
 {
   if (!fReadingOk) return nullptr;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(fLengthHisto) || lower_bckgrd > higher_bckgrd )
     return nullptr;

   double bckgrd = 0;
   for (int k = lower_bckgrd; k <= higher_bckgrd; k++) {
     bckgrd += double(fHisto[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   double *histo_goodBins_minus_bckgrd_array =
       new double[int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num)+1)/binning)];

   if (!histo_goodBins_minus_bckgrd_array) return nullptr;

   for (int i = 0; i < int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num))/binning); i++) {
     histo_goodBins_minus_bckgrd_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_goodBins_minus_bckgrd_array[i] +=
           double(fHisto[histo_num][i*binning+j+GetFirstGoodInt(histo_num)]) - bckgrd;
   }

   return histo_goodBins_minus_bckgrd_array;
 }


//*******************************
//Implementation GetHistoGoodBinsMinusBkgVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\>
 *   with binning \<binning\> from the point fFirstGood until the point fLastGood.
 *   A background calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> 
 *   is subtracted
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits 
 *  between which the background is calculated.
 */

 std::vector<double> MuSR_td_PSI_bin::GetHistoGoodBinsMinusBkgVector(int histo_num, int lower_bckgrd,
                                                                     int higher_bckgrd, int binning)
 {
   std::vector<double> histo_goodBins_minus_bckgrd_vector; ;

   if (!fReadingOk) return histo_goodBins_minus_bckgrd_vector;

   if ( histo_num < 0 || histo_num >= int(fNumberHisto) || binning <= 0 )
     return histo_goodBins_minus_bckgrd_vector;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(fLengthHisto) || lower_bckgrd > higher_bckgrd )
     return histo_goodBins_minus_bckgrd_vector;

   double bckgrd = 0;
   for (int k = lower_bckgrd; k <= higher_bckgrd; k++) {
     bckgrd += double(fHisto[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   for (int i = 0; i < int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num))/binning); i++)
     histo_goodBins_minus_bckgrd_vector.push_back(0.);

   for (int i = 0; i < int((GetLastGoodInt(histo_num)-GetFirstGoodInt(histo_num))/binning); i++) {
     for (int j = 0; j < binning; j++)
       histo_goodBins_minus_bckgrd_vector[i] +=
           double(fHisto[histo_num][i*binning+j+GetFirstGoodInt(histo_num)]) - bckgrd;
   }

   return histo_goodBins_minus_bckgrd_vector;
 }


//*******************************
//Implementation GetAsymmetryArray
//*******************************

/*! \brief Method to obtain an array of double containing the values of the asymmetry between 2 histograms.
 *
 *  The asymmetry is calculated between the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *  An \<offset\> value from t_0 can also be specified (otherwise = 0).
 *  Also an \<y_offset\> can be given to shift artificially the curve on the y-axis (otherwise = 0).
 *
 *  This method gives back:
 *    - a array of double
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *       - allocate failed
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and 
 *  \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are 
 *  also required.
 */

 double* MuSR_td_PSI_bin::GetAsymmetryArray(int histo_num_plus, int histo_num_minus, double alpha_param,
                                            int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                            int lower_bckgrd_minus, int higher_bckgrd_minus, int offset,
                                            double y_offset)
 {
   int max_t0 = Tmax(GetT0Int(histo_num_plus),GetT0Int(histo_num_minus));

   if (!fReadingOk) return nullptr;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return nullptr;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return nullptr;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return nullptr;


   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning, offset);
   if (dummy_1 == nullptr) return nullptr;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning, offset);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return nullptr;
   }

   double *asymmetry_array = new double[int((int(fLengthHisto)-max_t0-offset+1)/binning)];

   if (!asymmetry_array) return nullptr;

   for (int i = 0; i < int((int(fLengthHisto)-max_t0)/binning); i++) {
     asymmetry_array[i] = (dummy_1[i] - alpha_param * dummy_2[i]) /
                          (dummy_1[i] + alpha_param * dummy_2[i]) + y_offset;
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_array;
 }


//*******************************
//Implementation GetAsymmetryVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the asymmetry between 2 histograms.
 *
 *  The asymmetry is calculated between the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *  An \<offset\> value from t_0 can also be specified (otherwise = 0).
 *  Also an \<y_offset\> can be given to shift artificially the curve on the y-axis (otherwise = 0).
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 std::vector<double> MuSR_td_PSI_bin::GetAsymmetryVector(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                         int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                         int lower_bckgrd_minus, int higher_bckgrd_minus, int offset, double y_offset)
 {
   int max_t0 = Tmax(GetT0Int(histo_num_plus),GetT0Int(histo_num_minus));

   std::vector<double> asymmetry_vector; // (int((int(fLengthHisto)-max_t0+1)/binning));

   if (!fReadingOk) return asymmetry_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return asymmetry_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return asymmetry_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return asymmetry_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return asymmetry_vector;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning, offset);
   if (dummy_1 == nullptr) return asymmetry_vector;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning, offset);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return asymmetry_vector;
   }

   for (int i = 0; i < int((int(fLengthHisto)-max_t0-offset)/binning); i++)
     asymmetry_vector.push_back(0.);

   for (int i = 0; i < int((int(fLengthHisto)-max_t0-offset)/binning); i++) {
     asymmetry_vector[i] = (dummy_1[i] - alpha_param * dummy_2[i]) /
                           (dummy_1[i] + alpha_param * dummy_2[i]) + y_offset;
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_vector;
 }


//*******************************
//Implementation GetErrorAsymmetryArray
//*******************************

/*! \brief Method to obtain an array of double containing the values of the error of the asymmetry between 2 histograms.
 *
 *  The error of the asymmetry is calculated with the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *  An \<offset\> value from t_0 can also be specified (otherwise = 0)
 *
 *  This method gives back:
 *    - a array of double
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
         - allocate failed
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 double * MuSR_td_PSI_bin::GetErrorAsymmetryArray(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                  int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                  int lower_bckgrd_minus, int higher_bckgrd_minus, int offset)
 {
   int max_t0 = Tmax(GetT0Int(histo_num_plus),GetT0Int(histo_num_minus));

   if (!fReadingOk) return nullptr;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return nullptr;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return nullptr;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return nullptr;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning, offset);
   if (dummy_1 == nullptr) return nullptr;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning, offset);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return nullptr;
   }

   double *error_asymmetry_array = new double[int((int(fLengthHisto)-max_t0-offset+1)/binning)];

   if (!error_asymmetry_array) return nullptr;

   for (int i = 0; i < int((fLengthHisto-max_t0-offset)/binning); i++) {
     if (dummy_1[i] < 0.5 || dummy_2[i] < 0.5 )
       error_asymmetry_array[i] = 1.0;
     else
       error_asymmetry_array[i] = double(2.) * alpha_param * sqrt(dummy_1[i]*dummy_2[i]*(dummy_1[i]+dummy_2[i])) /
                                  pow(dummy_1[i] + alpha_param * dummy_2[i],2.);
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return error_asymmetry_array;
 }


//*******************************
//Implementation GetErrorAsymmetryVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the error of the asymmetry between 2 histograms.
 *
 *  The error of the asymmetry is calculated with the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *  An \<offset\> value from t_0 can also be specified (otherwise = 0)
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 std::vector<double> MuSR_td_PSI_bin::GetErrorAsymmetryVector(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                              int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                              int lower_bckgrd_minus, int higher_bckgrd_minus, int offset)
 {
   int max_t0 = Tmax(GetT0Int(histo_num_plus),GetT0Int(histo_num_minus));

   std::vector<double> error_asymmetry_vector; //(int((int(fLengthHisto)-max_t0+1)/binning));

   if (!fReadingOk) return error_asymmetry_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return error_asymmetry_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return error_asymmetry_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return error_asymmetry_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return error_asymmetry_vector;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning, offset);
   if (dummy_1 == nullptr) return error_asymmetry_vector;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning, offset);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return error_asymmetry_vector;
   }

   for (int i = 0; i < int((int(fLengthHisto)-max_t0-offset)/binning); i++)
     error_asymmetry_vector.push_back(0.);

   for (int i = 0; i < int((int(fLengthHisto-max_t0-offset))/binning); i++) {
     if (dummy_1[i] < 0.5 || dummy_2[i] < 0.5 )
       error_asymmetry_vector[i] = 1.0;
     else
       error_asymmetry_vector[i] = double(2.) * alpha_param * sqrt(dummy_1[i]*dummy_2[i]*(dummy_1[i]+dummy_2[i])) /
                                   pow(dummy_1[i] + alpha_param * dummy_2[i],2.);
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return error_asymmetry_vector;
 }


//*******************************
//Implementation GetAsymmetryGoodBinsArray
//*******************************

/*! \brief Method to obtain an array of double containing the values of the asymmetry between 2 histograms.
 *
 *  The array has a size corresponding to the minimum interval between fFirstGood and fLastGood.
 *  It begins at the fFirstGood coming at the latest after the corresponding t_0.
 *  The asymmetry is calculated between the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *
 *  This method gives back:
 *    - a array of double
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
         - allocate failed
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 double * MuSR_td_PSI_bin::GetAsymmetryGoodBinsArray(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                     int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                     int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((Tmin(GetLastGoodInt(histo_num_plus)-GetFirstGoodInt(histo_num_plus),
                         GetLastGoodInt(histo_num_minus)-GetFirstGoodInt(histo_num_minus))+1)/binning);

   if (!fReadingOk) return nullptr;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return nullptr;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return nullptr;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return nullptr;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning);
   if (dummy_1 == nullptr) return nullptr;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return nullptr;
   }

   int hstart = Tmax(GetFirstGoodInt(histo_num_plus)-GetT0Int(histo_num_plus),GetFirstGoodInt(histo_num_minus)-GetT0Int(histo_num_minus));

   double *asymmetry_goodBins_array = new double[hsize];

   if (!asymmetry_goodBins_array) return nullptr;

   for (int i = 0; i < hsize; i++) {
     asymmetry_goodBins_array[i] = (dummy_1[i+hstart] - alpha_param * dummy_2[i+hstart]) /
                                   (dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart]);
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_goodBins_array;
 }


//*******************************
//Implementation GetAsymmetryGoodBinsVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the asymmetry between 2 histograms.
 *
 *  The vector has a size corresponding to the minimum interval between fFirstGood and fLastGood.
 *  It begins at the fFirstGood coming at the latest after the corresponding t_0.
 *  The asymmetry is calculated between the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 std::vector<double> MuSR_td_PSI_bin::GetAsymmetryGoodBinsVector(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                                 int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                                 int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((Tmin(GetLastGoodInt(histo_num_plus)-GetFirstGoodInt(histo_num_plus),
                         GetLastGoodInt(histo_num_minus)-GetFirstGoodInt(histo_num_minus))+1)/binning);

   std::vector<double> asymmetry_goodBins_vector;

   if (!fReadingOk) return asymmetry_goodBins_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return asymmetry_goodBins_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return asymmetry_goodBins_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return asymmetry_goodBins_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return asymmetry_goodBins_vector;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning);
   if (dummy_1 == nullptr) return asymmetry_goodBins_vector;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return asymmetry_goodBins_vector;
   }

   for (int i = 0; i < hsize; i++)
     asymmetry_goodBins_vector.push_back(0.);

   int hstart = Tmax(GetFirstGoodInt(histo_num_plus)-GetT0Int(histo_num_plus),GetFirstGoodInt(histo_num_minus)-GetT0Int(histo_num_minus));

   for (int i = 0; i < hsize; i++) {
     asymmetry_goodBins_vector[i] = (dummy_1[i+hstart] - alpha_param * dummy_2[i+hstart]) /
                                    (dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart]);
   }
   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_goodBins_vector;
 }


//*******************************
//Implementation GetErrorAsymmetryGoodBinsArray
//*******************************

/*! \brief Method to obtain an array of double containing the values of the error of the asymmetry between 2 histograms.
 *
 *  The size is calculated as the asymmetry array.
 *  The error of the asymmetry is calculated with the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
 *  \<alpha_param\>. This method requires also a binning value \<binning\>, as well as the background limits for both histograms.
 *
 *  This method gives back:
 *    - a array of double
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *       - allocate failed
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms are also required.
 */

 double * MuSR_td_PSI_bin::GetErrorAsymmetryGoodBinsArray(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                          int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                          int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((Tmin(GetLastGoodInt(histo_num_plus)
                         -GetFirstGoodInt(histo_num_plus),
                         GetLastGoodInt(histo_num_minus)
                         -GetFirstGoodInt(histo_num_minus))+1)/binning);

   if (!fReadingOk) return nullptr;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return nullptr;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return nullptr;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return nullptr;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return nullptr;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus,
                                                 lower_bckgrd_plus, higher_bckgrd_plus, binning);
   if (dummy_1 == nullptr) return nullptr;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus,
                                                 lower_bckgrd_minus, higher_bckgrd_minus, binning);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return nullptr;
   }
   int hstart = Tmax(GetFirstGoodInt(histo_num_plus)-GetT0Int(histo_num_plus),
                     GetFirstGoodInt(histo_num_minus)-GetT0Int(histo_num_minus));

   double *error_asymmetry_goodBins_array = new double[hsize];

   if (!error_asymmetry_goodBins_array) return nullptr;

   for (int i = 0; i < hsize; i++) {
     if (dummy_1[i+hstart] < 0.5 || dummy_2[i+hstart] < 0.5 )
       error_asymmetry_goodBins_array[i] = 1.0;
     else
       error_asymmetry_goodBins_array[i] =
           double(2.) * alpha_param * sqrt(dummy_1[i+hstart]*dummy_2[i+hstart]
                                           *(dummy_1[i+hstart]+dummy_2[i+hstart])) /
           pow(dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart],2.);
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return error_asymmetry_goodBins_array;
 }


//*******************************
//Implementation GetErrorAsymmetryGoodBinsVector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the error of 
 *   the asymmetry between 2 histograms.
 *
 *  The size is calculated as the asymmetry array.
 *  The error of the asymmetry is calculated with the histograms \<histo_num_plus\> and 
 *  \<histo_num_minus\> with an alpha parameter \<alpha_param\>. This method requires 
 *  also a binning value \<binning\>, as well as the background limits for both 
 *  histograms.
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and 
 *  \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 std::vector<double> MuSR_td_PSI_bin::GetErrorAsymmetryGoodBinsVector(int histo_num_plus,
                                                                      int histo_num_minus, double alpha_param,
                                                                      int binning, int lower_bckgrd_plus,
                                                                      int higher_bckgrd_plus,
                                                                      int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((Tmin(GetLastGoodInt(histo_num_plus)-GetFirstGoodInt(histo_num_plus),
                         GetLastGoodInt(histo_num_minus)-GetFirstGoodInt(histo_num_minus))+1)/binning);

   std::vector<double> error_asymmetry_goodBins_vector;

   if (!fReadingOk) return error_asymmetry_goodBins_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(fNumberHisto) || binning <= 0 )
     return error_asymmetry_goodBins_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(fNumberHisto) )
     return error_asymmetry_goodBins_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(fLengthHisto) || lower_bckgrd_plus > higher_bckgrd_plus )
     return error_asymmetry_goodBins_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(fLengthHisto) || lower_bckgrd_minus > higher_bckgrd_minus )
     return error_asymmetry_goodBins_vector;

   double *dummy_1 = GetHistoFromT0MinusBkgArray(histo_num_plus, lower_bckgrd_plus,
                                                 higher_bckgrd_plus, binning);
   if (dummy_1 == nullptr) return error_asymmetry_goodBins_vector;

   double *dummy_2 = GetHistoFromT0MinusBkgArray(histo_num_minus, lower_bckgrd_minus,
                                                 higher_bckgrd_minus, binning);
   if (dummy_2 == nullptr) {
     delete [] dummy_1;
     return error_asymmetry_goodBins_vector;
   }

   for (int i = 0; i < hsize; i++)
     error_asymmetry_goodBins_vector.push_back(0.);

   int hstart = Tmax(GetFirstGoodInt(histo_num_plus)-GetT0Int(histo_num_plus),GetFirstGoodInt(histo_num_minus)-GetT0Int(histo_num_minus));

   for (int i = 0; i < hsize; i++) {
     if (dummy_1[i+hstart] < 0.5 || dummy_2[i+hstart] < 0.5 )
       error_asymmetry_goodBins_vector[i] = 1.0;
     else
       error_asymmetry_goodBins_vector[i] = double(2.) * alpha_param 
                                            * sqrt(dummy_1[i+hstart]*dummy_2[i+hstart]*(dummy_1[i+hstart]+dummy_2[i+hstart])) /
                                            pow(dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart],2.);
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return error_asymmetry_goodBins_vector;
 }


//*******************************
//Implementation GetNumberScalerInt
//*******************************

/*! \brief Method returning an integer representing the number of histograms
 */

 int MuSR_td_PSI_bin::GetNumberScalerInt()
 {
   return int(fNumberScaler);
 }

 //*******************************
 //Implementation PutNumberScalerInt
 //*******************************

 /*! \brief Method seting the number of scalers present
  *
  * return:
  *  - 0 on success
  *  - -1 if val is out of range (32)
  *
  * \param val the number of scalers present
  */

 int MuSR_td_PSI_bin::PutNumberScalerInt(int val)
 {
   if ((val < 0) || (val >= MAXSCALER))
     return -1;

   fNumberScaler = val;

   return 0;
 }

//*******************************
//Implementation GetScalersVector
//*******************************

/*! \brief Method providing a vector of long containing the values of the scalers
 */

 std::vector<long> MuSR_td_PSI_bin::GetScalersVector()
 {
   std::vector<long> scalers_vect(fNumberScaler);

   for ( int i = 0; i < fNumberScaler; i++ )
     scalers_vect[i] = long(fScalers[i]);

   return scalers_vect;
 }

 //*******************************
 //Implementation PutScalersVector
 //*******************************

 /*! \brief Method set a vector of long containing the values of the scalers
  *
  * return:
  *  - 0 on success
  *  - -1 if scalerData vector is too long (>MAXSCALER)
  *
  * \param scalerData vector
  */

 int MuSR_td_PSI_bin::PutScalersVector(std::vector<int> scalerData)
 {
   if ((int)scalerData.size() > MAXSCALER)
     return -1;

   for (unsigned int i=0; i<scalerData.size(); i++)
     fScalers[i] = scalerData[i];

   return 0;
 }

//*******************************
//Implementation GetMaxT0Int
//*******************************

/*! \brief Method to determine the maximum value of the t0 bins
 */

 int MuSR_td_PSI_bin::GetMaxT0Int()
 {
   int max_t0 = 0;

   for (int i = 0; i < int(fNumberHisto); i++) {
     if (int(fIntegerT0[i]) > max_t0)
       max_t0 = int(fIntegerT0[i]);
   }
   return max_t0;
 }


//*******************************
//Implementation GetMax2T0Int
//*******************************

/*! \brief Method to determine the maximum value of the last good bins of 2 histograms
 *
 *  returns -1 if the numbers of the histograms are invalid
 */

 int MuSR_td_PSI_bin::GetMax2T0Int(int k, int j)
 {
   if (( k < 0 || k >= int(fNumberHisto)) || ( j < 0 || j >= int(fNumberHisto)))
     return -1;

   int max_t0 = int(fIntegerT0[j]);
   if (int(fIntegerT0[k]) >= max_t0)
     max_t0 = int(fIntegerT0[k]);

   return max_t0;
 }

//*******************************
//Implementation GetMin2T0Int
//*******************************

/*! \brief Method to determine the minimum value of the last good bins of 2 histograms
 *
 *  returns -1 if the numbers of the histograms are invalid
 */

 int MuSR_td_PSI_bin::GetMin2T0Int(int k, int j)
 {
   if (( k < 0 || k >= int(fNumberHisto)) || ( j < 0 || j >= int(fNumberHisto)))
     return -1;

   int min_t0 = int(fIntegerT0[j]);
   if (int(fIntegerT0[k]) <= min_t0)
     min_t0 = int(fIntegerT0[k]);

   return min_t0;
 }

//*******************************
//Implementation GetMinT0Int
//*******************************

/*! \brief Method to determine the minimum value of the t0 bins
 */

 int MuSR_td_PSI_bin::GetMinT0Int()
 {
   int min_t0 = int(fLengthHisto);

   for (int i = 0; i < int(fNumberHisto); i++) {
     if (int(fIntegerT0[i]) < min_t0)
       min_t0 = int(fIntegerT0[i]);
   }

   return min_t0;
 }


//*******************************
//Implementation GetBinWidthPicoSec
//*******************************

/*! \brief Method returning a double representing the bin-width in picoseconds
 */

 double MuSR_td_PSI_bin::GetBinWidthPicoSec()
 {
   return fBinWidth*1.0e6;
 }

 //*******************************
 //Implementation PutBinWidthPicoSec
 //*******************************

 /*! \brief Method setting a double representing the bin-width in picoseconds
  *
  * \param binWidth bin-width given in picoseconds
  */

 void MuSR_td_PSI_bin::PutBinWidthPicoSec(double binWidth)
 {
   fBinWidth = binWidth*1.0e-6;
 }

//*******************************
//Implementation GetBinWidthNanoSec
//*******************************

/*! \brief Method returning a double representing the bin-width in nanoseconds
 */

 double MuSR_td_PSI_bin::GetBinWidthNanoSec()
 {
   return fBinWidth*1.0e3;
 }

 //*******************************
 //Implementation PutBinWidthNanoSec
 //*******************************

 /*! \brief Method setting a double representing the bin-width in nanoseconds
  *
  * \param binWidth bin-width given in nanoseconds
  */

 void MuSR_td_PSI_bin::PutBinWidthNanoSec(double binWidth)
 {
   fBinWidth = binWidth*1.0e-3;
 }

//*******************************
//Implementation GetBinWidthMicroSec
//*******************************

/*! \brief Method returning a double representing the bin-width in microseconds
 */

 double MuSR_td_PSI_bin::GetBinWidthMicroSec()
 {
   return fBinWidth;
 }

 //*******************************
 //Implementation PutBinWidthMicroSec
 //*******************************

 /*! \brief Method setting a double representing the bin-width in microseconds
  *
  * \param binWidth bin-width given in microseconds
  */

 void MuSR_td_PSI_bin::PutBinWidthMicroSec(double binWidth)
 {
   fBinWidth = binWidth;
 }

//*******************************
//Implementation GetEventsHistoLong
//*******************************

/*! \brief Method returning a long representing the number of events in a specified histograms
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 long MuSR_td_PSI_bin::GetEventsHistoLong(int i)
 {
   if ( i < 0 || i >= fNumberHisto)
     return -1;
   else
     return long(fEventsPerHisto[i]);
 }

//*******************************
//Implementation GetEventsHistoVector
//*******************************

/*! \brief Method returning a vector of long containing the number of events in the histograms
 */

 std::vector<long> MuSR_td_PSI_bin::GetEventsHistoVector()
 {
   std::vector<long> eventsHisto(fNumberHisto);

   for ( int i = 0; i < fNumberHisto; i++ )
     eventsHisto[i] = long(fEventsPerHisto[i]);
   return eventsHisto;
 }

//*******************************
//Implementation GetT0Double
//*******************************

/*! \brief Method returning a double representing the t0 point (from the "real" t0 in the header) for a specified histogram
 *
 *  A value of -1. is returned if the value of the histogram \<i\> specified is invalid.
 */

 double MuSR_td_PSI_bin::GetT0Double(int i)
 {
   if ( i < 0 || i >= int(fNumberHisto))
     return -1.;
   else
     return double(fRealT0[i]);
 }


//*******************************
//Implementation GetDefaultBinning
//*******************************

/*! \brief Method returning an integer representing the default binning
 *
 */

 int MuSR_td_PSI_bin::GetDefaultBinning()
 {
   if (fDefaultBinning < 1)
     return 1;
   else
     return fDefaultBinning;
 }

//*******************************
//Implementation GetT0Int
//*******************************

/*! \brief Method returning an integer representing the t0 point (from the "integer" t0 in the header) for a specified histogram
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 int MuSR_td_PSI_bin::GetT0Int(int i)
 {
   if ( i < 0 || i >= int(fNumberHisto))
     return -1;
   else
     return int(fIntegerT0[i]);
 }

 //*******************************
 //Implementation PutT0Int
 //*******************************

 /*! \brief Method setting an integer representing the t0 point (from the "integer" t0 in the header) for a specified histogram
  *
  *  return:
  *    - 0 on success
  *    - -1 if histoNo is out of range
  *
  * \param histoNo histogram number
  * \param t0 t0 bin value
  */

 int MuSR_td_PSI_bin::PutT0Int(int histoNo, int t0)
 {
   if ((histoNo < 0) || (histoNo >= MAXHISTO))
     return -1;

   fIntegerT0[histoNo] = t0;

   return 0;
 }

//*******************************
//Implementation GetT0Vector
//*******************************

/*! \brief Method returning a vector of integer containing the t0 values of the histograms specified in the header
 */

 std::vector<int> MuSR_td_PSI_bin::GetT0Vector()
 {
   std::vector<int> t0(fNumberHisto);

   for ( int i = 0; i < int(fNumberHisto); i++ )
     t0[i] = int(fIntegerT0[i]);

   return t0;
 }

 //*******************************
 //Implementation PutT0Vector
 //*******************************

 /*! \brief Method setting a vector of integer containing the t0 values of the histograms specified in the header
  *
  * return:
  *  - 0 on success
  *  - -1 if t0Data is too long (>MAXHISTO)
  */

 int MuSR_td_PSI_bin::PutT0Vector(std::vector<int> &t0Data)
 {
   if (static_cast<int>(t0Data.size()) >= MAXHISTO)
     return -1;

   for (unsigned int i=0; i<t0Data.size(); i++)
     fIntegerT0[i] = t0Data[i];

   return 0;
 }

//*******************************
//Implementation GetFirstGoodInt
//*******************************

/*! \brief Method returning an integer representing the first good bin specified in the header for a specified histogram
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 int MuSR_td_PSI_bin::GetFirstGoodInt(int i)
 {
   if ( i < 0 || i >= int(fNumberHisto))
     return -1;
   else
     return int(fFirstGood[i]);
 }

 //*******************************
 //Implementation PutFirstGoodInt
 //*******************************

 /*! \brief Method setting an integer representing the first good bin specified in the header for a specified histogram
  *
  *  return:
  *   - 0 on success
  *   - -1 if histo index is out of range
  *
  * \param i index of the histogram
  * \param j first good bin value
  */

 int MuSR_td_PSI_bin::PutFirstGoodInt(int i, int j)
 {
   if ((i < 0) || (i >= MAXHISTO))
     return -1;

   fFirstGood[i] = j;

   return 0;
 }

//*******************************
//Implementation GetFirstGoodVector
//*******************************

/*! \brief Method returning a vector of integer containing the first good bin values of the histograms specified in the header
 */

 std::vector<int> MuSR_td_PSI_bin::GetFirstGoodVector()
 {
   std::vector<int> firstGood(fNumberHisto);

   for ( int i = 0; i < fNumberHisto; i++ )
     firstGood[i] = int(fFirstGood[i]);

   return firstGood;
 }

//*******************************
//Implementation GetLastGoodInt
//*******************************

/*! \brief Method returning an integer representing the last good bin specified in the header for a specified histogram
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 int MuSR_td_PSI_bin::GetLastGoodInt(int i)
 {
   if ( i < 0 || i >= int(fNumberHisto))
     return -1;
   else
     return int(fLastGood[i]);
 }

//*******************************
//Implementation GetLastGoodVector
//*******************************

/*! \brief Method returning a vector of integer containing the last good bin values of the histograms specified in the header
 */

 std::vector<int> MuSR_td_PSI_bin::GetLastGoodVector()
 {
   std::vector<int> lastGood(fNumberHisto);

   for ( int i = 0; i < fNumberHisto; i++ )
     lastGood[i] = int(fLastGood[i]);

   return lastGood;
 }

//*******************************
//Implementation GetMaxLastGoodInt
//*******************************

/*! \brief Method returning an integer containing the maximum value of the "last good bins" of all histograms
 */

 int MuSR_td_PSI_bin::GetMaxLastGoodInt()
 {
   int max_lastGood = 0;

   for (int i = 0; i < int(fNumberHisto); i++) {
     if (int(fLastGood[i]) > max_lastGood)
       max_lastGood = int(fLastGood[i]);
   }

   return max_lastGood;
 }

//*******************************
//Implementation GetMax2LastGoodInt
//*******************************

/*! \brief Method to determine the maximum value of the "last good bins" of 2 histograms
 *
 *  returns -1 if something is invalid
 */

 int MuSR_td_PSI_bin::GetMax2LastGoodInt(int k, int j)
 {
   if (( k < 0 || k >= int(fNumberHisto)) || ( j < 0 || j >= int(fNumberHisto)))
     return -1;
   else {
     int max_lastGood = int(fLastGood[j]);

     if (int(fLastGood[k]) > max_lastGood)
       max_lastGood = int(fLastGood[k]);

     return max_lastGood;
   }
 }

//*******************************
//Implementation GetMinLastGoodInt
//*******************************

/*! \brief Method providing the minimum value of the last good bins
 */

 int MuSR_td_PSI_bin::GetMinLastGoodInt()
 {
   int min_lastGood = int(fLastGood[0]);

   for (int i = 1; i < int(fNumberHisto); i++) {
     if (int(fLastGood[i]) < min_lastGood)
       min_lastGood = int(fLastGood[i]);
   }

   return min_lastGood;
 }

//*******************************
//Implementation GetMin2LastGoodInt
//*******************************

/*! \brief Method to determine the minimum value of the last good bins of 2 histograms
 *
 *  returns -1 if something is invalid
 */

 int MuSR_td_PSI_bin::GetMin2LastGoodInt(int k, int j)
 {
   if (( k < 0 || k >= int(fNumberHisto)) || ( j < 0 || j >= int(fNumberHisto)))
     return -1;
   else {
     int min_lastGood = int(fLastGood[j]);

     if (int(fLastGood[k]) < min_lastGood)
       min_lastGood = int(fLastGood[k]);

     return min_lastGood;
   }
 }

//*******************************
//Implementation PutLastGoodInt
//*******************************

/*! \brief Method to modify the last good bin (value \<j\>) of the histogram \<i\>
 *
 *  returns -1 if the histogram specified was invalid
 */

 int MuSR_td_PSI_bin::PutLastGoodInt(int i, int j)
 {
   if ((i < 0) || (i >= fNumberHisto))
     return -1;

   fLastGood[i] = j;

   return  0;
 }

//*******************************
//Implementation PutRunNumberInt
//*******************************

/*! \brief Method to modify the run number (value \<i\>)
 *
 *  returns -1 if the integer specified was wrong
 */

 int MuSR_td_PSI_bin::PutRunNumberInt(int i)
 {
   if (i <= 0 )
     return -1;

   fNumRun = i;

   return 0;
 }


//*******************************
//Implementation GetSample()
//*******************************

/*! \brief Method returning a string containing the fSample name
 */

 std::string MuSR_td_PSI_bin::GetSample()
 {
   std::string strData;

   strData = fSample;

   return strData;
 }

 //*******************************
 //Implementation PutSample()
 //*******************************

 /*! \brief Method setting a string containing the fSample name
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided fSample string is too long (>= 11 char)
  *
  * \param fSample string containing the fSample information
  */

 int MuSR_td_PSI_bin::PutSample(std::string sampleStr)
 {
   if (sampleStr.size() >= 11)
     return -1;

   strcpy(fSample, sampleStr.c_str());

   return 0;
 }

//*******************************
//Implementation GetTemp()
//*******************************

/*! \brief Method returning a string containing the temperature specified in the title
 */

 std::string MuSR_td_PSI_bin::GetTemp()
 {
   std::string strData;

   strData = fTemp;

   return strData;
 }

 //*******************************
 //Implementation PutTemp()
 //*******************************

 /*! \brief Method setting a string containing the fSample temperature
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided temperature string is too long (>= 11 char)
  *
  * \param tempStr string containing the fSample information
  */

 int MuSR_td_PSI_bin::PutTemp(std::string tempStr)
 {
   if (tempStr.size() >= 11)
     return -1;

   strcpy(fTemp, tempStr.c_str());

   return 0;
 }

//*******************************
//Implementation GetOrient()
//*******************************

/*! \brief Method returning a string containing the orientation specified in the title
 */

 std::string MuSR_td_PSI_bin::GetOrient()
 {
   std::string strData;

   strData = fOrient;

   return strData;
 }

 //*******************************
 //Implementation PutOrient()
 //*******************************

 /*! \brief Method setting a string containing the fSample orientation
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided orientation string is too long (>= 11 char)
  *
  * \param orientStr string containing the fSample information
  */

 int MuSR_td_PSI_bin::PutOrient(std::string orientStr)
 {
   if (orientStr.size() >= 11)
     return -1;

   strcpy(fOrient, orientStr.c_str());

   return 0;
 }

//*******************************
//Implementation GetField()
//*******************************

/*! \brief Method returning a string containing the field specified in the title
 */

 std::string MuSR_td_PSI_bin::GetField()
 {
   std::string strData;

   strData = fField;

   return strData;
 }

 //*******************************
 //Implementation PutField()
 //*******************************

 /*! \brief Method setting a string containing the field
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided field string is too long (>= 11 char)
  *
  * \param fieldStr string containing the field
  */

 int MuSR_td_PSI_bin::PutField(std::string fieldStr)
 {
   if (fieldStr.size() >= 11)
     return -1;

   strcpy(fField, fieldStr.c_str());

   return 0;
 }

 //*******************************
 //Implementation GetSetup()
 //*******************************

 /*! \brief Method returning a string containing the setup
  */

  std::string MuSR_td_PSI_bin::GetSetup()
  {
    std::string strData;

    strData = fSetup;

    return strData;
  }

  //*******************************
  //Implementation PutSetup()
  //*******************************

  /*! \brief Method setting a string containing the setup
   *
   * return:
   *  - 0 on success
   *  - -1 if the provided setup string is too long (>= 11 char)
   *
   * \param commentStr string containing the setup
   */

  int MuSR_td_PSI_bin::PutSetup(std::string setupStr)
  {
    if (setupStr.size() >= 11)
      return -1;

    strcpy(fSetup, setupStr.c_str());

    return 0;
  }

//*******************************
//Implementation GetComment()
//*******************************

/*! \brief Method returning a string containing the comment specified in the title
 */

 std::string MuSR_td_PSI_bin::GetComment()
 {
   std::string strData;

   strData = fComment;

   return strData;
 }

 //*******************************
 //Implementation PutComment()
 //*******************************

 /*! \brief Method setting a string containing the comment
  *
  * return:
  *  - 0 on success
  *
  * \param commentStr string containing the comment
  */

 int MuSR_td_PSI_bin::PutComment(std::string commentStr)
 {
   strncpy(fComment, commentStr.c_str(), 62);
   fComment[62] = '\0';

   return 0;
 }

//*******************************
//Implementation Get_nameHisto()
//*******************************

/*! \brief Method returning a string containing the name of the histogram \<i\>
 *
 *  returns an empty string if the histogram specified is invalid
 */

 std::string MuSR_td_PSI_bin::GetNameHisto(int i)
 {
   std::string strData{""};

   if ((i >= 0) && (i < fNumberHisto))
     strData = fLabelsHisto[i];

   return strData;
 }

 //*******************************
 //Implementation PutNameHisto()
 //*******************************

 /*! \brief Method setting a string containing the name of the histogram \<i\>
  *
  *  returns:
  *   - 0 on success
  *   - -1 index i out of range
  *   - -2 histoName is too long
  *
  * \param histoName name of the histogram
  * \param i index of the histogram
  */

 int MuSR_td_PSI_bin::PutNameHisto(std::string histoName, int i)
 {
   if ((i<0) || (i>=fNumberHisto))
     return -1;

   if (static_cast<int>(histoName.length()) >= MAXLABELSIZE)
     return -2;

   strcpy(fLabelsHisto[i], histoName.c_str());

   return 0;
 }

//*******************************
//Implementation GetHistoNamesVector()
//*******************************

/*! \brief Method returning a vector of strings containing the names of the histograms
 */

 std::vector<std::string> MuSR_td_PSI_bin::GetHistoNamesVector()
 {
   std::vector<std::string> str_Vector;

   std::string strData;
   for (int i = 0; i < fNumberHisto; i++) {
     strData = fLabelsHisto[i];
     str_Vector.push_back(strData);
   }

   return str_Vector;
 }

 //*******************************
 //Implementation PutHistoNamesVector()
 //*******************************

 /*! \brief Method setting a vector containing the names of all the histograms
  *
  *  returns:
  *   - 0 on success
  *   - -1 size of vector larger than number of histograms
  *   - -2 length of label too large
  *
  * \param histoName name of the histogram
  */

 int MuSR_td_PSI_bin::PutHistoNamesVector(std::vector<std::string> &histoNames)
 {
   if (static_cast<int>(histoNames.size()) > fNumberHisto)
     return -1;

   for (unsigned int i=0; i<histoNames.size(); i++) {
     if (static_cast<int>(histoNames[i].length()) >= MAXLABELSIZE)
       return -2;
     else
       strcpy(fLabelsHisto[i], histoNames[i].c_str());
   }

   return 0;
 }

//*******************************
//Implementation GetScalersNamesVector()
//*******************************

/*! \brief Method returning a vector of strings containing the names of the scalers
 */

 std::vector<std::string> MuSR_td_PSI_bin::GetScalersNamesVector()
 {
   std::vector<std::string> str_Vector;

   std::string strData;
   for (int i = 0; i < fNumberScaler; i++) {
     strData = fLabelsScalers[i];
     str_Vector.push_back(strData);
   }

   return str_Vector;
 }

 //*******************************
 //Implementation PutScalersNamesVector()
 //*******************************

 /*! \brief Method setting a vector of strings containing the names of the scalers
  *
  * return:
  *  - 0 on success
  *  - -1 of scalersName is too long (>MAXSCALER)
  *
  * \param scalersName scaler names
  */

 int MuSR_td_PSI_bin::PutScalersNamesVector(std::vector<std::string> scalersName)
 {
   if (static_cast<int>(scalersName.size()) > MAXSCALER)
     return -1;

   for (unsigned int i=0; i<scalersName.size(); i++) {
     strncpy(fLabelsScalers[i], scalersName[i].c_str(), MAXLABELSIZE-1);
     fLabelsScalers[i][MAXLABELSIZE-1] = '\0';
   }

   return 0;
 }

 //*******************************
 //Implementation PutNumberTemperatureInt
 //*******************************

 /*! \brief Method setting an integer representing the number of temperatures
  *
  * return:
  *  - 0 on success
  *  - -1 if noOfTemps is out of range
  *
  * \param noOfTemps number of temperatures present
  */

 int MuSR_td_PSI_bin::PutNumberTemperatureInt(int noOfTemps)
 {
   if ((noOfTemps < 0) || (noOfTemps > MAXTEMPER))
     return -1;

   fNumberTemper = noOfTemps;

   return 0;
 }

//*******************************
//Implementation GetTemperaturesVector()
//*******************************

/*! \brief Method returning a vector of doubles containing monitored values (usually temperatures)
 */

 std::vector<double> MuSR_td_PSI_bin::GetTemperaturesVector()
 {
   std::vector<double> dbl_Temper;

   for (int i = 0; i < fNumberTemper; i++) {
     dbl_Temper.push_back(double(fTemper[i]));
   }

   return dbl_Temper;
 }

 //*******************************
 //Implementation PutTemperaturesVector()
 //*******************************

 /*! \brief Method setting a vector of doubles containing monitored values (usually temperatures)
  *
  * return:
  *  - 0 on success
  *  - -1 if the number of tempVals elements is out of range
  *
  * \param tempVals vector containing the monitored values
  */

 int MuSR_td_PSI_bin::PutTemperaturesVector(std::vector<double> &tempVals)
 {
   if (static_cast<int>(tempVals.size()) > MAXTEMPER)
     return -1;

   for (unsigned int i=0; i<tempVals.size(); i++)
     fTemper[i] = tempVals[i];

   return 0;
 }

//*******************************
//Implementation GetDevTemperaturesVector()
//*******************************

/*! \brief Method returning a vector of doubles containing standard deviations of the monitored values (usually temperatures)
 */

 std::vector<double> MuSR_td_PSI_bin::GetDevTemperaturesVector()
 {
   std::vector<double> dbl_devTemper;

   for (int i = 0; i < fNumberTemper; i++) {
     dbl_devTemper.push_back(double(fTempDeviation[i]));
   }

   return dbl_devTemper;
 }

 //*******************************
 //Implementation PutDevTemperaturesVector()
 //*******************************

 /*! \brief Method setting a vector of doubles containing standard deviations of the monitored values (usually temperatures)
  *
  * return:
  *  - 0 on success
  *  - -1 if the number of devTempVals elements is out of range
  *
  * \param devTempVals vector containing standard deviations of the monitored values
  */

 int MuSR_td_PSI_bin::PutDevTemperaturesVector(std::vector<double> &devTempVals)
 {
   if (static_cast<int>(devTempVals.size()) > MAXTEMPER)
     return -1;

   for (unsigned int i=0; i<devTempVals.size(); i++)
     fTempDeviation[i] = devTempVals[i];

   return 0;
 }

//*******************************
//Implementation GetTimeStartVector()
//*******************************

/*! \brief Method returning a vector of strings containing 1) the date when the run was 
 *   started and 2) the time when the run was started
 */

 std::vector<std::string> MuSR_td_PSI_bin::GetTimeStartVector()

 {
   std::vector<std::string> timeStart(2);

   timeStart[0] = fDateStart;
   timeStart[1] = fTimeStart;

   return timeStart;
 }

 //*******************************
 //Implementation PutTimeStartVector()
 //*******************************

 /*! \brief Method setting a vector of strings containing 1) the date when the run was
  *   started and 2) the time when the run was started
  *
  * return:
  *  - 0 on success
  *  - -1 timeStart vector has too many elements
  *  - -2 if date is too long
  *  - -3 if time is too long
  *
  * \param timeStart vector where [0] contains the date, [1] the time string
  */

 int MuSR_td_PSI_bin::PutTimeStartVector(std::vector<std::string> timeStart)
 {
   if (timeStart.size() != 2)
     return -1;

   // date
   if (timeStart[0].length() > 9)
     return -2;

   // time
   if (timeStart[1].length() > 8)
     return -3;

   strcpy(fDateStart, timeStart[0].c_str());
   strcpy(fTimeStart, timeStart[1].c_str());

   return 0;
 }

//*******************************
//Implementation GetTimeStopVector()
//*******************************

/*! \brief Method returning a vector of strings containing 1) the date when the run was 
 *   stopped and 2) the time when the run was stopped
 */

 std::vector<std::string> MuSR_td_PSI_bin::GetTimeStopVector()
 {
   std::vector<std::string> timeStop(2);

   timeStop[0] = fDateStop;
   timeStop[1] = fTimeStop;

   return timeStop;
 }

 //*******************************
 //Implementation PutTimeStopVector()
 //*******************************

 /*! \brief Method setting a vector of strings containing 1) the date when the run was
  *   started and 2) the time when the run was stopped
  *
  * return:
  *  - 0 on success
  *  - -1 timeStart vector has too many elements
  *  - -2 if date is too long
  *  - -3 if time is too long
  *
  * \param timeStop vector where [0] contains the date, [1] the time string
  */

 int MuSR_td_PSI_bin::PutTimeStopVector(std::vector<std::string> timeStop)
 {
   if (timeStop.size() != 2)
     return -1;

   // date
   if (timeStop[0].length() > 9)
     return -2;

   // time
   if (timeStop[1].length() > 8)
     return -3;

   strcpy(fDateStop, timeStop[0].c_str());
   strcpy(fTimeStop, timeStop[1].c_str());

   return 0;
 }

//*******************************
//Implementation Clear()
//*******************************

/*! \brief Method to clear member variables before using instance for next read
 */

 int MuSR_td_PSI_bin::Clear()
 {
   int i,j;

   // NIY maybe flag when histo should not be released

   // free private histograms
   fHisto.clear();

   // free public vector
   fHistosVector.clear();

   // init other member variables
   fFilename  = "?";
   fReadingOk = false;
   fWritingOk = false;
   fConsistencyOk = false;
   fReadStatus = "";
   fWriteStatus = "";
   fConsistencyStatus = "";

   strcpy(fFormatId,"??");

   fNumRun = 0;
   //01234567890
   strcpy(fSample,    "          ");
   strcpy(fTemp,      "          ");
   strcpy(fField,     "          ");
   strcpy(fOrient,    "          ");
   strcpy(fSetup,     "          ");
   strcpy(fComment,   "          ");
   strcpy(fDateStart,"         ");
   strcpy(fTimeStart,"        ");
   strcpy(fDateStop, "         ");
   strcpy(fTimeStop, "        ");

   fBinWidth = 0.;
   fNumberHisto = 0;
   fLengthHisto = 0;
   fTotalEvents = 0;
   fDefaultBinning = 1;

   for (i=0; i < MAXHISTO; i++) {
     for (j=0; j < MAXLABELSIZE-1; j++)
       fLabelsHisto[i][j] = ' ';
     fLabelsHisto[i][MAXLABELSIZE-1] = '\0';
     fEventsPerHisto[i] = 0;
     fRealT0[i]    = 0.f;
     fIntegerT0[i] = 0;
     fFirstGood[i] = 0;
     fLastGood[i]  = 0;
   }

   fNumberScaler = 0;
   for (i=0; i < MAXSCALER; i++) {
     for (j=0; j < MAXLABELSIZE-1; j++)
       fLabelsScalers[i][j] = ' ';
     fLabelsScalers[i][MAXLABELSIZE-1] = '\0';

     fScalers[i] = 0;
   }

   fNumberTemper = 0;
   for (i=0; i < MAXTEMPER; i++) {
     fTemper[i] = 0.f;
     fTempDeviation[i] = 0.f;
   }

   return 0;
 }

//*******************************
//Implementation Show()
//*******************************
/*! \brief Method to show current values of member variables
 */

 int MuSR_td_PSI_bin::Show()          const
 {
   std::cout << "Filename is " << fFilename << std::endl;
   if (fReadingOk) {
     int i;

     std::cout << "Format Identifier is " << fFormatId << std::endl;

     std::cout << "Run number is  " << fNumRun << std::endl;
     std::cout << "Sample is      " << fSample  << std::endl;
     std::cout << "Temperature is " << fTemp    << std::endl;
     std::cout << "Field is       " << fField   << std::endl;
     std::cout << "Orientation is " << fOrient  << std::endl;
     std::cout << "Comment is     " << fComment << std::endl;

     std::cout << "Start Date is  " << fDateStart << std::endl;
     std::cout << "Start Time is  " << fTimeStart << std::endl;

     std::cout << "End Date is    " << fDateStop << std::endl;
     std::cout << "End Time is    " << fTimeStop << std::endl;

     std::cout << "Bin width is   " << fBinWidth << " [usec]" << std::endl;
     std::cout << "Number of histograms is " << fNumberHisto << std::endl;
     std::cout << "Histogram length is     " << fLengthHisto << std::endl;
     std::cout << "Default binning is      " << fDefaultBinning << std::endl;
     std::cout << "Total number of events is " << fTotalEvents << std::endl;

     for (i=0; i < fNumberHisto; i++) {
       std::cout << "Histogram " << i << " Name is >" << fLabelsHisto[i]
           << "<  Events per histogram is " << fEventsPerHisto[i] << std::endl;
       std::cout << "      real t0 is              " << fRealT0[i] << std::endl;
       std::cout << "      t0 is                   " << fIntegerT0[i] << std::endl;
       std::cout << "      first good bin is       " << fFirstGood[i] << std::endl;
       std::cout << "      last good bin is        " << fLastGood[i] << std::endl;
     }

     std::cout << "Number of scalers is " << fNumberScaler << std::endl;
     for (i=0; i < fNumberScaler; i++) {
       std::cout << "Scaler " << i << " Name is >" << fLabelsScalers[i]
           << "<   Value is " << fScalers[i] << std::endl;
     }

     std::cout << "Number of temperatures is " << fNumberTemper << std::endl;
     for (i=0; i < fNumberTemper; i++) {
       std::cout << "Temperature " << i << " is " << fTemper[i]
           << "   Deviation is " << fTempDeviation[i] << std::endl;
     }

   } else {
     std::cout << fReadStatus << std::endl;
   }
   return 0;
 }

//*******************************
//Implementation Tmax
//*******************************

 int MuSR_td_PSI_bin::Tmax(int x, int y)
 {
   if (x >= y) {
     return x;
   }
   return y;
 }


//*******************************
//Implementation Tmin
//*******************************

 int MuSR_td_PSI_bin::Tmin(int x, int y)
 {
   if (x >= y) {
     return y;
   }
   return x;
 }

/************************************************************************************
 * EOF MuSR_td_PSI_bin.cpp                                                       *
 ************************************************************************************/

