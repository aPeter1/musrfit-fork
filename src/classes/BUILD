*************************************************************
 
 Build instructions for the shared library libPMusr.so
 
 Andreas Suter, 2008/01/08
 $Id$
 
*************************************************************

At the moment a lot of the build stuff has to be done manually.
I will eventually migrate to cmake and than it will be much more
automatically.

Required packages:

* ROOT    >= 5.16  : needed for read/write some data files, plotting, ...
* Minuit2 >= 5.08  : needed for fitting
* Spirit  >= 1.8.5 : needed for parsing (this is part of the boost lib, and
                     hence installed on most linux machines). The boost libs
                     are platform independent and hence it should be easy in
                     the future to migrate also to Windows and Mac OS

Build instructions:

make -f Makefile.PMusr

than as root

make -f Makefile.PMusr install

                                          
