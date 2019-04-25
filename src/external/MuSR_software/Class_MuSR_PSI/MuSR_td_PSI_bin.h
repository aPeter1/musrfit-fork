/**************************************************************************************

  MuSR_td_PSI_bin.h

  declaration file of the class 'MuSR_td_PSI_bin'

  Main class to read mdu and td_bin PSI MuSR data.

***************************************************************************************

    begin                : Alex Amato, October 2005
    modified             : Andrea Raselli, October 2009
                         : Andreas Suter, April 2019
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
// ------------------------------------start of the variables

    std::string filename;
    std::string readstatus;
    std::string writestatus;
    std::string consistencyStatus;
    bool     readingok;
    bool     writingok;
    bool     consistencyOk;

    char     format_id[3];

    int      num_run;

    char     sample[11];
    char     temp[11];
    char     field[11];
    char     orient[11];
    char     setup[11];
    char     comment[63];

    char     date_start[10];
    char     date_stop[10];
    char     time_start[9];
    char     time_stop[9];

    double   bin_width;

    int      number_histo;
    int      length_histo;
    char     labels_histo[MAXHISTO][MAXLABELSIZE];

    int      total_events;
    int      events_per_histo[MAXHISTO];

    int      default_binning;

    float    real_t0[MAXHISTO];
    int      integer_t0[MAXHISTO];
    int      first_good[MAXHISTO];
    int      last_good[MAXHISTO];

    int      number_scaler;
    int      scalers[MAXSCALER];
    char     labels_scalers[MAXSCALER][MAXLABELSIZE];

    int      number_temper;
    float    temper[MAXTEMPER];
    float    temp_deviation[MAXTEMPER];

    int      **histo;

  public:

/*!< this public variable provides a direct read/write access to the histograms.
     However all public methods use the protected variable histo.
     Histogram information returned by ..._vector or ..._array methods return
     information based on histo bin .

     NOTE: Histogram information returned by \<pointer_to_array\> = ..._array() methods
           should be freed by  delete [] \<pointer_to_array\>;
 */
    std::vector< std::vector<double> >  histos_vector ;

// ------------------------------------end of the variables

  public:

    int            read(const char* fileName);      // generic read
    int            write(const char *fileName);     // generic write

    int            readbin(const char* fileName);   // read MuSR PSI bin format
    int            writebin(const char *fileName);  // write MuSR PSI bin format
    int            readmdu(const char* fileName);   // read MuSR mdu format
    int            writemdu(const char* fileName);  // write MuSR mdu format

    bool           readingOK()     const;
    bool           writingOK()     const;
    bool           CheckDataConsistency(int tag=0); // tag: 0=reasonable, 1=strict
    std::string    ReadStatus()    const;
    std::string    WriteStatus()   const;
    std::string    ConsistencyStatus() const;
    std::string    Filename()      const;

    int            Show()          const;
    int            Clear();

    int             get_histo_int(int histo_num, int j);
    double          get_histo(int histo_num, int j);

    int            *get_histo_array_int(int histo_num);
    double         *get_histo_array(int histo_num, int binning);
    int             put_histo_array_int(std::vector< std::vector<int> > histo, int tag = 0);
    std::vector<double>  get_histo_vector(int histo_num, int binning);
    std::vector<double>  get_histo_vector_no0(int histo_num, int binning);

    double         *get_histo_fromt0_array(int histo_num, int binning, int offset = 0);

    std::vector<double>  get_histo_fromt0_vector(int histo_num, int binning, int offset = 0);

    double         *get_histo_goodBins_array(int histo_num, int binning);

    std::vector<double>  get_histo_goodBins_vector(int histo_num, int binning);

    double         *get_histo_fromt0_minus_bckgrd_array(int histo_num,
                                                         int lower_bckgdr,
                                                         int higher_bckgdr,
                                                         int binning,
                                                         int offset = 0);

   std::vector<double>  get_histo_fromt0_minus_bckgrd_vector(int histo_num,
                                                          int lower_bckgdr,
                                                          int higher_bckgdr,
                                                          int binning,
                                                          int offset = 0);

   double         *get_histo_goodBins_minus_bckgrd_array(int histo_num,
                                                          int lower_bckgrd,
                                                          int higher_bckgrd,
                                                          int binning);

   std::vector<double>   get_histo_goodBins_minus_bckgrd_vector(int histo_num,
                                                           int lower_bckgrd,
                                                           int higher_bckgrd,
                                                           int binning);

   double         *get_asymmetry_array(int histo_num_plus,
                                         int histo_num_minus,
                                         double alpha_param,
                                         int binning,
                                         int lower_bckgrd_plus,
                                         int higher_bckgrd_plus,
                                         int lower_bckgrd_minus,
                                         int higher_bckgrd_minus,
                                         int offset = 0,
                                         double y_offset = 0.);

   std::vector<double>  get_asymmetry_vector(int histo_num_plus,
                                          int histo_num_minus,
                                          double alpha_param,
                                          int binning,
                                          int lower_bckgrd_plus,
                                          int higher_bckgrd_plus,
                                          int lower_bckgrd_minus,
                                          int higher_bckgrd_minus,
                                          int offset = 0,
                                          double y_offset = 0.);

   double          *get_error_asymmetry_array(int histo_num_plus,
                                                int histo_num_minus,
                                                double alpha_param,
                                                int binning,
                                                int lower_bckgrd_plus,
                                                int higher_bckgrd_plus,
                                                int lower_bckgrd_minus,
                                                int higher_bckgrd_minus,
                                                int offset = 0);

   std::vector<double>  get_error_asymmetry_vector(int histo_num_plus,
                                                int histo_num_minus,
                                                double alpha_param,
                                                int binning,
                                                int lower_bckgrd_plus,
                                                int higher_bckgrd_plus,
                                                int lower_bckgrd_minus,
                                                int higher_bckgrd_minus,
                                                int offset = 0);

   double         *get_asymmetry_goodBins_array(int histo_num_plus,
                                                  int histo_num_minus,
                                                  double alpha_param,
                                                  int binning,
                                                  int lower_bckgrd_plus,
                                                  int higher_bckgrd_plus,
                                                  int lower_bckgrd_minus,
                                                  int higher_bckgrd_minus);

   std::vector<double> get_asymmetry_goodBins_vector(int histo_num_plus,
                                                  int histo_num_minus,
                                                  double alpha_param,
                                                  int binning,
                                                  int lower_bckgrd_plus,
                                                  int higher_bckgrd_plus,
                                                  int lower_bckgrd_minus,
                                                  int higher_bckgrd_minus);

    double        *get_error_asymmetry_goodBins_array(int histo_num_plus,
                                                       int histo_num_minus,
                                                       double alpha_param,
                                                       int binning,
                                                       int lower_bckgrd_plus,
                                                       int higher_bckgrd_plus,
                                                       int lower_bckgrd_minus,
                                                       int higher_bckgrd_minus);

   std::vector<double> get_error_asymmetry_goodBins_vector(int histo_num_plus,
                                                        int histo_num_minus,
                                                        double alpha_param,
                                                        int binning,
                                                        int lower_bckgrd_plus,
                                                        int higher_bckgrd_plus,
                                                        int lower_bckgrd_minus,
                                                        int higher_bckgrd_minus);


    double          get_binWidth_ps();
    void            put_binWidth_ps(double binWidth);
    double          get_binWidth_ns();
    void            put_binWidth_ns(double binWidth);
    double          get_binWidth_us();
    void            put_binWidth_us(double binWidth);

    int             get_histoLength_bin();
    void            put_histoLength_bin(int val) { length_histo = val; }

    int             get_numberHisto_int();
    void            put_numberHisto_int(int val) { number_histo = val; }

    std::string     get_nameHisto(int i);
    int             put_nameHisto(std::string histoName, int i);
    std::vector<std::string> get_histoNames_vector();
    int             put_histoNames_vector(std::vector<std::string> &histoNames);

    long            get_eventsHisto_long(int i);
    std::vector<long> get_eventsHisto_vector();

    long            get_totalEvents_long();

    int             get_numberScaler_int();
    int             put_numberScaler_int(int val);
    std::vector<long> get_scalers_vector();
    int             put_scalers_vector(std::vector<int> scalerData);
    std::vector<std::string> get_scalersNames_vector();
    int             put_scalersNames_vector(std::vector<std::string> scalersName);

    int             get_default_binning();
    int             get_t0_int(int i);
    int             put_t0_int(int histoNo, int t0);
    std::vector<int> get_t0_vector();
    int             put_t0_vector(std::vector<int> &t0Data);
    double          get_t0_double(int i);

    int             get_max_t0_int ();
    int             get_max_2_t0_int (int k, int j);
    int             get_min_t0_int ();
    int             get_min_2_t0_int (int k, int j);

    int             get_firstGood_int(int i);
    std::vector<int> get_firstGood_vector();
    int             put_firstGood_int(int i, int j);

    int             get_lastGood_int(int i);
    std::vector<int> get_lastGood_vector();
    int             put_lastGood_int(int i, int j);

    int             get_max_lastGood_int ();
    int             get_max_2_lastGood_int (int k, int j);
    int             get_min_lastGood_int ();
    int             get_min_2_lastGood_int (int k, int j);

    int             get_runNumber_int();
    int             put_runNumber_int(int i);

    std::string     get_sample();
    int             put_sample(std::string sample);
    std::string     get_field();
    int             put_field(std::string field);
    std::string     get_orient();
    int             put_orient(std::string orientation);
    std::string     get_temp();
    int             put_temp(std::string temp);
    std::string     get_setup();
    int             put_setup(std::string setup);
    std::string     get_comment();
    int             put_comment(std::string comment);

    std::vector<std::string>  get_timeStart_vector();
    int             put_timeStart_vector(std::vector<std::string> timeStart);
    std::vector<std::string>  get_timeStop_vector();
    int             put_timeStop_vector(std::vector<std::string> timeStop);

    int             get_numberTemperature_int();
    int             put_numberTemperature_int(int noOfTemps);
    std::vector<double>  get_temperatures_vector();
    int             put_temperatures_vector(std::vector<double> &temps);
    std::vector<double>  get_devTemperatures_vector();
    int             put_devTemperatures_vector(std::vector<double> &devTemps);

  private:

    int tmax(int x, int y);
    int tmin(int x, int y);

} ;
#endif
/************************************************************************************
 * EOF MuSR_td_PSI_bin.h                                                         *
 ************************************************************************************/
