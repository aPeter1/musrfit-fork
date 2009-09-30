// $Id$

#include <stdio.h>
#include <stdlib.h>

#include "mud/src/mud.h"


/***************************************************************
 * <p> A description of the routines is found here:
 * http://musr.org/mud/mud_friendly.html
 */
int main(int argc, char *argv[])
{
  int fh, success, i, j;
  UINT32  type;
  UINT32  val;
  UINT32  *pData;
  double  dval;
  int noOfHistos = 0;
  int noOfBins = 0;
  int firstGood = 0;
  char str[1024];

  if (argc != 2) {
    printf("\n");
    printf("usage: mud_test <filename>\n");
    printf("\n");
    return 0;
  }

  pData = NULL;

  fh = MUD_openRead( argv[1], &val );
  if ( fh == -1 ) {
    printf("\n");
    printf("**ERROR** Couldn't open MUD file %s\n", argv[1]);
    printf("\n");
    return 0;
  }

  success = MUD_getRunDesc( fh, &val );
  if ( !success ) {
    printf("\n");
    printf("**ERROR** Couldn't get run description tag.\n");
    printf("\n");
    MUD_closeRead( fh );
    return 0;
  }
  printf("run description tag: %lx\n", val);

  success = MUD_getRunNumber( fh, &val );
  if ( !success ) {
    printf("\n");
    printf("**ERROR** Couldn't get run number.\n");
    printf("\n");
    MUD_closeRead( fh );
    return 0;
  }
  printf("run number         : %ld\n", val);

  success = MUD_getTitle( fh, str, sizeof(str) );
  if ( !success ) {
    printf("\n");
    printf("**ERROR** Couldn't get run title.\n");
    printf("\n");
    MUD_closeRead( fh );
    return 0;
  }
  printf("run title          : %s\n", str);

  success = MUD_getTemperature( fh, str, sizeof(str) );
  if ( !success ) {
    printf("\n");
    printf("**ERROR** Couldn't get sample temperature.\n");
    printf("\n");
    MUD_closeRead( fh );
    return 0;
  }
  printf("sample temperature : %s\n", str);

  success = MUD_getField( fh, str, sizeof(str) );
  if ( !success ) {
    printf("\n");
    printf("**ERROR** Couldn't get field.\n");
    printf("\n");
    MUD_closeRead( fh );
    return 0;
  }
  printf("field              : %s\n", str);

  // ***********************************************************************
  // get histogram info's
  // ***********************************************************************

  // get number of histos and their type
  success = MUD_getHists(fh, &type, &val);
  if ( !success ) {
    printf("\n");
    printf("**ERROR** Couldn't get the number of histograms info.\n");
    printf("\n");
    MUD_closeRead( fh );
    return 0;
  }
  printf("histo: type/#histos: %ld/%ld\n", type, val);

  // keep # histos
  noOfHistos = (int)val;

  printf("------------\n");
  for (i=1; i<=noOfHistos; i++) {

    // get histo title
    success = MUD_getHistTitle( fh, i, str, sizeof(str) );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get the title of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    printf("histo # %d : title=%s\n", i, str);

    // get number of bins
    success = MUD_getHistNumBins( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get the number of bins of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    noOfBins = (int)val;
    printf("histo # %d : #bins=%ld\n", i, val);

    // get time resolution
    success = MUD_getHistFsPerBin( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get the time resolution of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    dval = (double)val/1.0e6;
    printf("histo # %d : time resolution=%lf (ns)\n", i, dval);

    // get t0
    success = MUD_getHistT0_Bin( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get t0 of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    printf("histo # %d : t0 bin=%ld\n", i, val);

    // get good bin 1
    success =  MUD_getHistGoodBin1( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get good bin 1 of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    firstGood = (int)val;
    printf("histo # %d : good bin 1=%ld\n", i, val);

    // get good bin 2
    success =  MUD_getHistGoodBin2( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get good bin 2 of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    printf("histo # %d : good bin 2=%ld\n", i, val);

    // get bkg bin 1
    success = MUD_getHistBkgd1( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get bkg bin 1 of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    printf("histo # %d : bkg bin 1=%ld\n", i, val);

    // get bkg bin 2
    success = MUD_getHistBkgd2( fh, i, &val );
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get bkg bin 2 of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }
    printf("histo # %d : bkg bin 2=%ld\n", i, val);

    // get data
    if (pData == NULL) {
      pData = (UINT32*)malloc(noOfBins*sizeof(pData));
      if (pData == NULL) {
        printf("\n");
        printf("**ERROR** Couldn't allocate memory for data.\n");
        printf("\n");
        MUD_closeRead( fh );
        return 0;
      }
    }

    success = MUD_getHistData( fh, i, pData);
    if ( !success ) {
      printf("\n");
      printf("**ERROR** Couldn't get data of histo %d.\n", i);
      printf("\n");
      MUD_closeRead( fh );
      return 0;
    }

    printf("histo # %d : bins from [firstGood(%d)-10] ... [firstGood+10]\n", i, firstGood);
    for (j=firstGood-10; j<firstGood+10; j++) {
      printf("%ld, ", pData[j]);
    }
    printf("\n");

    printf("------------\n");
  }

  MUD_closeRead( fh );

  free(pData);

  return 1;
}
