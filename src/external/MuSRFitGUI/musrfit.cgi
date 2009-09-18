#!/usr/bin/perl
#
# Copyright Zaher Salman and PSI LEM Group 2009.
# zaher.salman@psi.ch

######## Relevant subdirectories and urls ########
# May need modification on different servers
$WEBDIR   = "/~l_salman";
$REALDIR  = "/home/l_salman/public_html";
$BINDIR   = "/usr/local/bin";
$MAINPAGE = "/cgi-bin/musrfit.cgi";

$OUTPUT      = $WEBDIR . "/msrfit_out";
$OUTPUT_REAL = $REALDIR . "/msrfit_out";

$SUMM_DIR = "/mnt/data/nemu/summ";

# Programs used
if ( $TITLE eq $EMPTY || $TITLE eq "TITLE" ) {
    $MUSRFIT = $BINDIR . "/musrfit -t";
}
else {
    $MUSRFIT = $BINDIR . "/musrfit";
}
$MUSRVIEW  = $BINDIR . "/musrview --png";
$DUMPASCII = "$MUSRFIT --dump ascii";
$MSR2DAT   = $BINDIR . "/msr2dat";
#######################################################################

# Nothing need to be changed from here on
$EMPTY=q{};
$SPACE=q{ };
$COMMA=q{,};

# Additions to paremeters' names
$erradd = "d";
$minadd = "_min";
$maxadd = "_max";

%BeamLines = ( "LEM", "MUE4", "GPS", "PIM3", "LTF", "PIM3", "Dolly", "PIE1" , "GPD", "PIE1");

# Commands used often
$cln_out = "rm -f $OUTPUT_REAL/FILENAME*";
$cln     = "rm -f MINUIT2.* $OUTPUT_REAL/FILENAME* FILENAME.*";
$plotonly =
"$MUSRVIEW FILENAME.msr >> $OUTPUT_REAL/FILENAME.out; convert FILENAME\_0.png -crop 590x460+1+50 $OUTPUT_REAL/FILENAME.png; rm -f FILENAME\_0.png";
$fitplot =
"$MUSRFIT FILENAME.msr > $OUTPUT_REAL/FILENAME.out; $MUSRVIEW FILENAME.msr >> $OUTPUT_REAL/FILENAME.out; convert FILENAME\_0.png -crop 590x460+1+50 $OUTPUT_REAL/FILENAME.png; rm -f FILENAME\_0.png";
$dumpascii =
"$DUMPASCII FILENAME.msr > $OUTPUT_REAL/FILENAME_ascii.out; /bin/tar -cvzf $OUTPUT_REAL/FILENAME.tgz FILENAME*.dat >> $OUTPUT_REAL/FILENAME_ascii.out; rm -f FILENAME*.dat";
# TODO: Replace this with a call to $TABLE=MSR::ExportParams
$AllParameters{"Header"}=1;
$TABLE=MSR::ExportParams(\%AllParameters);
$msr2dat = "$MSR2DAT < FILENAME.msr > $OUTPUT_REAL/FILENAME_par.dat";

# "Smart" default value of the fit parameters.
%Defaults = (
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

#######################################################################
######## Main body of code ########
use CGI;
use MSR;

my $in = new CGI;



#######################################################################
# Common to all steps
# If you are coming from a previous round pass in various parameters
PassIn();

# This is the content of the html page..
$Content = $EMPTY;

# This is just for temporary debugging html text
$TC = $EMPTY;

# Put all input values in a hash
@All = $in->param();
$N   = $#All;
$i      = 0;
while ( $i <= $N ) {
    $One = $All[$i];
    $AllParameters{ $One } = $in->param($One);
#    $TC=$TC."$One=".$AllParameters{ $One }."<br>";
    ++$i;
}

# See which step or command we are coming from
# Step Value             Command
#
#         -1             CLEAR/START NEW
#          1             Defin Shared
#          2             Initialize parameters
#          3             PLOT
#          4             MIGRAD
#          5             MINOS
#          6             SIMPLEX
#######################################################################
$Step        = $in->param("go");
$FITMINTTYPE = "PLOT";
if ( $Step eq "PLOT" ) {
    $Step       = 3;
    $FITMINTYPE = $EMPTY;
}
elsif ( $Step eq "MIGRAD" || $Step eq "FIT") {
    $Step       = 4;
    $FITMINTYPE = "MINIMIZE\nMIGRAD\nHESSE";
}
elsif ( $Step eq "MINOS" ) {
    $Step       = 5;
    $FITMINTYPE = "MIGRAD\nMINOS";
}
elsif ( $Step eq "SIMPLEX" ) {
    $Step       = 6;
    $FITMINTYPE = "SCAN\nSIMPLEX";
}
elsif ( $Step eq "Restart" ) {
    $Step = -1;
}


if ( $All{"Minimization"} ne "" &&  $All{"ErrorCalc"} ne "" && $Step ne "PLOT" ) {
    $FITMINTYPE = $All{"Minimization"}."\n".$All{"ErrorCalc"};
}



#$TC=$TC."OUT:<br>".MSR::Test(\%AllParameters);
#$tmp="Frq";
#$TC=$TC."Sh_Frq= ".$AllParameters{"Sh_$tmp"}."<br>";

# Skip the shared parameters step if you have only one
if ( $Step == 1 && $#RUNS == 0 ) { $Step = 2; }

#######################################################################
# Choose shared parameters if you have multiple runs
#######################################################################
if ( $Step == 1 ) {

    # First create the THEORY block to get a parameters list
    ($Full_T_Block,$Paramcomp_ref)=MSR::CreateTheory(@FitTypes);
    @Paramcomp = @$Paramcomp_ref;
    StepShared();
}
#######################################################################

#######################################################################
# Initialize/Fix parameters values
#######################################################################
elsif ( $Step == 2 ) {
    $RUN = $RUNS[0];
    if    ( $RUN < 10 )   { $RUN = "000" . $RUN; }
    elsif ( $RUN < 100 )  { $RUN = "00" . $RUN; }
    elsif ( $RUN < 1000 ) { $RUN = "0" . $RUN; }

    # First create the THEORY block to get a parameters list
    ($Full_T_Block,$Paramcomp_ref)=MSR::CreateTheory(@FitTypes);
    @Paramcomp = @$Paramcomp_ref;
    if ( $BeamLine eq "LEM" ) {

        # Take the temperature from data file if you can
        ExtractInfo( $RUN, $YEAR, "Temp" );
        $TEMPERATURE = $RTRN_Val;
    }

    # If TITLE is empty or eq TITLE get it from the first run if you can
    if ( $TITLE eq $EMPTY || $TITLE eq "TITLE" ) {

        # This is a flag to keep setting TITLE to default
        $T_Keep = 1;
    }
    else {
        $T_Keep = 0;
    }

    # If the filename is empty or eq RUN put default
    if ( $FILENAME eq $EMPTY || $FILENAME eq "RUN" || $FILENAME eq "0" ) {

        # Flag to keep default filename
        $FN_Keep = 1;
        if ( $RUNS[0] ne $EMPTY ) {
            ( $TMP1, $TMP2, $BLine ) = split( /\//, $BeamLine );
            $FILENAME = $RUNS[0] . "_" . $BeamLine . "_" . $YEAR;
        }
    }
    else {
        $FN_Keep = 0;
    }

# If the file already exists do not continue running. The file may be
# used by someone else.
# TODO: Maybe create a different name and not die.
#    if (-e "$FILENAME.msr") {
#	$Content = $Content."The file name <b>\"$FILENAME\"</b> is being used, choose another name or <a href=\"$MAINPAGE?go=-1\">delete it</a>!";
#	&SpitHTML;
#	die;
#    }
    InitFixStep();
}
#######################################################################

#######################################################################
# Create musrfit .msr file and fit/plot the result
#######################################################################
elsif ( $Step <= 6 && $Step >= 3 ) {

    # Then create appropriate file
    ($Full_T_Block,$Paramcomp_ref)= MSR::CreateMSR(\%AllParameters);
    @Paramcomp = @$Paramcomp_ref;

    if ( $Step == 3 ) {
        $cmd = $plotonly;
    }
    elsif ( $Step <=6 && $Step >= 4 ) {
        $cmd = $fitplot;
    }

    # Fit and plot results
    $cmd =~ s/FILENAME/$FILENAME/g;
    $tmp = system("$cmd");

    # Generate ascii data and fit
    $cmd = $dumpascii;
    $cmd =~ s/FILENAME/$FILENAME/g;
    $tmp = system("$cmd");

    # Generate parameters table
    $cmd = $msr2dat;
    $cmd =~ s/FILENAME/$FILENAME/g;
    $tmp = system("$cmd");

    $Spectra = ( $#RUNS + 1 ) * ( $#TiVals + 1 );
    if ( $RRF != 0 && $Spectra == 1 ) { $Spectra = 2; }

    PrintOut();
}
else {
    if ( $Step == -1 ) {
	
        # Clean up
        $cmd = $cln;
        $cmd =~ s/FILENAME/$FILENAME/g;
        $tmp = system("$cmd");

        # Check flag for default
        $T_Keep = $in->param("T_Keep");
        if ( $T_Keep == 1 ) {
            $TITLE = "TITLE";
        }

        # Check flag for default
        $FN_Keep = $in->param("FN_Keep");
        if ( $FN_Keep == 1 ) { $FILENAME = "RUN"; }
        $Step    = 0;
        $Content = $EMPTY;
    }

    # Take title, runnumber, ti, tf, bin ...
    Step0();
}

# Insert passed on variables into the HTML code
PassOn();
SpitHTML();
exit(0);

######## Split functions and subroutines ########

########################
#sub SpitHTML
#
#This function prints the start of the feedback form web page.
########################
sub SpitHTML {
    print("Content-type: text/html; charset=iso-8859-1  \n\n");
    print(
        "<HTML>
<HEAD>
<TITLE>$TITLE</TITLE>
<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; no-cache; charset=iso-8859-8-I\">
</HEAD>
<BODY BGCOLOR=\"$BGCOLOR\" BACKGROUND=\"$BACKGROUND_URL\" TEXT=\"$TEXT_COLOR_NORMAL\">
$TC
$Content
<BR><HR>
If you have any problems please send me an <a href=\"mailto:zaher.salmam\@psi.ch\">email</a> about it <br>
</BODY>
</HTML>
"
    );
    return;
}
########################

########################
# Step 0
#
# Is the begining of the fitting pocess
# Take title and run numbers and the type of fitting function
########################
sub Step0 {

    # Prepare body of the HTML code
    $Content = $Content . "
Enter the label (default is run title)<br>
<br>
<form enctype=\"multipart/form-data\" action=\"$MAINPAGE\" method=\"post\">
<input type=\"text\" name=\"TITLE\" value=\"$TITLE\" size=\"60\">
<input type=\"hidden\" name=\"T_Keep\" value=\"$T_Keep\">
<input type=\"hidden\" name=\"TEMPERATURE\" value=\"$TEMPERATURE\">
<input type=\"hidden\" name=\"logx\" value=\"$logx\">
<input type=\"hidden\" name=\"logy\" value=\"$logy\">
<input type=\"hidden\" name=\"ltc\" value=\"$ltc\">
<br>
<br>
Enter [name] for output [name].msr file (optional) 
<input type=\"text\" name=\"FILENAME\" value=\"$FILENAME\">
<input type=\"hidden\" name=\"FN_Keep\" value=\"$FN_Keep\"><br>
<br>
Input the RUN numbers separated with commas<br>
<br>
<input type=\"text\" name=\"RunNumbers\" value=\"$RunNumbers\">  
On beam line 
<select name=\"BeamLine\">
INSERT_BEAMLINES_HERE
</select>
Year  
<select name=\"YEAR\">
INSERT_YEARS_HERE
</select>
<br>
<br>
Chose type of fitting function:<br>
<br>

                             
<table cellpadding=\"2\" cellspacing=\"2\" border=\"1\" width=\"20%\">
<tbody>
<tr>
<td> First Component </td><td> Second Component</td><td> Third Component</td>
</tr>
<tr>
<td>
<select name=\"FitType1\">
<option $FitSel1{\"Background\"} value=\"Background\">Constant Background</option>
<option $FitSel1{\"Exponential\"} value=\"Exponential\">Exponential</option>
<option $FitSel1{\"Gaussian\"} value=\"Gaussian\">Gaussian</option>
<option $FitSel1{\"Stretch\"} value=\"Stretch\">Stretch Exponential</option>
<option $FitSel1{\"ExponentialCos\"} value=\"ExponentialCos\">Exponential Cosine</option>
<option $FitSel1{\"GaussianCos\"} value=\"GaussianCos\">Gaussian Cosine</option>
<option $FitSel1{\"StretchCos\"} value=\"StretchCos\">Stretch Cosine</option>
<option $FitSel1{\"LDKTLF\"} value=\"LDKTLF\">Lorentzian Dynamic Kubo-Toyabe LF</option>
<option $FitSel1{\"GDKTLF\"} value=\"GDKTLF\">Gaussian Dynamic Kubo-Toyabe LF</option>
<option $FitSel1{\"LLFExp\"} value=\"LLFExp\">Lorentzian Kubo-Toyabe LF x Exp</option>
<option $FitSel1{\"GLFExp\"} value=\"GLFExp\">Gaussian Kubo-Toyabe LF x Exp</option>
<option $FitSel1{\"LLFSExp\"} value=\"LLFSExp\">Lorentzian Kubo-Toyabe LF x Str Exp</option>
<option $FitSel1{\"GLFSExp\"} value=\"GLFSExp\">Gaussian Kubo-Toyabe LF x Str Exp</option>
<option $FitSel1{\"Meissner\"} value=\"Meissner\">Meissner State Model</option>
<!option value=\"GaussianKTZF\">Gaussian Kubo-Toyabe ZF</option>
<!option value=\"GaussianKTLF\">Gaussian Kubo-Toyabe LF</option>
<!option value=\"DynamicKTZF\">Dynamic Kubo-Toyabe ZF</option>
</select>
</td>
<td>
<select name=\"FitType2\">
<option $FitSel2{\"None\"} value=\"None\">None</option>
<option $FitSel2{\"Background\"} value=\"Background\">Constant Background</option>
<option $FitSel2{\"Exponential\"} value=\"Exponential\">Exponential</option>
<option $FitSel2{\"Gaussian\"} value=\"Gaussian\">Gaussian</option>
<option $FitSel2{\"Stretch\"} value=\"Stretch\">Stretch Exponential</option>
<option $FitSel2{\"ExponentialCos\"} value=\"ExponentialCos\">Exponential Cosine</option>
<option $FitSel2{\"GaussianCos\"} value=\"GaussianCos\">Gaussian Cosine</option>
<option $FitSel2{\"StretchCos\"} value=\"StretchCos\">Stretch Cosine</option>
<option $FitSel2{\"LDKTLF\"} value=\"LDKTLF\">Lorentzian Dynamic Kubo-Toyabe LF</option>
<option $FitSel2{\"GDKTLF\"} value=\"GDKTLF\">Gaussian Dynamic Kubo-Toyabe LF</option>
<option $FitSel2{\"LLFExp\"} value=\"LLFExp\">Lorentzian Kubo-Toyabe LF x Exp</option>
<option $FitSel2{\"GLFExp\"} value=\"GLFExp\">Gaussian Kubo-Toyabe LF x Exp</option>
<option $FitSel2{\"LLFSExp\"} value=\"LLFSExp\">Lorentzian Kubo-Toyabe LF x Str Exp</option>
<option $FitSel2{\"GLFSExp\"} value=\"GLFSExp\">Gaussian Kubo-Toyabe LF x Str Exp</option>
<option $FitSel2{\"Meissner\"} value=\"Meissner\">Meissner State Model</option>
<!option value=\"GaussianKTZF\">Gaussian Kubo-Toyabe ZF</option>
<!option value=\"GaussianKTLF\">Gaussian Kubo-Toyabe LF</option>
<!option value=\"DynamicKTZF\">Dynamic Kubo-Toyabe ZF</option>
<!option value=\"DynamicKTLF\">Dynamic Kubo-Toyabe LF</option>
</select>
</td>
<td>
<select name=\"FitType3\">
<option $FitSel3{\"None\"} value=\"None\">None</option>
<option $FitSel3{\"Background\"} value=\"Background\">Constant Background</option>
<option $FitSel3{\"Exponential\"} value=\"Exponential\">Exponential</option>
<option $FitSel3{\"Gaussian\"} value=\"Gaussian\">Gaussian</option>
<option $FitSel3{\"Stretch\"} value=\"Stretch\">Stretch Exponential</option>
<option $FitSel3{\"ExponentialCos\"} value=\"ExponentialCos\">Exponential Cosine</option>
<option $FitSel3{\"GaussianCos\"} value=\"GaussianCos\">Gaussian Cosine</option>
<option $FitSel3{\"StretchCos\"} value=\"StretchCos\">Stretch Cosine</option>
<option $FitSel3{\"LDKTLF\"} value=\"LDKTLF\">Lorentzian Dynamic Kubo-Toyabe LF</option>
<option $FitSel3{\"GDKTLF\"} value=\"GDKTLF\">Gaussian Dynamic Kubo-Toyabe LF</option>
<option $FitSel3{\"LLFExp\"} value=\"LLFExp\">Lorentzian Kubo-Toyabe LF x Exp</option>
<option $FitSel3{\"GLFExp\"} value=\"GLFExp\">Gaussian Kubo-Toyabe LF x Exp</option>
<option $FitSel3{\"LLFSExp\"} value=\"LLFSExp\">Lorentzian Kubo-Toyabe LF x Str Exp</option>
<option $FitSel3{\"GLFSExp\"} value=\"GLFSExp\">Gaussian Kubo-Toyabe LF x Str Exp</option>
<option $FitSel3{\"Meissner\"} value=\"Meissner\">Meissner State Model</option>
<!option value=\"GaussianKTZF\">Gaussian Kubo-Toyabe ZF</option>
<!option value=\"GaussianKTLF\">Gaussian Kubo-Toyabe LF</option>
<!option value=\"DynamicKTZF\">Dynamic Kubo-Toyabe ZF</option>
<!option value=\"DynamicKTLF\">Dynamic Kubo-Toyabe LF</option>
</select>
</td>
</tr>
</tbody>
</table>
<br>
<br>
<table cellpadding=\"2\" cellspacing=\"2\" border=\"1\" width=\"20%\">
<tbody>
<tr>
<td>
From time t<sub>i</sub>=<br>\[&mu;sec\]<input type=\"text\" name=\"Tis\" value=\"$Tis\">
</td>
<td>
To time t<sub>f</sub>=<br>\[&mu;sec\]<input type=\"text\" name=\"Tfs\" value=\"$Tfs\">
</td>
<td>
Binning factor=<br>[points]<input type=\"text\" name=\"BINS\" value=\"$BINS\">
</td>
</tr>
</tbody>
</table>
<br>
Fit type <select name=\"FitAsyType\">
<option $FitAsyType{\"Asymmetry\"} value=\"Asymmetry\">Asymmetry</option>
<option $FitAsyType{\"SingleHist\"} value=\"SingleHist\">Single Histogram</option>
<!option $FitAsyType{\"RotRF\"} value=\"RotRF\">Rotating Reference Frame</option>
</select> 
Histograms: <input type=\"text\" name=\"LRBF\" value=\"$LRBF\">
<br>
<br>
<!RRF Frequency in MHz (Default 0, no RRF) = <input type=\"text\" name=\"RRF\" value=\"$RRF\">
<br>
PASS_ON_HTML_CODE
<input type=\"hidden\" name=\"go\" value=\"1\"><br>
<input type=\"reset\" value=\"Reset\"><input type=\"submit\" value=\"Next>\"></form>
";

    # Get current year
    ( $sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst ) =
      localtime( time() );
    $year  = $year + 1900;
    $Years = $EMPTY;

    # Creat a drop down menu for years from 1994 until current
    for ( $i = $year ; $i >= 1994 ; $i-- ) {
#    for ($i=1994..$year) {
        $Years =
	    $Years . "     <option $YEARSel{\"$i\"} value=\"$i\">$i</option>";
    }

    for my $BeamLine ( keys %BeamLines ) {
        $BeamLineDDM = $BeamLineDDM
          . "<option $BeamSel{\"$BeamLine\"} value=\"$BeamLine\">$BeamLine</option>\n";
    }

    # Insert the years into the HTML code
    $Content =~ s/INSERT_YEARS_HERE/$Years/g;

    # Insert the beamlines into the HTML code
    $Content =~ s/INSERT_BEAMLINES_HERE/$BeamLineDDM/g;
    return;
}
########################

########################
# StepShared
#
# Choose shared parameters if applicable
#
########################
sub StepShared {

    # Here we need first a list of parameters which depends
    # on the functions chosen and number of components

    $Content = $Content . "
Choose the shared parameters<br>
<form enctype=\"multipart/form-data\" action=\"$MAINPAGE\"
 method=\"post\"><br>
INSERT_PARAMETERS_TABLE_HERE
PASS_ON_HTML_CODE
<input type=\"hidden\" name=\"go\" value=\"2\"><br>
<input type=\"reset\" value=\"Reset\">
<input type=\"submit\" value=\"Next>\">
</form>
";

    # Prepare table of parameters
    $component   = 0;
    $Param_Table = $EMPTY;
    foreach my $FitType (@FitTypes) {
        ++$component;
        $Parameters  = $Paramcomp[ $component - 1 ];
        $Param_Table = $Param_Table . "
Component $component: $FitType
<table cellpadding=\"2\" cellspacing=\"2\" border=\"1\" width=\"20%\">
<tbody>
<tr>
<td style=\"vertical-align: top; text-align: center;\"><b><i>Parameter</i></b><br>
</td>
<td style=\"vertical-align: top; text-align: center;\"><b><i>Share</i></b><br>
</td>
</tr>";

        @Params = split( /\s+/, $Parameters );

        # For the first component we need Alpha for Asymmetry fits
        if ( $component == 1 && $FitAsyType eq "Asymmetry" ) {
            unshift( @Params, "Alpha" );
        }
        elsif ( $component == 1 && $FitAsyType eq "SingleHist" ) {
            unshift( @Params, ( "N0", "NBg" ) );
        }

        foreach my $Param (@Params) {
            if ( ($#FitTypes != 0)
                && ( $Param ne "Alpha" && $Param ne "N0" && $Param ne "NBg" ) )
            {
                $Param = join( $EMPTY, $Param, "_", "$component");
            }
            $ischecked = $in->param("Sh_$Param");
            if ($ischecked) {
                $checked = "checked";
            }
            else {
                $checked = $EMPTY;
            }
            $Param_Table = $Param_Table . "
<tr>
<td style=\"vertical-align: top; text-align: center;\">$Param<br>
</td>
<td style=\"vertical-align: top; text-align: center;\"><INPUT TYPE=\"checkbox\" NAME=\"Sh_$Param\" VALUE=\"1\" $checked><br>
</td>
</tr>
";
        }
        $Param_Table = $Param_Table . "
</tbody>
</table>
<hr>
";
    }

    # Insert the table of parameters into the HTML code
    $Content =~ s/INSERT_PARAMETERS_TABLE_HERE/$Param_Table/g;
    return;
}
########################

########################
# InitFixStep
########################
sub InitFixStep {

    $Content = $Content . "
<b><i>Initialize parameters:</i></b><br>
<form enctype=\"multipart/form-data\" action=\"$MAINPAGE\" method=\"post\">
<input type=\"hidden\" name=\"go\" value=\"3\"><br>
<input type=\"reset\" value=\"Reset\">
<input type=\"submit\" value=\"Next>\">
INSERT_PARAMETERS_TABLE_HERE
PASS_ON_HTML_CODE
<input type=\"reset\" value=\"Reset\">
<input type=\"submit\" value=\"Next>\">
</form>";

    # Counter for runs
    $Param_Table = $EMPTY;
    $i           = 0;
    foreach my $RUN (@RUNS) {
        ++$i;
        $Param_Table = $Param_Table . "<hr>Run Number: $RUN<br>";
        $component   = 0;
        foreach my $FitType (@FitTypes) {
            ++$component;

            $Parameters = $Paramcomp[ $component - 1 ];
            @Params = split( /\s+/, $Parameters );

            # For the first component we need Alpha for Asymmetry fits
            if ( $component == 1 && $FitAsyType eq "Asymmetry" ) {
                unshift( @Params, "Alpha" );
            }
            elsif ( $component == 1 && $FitAsyType eq "SingleHist" ) {
                unshift( @Params, ( "N0", "NBg" ) );
            }
            $Param_Table = $Param_Table . "
<u>Component $component: $FitType</u>
<table cellpadding=\"2\" cellspacing=\"2\" border=\"2\" style=\"text-align: left; width: 50%;\">
<tbody>
<tr bgcolor=\"#FFFF99\">
<td valign=\"top\">Param<br>
</td>
<td valign=\"top\">Value<br>
</td>
<td valign=\"top\">Error (0 for fixed)<br>
</td>
<td valign=\"top\">Min Value<br>
</td>
<td valign=\"top\">Max Value<br>
</td>
</tr>
<tr>";
            foreach my $Param (@Params) {
                $Param_ORG = $Param;
                if ( $#FitTypes != 0
                    && (   $Param ne "Alpha"
                        && $Param ne "N0"
                        && $Param ne "NBg" ) )
                {
                    $Param = join( $EMPTY, $Param, "_", "$component" );
                }
                if ( $#RUNS == 0 ) {
                    $Shared = 1;
                }
                else {
                    $Shared = $in->param("Sh_$Param");
                }
                if ( ( $Shared && $i == 1 ) || !$Shared ) {
                    $value = $in->param("$Param\_$i");
                    if ( $value ne $EMPTY ) {
                        $error    = $in->param("$erradd$Param\_$i");
                        $minvalue = $in->param("$Param\_$i$minadd");
                        $maxvalue = $in->param("$Param\_$i$maxadd");
                    }
                    else {
                        $value = $Defaults{$Param_ORG};
                        $error = $Defaults{ join( $EMPTY, $erradd, $Param_ORG ) };
                        $minvalue =
                          $Defaults{ join( $EMPTY, $Param_ORG, $minadd ) };
                        $maxvalue =
                          $Defaults{ join( $EMPTY, $Param_ORG, $maxadd ) };
                    }
                    $Param_Table = $Param_Table . "
<tr>
<td valign=\"top\" bgcolor=\"#99CCFF\">$Param\_$i</td>
<td valign=\"top\"><input type=\"text\" value=\"$value\" name=\"$Param\_$i\"></td>
<td valign=\"top\"><input type=\"text\" value=\"$error\" name=\"$erradd$Param\_$i\"></td>
<td valign=\"top\"><input type=\"text\" value=\"$minvalue\" name=\"$Param\_$i$minadd\"></td>
<td valign=\"top\"><input type=\"text\" value=\"$maxvalue\" name=\"$Param\_$i$maxadd\"></td>
</tr>";
                }
            }
            $Param_Table = $Param_Table . "
</tbody>
</table>
<br>";
        }
    }

    # Insert the table of parameters into the HTML code
    $Content =~ s/INSERT_PARAMETERS_TABLE_HERE/$Param_Table/g;
    return;
}
########################

########################
# PrintOut
#
# Make nice plot and preset values of fit parameters nicely
########################
sub PrintOut {

    $Content = $Content . "
<center>
<form enctype=\"multipart/form-data\" action=\"$MAINPAGE\" method=\"post\">
<table cellpadding=\"2\" cellspacing=\"2\" border=\"2\" >
<tbody>
<tr>
<td colspan=\"7\" align=\"middle\" bgcolor=\"#E0E0E0\">
<font color=\"#FF0000\"><i>
$TITLE
</i></font>
</td>
</tr>
<tr>
<td colspan=\"1\" valign=\"top\" style=\"width: 90px\">
<table cellspacing=\"2\" border=\"2\">
<tbody>
 <tr>
  <td>
   <select name=\"Minimization\" style=\"width: 90px\">
    <option $MinSel{\"MIGRAD\"} value=\"MIGRAD\">MIGRAD</option>
    <option $MinSel{\"MINIMIZE\"} value=\"MINIMIZE\">MINIMIZE</option>
    <option $MinSel{\"SIMPLEX\"} value=\"SIMPLEX\">SIMPLEX</option>
   </select>
  </td>
 </tr>
 <tr>
  <td>
   <select name=\"ErrorCalc\" style=\"width: 90px\">
    <option $ErrSel{\"HESSE\"} value=\"HESSE\">HESSE</option>
    <option $ErrSel{\"MINOS\"} value=\"MINOS\">MINOS</option>
   </select>
  </td>
 </tr>
 <tr>
  <td>
   <input type=\"submit\" name=\"go\" value=\"FIT\" style=\"width: 90px\">
  </td>
 </tr>
</tbody>
</table>
<td colspan=\"5\">
<center>
<table cellspacing=\"2\" border=\"2\" bgcolor=\"#E0E0E0\">
<tbody>
 <tr>
  <img src=\"$OUTPUT/$FILENAME.png\" width=\"400\" align=middle>
 </tr>
 <tr>
  <td colspan=\"3\">
   RUN numbers separated with commas<br>
   <input type=\"text\" name=\"RunNumbers\" value=\"$RunNumbers\"  size=\"40\">
  </td>
 </tr>
 <tr>
  <td>
   t<sub>i</sub>= <input type=\"text\" name=\"Tis\" value=\"$Tis\"  size=\"10\">
  </td>
  <td>
   t<sub>f</sub>= <input type=\"text\" name=\"Tfs\" value=\"$Tfs\" size=\"10\">
  </td>
  <td>
   Bin= <input type=\"text\" name=\"BINS\" value=\"$BINS\" size=\"10\">
  </td>
 </tr>
</tbody>
</table>
</center>
</td>
<td valign=\"top\"  style=\"width: 90px\">
<center>
<table cellspacing=\"2\" border=\"2\"  bgcolor=\"#E0E0E0\">
<tbody>
<tr>
<td style=\"width: 90px\">
   Log x <input type=\"checkbox\" name=\"logx\" value=\"y\" $logxcheck>
</td>
</tr>
<tr>
<td>
   Log y <input type=\"checkbox\" name=\"logy\" value=\"y\" $logycheck>
</td>
</tr>
<tr>
<td>
   &tau; cor. <input type=\"checkbox\" name=\"ltc\" value=\"y\" $ltccheck>
</td>
</tr>
<tr>
<td>
X,Y range<br>
Xi= <input type=\"text\" name=\"Xi\" value=\"$Xi\" size=\"3\"> <br>
Xf= <input type=\"text\" name=\"Xf\" value=\"$Xf\" size=\"3\"> <br>
Yi= <input type=\"text\" name=\"Yi\" value=\"$Yi\" size=\"3\"> <br>
Yf= <input type=\"text\" name=\"Yf\" value=\"$Yf\" size=\"3\"> <br>
</td>
</tr>
 <tr>
  <td>
   <input type=\"submit\" name=\"go\" value=\"PLOT\" style=\"width: 90px\">
  </td>
 </tr>
 <tr>
  <td>
   <input type=\"submit\" name=\"go\" value=\"Restart\" style=\"width: 90px\">
  </td>
 </tr>
</tbody>
</table>
</center>
</td>
</tr>
<tr>
<td colspan=\"7\">
<font color=\"#FF0000\"><i>
INSERT_CHI_LINE_HERE
</i></font>
</td>
</tr>
<tr bgcolor=\"#FFFF99\">
<td></td><td style=\"width: 90px\">RUN \#</td><td style=\"width: 90px\">Name</td><td style=\"width: 90px\">Value</td><td style=\"width: 90px\">Error</td><td style=\"width: 90px\">Min</td><td style=\"width: 90px\">Max</td>
</tr>
INSERT_PARAMETERS_TABLE_HERE
</tbody></table>
</td>
</tr>
</tbody>
</table>
PASS_ON_HTML_CODE
</form>
</center>
<hr>Download <a href=\"$OUTPUT/$FILENAME.msr\">$FILENAME.msr</a>.
<br>Download ascii files <a href=\"$OUTPUT/$FILENAME.tgz\">$FILENAME.tgz</a>.
<br>Download parameters table file <a href=\"$OUTPUT/$FILENAME\_par.dat\">$FILENAME\_par.dat</a>.
";

    # Initialize the extra pass on parameters
    system("cp -f $FILENAME.* $OUTPUT_REAL/.; chmod -R a+r $OUTPUT_REAL");

    # Reset the values of the TITLE and FILENAME if default was chosen
    if ( $T_Keep == 1 ) {
        $TITLE = "TITLE";
    }
    if ( $FN_Keep == 1 ) {
        $FILENAME = "RUN";
    }

    # Check log x and/or log y by default depending on previous setting
    if ( $logx eq "y" ) {
        $logxcheck = "checked";
    }
    else {
        $logxcheck = $EMPTY;
    }
    if ( $logy eq "y" ) {
        $logycheck = "checked";
    }
    else {
        $logycheck = $EMPTY;
    }

    # Check lifetime correction by default depending on previous settings
    if ( $ltc eq "y" ) {
        $ltccheck = "checked";
    }
    else {
        $ltccheck = $EMPTY;
    }

    # Reset log x and log y flags
    $logx = "n";
    $logy = "n";
    $ltc  = "n";

    $Param_Table = $EMPTY;
    open( IFILE,q{<}, "$FILENAME.msr" );
    @lines = <IFILE>;
    close(IFILE);

    # Get Chi line and reformat
    @Chi_Line = grep { /chisq/ } @lines;
    if ( $TITLE eq $EMPTY || $TITLE eq "TITLE" ) { $TITLE = $lines[0]; }
    if ( $Chi_Line[0] eq $EMPTY ) { $Chi_Line[0] = "Fit did not converge yet"; }

# Remove comment lines
    @lines = grep {!/^\#/} @lines;
# Remove empty lines
    @lines = grep {/\S/} @lines;

# Identify different blocks
    $i=0;
    foreach $line (@lines)
    {
	if (grep {/FITPARAMETER/} $line) {$NFITPARAMETERS=$i;}
	if (grep {/THEORY/} $line) { $NTHEORY=$i;} 
	if ((grep {/RUN/} $line) & $NRUN==0) { $NRUN=$i;} 
	$i++;
    }
    @FPBlock=@lines[$NFITPARAMETERS+1..$NTHEORY-1];
    @TBlock=@lines[$NTHEORY+1..$NRUN-1];

    foreach my $line (@FPBlock) {
# TODO: Better treatement for FPBlock is to remove lines starting with # or empty.
	$line =~ s/\n//g;
	
	(
	 $tmp, $order, $Param, $value, $error, $errorp, $minvalue,
	 $maxvalue
	 ) = split(/\s+/,$line);
#	$TC=$TC.$line."<br>";
#	$TC=$TC."$tmp,$order,$Param,$value,$error,$errorp,$minvalue,$maxvalue<br>";
# Take the number/order of the spectrum
	@itmp = split( /\_/, $Param );
		
	# If it is a multiple fit take also the run number
	if ( $itmp[1] ne $EMPTY ) {
	    if ( $RUNS[ $itmp[1] - 1 ] ne $RUN ) {
		$RUN  = $RUNS[ $itmp[1] - 1 ];
		$PRUN = $RUN;
	    }
	    else {
		$PRUN = $SPACE;
	    }
	}
		
	# Keep the values from the fit for the next round..
	$errParam = $erradd . $Param;
	$minParam = $Param . $minadd;
	$maxParam = $Param . $maxadd;
	if ( $value eq $EMPTY ) {
	    
	    # Take original defaults
	    ( $Param_ORG, $tmp ) = split( '_', $Param );
	    $value    = $Defaults{$Param_ORG};
	    $error    = $Defaults{ join( $EMPTY, $erradd, $Param_ORG ) };
	    $minvalue = $Defaults{ join( $EMPTY, $Param_ORG, $minadd ) };
	    $maxvalue = $Defaults{ join( $EMPTY, $Param_ORG, $maxadd ) };
	}
	else {
	    
	    # This is the trick to make an expression from the names :)
	    $$Param    = $value;
	    $$errParam = $error;
	    $$minParam = $minvalue;
	    $$maxParam = $maxvalue;
	}
		
	#	    $TC=$TC."$order,$Param,$value,$error,$errorp,$minvalue,$maxvalue<br>";
	
	$Param_Table = $Param_Table . "
<tr>
<td>$order</td>
<td>$PRUN</td>
<td bgcolor=\"#99CCFF\">$Param</td>
<td><input type=\"text\" name=\"$Param\" value=\"$value\" size=\"14\"></td>
<td><input type=\"text\" name=\"$erradd$Param\" value=\"$error\" size=\"14\"></td>
<td><input type=\"text\" name=\"$Param$minadd\" value=\"$minvalue\" size=\"11\"></td>
<td><input type=\"text\" name=\"$Param$maxadd\" value=\"$maxvalue\" size=\"11\"></td>
</tr>
";
    }
    $Content =~ s/INSERT_PARAMETERS_TABLE_HERE/$Param_Table/g;
    $Content =~ s/INSERT_CHI_LINE_HERE/$Chi_Line[0]/g;
    return;
}    #PrintOut
########################

########################
#This subroutine adds the parameters to pass on
########################

sub PassOn {
    @All = $in->param();
    $N   = $#All;

    # For the initialize parameters step do not pass the parameters themselves

    if ( $in->param("go") == 2 ) {
        for ($i=0..$N) {
            $find = substr( $All[$i], 0, 5 );
            if ( $find eq "Alpha" || $find eq "N0" || $find eq "NBg" ) {
                $tmp = $i;
                $i   = $N;
                $N   = $tmp - 1;
            }
        }
    }
    $PassOn = $EMPTY;
    $i      = 0;
    while ( $i <= $N ) {
        $One = $All[$i];
        if ( $One ne $EMPTY && $One ne "go" ) {

            #	    if ($Step == 1 && substr($One,0,3) ne "Sh\_") {
            if ( $$One ne $EMPTY ) {
                $OneValue = $$One;
            }
            else {
                $OneValue = $in->param($One);
                if ( $OneValue == 0 && $One ne "TITLE" ) { $OneValue = 0; }
            }

            # Exclude logx logy ltx
            #		if ($One ne "ltc" && $One ne "logx" && $One ne "logy") {
            $PassOn = $PassOn
              . "<input type=\"hidden\" name=\"$One\" value=\"$OneValue\">\n";

            #		}
            #	    }
            #	    }
        }
        ++$i;
    }
    $Content =~ s/PASS_ON_HTML_CODE/$PassOn/g;
    return;
}

########################
#This subroutine takes the parameters to pass in
########################
sub PassIn {
    $RunNumbers = $in->param("RunNumbers");
    $RunNumbers =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
    @RUNS = split( /,/, $RunNumbers );

    #    $TC=$TC."<br>RunNumbers=".$RunNumbers."   \@RUNS=".$RUNS[0]."<br>";

    $YEAR = $in->param("YEAR");
    %YEARSel = ( "$YEAR", "selected=\"selected\"" );

    $BeamLine = $in->param("BeamLine");
    if ( $BeamLine eq "LEM" ) {
        $LEMSel = "selected=\"selected\"";
    }
    elsif ( $BeamLine eq "GPS" ) {
        $GPSSel = "selected=\"selected\"";
    }
    elsif ( $BeamLine eq "LTF" ) {
        $LTFSel = "selected=\"selected\"";
    }
    elsif ( $BeamLine eq "GPD" ) {
        $GPDSel = "selected=\"selected\"";
    }
    else {
        $BeamLine eq "Dolly";
        $DollySel = "selected=\"selected\"";
    }

    # Take the values which were passed on...
    $TITLE       = $in->param("TITLE");
    $TEMPERATURE = $in->param("TEMPERATURE");

    $FILENAME = $in->param("FILENAME");
    $FILENAME =~ s/[\.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]//g;
    $FILENAME =~ s/ /_/g;

    $BeamLine = $in->param("BeamLine");
    if ( $BeamLine eq $EMPTY ) {
        %BeamSel = ( "LEM", "selected=\"selected\"" );
    }
    else {
        %BeamSel = ( "$BeamLine", "selected=\"selected\"" );
    }

    $Minimization = $in->param("Minimization");
    if ( $Minimization eq $EMPTY ) {
	%MinSel = ("MIGRAD", "selected=\"selected\"" );
    }
    else {
	%MinSel = ( "$Minimization", "selected=\"selected\"" );
    }

    $ErrorCalc = $in->param("ErrorCalc");
    if ( $ErrorCalc eq $EMPTY ) {
	%ErrSel = ("HESSE", "selected=\"selected\"" );
    }
    else {
	%ErrSel = ( "$ErrorCalc", "selected=\"selected\"" );
    }

    $FitType1 = $in->param("FitType1");
    $FitType2 = $in->param("FitType2");
    $FitType3 = $in->param("FitType3");
    if ( $FitType1 eq $EMPTY ) {
        %FitSel1 = ( "ExponentialCos", "selected=\"selected\"" );
    }
    else {
        %FitSel1 = ( "$FitType1", "selected=\"selected\"" );
    }
    if ( $FitType2 eq $EMPTY ) {
        %FitSel2 = ( "None", "selected=\"selected\"" );
    }
    else {
        %FitSel2 = ( "$FitType2", "selected=\"selected\"" );
    }
    if ( $FitType3 eq $EMPTY ) {
        %FitSel3 = ( "None", "selected=\"selected\"" );
    }
    else {
        %FitSel3 = ( "$FitType3", "selected=\"selected\"" );
    }

    foreach my $FitType ( $FitType1, $FitType2, $FitType3 ) {
        if ( $FitType ne "None" ) { push( @FitTypes, $FitType ); }
    }
    $BINS = $in->param("BINS");
    $Tis  = $in->param("Tis");
    $Tfs  = $in->param("Tfs");

    $Xi = $in->param("Xi");
    $Xf = $in->param("Xf");
    $Yi = $in->param("Yi");
    $Yf = $in->param("Yf");

    # Initail value if empty
    if ( $BINS eq $EMPTY ) { $BINS = 100; }
    if ( $Tis  eq $EMPTY ) { $Tis  = 0; }
    if ( $Tfs  eq $EMPTY ) { $Tfs  = 8; }

    $BINS =~ s/[\ \~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
    $Tis  =~ s/[\ \~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
    $Tfs  =~ s/[\ \~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;

    @BINVals = split( /,/, $BINS );
    @TiVals  = split( /,/, $Tis );
    @TfVals  = split( /,/, $Tfs );

    $j = 0;
    foreach my $Ti (@TiVals) {

        #Check conditions on numbers
        #    $BINS = int($BINS);
        $BIN = $BINVals[$j];
        $Tf  = $TfVals[$j];
        if ( $BIN <= 0 ) { $BINVals[$j] = 100; }
        if ( $Ti > $Tf ) {
            $tmp        = $Tf;
            $TfVals[$j] = $Ti;
            $TiVals[$j] = $tmp;
        }
        if ( $Ti < 0  || $Ti > 13 ) { $TiVals[$j] = 0.0; }
        if ( $Tf <= 0 || $Tf > 13 ) { $TfVals[$j] = 10; }

        $LRBF = $in->param("LRBF");
        if ( $LRBF eq $EMPTY ) {
            $LRBF = "1,3";
        }
        @Hists = split( /,/, $LRBF );

        $RRF = $in->param("RRF");
        ++$j;
    }
    $BINS = join( $COMMA, @BINVals );
    $Tis  = join( $COMMA, @TiVals );
    $Tfs  = join( $COMMA, @TfVals );

    $logx = $in->param("logx");
    $logy = $in->param("logy");
    $ltc  = $in->param("ltc");

    # Initail value if empty
    if ( $logx ne "y" ) {
        $logx = "n";
    }
    else {
        $logxcheck = "checked";
    }
    if ( $logy ne "y" ) {
        $logy = "n";
    }
    else {
        $logycheck = "checked";
    }
    if ( $ltc ne "y" ) {
        $ltc = "n";
    }
    else {
        $ltccheck = "checked";
    }

    # Fit type: Asymmetry, Single Histogram, or Rotatting Reference Frame
    $FitAsyType = $in->param("FitAsyType");
    if ( $FitAsyType eq $EMPTY ) {
        %FitAsyType = ( "Asymmetry", "selected=\"selected\"" );
    }
    else {
        %FitAsyType = ( "$FitAsyType", "selected=\"selected\"" );
    }

    return;
}

########################
# ExtractInfo
########################
# Uset to extract information from summary files
sub ExtractInfo {
    my ($RUN,$YEAR,$Arg) = @_;
    my $Summ_File_Name = "lem" . substr( $YEAR, 2 ) . "_" . $RUN . ".summ";
    my $SummFile = "$SUMM_DIR/$YEAR/$Summ_File_Name";

    open( SFILE,q{<}, "$SummFile" );
    my @lines = <SFILE>;
    close(SFILE);

    if ( $Arg eq "TITLE" ) {
        $RTRN_Val = $lines[3];
        $RTRN_Val =~ s/\n//g;

        #	$RTRN_Val =~ s/[\.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]//g;
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
    return;
}

# Uset to extract information from log files
sub ExtractInfoBulk {
    my ($RUN,$YEAR,$Arg) = @_;

    my $Summ_File_Name = "lem" . substr( $YEAR, 2 ) . "_" . $RUN . ".summ";
    my $SummFile = "$SUMM_DIR/$YEAR/$Summ_File_Name";

    open( SFILE,q{<}, "$SummFile" );
    @lines = <SFILE>;
    close(SFILE);

    if ( $Arg eq "TITLE" ) {
        $RTRN_Val = $lines[3];
        $RTRN_Val =~ s/\n//g;

        #	$RTRN_Val =~ s/[\.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]//g;
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
    return;
}

