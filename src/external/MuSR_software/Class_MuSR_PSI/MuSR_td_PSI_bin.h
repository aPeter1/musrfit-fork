/**************************************************************************************

  MuSR_td_PSI_bin.h

  declaration file of the class 'MuSR_td_PSI_bin'

  Main class to read mdu and td_bin PSI MuSR data.

***************************************************************************************

    begin                : Alex Amato, October 2005
    modified             : Andrea Raselli, October 2009
                         : Andreas Suter, April 2019, May 2020
    copyright            : (C) 2005 by
    email                : alex.amato@psi.ch

***************************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef MuSR_td_PSI_bin_H_
#define MuSR_td_PSI_bin_H_

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>

/* ------------------------------------------------------------------ */

const int MAXHISTO     = 32;  // maximum number of histos to process/store
const int MAXSCALER    = 32;  // maximum number of scalers to process/store
const int MAXTEMPER    =  4;  // maximum number of average temperatures

const int MAXLABELSIZE = 12;  // maximum size of labels

const int MAXREC = 4096;      // maximum allowed data record size

/* ------------------------------------------------------------------ */

class MuSR_td_PSI_bin {

  public:
    MuSR_td_PSI_bin();
   ~MuSR_td_PSI_bin();

  private:
    std::string fFilename;
    std::string fReadStatus;
    std::string fWriteStatus;
    std::string fConsistencyStatus;
    bool     fReadingOk;
    bool     fWritingOk;
    bool     fConsistencyOk;

    char     fFormatId[3];

    int      fNumRun;

    char     fSample[11];
    char     fTemp[11];
    char     fField[11];
    char     fOrient[11];
    char     fSetup[11];
    char     fComment[63];

    char     fDateStart[10];
    char     fDateStop[10];
    char     fTimeStart[9];
    char     fTimeStop[9];

    double   fBinWidth;

    int      fNumberHisto;
    int      fLengthHisto;
    char     fLabelsHisto[MAXHISTO][MAXLABELSIZE];

    int      fTotalEvents;
    int      fEventsPerHisto[MAXHISTO];

    int      fDefaultBinning;

    float    fRealT0[MAXHISTO];
    int      fIntegerT0[MAXHISTO];
    int      fFirstGood[MAXHISTO];
    int      fLastGood[MAXHISTO];

    int      fNumberScaler;
    int      fScalers[MAXSCALER];
    char     fLabelsScalers[MAXSCALER][MAXLABELSIZE];

    int      fNumberTemper;
    float    fTemper[MAXTEMPER];
    float    fTempDeviation[MAXTEMPER];

    std::vector< std::vector<int> > fHisto;

  public:

/*!< this public variable provides a direct read/write access to the histograms.
     However all public methods use the protected variable histo.
     Histogram information returned by ..._vector or ..._array methods return
     information based on histo bin .

     NOTE: Histogram information returned by \<pointer_to_array\> = ..._array() methods
           should be freed by  delete [] \<pointer_to_array\>;
 */
    std::vector< std::vector<double> >  fHistosVector ;

// ------------------------------------end of the variables

  public:

    int            Read(const char* fileName);      // generic read
    int            Write(const char *fileName);     // generic write

    int            ReadBin(const char* fileName);   // read MuSR PSI bin format
    int            WriteBin(const char *fileName);  // write MuSR PSI bin format
    int            ReadMdu(const char* fileName);   // read MuSR mdu format
    int            WriteMdu(const char* fileName);  // write MuSR mdu format

    bool           ReadingOK()     const;
    bool           WritingOK()     const;
    bool           CheckDataConsistency(int tag=0); // tag: 0=reasonable, 1=strict
    std::string    ReadStatus()    const;
    std::string    WriteStatus()   const;
    std::string    ConsistencyStatus() const;
    std::string    Filename()      const;

    int            Show()          const;
    int            Clear();

    int             GetHistoInt(int histo_num, int j);
    double          GetHisto(int histo_num, int j);

    std::vector<int>    GetHistoArrayInt(int histo_num);
    std::vector<double> GetHistoArray(int histo_num, int binning);
    int                 PutHistoArrayInt(std::vector< std::vector<int> > &histo, int tag = 0);
    std::vector<double> GetHistoVector(int histo_num, int binning);
    std::vector<double> GetHistoVectorNo0(int histo_num, int binning);

    double             *GetHistoFromT0Array(int histo_num, int binning, int offset = 0);

    std::vector<double> GetHistoFromT0Vector(int histo_num, int binning, int offset = 0);

    double             *GetHistoGoodBinsArray(int histo_num, int binning);

    std::vector<double> GetHistoGoodBinsVector(int histo_num, int binning);

    double             *GetHistoFromT0MinusBkgArray(int histo_num,
                                                    int lower_bckgdr,
                                                    int higher_bckgdr,
                                                    int binning,
                                                    int offset = 0);

   std::vector<double>  GetHistoFromT0MinusBkgVector(int histo_num,
                                                     int lower_bckgdr,
                                                     int higher_bckgdr,
                                                     int binning,
                                                     int offset = 0);

   double              *GetHistoGoodBinsMinusBkgArray(int histo_num,
                                                      int lower_bckgrd,
                                                      int higher_bckgrd,
                                                      int binning);

   std::vector<double>  GetHistoGoodBinsMinusBkgVector(int histo_num,
                                                       int lower_bckgrd,
                                                       int higher_bckgrd,
                                                       int binning);

   double              *GetAsymmetryArray(int histo_num_plus,
                                          int histo_num_minus,
                                          double alpha_param,
                                          int binning,
                                          int lower_bckgrd_plus,
                                          int higher_bckgrd_plus,
                                          int lower_bckgrd_minus,
                                          int higher_bckgrd_minus,
                                          int offset = 0,
                                          double y_offset = 0.);

   std::vector<double>  GetAsymmetryVector(int histo_num_plus,
                                           int histo_num_minus,
                                           double alpha_param,
                                           int binning,
                                           int lower_bckgrd_plus,
                                           int higher_bckgrd_plus,
                                           int lower_bckgrd_minus,
                                           int higher_bckgrd_minus,
                                           int offset = 0,
                                           double y_offset = 0.);

   double              *GetErrorAsymmetryArray(int histo_num_plus,
                                               int histo_num_minus,
                                               double alpha_param,
                                               int binning,
                                               int lower_bckgrd_plus,
                                               int higher_bckgrd_plus,
                                               int lower_bckgrd_minus,
                                               int higher_bckgrd_minus,
                                               int offset = 0);

   std::vector<double>  GetErrorAsymmetryVector(int histo_num_plus,
                                                int histo_num_minus,
                                                double alpha_param,
                                                int binning,
                                                int lower_bckgrd_plus,
                                                int higher_bckgrd_plus,
                                                int lower_bckgrd_minus,
                                                int higher_bckgrd_minus,
                                                int offset = 0);

   double              *GetAsymmetryGoodBinsArray(int histo_num_plus,
                                                  int histo_num_minus,
                                                  double alpha_param,
                                                  int binning,
                                                  int lower_bckgrd_plus,
                                                  int higher_bckgrd_plus,
                                                  int lower_bckgrd_minus,
                                                  int higher_bckgrd_minus);

   std::vector<double>  GetAsymmetryGoodBinsVector(int histo_num_plus,
                                                   int histo_num_minus,
                                                   double alpha_param,
                                                   int binning,
                                                   int lower_bckgrd_plus,
                                                   int higher_bckgrd_plus,
                                                   int lower_bckgrd_minus,
                                                   int higher_bckgrd_minus);

    double             *GetErrorAsymmetryGoodBinsArray(int histo_num_plus,
                                                       int histo_num_minus,
                                                       double alpha_param,
                                                       int binning,
                                                       int lower_bckgrd_plus,
                                                       int higher_bckgrd_plus,
                                                       int lower_bckgrd_minus,
                                                       int higher_bckgrd_minus);

   std::vector<double>   GetErrorAsymmetryGoodBinsVector(int histo_num_plus,
                                                         int histo_num_minus,
                                                         double alpha_param,
                                                         int binning,
                                                         int lower_bckgrd_plus,
                                                         int higher_bckgrd_plus,
                                                         int lower_bckgrd_minus,
                                                         int higher_bckgrd_minus);


    double          GetBinWidthPicoSec();
    void            PutBinWidthPicoSec(double binWidth);
    double          GetBinWidthNanoSec();
    void            PutBinWidthNanoSec(double binWidth);
    double          GetBinWidthMicroSec();
    void            PutBinWidthMicroSec(double binWidth);

    int             GetHistoLengthBin() { return fLengthHisto; }
    void            PutHistoLengthBin(int val) { fLengthHisto = val; }

    int             GetNumberHistoInt() { return fNumberHisto; }
    void            PutNumberHistoInt(int val) { fNumberHisto = val; }

    std::string     GetNameHisto(int i);
    int             PutNameHisto(std::string histoName, int i);
    std::vector<std::string> GetHistoNamesVector();
    int             PutHistoNamesVector(std::vector<std::string> &histoNames);

    long            GetEventsHistoLong(int i);
    std::vector<long> GetEventsHistoVector();

    long            GetTotalEventsLong() { return long(fTotalEvents); }

    int             GetNumberScalerInt();
    int             PutNumberScalerInt(int val);
    std::vector<long> GetScalersVector();
    int             PutScalersVector(std::vector<int> scalerData);
    std::vector<std::string> GetScalersNamesVector();
    int             PutScalersNamesVector(std::vector<std::string> scalersName);

    int             GetDefaultBinning();
    int             GetT0Int(int i);
    int             PutT0Int(int histoNo, int t0);
    std::vector<int> GetT0Vector();
    int             PutT0Vector(std::vector<int> &t0Data);
    double          GetT0Double(int i);

    int             GetMaxT0Int();
    int             GetMax2T0Int (int k, int j);
    int             GetMinT0Int();
    int             GetMin2T0Int (int k, int j);

    int             GetFirstGoodInt(int i);
    std::vector<int> GetFirstGoodVector();
    int             PutFirstGoodInt(int i, int j);

    int             GetLastGoodInt(int i);
    std::vector<int> GetLastGoodVector();
    int             PutLastGoodInt(int i, int j);

    int             GetMaxLastGoodInt();
    int             GetMax2LastGoodInt (int k, int j);
    int             GetMinLastGoodInt();
    int             GetMin2LastGoodInt (int k, int j);

    int             GetRunNumberInt() { return fNumRun; }
    int             PutRunNumberInt(int i);

    std::string     GetSample();
    int             PutSample(std::string sample);
    std::string     GetField();
    int             PutField(std::string field);
    std::string     GetOrient();
    int             PutOrient(std::string orientation);
    std::string     GetTemp();
    int             PutTemp(std::string temp);
    std::string     GetSetup();
    int             PutSetup(std::string setup);
    std::string     GetComment();
    int             PutComment(std::string comment);

    std::vector<std::string>  GetTimeStartVector();
    int             PutTimeStartVector(std::vector<std::string> timeStart);
    std::vector<std::string>  GetTimeStopVector();
    int             PutTimeStopVector(std::vector<std::string> timeStop);

    int             GetNumberTemperatureInt() { return fNumberTemper; }
    int             PutNumberTemperatureInt(int noOfTemps);
    std::vector<double>  GetTemperaturesVector();
    int             PutTemperaturesVector(std::vector<double> &temps);
    std::vector<double>  GetDevTemperaturesVector();
    int             PutDevTemperaturesVector(std::vector<double> &devTemps);

  private:

    int Tmax(int x, int y);
    int Tmin(int x, int y);

} ;
#endif
/************************************************************************************
 * EOF MuSR_td_PSI_bin.h                                                         *
 ************************************************************************************/
