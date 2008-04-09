/********************************************************************************************

  MuSR_td_PSI_bin.cpp

  implementation of the class 'MuSR_td_PSI_bin'

  Main class to read td_bin PSI MuSR data.

*********************************************************************************************

    begin                : Alex Amato, October 2005
    modfied:             :
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
using namespace std ;
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

    number_histo = 0;
    histo = NULL;
  }


//*******************************
//Implementation desructor
//*******************************

/*! \brief Simple Destructor clearing some pointers and variables
 */

 MuSR_td_PSI_bin::~MuSR_td_PSI_bin()
  {

//    if (histo != NULL)
//      {
//        for (int i=0; i < number_histo; i++)
//          if (*(histo+i) != NULL) delete[] *(histo+i);
//        delete [] histo;
//      }

    number_histo = 0;
  }


//*******************************
//Implementation read
//*******************************

/*! \brief Method to read a PSI-bin file
 *
 *  This method gives back:
 *    - 0 for succesful reading
 *    - 1 if the open file action or the reading of the header failed
 *    - 2 for an unsupported version of the data
 *    - 3 for an error when allocating data buffer
 *    - 4 if number of histograms/record not equals 1\n
 *
 *  The parameter of the method is a const char * representing the name of the file to be open.
 */

 int MuSR_td_PSI_bin::read(const char * fileName)
  {
    ifstream  file_name ;
    Int16     *dum_Int16 ;
    Int32     *dum_Int32 ;
    Float32   *dum_Float32 ;
    Int32     i ;

    if (histo != NULL)
    {
      for (int i=0; i < number_histo; i++)
        if (*(histo+i) != NULL) delete[] *(histo+i);
      delete [] histo;
    }
    number_histo = 0;

    file_name.open(fileName, ios_base::binary);  // open PSI bin file
      if (file_name.fail()) return 1;            // ERROR open failed

    char *buffer_file = new char[1024] ;
    if (!buffer_file)  return 3;                 // ERROR allocating data buffer

    file_name.read(buffer_file, 1024) ;          // read header into buffer
    if (file_name.fail())
		{
      file_name.close();
      delete [] buffer_file;
      return 1;                                  // ERROR reading header failed
		}
                                                 // fill header data into member variables
    strncpy(format_id,buffer_file,2);
    format_id[2] = '\0' ;
    if (strcmp(format_id,"1N") == 1)
		{
      file_name.close();
      delete [] buffer_file;
      return 2 ;                                 // ERROR unsupported version
		}

                      dum_Int16 = (Int16 *) &buffer_file[2] ;
    tdc_resolution = *dum_Int16 ;
                      dum_Int16 = (Int16 *) &buffer_file[4] ;
    tdc_overflow   = *dum_Int16 ;
                      dum_Int16 = (Int16 *) &buffer_file[6] ;
    num_run        = *dum_Int16 ;
                      dum_Int16 = (Int16 *) &buffer_file[28] ;
    length_histo   = *dum_Int16 ;
                      dum_Int16 = (Int16 *) &buffer_file[30] ;
    number_histo   = *dum_Int16 ;
    strncpy(sample,buffer_file+138,10) ;
            sample[10] = '\0' ;
    strncpy(temp,buffer_file+148,10) ;
            temp[10] = '\0' ;
    strncpy(field,buffer_file+158,10) ;
            field[10] = '\0' ;
    strncpy(orient,buffer_file+168,10) ;
            orient[10] = '\0' ;
    strncpy(comment,buffer_file+860,62) ;
            comment[62] = '\0' ;
    strncpy(date_start,buffer_file+218,9) ;
            date_start[9] = '\0' ;
    strncpy(date_stop,buffer_file+227,9) ;
            date_stop[9] = '\0' ;
    strncpy(time_start,buffer_file+236,8) ;
            time_start[8] = '\0' ;
    strncpy(time_stop,buffer_file+244,8) ;
            time_stop[8] = '\0' ;

                      dum_Int32 = (Int32 *) &buffer_file[424] ;
    total_events   = *dum_Int32 ;

    for (i=0; i<=15; i++)
    {
      strncpy(labels_histo[i],buffer_file+948+i*4,4) ;
              labels_histo[i][4] = '\0' ;
                             dum_Int32 = (Int32 *) &buffer_file[296+i*4] ;
      events_per_histo[i] = *dum_Int32 ;
                             dum_Int16 = (Int16 *) &buffer_file[458+i*2] ;
      integer_t0[i]       = *dum_Int16 ;
                             dum_Int16 = (Int16 *) &buffer_file[490+i*2] ;
      first_good[i]       = *dum_Int16 ;
                             dum_Int16 = (Int16 *) &buffer_file[522+i*2] ;
      last_good[i]        = *dum_Int16 ;
    }

    for (i=0; i<=16; i++)
    {
                             dum_Float32 = (Float32 *) &buffer_file[792+i*4] ;
      real_t0[i]          = *dum_Float32 ;
    }

    for (i=0; i<=5; i++)
    {
                             dum_Int32 = (Int32 *) &buffer_file[670+i*4] ;
      scalers[i]          = *dum_Int32 ;
      strncpy(labels_scalers[i],buffer_file+924+i*4,4) ;
              labels_scalers[i][4] = '\0' ;
    }

    for (i=6; i<=17; i++)
    {
                             dum_Int32 = (Int32 *) &buffer_file[360+(i-6)*4] ;
      scalers[i]          = *dum_Int32 ;
      strncpy(labels_scalers[i],buffer_file+554+(i-6)*4,4) ;
              labels_scalers[i][4] = '\0' ;
    }

                             dum_Float32 = (Float32 *) &buffer_file[1012] ;
    bin_width             = *dum_Float32 ;

    if (bin_width == 0.)
    {
      bin_width=(625.E-6)/8.*pow(Float32(2.),Float32(tdc_resolution)) ;
    }

    for (i=0; i<=3; i++)
    {
                             dum_Float32 = (Float32 *) &buffer_file[716+i*4] ;
      temper[i]           = *dum_Float32 ;
                             dum_Float32 = (Float32 *) &buffer_file[738+i*4] ;
      temp_deviation[i]   = *dum_Float32 ;
                             dum_Float32 = (Float32 *) &buffer_file[72+i*4] ;
      mon_low[i]          = *dum_Float32 ;
                             dum_Float32 = (Float32 *) &buffer_file[88+i*4] ;
      mon_high[i]         = *dum_Float32 ;
    }

                             dum_Int32 = (Int32 *) &buffer_file[712] ;
    mon_num_events        = *dum_Int32 ;
    strncpy(mon_dev,buffer_file+60,12) ;
            mon_dev[12] = '\0' ;

                                dum_Int16 = (Int16 *) &buffer_file[128] ; // numdaf
    num_data_records_file    = *dum_Int16 ;
                                dum_Int16 = (Int16 *) &buffer_file[130] ; // lendaf
    length_data_records_bins = *dum_Int16 ;
                                dum_Int16 = (Int16 *) &buffer_file[132] ; // kdafhi
    num_data_records_histo   = *dum_Int16 ;
                                dum_Int16 = (Int16 *) &buffer_file[134] ; // khidaf

    if (*dum_Int16 != 1)
		{
      cout << "ERROR number of histograms/record not equals 1!"
           << " Required algorithm is not implemented!" << endl;
      delete [] buffer_file ;
      file_name.close();
      return 4;                                // ERROR algorithm not implemented
    }

                                dum_Int32 = (Int32 *) &buffer_file[654] ;
    period_save              = *dum_Int32 ;
                                dum_Int32 = (Int32 *) &buffer_file[658] ;
    period_mon               = *dum_Int32 ;

    if (buffer_file) delete [] buffer_file ;

    // allocate histograms
    histo = new Int32* [int(number_histo)];

    for (Int32 i=0; i<number_histo; i++)
    {
      histo[i] = new Int32 [length_histo];
    }

    char *buffer_file_histo = new char[Int32(num_data_records_file)
                                      *Int32(length_data_records_bins)*4];
    if (!buffer_file_histo) {
      file_name.close();
      return 5;                                // ERROR allocating histogram buffer
    }
    file_name.seekg(1024, ios_base::beg) ;     // beginning of histogram data

    file_name.read(buffer_file_histo, Int32(num_data_records_file)
                                     *Int32(length_data_records_bins)*4) ;
    if (file_name.fail()) {
      delete [] buffer_file_histo;
      file_name.close();
      return 6;                                // ERROR reading data failed
    }

    vector<double> dummy_vector ;
    for (i=0; i<number_histo; i++)
    {
      dummy_vector.clear() ;
      for (Int32 j=0; j<length_histo; j++)
      {
        dum_Int32 = (Int32 *) &buffer_file_histo[i*4*Int32(num_data_records_histo)
                                                    *Int32(length_data_records_bins)
                                                +j*4];
        histo[i][j]= *dum_Int32 ;
        dummy_vector.push_back(double(histo[i][j])) ;
      }
      histos_vector.push_back(dummy_vector) ;
    }


/*
    for (Int32 j=0;i=0; i<Int32(number_histo); i++)   // loop over histograms
    {
      for (Int32 k=0; k< Int32(num_data_records_histo); k++) // loop over records
      {
        for (Int32 l=0; l < Int32(length_data_records_bins); l++) // loop
        {
          dum_Int32 = (Int32 *) &buffer_file_histo[i*4*Int32(num_data_records_histo)
                                                      *Int32(length_data_records_bins)
                                                  +k*4*Int32(length_data_records_bins)
                                                  +l*4];
          if (j<length_histo) histo[i][j++]= *dum_Int32;
        }
      }
    }
*/
    file_name.close();

    if (buffer_file_histo) delete [] buffer_file_histo;

    return 0;
  }


//*******************************
//Implementation get_histo_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the histogram \<histo_num\> with binning \<binning\>
 *
 *  This method gives back:
 *    - an pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 double * MuSR_td_PSI_bin::get_histo_array(int histo_num , int binning)
  {
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return NULL ;
    double *histo_array = new double[int(int(length_histo)/binning)] ;
    for (int i = 0 ; i < int(int(length_histo)/binning) ; i++)
    {
      histo_array[i] = 0 ;
      for (int j = 0 ; j < binning ; j++)
      histo_array[i] += double(histo[histo_num][i*binning+j]) ;
    }
    return histo_array ;
  }


//*******************************
//Implementation get_histo_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\> with binning \<binning\>
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_vector(int histo_num , int binning)
  {
    vector<double> histo_vector ; //(int(length_histo/binning))
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return histo_vector ;
    for (int i = 0 ; i < int(length_histo/binning) ; i++)
      histo_vector.push_back(0.) ;
    for (int i = 0 ; i < int(length_histo/binning) ; i++)
    {
      for (int j = 0 ; j < binning ; j++)
      histo_vector[i] += double(histo[histo_num][i*binning+j]) ;
    }
    return histo_vector ;
  }


//*******************************
//Implementation get_histo_vector_no0
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\> with binning \<binning\> but wwhere the bins with zero counts are replaced by a count 0.1
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_vector_no0(int histo_num , int binning)
  {
    vector<double> histo_vector; //(int(length_histo/binning)) ;
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return histo_vector ;
    for (int i = 0 ; i < int(length_histo/binning) ; i++)
      histo_vector.push_back(0.) ;
    for (int i = 0 ; i < int(length_histo/binning) ; i++)
    {
      for (int j = 0 ; j < binning ; j++)
      histo_vector[i] += double(histo[histo_num][i*binning+j]) ;
      if (histo_vector[i] < 0.5 )
      {
        histo_vector[i] = 0.1 ;
      }
    }
    return histo_vector ;
  }


//**********************************
//Implementation get_histo_array_int
//**********************************

/*! \brief Method to obtain an array of type integer containing the values of the histogram \<histo_num\>
 *
 *  This method gives back:
 *    - an pointer of a integer array
 *    - the NULL pointer if an invalid histogram number is choosen
 *
 *  The parameter of the method is the integer \<histo_num\> representing the desired histogram number.
 */

  int * MuSR_td_PSI_bin::get_histo_array_int(int histo_num)
  {
    if ( histo_num < 0 || histo_num >= int(number_histo))
      return NULL ;
    int *histo_array = new int[length_histo] ;
    for (int i = 0 ; i < int(length_histo) ; i++)
    {
      histo_array[i] = int(histo[histo_num][i]) ;
    }
    return histo_array ;
  }


//*******************************
//Implementation get_histo_fromt0_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point t0. An \<offset\> can also be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - an pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 double * MuSR_td_PSI_bin::get_histo_fromt0_array(int histo_num , int binning , int offset)
  {
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return NULL ;
    double *histo_fromt0_array = new double[int((int(length_histo)-get_t0_int(histo_num)-offset+1)/binning)] ;
    for (int i = 0 ; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning) ; i++)
    {
      histo_fromt0_array[i] = 0 ;
      for (int j = 0 ; j < binning ; j++)
      histo_fromt0_array[i] += double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]) ;
    }
    return histo_fromt0_array ;
  }


//*******************************
//Implementation get_histo_fromt0_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point t0.  An \<offset\> can also be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_fromt0_vector(int histo_num , int binning , int offset)
  {
    vector<double> histo_fromt0_vector ; // (int((int(length_histo)-get_t0_int(histo_num)+1)/binning)) ;
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return histo_fromt0_vector ;
    for (int i = 0 ; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning) ; i++)
      histo_fromt0_vector.push_back(0.) ;
    for (int i = 0 ; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning) ; i++)
    {
      for (int j = 0 ; j < binning ; j++)
      histo_fromt0_vector[i] += double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]) ;
    }
    return histo_fromt0_vector ;
  }


//*******************************
//Implementation get_histo_goodBins_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point first_good until last_good
 *
 *  This method gives back:
 *    - an pointer of a double array
 *    - the NULL pointer if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 double * MuSR_td_PSI_bin::get_histo_goodBins_array(int histo_num , int binning)
  {
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return NULL ;
    double *histo_goodBins_array = new double[int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num)+1)/binning)] ;
    for (int i = 0 ; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning) ; i++)
    {
      histo_goodBins_array[i] = 0 ;
      for (int j = 0 ; j < binning ; j++)
      histo_goodBins_array[i] += double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]) ;
    }
    return histo_goodBins_array ;
  }


//*******************************
//Implementation get_histo_goodBins_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point first_good until last_good
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if an invalid histogram number or binning is choosen
 *
 *  The parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_goodBins_vector(int histo_num , int binning)
  {
    vector<double> histo_goodBins_vector ;
    if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return histo_goodBins_vector ;
    for (int i = 0 ; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning) ; i++)
      histo_goodBins_vector.push_back(0.) ;
    for (int i = 0 ; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning) ; i++)
    {
      for (int j = 0 ; j < binning ; j++)
      histo_goodBins_vector[i] += double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]) ;
    }
    return histo_goodBins_vector ;
  }


//*******************************
//Implementation get_histo_fromt0_minus_bckgrd_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point t0. A background calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> is subtracted. An \<offset\> can also be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - a pointer of a double array
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits between which the background is calculated.
 */

double * MuSR_td_PSI_bin::get_histo_fromt0_minus_bckgrd_array(int histo_num , int lower_bckgrd ,
	                                                             int higher_bckgrd , int binning, int offset)
  {
    double *histo_fromt0_minus_bckgrd_array = new double[int((int(length_histo)-get_t0_int(histo_num)-offset+1)/binning)] ;
	if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return NULL ;
	if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
      return NULL ;
	double bckgrd = 0 ;
	for (int k = lower_bckgrd ; k <= higher_bckgrd ; k++)
    {
	  bckgrd += double(histo[histo_num][k]) ;
	}
	bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1) ;
    for (int i = 0 ; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning) ; i++)
    {
      histo_fromt0_minus_bckgrd_array[i] = 0 ;
      for (int j = 0 ; j < binning ; j++)
      histo_fromt0_minus_bckgrd_array[i] +=
	  double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]) - bckgrd;
    }
    return histo_fromt0_minus_bckgrd_array ;
  } ;


//*******************************
//Implementation get_histo_fromt0_minus_bckgrd_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point t0. A background calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> is subtracted. An \<offset\> can also be specified (otherwise = 0).
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits between which the background is calculated.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_fromt0_minus_bckgrd_vector(int histo_num , int lower_bckgrd ,
	                                                             int higher_bckgrd , int binning, int offset)
  {
    vector<double> histo_fromt0_minus_bckgrd_vector ; // (int((int(length_histo)-get_t0_int(histo_num)+1)/binning)) ;
	if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return histo_fromt0_minus_bckgrd_vector ;
	if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
      return histo_fromt0_minus_bckgrd_vector ;
    double bckgrd = 0 ;
	for (int k = lower_bckgrd ; k <= higher_bckgrd ; k++)
    {
	  bckgrd += double(histo[histo_num][k]) ;
	}
	bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1) ;
    for (int i = 0 ; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning) ; i++)
      histo_fromt0_minus_bckgrd_vector.push_back(0.) ;
    for (int i = 0 ; i < int((int(length_histo)-get_t0_int(histo_num)-offset)/binning) ; i++)
    {
      for (int j = 0 ; j < binning ; j++)
      histo_fromt0_minus_bckgrd_vector[i] +=
	  double(histo[histo_num][i*binning+j+get_t0_int(histo_num)+offset]) - bckgrd;
    }
    return histo_fromt0_minus_bckgrd_vector ;
  }


//*******************************
//Implementation get_histo_goodBins_minus_bckgrd_array
//*******************************

/*! \brief Method to obtain an array of type double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point first_good until the point last_good. A background calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> is subtracted
 *
 *  This method gives back:
 *    - a pointer of a double array
 *    - the NULL pointer if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits between which the background is calculated.
 */

double * MuSR_td_PSI_bin::get_histo_goodBins_minus_bckgrd_array(int histo_num , int lower_bckgrd ,
	                                                             int higher_bckgrd , int binning)
  {
    double *histo_goodBins_minus_bckgrd_array = new double[int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num)+1)/binning)] ;
	if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return NULL ;
	if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
      return NULL ;
	double bckgrd = 0 ;
	for (int k = lower_bckgrd ; k <= higher_bckgrd ; k++)
    {
	  bckgrd += double(histo[histo_num][k]) ;
	}
	bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1) ;
    for (int i = 0 ; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning) ; i++)
    {
      histo_goodBins_minus_bckgrd_array[i] = 0 ;
      for (int j = 0 ; j < binning ; j++)
      histo_goodBins_minus_bckgrd_array[i] +=
	  double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]) - bckgrd;
    }
    return histo_goodBins_minus_bckgrd_array ;
  } ;


//*******************************
//Implementation get_histo_goodBins_minus_bckgrd_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the histogram \<histo_num\> with binning \<binning\> from the point first_good until the point last_good. A background calculated from the points \<lower_bckgrd\> and \<higher_bckgrd\> is subtracted
 *
 *  This method gives back:
 *    - a vector of double
 *    - an empty vector of double if one provides
 *       - an invalid histogram number
 *       - a binning smaller than 1
 *       - invalid background limits
 *
 *  The first parameters of the method are the integers \<histo_num\> and \<binning\> representing the desired histogram number and binning.
 *  Also the parameters \<lower_bckgrd\> and \<higher_bckgrd\> define the limits between which the background is calculated.
 */

 vector<double> MuSR_td_PSI_bin::get_histo_goodBins_minus_bckgrd_vector(int histo_num , int lower_bckgrd ,
	                                                             int higher_bckgrd , int binning)
  {
    vector<double> histo_goodBins_minus_bckgrd_vector ;  ;
	if ( histo_num < 0 || histo_num >= int(number_histo) || binning <= 0 )
      return histo_goodBins_minus_bckgrd_vector ;
	if ( lower_bckgrd < 0 || higher_bckgrd >= int(length_histo) || lower_bckgrd > higher_bckgrd )
      return histo_goodBins_minus_bckgrd_vector ;
    double bckgrd = 0 ;
	for (int k = lower_bckgrd ; k <= higher_bckgrd ; k++)
    {
	  bckgrd += double(histo[histo_num][k]) ;
	}
	bckgrd = bckgrd/(higher_bckgrd-lower_bckgrd+1) ;
    for (int i = 0 ; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning) ; i++)
      histo_goodBins_minus_bckgrd_vector.push_back(0.) ;
    for (int i = 0 ; i < int((get_lastGood_int(histo_num)-get_firstGood_int(histo_num))/binning) ; i++)
    {
      for (int j = 0 ; j < binning ; j++)
      histo_goodBins_minus_bckgrd_vector[i] +=
	  double(histo[histo_num][i*binning+j+get_firstGood_int(histo_num)]) - bckgrd;
    }
    return histo_goodBins_minus_bckgrd_vector ;
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
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 double * MuSR_td_PSI_bin::get_asymmetry_array(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                int lower_bckgrd_minus , int higher_bckgrd_minus , int offset ,
                                                double y_offset)
  {
    int max_t0 = max(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus)) ;
    double *asymmetry_array = new double[int((int(length_histo)-max_t0-offset+1)/binning)] ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return NULL ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return NULL ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return NULL ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return NULL ;
	double *dummy_1 = new double ;
	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning , offset) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning , offset) ;
	for (int i = 0 ; i < int((int(length_histo)-max_t0)/binning) ; i++)
	{
	  asymmetry_array[i] = (dummy_1[i] - alpha_param * dummy_2[i]) /
                           (dummy_1[i] + alpha_param * dummy_2[i]) + y_offset ;
	}
	return asymmetry_array ;
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

 vector<double> MuSR_td_PSI_bin::get_asymmetry_vector(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                 int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                 int lower_bckgrd_minus , int higher_bckgrd_minus , int offset , double y_offset)
  {
    int max_t0 = max(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus)) ;
    vector<double> asymmetry_vector ; // (int((int(length_histo)-max_t0+1)/binning)) ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return asymmetry_vector ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return asymmetry_vector ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return asymmetry_vector ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return asymmetry_vector ;
   	double *dummy_1 = new double ;
	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning, offset) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning, offset) ;
	for (int i = 0 ; i < int((int(length_histo)-max_t0-offset)/binning) ; i++)
      asymmetry_vector.push_back(0.) ;
	for (int i = 0 ; i < int((int(length_histo)-max_t0-offset)/binning) ; i++)
	{
	  asymmetry_vector[i] = (dummy_1[i] - alpha_param * dummy_2[i]) /
                            (dummy_1[i] + alpha_param * dummy_2[i]) + y_offset ;
	}
	return asymmetry_vector ;
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
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 double * MuSR_td_PSI_bin::get_error_asymmetry_array(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                 int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                 int lower_bckgrd_minus , int higher_bckgrd_minus, int offset)
  {
    int max_t0 = max(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus)) ;
    double *error_asymmetry_array = new double[int((int(length_histo)-max_t0-offset+1)/binning)] ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return NULL ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return NULL ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return NULL ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return NULL ;
	double *dummy_1 = new double ;
	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning, offset) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning, offset) ;
	for (int i = 0 ; i < int((length_histo-max_t0-offset)/binning) ; i++)
	{
	  if (dummy_1[i] < 0.5 || dummy_2[i] < 0.5 )
      error_asymmetry_array[i] = 1.0 ;
	  else
	    error_asymmetry_array[i] = double(2.) * alpha_param * sqrt(dummy_1[i]*dummy_2[i]*(dummy_1[i]+dummy_2[i])) /
	                               pow(dummy_1[i] + alpha_param * dummy_2[i],2.) ;
	}
	return error_asymmetry_array ;
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

 vector<double> MuSR_td_PSI_bin::get_error_asymmetry_vector(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                 int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                 int lower_bckgrd_minus , int higher_bckgrd_minus, int offset)
  {
    int max_t0 = max(get_t0_int(histo_num_plus),get_t0_int(histo_num_minus)) ;
    vector<double> error_asymmetry_vector ; //(int((int(length_histo)-max_t0+1)/binning)) ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return error_asymmetry_vector ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return error_asymmetry_vector ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return error_asymmetry_vector ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return error_asymmetry_vector ;
	double *dummy_1 = new double ;
  	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning, offset) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning, offset) ;
	for (int i = 0 ; i < int((int(length_histo)-max_t0-offset)/binning) ; i++)
      error_asymmetry_vector.push_back(0.) ;
	for (int i = 0 ; i < int((int(length_histo-max_t0-offset))/binning) ; i++)
	{
	  if (dummy_1[i] < 0.5 || dummy_2[i] < 0.5 )
      error_asymmetry_vector[i] = 1.0 ;
	  else
	    error_asymmetry_vector[i] = double(2.) * alpha_param * sqrt(dummy_1[i]*dummy_2[i]*(dummy_1[i]+dummy_2[i])) /
		                            pow(dummy_1[i] + alpha_param * dummy_2[i],2.) ;
	}
	return error_asymmetry_vector ;
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
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms.are also required.
 */

 double * MuSR_td_PSI_bin::get_asymmetry_goodBins_array(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                int lower_bckgrd_minus , int higher_bckgrd_minus)
  {
    int hsize = int((min(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning) ;
    double *asymmetry_goodBins_array = new double[hsize] ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return NULL ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return NULL ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return NULL ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return NULL ;
	double *dummy_1 = new double ;
	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning) ;
	int hstart = max(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus)) ;
    for (int i = 0 ; i < hsize ; i++)
	{
	  asymmetry_goodBins_array[i] = (dummy_1[i+hstart] - alpha_param * dummy_2[i+hstart]) /
                                    (dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart]) ;
	}
	return asymmetry_goodBins_array ;
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

 vector<double> MuSR_td_PSI_bin::get_asymmetry_goodBins_vector(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                 int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                 int lower_bckgrd_minus , int higher_bckgrd_minus)
  {
    int hsize = int((min(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning) ;
    vector<double> asymmetry_goodBins_vector ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return asymmetry_goodBins_vector ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return asymmetry_goodBins_vector ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return asymmetry_goodBins_vector ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return asymmetry_goodBins_vector ;
   	double *dummy_1 = new double ;
	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning) ;
	for (int i = 0 ; i < hsize ; i++)
      asymmetry_goodBins_vector.push_back(0.) ;
	int hstart = max(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus)) ;
	for (int i = 0 ; i < hsize ; i++)
	{
	  asymmetry_goodBins_vector[i] = (dummy_1[i+hstart] - alpha_param * dummy_2[i+hstart]) /
                            (dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart]) ;
	}
	return asymmetry_goodBins_vector ;
cout << hsize << "  " << hstart << endl ;
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
 *
 *  The first parameters of the method are the integers \<histo_num_plus\> and \<histo_num_minus\>, as well as the double \<alpha_param\>.
 *  Integers for the binning and for the background limits for both histograms are also required.
 */

 double * MuSR_td_PSI_bin::get_error_asymmetry_goodBins_array(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                 int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                 int lower_bckgrd_minus , int higher_bckgrd_minus)
  {
    int hsize = int((min(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning) ;
    double *error_asymmetry_goodBins_array = new double[hsize] ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return NULL ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return NULL ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return NULL ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return NULL ;
	double *dummy_1 = new double ;
	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning) ;
	int hstart = max(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus)) ;
	for (int i = 0 ; i < hsize ; i++)
	{
	  if (dummy_1[i+hstart] < 0.5 || dummy_2[i+hstart] < 0.5 )
        error_asymmetry_goodBins_array[i] = 1.0 ;
	  else
	    error_asymmetry_goodBins_array[i] = double(2.) * alpha_param * sqrt(dummy_1[i+hstart]*dummy_2[i+hstart]*(dummy_1[i+hstart]+dummy_2[i+hstart])) /
	                                        pow(dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart],2.) ;
	}
	return error_asymmetry_goodBins_array ;
  }


//*******************************
//Implementation get_error_asymmetry_goodBins_vector
//*******************************

/*! \brief Method to obtain a vector of double containing the values of the error of the asymmetry between 2 histograms.
 *
 *  The size is calculated as the asymmetry array.
 *  The error of the asymmetry is calculated with the histograms \<histo_num_plus\> and \<histo_num_minus\> with an alpha parameter
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

 vector<double> MuSR_td_PSI_bin::get_error_asymmetry_goodBins_vector(int histo_num_plus , int histo_num_minus , double alpha_param ,
                                                 int binning , int lower_bckgrd_plus , int higher_bckgrd_plus ,
                                                 int lower_bckgrd_minus , int higher_bckgrd_minus)
  {
    int hsize = int((min(get_lastGood_int(histo_num_plus)-get_firstGood_int(histo_num_plus),get_lastGood_int(histo_num_minus)-get_firstGood_int(histo_num_minus))+1)/binning) ;
    vector<double> error_asymmetry_goodBins_vector ;
	if ( histo_num_plus < 0 || histo_num_plus >= int(number_histo) || binning <= 0 )
      return error_asymmetry_goodBins_vector ;
	if ( histo_num_minus < 0 || histo_num_minus >= int(number_histo) )
      return error_asymmetry_goodBins_vector ;
	if ( lower_bckgrd_plus < 0 || higher_bckgrd_plus >= int(length_histo) || lower_bckgrd_plus > higher_bckgrd_plus )
      return error_asymmetry_goodBins_vector ;
	if ( lower_bckgrd_minus < 0 || higher_bckgrd_minus >= int(length_histo) || lower_bckgrd_minus > higher_bckgrd_minus )
      return error_asymmetry_goodBins_vector ;
	double *dummy_1 = new double ;
  	dummy_1 = get_histo_fromt0_minus_bckgrd_array(histo_num_plus , lower_bckgrd_plus ,
                                                                   higher_bckgrd_plus , binning) ;
  	double *dummy_2 = new double ;
    dummy_2 = get_histo_fromt0_minus_bckgrd_array(histo_num_minus , lower_bckgrd_minus ,
                                                                    higher_bckgrd_minus , binning) ;
	for (int i = 0 ; i < hsize ; i++)
      error_asymmetry_goodBins_vector.push_back(0.) ;
	int hstart = max(get_firstGood_int(histo_num_plus)-get_t0_int(histo_num_plus),get_firstGood_int(histo_num_minus)-get_t0_int(histo_num_minus)) ;
	for (int i = 0 ; i < hsize ; i++)
	{
	  if (dummy_1[i+hstart] < 0.5 || dummy_2[i+hstart] < 0.5 )
      error_asymmetry_goodBins_vector[i] = 1.0 ;
	  else
	    error_asymmetry_goodBins_vector[i] = double(2.) * alpha_param * sqrt(dummy_1[i+hstart]*dummy_2[i+hstart]*(dummy_1[i+hstart]+dummy_2[i+hstart])) /
		                                     pow(dummy_1[i+hstart] + alpha_param * dummy_2[i+hstart],2.) ;
	}
	return error_asymmetry_goodBins_vector ;
  }


//*******************************
//Implementation get_scalers_vector
//*******************************

/*! \brief Method providing a vector of long containing the values of the scalers
 */

 vector<long> MuSR_td_PSI_bin::get_scalers_vector()
  {
    vector<long> scalers_vect(16) ;
	  for ( int i = 0 ; i < 16 ; i++ )
      scalers_vect[i] = long(scalers[i]) ;
    return scalers_vect ;
  }


//*******************************
//Implementation get_max_t0_int
//*******************************

/*! \brief Method to determine the maximum value of the t0 bins
 */

 int MuSR_td_PSI_bin::get_max_t0_int()
  {
    int max_t0 = 0 ;
    for (int i = 0 ; i < int(number_histo) ; i++)
    {
	    if (int(integer_t0[i]) > max_t0)
	      max_t0 = int(integer_t0[i]) ;
    }
    return max_t0 ;
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
      return -1 ;
    int max_t0 = int(integer_t0[j]) ;
 	  if (int(integer_t0[k]) >= max_t0)
	    max_t0 = int(integer_t0[k]) ;
    return max_t0 ;
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
      return -1 ;
    int min_t0 = int(integer_t0[j]) ;
 	if (int(integer_t0[k]) <= min_t0)
	    min_t0 = int(integer_t0[k]) ;
    return min_t0 ;
  }


//*******************************
//Implementation get_min_t0_int
//*******************************

/*! \brief Method to determine the minimum value of the t0 bins
 */

 int MuSR_td_PSI_bin::get_min_t0_int()
  {
    int min_t0 = int(length_histo) ;
    for (int i = 0 ; i < int(number_histo) ; i++)
    {
	if (int(integer_t0[i]) < min_t0)
	    min_t0 = int(integer_t0[i]) ;
    }
    return min_t0 ;
  }


 //*******************************
//Implementation get_binWidth_ps
//*******************************

/*! \brief Method returning a double representing the bin-width in picoseconds
 */

 double MuSR_td_PSI_bin::get_binWidth_ps()
  {
    return double((double)bin_width*(double)1000000.) ;
  }


//*******************************
//Implementation get_binWidth_ns
//*******************************

/*! \brief Method returning a double representing the bin-width in nanoseconds
 */

 double MuSR_td_PSI_bin::get_binWidth_ns()
  {
    return double((double)bin_width*(double)1000.) ;
  } ;

//*******************************
//Implementation get_binWidth_us
//*******************************

/*! \brief Method returning a double representing the bin-width in microseconds
 */

 double MuSR_td_PSI_bin::get_binWidth_us()
  {
    return double(bin_width) ;
  }


//*******************************
//Implementation get_histoLength_bin
//*******************************

/*! \brief Method returning an integer representing the histogram length in bins
 */

 int MuSR_td_PSI_bin::get_histoLength_bin()
  {
    return int(length_histo) ;
  }


//*******************************
//Implementation get_numberHisto_int
//*******************************

/*! \brief Method returning an integer representing the number of histograms
 */

 int MuSR_td_PSI_bin::get_numberHisto_int()
  {
    return int(number_histo) ;
  }


//*******************************
//Implementation get_totalEvents_long
//*******************************

/*! \brief Method returning a long representing the total number of events
 */

 long MuSR_td_PSI_bin::get_totalEvents_long()
  {
    return long(total_events) ;
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
      return -1 ;
    else
      return long(events_per_histo[i]) ;
  }


//*******************************
//Implementation get_eventsHisto_vector
//*******************************

/*! \brief Method returning a vector of long containing the number of events in the histograms
 */

  vector<long> MuSR_td_PSI_bin::get_eventsHisto_vector()
  {
    vector<long> eventsHisto(16) ;
	for ( int i = 0 ; i < 16 ; i++ )
      eventsHisto[i] = long(events_per_histo[i]) ;
    return eventsHisto ;
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
      return -1. ;
    else
      return double(real_t0[i]) ;
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
      return -1 ;
    else
      return int(integer_t0[i]) ;
  }

//*******************************
//Implementation get_t0_vector
//*******************************

/*! \brief Method returning a vector of integer containing the t0 values of the histograms specified in the header
 */

 vector<int> MuSR_td_PSI_bin::get_t0_vector()
  {
    vector<int> t0(number_histo) ;
	  for ( int i = 0 ; i < int(number_histo) ; i++ )
        t0[i] = int(integer_t0[i]) ;
    return t0 ;
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
      return -1 ;
    else
      return int(first_good[i]) ;
  }


//*******************************
//Implementation get_firstGood_vector
//*******************************

/*! \brief Method returning a vector of integer containing the first good bin values of the histograms specified in the header
 */

 vector<int> MuSR_td_PSI_bin::get_firstGood_vector()
  {
    vector<int> firstGood(number_histo) ;
	for ( int i = 0 ; i < number_histo ; i++ )
      firstGood[i] = int(first_good[i]) ;
    return firstGood ;
  }


//*******************************
//Implementation put_firstGood_int
//*******************************

/*! \brief Method to modify the first good bin (value \<j\>) of the histogram \<i\>
 *
 *  returns -1 if the histogram specified was invalid
 */

 int MuSR_td_PSI_bin::put_firstGood_int(int i, int j)
  {
    if ( i < 0 || i >= int(number_histo))
      return -1 ;
    else
      {
        first_good[i] = Int16 (j) ;
	    return  0;
      }
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
      return -1 ;
    else
      return int(last_good[i]) ;
  }


//*******************************
//Implementation get_lastGood_vector
//*******************************

/*! \brief Method returning a vector of integer containing the last good bin values of the histograms specified in the header
 */

 vector<int> MuSR_td_PSI_bin::get_lastGood_vector()
  {
    vector<int> lastGood(number_histo) ;
	for ( int i = 0 ; i < number_histo ; i++ )
      lastGood[i] = int(last_good[i]) ;
    return lastGood ;
  }


//*******************************
//Implementation get_max_lastGoog_int
//*******************************

/*! \brief Method returning an integer containing the maximum value of the "last good bins" of all histograms
 */

 int MuSR_td_PSI_bin::get_max_lastGood_int()
  {
    int max_lastGood = 0 ;
    for (int i = 0 ; i < int(number_histo) ; i++)
    {
	if (int(last_good[i]) > max_lastGood)
	    max_lastGood = int(last_good[i]) ;
    }
    return max_lastGood ;
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
      return -1 ;
	else
	{
	  int max_lastGood = int(last_good[j]) ;
	  if (int(last_good[k]) > max_lastGood)
	    max_lastGood = int(last_good[k]) ;
      return max_lastGood ;
    }
  }


//*******************************
//Implementation get_min_lastGoog_int
//*******************************

/*! \brief Method providing the minimum value of the last good bins
 */

 int MuSR_td_PSI_bin::get_min_lastGood_int()
  {
    int min_lastGood = int(last_good[0]) ;
    for (int i = 1 ; i < int(number_histo) ; i++)
    {
	if (int(last_good[i]) < min_lastGood)
	    min_lastGood = int(last_good[i]) ;
    }
    return min_lastGood ;
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
      return -1 ;
	else
	{ int min_lastGood = int(last_good[j]) ;
	  if (int(last_good[k]) < min_lastGood)
	    min_lastGood = int(last_good[k]) ;
      return min_lastGood ;
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
  if ( i < 0 || i >= int(number_histo))
    return -1 ;
    else
      {
        last_good[i] = Int16 (j) ;
	      return  0;
      }
  }


//*******************************
//Implementation get_runNumber_int
//*******************************

/*! \brief Method returning an integer containing the run number
 */

 int MuSR_td_PSI_bin::get_runNumber_int()
  {
    return int(num_run) ;
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
      return -1 ;
    num_run = short(i) ;
    return 0 ;
  }


//*******************************
//Implementation get_sample()
//*******************************

/*! \brief Method returning a string containing the sample name
 */

 string MuSR_td_PSI_bin::get_sample()
  {
    string strData ;
    strData = sample ;
    return strData ;
  }


//*******************************
//Implementation get_temp()
//*******************************

/*! \brief Method returning a string containing the temperature specified in the title
 */

 string MuSR_td_PSI_bin::get_temp()
  {
    string strData ;
    strData = temp ;
    return strData ;
  }


//*******************************
//Implementation get_orient()
//*******************************

/*! \brief Method returning a string containing the orientation specified in the title
 */

 string MuSR_td_PSI_bin::get_orient()
  {
    string strData ;
    strData = orient ;
    return strData ; ;
  }


//*******************************
//Implementation get_field()
//*******************************

/*! \brief Method returning a string containing the field specified in the title
 */

 string MuSR_td_PSI_bin::get_field()
  {
    string strData ;
    strData = field ;
    return strData ;
  }


//*******************************
//Implementation get_comments()
//*******************************

/*! \brief Method returning a string containing the comment specified in the title
 */

 string MuSR_td_PSI_bin::get_comment()
  {
    string strData ;
    strData = comment ;
    return strData ;
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
    string strData ;
    if (i < 0 || i >= int(number_histo))
      return NULL ;
    else
    {
      strData = labels_histo[i] ;
      return strData ;
	}
  }


//*******************************
//Implementation get_histoNames_vector()
//*******************************

/*! \brief Method returning a vector of strings containing the names of the histograms
 */

 vector<string> MuSR_td_PSI_bin::get_histoNames_vector()
  {
    vector <string> str_Vector ;
	  string strData ;
    for (int i = 0 ; i <= 15 ; i++)
    {
	    strData = labels_histo[i] ;
      str_Vector.push_back(strData) ;
		}
	  return str_Vector;
  }


//*******************************
//Implementation get_scalersNames_vector()
//*******************************

/*! \brief Method returning a vector of strings containing the names of the scalers
 */

 vector<string> MuSR_td_PSI_bin::get_scalersNames_vector()
  {
    vector <string> str_Vector ;
	  string strData ;
    for (int i = 0 ; i <= 17 ; i++)
    {
	    strData = labels_scalers[i] ;
      str_Vector.push_back(strData) ;
    }
	  return str_Vector;
  }


//*******************************
//Implementation get_temperatures_vector()
//*******************************

/*! \brief Method returning a vector of doubles containing monitored values (usually temperatures)
 */

 vector<double> MuSR_td_PSI_bin::get_temperatures_vector()
	{
		vector <double> dbl_Temper ;
		for (int i = 0 ; i <= 3 ; i++)
		{
			dbl_Temper.push_back(double(temper[i])) ;
		}
		return dbl_Temper;
	}


//*******************************
//Implementation get_devTemperatures_vector()
//*******************************

/*! \brief Method returning a vector of doubles containing standard deviations of the monitored values (usually temperatures)
 */

 vector<double> MuSR_td_PSI_bin::get_devTemperatures_vector()
	{
		vector <double> dbl_devTemper ;
		for (int i = 0 ; i <= 3 ; i++)
		{
			dbl_devTemper.push_back(double(temp_deviation[i])) ;
		}
	  return dbl_devTemper;
	}


//*******************************
//Implementation get_timeStart_vector()
//*******************************

/*! \brief Method returning a vector of strings containing 1) the date when the run was started and 2) the time when the run was started
 */

 vector<string> MuSR_td_PSI_bin::get_timeStart_vector()

	{
    vector<string> timeStart(2) ;
    timeStart[0] = date_start ;
    timeStart[1] = time_start ;
	  return timeStart ;
	}


//*******************************
//Implementation get_timeStop_vector()
//*******************************

/*! \brief Method returning a vector of strings containing 1) the date when the run was stopped and 2) the time when the run was stopped
 */

 vector<string> MuSR_td_PSI_bin::get_timeStop_vector()

  {
    vector<string> timeStop(2) ;
    timeStop[0] = date_stop ;
    timeStop[1] = time_stop ;
    return timeStop ;
  }


//*******************************
//Implementation max
//*******************************

 int MuSR_td_PSI_bin::max(int x, int y)
  {
     if (x >= y)
     {
         return x ;
     }
     return y ;
  }


//*******************************
//Implementation min
//*******************************

 int MuSR_td_PSI_bin::min(int x, int y)
  {
     if (x >= y)
     {
         return y ;
     }
     return x ;
  }























