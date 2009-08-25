#use strict;
package MSR;

my $EMPTY="";
my $SPACE=" ";

my %DATADIRS = (
		"LEM",   "/mnt/data/nemu/his",
		"GPS",   "/afs/psi.ch/project/bulkmusr/data/gps",
		"LTF",   "/afs/psi.ch/project/bulkmusr/data/ltf",
		"Dolly", "/afs/psi.ch/project/bulkmusr/data/dolly",
		"GPD", "/afs/psi.ch/project/bulkmusr/data/gpd"
		);

my %BeamLines = ( "LEM", "MUE4", "GPS", "PIM3", "LTF", "PIM3", "Dolly", "PIE1", "GPD", "PIE1" );

my %Def_Format =
  ( "LEM", "ROOT-NPP", "GPS", "PSI-BIN", "LTF", "PSI-BIN", "Dolly", "PSI-BIN" , "GPD", "PSI-BIN");

# Additions to paremeters' names
my $erradd = "d";
my $minadd = "_min";
my $maxadd = "_max";

##########################################################################
# CreateMSR
#
# Input in %All
# Required:
#  $All{"FitType1/2/3"} - Function types, 3 components
#  $All{"LRBF"}         - Histograms, comma separated
#  $All{"Tis"}           
#  $All{"Tfs"}
#  $All{"BINS"}
#  $All{"FILENAME"}
#  $All{"go"}
#  $All{"TITLE"}
#  $All{"RunNumbers"}
#  $All{"FitAsyType"}
#  $All{"BeamLine"}
#  $All{"YEAR"}
# 
# Optional:
#  $All{"Sh_$Param"}
#  $All{"ltc"}
#  $All{"$Param"} value, error, min, and max
#  $All{"logx/y"}
#  $All{"Xi/f"}
#
# Output
#  $Full_T_Block - Full theory block
#  @Paramcomp    - Space separated list of parameters for each component
#  FILENAME.msr  - MSR file saved
#
##########################################################################
sub CreateMSR {
    my %All = %{$_[0]};

# "Smart" default value of the fit parameters.
    my %Defaults = (
		    "Asy",           "0.15",  "dAsy",          "0.01",
		    "Asy_min",       "0",     "Asy_max",       "0",
		    "Alpha",         "1.0",   "dAlpha",        "0.01",
		    "Alpha_min",     "0",     "Alpha_max",     "0",
		    "N0",            "300.0", "dN0",           "0.01",
		    "N0_min",        "0",     "N0_max",        "0",
		    "NBg",           "30.0",  "dNBg",          "0.01",
		    "NBg_min",       "0",     "NBg_max",       "0",
		    "Lam",           "1.0",   "dLam",          "0.01",
		    "Lam_min",       "0",     "Lam_max",       "0",
		    "Gam",           "1.0",   "dGam",          "0.01",
		    "Gam_min",       "0",     "Gam_max",       "0",
		    "Bet",           "0.5",   "dBet",          "0.01",
		    "Bet_min",       "0",     "Bet_max",       "0",
		    "Two",           "2.0",   "dTwo",          "0.0",
		    "Two_min",       "0",     "Two_max",       "0",
		    "Del",           "0.1",   "dDel",          "0.01",
		    "Del_min",       "0",     "Del_max",       "0",
		    "Sgm",           "0.1",   "dSgm",          "0.01",
		    "Sgm_min",       "0",     "Sgm_max",       "0",
		    "Aa",            "0.1",   "dAa",           "0.01",
		    "Aa_min",        "0",     "Aa_max",        "0",
		    "q",             "0.1",   "dq",            "0.01",
		    "q_min",         "0",     "q_max",         "0",
		    "Bg",            "0.036", "dBg",           "0.01",
		    "Bg_min",        "0",     "Bg_max",        "0",
		    "bgrlx",         "0.",    "dbgrlx",        "0.0",
		    "bgrlx_min",     "0",     "bgrlx_max",     "0",
		    "Frq",           "1.0",   "dFrq",          "1.",
		    "Frq_min",       "0",     "Frq_max",       "0",
		    "Field",         "100.0", "dField",        "1.",
		    "Field_min",     "0",     "Field_max",     "0",
		    "Energy",        "14.1",  "dEnergy",       "0.",
		    "Energy_min",    "0",     "Energy_max",    "0",
		    "DeadLayer",     "10.",   "dDeadLayer",    "0.1",
		    "DeadLayer_min", "0",     "DeadLayer_max", "0",
		    "Lambda",        "128.1", "dLambda",       "0.1",
		    "Lambda_min",    "0",     "Lambda_max",    "0",
		    "Phi",           "1.",    "dPhi",          "0.01",
		    "Phi_min",       "0",     "Phi_max",       "0"
		    );
    # Start with empty array
    my @FitTypes = ();

    foreach ($All{"FitType1"},$All{"FitType2"},$All{"FitType3"}) {
	if ($_ ne "None") {
	    @FitTypes=(@FitTypes,$_);
	}
    }

    my @Hists = split( /,/, $All{"LRBF"} );
    my @TiVals = split( /,/, $All{"Tis"} );
    my @TfVals = split( /,/, $All{"Tfs"} );
    my @BINVals = split( /,/, $All{"BINS"} );
    my $FILENAME = $All{"FILENAME"};
    my $BeamLine = $All{"BeamLine"};
    my $YEAR = $All{"YEAR"};
    my $DATADIR = $DATADIRS{$BeamLine};

    my $Step = $All{"go"};
    if ( $Step eq "PLOT" ) {
	$FITMINTYPE = $EMPTY;
    }
    elsif ( $Step eq "MIGRAD" ) {
	$FITMINTYPE = "MINIMIZE\nMIGRAD\nHESSE";
    }
    elsif ( $Step eq "MINOS" ) {
	$FITMINTYPE = "MIGRAD\nMINOS";
    }
    elsif ( $Step eq "SIMPLEX" ) {
	$FITMINTYPE = "SCAN\nSIMPLEX";
    }


    # First create the THEORY Block
    my ($Full_T_Block,$Paramcomp_ref)=MSR::CreateTheory(@FitTypes);

    my @Paramcomp = @$Paramcomp_ref;

    my $TitleLine = $All{"TITLE"}."\n# Run Numbers: ".$All{"RunNumbers"};
    $TitleLine =~ s/,/:/g;

    # Fit parameters block
    my $FitParaBlk = "
###################################################################
FITPARAMETER
###################################################################
#     No     Name       Value    Err     Min  Max                  ";

    # Counter for RUNS
    my $iRun = 1;

    # Counter of Params
    my $j = 1;

    my @RUNS=split( /,/, $All{"RunNumbers"});
    # $shcount is a counter for shared parameters
    if ( $#RUNS == 0 ) {
        my $shcount = 1;
    }
    else {
        if ( $All{"Sh_Alpha"} == 1 ) {
            my $shcount = 1;
        }
        elsif ( $All{"Sh_N0"} == 1 ) {
            my $shcount = 1;
        }
        elsif ( $All{"Sh_NBg"} == 1 ) {
            my $shcount = 1;
        }
        else {
            my $shcount = 0;
        }
    }

    $shcount   = 1;
    my $RUN_Block = $EMPTY;
    my $RUNS_Line = $EMPTY;

    # range order
    my $Range_Order = 1;
    foreach my $RUN (@RUNS) {
#######################################################################
        # Prepare the Parameters and initial values block
        my $component  = 0;
        my $Single_RUN = $EMPTY;

        # Prepare map line for each run
        my $MAP_Line = "map            ";

        # How many non-shared parameter for this RUN?
        my $nonsh = 0;

        # Prepeare Alpha/N0/NBg line for the RUN block. Empty initially.
        my $Alpha_Line = $EMPTY;

        # Loop over all components in the fit
        foreach my $FitType (@FitTypes) {
            ++$component;
            my $Parameters = $Paramcomp[ $component - 1 ];
            my @Params = split( /\s+/, $Parameters );

            # For the first component we need Alpha for Asymmetry fits
            if ( $component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
                unshift( @Params, "Alpha" );
	    }
	    # For the first component we need No and NBg for SingleHist fits
	    elsif ( $component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
                unshift( @Params, ( "N0", "NBg" ) );
            }

            foreach $Param (@Params) {
                $Param_ORG = $Param;
                if ( ($#FitTypes != 0) && ($Param ne "Alpha" && $Param ne "N0" && $Param ne "NBg" ) ) {
                    $Param = join( $EMPTY, $Param, "_", "$component" );
                }

                # If we have only one RUN then everything is shared
                if ( $#RUNS == 0 ) {
                    $Shared = 1;
                }
		# Otherwise check input if it was marked as shared
                else {
                    $Shared = $All{"Sh_$Param"};
                }

                # Alpha, N0 and NBg Lines
                #
                # If you encounter alpha in the parameters list make sure
                # to fill this line for the RUN block.
                if ( $Param_ORG eq "Alpha" ) {
                    if ($Shared) {
                        # If alpha is shared use always the same line
                        $Alpha_Line = "alpha           1\n";
                    }
                    else {
                        # Otherwise modify alpha line accordingly
                        $Alpha_Line = "alpha           $j\n";
                    }
                }
		# If you encounter N0 in the parameters list make sure
		# to fill this line for the RUN block.
                elsif ( $Param_ORG eq "N0" ) {
                    if ($Shared) {
                        $Alpha_Line = "norm            1\n";
                    }
                    else {
                        $Alpha_Line = "norm            $j\n";
                    }

                    # Optional - add lifetime correction for SingleHist fits
                    if ( $All{"ltc"} eq "y" ) {
                        $Alpha_Line = $Alpha_Line . "lifetimecorrection\n";
                    }

                    # If you encounter NBg in the parameters list make sure
                    # to fill this line for the RUN block.
                }
                elsif ( $Param_ORG eq "NBg" ) {
                    if ($Shared) {
                        $Alpha_Line = $Alpha_Line . "backgr.fit      2\n";
                    }
                    else {
                        $Alpha_Line = $Alpha_Line . "backgr.fit      $j\n";
                    }
                }

                # End of Alpha, N0 and NBg Lines
####################################################################################################

####################################################################################################
                # Extract initial values, error and max/min from input
                $Name     = $Param . "_" . $iRun;
                $value    = $All{"$Name"};
                $error    = $All{"$erradd$Name"};
                $minvalue = $All{"$Name$minadd"};
                $maxvalue = $All{"$Name$maxadd"};
# If these values are not in the input then take the defaulst
# I think that this can go at the end if we use a separate initialization routine
		if ($value eq $EMPTY) {
		    $value = $Defaults{"$Param_ORG"};
		    $error = $Defaults{"$erradd$Param_ORG"};
		    $minvalue = $Defaults{"$Param_ORG$minadd"};
		    $maxvalue = $Defaults{"$Param_ORG$maxadd"};
		}
                if ( $minvalue == $maxvalue ) {
                    $minvalue = $EMPTY;
                    $maxvalue = $EMPTY;
                }
####################################################################################################

                # Start preparing the parameters block
                if ($Shared) {

                    # Parameter is shared enough to keep order from first run
                    if ( $iRun == 1 ) {
                        $FitParaBlk = $FitParaBlk . "
      $j      $Name    $value     $error    $error    $minvalue    $maxvalue";
                        $Full_T_Block =~ s/$Param_ORG/$j/;
                        ++$shcount;
                        ++$j;
                    }
                } else {

                # Parameter is not shared, use map unless it is a single RUN fit
                    $FitParaBlk = $FitParaBlk . "
      $j      $Name    $value     $error    $error    $minvalue    $maxvalue";

                    # Skip adding to map line in these cases
                    if (   $Param ne "Alpha"
			   && $Param ne "N0"
			   && $Param ne "NBg"
			   && $#RUNS != 0 )
                    {
                        ++$nonsh;
                        $Full_T_Block =~ s/$Param_ORG/map$nonsh/;
                        $MAP_Line = join( ' ', $MAP_Line, $j );
                    }
                    ++$j;
                }
                $NtotPar = $j;
            }
        }

        # Finished preparing the FITPARAMETERS block
#######################################################################

        # For each defined range we need a block in the RUN-Block
        # Also for each histogram in Single Histograms fits
        # Also for Imaginaryand and Real for RRF fits

        $RUN = $RUNS[ $iRun - 1 ];
        if    ( $RUN < 10 )   { $RUN = "000" . $RUN; }
        elsif ( $RUN < 100 )  { $RUN = "00" . $RUN; }
        elsif ( $RUN < 1000 ) { $RUN = "0" . $RUN; }

        # Get current year
        ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
          localtime( time() );
        my $current_year = $year + 1900;

        #	    $TC="$TC<br>Data dir:".$DATADIRS{$BeamLine}."<br>";
        if ( $BeamLine eq "LEM" ) {
            $RUN_File_Name = "lem" . substr( $YEAR, 2 ) . "_his_" . $RUN;
            $RUNFILE       = "$DATADIR/$YEAR/$RUN_File_Name";
            $RUN_Line      = join( $SPACE,
                "RUN", $RUNFILE, $BeamLines{$BeamLine}, "PSI",
                $Def_Format{$BeamLine} );
        }
        elsif ( $BeamLine eq "GPS" ) {
            $RUN_File_Name = "deltat_pta_gps_" . $RUN;
            if ( $YEAR == $current_year ) {
                $RUNFILE = "$DATADIR/$RUN_File_Name";
            }
            else {
                $RUNFILE = "$DATADIR/d$YEAR/pta/$RUN_File_Name";
            }
            $RUN_Line = join( $SPACE,
                "RUN", $RUNFILE, $BeamLines{$BeamLine}, "PSI",
                $Def_Format{$BeamLine} );
        }
        elsif ( $BeamLine eq "LTF" ) {
            $RUN_File_Name = "deltat_pta_ltf_" . $RUN;
            if ( $YEAR == $current_year ) {
                $RUNFILE = "$DATADIR/$RUN_File_Name";
            }
            else {
                $RUNFILE = "$DATADIR/d$YEAR/pta/$RUN_File_Name";
            }
            $RUN_Line = join( $SPACE,
                "RUN", $RUNFILE, $BeamLines{$BeamLine}, "PSI",
                $Def_Format{$BeamLine} );
        }
        elsif ( $BeamLine eq "Dolly" ) {
            $RUN_File_Name = "deltat_pta_dolly_" . $RUN;
            if ( $YEAR == $current_year ) {
                $RUNFILE = "$DATADIR/$RUN_File_Name";
            }
            else {
                $RUNFILE = "$DATADIR/d$YEAR/pta/$RUN_File_Name";
            }
            $RUN_Line = join( $SPACE,
                "RUN", $RUNFILE, $BeamLines{$BeamLine}, "PSI",
                $Def_Format{$BeamLine} );
        }
        elsif ( $BeamLine eq "GPD" ) {
            $RUN_File_Name = "deltat_pta_gpd_" . $RUN;
            if ( $YEAR == $current_year ) {
                $RUNFILE = "$DATADIR/$RUN_File_Name";
            }
            else {
                $RUNFILE = "$DATADIR/d$YEAR/pta/$RUN_File_Name";
            }
            $RUN_Line = join( $SPACE,
                "RUN", $RUNFILE, $BeamLines{$BeamLine}, "PSI",
                $Def_Format{$BeamLine} );
        }

        # What kind of fit? 0 - Single Histogram, 2 - Asymmetry, 4 - RRF
        if ( $All{"FitAsyType"} eq "Asymmetry" ) {
            $Type_Line = "fittype         2";
            $PLT       = 2;
            $Hist_Lines =
              "forward         $Hists[0]\nbackward        $Hists[1]";
            $Bg_Line   = "background      66000   66500   66000   66500";
            $Data_Line = "data            3419    63000   3419    63000";
        }
        elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
            $Type_Line  = "fittype         0";
            $PLT        = 0;
            $Hist_Lines = "forward         HIST";
            $Bg_Line    = $EMPTY;
            $Data_Line  = "data            3419    63000";
        }
        elsif ( $All{"FitAsyType"} eq "RotRF" ) {
            $Type_Line = "fittype         4";
        }

        # Omit background and data lines for LTG,GPS and Dolly
        if ( $BeamLine ne "LEM" ) {
            $Bg_Line = "background 50 250 50 250";
            $Data_Line =
"t0              287     284\ndata            297     8000    294     8000";
        }

 #	$MAP_Line = "map             0    0    0    0    0    0    0    0    0    0";
        $FRANGE_Line = "fit             TINI    TFIN";
        $PAC_Line    = "packing         BINNING";

        if ( $All{"FitAsyType"} eq "Asymmetry" ) {
            $Single_RUN =
"$RUN_Line\n$Type_Line\n$Alpha_Line$Hist_Lines\n$Bg_Line\n$Data_Line\n$MAP_Line\n$FRANGE_Line\n$PAC_Line\n\n";
        }
        elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
            $Single_RUN = $EMPTY;
            foreach my $Hist (@Hists) {
                $Tmp_Hist_Line = $Hist_Lines;
                $Tmp_Hist_Line =~ s/HIST/$Hist/g;
                $Single_RUN = $Single_RUN
                  . "$RUN_Line\n$Type_Line\n$Alpha_Line$Tmp_Hist_Line\n$Data_Line\n$MAP_Line\n$FRANGE_Line\n$PAC_Line\n\n";
            }
        }

        # Now add the appropriate values of fit range and packing
        my $Range_Min = 8;
        my $Range_Max = 0;
        my $k         = 0;
        foreach my $Ti (@TiVals) {
            my $Tf        = $TfVals[$k];
            my $BIN       = $BINVals[$k];
            $RUN_Block = $RUN_Block . $Single_RUN;
            $RUN_Block =~ s/TINI/$Ti/g;
            $RUN_Block =~ s/TFIN/$Tf/g;
            $RUN_Block =~ s/BINNING/$BIN/g;

            # For multiple ranges use this
            if ( $Ti < $Range_Min ) { $Range_Min = $Ti; }
            if ( $Tf > $Range_Max ) { $Range_Max = $Tf; }
            if ( $All{"FitAsyType"} eq "Asymmetry" ) {
                $RUNS_Line = "$RUNS_Line " . $Range_Order;
            }
            elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
                $RUNS_Line = "$RUNS_Line " . $Range_Order;
            }
            ++$k;
            ++$Range_Order;
        }

        #	if ($FitAsyType eq "Asymmetry") {$RUNS_Line="$RUNS_Line ".$iRun*$k;}
        ++$iRun;
    }

    # The number of runs is
    $NRUNS = $iRun - 1;

    $Full_T_Block = "
###################################################################
THEORY
###################################################################
$Full_T_Block
";

    $RUN_Block =
      "###################################################################
$RUN_Block";

    $COMMANDS_Block =
      "###################################################################
COMMANDS
FITMINTYPE
SAVE
";
    $COMMANDS_Block =~ s/FITMINTYPE/$FITMINTYPE/g;

    # Check if log x and log y are selected
    my $logxy = $EMPTY;
    if ( $All{"logx"} eq "y" ) { $logxy = $logxy . "logx\n"; }
    if ( $All{"logy"} eq "y" ) { $logxy = $logxy . "logy\n"; }

    # Check if a plot range is defined (i.e. different from fit)
    $PRANGE_Line = "use_fit_ranges";
    if ( $All{"Xi"} != $All{"Xf"} ) {

        #	if ($Yi != $Yf) {
        $PRANGE_Line = "range  ".$All{"Xi"}."  ".$All{"Xf"}."  ".$All{"Yi"}."  ".$All{"Yf"};

        #	} else {
        #	    $PRANGE_Line = "range  $Xi  $Xf";
        #	}
    }

    $PLOT_Block =
      "###################################################################
PLOT $PLT
runs     $RUNS_Line
$PRANGE_Line
$logxy";

    # Don't know why but it is needed initially
    $STAT_Block =
      "###############################################################
STATISTIC --- 0000-00-00 00:00:00
*** FIT DID NOT CONVERGE ***";

    # Empty line at the end of each block
    my $FullMSRFile = "$TitleLine$FitParaBlk\n$Full_T_Block\n$RUN_Block\n$COMMANDS_Block\n$PLOT_Block\n$STAT_Block\n";

# Open output file FILENAME.msr
    open( OUTF,q{>},"$FILENAME.msr" );
    print OUTF ("$FullMSRFile");
    close(OUTF);
    return($Full_T_Block,\@Paramcomp);
}

########################
# Createheory
########################
sub CreateTheory { 
    # This subroutine requires:
    # @FitTypes     - the array of the types of components (summed)
    
    # It will return:
    # $Full_T_Block - the full theory block with names of parameters
    # @Paramcomp    - an array of space separated list of parameters 
    #                 each member of the array is one component

    my (@FitTypes) = @_;
    
    # Start from this theory line for the different fitting functions
    my %THEORY = (
		  "asymmetry",   "Asy",
		  "simplExpo",   "Lam",
		  "generExpo",   "Lam Bet",
		  "simpleGss",   "Sgm",
		  "statGssKT",   "Sgm",
		  "statGssKTLF", "Frq Sgm",
		  "dynGssKTLF",  "Frq Sgm Lam",
		  "statExpKT",   "Lam",
		  "statExpKTLF", "Frq Aa",
		  "dynExpKTLF",  "Frq Aa Lam",
		  "combiLGKT",   "Lam Sgm",
		  "spinGlass",   "Lam gam q",
		  "rdAnisoHf",   "Frq Lam",
		  "TFieldCos",   "Phi Frq",
		  "internFld",   "Alp Phi Frq LamT LamL",
		  "Bessel",      "Phi Frq",
		  "internBsl",   "Alp Phi Frq LamT LamL",
		  "abragam",     "Sgm gam",
		  "Meissner",    "Phi Energy Field DeadLayer Lambda",
		  "skewedGss",   "Phi Frq Sgmm Sgmp"
		  );

#######################################################################
    # Generate the full THEORY block
    my $Full_T_Block = $EMPTY;
    my $component    = 0;
    foreach my $FitType (@FitTypes) {
        ++$component;
	# Add components
	if ( $component > 1 ) {
	    $Full_T_Block = $Full_T_Block . "\n+\n";
	}
	    
	# For each component there is a THEORY block starting with "asymmetry"
	my $T_Block    = "asymmetry " . $THEORY{'asymmetry'};
	my $Parameters = $THEORY{'asymmetry'};
	
	# Compose the THEORY block
	# The duplicate names are changed for multiplied signals or just
	# change names to match parameters list declared for the function
	
	# Stretch exponential
	if ( $FitType eq "Stretch" ) {
	    $T_Block = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	}
	
	# Exponential
	elsif ( $FitType eq "Exponential" ) {
	    $T_Block = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
	}
	
	# Gaussian
	elsif ( $FitType eq "Gaussian" ) {
	    $T_Block = $T_Block . "\n" . "simpleGss " . $THEORY{'simpleGss'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simpleGss'} );
	}
	
	# Exponential with 0 relaxation
	elsif ( $FitType eq "Background" ) {
	    
	    # Do nothing
	}
	
	# Oscillationg exponential
	elsif ( $FitType eq "ExponentialCos" ) {
	    $T_Block    = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
	    $T_Block    = $T_Block . "\n" . "TFieldCos " . $THEORY{'TFieldCos'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'TFieldCos'} );
	}
	
	# Oscillationg gaussian
	elsif ( $FitType eq "GaussianCos" ) {
	    $T_Block    = $T_Block . "\n" . "simpleGss " . $THEORY{'simpleGss'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simpleGss'} );
	    $T_Block    = $T_Block . "\n" . "TFieldCos " . $THEORY{'TFieldCos'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'TFieldCos'} );
	    }
	
	# Oscillationg stretch exponential
	elsif ( $FitType eq "StretchCos" ) {
	    $T_Block    = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	    $T_Block    = $T_Block . "\n" . "TFieldCos " . $THEORY{'TFieldCos'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'TFieldCos'} );
	}
	
	# Oscillationg Gaussian
	elsif ( $FitType eq "GaussianCos" ) {
	    $T_Block    = $T_Block . "\n" . "simpelGss " . $THEORY{'simpelGss'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simpleGss'} );
	    $T_Block    = $T_Block . "\n" . "TFieldCos " . $THEORY{'TFieldCos'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'TFieldCos'} );
	    }
	
	# Static Lorentzian KT
	elsif ( $FitType eq "SLKT" ) {
	    $T_Block = $T_Block . "\n" . "statExpKT " . $THEORY{'statExpKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statExpKT'} );
	}
	
	# Static Lorentzian KT LF
	elsif ( $FitType eq "SLKTLF" ) {
	    $T_Block =
		$T_Block . "\n" . "statExpKTLF " . $THEORY{'statExpKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statExpKTLF'} );
	}
	
	# Dynamic Lorentzian KT LF
	elsif ( $FitType eq "LDKTLF" ) {
	    $T_Block = $T_Block . "\n" . "dynExpKTLF " . $THEORY{'dynExpKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'dynExpKTLF'} );
	}
	
	# Static Gaussian KT
	elsif ( $FitType eq "SGKT" ) {
	    $T_Block = $T_Block . "\n" . "statGssKT " . $THEORY{'statGssKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statGssKT'} );
	}
	
	# Static Gaussian KT LF
	elsif ( $FitType eq "SGKTLF" ) {
	    $T_Block =
		$T_Block . "\n" . "statGssKTLF " . $THEORY{'statGssKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statGssKTLF'} );
	}
	
	# Dynamic Gaussian KT LF
	elsif ( $FitType eq "GDKTLF" ) {
	    $T_Block = $T_Block . "\n" . "dynGssKTLF " . $THEORY{'dynGssKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'dynGssKTLF'} );
	}
	
	# Now some more combined functions (multiplication).
	
	# Lorentzian KT LF multiplied by exponential
	elsif ( $FitType eq "LLFExp" ) {
	    $T_Block = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
		$Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
	    $T_Block =
		$T_Block . "\n" . "statExpKTLF " . $THEORY{'statExpKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statExpKTLF'} );
	}
	
	# Lorentzian KT LF multiplied by stretched exponential
	elsif ( $FitType eq "LLFSExp" ) {
	    $T_Block = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	    $T_Block =
		$T_Block . "\n" . "statExpKTLF " . $THEORY{'statExpKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statExpKTLF'} );
	}
	
	# Gaussian KT LF multiplied by exponential
	elsif ( $FitType eq "GLFExp" ) {
	    $T_Block = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
	    $T_Block =
		$T_Block . "\n" . "statGssKTLF " . $THEORY{'statGssKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statGssKTLF'} );
	}
	
	# Gaussian KT LF multiplied by stretched exponential
	elsif ( $FitType eq "GLFSExp" ) {
	    $T_Block = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	    $T_Block =
		$T_Block . "\n" . "statGssKTLF " . $THEORY{'statGssKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statGssKTLF'} );
	}
	
	# Meissner state model
	elsif ( $FitType eq "Meissner" ) {
	    $T_Block = $T_Block . "\n" . "simpleGss " . $THEORY{'simpleGss'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simpleGss'} );
	    $T_Block =
		$T_Block . "\n"
		. "userFcn  libTFitPofB.so TLondon1DHS "
		. $THEORY{'Meissner'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'Meissner'} );
	}
	
	$Paramcomp[ $component - 1 ] = $Parameters;
	$Full_T_Block = $Full_T_Block . $T_Block;
	
	# Finished preparing the THEORY block
#######################################################################
    }
    return($Full_T_Block,\@Paramcomp);
}    # End CreateTheory

########################
# ExtractParamBlk
########################
sub ExtractParamBlk {
# This subroutine takes the MSR file as input and extracts the parameters
# with the corresponding values, errors etc...

# Take the msr file as input array of lines
    my @file=@_;

    my $NFITPARAMETERS=0;
    my $NTHEORY=0;
    my $NRUN=0;

# Remove comment lines
    @file = grep {!/^\#/} @file;
# Remove empty lines
    @file = grep {/\S/} @file;

# Identify different blocks
    my $i=0;
    my $line ="";
    foreach $line (@file)
    {
	if (grep {/FITPARAMETER/} $line) {$NFITPARAMETERS=$i;}
	if (grep {/THEORY/} $line) { $NTHEORY=$i;} 
	if ((grep {/RUN/} $line) & $NRUN==0) { $NRUN=$i;} 
	$i++;
    }
    my @FPBlock=@file[$NFITPARAMETERS+1..$NTHEORY-1];
    my @TBlock=@file[$NTHEORY+1..$NRUN-1];

# Split parameter's line to extract values and errors
    foreach $line (@FPBlock) {
	my @Param=split(/\s+/,$line);
    }

    return(\@FPBlock)
}


1;
