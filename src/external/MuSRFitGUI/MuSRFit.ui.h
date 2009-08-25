/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

# This file is part of MuSRFitGUI.
#
#   MuSRFitGUI is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   MuSRFitGUI is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with MuSRFitGUI.  If not, see <http://www.gnu.org/licenses/>.
#
# Copyright 2009 by Zaher Salman and the LEM Group.
# <zaher.salman@psi.ch>

void Form1::fileNew()
{
}

void Form1::fileOpen()
{
    my $file=Qt::FileDialog::getOpenFileName(
	    ".",
	    "MSR Files (*.msr *.mlog)",
	    this,
	    "open file dialog",
	    "Choose a MSR file");
    print "Selected file: $file\n";
# TODO: Possibly decipher the MSR file and setup the GUI accordingly
# Find run numbers, beamlines etc.
# Get theory block and understand it
# Get parameters list and update table
    
}


void Form1::fileSave()
{
    my $file=Qt::FileDialog::getSaveFileName(
	    "",
	    "MSR Files (*.msr *.mlog)",
	    this,
	    "save file dialog",
	    "Choose a filename to save under");
    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"}.".msr";
    if (-e $FILENAME) {
# TODO: check if the extension is correct, or add it.
	my $cmd="cp $FILENAME $file";
	my $pid=system($cmd);
    } else {
	if ($file ne "") {
	    my $Warning = "Warning: No MSR file found yet!";
	    my $WarningWindow = Qt::MessageBox::information( this, "Warning",$Warning);
	}
    }    
}

void MuSRFitform::fileChangeDir()
{
    my $newdir=Qt::FileDialog::getExistingDirectory(
	    "",
	    this,
	    "get existing directory",
	    "Choose a directory",
	    1);
   chdir ("$newdir");
}

void Form1::filePrint()
{
    
}


void Form1::fileExit()
{
    Qt::Application::exit( 0 );
}


void Form1::editUndo()
{
    
}


void Form1::editRedo()
{
    
}


void Form1::editCut()
{
    
}


void Form1::editCopy()
{
    
}


void Form1::editPaste()
{
    
}


void Form1::helpIndex()
{
    
}


void Form1::helpContents()
{
    
}


void Form1::helpAbout()
{
        my $AboutText="
This is a GUI that uses the musrfit binary, developed by Andreas Suter,
to fit muSR spectra. 

MuSRFitGUI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MuSRFitGUI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MuSRFitGUI.  If not, see <http://www.gnu.org/licenses/>.

Copyright 2009 by Zaher Salman and the LEM Group.
<zaher.salman\@psi.ch>
";
    my $AboutWindow = Qt::MessageBox::information( this, "About MuSRFit GUI",$AboutText);
}

void MuSRFitform::CreateAllInput()
{
# TODO: Need to deliver shared parameters also
    my %All=();
    $All{"TITLE"}= TITLE->text;
    $All{"FILENAME"}= FILENAME->text;
    $All{"RunNumbers"} = RunNumbers->text;
    $All{"BeamLine"} = BeamLine->currentText;
    $All{"YEAR"} =YEAR->currentText;
    $All{"Tis"} = Tis->text;
    $All{"Tfs"} = Tfs->text;
    $All{"BINS"} = BINS->text;
    $All{"FitAsyType"} = FitAsyType->currentText;
    $All{"LRBF"} = LRBF->text;
    $All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
    my @RUNS = split( /,/, $All{"RunNumbers"} );
    
# Construct fittypes that can be understood by MSR.pm
    my %FTs=(0,"Exponential",
	     1,"Gaussian",
	     2,"Stretch",
	     3,"ExponentialCos",
	     4,"GaussianCos",
	     5,"StretchCos",
	     6,"LDKTLF",
	     7,"GDKTLF",
	     8,"Background",
	     9,"LLFExp",
	     10,"GLFExp",
	     11,"LLFSExp",
	     12,"GLFSExp",
	     13,"Meissner",
	     14,"None"
	     );
    
    my $FT1=FitType1->currentItem;
    my $FT2=FitType2->currentItem;
    my $FT3=FitType3->currentItem;
    $All{"FitType1"} = $FTs{$FT1};
    $All{"FitType2"} = $FTs{$FT2};
    $All{"FitType3"} = $FTs{$FT3};
    my @FitTypes =();
    my $FitType="";
    foreach $FitType ($All{"FitType1"}, $All{"FitType2"}, $All{"FitType3"}) {
	if ( $FitType ne "None" ) {
	    push( @FitTypes, $FitType );	    
	}
    }
    
# Also theory block and paramets list
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
    $All{"Full_T_Block"}=$Full_T_Block;    
    $All{"Paramcomp_ref"}=$Paramcomp_ref;
    my @Paramcomp = @$Paramcomp_ref;
    
# TODO: Read initial values of paramets from tabel
    my $erradd = "d";
    my $minadd = "_min";
    my $maxadd = "_max";
    my $NRows = InitParamTable->numRows();
    my $Header=InitParamTable->verticalHeader();
    if ($NRows > 0) {
	for (my $i=0;$i<$NRows;$i++) {
# Take label of row, i.e. name of parameter
	    my $Param=$Header->label($i);
# Then take the value, error, max and min (as numbers)
	    $All{"$Param"}=1.0*InitParamTable->text($i,0);
	    $All{"$erradd$Param"}=1.0*InitParamTable->text($i,1);
	    $All{"$Param$minadd"}=1.0*InitParamTable->text($i,2);
	    $All{"$Param$maxadd"}=1.0*InitParamTable->text($i,3);
	}
    }

    
# Shared settings are detected here
    my $Shared = 0; 
    my $PCount =0;
    my $Component=1;
    foreach $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );
	
	if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
	    unshift( @Params, "Alpha" );
	}	
	elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    unshift( @Params, ( "N0", "NBg" ) );
	}
	    
# This is the counter for parameters of this component
	my $NP=1;
	$Shared = 0;
# Change state/label of parameters
	foreach my $Param (@Params) {
	    my $Param_ORG = $Param;
	    if ( $#FitTypes != 0 && (   $Param ne "Alpha" && $Param ne "N0" && $Param ne "NBg" ) ){
		$Param = join( "", $Param, "_", $Component);
	    }
		
# Is there any point of sharing, multiple runs?
	    if ( $#RUNS == 0 ) {
		$Shared = 1;
	    } else {	
# Check if shared or not, construct name of checkbox, find its handle and then 
# check if it is checked		    
		my $ChkName="ShParam_".$Component."_".$NP;
		my $ChkBx = child($ChkName);
		$Shared = $ChkBx->isChecked();
	    }
	    $All{"Sh_$Param"}=$Shared;
	    $NP++;
	}#Loop on parameters
                $Component++;
      }# Loop on components
# Done with shared parameters detecting

# Construct a default filename if empty
    if ( $All{"FILENAME"} eq "") {
	$All{"FILENAME"}=$RUNS[0]."_".$All{"BeamLine"}."_".$All{"YEAR"};
    }

    if ( $All{"go"} eq "" ) {
	$All{"go"}="PLOT";
    }

# Get minimization process
    my $Min = Minimization->selectedId();
    if ($Min==0) {
	$All{"Minimization"}= "MINIMIZE";
	$All{"go"}="MIGRAD";
    }
    elsif ($Min==1) {
	$All{"Minimization"}= "MIGRAD";
	$All{"go"}="MIGRAD";
    }
    elsif ($Min==2) {
	$All{"Minimization"}= "SIMPLEX";
	$All{"go"}="SIMPLAEX";
    }
    
# Get Error calculation process
    my $Err = ErrorCalc->selectedId();
     if ($Err==0) {
	$All{"ErrorCalc"}= "HESSE";
#	$All{"go"}="MIGRAD";	
    }
    elsif ($Err==1) {
	$All{"ErrorCalc"}= "MINOS";
	$All{"go"}="MINOS";
    }
    
# Return Hash with all important values
    return %All;  
}

void MuSRFitform::CallMSRCreate()
{
    use MSR;
    my %All=CreateAllInput();
    if ($All{"RunNumbers"} ne "") {
	my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateMSR(\%All);
	UpdateMSRFileInitTable();
    }
}

void MuSRFitform::UpdateMSRFileInitTable()
{
    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"};
    open (MSRF,q{<},"$FILENAME.msr" );
    my @lines = <MSRF>;
    close(IFILE);
    textMSROutput->setText("");
    foreach my $line (@lines) {
	textMSROutput->append("$line");
    }
       
    my $FPBlock_ref=MSR::ExtractParamBlk(@lines);
    my @FPBloc = @$FPBlock_ref;

    my $PCount=0;
    foreach my $line (@FPBloc) {
	$PCount++;
#	print "line $PCount:  $line \n";
	my @Param=split(/\s+/,$line);
#	print "$Param[2]=$Param[3]+-$Param[4]  from $Param[5] to $Param[6]\n";
	InitParamTable->setText($PCount-1,0,1.0*$Param[3]);
	InitParamTable->setText($PCount-1,1,1.0*$Param[4]);
	InitParamTable->setText($PCount-1,2,1.0*$Param[5]);
	InitParamTable->setText($PCount-1,3,1.0*$Param[6]);
    }
    return;
}

void MuSRFitform::ActivateShComp()
{
    my %All=CreateAllInput();
    my @RUNS = split( /,/, $All{"RunNumbers"} );

   # Hide all sharing components
    SharingComp1->setHidden(1);
    SharingComp2->setHidden(1);
    SharingComp3->setHidden(1);
    SharingComp1->setEnabled(0);
    SharingComp2->setEnabled(0);
    SharingComp3->setEnabled(0);
    
    my @FitTypes =();
    foreach my $FitType ($All{"FitType1"}, $All{"FitType2"}, $All{"FitType3"}) {
	if ( $FitType ne "None" ) {
	    push( @FitTypes, $FitType );	    
	}
    }

# Get number of parameters to determine the size of the table 
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
# For now the line below does not work. Why?    
#    my $Paramcomp_ref=$All{"Paramcomp_ref"};
    my @Paramcomp = @$Paramcomp_ref;
    my $Full_T_Block= $All{"Full_T_Block"};
        
    my $Component=1;
    foreach my $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );
	
	if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
	    unshift( @Params, "Alpha" );
	}
	elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    unshift( @Params, ( "N0", "NBg" ) );
	}
	    
	
# Make the component appear first (only if we have multiple runs)
	my $ShCompG="SharingComp".$Component;
	my $ShCG = child($ShCompG);
	if ($#RUNS>0) {
	    $ShCG->setHidden(0);
	    $ShCG->setEnabled(1);
	}
                my $CompShLabel = "Comp".$Component."ShLabel";
	my $CompShL = child($CompShLabel);
	$CompShL->setText($All{"FitType$Component"});
	
# Change state/label of parameters
	for (my $i=1; $i<=5;$i++) {		
	    my $ParamChkBx="ShParam_".$Component."_".$i;
	    my $ChkBx = child($ParamChkBx);
	    if ($Params[$i-1] ne "") {
		$ChkBx->setHidden(0);
		$ChkBx->setEnabled(1);
		$ChkBx ->setText($Params[$i-1]);
	    } else {
		$ChkBx->setHidden(1);
	    }
	}
	$Component++;
    }  
}

void MuSRFitform::InitializeTab()
{
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

   my $erradd = "d";
   my $minadd = "_min";
   my $maxadd = "_max";
   
# First assume nothing is shared
   my $Shared = 0;
 
   my %All=CreateAllInput();
   my @RUNS = split( /,/, $All{"RunNumbers"} );

    my @FitTypes =();
    foreach my $FitType ($All{"FitType1"}, $All{"FitType2"}, $All{"FitType3"}) {
	if ( $FitType ne "None" ) { push( @FitTypes, $FitType ); }
    }
    
    InitParamTable->setLeftMargin(100);
		    
# Get theory block to determine the size of the table 
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
# For now the line below does not work. Why?    
#    my $Paramcomp_ref=$All{"Paramcomp_ref"};
    my @Paramcomp = @$Paramcomp_ref;
    my $Full_T_Block= $All{"Full_T_Block"};
  
    my $NRows = InitParamTable->numRows();

# Remove any rows in table
    if ($NRows > 0) {
	for (my $i=0;$i<$NRows;$i++) {
# TODO: Better remove the row rather than hide it.
	    InitParamTable->hideRow($i);
	}
    }

    my $PCount =0;
    my $iRun =0;
    my $value =0;
    my $error    = 0;
    my $minvalue = 0;
    my $maxvalue = 0;
    
    foreach my $RUN (@RUNS) {
	$iRun++;
	my $Component=1;
	foreach my $FitType (@FitTypes) {
	    my $Parameters=$Paramcomp[$Component-1];
	    my @Params = split( /\s+/, $Parameters );
	
	    if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
		unshift( @Params, "Alpha" );
	    }
	    elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
		unshift( @Params, ( "N0", "NBg" ) );
	    }

	    
# This is the counter for parameters of this component
	    my $NP=1;
	    $Shared = 0;
# Change state/label of parameters
	    foreach my $Param (@Params) {
		my $Param_ORG = $Param;
		if ( $#FitTypes != 0 && (   $Param ne "Alpha" && $Param ne "N0" && $Param ne "NBg" ) ){
		    $Param = join( "", $Param, "_", "$Component" );
		}
		
		$Shared = $All{"Sh_$Param"};
# It there are multiple runs index the parameters accordingly
		$Param=$Param."_".$iRun;
# Check if this parameter has been initialized befor. If not take from defaults
#		print "$Param=".$All{"$Param"}."\n";
		$value = $All{"$Param"};
		if ( $value ne "" ) {
		    $error    = $All{"$erradd$Param"};
		    $minvalue = $All{"$Param$minadd"};
		    $maxvalue = $All{"$Param$maxadd"};
		} else {
# I need this although it is already in the MSR.pm module, just for this table
# We can remove it from the MSR module later...
		    $value = $Defaults{$Param_ORG};
		    $error = $Defaults{ join( "", $erradd, $Param_ORG ) };
		    $minvalue = $Defaults{ join("", $Param_ORG, $minadd ) };
		    $maxvalue = $Defaults{ join("", $Param_ORG, $maxadd ) };
		}
		
		if ( $Shared!=1 || $iRun == 1 ) {
		    $PCount++;
			
# Add row in table, set its label and fill the values of parametr 	    
		    if ($PCount>$NRows) {	
			InitParamTable->setNumRows($PCount);
		    }
		    InitParamTable->verticalHeader()->setLabel( $PCount-1,$Param);
		    InitParamTable->showRow($PCount-1);
		    InitParamTable->setText($PCount-1,0,$value);
		    InitParamTable->setText($PCount-1,1,$error);
		    InitParamTable->setText($PCount-1,2,$minvalue);
		    InitParamTable->setText($PCount-1,3,$maxvalue);
		}
		$NP++;
	    }#Loop on parameters
	    $Component++;
	}# Loop on components
    }# Loop on runs
}

void MuSRFitform::TabChanged()
{
# TODO: First check if there are some runs given, otherwise disbale
# TODO: Check if the MSR file exists and decide whether to use it or not
    my %All=CreateAllInput();
    
    my $SlectedTab = musrfit_tabs->currentPageIndex;
# Check if the option for checking for existing files is selected    
    my $FileExistCheck= FileExistCheck->isOn();
    my $FILENAME=$All{"FILENAME"}.".msr";
    if ($All{"RunNumbers"} ne "" && $SlectedTab==4 && $FileExistCheck==1) {
	if (-e $FILENAME) {
# Warning: MSR file exists
	    my $Warning = "Warning: MSR file $FILENAME Already exists!\nChange name or backup file if you do not want to loose it.";
	    my $WarningWindow = Qt::MessageBox::information( this, "Warning",$Warning);
	}
    }
 
# First make sure we have sharing initialized    
    ActivateShComp();
    InitializeTab();
    UpdateMSRFileInitTable();
}


void MuSRFitform::GoFit()
{
    my %All=CreateAllInput();
    CallMSRCreate();
    my $FILENAME=$All{"FILENAME"}.".msr";
    if (-e $FILENAME) {
	my $cmd="musrfit -t $FILENAME";
	my $pid = open(FTO,"$cmd 2>&1 |");
	while (<FTO>) {
	    FitTextOutput->append("$_");
#		print "line= ".$_;
	}
	close(FTO);
	$cmd="musrview $FILENAME &";
	$pid = system($cmd);
    } else {
	FitTextOutput->append("Cannot find MSR file!");
    }
    FitTextOutput->append("-----------------------------------------------------------------------------------------------------------------------------");
# update MSR File tab and initialization table
    UpdateMSRFileInitTable();
    return;
}



void MuSRFitform::UpdeateTable()
{

}


