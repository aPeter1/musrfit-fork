/********************************************************************************************

  MuSR_td_PSI_bin.cpp

  implementation of the class 'MuSR_td_PSI_bin'

  Main class to read mdu and td_bin PSI MuSR data.

*********************************************************************************************

    begin                : Alex Amato, October 2005
    modified             : Andrea Raselli, October 2009
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
using namespace std;
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
  histo = NULL;
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
//Implementation read (generic read)
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

 int MuSR_td_PSI_bin::read(const char * fileName)
 {
   ifstream  file_name;

   Clear();

   filename    = fileName;

   file_name.open(fileName, ios_base::binary);  // open file
   if (file_name.fail())
   {
     readstatus  = "ERROR Open "+filename+" failed!";
     return 1;            // ERROR open failed
   }

   char *buffer_file = new char[3];
   if (!buffer_file)
   {
     readstatus = "ERROR Allocating data buffer";
     return 3;                 // ERROR allocating data buffer
   }

   file_name.read(buffer_file, 2);             // read format identifier of header
   // into buffer
   if (file_name.fail())
   {
     file_name.close();
     delete [] buffer_file;
     readstatus  = "ERROR Reading "+filename+" header failed!";
     return 1;                                  // ERROR reading header failed
   }

   strncpy(format_id,buffer_file,2);
   format_id[2] = '\0';

   file_name.close();
   delete [] buffer_file;

   // file may either be PSI binary format
   if (strncmp(format_id,"1N",2) == 0)
   {
     return readbin(fileName);  // then read it as PSI bin
   }

   // or MDU format (pTA, TDC or 32 channel TDC)
   else if ((strncmp(format_id,"M3",2) == 0) ||(strncmp(format_id,"T4",2) == 0) ||
            (strncmp(format_id,"T5",2) == 0))
   {
     return readmdu(fileName); // else read it as MDU
   }
   else
   {
     readstatus  = "ERROR Unknown file format in "+filename+"!";
     return 2;                                 // ERROR unsupported version
   }

 }

 //*******************************
 //Implementation write (generic write)
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

int MuSR_td_PSI_bin::write(const char *fileName)
{
   string fln = fileName;
   size_t found = fln.find_last_of(".");
   if (found == fln.npos) {
     return 1; // no extension found
   }
   string ext = fln.substr(found+1);
   int status = 0;
   if (ext == "bin")
     status = writebin(fileName);
   else if (ext == "mdu")
     status = writemdu(fileName);
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
#if (defined(_WIN32) || defined(__linux__) || defined(_WIN32GCC))
typedef                 short    Int16;
typedef                 int      Int32;
#else
typedef                 int      Int16;
typedef            long int      Int32;
#endif

#endif

typedef                 float   Float32;

//*******************************
//Implementation readbin
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

int MuSR_td_PSI_bin::readbin(const char * fileName)
{
  ifstream  file_name;
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
    readstatus  = "ERROR Size of Int16 data type is not 2 bytes!";
    return 1;            // ERROR open failed
  }

  if (sizeof(Int32) != 4)
  {
    readstatus  = "ERROR Sizeof Int32 data type is not 4 bytes";
    return 1;            // ERROR open failed
  }

  if (sizeof(Float32) != 4)
  {
    readstatus  = "ERROR Sizeof Float32 data type is not 4 bytes";
    return 1;            // ERROR open failed
  }

  filename    = fileName;

  file_name.open(fileName, ios_base::binary);  // open PSI bin file
  if (file_name.fail())
  {
    readstatus  = "ERROR Open "+filename+" failed!";
    return 1;            // ERROR open failed
  }

  char *buffer_file = new char[1024];
  if (!buffer_file)
  {
    readstatus = "ERROR Allocating buffer to read header failed!";
    return 3;                 // ERROR allocating data buffer
  }

  file_name.read(buffer_file, 1024);          // read header into buffer
  if (file_name.fail())
  {
    file_name.close();
    delete [] buffer_file;
    readstatus  = "ERROR Reading "+filename+" header failed!";
    return 1;                                  // ERROR reading header failed
  }
  // fill header data into member variables
  strncpy(format_id,buffer_file,2);
  format_id[2] = '\0';

  if (strcmp(format_id,"1N") != 0)
  {
    file_name.close();
    delete [] buffer_file;
    readstatus  = "ERROR Unknown file format in "+filename+"!";
    return 2;                                 // ERROR unsupported version
  }

  dum_Int16 = (Int16 *) &buffer_file[2];
  tdc_resolution = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[4];
  tdc_overflow   = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[6];
  num_run        = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[28];
  length_histo   = *dum_Int16;

  dum_Int16 = (Int16 *) &buffer_file[30];
  number_histo   = *dum_Int16;

  strncpy(sample,buffer_file+138,10);
  sample[10] = '\0';

  strncpy(temp,buffer_file+148,10);
  temp[10] = '\0';

  strncpy(field,buffer_file+158,10);
  field[10] = '\0';

  strncpy(orient,buffer_file+168,10);
  orient[10] = '\0';

  strncpy(setup, buffer_file+178, 10);
  setup[10] = '\0';

  strncpy(comment,buffer_file+860,62);
  comment[62] = '\0';

  strncpy(date_start,buffer_file+218,9);
  date_start[9] = '\0';

  strncpy(date_stop,buffer_file+227,9);
  date_stop[9] = '\0';

  strncpy(time_start,buffer_file+236,8);
  time_start[8] = '\0';

  strncpy(time_stop,buffer_file+244,8);
  time_stop[8] = '\0';

  dum_Int32 = (Int32 *) &buffer_file[424];
  total_events   = *dum_Int32;

  for (i=0; i<=15; i++)
  {
    strncpy(labels_histo[i],buffer_file+948+i*4,4);
    labels_histo[i][4] = '\0';

    dum_Int32 = (Int32 *) &buffer_file[296+i*4];
    events_per_histo[i] = *dum_Int32;

    dum_Int16 = (Int16 *) &buffer_file[458+i*2];
    integer_t0[i]       = *dum_Int16;

    dum_Int16 = (Int16 *) &buffer_file[490+i*2];
    first_good[i]       = *dum_Int16;

    dum_Int16 = (Int16 *) &buffer_file[522+i*2];
    last_good[i]        = *dum_Int16;
  }

  for (i=0; i<=15; i++)
  {
    dum_Float32 = (Float32 *) &buffer_file[792+i*4];
    real_t0[i]          = *dum_Float32;
  }

  number_scaler = 18;

  for (i=0; i<=5; i++)
  {
    dum_Int32 = (Int32 *) &buffer_file[670+i*4];
    scalers[i]          = *dum_Int32;

    strncpy(labels_scalers[i],buffer_file+924+i*4,4);
    labels_scalers[i][4] = '\0';
  }

  for (i=6; i<number_scaler; i++)
  {
    dum_Int32 = (Int32 *) &buffer_file[360+(i-6)*4];
    scalers[i]          = *dum_Int32;

    strncpy(labels_scalers[i],buffer_file+554+(i-6)*4,4);
    labels_scalers[i][4] = '\0';
  }

  dum_Float32 = (Float32 *) &buffer_file[1012];
  bin_width             = static_cast<double>(*dum_Float32);

  if (bin_width == 0.)
  {
    bin_width=0.125*(625.E-6)*pow(static_cast<double>(2.0),static_cast<double>(tdc_resolution));
  }

  default_binning = 1;

  number_temper = 4;
  for (i=0; i< number_temper; i++)
  {
    dum_Float32 = (Float32 *) &buffer_file[716+i*4];
    temper[i]           = *dum_Float32;

    dum_Float32 = (Float32 *) &buffer_file[738+i*4];
    temp_deviation[i]   = *dum_Float32;

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
  if (*dum_Int16 != 1)
  {
    cout << "ERROR number of histograms/record not equals 1!"
        << " Required algorithm is not implemented!" << endl;
    delete [] buffer_file;
    file_name.close();
    readstatus  = "ERROR Algorithm to read multiple histograms in one block -"
                  " necessary to read " + filename + " - is not implemented!";
    return 4;                                // ERROR algorithm not implemented
  }

  dum_Int32 = (Int32 *) &buffer_file[654];
  period_save              = *dum_Int32;

  dum_Int32 = (Int32 *) &buffer_file[658];
  period_mon               = *dum_Int32;

  if (buffer_file) delete [] buffer_file;

  if (number_histo <= 0)
  {
    file_name.close();
    readstatus  = "ERROR Less than 1 histogram in "  + filename;
    return 5;                                // ERROR number of histograms < 1
  }

  // allocate histograms
  histo = new int* [int(number_histo)];
  if (!histo)
  {
    Clear();
    file_name.close();
    readstatus = "ERROR Allocating histo[] failed!";
    return 3;                                // ERROR allocating histogram buffer
  }

  for (i=0; i<number_histo; i++)
  {
    histo[i] = new int [length_histo];

    if (!histo[i])
    {
      for (int j=0; j<i; j++)
        delete histo[j];
      delete [] histo;
      histo = NULL;
      Clear();
      file_name.close();
      readstatus = "ERROR Allocating histo[] failed!";
      return 3;                                // ERROR allocating histogram buffer
    }
  }

  char *buffer_file_histo = new char[Int32(num_data_records_file)
                                     *Int32(length_data_records_bins)*4];
  if (!buffer_file_histo)
  {
    Clear();
    file_name.close();
    readstatus = "ERROR Allocating buffer to read histogram failed!";
    return 3;                                // ERROR allocating histogram buffer
  }
  file_name.seekg(1024, ios_base::beg);     // beginning of histogram data

  file_name.read(buffer_file_histo, Int32(num_data_records_file)
                 *Int32(length_data_records_bins)*4);
  if (file_name.fail())
  {
    Clear();
    delete [] buffer_file_histo;
    file_name.close();
    readstatus  = "ERROR Reading data in "+filename+" failed!";
    return 6;                                // ERROR reading data failed
  }
  file_name.close();

  // process histograms
  vector<double> dummy_vector;

  histos_vector.clear();
  for (i=0; i<number_histo; i++)
  {
    dummy_vector.clear();
    for (int j=0; j<length_histo; j++)
    {
      dum_Int32 = (Int32 *) &buffer_file_histo[(i*Int32(num_data_records_histo)*
                                                Int32(length_data_records_bins)+j)*sizeof(Int32)];
      histo[i][j]= *dum_Int32;
      dummy_vector.push_back(double(histo[i][j]));
    }
    histos_vector.push_back(dummy_vector);
  }

  if (buffer_file_histo) delete [] buffer_file_histo;

  readstatus = "SUCCESS";
  readingok = true;

  return 0;
}

//*******************************
//Implementation readbin
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

int MuSR_td_PSI_bin::writebin(const char *fileName)
{
  if (!CheckDataConsistency()) {
    writestatus = "ERROR Given data set is incompatible with the PSI-BIN format!";
    return 4;
  }

  ofstream  fout;

  // prepare buffer
  char *buffer = new char[1024];
  if (!buffer) {
    writestatus = "ERROR Allocating buffer to write header failed!";
    return 3; // ERROR allocating data buffer
  }
  // initialize buffer
  memset(buffer, 0, 1024);

  // fill header info into the buffer

  fout.open(fileName, ios_base::binary);  // open PSI bin file
  if (fout.fail())
  {
    writestatus  = "ERROR Open " + string(fileName) + " failed!";
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
  dum_Int16 = (Int16)num_run;
  memcpy(buffer+6, &dum_Int16, 2);
  // write length of histogram: LENHIS
  dum_Int16 = (Int16)length_histo;
  memcpy(buffer+28, &dum_Int16, 2);
  // write number of histogram (maximum == 16): NUMHIS
  dum_Int16 = (Int16)number_histo;
  memcpy(buffer+30, &dum_Int16, 2);
  // write number of data records: NUMDAF = NUMHIS*KDAFHI
  dum_Int16 = (Int16)(number_histo * (int)((length_histo + MAXREC - 1) / MAXREC));
  memcpy(buffer+128, &dum_Int16, 2);
  // write record size: LENDAF
  dum_Int16 = MAXREC;
  memcpy(buffer+130, (const char*)&dum_Int16, 2);
  // write number of records per histogram: KDAFHI
  dum_Int16 = (Int16)(length_histo + MAXREC - 1) / MAXREC;
  memcpy(buffer+132, &dum_Int16, 2);
  // write number of histograms per record: KHIDAF
  dum_Int16 = 1;
  memcpy(buffer+134, &dum_Int16, 2);
  // write sample info
  strncpy(buffer+138, sample, 10);
  // write temperature info
  strncpy(buffer+148, temp, 10);
  // write field info
  strncpy(buffer+158, field, 10);
  // write orientation info
  strncpy(buffer+168, orient, 10);
  // write setup info
  strncpy(buffer+178, setup, 10);
  // write run start date
  strncpy(buffer+218, date_start, 9);
  // write run stop date
  strncpy(buffer+227, date_stop, 9);
  // write run start time
  strncpy(buffer+236, time_start, 8);
  // write run stop time
  strncpy(buffer+244, time_stop, 8);
  // write number of events of each histogram
  Int32 noEvents = 0;
  Int32 totalEvents = 0;
  for (int i=0; i<number_histo; i++) {
    noEvents = 0;
    for (int j=0; j<length_histo; j++) {
      noEvents += histo[i][j];
    }
    totalEvents += noEvents;
    memcpy(buffer+296+4*i, &noEvents, 4);
  }
  // write scaler contents 7 to 18
  for (int i=0; i<12; i++) {
    dum_Int32 = (Int32)scalers[i+6];
    memcpy(buffer+360+4*i, &dum_Int32, 4);
  }
  // write total number of events
  memcpy(buffer+424, &totalEvents, 4);

  for (int i=0; i<number_histo; i++) {
    // write t0 for each histogram
    dum_Int16 = (Int16)integer_t0[i];
    memcpy(buffer+458+2*i, (const char *)&dum_Int16, 2);
    // write first good bin for each histogram
    dum_Int16 = (Int16)first_good[i];
    memcpy(buffer+490+2*i, (const char *)&dum_Int16, 2);
    // write last good bin for each histogram
    dum_Int16 = (Int16)last_good[i];
    memcpy(buffer+522+2*i, (const char *)&dum_Int16, 2);
  }
  // write labels for scalers 7-18
  for (int i=6; i<number_scaler; i++) {
    strncpy(buffer+554+i*4, labels_scalers[i], 4);
  }
  // write scaler contents 1 to 6
  for (int i=0; i<6; i++) {
    dum_Int32 = (Int32)scalers[i];
    memcpy(buffer+670+i*4, &dum_Int32, 4);
  }
  // write number of measurements used for temperature
  dum_Int32 = (Int32)number_temper;
  memcpy(buffer+712, (const char *)&dum_Int32, 4);
  for (int i=0; i<number_temper; i++) {
    // write temperature means
    dum_Float32 = (Float32)temper[i];
    memcpy(buffer+716+i*4, (const char *)&dum_Float32, 4);
    // write temperature std dev
    dum_Float32 = (Float32)temp_deviation[i];
    memcpy(buffer+738+i*4, (const char *)&dum_Float32, 4);
  }
  // write run comment
  strncpy(buffer+860, comment, 62);
  // write labels of scalers 1-6
  for (int i=0; i<6; i++) {
    strncpy(buffer+924+i*4, labels_scalers[i], 4);
  }
  // write labels of histograms
  for (int i=0; i<number_histo; i++) {
    strncpy(buffer+948+i*4, labels_histo[i], 4);
  }
  // write TDS time resolution
  dum_Float32 = static_cast<Float32>(bin_width);
  memcpy(buffer+1012, (const char*)&dum_Float32, 4);

  // write header information
  fout.write(buffer, 1024);

  // handle histograms -----------------------------------------------------------------

  // prepare write buffer
  if (buffer) { // get rid of the header buffer
    delete [] buffer;
    buffer = 0;
  }
  buffer = new char[4*MAXREC];
  if (!buffer) {
    writestatus = "ERROR Allocating buffer to write data failed!";
    return 3; // ERROR allocating data buffer
  }
  // initialize buffer
  memset(buffer, 0, 4*MAXREC);

  bool buffer_empty = false;
  for (int i=0; i<number_histo; i++) {
    for (int j=0; j<length_histo; j++) {
      dum_Int32 = (Int32)histo[i][j];
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
    buffer = 0;
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
  Int32  HistoResolution;          // TDC resolution factor for target format
                                   // or pTA timespan
  Int32  BinOffset;
  Int32  BinsPerHistogram;
  Int32  NumberOfDetectors;
  char DetectorNumberList[DETECTLISTSTR]; // list of detectors to be converted
                                          // to the target data format
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
  Int32   tfb;   /* NOTE: t0b, tfb, tlb are in bin units of the target format!! */
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
//Implementation readmdu
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

int MuSR_td_PSI_bin::readmdu(const char * fileName)
{
  ifstream  file_name;
  int       i, j;

  Clear();

  if (sizeof(Int32) != 4)
  {
    readstatus  = "ERROR Sizeof( Int32 ) data type is not 4 bytes";
    return 1;            // ERROR open failed
  }

  filename    = fileName;

  file_name.open(fileName, ios_base::binary);  // open PSI bin file
  if (file_name.fail())
  {
    readstatus  = "ERROR Open "+filename+" failed!";
    return 1;            // ERROR open failed
  }

  pTAFileHeaderRec      gpTAfhead;
  //FeFileHeaderPtr     gpFehead  = &gpTAfhead.Header;

  file_name.read((char *)&gpTAfhead, sizeof gpTAfhead);    // read header into buffer
  if (file_name.fail())
  {
    file_name.close();
    readstatus  = "ERROR Reading "+filename+" header failed!";
    return 1;                                  // ERROR reading header failed
  }
  // fill header data into member variables
  format_id[0] = gpTAfhead.Header.FmtId;
  format_id[1] = gpTAfhead.Header.FmtVersion;
  format_id[2] = '\0';

  if ((strcmp(format_id,"M3") != 0) && (strcmp(format_id,"T4") != 0) &&
      (strcmp(format_id,"T5") != 0))
  {
    file_name.close();
    readstatus  = "ERROR Unknown file format in "+filename+"!";
    return 2;                                 // ERROR unsupported version
  }

  if (sizeof(pTAFileHeaderRec) != gpTAfhead.NumBytesHeader)
  {
    file_name.close();
    readstatus  = "ERROR Reading "+filename+" incorrect pTAFileHeaderRec size";
    return 1;                                  // ERROR reading header failed
  }

  // header size OK read header information
  strncpy(sample,&gpTAfhead.Header.RunTitle[0],10);
  sample[10] = '\0';

  strncpy(temp,  &gpTAfhead.Header.RunTitle[10],10);
  temp[10] = '\0';

  strncpy(field, &gpTAfhead.Header.RunTitle[20],10);
  field[10] = '\0';

  strncpy(orient,&gpTAfhead.Header.RunTitle[30],10);
  orient[10] = '\0';

  strncpy(comment,&gpTAfhead.Header.RunSubTitle[0],62);
  comment[62] = '\0';

  strncpy(&date_start[0],&gpTAfhead.Header.StartDate[0],7);
  strncpy(&date_start[7],&gpTAfhead.Header.StartDate[9],2);
  date_start[9] = '\0';

  strncpy(&date_stop[0],&gpTAfhead.Header.EndDate[0],7);
  strncpy(&date_stop[7],&gpTAfhead.Header.EndDate[9],2);
  date_stop[9] = '\0';

  strncpy(time_start,&gpTAfhead.Header.StartTime[0],8);
  time_start[8] = '\0';

  strncpy(time_stop,&gpTAfhead.Header.EndTime[0],8);
  time_stop[8] = '\0';

  num_run = gpTAfhead.Header.RunNumber;

  if (sizeof(pTATagRec) != gpTAfhead.NumBytesTag)
  {
    file_name.close();
    readstatus  = "ERROR Reading "+filename+" incorrect pTATagRec size";
    return 1;                                  // ERROR reading header failed
  }

#ifdef MIDEBUG1
  cout << "Header.MeanTemp = " << gpTAfhead.Header.MeanTemp << endl;
  cout << "Header.TempDev  = " << gpTAfhead.Header.TempDev << endl;
#endif

  // read temperature deviation from header string (td0 [td1 [td2 [td3]]])
  number_temper = sscanf(gpTAfhead.Header.TempDev,"%f %f %f %f",
                         &temp_deviation[0], &temp_deviation[1], &temp_deviation[2],
                         &temp_deviation[3]);

  // fill unused
  for (i=number_temper; i<MAXTEMPER; i++)
    temp_deviation[i] = 0.f;

  // read temperature from header string (t0 [t1 [t2 [t3]]])
  number_temper = sscanf(gpTAfhead.Header.MeanTemp,"%f %f %f %f",
                         &temper[0], &temper[1], &temper[2], &temper[3]);

  // fill unused
  for (i=number_temper; i<MAXTEMPER; i++)
  {
    temper[i] = 0.f;
    temp_deviation[i] = 0.f;
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
          cout << "error " << it << " is out of range |0 - " << MAXHISTO-1 << "|"
              <<endl;
        }
      } else {
        cout << "error reading " << &gpTAfhead.Header.DetectorNumberList[j] << endl;
      }
    }
  }

  int tothist = 0;
  int resolutionfactor = 1;

  // ---- process version specific settings and total statistics
  if        (strcmp(format_id,"M3") == 0)
  {

    if (sizeof(pTASettingsRec) != gpTAfhead.NumBytesSettings)
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" incorrect pTASettingsRec size";
      return 1;                                  // ERROR reading header failed
    }

    if (sizeof(pTAStatisticRec) != gpTAfhead.NumBytesStatistics)
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" incorrect pTAStatisticRec size";
      return 1;                                  // ERROR reading header failed
    }

    pTASettingsRec   gpTAsetpta;
    pTAStatisticRec  gpTAstattotpta;

    tothist = PTAMAXTAGS;

    file_name.read((char *)&gpTAsetpta, sizeof gpTAsetpta);//read settings into buffer
    if (file_name.fail())
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" settings failed!";
      return 1;                                  // ERROR reading settings failed
    }

    // read stat into buffer
    file_name.read((char *)&gpTAstattotpta, sizeof gpTAstattotpta);
    if (file_name.fail())
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" statistics failed!";
      return 1;                                  // ERROR reading statistics failed
    }

    number_scaler = PTAMAXTAGS;
    for (i=0; i < number_scaler; i++)
    {
      strncpy(labels_scalers[i],gpTAsetpta.tag[i].Label,MAXLABELSIZE);
      labels_scalers[i][MAXLABELSIZE-1] = '\0';

      scalers[i] = gpTAstattotpta.TagScaler[i];
    }

    int timespan;

    resolutionfactor = 1;
    timespan = gpTAfhead.Header.HistoResolution; // target timespan (PSIBIN)
    // t0, fg, lg
    if      (gpTAsetpta.timespan == 11) // pta timespan
      bin_width = 0.000625;
    else if (gpTAsetpta.timespan == 10)
      bin_width = 0.0003125;
    else if (gpTAsetpta.timespan ==  9)
      bin_width = 0.00015625;
    else if (gpTAsetpta.timespan ==  8)
      bin_width = 0.000078125;
    else if (gpTAsetpta.timespan ==  7)
      bin_width = 0.0000390625;
    else if (gpTAsetpta.timespan ==  6)
      bin_width = 0.00001953125;
    else
    {
      file_name.close();
      readstatus  = "ERROR "+filename+" settings resolution code failed!";
      return 1;                                   // ERROR reading settings failed
    }

    if (timespan+8-gpTAsetpta.timespan < 0)
    {
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

    length_histo   = 0;
    number_histo   = 0;
    for (i=0; i<tothist; i++)
    {
      /* read histogram data */
      if (gpTAsetpta.tag[i].Type == PTATAGC_POSITRON)
      {
        int nbins;

#ifdef MIDEBUG1
        cout << "Tag[" << i << "] Histomin = " << gpTAsetpta.tag[i].Histominb
            << " Histomax = " << gpTAsetpta.tag[i].Histomaxb << endl;
#endif
        // is a histogram there
        if ((nbins=(gpTAsetpta.tag[i].Histomaxb-gpTAsetpta.tag[i].Histominb + 1))>1)
        {

          // for pTA only: read histogram only if histogram was selected
          if (selected[i])
          {
            // first histo -> take histogram length
            if (number_histo == 0)
              length_histo = nbins+gpTAsetpta.tag[i].Histominb;
            // different histogram length?
            else if (length_histo != nbins+gpTAsetpta.tag[i].Histominb)
            {
              cout << "Different histogram lengths!" << endl;
            }
            number_histo++;
          }
        }
      }
    }

    // check gpTAfhead.Header.NumberOfDetectors
    if (number_histo != gpTAfhead.Header.NumberOfDetectors)
      cout << "Number of found histos " << number_histo << " and number in header "
          << gpTAfhead.Header.NumberOfDetectors << " differ!" << endl;

    // special case: subtract 1 from stored histogram to get desired histogram length
    if (length_histo > 0) length_histo -= 1;

  }
  else if (strcmp(format_id,"T4") == 0)
  {

    if (sizeof(pTATDCSettingsRec) != gpTAfhead.NumBytesSettings)
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" incorrect pTATDCSettingsRec size";
      return 1;                                  // ERROR reading header failed
    }

    if (sizeof(pTATDCStatisticRec) != gpTAfhead.NumBytesStatistics)
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" incorrect pTATDCStatisticRec size";
      return 1;                                  // ERROR reading header failed
    }

    pTATDCSettingsRec   gpTAsettdc;
    pTATDCStatisticRec  gpTAstattottdc;

    tothist = TDCMAXTAGS16;

    file_name.read((char *)&gpTAsettdc, sizeof gpTAsettdc);//read settings into buffer
    if (file_name.fail())
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" settings failed!";
      return 1;                                  // ERROR reading settings failed
    }

    file_name.read((char *)&gpTAstattottdc, sizeof gpTAstattottdc); // read stat into buffer
    if (file_name.fail())
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" statistics failed!";
      return 1;                                  // ERROR reading statistics failed
    }

    number_scaler = TDCMAXTAGS16;
    for (i=0; i < number_scaler; i++)
    {
      strncpy(labels_scalers[i],gpTAsettdc.tag[i].Label,MAXLABELSIZE);
      labels_scalers[i][MAXLABELSIZE-1] = '\0';

      scalers[i] = gpTAstattottdc.TagScaler[i];
    }

    resolutionfactor = gpTAfhead.Header.HistoResolution;
    if      (gpTAsettdc.resolutioncode ==  25)
      bin_width = 0.0000244140625;
    else if (gpTAsettdc.resolutioncode == 100)
      bin_width = 0.00009765625;
    else if (gpTAsettdc.resolutioncode == 200)
      bin_width = 0.0001953125;
    else if (gpTAsettdc.resolutioncode == 800)
      bin_width = 0.0007812500;
    else
    {
      file_name.close();
      readstatus  = "ERROR "+filename+" settings resolution code failed!";
      return 1;                                  // ERROR reading settings failed
    }

    length_histo   = 0;
    number_histo   = 0;
    for (i=0; i<tothist; i++)
    {
      /* read histogram data */
      if (gpTAsettdc.tag[i].Type == PTATAGC_POSITRON)
      {
        int nbins;

#ifdef MIDEBUG1
        cout << "Tag[" << i << "] Histomin = " << gpTAsettdc.tag[i].Histominb
            << " Histomax = " << gpTAsettdc.tag[i].Histomaxb << endl;
#endif
        // is a histogram there
        if ((nbins=(gpTAsettdc.tag[i].Histomaxb-gpTAsettdc.tag[i].Histominb + 1))>1)
        {
          // first histo -> take histogram length
          if (number_histo == 0)
            length_histo = nbins+gpTAsettdc.tag[i].Histominb;
          // different histogram length?
          else if (length_histo != nbins+gpTAsettdc.tag[i].Histominb)
          {
            cout << "Different histogram lengths!" << endl;
          }
          number_histo++;
        }
      }
    }

  }
  else if (strcmp(format_id,"T5") == 0)
  {

    if (sizeof(pTATDC32SettingsRec) != gpTAfhead.NumBytesSettings)
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" incorrect pTATDC32SettingsRec size";
      return 1;                                  // ERROR reading header failed
    }

    if (sizeof(pTATDC32StatisticRec) != gpTAfhead.NumBytesStatistics)
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" incorrect pTATDC32StatisticRec size";
      return 1;                                  // ERROR reading header failed
    }

    pTATDC32SettingsRec   gpTAsettdc32;
    pTATDC32StatisticRec  gpTAstattottdc32;

    tothist = TDCMAXTAGS32;

    // read settings into buffer
    file_name.read((char *)&gpTAsettdc32, sizeof gpTAsettdc32);
    if (file_name.fail())
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" settings failed!";
      return 1;                                  // ERROR reading settings failed
    }

    // read stat into buffer
    file_name.read((char *)&gpTAstattottdc32, sizeof gpTAstattottdc32);
    if (file_name.fail())
    {
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" statistics failed!";
      return 1;                                  // ERROR reading statistics failed
    }

    number_scaler = TDCMAXTAGS32;
    for (i=0; i < number_scaler; i++)
    {
      strncpy(labels_scalers[i],gpTAsettdc32.tag[i].Label,MAXLABELSIZE);
      labels_scalers[i][MAXLABELSIZE-1] = '\0';

      scalers[i] = gpTAstattottdc32.TagScaler[i];
    }

    resolutionfactor = gpTAfhead.Header.HistoResolution;
    if      (gpTAsettdc32.resolutioncode ==  25)
      bin_width = 0.0000244140625;
    else if (gpTAsettdc32.resolutioncode == 100)
      bin_width = 0.00009765625;
    else if (gpTAsettdc32.resolutioncode == 200)
      bin_width = 0.0001953125;
    else if (gpTAsettdc32.resolutioncode == 800)
      bin_width = 0.0007812500;
    else
    {
      file_name.close();
      readstatus  = "ERROR "+filename+" settings resolution code failed!";
      return 1;                                  // ERROR reading settings failed
    }

    length_histo   = 0;
    number_histo   = 0;
    for (i=0; i<tothist; i++)
    {
      /* read histogram data */
      if (gpTAsettdc32.tag[i].Type == PTATAGC_POSITRON)
      {
        int nbins;

#ifdef MIDEBUG1
        cout << "Tag[" << i << "] Histomin = " << gpTAsettdc32.tag[i].Histominb
            << " Histomax = " << gpTAsettdc32.tag[i].Histomaxb << endl;
#endif
        // is a histogram there?
        if ((nbins=(gpTAsettdc32.tag[i].Histomaxb-gpTAsettdc32.tag[i].Histominb + 1))
          >1)
          {

          // first histo -> take histogram length
          if (number_histo == 0)
            length_histo = nbins+gpTAsettdc32.tag[i].Histominb;
          // different histogram length?
          else if (length_histo != nbins+gpTAsettdc32.tag[i].Histominb)
          {
            cout << "Different histogram lengths!" << endl;
          }
          number_histo++;
        }
      }
    }

  }
  else
  {
    tothist = 0;
  }

  // no histograms to process?
  if (tothist <= 0)
  {
    Clear();
    file_name.close();
    readstatus  = "ERROR Less than 1 histogram in "  + filename;
    return 5;                                // ERROR number of histograms < 1
  }

  if (tothist > MAXHISTO)
  {
    cout << "ERROR number of histograms " << tothist << " exceedes maximum "
        << MAXHISTO << "! - Setting maximum number " << endl;
    tothist = MAXHISTO;
  }

#ifdef MIDEBUG1
  cout << "Number of histograms is " << number_histo << endl;
  cout << "Histogram length is "     << length_histo << endl;
  cout << "Resolutionfactor for t0, fg, lg is " << resolutionfactor << endl;
#endif

  default_binning = resolutionfactor;

  // allocate histograms
  histo = new int* [int(number_histo)];
  if (!histo)
  {
    Clear();
    file_name.close();
    readstatus = "ERROR Allocating histo failed!";
    return 3;                                // ERROR allocating histogram buffer
  }

  for (i=0; i<number_histo; i++)
  {
    histo[i] = new int [length_histo];

    if (!histo[i])
    {
      for (j=0; j<i; j++)
        delete histo[j];
      delete [] histo;
      histo = NULL;
      Clear();
      file_name.close();
      readstatus = "ERROR Allocating histo[] failed!";
      return 3;                                // ERROR allocating histogram buffer
    }
  }

  pTATagRec tag;

  total_events   = 0;

  for (i=0; i<number_histo; i++)
    events_per_histo[i] = 0;

  int ihist = 0;
  Int32 *thist = NULL;
  vector<double> dummy_vector;

  histos_vector.clear();
  for (i=0,ihist=0; i< tothist; i++)
  {
    file_name.read((char *)&tag, sizeof tag); // read tag into buffer
    if (file_name.fail())
    {
      dummy_vector.clear();
      Clear();
      if (thist != NULL) delete [] thist; thist = NULL;
      file_name.close();
      readstatus  = "ERROR Reading "+filename+" tag failed!";
      return 6;                                  // ERROR reading tag failed
    }
    /* read histogram data */
    if (tag.Type == PTATAGC_POSITRON)
    {
      int nbins;

#ifdef MIDEBUG1
      cout << "Tag[" << i << "] " << tag.Label << " : Histomin = " << tag.Histominb
          << " Histomax = " << tag.Histomaxb << endl;
#endif
      // is a histogram there?
      if ((nbins=(tag.Histomaxb-tag.Histominb + 1))>1)
      {
        if (thist == NULL) thist = new Int32[nbins];
        if (thist == NULL)
        {
          Clear();
          file_name.close();
          readstatus = "ERROR Allocating histogram buffer failed!";
          return 3;                                // ERROR allocating histogram buffer
        }

        file_name.read((char *)thist, sizeof(Int32)*nbins);// read hist into buffer
        if (file_name.fail())
        {
          Clear();
          if (thist != NULL) delete [] thist; thist = NULL;
          file_name.close();
          readstatus  = "ERROR Reading "+filename+" hist failed!";
          return 6;                                  // ERROR reading hist failed
        }

        // for pTA only: use histogram only, if histogram was selected
        // else take all histos but mark not selected
        if (selected[i] || (strcmp(format_id,"M3") != 0))
        {

          if (ihist < MAXHISTO) // max number of histos not yet reached?
          {
            dummy_vector.clear();

            strncpy(labels_histo[ihist],tag.Label,MAXLABELSIZE);
            labels_histo[ihist][MAXLABELSIZE-1] = '\0';

            // mark with ** when not selected
            if (!selected[i] && (strlen(labels_histo[ihist])<MAXLABELSIZE-2))
              strcat(labels_histo[ihist],"**");

            // calculate t0, fg, lg for "raw" TDC /pTA actually specified for binned
            // histograms
            // taking largest possible bin value for t0 and fg
            integer_t0[ihist] = (tag.t0b+1)*resolutionfactor -1;
            first_good[ihist] = (tag.tfb+1)*resolutionfactor -1;
            last_good[ihist]  =  tag.tlb*resolutionfactor;

            // store histogram
            // in case of non zero offset init
            for (j=0; j<tag.Histominb; j++)
            {
              histo[ihist][j]= 0;
              dummy_vector.push_back(double(histo[ihist][j]));
            }
            // fill histogram
            for (j=tag.Histominb; j<length_histo; j++)
            {
              histo[ihist][j]= *(thist+j-tag.Histominb);
              dummy_vector.push_back(double(histo[ihist][j]));

              // do summation of events between fg and lg
              if ((j >= first_good[ihist]) && (j <= last_good[ihist]))
                events_per_histo[ihist] += *(thist+j-tag.Histominb);
            }
            histos_vector.push_back(dummy_vector);

            // only add selected histo(s) to total events
            if (selected[i])
              total_events += events_per_histo[ihist];
          }
          ihist++;
        }
      }
    }
  }

  if (thist != NULL) delete [] thist; thist = NULL;

  file_name.close();

  readstatus = "SUCCESS";
  readingok = true;

  return 0;
}

//*******************************
//Implementation writemdu
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

int MuSR_td_PSI_bin::writemdu(const char * fileName)
{
  cerr << endl << "MuSR_td_PSI_bin::writemdu - not yet implemented" << endl;
  return 0;
}


//*******************************
//Implementation readingOK
//*******************************

/*! \brief Method to obtain if reading and processing of the data file was OK.
 *
 *  This method gives back:
 *    - true if reading was OK
 *    - false if reading was NOT OK
 */
bool MuSR_td_PSI_bin::readingOK() const
{
   return readingok;
}

//*******************************
//Implementation writingOK
//*******************************

/*! \brief Method to obtain if writing and processing of the data file was OK.
 *
 *  This method gives back:
 *    - true if writing was OK
 *    - false if writing was NOT OK
 */
bool MuSR_td_PSI_bin::writingOK() const
{
   return writingok;
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
 * \param tag tag provided to tell how strict the tests should be. 0=reasonable, 1=strict
 */

bool MuSR_td_PSI_bin::CheckDataConsistency(int tag)
{
  if (number_histo <= 0) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** number histograms is zero or less!";
    return consistencyOk;
  }

  if (number_histo > MAXHISTO) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** number of histograms requested: ";
    consistencyStatus += number_histo;
    consistencyStatus += ", which is larger than the possible maximum of 32.";
    return consistencyOk;
  }

  if (length_histo <= 0) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** histogram length is zero or less!";
    return consistencyOk;
  }

  if (length_histo > 32767) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** histogram length is too large (maximum being 32767)!";
    return consistencyOk;
  }

  if ((length_histo % 256 != 0) && (tag == 1)) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** histogram length is not a multiple of 256!";
    return consistencyOk;
  }

  if (number_histo * length_histo > 65536) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** number_histo * length_histo > 65536!";
    return consistencyOk;
  }

  if (histo == 0) {
    consistencyOk = false;
    consistencyStatus  = "**ERROR** no histograms present!";
    return consistencyOk;
  }

  consistencyOk = true;
  consistencyStatus = "SUCCESS";

  return consistencyOk;
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
string MuSR_td_PSI_bin::ReadStatus() const
{
   return readstatus;
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
string MuSR_td_PSI_bin::WriteStatus() const
{
   return writestatus;
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
string MuSR_td_PSI_bin::ConsistencyStatus() const
{
  return consistencyStatus;
}

//*******************************
//Implementation Filename
//*******************************

/*! \brief Method to obtain the file name.
 *
 *  This method gives back:
 *    - <filename>
 */
string MuSR_td_PSI_bin::Filename() const
{
   return filename;
}


//*******************************
//Implementation get_histo_int
//*******************************

/*! \brief Method to return the value of a single bin as integer.
 *
 *  This method gives back:
 *    - bin value as int
 *    - 0 if an invalid histogram number or bin is choosen
 */
int MuSR_td_PSI_bin::get_histo_int(int histo_num, int j)
{
    if (!readingok) return 0;

    if (( histo_num < 0) || (histo_num >= int(number_histo)) ||
       (j < 0 ) || (j >= length_histo))
      return 0;
#ifdef MIDEBUG
    cout << "histos_vector[0][0] = " << histos_vector[0][0] << endl;
#endif
    return histo[histo_num][j];
}

//*******************************
//Implementation get_histo
//*******************************

/*! \brief Method to return the value of a single bin as double.
 *
 *  This method gives back:
 *    - bin value as double
 *    - 0 if an invalid histogram number or bin is choosen
 */
double MuSR_td_PSI_bin::get_histo(int histo_num, int j)
{
    if (!readingok) return 0.;

    if (( histo_num < 0) || (histo_num >= int(number_histo)) ||
       (j < 0 ) || (j >= length_histo))
      return 0.;
#ifdef MIDEBUG
    cout << "histos_vector[0][0] = " << histos_vector[0][0] << endl;
#endif
    return (double)histo[histo_num][j];
}

//*******************************
//Implementation get_histo_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
    histogram \<histo_num\> with binning \<binning\>
 *
 *  This method gives back:
 *    - an pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen or
 *      allocation failed
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
    representing the desired histogram number and binning.
 */

double * MuSR_td_PSI_bin::get_histo_array(int histo_num, int binning)
{
  if (!readingok) return NULL;

  if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
    return NULL;

  double *histo_array = new double[int(int(length_histo)/binning)];

  if (!histo_array) return NULL;

  for (int i = 0; i < int(int(length_histo)/binning); i++)
  {
    histo_array[i] = 0;
    for (int j = 0; j < binning; j++)
      histo_array[i] += double(histo[histo_num][i*binning+j]);
  }

  return histo_array;
}

 //*******************************
 //Implementation put_histo_array_int
 //*******************************

 /*! \brief Method to set the histograms which is a vector of vector of int's (histogram).
  * There are two different ways to get the data conform to PSI-BIN limitations:
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
  *  \param tag 0: rebin and zero pad at need, 1: truncate at need
  */

 int MuSR_td_PSI_bin::put_histo_array_int(vector< vector<int> > histoData, int tag)
 {
   // check that the number of histograms are within allowed boundaries
   if ((histoData.size() == 0) || (histoData.size() > 32)) {
     consistencyOk = false;
     consistencyStatus = "**ERROR** number of histograms out of range! Must be > 0 and < 32.";
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
     consistencyOk = false;
     consistencyStatus = "**ERROR** not all histograms have the same length!";
     return -2;
   }

   // overwrite number_histo
   number_histo = (int)histoData.size();

   // calculate the allowed histo length
   int lengthHisto = (65536 / histoData.size()) - ((65536 / histoData.size()) % 256);
   if (lengthHisto > 32512)
     lengthHisto = 32512;

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
   } else if (tag == 1) {
     if (((int)histoData[0].size() % 256) != 0)
       dataLength = (((int)histoData[0].size()/256)+1)*256;
   }

   // overwrite length_histo
   length_histo = dataLength;

   // clean up if needed
   if (histo) {
     for (int i=0; i<number_histo; i++)
       if (*(histo+i) != NULL) {
       delete[] *(histo+i);
       *(histo+i) = 0;
     }
     delete [] histo;
     histo = 0;
   }

   // allocate the necessary memory
   histo = new int* [histoData.size()];
   if (histo == 0) {
     consistencyOk = false;
     consistencyStatus = "**ERROR** failed to allocate memory for the histos!";
     return -4;
   }

   for (unsigned int i=0; i<histoData.size(); i++) {
     histo[i] = new int[lengthHisto];
     if (histo[i] == 0) {
       // clean up
       for (unsigned int j=0; j<i; j++)
         delete [] histo[j];
       delete [] histo;
       histo = 0;

       consistencyOk = false;
       consistencyStatus = "**ERROR** failed to allocate memory for the histos!";
       return -4;
     }
   }

   // check how the data shall be treated
   if (tag == 0) { // rebin and zero pad at need (strict)
     // rebin data such that it is compatible with PSI-BIN
     vector< vector<int> > data;
     data.resize(number_histo);

     int val = 0;
     for (int i=0; i<number_histo; i++) {
       for (unsigned int j=0; j<histoData[i].size(); j++) {
         if ((j>0) && (j%rebin == 0)) {
           data[i].push_back(val);
           val = 0;
         }
         val += histoData[i][j];
       }
     }

     // fill the histograms
     for (int i=0; i<number_histo; i++) {
       for (int j=0; j<length_histo; j++) {
         if (j<(int)data[i].size())
           histo[i][j] = data[i][j];
         else
           histo[i][j] = 0;
       }
     }

     // rescale T0, fgb, lgb, and time resolution
     for (int i=0; i<number_histo; i++) {
       real_t0[i] = bin_width*integer_t0[i];
       integer_t0[i] /= rebin;
       first_good[i] /= rebin;
       last_good[i]  /= rebin;
     }
     bin_width *= rebin;
   } else if (tag == 1) { // truncate at need (strict)
     // feed the histograms
     for (int i=0; i<number_histo; i++) {
       for (int j=0; j<length_histo; j++) {
         histo[i][j] = histoData[i][j];
       }
     }

     // check if T0, fgb, and lgb are still within proper boundaries
     for (int i=0; i<number_histo; i++) {
       if (integer_t0[i] > length_histo)
         integer_t0[i] = 0;
       real_t0[i] = bin_width*integer_t0[i];
       if (first_good[i] > length_histo)
         first_good[i] = 0;
       if (last_good[i] > length_histo)
         last_good[i] = length_histo-1;
     }
   } else {
     consistencyOk = false;
     consistencyStatus = "**ERROR** found unsupported tag!";
     return -3;
   }

   return 0;
 }

//*******************************
//Implementation get_histo_vector
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

 vector<double> MuSR_td_PSI_bin::get_histo_vector(int histo_num, int binning)
 {
   vector<double> histo_vector; //(int(length_histo/binning))

   if (!readingok) return histo_vector;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return histo_vector;

   for (int i = 0; i < int(length_histo/binning); i++)
     histo_vector.push_back(0.);

   for (int i = 0; i < int(length_histo/binning); i++)
   {
     for (int j = 0; j < binning; j++)
       histo_vector[i] += double(histo[histo_num][i*binning+j]);
   }

   return histo_vector;
 }

//*******************************
//Implementation get_histo_vector_no0
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

 vector<double> MuSR_td_PSI_bin::get_histo_vector_no0(int histo_num, int binning)
 {
   vector<double> histo_vector; //(int(length_histo/binning));

   if (!readingok) return histo_vector;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return histo_vector;

   for (int i = 0; i < int(length_histo/binning); i++)
     histo_vector.push_back(0.);

   for (int i = 0; i < int(length_histo/binning); i++)
   {
     for (int j = 0; j < binning; j++)
       histo_vector[i] += double(histo[histo_num][i*binning+j]);

     if (histo_vector[i] < 0.5 )
     {
       histo_vector[i] = 0.1;
     }
   }

   return histo_vector;
 }


//**********************************
//Implementation get_histo_array_int
//**********************************

/*! \brief Method to obtain an array of type integer containing the values of the 
 *   histogram \<histo_num\>
 *
 *  This method gives back:
 *    - an pointer of an integer array
 *    - the NULL pointer if an invalid histogram number is choosen or allocate failed
 *
 *  The parameter of the method is the integer \<histo_num\> representing the desired 
 *  histogram number.
 */

 int * MuSR_td_PSI_bin::get_histo_array_int(int histo_num)
 {
   if (!readingok) return NULL;

   if ( histo_num < 0 || histo_num >= int(number_histo))
     return NULL;

   int *histo_array = new int[length_histo];

   if (!histo_array) return NULL;

   for (int i = 0; i < int(length_histo); i++)
   {
     histo_array[i] = int(histo[histo_num][i]);
   }

   return histo_array;
 }


//*******************************
//Implementation get_histo_fromt0_array
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

 double * MuSR_td_PSI_bin::get_histo_fromt0_array(int histo_num, int binning, int offset)
 {
   if (!readingok) return NULL;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return NULL;

   double *histo_fromt0_array =
       new double[int((int(length_histo)-get_t0_int(histo_num)-offset+1)/binning)];

   if (!histo_fromt0_array) return NULL;

   for (int i = 0; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning); i++)
   {
     histo_fromt0_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_fromt0_array[i] +=
           double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]);
   }

   return histo_fromt0_array;
 }


//*******************************
//Implementation get_histo_fromt0_vector
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

 vector<double> MuSR_td_PSI_bin::get_histo_fromt0_vector(int histo_num, int binning, int offset)
 {
   vector<double> histo_fromt0_vector; // (int((int(length_histo)-get_t0_int(histo_num)+1)/binning));

   if (!readingok) return histo_fromt0_vector;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return histo_fromt0_vector;

   for (int i = 0; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning); i++)
     histo_fromt0_vector.push_back(0.);

   for (int i = 0; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning); i++)
   {
     for (int j = 0; j < binning; j++)
       histo_fromt0_vector[i] +=
           double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]);
   }

   return histo_fromt0_vector;
 }


//*******************************
//Implementation get_histo_goodBins_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point first_good until 
 *   last_good
 *
 *  This method gives back:
 *    - an pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen or
 *      allocate failed
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 double * MuSR_td_PSI_bin::get_histo_goodBins_array(int histo_num, int binning)
 {
   if (!readingok) return NULL;
   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return NULL;

   double *histo_goodBins_array =
       new double[int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num)+1)/binning)];

   if (!histo_goodBins_array) return NULL;

   for (int i = 0; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning); i++)
   {
     histo_goodBins_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_goodBins_array[i] +=
           double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]);
   }

   return histo_goodBins_array;
 }


//*******************************
//Implementation get_histo_goodBins_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point first_good until 
 *   last_good
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> 
 *  representing the desired histogram number and binning.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_goodBins_vector(int histo_num, int binning)
 {
   vector<double> histo_goodBins_vector;

   if (!readingok) return histo_goodBins_vector;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return histo_goodBins_vector;

   for (int i = 0; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning); i++)
     histo_goodBins_vector.push_back(0.);

   for (int i = 0; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning); i++)
   {
     for (int j = 0; j < binning; j++)
       histo_goodBins_vector[i] +=
           double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]);
   }

   return histo_goodBins_vector;
 }


//*******************************
//Implementation get_histo_fromt0_minus_bckgrd_array
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

 double * MuSR_td_PSI_bin::get_histo_fromt0_minus_bckgrd_array(int histo_num,
                                                               int lower_bckgrd, int higher_bckgrd, int binning, int offset)
 {
   if (!readingok) return NULL;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return NULL;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
     return NULL;

   double bckgrd = 0;

   for (int k = lower_bckgrd; k <= higher_bckgrd; k++)
   {
     bckgrd += double(histo[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   double *histo_fromt0_minus_bckgrd_array =
       new double[int((int(length_histo)-get_t0_int(histo_num)-offset+1)/binning)];

   if (!histo_fromt0_minus_bckgrd_array) return NULL;

   for (int i = 0; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning); i++)
   {
     histo_fromt0_minus_bckgrd_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_fromt0_minus_bckgrd_array[i] +=
           double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]) - bckgrd;
   }

   return histo_fromt0_minus_bckgrd_array;
 }

//*******************************
//Implementation get_histo_fromt0_minus_bckgrd_vector
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

 vector<double> MuSR_td_PSI_bin::get_histo_fromt0_minus_bckgrd_vector(int histo_num, int lower_bckgrd,
                                                                      int higher_bckgrd, int binning, int offset)
 {
   vector<double> histo_fromt0_minus_bckgrd_vector; // (int((int(length_histo)-get_t0_int(histo_num)+1)/binning));

   if (!readingok) return histo_fromt0_minus_bckgrd_vector;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return histo_fromt0_minus_bckgrd_vector;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
     return histo_fromt0_minus_bckgrd_vector;

   double bckgrd = 0;
   for (int k = lower_bckgrd; k <= higher_bckgrd; k++)
   {
     bckgrd += double(histo[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   for (int i = 0; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning); i++)
     histo_fromt0_minus_bckgrd_vector.push_back(0.);

   for (int i = 0; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning); i++)
   {
     for (int j = 0; j < binning; j++)
       histo_fromt0_minus_bckgrd_vector[i] +=
           double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]) - bckgrd;
   }

   return histo_fromt0_minus_bckgrd_vector;
 }


//*******************************
//Implementation get_histo_goodBins_minus_bckgrd_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the 
 *   histogram \<histo_num\> with binning \<binning\> from the point first_good until 
 *   the point last_good. A background calculated from the points \<lower_bckgrd\> and 
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

 double * MuSR_td_PSI_bin::get_histo_goodBins_minus_bckgrd_array(int histo_num, int lower_bckgrd,
                                                                 int higher_bckgrd, int binning)
 {
   if (!readingok) return NULL;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return NULL;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
     return NULL;

   double bckgrd = 0;
   for (int k = lower_bckgrd; k <= higher_bckgrd; k++)
   {
     bckgrd += double(histo[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   double *histo_goodBins_minus_bckgrd_array =
       new double[int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num)+1)/binning)];

   if (!histo_goodBins_minus_bckgrd_array) return NULL;

   for (int i = 0; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning); i++)
   {
     histo_goodBins_minus_bckgrd_array[i] = 0;
     for (int j = 0; j < binning; j++)
       histo_goodBins_minus_bckgrd_array[i] +=
           double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]) - bckgrd;
   }

   return histo_goodBins_minus_bckgrd_array;
 }


//*******************************
//Implementation get_histo_goodBins_minus_bckgrd_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\>
 *   with binning \<binning\> from the point first_good until the point last_good. 
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

 vector<double> MuSR_td_PSI_bin::get_histo_goodBins_minus_bckgrd_vector(int histo_num, int lower_bckgrd,
                                                                        int higher_bckgrd, int binning)
 {
   vector<double> histo_goodBins_minus_bckgrd_vector; ;

   if (!readingok) return histo_goodBins_minus_bckgrd_vector;

   if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
     return histo_goodBins_minus_bckgrd_vector;

   if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
     return histo_goodBins_minus_bckgrd_vector;

   double bckgrd = 0;
   for (int k = lower_bckgrd; k <= higher_bckgrd; k++)
   {
     bckgrd += double(histo[histo_num][k]);
   }
   bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1);

   for (int i = 0; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning); i++)
     histo_goodBins_minus_bckgrd_vector.push_back(0.);

   for (int i = 0; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning); i++)
   {
     for (int j = 0; j < binning; j++)
       histo_goodBins_minus_bckgrd_vector[i] +=
           double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]) - bckgrd;
   }

   return histo_goodBins_minus_bckgrd_vector;
 }


//*******************************
//Implementation get_asymmetry_array
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

 double * MuSR_td_PSI_bin::get_asymmetry_array(int histo_num_plus, int histo_num_minus, double alpha_param,
                                               int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                               int lower_bckgrd_minus, int higher_bckgrd_minus, int offset,
                                               double y_offset)
 {
   int max_t0 = tmax(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus));

   if (!readingok) return NULL;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return NULL;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return NULL;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return NULL;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return NULL;


   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning, offset);
   if (dummy_1 == NULL) return NULL;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning, offset);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return NULL;
   }

   double *asymmetry_array = new double[int((int(length_histo)-max_t0-offset+1)/binning)];

   if (!asymmetry_array) return NULL;

   for (int i = 0; i < int((int(length_histo)-max_t0)/binning); i++)
   {
     asymmetry_array[i] = (dummy_1[i] - alpha_param * dummy_2[i]) /
                          (dummy_1[i] + alpha_param * dummy_2[i]) + y_offset;
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_array;
 }


//*******************************
//Implementation get_asymmetry_vector
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

 vector<double> MuSR_td_PSI_bin::get_asymmetry_vector(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                      int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                      int lower_bckgrd_minus, int higher_bckgrd_minus, int offset, double y_offset)
 {
   int max_t0 = tmax(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus));

   vector<double> asymmetry_vector; // (int((int(length_histo)-max_t0+1)/binning));

   if (!readingok) return asymmetry_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return asymmetry_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return asymmetry_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return asymmetry_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return asymmetry_vector;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning, offset);
   if (dummy_1 == NULL) return asymmetry_vector;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning, offset);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return asymmetry_vector;
   }

   for (int i = 0; i < int((int(length_histo)-max_t0-offset)/binning); i++)
     asymmetry_vector.push_back(0.);

   for (int i = 0; i < int((int(length_histo)-max_t0-offset)/binning); i++)
   {
     asymmetry_vector[i] = (dummy_1[i] - alpha_param * dummy_2[i]) /
                           (dummy_1[i] + alpha_param * dummy_2[i]) + y_offset;
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_vector;
 }


//*******************************
//Implementation get_error_asymmetry_array
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

 double * MuSR_td_PSI_bin::get_error_asymmetry_array(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                     int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                     int lower_bckgrd_minus, int higher_bckgrd_minus, int offset)
 {
   int max_t0 = tmax(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus));

   if (!readingok) return NULL;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return NULL;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return NULL;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return NULL;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return NULL;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning, offset);
   if (dummy_1 == NULL) return NULL;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning, offset);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return NULL;
   }

   double *error_asymmetry_array = new double[int((int(length_histo)-max_t0-offset+1)/binning)];

   if (!error_asymmetry_array) return NULL;

   for (int i = 0; i < int((length_histo-max_t0-offset)/binning); i++)
   {
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
//Implementation get_error_asymmetry_vector
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

 vector<double> MuSR_td_PSI_bin::get_error_asymmetry_vector(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                            int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                            int lower_bckgrd_minus, int higher_bckgrd_minus, int offset)
 {
   int max_t0 = tmax(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus));

   vector<double> error_asymmetry_vector; //(int((int(length_histo)-max_t0+1)/binning));

   if (!readingok) return error_asymmetry_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return error_asymmetry_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return error_asymmetry_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return error_asymmetry_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return error_asymmetry_vector;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning, offset);
   if (dummy_1 == NULL) return error_asymmetry_vector;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning, offset);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return error_asymmetry_vector;
   }

   for (int i = 0; i < int((int(length_histo)-max_t0-offset)/binning); i++)
     error_asymmetry_vector.push_back(0.);

   for (int i = 0; i < int((int(length_histo-max_t0-offset))/binning); i++)
   {
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
//Implementation get_asymmetry_goodBins_array
//*******************************

/*! \brief Method to obtain an array of double containing the values of the asymmetry between 2 histograms.
 *
 *  The array has a size corresponding to the minimum interval between first_good and last_good.
 *  It begins at the first_good coming at the latest after the corresponding t_0.
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

 double * MuSR_td_PSI_bin::get_asymmetry_goodBins_array(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                        int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                        int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((tmin(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),
                         get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning);

   if (!readingok) return NULL;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return NULL;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return NULL;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return NULL;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return NULL;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning);
   if (dummy_1 == NULL) return NULL;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return NULL;
   }

   int hstart = tmax(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus));

   double *asymmetry_goodBins_array = new double[hsize];

   if (!asymmetry_goodBins_array) return NULL;

   for (int i = 0; i < hsize; i++)
   {
     asymmetry_goodBins_array[i] = (dummy_1[i+hstart] - alpha_param * dummy_2[i+hstart]) /
                                   (dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart]);
   }

   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_goodBins_array;
 }


//*******************************
//Implementation get_asymmetry_goodBins_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the asymmetry between 2 histograms.
 *
 *  The vector has a size corresponding to the minimum interval between first_good and last_good.
 *  It begins at the first_good coming at the latest after the corresponding t_0.
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

 vector<double> MuSR_td_PSI_bin::get_asymmetry_goodBins_vector(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                               int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                               int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((tmin(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),
                         get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning);

   vector<double> asymmetry_goodBins_vector;

   if (!readingok) return asymmetry_goodBins_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return asymmetry_goodBins_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return asymmetry_goodBins_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return asymmetry_goodBins_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return asymmetry_goodBins_vector;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning);
   if (dummy_1 == NULL) return asymmetry_goodBins_vector;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return asymmetry_goodBins_vector;
   }

   for (int i = 0; i < hsize; i++)
     asymmetry_goodBins_vector.push_back(0.);

   int hstart = tmax(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus));

   for (int i = 0; i < hsize; i++)
   {
     asymmetry_goodBins_vector[i] = (dummy_1[i+hstart] - alpha_param * dummy_2[i+hstart]) /
                                    (dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart]);
   }
   delete [] dummy_1;
   delete [] dummy_2;

   return asymmetry_goodBins_vector;
 }


//*******************************
//Implementation get_error_asymmetry_goodBins_array
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

 double * MuSR_td_PSI_bin::get_error_asymmetry_goodBins_array(int histo_num_plus, int histo_num_minus, double alpha_param,
                                                              int binning, int lower_bckgrd_plus, int higher_bckgrd_plus,
                                                              int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((tmin(get_lastGood_int(histo_num_plus)
                         -get_firstGood_int(histo_num_plus),
                         get_lastGood_int(histo_num_minus)
                         -get_firstGood_int(histo_num_minus))+1)/binning);

   if (!readingok) return NULL;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return NULL;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return NULL;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return NULL;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return NULL;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus,
                                                         lower_bckgrd_plus, higher_bckgrd_plus, binning);
   if (dummy_1 == NULL) return NULL;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus,
                                                         lower_bckgrd_minus, higher_bckgrd_minus, binning);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return NULL;
   }
   int hstart = tmax(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),
                     get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus));

   double *error_asymmetry_goodBins_array = new double[hsize];

   if (!error_asymmetry_goodBins_array) return NULL;

   for (int i = 0; i < hsize; i++)
   {
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
//Implementation get_error_asymmetry_goodBins_vector
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

 vector<double> MuSR_td_PSI_bin::get_error_asymmetry_goodBins_vector(int histo_num_plus,
                                                                     int histo_num_minus, double alpha_param,
                                                                     int binning, int lower_bckgrd_plus,
                                                                     int higher_bckgrd_plus,
                                                                     int lower_bckgrd_minus, int higher_bckgrd_minus)
 {
   int hsize = int((tmin(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),
                         get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning);

   vector<double> error_asymmetry_goodBins_vector;

   if (!readingok) return error_asymmetry_goodBins_vector;

   if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
     return error_asymmetry_goodBins_vector;

   if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
     return error_asymmetry_goodBins_vector;

   if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
     return error_asymmetry_goodBins_vector;

   if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
     return error_asymmetry_goodBins_vector;

   double *dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus, lower_bckgrd_plus,
                                                         higher_bckgrd_plus, binning);
   if (dummy_1 == NULL) return error_asymmetry_goodBins_vector;

   double *dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus, lower_bckgrd_minus,
                                                         higher_bckgrd_minus, binning);
   if (dummy_2 == NULL)
   {
     delete [] dummy_1;
     return error_asymmetry_goodBins_vector;
   }

   for (int i = 0; i < hsize; i++)
     error_asymmetry_goodBins_vector.push_back(0.);

   int hstart = tmax(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus));

   for (int i = 0; i < hsize; i++)
   {
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
//Implementation get_numberScaler_int
//*******************************

/*! \brief Method returning an integer representing the number of histograms
 */

 int MuSR_td_PSI_bin::get_numberScaler_int()
 {
   return int(number_scaler);
 }

 //*******************************
 //Implementation put_numberScaler_int
 //*******************************

 /*! \brief Method seting the number of scalers present
  *
  * return:
  *  - 0 on success
  *  - -1 if val is out of range (32)
  *
  * \param val the number of scalers present
  */

 int MuSR_td_PSI_bin::put_numberScaler_int(int val)
 {
   if ((val < 0) || (val >= MAXSCALER))
     return -1;

   number_scaler = val;

   return 0;
 }

//*******************************
//Implementation get_scalers_vector
//*******************************

/*! \brief Method providing a vector of long containing the values of the scalers
 */

 vector<long> MuSR_td_PSI_bin::get_scalers_vector()
 {
   vector<long> scalers_vect(number_scaler);

   for ( int i = 0; i < number_scaler; i++ )
     scalers_vect[i] = long(scalers[i]);

   return scalers_vect;
 }

 //*******************************
 //Implementation put_scalers_vector
 //*******************************

 /*! \brief Method set a vector of long containing the values of the scalers
  *
  * return:
  *  - 0 on success
  *  - -1 if scalerData vector is too long (>MAXSCALER)
  *
  * \param scalerData vector
  */

 int MuSR_td_PSI_bin::put_scalers_vector(vector<int> scalerData)
 {
   if ((int)scalerData.size() > MAXSCALER)
     return -1;

   for (unsigned int i=0; i<scalerData.size(); i++)
     scalers[i] = scalerData[i];

   return 0;
 }

//*******************************
//Implementation get_max_t0_int
//*******************************

/*! \brief Method to determine the maximum value of the t0 bins
 */

 int MuSR_td_PSI_bin::get_max_t0_int()
 {
   int max_t0 = 0;

   for (int i = 0; i < int(number_histo); i++)
   {
     if (int(integer_t0[i]) > max_t0)
       max_t0 = int(integer_t0[i]);
   }
   return max_t0;
 }


//*******************************
//Implementation get_max_2_t0_int
//*******************************

/*! \brief Method to determine the maximum value of the last good bins of 2 histograms
 *
 *  returns -1 if the numbers of the histograms are invalid
 */

 int MuSR_td_PSI_bin::get_max_2_t0_int(int k, int j)
 {
   if (( k < 0 || k >= int(number_histo)) || ( j < 0 || j >= int(number_histo)))
     return -1;

   int max_t0 = int(integer_t0[j]);
   if (int(integer_t0[k]) >= max_t0)
     max_t0 = int(integer_t0[k]);

   return max_t0;
 }

//*******************************
//Implementation get_min_2_t0_int
//*******************************

/*! \brief Method to determine the minimum value of the last good bins of 2 histograms
 *
 *  returns -1 if the numbers of the histograms are invalid
 */

 int MuSR_td_PSI_bin::get_min_2_t0_int(int k, int j)
 {
   if (( k < 0 || k >= int(number_histo)) || ( j < 0 || j >= int(number_histo)))
     return -1;

   int min_t0 = int(integer_t0[j]);
   if (int(integer_t0[k]) <= min_t0)
     min_t0 = int(integer_t0[k]);

   return min_t0;
 }

//*******************************
//Implementation get_min_t0_int
//*******************************

/*! \brief Method to determine the minimum value of the t0 bins
 */

 int MuSR_td_PSI_bin::get_min_t0_int()
 {
   int min_t0 = int(length_histo);

   for (int i = 0; i < int(number_histo); i++)
   {
     if (int(integer_t0[i]) < min_t0)
       min_t0 = int(integer_t0[i]);
   }

   return min_t0;
 }


//*******************************
//Implementation get_binWidth_ps
//*******************************

/*! \brief Method returning a double representing the bin-width in picoseconds
 */

 double MuSR_td_PSI_bin::get_binWidth_ps()
 {
   return bin_width*1.0e6;
 }

 //*******************************
 //Implementation put_binWidth_ps
 //*******************************

 /*! \brief Method setting a double representing the bin-width in picoseconds
  *
  * \param binWidth bin-width given in picoseconds
  */

 void MuSR_td_PSI_bin::put_binWidth_ps(double binWidth)
 {
   bin_width = binWidth*1.0e-6;
 }

//*******************************
//Implementation get_binWidth_ns
//*******************************

/*! \brief Method returning a double representing the bin-width in nanoseconds
 */

 double MuSR_td_PSI_bin::get_binWidth_ns()
 {
   return bin_width*1.0e3;
 }

 //*******************************
 //Implementation put_binWidth_ns
 //*******************************

 /*! \brief Method setting a double representing the bin-width in nanoseconds
  *
  * \param binWidth bin-width given in nanoseconds
  */

 void MuSR_td_PSI_bin::put_binWidth_ns(double binWidth)
 {
   bin_width = binWidth*1.0e-3;
 }

//*******************************
//Implementation get_binWidth_us
//*******************************

/*! \brief Method returning a double representing the bin-width in microseconds
 */

 double MuSR_td_PSI_bin::get_binWidth_us()
 {
   return bin_width;
 }

 //*******************************
 //Implementation put_binWidth_us
 //*******************************

 /*! \brief Method setting a double representing the bin-width in microseconds
  *
  * \param binWidth bin-width given in microseconds
  */

 void MuSR_td_PSI_bin::put_binWidth_us(double binWidth)
 {
   bin_width = binWidth;
 }

//*******************************
//Implementation get_histoLength_bin
//*******************************

/*! \brief Method returning an integer representing the histogram length in bins
 */

 int MuSR_td_PSI_bin::get_histoLength_bin()
 {
   return int(length_histo);
 }

//*******************************
//Implementation get_numberHisto_int
//*******************************

/*! \brief Method returning an integer representing the number of histograms
 */

 int MuSR_td_PSI_bin::get_numberHisto_int()
 {
   return int(number_histo);
 }

//*******************************
//Implementation get_totalEvents_long
//*******************************

/*! \brief Method returning a long representing the total number of events
 */

 long MuSR_td_PSI_bin::get_totalEvents_long()
 {
   return long(total_events);
 }

//*******************************
//Implementation get_eventsHisto_long
//*******************************

/*! \brief Method returning a long representing the number of events in a specified histograms
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 long MuSR_td_PSI_bin::get_eventsHisto_long(int i)
 {
   if ( i < 0 || i >= number_histo)
     return -1;
   else
     return long(events_per_histo[i]);
 }

//*******************************
//Implementation get_eventsHisto_vector
//*******************************

/*! \brief Method returning a vector of long containing the number of events in the histograms
 */

 vector<long> MuSR_td_PSI_bin::get_eventsHisto_vector()
 {
   vector<long> eventsHisto(number_histo);

   for ( int i = 0; i < number_histo; i++ )
     eventsHisto[i] = long(events_per_histo[i]);
   return eventsHisto;
 }

//*******************************
//Implementation get_t0_double
//*******************************

/*! \brief Method returning a double representing the t0 point (from the "real" t0 in the header) for a specified histogram
 *
 *  A value of -1. is returned if the value of the histogram \<i\> specified is invalid.
 */

 double MuSR_td_PSI_bin::get_t0_double(int i)
 {
   if ( i < 0 || i >= int(number_histo))
     return -1.;
   else
     return double(real_t0[i]);
 }


//*******************************
//Implementation get_default_binning
//*******************************

/*! \brief Method returning an integer representing the default binning
 *
 */

 int MuSR_td_PSI_bin::get_default_binning()
 {
   if (default_binning < 1)
     return 1;
   else
     return default_binning;
 }

//*******************************
//Implementation get_t0_int
//*******************************

/*! \brief Method returning an integer representing the t0 point (from the "integer" t0 in the header) for a specified histogram
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 int MuSR_td_PSI_bin::get_t0_int(int i)
 {
   if ( i < 0 || i >= int(number_histo))
     return -1;
   else
     return int(integer_t0[i]);
 }

 //*******************************
 //Implementation put_t0_int
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

 int MuSR_td_PSI_bin::put_t0_int(int histoNo, int t0)
 {
   if ((histoNo < 0) || (histoNo >= MAXHISTO))
     return -1;

   integer_t0[histoNo] = t0;

   return 0;
 }

//*******************************
//Implementation get_t0_vector
//*******************************

/*! \brief Method returning a vector of integer containing the t0 values of the histograms specified in the header
 */

 vector<int> MuSR_td_PSI_bin::get_t0_vector()
 {
   vector<int> t0(number_histo);

   for ( int i = 0; i < int(number_histo); i++ )
     t0[i] = int(integer_t0[i]);

   return t0;
 }

 //*******************************
 //Implementation put_t0_vector
 //*******************************

 /*! \brief Method setting a vector of integer containing the t0 values of the histograms specified in the header
  *
  * return:
  *  - 0 on success
  *  - -1 if t0Data is too long (>MAXHISTO)
  */

 int MuSR_td_PSI_bin::put_t0_vector(vector<int> &t0Data)
 {
   if ((int)t0Data.size() >= MAXHISTO)
     return -1;

   for (unsigned int i=0; i<t0Data.size(); i++)
     integer_t0[i] = t0Data[i];

   return 0;
 }

//*******************************
//Implementation get_firstGood_int
//*******************************

/*! \brief Method returning an integer representing the first good bin specified in the header for a specified histogram
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 int MuSR_td_PSI_bin::get_firstGood_int(int i)
 {
   if ( i < 0 || i >= int(number_histo))
     return -1;
   else
     return int(first_good[i]);
 }

 //*******************************
 //Implementation put_firstGood_int
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

 int MuSR_td_PSI_bin::put_firstGood_int(int i, int j)
 {
   if ((i < 0) || (i >= MAXHISTO))
     return -1;

   first_good[i] = j;

   return 0;
 }

//*******************************
//Implementation get_firstGood_vector
//*******************************

/*! \brief Method returning a vector of integer containing the first good bin values of the histograms specified in the header
 */

 vector<int> MuSR_td_PSI_bin::get_firstGood_vector()
 {
   vector<int> firstGood(number_histo);

   for ( int i = 0; i < number_histo; i++ )
     firstGood[i] = int(first_good[i]);

   return firstGood;
 }

//*******************************
//Implementation get_lastGood_int
//*******************************

/*! \brief Method returning an integer representing the last good bin specified in the header for a specified histogram
 *
 *  A value of -1 is returned if the value of the histogram \<i\> specified is invalid.
 */

 int MuSR_td_PSI_bin::get_lastGood_int(int i)
 {
   if ( i < 0 || i >= int(number_histo))
     return -1;
   else
     return int(last_good[i]);
 }

//*******************************
//Implementation get_lastGood_vector
//*******************************

/*! \brief Method returning a vector of integer containing the last good bin values of the histograms specified in the header
 */

 vector<int> MuSR_td_PSI_bin::get_lastGood_vector()
 {
   vector<int> lastGood(number_histo);

   for ( int i = 0; i < number_histo; i++ )
     lastGood[i] = int(last_good[i]);

   return lastGood;
 }

//*******************************
//Implementation get_max_lastGoog_int
//*******************************

/*! \brief Method returning an integer containing the maximum value of the "last good bins" of all histograms
 */

 int MuSR_td_PSI_bin::get_max_lastGood_int()
 {
   int max_lastGood = 0;

   for (int i = 0; i < int(number_histo); i++)
   {
     if (int(last_good[i]) > max_lastGood)
       max_lastGood = int(last_good[i]);
   }

   return max_lastGood;
 }

//*******************************
//Implementation get_max_2_lastGood_int
//*******************************

/*! \brief Method to determine the maximum value of the "last good bins" of 2 histograms
 *
 *  returns -1 if something is invalid
 */

 int MuSR_td_PSI_bin::get_max_2_lastGood_int(int k, int j)
 {
   if (( k < 0 || k >= int(number_histo)) || ( j < 0 || j >= int(number_histo)))
     return -1;
   else
   {
     int max_lastGood = int(last_good[j]);

     if (int(last_good[k]) > max_lastGood)
       max_lastGood = int(last_good[k]);

     return max_lastGood;
   }
 }

//*******************************
//Implementation get_min_lastGood_int
//*******************************

/*! \brief Method providing the minimum value of the last good bins
 */

 int MuSR_td_PSI_bin::get_min_lastGood_int()
 {
   int min_lastGood = int(last_good[0]);

   for (int i = 1; i < int(number_histo); i++)
   {
     if (int(last_good[i]) < min_lastGood)
       min_lastGood = int(last_good[i]);
   }

   return min_lastGood;
 }

//*******************************
//Implementation get_min_2_lastGood_int
//*******************************

/*! \brief Method to determine the minimum value of the last good bins of 2 histograms
 *
 *  returns -1 if something is invalid
 */

 int MuSR_td_PSI_bin::get_min_2_lastGood_int(int k, int j)
 {
   if (( k < 0 || k >= int(number_histo)) || ( j < 0 || j >= int(number_histo)))
     return -1;
   else
   {
     int min_lastGood = int(last_good[j]);

     if (int(last_good[k]) < min_lastGood)
       min_lastGood = int(last_good[k]);

     return min_lastGood;
   }
 }

//*******************************
//Implementation put_lastGood_int
//*******************************

/*! \brief Method to modify the last good bin (value \<j\>) of the histogram \<i\>
 *
 *  returns -1 if the histogram specified was invalid
 */

 int MuSR_td_PSI_bin::put_lastGood_int(int i, int j)
 {
   if ((i < 0) || (i >= number_histo))
     return -1;

   last_good[i] = j;

   return  0;
 }


//*******************************
//Implementation get_runNumber_int
//*******************************

/*! \brief Method returning an integer containing the run number
 */

 int MuSR_td_PSI_bin::get_runNumber_int()
 {
   return int(num_run);
 }


//*******************************
//Implementation put_runNumber_int
//*******************************

/*! \brief Method to modify the run number (value \<i\>)
 *
 *  returns -1 if the integer specified was wrong
 */

 int MuSR_td_PSI_bin::put_runNumber_int(int i)
 {
   if (i <= 0 )
     return -1;

   num_run = i;

   return 0;
 }


//*******************************
//Implementation get_sample()
//*******************************

/*! \brief Method returning a string containing the sample name
 */

 string MuSR_td_PSI_bin::get_sample()
 {
   string strData;

   strData = sample;

   return strData;
 }

 //*******************************
 //Implementation put_sample()
 //*******************************

 /*! \brief Method setting a string containing the sample name
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided sample string is too long (>= 11 char)
  *
  * \param sample string containing the sample information
  */

 int MuSR_td_PSI_bin::put_sample(string sampleStr)
 {
   if (sampleStr.size() >= 11)
     return -1;

   strcpy(sample, sampleStr.c_str());

   return 0;
 }

//*******************************
//Implementation get_temp()
//*******************************

/*! \brief Method returning a string containing the temperature specified in the title
 */

 string MuSR_td_PSI_bin::get_temp()
 {
   string strData;

   strData = temp;

   return strData;
 }

 //*******************************
 //Implementation put_temp()
 //*******************************

 /*! \brief Method setting a string containing the sample temperature
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided temperature string is too long (>= 11 char)
  *
  * \param tempStr string containing the sample information
  */

 int MuSR_td_PSI_bin::put_temp(string tempStr)
 {
   if (tempStr.size() >= 11)
     return -1;

   strcpy(temp, tempStr.c_str());

   return 0;
 }

//*******************************
//Implementation get_orient()
//*******************************

/*! \brief Method returning a string containing the orientation specified in the title
 */

 string MuSR_td_PSI_bin::get_orient()
 {
   string strData;

   strData = orient;

   return strData;
 }

 //*******************************
 //Implementation put_orient()
 //*******************************

 /*! \brief Method setting a string containing the sample orientation
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided orientation string is too long (>= 11 char)
  *
  * \param orientStr string containing the sample information
  */

 int MuSR_td_PSI_bin::put_orient(string orientStr)
 {
   if (orientStr.size() >= 11)
     return -1;

   strcpy(orient, orientStr.c_str());

   return 0;
 }

//*******************************
//Implementation get_field()
//*******************************

/*! \brief Method returning a string containing the field specified in the title
 */

 string MuSR_td_PSI_bin::get_field()
 {
   string strData;

   strData = field;

   return strData;
 }

 //*******************************
 //Implementation put_field()
 //*******************************

 /*! \brief Method setting a string containing the field
  *
  * return:
  *  - 0 on success
  *  - -1 if the provided field string is too long (>= 11 char)
  *
  * \param fieldStr string containing the field
  */

 int MuSR_td_PSI_bin::put_field(string fieldStr)
 {
   if (fieldStr.size() >= 11)
     return -1;

   strcpy(field, fieldStr.c_str());

   return 0;
 }

 //*******************************
 //Implementation get_setup()
 //*******************************

 /*! \brief Method returning a string containing the setup
  */

  string MuSR_td_PSI_bin::get_setup()
  {
    string strData;

    strData = setup;

    return strData;
  }

  //*******************************
  //Implementation put_setup()
  //*******************************

  /*! \brief Method setting a string containing the setup
   *
   * return:
   *  - 0 on success
   *  - -1 if the provided setup string is too long (>= 11 char)
   *
   * \param commentStr string containing the setup
   */

  int MuSR_td_PSI_bin::put_setup(string setupStr)
  {
    if (setupStr.size() >= 11)
      return -1;

    strcpy(setup, setupStr.c_str());

    return 0;
  }

//*******************************
//Implementation get_comments()
//*******************************

/*! \brief Method returning a string containing the comment specified in the title
 */

 string MuSR_td_PSI_bin::get_comment()
 {
   string strData;

   strData = comment;

   return strData;
 }

 //*******************************
 //Implementation put_comments()
 //*******************************

 /*! \brief Method setting a string containing the comment
  *
  * return:
  *  - 0 on success
  *
  * \param commentStr string containing the comment
  */

 int MuSR_td_PSI_bin::put_comment(string commentStr)
 {
   strncpy(comment, commentStr.c_str(), 62);
   comment[62] = '\0';

   return 0;
 }

//*******************************
//Implementation get_nameHisto()
//*******************************

/*! \brief Method returning a string containing the name of the histogram \<i\>
 *
 *  returns NULL if the histogram specified is invalid
 */

 string MuSR_td_PSI_bin::get_nameHisto(int i)
 {
   string strData;

   if (i < 0 || i >= int(number_histo))
     return NULL;
   else
   {
     strData = labels_histo[i];
     return strData;
   }
 }

 //*******************************
 //Implementation put_nameHisto()
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

 int MuSR_td_PSI_bin::put_nameHisto(string histoName, int i)
 {
   if ((i<0) || (i>=number_histo))
     return -1;

   if ((int)histoName.length() >= MAXLABELSIZE)
     return -2;

   strcpy(labels_histo[i], histoName.c_str());

   return 0;
 }

//*******************************
//Implementation get_histoNames_vector()
//*******************************

/*! \brief Method returning a vector of strings containing the names of the histograms
 */

 vector<string> MuSR_td_PSI_bin::get_histoNames_vector()
 {
   vector <string> str_Vector;

   string strData;
   for (int i = 0; i < number_histo; i++)
   {
     strData = labels_histo[i];
     str_Vector.push_back(strData);
   }

   return str_Vector;
 }

 //*******************************
 //Implementation put_histoNames_vector()
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

 int MuSR_td_PSI_bin::put_histoNames_vector(vector<string> &histoNames)
 {
   if ((int)histoNames.size() > number_histo)
     return -1;

   for (unsigned int i=0; i<histoNames.size(); i++) {
     if ((int)histoNames[i].length() >= MAXLABELSIZE)
       return -2;
     else
       strcpy(labels_histo[i], histoNames[i].c_str());
   }

   return 0;
 }

//*******************************
//Implementation get_scalersNames_vector()
//*******************************

/*! \brief Method returning a vector of strings containing the names of the scalers
 */

 vector<string> MuSR_td_PSI_bin::get_scalersNames_vector()
 {
   vector <string> str_Vector;

   string strData;
   for (int i = 0; i < number_scaler; i++)
   {
     strData = labels_scalers[i];
     str_Vector.push_back(strData);
   }

   return str_Vector;
 }

 //*******************************
 //Implementation put_scalersNames_vector()
 //*******************************

 /*! \brief Method setting a vector of strings containing the names of the scalers
  *
  * return:
  *  - 0 on success
  *  - -1 of scalersName is too long (>MAXSCALER)
  *
  * \param scalersName scaler names
  */

 int MuSR_td_PSI_bin::put_scalersNames_vector(vector<string> scalersName)
 {
   if ((int)scalersName.size() > MAXSCALER)
     return -1;

   for (unsigned int i=0; i<scalersName.size(); i++) {
     strncpy(labels_scalers[i], scalersName[i].c_str(), MAXLABELSIZE-1);
     labels_scalers[i][MAXLABELSIZE-1] = '\0';
   }

   return 0;
 }

//*******************************
//Implementation get_numberTemperature_int
//*******************************

/*! \brief Method returning an integer representing the number of temperatures
 */

 int MuSR_td_PSI_bin::get_numberTemperature_int()
 {
   return number_temper;
 }

 //*******************************
 //Implementation put_numberTemperature_int
 //*******************************

 /*! \brief Method setting an integer representing the number of temperatures
  *
  * return:
  *  - 0 on success
  *  - -1 if noOfTemps is out of range
  *
  * \param noOfTemps number of temperatures present
  */

 int MuSR_td_PSI_bin::put_numberTemperature_int(int noOfTemps)
 {
   if ((noOfTemps < 0) || (noOfTemps > MAXTEMPER))
     return -1;

   number_temper = noOfTemps;

   return 0;
 }

//*******************************
//Implementation get_temperatures_vector()
//*******************************

/*! \brief Method returning a vector of doubles containing monitored values (usually temperatures)
 */

 vector<double> MuSR_td_PSI_bin::get_temperatures_vector()
 {
   vector <double> dbl_Temper;

   for (int i = 0; i < number_temper; i++)
   {
     dbl_Temper.push_back(double(temper[i]));
   }

   return dbl_Temper;
 }

 //*******************************
 //Implementation put_temperatures_vector()
 //*******************************

 /*! \brief Method setting a vector of doubles containing monitored values (usually temperatures)
  *
  * return:
  *  - 0 on success
  *  - -1 if the number of tempVals elements is out of range
  *
  * \param tempVals vector containing the monitored values
  */

 int MuSR_td_PSI_bin::put_temperatures_vector(vector<double> &tempVals)
 {
   if ((int)tempVals.size() > MAXTEMPER)
     return -1;

   for (unsigned int i=0; i<tempVals.size(); i++)
     temper[i] = tempVals[i];

   return 0;
 }

//*******************************
//Implementation get_devTemperatures_vector()
//*******************************

/*! \brief Method returning a vector of doubles containing standard deviations of the monitored values (usually temperatures)
 */

 vector<double> MuSR_td_PSI_bin::get_devTemperatures_vector()
 {
   vector <double> dbl_devTemper;

   for (int i = 0; i < number_temper; i++)
   {
     dbl_devTemper.push_back(double(temp_deviation[i]));
   }

   return dbl_devTemper;
 }

 //*******************************
 //Implementation put_devTemperatures_vector()
 //*******************************

 /*! \brief Method setting a vector of doubles containing standard deviations of the monitored values (usually temperatures)
  *
  * return:
  *  - 0 on success
  *  - -1 if the number of devTempVals elements is out of range
  *
  * \param devTempVals vector containing standard deviations of the monitored values
  */

 int MuSR_td_PSI_bin::put_devTemperatures_vector(vector<double> &devTempVals)
 {
   if ((int)devTempVals.size() > MAXTEMPER)
     return -1;

   for (unsigned int i=0; i<devTempVals.size(); i++)
     temp_deviation[i] = devTempVals[i];

   return 0;
 }

//*******************************
//Implementation get_timeStart_vector()
//*******************************

/*! \brief Method returning a vector of strings containing 1) the date when the run was 
 *   started and 2) the time when the run was started
 */

 vector<string> MuSR_td_PSI_bin::get_timeStart_vector()

 {
   vector<string> timeStart(2);

   timeStart[0] = date_start;
   timeStart[1] = time_start;

   return timeStart;
 }

 //*******************************
 //Implementation put_timeStart_vector()
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

 int MuSR_td_PSI_bin::put_timeStart_vector(vector<string> timeStart)
 {
   if (timeStart.size() != 2)
     return -1;

   // date
   if (timeStart[0].length() > 9)
     return -2;

   // time
   if (timeStart[1].length() > 8)
     return -3;

   strcpy(date_start, timeStart[0].c_str());
   strcpy(time_start, timeStart[1].c_str());

   return 0;
 }

//*******************************
//Implementation get_timeStop_vector()
//*******************************

/*! \brief Method returning a vector of strings containing 1) the date when the run was 
 *   stopped and 2) the time when the run was stopped
 */

 vector<string> MuSR_td_PSI_bin::get_timeStop_vector()
 {
   vector<string> timeStop(2);

   timeStop[0] = date_stop;
   timeStop[1] = time_stop;

   return timeStop;
 }

 //*******************************
 //Implementation put_timeStop_vector()
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

 int MuSR_td_PSI_bin::put_timeStop_vector(vector<string> timeStop)
 {
   if (timeStop.size() != 2)
     return -1;

   // date
   if (timeStop[0].length() > 9)
     return -2;

   // time
   if (timeStop[1].length() > 8)
     return -3;

   strcpy(date_stop, timeStop[0].c_str());
   strcpy(time_stop, timeStop[1].c_str());

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
   if (histo != NULL)
   {
     for (i=0; i < number_histo; i++)
       if (*(histo+i) != NULL)
       {
       delete[] *(histo+i);
       *(histo+i) = NULL;
     }
     delete [] histo;
     histo = NULL;
   }

   // free public vector
   histos_vector.clear();

   // init other member variables
   filename  = "?";
   readingok = false;
   writingok = false;
   consistencyOk = false;
   readstatus = "";
   writestatus = "";
   consistencyStatus = "";

   strcpy(format_id,"??");

   num_run = 0;
   //01234567890
   strcpy(sample,    "          ");
   strcpy(temp,      "          ");
   strcpy(field,     "          ");
   strcpy(orient,    "          ");
   strcpy(setup,     "          ");
   strcpy(comment,   "          ");
   strcpy(date_start,"         ");
   strcpy(time_start,"        ");
   strcpy(date_stop, "         ");
   strcpy(time_stop, "        ");

   bin_width = 0.;
   number_histo = 0;
   length_histo = 0;
   total_events = 0;
   default_binning = 1;

   for (i=0; i < MAXHISTO; i++)
   {
     for (j=0; j < MAXLABELSIZE-1; j++)
       labels_histo[i][j] = ' ';
     labels_histo[i][MAXLABELSIZE-1] = '\0';
     events_per_histo[i] = 0;
     real_t0[i]    = 0.f;
     integer_t0[i] = 0;
     first_good[i] = 0;
     last_good[i]  = 0;
   }

   number_scaler = 0;
   for (i=0; i < MAXSCALER; i++)
   {
     for (j=0; j < MAXLABELSIZE-1; j++)
       labels_scalers[i][j] = ' ';
     labels_scalers[i][MAXLABELSIZE-1] = '\0';

     scalers[i] = 0;
   }

   number_temper = 0;
   for (i=0; i < MAXTEMPER; i++)
   {
     temper[i] = 0.f;
     temp_deviation[i] = 0.f;
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
   cout << "Filename is " << filename << endl;
   if (readingok) {
     int i;

     cout << "Format Identifier is " << format_id << endl;

     cout << "Run number is  " << num_run << endl;
     cout << "Sample is      " << sample  << endl;
     cout << "Temperature is " << temp    << endl;
     cout << "Field is       " << field   << endl;
     cout << "Orientation is " << orient  << endl;
     cout << "Comment is     " << comment << endl;

     cout << "Start Date is  " << date_start << endl;
     cout << "Start Time is  " << time_start << endl;

     cout << "End Date is    " << date_stop << endl;
     cout << "End Time is    " << time_stop << endl;

     cout << "Bin width is   " << bin_width << " [usec]" << endl;
     cout << "Number of histograms is " << number_histo << endl;
     cout << "Histogram length is     " << length_histo << endl;
     cout << "Default binning is      " << default_binning << endl;
     cout << "Total number of events is " << total_events << endl;

     for (i=0; i < number_histo; i++) {
       cout << "Histogram " << i << " Name is >" << labels_histo[i]
           << "<  Events per histogram is " << events_per_histo[i] << endl;
       cout << "      real t0 is              " << real_t0[i] << endl;
       cout << "      t0 is                   " << integer_t0[i] << endl;
       cout << "      first good bin is       " << first_good[i] << endl;
       cout << "      last good bin is        " << last_good[i] << endl;
     }

     cout << "Number of scalers is " << number_scaler << endl;
     for (i=0; i < number_scaler; i++) {
       cout << "Scaler " << i << " Name is >" << labels_scalers[i]
           << "<   Value is " << scalers[i] << endl;
     }

     cout << "Number of temperatures is " << number_temper << endl;
     for (i=0; i < number_temper; i++) {
       cout << "Temperature " << i << " is " << temper[i]
           << "   Deviation is " << temp_deviation[i] << endl;
     }

   } else {
     cout << readstatus << endl;
   }
   return 0;
 }

//*******************************
//Implementation tmax
//*******************************

 int MuSR_td_PSI_bin::tmax(int x, int y)
 {
   if (x >= y)
   {
     return x;
   }
   return y;
 }


//*******************************
//Implementation tmin
//*******************************

 int MuSR_td_PSI_bin::tmin(int x, int y)
 {
   if (x >= y)
   {
     return y;
   }
   return x;
 }

/************************************************************************************
 * EOF MuSR_td_PSI_bin.cpp                                                       *
 ************************************************************************************/

