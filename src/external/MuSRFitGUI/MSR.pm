#use strict;
package MSR;

# This module provides some tools to create, manipulate and extract information from MSR files

my $EMPTY="";
my $SPACE=" ";

my %DATADIRS = (
		"LEM",   "/afs/psi.ch/project/bulkmusr/data/lem",
		"GPS",   "/afs/psi.ch/project/bulkmusr/data/gps",
		"LTF",   "/afs/psi.ch/project/bulkmusr/data/ltf",
		"Dolly", "/afs/psi.ch/project/bulkmusr/data/dolly",
		"GPD", "/afs/psi.ch/project/bulkmusr/data/gpd",
		"HAL",   "/afs/psi.ch/project/bulkmusr/data/hifi"
		);

my %BeamLines = ( "LEM", "MUE4", "LEM (PPC)", "MUE4", "GPS", "PIM3", "LTF", "PIM3", "Dolly", "PIE1", "GPD", "PIE1" , "HAL", "PIE3");

my %Def_Format =
  ( "LEM", "MUSR-ROOT", "LEM (PPC)", "ROOT-PPC", "GPS", "PSI-BIN", "LTF", "PSI-BIN", "Dolly", "PSI-BIN" , "GPD", "PSI-BIN", "HAL", "PSI-MDU");

# Additional information to extract run properties from database
# For LEM use summary files
my $SUMM_DIR="/afs/psi.ch/project/nemu/data/summ/";
# For Bulok use list files
my %DBDIR=("LEM","/afs/psi.ch/project/nemu/data/log/",
	"GPS","/afs/psi.ch/project/bulkmusr/olddata/list/",
	"Dolly","/afs/psi.ch/project/bulkmusr/olddata/list/",
        "GPD","/afs/psi.ch/project/bulkmusr/olddata/list/",
        "ALC","/afs/psi.ch/project/bulkmusr/olddata/list/",
        "HAL","/afs/psi.ch/project/bulkmusr/olddata/list/",
        "LTF","/afs/psi.ch/project/bulkmusr/olddata/list/",
        "ALL","/afs/psi.ch/project/bulkmusr/olddata/list/");

# Information available since
my %MinYears=("LEM","2001",
	   "GPS","1993",
	   "Dolly","1998",
	   "GPD","1993",
	   "ALC","1993",
	   "HAL","2012",
	   "LTF","1995");

# And to deal with old names of bulk muons
my %AltArea=("GPS","PIM3","LTF","PIM3","ALC","PIE3","Dolly","PIE1","GPD","MUE1");


# Additions to paremeters' names
my $erradd = "d";
my $minadd = "_min";
my $maxadd = "_max";

sub CreateMSRUni {
##########################################################################
# CreateMSRUni
#
# Input in %All
# Required:
#  $All{"FitTypei"}     - Function types, i components
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
    my %All = %{$_[0]};
    my $DEBUG = "";
    # Start with empty array
    my @FitTypes = ();
    # loop over fitTypes
    if (!defined($All{"numComps"})) {$All{"numComps"}=3;}
    for (my $i=1;$i<=$All{"numComps"};$i++) {
        if ( defined($All{"FitType$i"}) &&$All{"FitType$i"} ne "None" ) {
            push( @FitTypes, $All{"FitType$i"} );           
        }
    }

    my @Hists = split( /,/, $All{"LRBF"} );
    # TODO
    # : to separate two sets of asymmetries with same parameters
    # Check the number of histograms
    # should be 2 or 4 histograms
    # should be checked in GUI
    # for 2 simple asymmetry fit
    # for 4 two run blocks with different geometric parameters:
    # Alpha, No, NBg, Phi, Asy

    my @TiVals = split( /,/, $All{"Tis"} );
    my @TfVals = split( /,/, $All{"Tfs"} );
    my @BINVals = split( /,/, $All{"BINS"} );
    my $FILENAME = $All{"FILENAME"};
    my $BeamLine = $All{"BeamLine"};
    my $YEAR = $All{"YEAR"};

    my $FITMINTYPE = $EMPTY;
    if ( $All{"Minimization"} ne $EMPTY &&  $All{"ErrorCalc"} ne $EMPTY && $Step ne "PLOT" ) {
	$FITMINTYPE = $All{"Minimization"}."\n".$All{"ErrorCalc"};
    }

    # First create the THEORY Block
    my ($Full_T_Block,$Paramcomp_ref)=MSR::CreateTheory(@FitTypes);
    my @Paramcomp = @$Paramcomp_ref;

    # If we have a FUNCTIONS Block the Full_T_Block should be 
    # replaced by Func_T_Block
    my $FUNCTIONS_Block = $EMPTY;
    if ($All{"FunctionsBlock"} ne $EMPTY) {
        $FUNCTIONS_Block = "FUNCTIONS\n###############################################################\n".$All{"FunctionsBlock"}."\n";
        $Full_T_Block=$All{"Func_T_Block"};
	# remove all _N to end (may fail with large number of parameters)
	$Full_T_Block =~ s/_\d\b//g;
    }

    # Counter for RUNS
    my $iRun = 1;

    # Counter of Params
    my $PCount = 1;

    my $PLT = 2;
    # For SingleHist fit
    if ($All{"FitAsyType"} eq "SingleHist" ) { $PLT = 0;}

    # Need to select here RUNSAuto or RUNSManual
    # $RUNSType = 0 (Auto) or 1 (Manual)
    my $RUNSType = 0;
    my @RUNS=();
    if ($All{"RunNumbers"} ne $EMPTY) {
	# Remove spaces and other illegal characters
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	$All{"RunNumbers"} =~ s/[a-zA-Z]//g;
	# Expand hyphens and colons
	$All{"RunNumbers"} = MSR::ExpandRunNumbers($All{"RunNumbers"});
	# Split commas
	@RUNS=split( /,/, $All{"RunNumbers"});
	$RUNSType = 0;
    } elsif ($All{"RunFiles"} ne $EMPTY) {
	@RUNS=split( /,/, $All{"RunFiles"});
	$RUNSType = 1;
    }

    # Initialise $shcount, a counter for shared parameters
    my $shcount   = 1;
    my $RUN_Block = $EMPTY;
    my $RUNS_Line = $EMPTY;

    # range order
    my $Range_Order = 1;
    foreach my $RUN (@RUNS) {
	if ($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB") {
	    # Prepare the Parameters and initial values block
	    my $component  = 0;
	    my $Single_RUN = $EMPTY;

	    # Prepare map line for each run
	    my $MAP_Line = "map            ";

	    # How many non-shared parameter for this RUN?
	    my $nonsh = 0;

	    # Prepeare Alpha line for the RUN block. Empty initially.
	    my $Alpha_Line = $EMPTY;
	    
	    # Loop over all components in the fit
	    foreach my $FitType (@FitTypes) {
		++$component;
		my $Parameters = $Paramcomp[ $component - 1 ];
		my @Params = split( /\s+/, $Parameters );
		
		# For the first component we need Alpha for Asymmetry fits
		if ($component == 1) {
		    unshift( @Params, "Alpha" );
		}

		foreach my $Param (@Params) {
		    my $Param_ORG = $Param;
		    if ( ($#FitTypes != 0) && ($Param ne "Alpha") ) {
			$Param = join( $EMPTY, $Param, "_", "$component" );
		    }

		    # If we have only one RUN then everything is shared
		    my $Shared = 0;
		    if ( $#RUNS == 0 ) {
			$Shared = 1;
		    }
		    # Otherwise check input if it was marked as shared
		    else {
			$Shared = $All{"Sh_$Param"};
		    }

		    # Alpha Line
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
			    $Alpha_Line = "alpha           $PCount\n";
			}
		    }
		    
		    # End of Alpha Line
		    
		    # Start preparing the parameters block
		    if ($Shared) {
			# Parameter is shared enough to keep order from first run
			if ( $iRun == 1 ) {
			    $Full_T_Block =~ s/$Param_ORG/$PCount/;
			    ++$shcount;
			    ++$PCount;
			}
		    } else {
			# Parameter is not shared, use map unless it is a single RUN fit
			# Skip adding to map line in these cases
			if ( ( $Param ne "Alpha" && $#RUNS != 0 ) ||  ( !( $Param_ORG =~ m/^(No|NBg)/ ) && $#RUNS != 0 )) {
			    ++$nonsh;
			    $Full_T_Block =~ s/$Param_ORG/map$nonsh/;
			    $MAP_Line = join( ' ', $MAP_Line, $PCount );
			}
			++$PCount;
		    }
		    my $NtotPar = $PCount;
		}
	    }

	    # Finished preparing the FITPARAMETERS block

	    # Start constructing RUN block here
	    # For each defined range we need a block in the RUN-Block
	    # Also for each histogram in Single Histograms fits

	    $RUN = $RUNS[ $iRun - 1 ];

	    #$DEBUG = $DEBUG.MSR::CreateRUNBlk($RUN, $YEAR, $BeamLine, $All{"FitAsyType"}, $All{"LRBF"}, $All{"Tis"}, $All{"Tfs"}, $All{"BINS"});
	    
	    my $RUN_Line = $EMPTY;
	    if ($All{"RUNSType"}) {
		$RUN_Line = MSR::RUNFileNameAuto($RUN,"0000",$EMPTY);
	    } else {
		$RUN_Line = MSR::RUNFileNameAuto($RUN,$YEAR,$BeamLine);
	    }

	    my $Type_Line = "fittype         2";
	    my $Hist_Lines =
		"forward         $Hists[0]\nbackward        $Hists[1]";

	    my $Bg_Line = "background";
	    my $Data_Line = "data";
	    my $T0_Line = "t0";
	    my $NHist=1;
	    foreach my $Hist (@Hists) {
		# If empty skip lines
		if ($All{"Bg1$NHist"} ne $EMPTY && $All{"Bg2$NHist"} ne $EMPTY) {
		    $Bg_Line = $Bg_Line."    ".$All{"Bg1$NHist"}."    ".$All{"Bg2$NHist"};
		}
		if ($All{"Data1$NHist"} ne $EMPTY && $All{"Data2$NHist"} ne $EMPTY) {
		    $Data_Line =$Data_Line."    ".$All{"Data1$NHist"}."    ".$All{"Data2$NHist"};
		}
		if ($All{"t0$NHist"} ne $EMPTY) {
		    $T0_Line=$T0_Line."      ".$All{"t0$NHist"};  
		}
		$NHist++;
	    }

	    # Put T0_Line Bg_Line and Data_Line together if not empty
	    my $T0DataBg=$EMPTY;
	    if ($T0_Line ne "t0") {
		$T0DataBg = $T0DataBg.$T0_Line."\n";
	    }
	    if ($Bg_Line ne "background") {
		$T0DataBg = $T0DataBg.$Bg_Line."\n";
	    }
	    if ($Data_Line ne "data") {
		$T0DataBg = $T0DataBg.$Data_Line."\n";
	    }

	    $FRANGE_Line = "fit             TINI    TFIN";
            $PAC_Line    = "packing         BINNING";

            $Single_RUN =
                "$RUN_Line\n$Alpha_Line$Hist_Lines\n$T0DataBg$MAP_Line\n";
        
            # Now add the appropriate values of fit range and packing
            # If there no multiple ranges/packing then simply use GLOBAL block
            if ($#TiVals == 0) {
                $GLOBAL_Block = "GLOBAL\n$Type_Line\n$FRANGE_Line\n$PAC_Line\n\n";
            } else {
                $GLOBAL_Block = "GLOBAL\n$Type_Line\n\n";
                $Single_RUN = $Single_RUN."$FRANGE_Line\n$PAC_Line\n";
            }
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
                $GLOBAL_Block =~ s/TINI/$Ti/g;
                $GLOBAL_Block =~ s/TFIN/$Tf/g;
                $GLOBAL_Block =~ s/BINNING/$BIN/g;
                
                # For multiple ranges use this
                if ( $Ti < $Range_Min ) { $Range_Min = $Ti; }
		if ( $Tf > $Range_Max ) { $Range_Max = $Tf; }
		$RUNS_Line = "$RUNS_Line " . $Range_Order;
		++$k;
		++$Range_Order;
	    }
	} else {
	    #######################################################################
	    # For a single histogram fit we basically need to repeat
	    # this for each hist However, "physical" parameters such
	    # as Asymmetry, relaxation etc. should be the same for all
	    # histograms!

	    # We distinguich between sharing among different runs to
	    # common parameters for different histograms. The first is
	    # done in the usual "Sharing" schame, while the second has
	    # to be done in the functions block. This can be done in a
	    # consistent, non-confusing algorithm

	    # iHist is the histogram counter, used to catch Phi
	    my $iHist = 1;
	    foreach my $Hist (@Hists) {
		# Prepare the Parameters and initial values block
		my $component  = 0;
		my $Single_RUN = $EMPTY;
		
		# Prepare map line for each run
		my $MAP_Line = "map            ";
		
		# How many non-shared parameter for this RUN?
		my $nonsh = 0;
		
		# Prepeare No/NBg line for the RUN block. Empty initially.
		my $NoBg_Line = $EMPTY;
		
		# Loop over all components in the fit
		foreach my $FitType (@FitTypes) {
		    ++$component;
		    my $Parameters = $Paramcomp[ $component - 1 ];
		    my @Params = split( /\s+/, $Parameters );
		    # Only the first histiograms has new physical parameters
		    # the others keep only Phi if they have it
		    if ($iHist != 1 ) {
			# @Param contains only Phi
			if (grep(/Phi/,@Params)) {
			    @Params=("Phi");
			} else {
			    @Params=();
			}
		    }	

		    # For the first component we need No and NBg for SingleHist fits
		    if ( $component == 1 ) {
			unshift( @Params, ( "No", "NBg" ) );
		    }

		    foreach my $Param (@Params) {
			my $Param_ORG = $Param;
			# Tag with histogram number only No, NBg and Phi
			if ($Param_ORG =~ m/^(No|NBg|Phi)/ ) {$Param=$Param.$Hist;}
			if ( ($#FitTypes != 0) && !( $Param_ORG =~ m/^(No|NBg)/ ) ) {
			    $Param = join( $EMPTY, $Param, "_", "$component" );
			}
		    
			# If we have only one RUN with one Histogram then everything is shared
			my $Shared = 0;
			if ( $#RUNS == 0 && $#Hists == 0 ) {
			    $Shared = 1;
			} else { 
			    # Otherwise check input if it was marked as shared
			    $Shared = $All{"Sh_$Param"};
			}
		    
			# No and NBg Lines
			#
			# If you encounter No in the parameters list make sure
			# to fill this line for the RUN block.
			if ( $Param_ORG eq "No" ) {
			    if ($Shared) {
				$NoBg_Line = "norm            1\n";
			    }
                            else {
                                $NoBg_Line = "norm            $PCount\n";
                            }
                        }
                        # If you encounter NBg in the parameters list make sure
                        # to fill this line for the RUN block.
			elsif ( $Param_ORG eq "NBg" ) {
			    if ($Shared) {
				$NoBg_Line = $NoBg_Line . "backgr.fit      2\n";
			    } else {
				$NoBg_Line = $NoBg_Line . "backgr.fit      $PCount\n";
			    }
			}
			# End of No and NBg Lines
		    
			# Start preparing the parameters block
			if ((($Shared && $iRun == 1) || $#RUNS == 0) && $Param_ORG ne "Phi" ) {
			    # Do not map a parameter if 
			    # - it is shared and this is the first run
			    # - this is a single run fit
			    # it is enough to keep order from first run
			    $Full_T_Block =~ s/$Param_ORG/$PCount/;
			    if ($Shared) {++$shcount;}
			    ++$PCount;
			} else {
			    # Parameter is not shared, use map unless it is a single RUN fit
			    # Skip adding to map line in these cases
#			    if ( !( $Param_ORG =~ m/^(No|NBg)/ ) && ($#RUNS != 0 || $#Hist != 0)) {
			    if ( !( $Param_ORG =~ m/^(No|NBg)/ ) ) {
				$DEBUG = $DEBUG."# I am here: $Param_ORG\n";
				++$nonsh;
				$Full_T_Block =~ s/$Param_ORG/map$nonsh/;
				$MAP_Line = join( ' ', $MAP_Line, $PCount );
			    }
			    ++$PCount;
			}
			$NtotPar = $PCount;
		    }
		}

		# Finished preparing the FITPARAMETERS block

		# Start constructing RUN block here
		# For each defined range we need a block in the RUN-Block
		# Also for each histogram in Single Histograms fits

		$RUN = $RUNS[ $iRun - 1 ];

		#$DEBUG = $DEBUG.MSR::CreateRUNBlk($RUN, $YEAR, $BeamLine, $All{"FitAsyType"}, $All{"LRBF"}, $All{"Tis"}, $All{"Tfs"}, $All{"BINS"});

		if ($All{"RUNSType"}) {
		    $RUN_Line = MSR::RUNFileNameAuto($RUN,"0000",$EMPTY);
		} else {
		    $RUN_Line = MSR::RUNFileNameAuto($RUN,$YEAR,$BeamLine);
		}

		$Type_Line  = "fittype         0";
		$PLT        = 0;
		$Hist_Lines = "forward         HIST";
		$Bg_Line    = $EMPTY;
		$Data_Line  = "data";
		
		foreach ("t0","Bg1","Bg2","Data1","Data2") {
		    $Name = "$_$Hist";
		    # If empty fill with defaults
		    if ($All{$Name} eq $EMPTY) {
			$All{$Name}=MSR::T0BgData($_,$Hist,$BeamLine);
		    }
		}
		$Bg_Line = $Bg_Line."    ".$All{"Bg1$Hist"}."    ".$All{"Bg2$Hist"};
		$Data_Line =$Data_Line."    ".$All{"Data1$Hist"}."    ".$All{"Data2$Hist"};
		if ($All{"t0$Hist"} ne $EMPTY) {
		    $Data_Line=$Data_Line."\nt0       ".$All{"t0$Hist"};  
		}
	    
                $FRANGE_Line = "fit             TINI    TFIN";
                $PAC_Line    = "packing         BINNING";
                
                $Tmp_Hist_Line = $Hist_Lines;
                $Tmp_Hist_Line =~ s/HIST/$Hist/g;
                $Single_RUN = "$RUN_Line\n$NoBg_Line$Tmp_Hist_Line\n$Data_Line\n$MAP_Line\n";
                
                # Now add the appropriate values of fit range and packing
                # If there no multiple ranges/packing then simply use GLOBAL block
                if ($#TiVals == 0) {
                    $GLOBAL_Block = "GLOBAL\n$Type_Line\n$FRANGE_Line\n$PAC_Line\n\n";
                } else {
                    $GLOBAL_Block = "GLOBAL\n$Type_Line\n\n";
                    $Single_RUN = $Single_RUN."$FRANGE_Line\n$PAC_Line\n";
                }

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
                    $GLOBAL_Block =~ s/TINI/$Ti/g;
                    $GLOBAL_Block =~ s/TFIN/$Tf/g;
                    $GLOBAL_Block =~ s/BINNING/$BIN/g;
                
                    # For multiple ranges use this
                    if ( $Ti < $Range_Min ) { $Range_Min = $Ti; }
		    if ( $Tf > $Range_Max ) { $Range_Max = $Tf; }
		    $RUNS_Line = "$RUNS_Line " . $Range_Order;
		    ++$k;
		    ++$Range_Order;
		}
		++$iHist;
	    }
	}
        ++$iRun;
    }

    # The number of runs is
    $NRUNS = $iRun - 1;
    # Start constructing all blocks
    my $TitleLine = $All{"TITLE"}."\n# Run Numbers: ".$All{"RunNumbers"};

    # Get parameter block from MSR::PrepParamTable(\%All);
    my $FitParaBlk = "FITPARAMETER
###############################################################
#     No     Name       Value    Err     Min  Max                  ";
    my %PTable=MSR::PrepParamTable(\%All);
    my $NParam=scalar keys( %PTable );
# Fill the table with labels and values of parametr 
    for (my $iP=0;$iP<$NParam;$iP++) {
	my ($Param,$value,$error,$minvalue,$maxvalue,$RUNtmp) = split(/,/,$PTable{$iP});
	if ( $minvalue == $maxvalue ) {
	    $minvalue = $EMPTY;
	    $maxvalue = $EMPTY;
	}
	$PCount=$iP+1;
	$FitParaBlk = $FitParaBlk."
      $PCount      $Param    $value     $error    $error    $minvalue    $maxvalue";
    }

    $Full_T_Block = "THEORY\n$Full_T_Block\n";

    # COMMAND Block
    $COMMANDS_Block = "COMMANDS\nFITMINTYPE\nSAVE\n";
    $COMMANDS_Block =~ s/FITMINTYPE/$FITMINTYPE/g;
    # END- COMMAND Block

    # PLOT Block
    # Check if log x and log y are selected
    my $logxy = $EMPTY;
    if ( $All{"logx"} eq "y" ) { $logxy = $logxy . "logx\n"; }
    if ( $All{"logy"} eq "y" ) { $logxy = $logxy . "logy\n"; }
    # Optional - add lifetime correction for SingleHist fits
    if ( $All{"ltc"} eq "y" ) { $logxy = $logxy . "lifetimecorrection"; }

    # Check if a plot range is defined (i.e. different from fit)
    $PRANGE_Line = "use_fit_ranges";
    if ( $All{"Xi"} != $All{"Xf"} ) {
	if ($All{"Yi"} != $All{"Yf"}) {
	    $PRANGE_Line = "range  ".$All{"Xi"}."  ".$All{"Xf"}."  ".$All{"Yi"}."  ".$All{"Yf"};
	} else {
	    $PRANGE_Line = "range ".$All{"Xi"}." ".$All{"Xf"};
	}
    }

    $VIEWBIN_Line ="";
    if ( $All{"ViewBin"}!=0 ) { $VIEWBIN_Line = "view_packing ".$All{"ViewBin"};}

    my $RRFBlock=MSR::CreateRRFBlk(\%All);
    $PLOT_Block = "PLOT $PLT\n$logxy\nruns     $RUNS_Line\n$PRANGE_Line\n$VIEWBIN_Line\n$RRFBlock";
    #END - PLOT Block

    # FFT Block
    my $FOURIER_Block = MSR::CreateFFTBlk(\%All);
    # END - FFT Block

    # STATS Block
    # Don't know why but it is needed initially
    my $STAT_Block = "STATISTIC --- 0000-00-00 00:00:00\n*** FIT DID NOT CONVERGE ***";
    # END - STATS Block

    # Empty line at the end of each block
    my $FullMSRFile = "$TitleLine
###############################################################
$FitParaBlk
###############################################################
$Full_T_Block
###############################################################
$FUNCTIONS_Block
###############################################################
$GLOBAL_Block
###############################################################
$RUN_Block
###############################################################
$COMMANDS_Block
###############################################################
$PLOT_Block
###############################################################
$FOURIER_Block
###############################################################
$STAT_Block\n";

# Return information and file
    return($Full_T_Block,\@Paramcomp,$FullMSRFile);
}


sub CreateMSRGLB {
##########################################################################
# CreateMSRGLB
#
# Uses CreateMSR to produce a template and then msr2data to globalize it
#
# Input in %All
#
# Output
#  $Full_T_Block - Full theory block
#  @Paramcomp    - Space separated list of parameters for each component
#  FILENAME.msr  - MSR file saved
#
##########################################################################
    my %All = %{$_[0]};

    my @RUNS = ();
    if ($All{"RUNSType"}) {
# Make sure this globalization is disabled if RunFiles are used
	@RUNS = split( /,/, $All{"RunFiles"} );
# Make example from first run
	$All{"RunFiles"}=$RUNS[0];
    } else {
	@RUNS = split( /,/, MSR::ExpandRunNumbers($All{"RunNumbers"}) );
# Make example from first run
        $All{"RunNumbers"}=$RUNS[0];
    }

    my ($Full_T_Block,$Paramcomp_ref,$MSRTemplate)= MSR::CreateMSRUni(\%All);

# It is better to call msr2data here, but can we do that without writing to the disk? Not yet.
    
# Return as usual
    my @Paramcomp = @$Paramcomp_ref;

    return($Full_T_Block,\@Paramcomp,$MSRTemplate);
}

sub CreateTheory { 
########################
# CreateTheory
########################
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
                  "abragam",     "Del Lam",
                  "simpleGss",   "Sgm",
                  "statGssKT",   "Sgm",
                  "statGssKTLF", "Frqg Sgm",
		  "dynGssKTLF",  "Frql Sgm Lam",
		  "statExpKT",   "Lam",
		  "statExpKTLF", "Frq Aa",
		  "dynExpKTLF",  "Frq Aa Lam",
		  "combiLGKT",   "Del Sgm",
                  "spinGlass",   "Lam gam q",
                  "rdAnisoHf",   "Frq Lam",
                  "TFieldCos",   "Phi Frq",
                  "Bessel",      "Phi Frq",
                  "internFld",   "Alp Phi Frq LamT LamL",
                  "Bessel",      "Phi Frq",
                  "internBsl",   "Alp Phi Frq LamT LamL",
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
        
        # Oscillationg Gaussian
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
        
        # Oscillationg Abragam function
        elsif ( $FitType eq "AbragamCos" ) {
            $T_Block    = $T_Block . "\n" . "abragam " . $THEORY{'abragam'};
            $Parameters = join( $SPACE, $Parameters, $THEORY{'abragam'} );
            $T_Block    = $T_Block . "\n" . "TFieldCos " . $THEORY{'TFieldCos'};
            $Parameters = join( $SPACE, $Parameters, $THEORY{'TFieldCos'} );
        }
        
        # Oscillationg Bessel Gaussian
        elsif ( $FitType eq "GaussianBessel" ) {
            $T_Block    = $T_Block . "\n" . "simpleGss " . $THEORY{'simpleGss'};
            $Parameters = join( $SPACE, $Parameters, $THEORY{'simpleGss'} );
            $T_Block    = $T_Block . "\n" . "Bessel " . $THEORY{'Bessel'};
            $Parameters = join( $SPACE, $Parameters, $THEORY{'Bessel'} );
            }
        
        # Oscillationg Bessel Exponential
        elsif ( $FitType eq "ExponentialBessel" ) {
            $T_Block    = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
            $Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
            $T_Block    = $T_Block . "\n" . "Bessel " . $THEORY{'Bessel'};
            $Parameters = join( $SPACE, $Parameters, $THEORY{'Bessel'} );
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
	
	# Static Lorentzian or Gaussian KF ZF
	elsif ( $FitType eq "LGKT" ) {
	    $T_Block = $T_Block . "\n" . "combiLGKT " . $THEORY{'combiLGKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'combiLGKT'} );
	} 

	# Static Stretched KF ZF
	elsif ( $FitType eq "STRKT" ) {
	    $T_Block = $T_Block . "\n" . "strKT " . $THEORY{'strKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'strKT'} );
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
	
	# Lorentzian or Gaussian KT ZF multiplied by exponential
	elsif ( $FitType eq "LGKTExp" ) {
	    $T_Block = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
	    $T_Block =
		$T_Block . "\n" . "combiLGKT " . $THEORY{'combiLGKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'combiLGKT'} );
	}

	# Stretched KT ZF multiplied by exponential
	elsif ( $FitType eq "STRKTExp" ) {
	    $T_Block = $T_Block . "\n" . "simplExpo " . $THEORY{'simplExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'simplExpo'} );
	    $T_Block =
		$T_Block . "\n" . "strKT " . $THEORY{'strKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'strKT'} );
	}

	# Lorentzian or Gaussian KT ZF multiplied by stretched exponential
	elsif ( $FitType eq "LGKTSExp" ) {
	    $T_Block = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	    $T_Block =
		$T_Block . "\n" . "combiLGKT " . $THEORY{'combiLGKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'combiLGKT'} );
	}

	# Stretched KT ZF multiplied by stretched exponential
	elsif ( $FitType eq "STRKTSExp" ) {
	    $T_Block = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	    $T_Block =
		$T_Block . "\n" . "strKT " . $THEORY{'strKT'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'strKT'} );
	}

	# Lorentzian or Gaussian KT LF multiplied by stretched exponential
	elsif ( $FitType eq "MolMag" ) {
	    $T_Block = $T_Block . "\n" . "generExpo " . $THEORY{'generExpo'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'generExpo'} );
	    $T_Block =
		$T_Block . "\n" . "statExpKTLF " . $THEORY{'statExpKTLF'};
	    $Parameters = join( $SPACE, $Parameters, $THEORY{'statExpKTLF'} );
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
}

sub ExtractBlks {
########################
# ExtractBlks
########################
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
    my $line =$EMPTY;
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

    return(\@TBlock,\@FPBlock);
}


sub T0BgData { 
########################
# T0BgData
# Function return the defaul value of t0, Bg and Data bin
# input should be 
# $Name is t0,Bg1,Bg2,Data1,Data2
# $Hist is the histogram number
# $BeamLine in the name of beamline
########################
# Take this information as input arguments
    (my $Name, my $Hist, my $BeamLine) = @_;
    
# These are the default values, ordered by beamline
# Comma at the beginning means take default t0 from file
# The order is pairs of "HistNumber","t0,Bg1,Bg2,Data1,Data2"
    my %LEM=("1",",66000,66500,3419,63000",
	     "2",",66000,66500,3419,63000",
	     "3",",66000,66500,3419,63000",
	     "4",",66000,66500,3419,63000",
	     "5",",66000,66500,3419,63000",
	     "6",",66000,66500,3419,63000",
	     "7",",66000,66500,3419,63000",
	     "8",",66000,66500,3419,63000",
	     "9",",66000,66500,3419,63000",
	     "10",",66000,66500,3419,63000");
    
#    my %GPS=("1",",40,120,135,8000",
#	     "2",",40,120,135,8000",
#	     "3",",40,120,135,8000",
#	     "4",",40,120,135,8000");
    
#    my %Dolly=("1",",50,250,297,8000",
#	     "2",",50,250,297,8000",
#	     "3",",50,250,297,8000",
#	     "4",",50,250,297,8000");
    
    my %RV=();
     
# If multiple histograms (sum or difference) take the first histogram only
    my @Hists=split(/ /,$Hist);

    if ($BeamLine eq "LEM") {
	my $HistParams=$LEM{$Hists[0]};
	($RV{"t0"},$RV{"Bg1"},$RV{"Bg2"},$RV{"Data1"},$RV{"Data2"})=split(/,/,$HistParams);
    } 
    elsif ($BeamLine eq "Dolly") {
 	my $HistParams=$Dolly{$Hists[0]};
	($RV{"t0"},$RV{"Bg1"},$RV{"Bg2"},$RV{"Data1"},$RV{"Data2"})=split(/,/,$HistParams);
   }
    elsif ($BeamLine eq "GPS") {
        my $HistParams=$GPS{$Hists[0]};
        ($RV{"t0"},$RV{"Bg1"},$RV{"Bg2"},$RV{"Data1"},$RV{"Data2"})=split(/,/,$HistParams);
    } 
    elsif ($BeamLine eq "GPD") {
        my $HistParams=$GPD{$Hists[0]};
        ($RV{"t0"},$RV{"Bg1"},$RV{"Bg2"},$RV{"Data1"},$RV{"Data2"})=split(/,/,$HistParams);
    } 
    else {
        my $HistParams=$GPS{$Hists[0]};
        ($RV{"t0"},$RV{"Bg1"},$RV{"Bg2"},$RV{"Data1"},$RV{"Data2"})=split(/,/,$HistParams);
    } 
    return $RV{$Name};

}

sub PrepParamTable { 
########################
# PrepParamTable
# Function return a Hash with a table of parameters for the fit
# input should be 
# %All
# output is 
# hash of parameter table 
# %ParTable = PCount / $Param,$value,$error,$minvalue,$maxvalue,$RUN
########################
    my $erradd = "d";
    my $minadd = "_min";
    my $maxadd = "_max";
   
# First assume nothing is shared
    my $Shared = 0;

# Reset output Hash
    %ParTable = ();
 
    my %All = %{$_[0]};
    my @RUNS = ();
    if ($All{"RUNSType"}) {
	@RUNS = split( /,/, $All{"RunFiles"} );
    } else {
	# Remove spaces and other illegal characters
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	$All{"RunNumbers"} =~ s/[a-zA-Z]//g;
	# Expand hyphens and colons
	$All{"RunNumbers"} = MSR::ExpandRunNumbers($All{"RunNumbers"});
	# Split commas
	@RUNS=split( /,/, $All{"RunNumbers"});
    }
    my @Hists = split( /,/, $All{"LRBF"} );

    my @FitTypes = ();
    # loop over fitTypes
    if (!defined($All{"numComps"})) {$All{"numComps"}=3;}
    for (my $i=1;$i<=$All{"numComps"};$i++) {
        if ( defined($All{"FitType$i"}) &&$All{"FitType$i"} ne "None" ) {
            push( @FitTypes, $All{"FitType$i"} );           
        }
    }

# Get theory block to determine the size of the table 
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
# For now the line below does not work. Why?    
#    my $Paramcomp_ref=$All{"Paramcomp_ref"};
    my @Paramcomp = @$Paramcomp_ref;
    my $Full_T_Block= $All{"Full_T_Block"};
    my $PCount =0;
    my $iRun =0;
    my $value =0;
    my $error    = 0;
    my $minvalue = 0;
    my $maxvalue = 0;
    my $Component=1;
    
    foreach my $RUN (@RUNS) {
        $iRun++;
        $Component=1;
        if ($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB") {
            foreach my $FitType (@FitTypes) {
                my $Parameters=$Paramcomp[$Component-1];
                my @Params = split( /\s+/, $Parameters );               
		if ( $Component == 1 ) {
		    unshift( @Params, "Alpha" );
		}
		
		# This is the counter for parameters of this component
		my $NP=1;
		$Shared = 0;
		# Change state/label of parameters
		foreach my $Param (@Params) {
		    my $Param_ORG = $Param;
		    if ( $#FitTypes != 0 && ( $Param ne "Alpha" ) ){
			$Param = join( $EMPTY, $Param, "_", "$Component" );
		    }
		    
		    $Shared = $All{"Sh_$Param"};
		    if ( $Shared!=1 || $iRun == 1 ) {
			# It there are multiple runs index the
			# parameters accordingly
			my $RUNtmp = sprintf("%04d",$RUN);
			if ($All{"RUNSType"}) {
			    $RUNtmp = $iRun;
			}
			if ($Shared!=1) {$Param=$Param."_".$RUNtmp;}
			# Check if this parameter has been initialized
			# befor. If not take from defaults
			$value = $All{"$Param"};
			if ( $value ne $EMPTY ) {
			    $error    = $All{"$erradd$Param"};
			    $minvalue = $All{"$Param$minadd"};
			    $maxvalue = $All{"$Param$maxadd"};
			} else {
			    # I need this although it is already in
			    # the MSR.pm module, just for this table
			    # We can remove it from the MSR module
			    # later...  Or keep in the MSR as function
			    # ??

			    # We have two options here, either take
			    # default values or take values of
			    # previous run if available
			    
			    # $ParamPrev =~ s/$iRun-1/$iRun/g;
			    $value = MSR::Defaults{$Param_ORG};
			    $error = MSR::Defaults{ join( $EMPTY, $erradd, $Param_ORG ) };
			    $minvalue = MSR::Defaults{ join($EMPTY, $Param_ORG, $minadd ) };
			    $maxvalue = MSR::Defaults{ join($EMPTY, $Param_ORG, $maxadd ) };
			}
			$values=join(",",$Param,$value,$error,$minvalue,$maxvalue,$RUN);
			$ParTable{$PCount}=$values;
			$PCount++;
		    }
		    $NP++;
		}
                $Component++;
            }
        } else {
	    # For a single histogram fit we basically need to repeat
	    # this for each hist
	    my $iHist = 1;
            foreach my $Hist (@Hists) {
                $Component=1;
		foreach my $FitType (@FitTypes) {
		    my $Parameters=$Paramcomp[$Component-1];
		    my @Params = split( /\s+/, $Parameters );
		    # Only the first Hist gets new physical
		    # parameters, the rest only Phi if present
		    if ($iHist != 1 ) {
			# does @Param contains only Phi
			if (grep(/Phi/,@Params)) {
			    @Params=("Phi");
			} else {
			    @Params=();
			}
		    }	
		    if ( $Component == 1 ) {
			unshift( @Params, ( "No", "NBg" ) );
		    }
		
		    # This is the counter for parameters of this component
		    my $NP=1;
		    $Shared = 0;
		    # Change state/label of parameters
		    foreach my $Param (@Params) {
			my $Param_ORG = $Param;
			# If multiple histograms (sum or difference)
			# take the first histogram only
			($Hist,$tmp) = split(/ /,$Hist);
			# Tag with histogram number only No, NBg and Phi
			if ($Param_ORG =~ m/^(No|NBg|Phi)/ ) {$Param=$Param.$Hist;}
			if ( $#FitTypes != 0 && ( $Param_ORG ne "No" && $Param_ORG ne "NBg" ) ){
			    $Param = join( $EMPTY, $Param, "_", "$Component" );
			}
			
			$Shared = $All{"Sh_$Param"};
			if ( $Shared!=1 || $iRun == 1 ) {
			    # It there are multiple runs index the parameters accordingly
			    my $RUNtmp=sprintf("%04d",$RUN);
			    if ($Shared!=1) {$Param=$Param."_".$RUNtmp;}
			    # Check if this parameter has been
			    # initialized befor. If not take from
			    # defaults
			    $value = $All{"$Param"};
			    if ( $value ne $EMPTY ) {
				$error    = $All{"$erradd$Param"};
				$minvalue = $All{"$Param$minadd"};
				$maxvalue = $All{"$Param$maxadd"};
			    } else {
				# I need this although it is already
				# in the MSR.pm module, just for this
				# table We can remove it from the MSR
				# module later...  Or keep in the MSR
				# as function ??
				$value = MSR::Defaults{$Param_ORG};
				$error = MSR::Defaults{ join( $EMPTY, $erradd, $Param_ORG ) };
				$minvalue = MSR::Defaults{ join($EMPTY, $Param_ORG, $minadd ) };
				$maxvalue = MSR::Defaults{ join($EMPTY, $Param_ORG, $maxadd ) };
			    }
			    $values=join(",",$Param,$value,$error,$minvalue,$maxvalue,$RUN);
			    $ParTable{$PCount}=$values;
			    $PCount++;
			}
			$NP++;
		    }
		    $Component++;
		}
		++$iHist;
	    }
	}
    }
    return %ParTable;
}

sub ExportParams { 
########################
# ExportParams
# Function return a tab separated table of parameters for the fit
# input should be 
# %All
# output is
# $TABLE containing an ascii table of fit parameters
########################
    my $erradd = "d";
    my $minadd = "_min";
    my $maxadd = "_max";

    # First assume nothing is shared
    my $Shared = 0;

    my $TABLE=$EMPTY;
    my $HEADER="RUN";

    my %All = %{$_[0]};
    
    my @RUNS = ();
    if ($All{"RUNSType"}) {
	@RUNS = split( /,/, $All{"RunFiles"} );
    } else {
	# Remove spaces and other illegal characters
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	$All{"RunNumbers"} =~ s/[a-zA-Z]//g;
	# Expand hyphens and colons
	$All{"RunNumbers"} = MSR::ExpandRunNumbers($All{"RunNumbers"});
	# Split commas
	@RUNS=split( /,/, $All{"RunNumbers"});
    }
    my @Hists = split( /,/, $All{"LRBF"} );

    my @FitTypes = ();
    # loop over fitTypes
    if (!defined($All{"numComps"})) {$All{"numComps"}=3;}
    for (my $i=1;$i<=$All{"numComps"};$i++) {
        if ( defined($All{"FitType$i"}) &&$All{"FitType$i"} ne "None" ) {
            push( @FitTypes, $All{"FitType$i"} );           
        }
    }

    # Get theory block to determine the size of the table
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
    # For now the line below does not work. Why?    
    # my $Paramcomp_ref=$All{"Paramcomp_ref"};
    my @Paramcomp = @$Paramcomp_ref;
    my $Full_T_Block= $All{"Full_T_Block"};

    # Then loop over expected parameters and extract their values and
    # error bar
    my $PCount =0;
    my $iRun =0;
    my $value =0;
    my $error    = 0;
    my $minvalue = 0;
    my $maxvalue = 0;
    my $Component=1;
    
    foreach my $RUN (@RUNS) {
	my $line="$RUN";
	$iRun++;
	$Component=1;
	if ($All{"FitAsyType"} eq "Asymmetry") {
	    foreach my $FitType (@FitTypes) {
		my $Parameters=$Paramcomp[$Component-1];
		my @Params = split( /\s+/, $Parameters );		
		if ( $Component == 1 ) {
		    unshift( @Params, "Alpha" );
		}
		
		# This is the counter for parameters of this component
		my $NP=1;
		$Shared = 0;
		# Change state/label of parameters
		foreach my $Param (@Params) {
		    my $Param_ORG = $Param;
		    if ( $#FitTypes != 0 && ( $Param ne "Alpha" ) ){
			$Param = join( $EMPTY, $Param, "_", "$Component" );
		    }
		    # $All{"Header"} - 0/1 for with/without header
		    if ($All{"Header"} && $iRun == 1) {
			$HEADER=join("\t",$HEADER,$Param,"$erradd$Param");
		    }
		    
		    $Shared = $All{"Sh_$Param"};
		    if ( $Shared!=1 || $iRun == 1 ) {
			# If there are multiple runs index the
			# parameters accordingly
			my $RUNtmp=sprintf("%04d",$RUN);
			if ($Shared!=1) {$Param=$Param."_".$RUNtmp;}
			# Check if this parameter has been initialized
			# befor. (should be)
			$value = $All{"$Param"};
			$error = $All{"$erradd$Param"};

			$line=join("\t",$line,$value,$error);
			$PCount++;
		    }
		    elsif ($Shared==1) {
			# The parameter is shared, take the value from
			# the first run
			$Param=$Param;
			$value = $All{"$Param"};
			$error = $All{"$erradd$Param"};
			$line=join("\t",$line,$value,$error);
		    }
		    $NP++;
		}
		$Component++;
	    }
	} 
	elsif ($All{"FitAsyType"} eq "SingleHist" ) {
	    # For a single histogram fit we basically need to repeat
	    # this for each hist
	    foreach my $Hist (@Hists) {
		$Component=1;
		foreach my $FitType (@FitTypes) {
		    my $Parameters=$Paramcomp[$Component-1];
		    my @Params = split( /\s+/, $Parameters );		
		    if ( $Component == 1 ) {
			unshift( @Params, ( "No", "NBg" ) );
		    }
		
		    # This is the counter for parameters of this
		    # component
		    my $NP=1;
		    $Shared = 0;
		    # Change state/label of parameters
		    foreach my $Param (@Params) {
			my $Param_ORG = $Param;
			# Tag with histogram number only No, NBg and Phi
			if ($Param_ORG =~ m/^(No|NBg|Phi)/ ) {$Param=$Param.$Hist;}
			if ( $#FitTypes != 0 && ( $Param_ORG ne "No" && $Param_ORG ne "NBg" ) ){
			    $Param = join( $EMPTY, $Param, "_", "$Component" );
			}
			
			$Shared = $All{"Sh_$Param"};
			if ( $Shared!=1 || $iRun == 1 ) {
			    # If there are multiple runs index the
			    # parameters accordingly
			    my $RUNtmp=sprintf("%04d",$RUN);
			    if ($Shared!=1) {$Param=$Param."_".$RUNtmp;}
			    # Check if this parameter has been
			    # initialized befor. (should be)
			    $value = $All{"$Param"};
			    $error    = $All{"$erradd$Param"};
			    $minvalue = $All{"$Param$minadd"};
			    $maxvalue = $All{"$Param$maxadd"};

			    $values=join("\t",$Param,$value,$error,$minvalue,$maxvalue,$RUN);
			    $ParTable{$PCount}=$values;
			    $PCount++;
			}
			$NP++;
		    }
		    $Component++;
		}
	    }
	}
	$TABLE=$TABLE."$line\n"
    }
    if ($All{"Header"}) {
	$TABLE=$HEADER."\n".$TABLE;
    }

    return $TABLE;
}

sub MSR2Dat { 
########################
# MSR2Dat
# Function return a tab separated table of parameters from an MSR file
# input should be 
# @msrfile
########################
    # Take the msr file as input array of lines
    my @file=@_;

    # Extract PRAMETERS and THEORY Blocks
    (my $TBlock_ref, my $FPBlock_ref)=MSR::ExtractBlks(@file);
    my @FPBlock = @$FPBlock_ref;
    my @TBlock = @$TBlock_ref;

# Get shared parameters
    foreach $TLine (@TBlock) {
	# Then split it to find numbers of shared parameters
	@tmp=split(/\s+/,$TLine);
	foreach (@tmp) {
	    if ($_ eq $_+0 ) {
		# This is a number, keep it in the Shared arry
		@Shared=(@Shared,$_);
	    }
	}
    }

# Nice trick, make a hash for unique RUN lines
# Find spectrum lines
    my @MAPS = grep {/map /} @file;
    my @RUNS = grep {/RUN/} @file;
    my $counter=0;
    foreach $key (@RUNS){
	# This gets rid of duplicates
	$RUN{$key}=$counter;
	$MAP{$key}=$MAPS[$counter];
	$counter++;
    } 

# Number of runs (or independent sets of parameters) in this file
    my $NRuns=1;
    foreach (sort { $RUN{$a} <=> $RUN{$b}} keys %RUN ) {
	@RunParams=();
	$NP=0;
	@tmp=split(/\s+/,$MAP{$_});
	# Remove first element (map)
	shift(@tmp);
	foreach (@tmp) {
	    if ($_ ne $EMPTY && $_>0 ) {
		@RunParams=(@RunParams,$_);
		$NP++;
	    }
	}
	if ($NP>0) {
	    $orders=join(",",@RunParams);
	    $RUNParams[$NRuns]=$orders;
	    $NRuns++;
	}
    }
    
# Split parameter's line to extract values and errors
    foreach $line (@FPBlock) {
	@Param=split(/\s+/,$line);
	# Create a hash with the parameter order as a key
	# and the value and error as value
	$P{$Param[1]}=$Param[3].",".$Param[4];
	$PName{$Param[1]}=$Param[2];
    }

# Now we have everything. Lets start ordering
    # First lines is names
    @Pnum=split(/,/,$RUNParams[1]);
    foreach (@Pnum,@Shared) {
	$DatFile=join("\t",$DatFile,$PName{$_},"d".$PName{$_});
    }
    $DatFile=$DatFile."\n";
    
    # For the values from all the files.
    # I am not checking if all the files have the same theory function
    for ($i=1;$i<=$NRuns-1;$i++) {
	@Pnum=split(/,/,$RUNParams[$i]);
	# First go for the shared parameters
	foreach (@Pnum,@Shared) {
	    ($value,$err)=split(/,/,$P{$_});
	    $DatFile=join("\t",$DatFile,$value,$err);
	}
	$DatFile=$DatFile."\n";
    }
    return $DatFile;
}

sub RUNFileNameAuto { 
########################
# RUNFileNameAuto
# Function return the RUN_Line for a given RUN
# input should be 
# $RUN is the run number
# $YEAR is the year
# $BeamLine in the name of beamline
########################
# Take this information as input arguments
    (my $RUN, my $YEAR, my $BeamLine) = @_;

    my $RUN_Line=$EMPTY;
# if BeamLine is empty assume manual name
    if ($BeamLine eq $EMPTY) {
	my %EXTs = ("root","MUSR-ROOT",
		    "bin","PSI-BIN",
		    "msr","MUD");

# Take this information as input arguments
	(my $RUN) = @_;
	my $EXT = $EMPTY;
	my $key = $EMPTY;
# Find out the type of file (possibilities are root, bin and msr).
	foreach $key (keys %EXTs) {
	    if (index($RUN,$key) != -1) {
		$EXT = $key;
	    }
	}
	
	$RUN =~  s/\.[^.]+$//;

	$RUN_Line = join( $SPACE,
			     "RUN", $RUN, "MUE4", "PSI",$EXTs{$EXT});
    } else {
    my $DATADIR = $DATADIRS{$BeamLine};
    my $RUNtmp=sprintf("%04d",$RUN);
	    
    # Get current year
    my ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
	localtime( time() );
    my $current_year = $year + 1900;
    
    if ( $BeamLine eq "LEM" || $BeamLine eq "LEM (PPC)") {
	$RUN_File_Name = "lem" . substr( $YEAR, 2 ) . "_his_" . $RUNtmp;
	$RUNFILE       = "$DATADIR/d$YEAR/tdc/$RUN_File_Name";
    }
    elsif ( $BeamLine eq "GPS" ) {
	if ($YEAR >= 2008) {
	    $RUN_File_Name = "deltat_tdc_gps_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/tdc/$RUN_File_Name";
	} else {
	    $RUN_File_Name = "deltat_pta_gps_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/$RUN_File_Name";
	}
    }
    elsif ( $BeamLine eq "LTF" ) {
	if ($YEAR >= 2009) {
	    $RUN_File_Name = "deltat_tdc_ltf_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/tdc/$RUN_File_Name";
	} else {
	    $RUN_File_Name = "deltat_pta_ltf_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/$RUN_File_Name";
	}
    }
    elsif ( $BeamLine eq "Dolly" ) {
	if ($YEAR >= 2008) {
	    $RUN_File_Name = "deltat_tdc_dolly_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/tdc/$RUN_File_Name";
	} else {
	    $RUN_File_Name = "deltat_pta_dolly_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/$RUN_File_Name";
	}	    
    }
    elsif ( $BeamLine eq "GPD" ) {
	if ($YEAR >= 2008) {
	    $RUN_File_Name = "deltat_tdc_gpd_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/tdc/$RUN_File_Name";
	} else {
	    $RUN_File_Name = "deltat_pta_gpd_" . $RUNtmp;
	    $RUNFILE = "$DATADIR/d$YEAR/$RUN_File_Name";
	}  
    }
    elsif ( $BeamLine eq "HAL" ) {
	$RUNtmp=sprintf("%05d",$RUNtmp);
	$RUN_File_Name = "tdc_hifi_" . $YEAR . "_" . $RUNtmp;
	$RUNFILE = "$DATADIR/d$YEAR/tdc/$RUN_File_Name";
    }
	$RUN_Line = join( $SPACE,
			 "RUN", $RUNFILE, $BeamLines{$BeamLine}, "PSI",
			 $Def_Format{$BeamLine} );
}
    return $RUN_Line;
}

sub CreateRRFBlk {
########################
# CreateRRFBlk
########################
# This creates the RRF related lines, these are the same always

    my %All = %{$_[0]};

    $RRFBlock="";
    if ($All{"RRFFrq"}!= 0) {
	if ($All{"RRFPhase"} eq $EMPTY) {$All{"RRFPhase"}=0;}
	if ($All{"RRFPack"} eq $EMPTY) {$All{"RRFPack"}=1;}
	$RRFBlock="rrf_freq  ".$All{"RRFFrq"}."  ".$All{"RRFUnits"}."\n";
	$RRFBlock=$RRFBlock."rrf_phase  ".$All{"RRFPhase"}."\n";
	$RRFBlock=$RRFBlock."rrf_packing  ".$All{"RRFPack"}."\n";
    }
    return $RRFBlock;
}

sub ExtractInfo {
########################
# ExtractInfo
########################
# Used to extract information from data file header. The header (using
# "dump_header" is sent to this function from musrfit.cgi or MuSRFit
    my ($header,$Arg) = @_;

    my @lines = split(/\n/,$header);

    if ( $Arg eq "TITLE" ) {
        $RTRN_Val = $lines[3];
        $RTRN_Val =~ s/\n//g;
    }
    elsif ( $Arg eq "Temp" ) {
        foreach my $line (@lines) {
            if ( $line =~ /Temp/ ) {
                ( my $tmp, my $T )   = split( /=/,  $line );
                ( $T,   $tmp ) = split( /\(/, $T );
                $RTRN_Val = $T;
            }
        }

    }
    elsif ( $Arg eq "Field" ) {
        foreach my $line (@lines) {
            if ( $line =~ /Mean B field/ ) {
                ( $tmp, my $B )   = split( /=/,  $line );
                ( $B,   $tmp ) = split( /\(/, $B );
                $RTRN_Val = $B;
            }
        }
    }
    elsif ( $Arg eq "Energy" ) {
        foreach my $line (@lines) {
            if ( $line =~ /implantation energy/ ) {
                ( my $tmp1, my $tmp2, my $E ) = split( /=/, $line );
                ( $E, $tmp ) = split( /keV/, $E );
                $RTRN_Val = $E;
            }
        }

    }
    #	$RTRN_Val =~ s/[\.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]//g;
    return $RTRN_Val;
}

sub ExtractInfoLEM {
########################
# ExtractInfoLEM
########################
# Uset to extract information from summary files
    my ($RUN,$YEAR,$Arg) = @_;
# Use file header
    my $Summ_File_Name = "lem" . substr( $YEAR, 2 ) . "_" . $RUN . ".summ";
    my $SummFile = "$SUMM_DIR/$YEAR/$Summ_File_Name";

    open( SFILE,q{<}, "$SummFile" );
    my @lines = <SFILE>;
    close(SFILE);

    if ( $Arg eq "TITLE" ) {
        $RTRN_Val = $lines[3];
        $RTRN_Val =~ s/\n//g;
    }
    elsif ( $Arg eq "Temp" ) {
        foreach my $line (@lines) {
            if ( $line =~ /Mean Sample_CF1/ ) {
                ( my $tmp, my $T )   = split( /=/,  $line );
                ( $T,   $tmp ) = split( /\(/, $T );
                $RTRN_Val = $T;
            }
        }

    }
    elsif ( $Arg eq "Field" ) {
        foreach my $line (@lines) {
            if ( $line =~ /Mean B field/ ) {
                ( $tmp, my $B )   = split( /=/,  $line );
                ( $B,   $tmp ) = split( /\(/, $B );
                $RTRN_Val = $B;
            }
        }
    }
    elsif ( $Arg eq "Energy" ) {
        foreach my $line (@lines) {
            if ( $line =~ /implantation energy/ ) {
                ( my $tmp1, my $tmp2, my $E ) = split( /=/, $line );
                ( $E, $tmp ) = split( /keV/, $E );
                $RTRN_Val = $E;
            }
        }

    }
    #	$RTRN_Val =~ s/[\.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]//g;
    return $RTRN_Val;
}

sub ExtractInfoBulk {
# Uset to extract information from log files
    my ($RUN,$AREA,$YEAR,$Arg) = @_;
    my $RUNtmp=sprintf("%04d",$RUN);
    $RUN=$RUNtmp;

# Information may be found in these file
    my $DBFILE=$DBDIR{$AREA}.$YEAR."/*.runs";
    my @Lines =`cat $DBFILE`;
    
# Select intries with the right area
    my $area=lc $AREA;
    my @Lines1 = grep { /$area/ } @Lines;
    my @Lines2 = grep { /$AltArea{$AREA}/ } @Lines;
    @Lines=(@Lines1,@Lines2);
# Select intries with the right run number
    @Lines = grep { /$RUN/ } @Lines;
    @Words=split(/\s+/,$Lines[0]);

    if ( $Arg eq "TITLE" ) {
        $RTRN_Val = substr($Lines[0],104);
    }
    elsif ( $Arg eq "Temp" ) {
	$RTRN_Val = $Words[6];
    }
    elsif ( $Arg eq "Field" ) {
	$RTRN_Val = $Words[7];
    }
    
    return $RTRN_Val;
}

sub CreateRUNBlk {
# Subroutine to create a clean run block (Not finished or used yet)
# Take this information as input arguments
    my ($RUN,$YEAR,$BeamLine,$FitAsyType,$LRBF,$Ti,$Tf,$BIN) = @_;
# $RUN is the run number
# $YEAR is the corresponding year
# $BeamLine is the corresponding beam line
# $FitAsyType is SH or Asy
# $LRBF are the histogram numbers
# $Ti, $Tf and $BIN are the corresponding initial/final time range and binning factor respectively

    my $RUN_Block = 
"#RUN_LINE
#fittype  FIT_TYPE
#ALPHA_LINE
#HISTOLINES
#MAP_LINE
#T0LINE
#BGLINE
#DATALINE
#fit      TIVAL TFVAL
#packing  BINNING
";

    # HISTOLINES is the only thing that significantly changes between SH and Asy

    # Prepare the RUN_LINE
    my $RUN_Line = MSR::RUNFileNameAuto($RUN,$YEAR,$BeamLine);
    # Split the histograms
    my @Hists = split( /,/, $LRBF );
    $Fit_Type = 0;
    if ($FitAsyType eq "Asymmetry" || $FitAsyType eq "AsymmetryGLB") { 
        $Fit_Type = 2;
    }

    $RUN_Block =~ s/RUN_LINE/$RUN_Line/;
    $RUN_Block =~ s/FIT_TYPE/$Fit_Type/;
    $RUN_Block =~ s/TIVAL TFVAL/$Ti   $Tf/;
    $RUN_Block =~ s/BINNING/$BIN/;

    my $HistLines = $EMPTY;
    my $Run_Block_ORG = $RUN_Block;
    $RUN_Block = $EMPTY;
    if ($Fit_Type == 2) {
	# Asymmetry fit
	$HistLines = "forward         $Hists[0]\n#backward       $Hists[1]";
	$RUN_Block = $RUN_Block.$Run_Block_ORG."\n\n";
	$RUN_Block =~ s/HISTOLINES/$HistLines/;
    } else { 
	# Single histogram fit
	foreach my $Hist (@Hists) {
	    $HistLines = "forward $Hist";
	    # Create multiple blocks with the same lines except for the histogram line
	    $RUN_Block = $RUN_Block.$Run_Block_ORG."\n\n";
	    $RUN_Block =~ s/HISTOLINES/$HistLines/;
	}
    }
	    
    return $RUN_Block;

}

sub CreateFFTBlk {
    my %All = %{$_[0]};
    if ($All{"FUNITS"} eq $EMPTY) {$All{"FUNITS"}="MHz";}
    if ($All{"FAPODIZATION"} eq $EMPTY) {$All{"FAPODIZATION"}="STRONG";}
    if ($All{"FPLOT"} eq $EMPTY) {$All{"FPLOT"}="POWER";}
    if ($All{"FPHASE"} eq $EMPTY) {$All{"FPHASE"}="8.5";}
    my $FrqRange = $EMPTY;
    if ($All{"FRQMAX"} ne $EMPTY && $All{"FRQMIN"} ne $EMPTY && $All{"FRQMAX"} ne $All{"FRQMIN"}) {
	$FrqRange = "range ".$All{"FRQMIN"}."  ".$All{"FRQMAX"};
    }

    my $FOURIER_Block="FOURIER
units            FUNITS    # units either 'Gauss', 'MHz', or 'Mc/s'
fourier_power    12
apodization      FAPODIZATION  # NONE, WEAK, MEDIUM, STRONG
plot             FPLOT   # REAL, IMAG, REAL_AND_IMAG, POWER, PHASE
phase            FPHASE
FRQRANGE";

    $FOURIER_Block=~ s/FUNITS/$All{"FUNITS"}/g;
    $FOURIER_Block=~ s/FAPODIZATION/$All{"FAPODIZATION"}/g;
    $FOURIER_Block=~ s/FPLOT/$All{"FPLOT"}/g;
    $FOURIER_Block=~ s/FPHASE/$All{"FPHASE"}/g;
    $FOURIER_Block=~ s/FRQRANGE/$FrqRange/g;

    return $FOURIER_Block;
}

sub ExpandRunNumbers {
# Subroutine to conver RunNumbers string into comma separated array
# following the msr2data notations; hyphens denote from-to and a colon 
# denotes a loop with steps from:to:step
    my ($RunNumbers) = @_;
    # Split by commas
    @RUNS=split( /,/, $RunNumbers);
    # Find array members that include a hyphen (-)
    my @RUNseries = grep { /\-/ } @RUNS;
    foreach my $tmp (@RUNseries) {
	# Each item that includes a hyphen is expanded as from-to array
	my ($Begin,$End)=split('-',$tmp);
	my @Series = ($Begin..$End);
	# Join the expanded series using commas
	my $RunJoined = join(',',@Series);
        # Replace the original hyphens by the expanded series
        $RunNumbers =~ s/$tmp/$RunJoined/;
    }
    # Find array members that include a colon (:)
    my @RUNLoops = grep { /\:/ } @RUNS;
    foreach my $tmp (@RUNLoops) {
        # Each item that includes a colon is expanded as from:to:step array
        my ($Begin,$End,$Step)=split(':',$tmp);
        if ($Begin>$End) {
            # Swap $Begin and $End
            my $tmpBegin = $Begin;
            $Begin = $End;
            $End = $tmpBegin;
        }
        my @Series = ();
        for (my $i = $Begin; $i<=$End; $i += $Step) {
            push(@Series,$i);
        }
        # Join the expanded series using commas
        my $RunJoined = join(',',@Series);
        # Replace the original hyphens by the expanded series
        $RunNumbers =~ s/$tmp/$RunJoined/;
    }
    return $RunNumbers;
}

sub Defaults { 
########################
# Defaults
# Function return a default value of parameter
# input should be 
# $Parameter
# output is 
# $value
########################
# Take this information as input arguments
# "Smart" default value of the fit parameters.
    my ($Param)=@_;

    my %Defaults = (
    "Asy",           "0.20",  "dAsy",          "0.01",
    "Asy_min",       "0",     "Asy_max",       "0",
    "Alpha",         "1.0",   "dAlpha",        "0.01",
    "Alpha_min",     "0",     "Alpha_max",     "0",
    "No",            "300.0", "dNo",           "0.01",
    "No_min",        "0",     "No_max",        "0",
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
    "Aa",            "60.",   "dAa",           "0.01",
    "Aa_min",        "0",     "Aa_max",        "0",
    "q",             "0.1",   "dq",            "0.01",
    "q_min",         "0",     "q_max",         "0",
    "Bg",            "0.036", "dBg",           "0.01",
    "Bg_min",        "0",     "Bg_max",        "0",
    "bgrlx",         "0.",    "dbgrlx",        "0.0",
    "bgrlx_min",     "0",     "bgrlx_max",     "0",
    "Frq",           "1.0",   "dFrq",          "1.",
    "Frq_min",       "0",     "Frq_max",       "0",
    "Frql",          "1.0",   "dFrql",          "1.",
    "Frql_min",      "0",     "Frql_max",       "0",
    "Frqg",          "1.0",   "dFrqg",          "1.",
    "Frqg_min",      "0",     "Frqg_max",       "0",
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

    return $Defaults{$Param};
}

1;
