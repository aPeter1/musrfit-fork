/**************************************************************************************

  MuSR_td_PSI_bin.h

  declaration file of the class 'MuSR_td_PSI_bin'

  Main class to read td_bin PSI MuSR data.

***************************************************************************************

    begin                : Alex Amato, October 2005
    modfied:             :
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
using namespace std ;
#include <fstream>
#include <cstring>
#include <cmath>
#include <string>
#include <vector>

#include "tydefs.h"

class MuSR_td_PSI_bin {

  public:
    MuSR_td_PSI_bin();
   ~MuSR_td_PSI_bin();

  private:
// ------------------------------------start of the variables
    char     format_id[3] ;

    Int16    num_run ;

    char     sample[11] ;
    char     temp[11] ;
    char     field[11] ;
    char     orient[11] ;
    char     comment[63] ;

    char     date_start[10] ;
    char     date_stop[10] ;
    char     time_start[9] ;
    char     time_stop[9] ;

    Float32  bin_width ;
    Int16    tdc_resolution ;
    Int16    tdc_overflow  ;

    Int16    number_histo ;
    Int16    length_histo ;
    char     labels_histo[16][5] ;

    Int32    total_events ;
    Int32    events_per_histo[16] ;

    Int16    integer_t0[16] ;
    Int16    first_good[16] ;
    Int16    last_good[16] ;
    Float32  real_t0[17] ;

    Int32    scalers[18] ;
    char     labels_scalers[18][5] ;

    Float32  temper[4] ;
    Float32  temp_deviation[4] ;
    Float32  mon_low[4] ;
    Float32  mon_high[4] ;
    Int32    mon_num_events ;
    char     mon_dev[13] ;

    Int16    num_data_records_file ;
    Int16    length_data_records_bins ;
    Int16    num_data_records_histo ;

    Int32    period_save ;
    Int32    period_mon ;
    Int32    **histo ;

  public:

    vector< vector<double> >  histos_vector ; /*!< this public variables provides a direct access to the histograms
                                               */

// ------------------------------------end of the variables

  public:

    int            read(const char* fileName);

    int            *get_histo_array_int(int histo_num);
    double         *get_histo_array(int histo_num , int binning) ;
    vector<double>  get_histo_vector(int histo_num , int binning) ;
    vector<double>  get_histo_vector_no0(int histo_num , int binning) ;

    double         *get_histo_fromt0_array(int histo_num ,
                                           int binning ,
                                           int offset = 0) ;

    vector<double>  get_histo_fromt0_vector(int histo_num ,
                                            int binning ,
                                            int offset = 0) ;

    double         *get_histo_goodBins_array(int histo_num , int binning) ;

    vector<double>  get_histo_goodBins_vector(int histo_num , int binning) ;

    double         *get_histo_fromt0_minus_bckgrd_array(int histo_num ,
                                                         int lower_bckgdr ,
                                                         int higher_bckgdr ,
                                                         int binning ,
                                                         int offset = 0) ;

    vector<double>  get_histo_fromt0_minus_bckgrd_vector(int histo_num ,
                                                          int lower_bckgdr ,
	                                                      int higher_bckgdr ,
                                                          int binning ,
                                                          int offset = 0) ;

    double         *get_histo_goodBins_minus_bckgrd_array(int histo_num ,
                                                          int lower_bckgrd ,
	                                                      int higher_bckgrd ,
                                                          int binning) ;

   vector<double>   get_histo_goodBins_minus_bckgrd_vector(int histo_num ,
                                                           int lower_bckgrd ,
                                                           int higher_bckgrd ,
                                                           int binning) ;

    double         *get_asymmetry_array(int histo_num_plus,
                                         int histo_num_minus,
                                         double alpha_param,
                                         int binning,
                                         int lower_bckgrd_plus,
                                         int higher_bckgrd_plus,
                                         int lower_bckgrd_minus,
                                         int higher_bckgrd_minus ,
                                         int offset = 0,
                                         double y_offset = 0.) ;

    vector<double>  get_asymmetry_vector(int histo_num_plus ,
                                          int histo_num_minus ,
                                          double alpha_param ,
                                          int binning ,
                                          int lower_bckgrd_plus ,
                                          int higher_bckgrd_plus ,
                                          int lower_bckgrd_minus ,
                                          int higher_bckgrd_minus ,
                                          int offset = 0 ,
                                          double y_offset = 0.) ;

    double          *get_error_asymmetry_array(int histo_num_plus ,
                                                int histo_num_minus ,
                                                double alpha_param ,
                                                int binning ,
                                                int lower_bckgrd_plus ,
                                                int higher_bckgrd_plus ,
                                                int lower_bckgrd_minus ,
                                                int higher_bckgrd_minus ,
                                                int offset = 0) ;

    vector<double>  get_error_asymmetry_vector(int histo_num_plus ,
                                                int histo_num_minus ,
                                                double alpha_param ,
                                                int binning ,
                                                int lower_bckgrd_plus ,
                                                int higher_bckgrd_plus ,
                                                int lower_bckgrd_minus ,
                                                int higher_bckgrd_minus ,
                                                int offset = 0) ;

    double         *get_asymmetry_goodBins_array(int histo_num_plus ,
                                                  int histo_num_minus ,
                                                  double alpha_param ,
                                                  int binning ,
                                                  int lower_bckgrd_plus ,
                                                  int higher_bckgrd_plus ,
                                                  int lower_bckgrd_minus ,
                                                  int higher_bckgrd_minus) ;

    vector<double> get_asymmetry_goodBins_vector(int histo_num_plus ,
                                                  int histo_num_minus ,
                                                  double alpha_param ,
                                                  int binning ,
                                                  int lower_bckgrd_plus ,
                                                  int higher_bckgrd_plus ,
                                                  int lower_bckgrd_minus ,
                                                  int higher_bckgrd_minus) ;

    double        *get_error_asymmetry_goodBins_array(int histo_num_plus ,
                                                       int histo_num_minus ,
                                                       double alpha_param ,
                                                       int binning ,
                                                       int lower_bckgrd_plus ,
                                                       int higher_bckgrd_plus ,
                                                       int lower_bckgrd_minus ,
                                                       int higher_bckgrd_minus) ;

    vector<double> get_error_asymmetry_goodBins_vector(int histo_num_plus ,
                                                        int histo_num_minus ,
                                                        double alpha_param ,
                                                        int binning ,
                                                        int lower_bckgrd_plus ,
                                                        int higher_bckgrd_plus ,
                                                        int lower_bckgrd_minus ,
                                                        int higher_bckgrd_minus) ;


    double          get_binWidth_ps();
    double          get_binWidth_ns();
    double          get_binWidth_us();

    int             get_histoLength_bin();

    int             get_numberHisto_int();

    string          get_nameHisto(int i) ;
    vector<string>  get_histoNames_vector();

    long            get_eventsHisto_long(int i);
    vector<long>    get_eventsHisto_vector();

    long            get_totalEvents_long();

    vector<long>    get_scalers_vector() ;
    vector<string>  get_scalersNames_vector() ;

    int             get_t0_int(int i) ;
    vector<int>     get_t0_vector() ;
    double          get_t0_double(int i) ;

    int             get_max_t0_int () ;
    int             get_max_2_t0_int (int k, int j) ;
    int             get_min_t0_int () ;
    int             get_min_2_t0_int (int k, int j) ;

    int             get_firstGood_int(int i) ;
    vector<int>     get_firstGood_vector() ;
    int             put_firstGood_int(int i, int j) ;

    int             get_lastGood_int(int i) ;
    vector<int>     get_lastGood_vector() ;
    int             put_lastGood_int(int i, int j) ;

    int             get_max_lastGood_int () ;
    int             get_max_2_lastGood_int (int k, int j) ;
    int             get_min_lastGood_int () ;
    int             get_min_2_lastGood_int (int k, int j) ;

    int             get_runNumber_int() ;
    int             put_runNumber_int(int i) ;

    string          get_sample() ;
    string          get_field() ;
    string          get_orient() ;
    string          get_temp() ;
    string          get_comment() ;

    vector<string>  get_timeStart_vector() ;
    vector<string>  get_timeStop_vector() ;

    vector<double>  get_devTemperatures_vector() ;
    vector<double>  get_temperatures_vector() ;

  private:

	int max(int x, int y) ;
    int min(int x, int y) ;

} ;
#endif
