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
    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"}.".msr";
    my $file=Qt::FileDialog::getSaveFileName(
	    "$FILENAME",
	    "MSR Files (*.msr *.mlog)",
	    this,
	    "save file dialog",
	    "Choose a filename to save under");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
# TODO: check if the extension is correct, or add it.
	if (-e $FILENAME) {
	    my $cmd="cp $FILENAME $file";
	    my $pid=system($cmd);
	} else {
	    if ($file ne "") {
		my $Warning = "Warning: No MSR file found yet!";
		my $WarningWindow = Qt::MessageBox::information( this, "Warning",$Warning);
	    }
	}
    }
}

void Form1::fileChangeDir()
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
    my $Ans = Qt::MessageBox::question( this, "Quit?","Are you sure you want to quit?","&Yes","&No","",0,1);
    if ($Ans==0) {
# Then quit
	Qt::Application::exit( 0 );
    }
# Otherwize go back
}

void Form1::parametersExport()
{
    my %All=CreateAllInput();      
# Add also a flag for header
    $All{"Header"}=1;
    my $FILENAME=$All{"FILENAME"}.".dat";
    my $file=Qt::FileDialog::getSaveFileName(
	    "$FILENAME",
	    "Data Files (*.dat)",
	    this,
	    "export file dialog",
	    "Choose a filename to export to");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
	my $Text = MSR::ExportParams(\%All);
	open( DATF,q{>},"$file" );
	print DATF $Text;
	close(DATF);
    }
}


void Form1::parametersAppend()
{
    my %All=CreateAllInput();
# Add also a flag for header
    $All{"Header"}=0;
    my $FILENAME=$All{"FILENAME"}.".dat";
    my $file=Qt::FileDialog::getOpenFileName(
	    "./",
	    "Data Files (*.dat)",
	    this,
	    "append file dialog",
	    "Choose a filename to append to");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
	my $Text = MSR::ExportParams(\%All);
	open( DATF,q{>>},"$file" );
	print DATF $Text;
	close(DATF);
    }
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
# TODO: Need to automatically generage years list depending on beamline
    my %All=();
# From RUNS Tab    
    $All{"TITLE"}= TITLE->text;
    $All{"FILENAME"}= FILENAME->text;
    $All{"RunNumbers"} = RunNumbers->text;
    $All{"RunFiles"} = RunFiles->text;
    $All{"BeamLine"} = BeamLine->currentText;
    $All{"RUNSType"} = ManualFile->isOn();
    $All{"YEAR"} =YEAR->currentText;
    $All{"Tis"} = Tis->text;
    $All{"Tfs"} = Tfs->text;
    $All{"BINS"} = BINS->text;
    $All{"FitAsyType"} = FitAsyType->currentText;
    $All{"LRBF"} = LRBF->text;
    $All{"Xi"}=Xi->text;
    $All{"Xf"}=Xf->text;
    $All{"Yi"}=Yi->text;
    $All{"Yf"}=Yf->text;
    
# Lifetime corrections in enabled/visible only for SingleHis fits
    if ( $All{"FitAsyType"} eq "Asymmetry" ) {
	ltc->setHidden(1);
    }	
    elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
	ltc->setHidden(0);
    }
    
    if (ltc->isChecked()) {
	$All{"ltc"}="y";
    } else {
	$All{"ltc"}="n";
    }
    
    RunSelectionToggle();
    my @RUNS = ();
    if ($All{"RUNSType"} ) {
	@RUNS = split( /,/, $All{"RunFiles"});
    } else {
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	@RUNS = split( /,/, $All{"RunNumbers"} );	
    }
    
    my @Hists = split(/,/, $All{"LRBF"} );
    
# From Fourier Tab
    $All{"FUNITS"}= FUnits->currentText;
    $All{"FAPODIZATION"}= FApodization->currentText;
    $All{"FPLOT"}= FPlot->currentText;
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","Bg1","Bg2","Data1","Data2") {
	    my $Name = "$_$NHist";
	    $All{$Name}=child($Name)->text;
# TODO: If empty fill with defaults
	    if ($All{$Name} eq "") {
		$All{$Name}=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
		child($Name)->setText($All{$Name});
	    }
	}
	$NHist++;
    }
    
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
    
# Read initial values of paramets from tabel
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
# TODO: I need to take care of single hist fits here
	    if ( $All{"FitAsyType"} eq "SingleHist" ) {
		$Param=$Param.$Hists[0];	    
	    }
	    if ( $#FitTypes != 0 && (   $Param ne "Alpha" && $Param ne "N0" && $Param ne "NBg" ) ){
		$Param = join( "", $Param, "_", $Component);
	    }
	    
# Is there any point of sharing, multiple runs?
	    if ( $#RUNS == 0 && $All{"FitAsyType"} eq "Asymmetry") {
		$Shared = 1;
	    }
	    elsif ( $#RUNS == 0 && $#Hists == 0 &&  $All{"FitAsyType"} eq "SingleHist" )  {
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
	}
#Loop on parameters
	$Component++;
    }
# Loop on components
# Done with shared parameters detecting
    
# Construct a default filename if empty
    if ( $All{"FILENAME"} eq ""  && !$All{"RUNSType"}) {
	$All{"FILENAME"}=$RUNS[0]."_".$All{"BeamLine"}."_".$All{"YEAR"};
    } else {
	$All{"FILENAME"}="TMP";
    }
    
    if ( $All{"go"} eq "" ) {
	$All{"go"}="PLOT";
    }
    
# Get minimization process
    $All{"Minimization"} = Minimization->currentText();
    $All{"go"}=$All{"Minimization"};
    
# Get Error calculation process
    $All{"ErrorCalc"} = ErrorCalc->currentText();
    $All{"go"}=$All{"ErrorCalc"};
    
# Return Hash with all important values
    return %All;  
}

void MuSRFitform::CallMSRCreate()
{
    use MSR;
    my %All=CreateAllInput();
    
# Check if the option for checking for existing files is selected    
    my $FileExistCheck= FileExistCheck->isOn();
    my $FILENAME=$All{"FILENAME"}.".msr";
    my $Answer=0;
    if ($All{"RunNumbers"} ne ""  || $All{"RunFiles"} ne "") {
	if ( $FileExistCheck==1 ) {
	    if (-e $FILENAME) {
# Warning: MSR file exists
#		my $Warning = "Warning: MSR file $FILENAME Already exists!\nIf you continue it will overwriten.";
		my $Warning = "Warning: MSR file $FILENAME Already exists!\nDo you want to overwrite it?";
#		my $WarningWindow = Qt::MessageBox::information( this, "Warning",$Warning);
# $Answer =1,0 for yes and no
		$Answer= Qt::MessageBox::warning( this, "Warning",$Warning, "&No", "&Yes", undef, 1,1);
	    }
	} else {
# Just overwrite file
	    $Answer=1;
	}
	
	if ($Answer) {
	    if ( $All{"FitAsyType"} eq "Asymmetry" ) {
		my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateMSR(\%All);
	    }
	    elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
		my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateMSRSingleHist(\%All);
	    }
	    UpdateMSRFileInitTable();
	}
    }
    return $Answer;
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
	my @Param=split(/\s+/,$line);
	
# Depending on home many elements in @Param determine what they mean
# 0th element is empty (always)
# 1st element is the order (always)	
# 2nd element is the name (always)
# 3rd element is the value (always)
# 4th element can be taken as step/error (always)
# 5th element can be
#  if it is last element or there are two more = positive error, check $#Param=5/7
#  if there is only one more                            = minimum, check $#Param=6
	
# To summarize, check the value of $#Param
	my $value=1.0*$Param[3];
	my $error = 1.0*$Param[4];
	my $minvalue=0.0;
	my $maxvalue=0.0;
	if ($#Param == 4) {
	    $minvalue=0.0;
	    $maxvalue=0.0;
	}
	elsif ($#Param == 6) {
	    $minvalue=1.0*$Param[5];
	    $maxvalue=1.0*$Param[6];
	}
	elsif ($#Param == 5 || $#Param == 7) {
	    $minvalue=1.0*$Param[6];
	    $maxvalue=1.0*$Param[7];
	}	    
# Now update the initialization tabel	
	InitParamTable->setText($PCount-1,0,$value);
	InitParamTable->setText($PCount-1,1,$error);
	InitParamTable->setText($PCount-1,2,$minvalue);
	InitParamTable->setText($PCount-1,3,$maxvalue);
# Set bg color to mark different runs
    }
    return;
}

void MuSRFitform::ActivateT0Hists()
{
    my %All=CreateAllInput();
    my @Hists = split(/,/, $All{"LRBF"} );
    my $HistBox = "";
    for (my $iHist=1; $iHist<=4; $iHist++) {
	$HistBox="groupHist$iHist";
	my $HistBoxHandle = child($HistBox);
	if ($iHist<=$#Hists+1) {
# Activate this histogram box
	    $HistBoxHandle->setHidden(0);
	    $HistBoxHandle->setEnabled(1);
	    $HistBoxHandle->setTitle("Hist # $Hists[$iHist-1]");
	} else {
# Deactivate this histogram box
	    $HistBoxHandle->setHidden(1);
	    $HistBoxHandle->setEnabled(0);
	}
    }
    
# TODO: Set default values
    
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
	for (my $i=1; $i<=9;$i++) {		
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
    my %All=CreateAllInput();
    InitParamTable->setLeftMargin(100);
    my $NRows = InitParamTable->numRows();
    
# Remove any rows in table
    if ($NRows > 0) {
	for (my $i=0;$i<$NRows;$i++) {
# TODO: Better remove the row rather than hide it.
	    InitParamTable->hideRow($i);
	}
    }
    
    my %PTable=MSR::PrepParamTable(\%All);
    
# Setup the table with the right size    
    my $NParam=scalar keys( %PTable );
    if ($NParam>$NRows) {	
	InitParamTable->setNumRows($NParam);
    }
    
# Fill the table with labels and values of parametr 
    for (my $PCount=0;$PCount<$NParam;$PCount++) {
	my ($Param,$value,$error,$minvalue,$maxvalue,$RUN) = split(/,/,$PTable{$PCount});
# If you use this then reading the parameters from the table is a problem
# You need to extract the correct parameter name from the row label
#	InitParamTable->verticalHeader()->setLabel( $PCount,"$RUN: $Param");
	InitParamTable->verticalHeader()->setLabel( $PCount,"$Param");
	InitParamTable->showRow($PCount);
	InitParamTable->setText($PCount,0,$value);
	InitParamTable->setText($PCount,1,$error);
	InitParamTable->setText($PCount,2,$minvalue);
	InitParamTable->setText($PCount,3,$maxvalue);
    }
}

void MuSRFitform::TabChanged()
{
# TODO: First check if there are some runs given, otherwise disbale
    my %All=CreateAllInput();
    
# First make sure we have sharing initialized    
    ActivateShComp();
    InitializeTab();
    UpdateMSRFileInitTable();
# And also setup T0 and Bg bins
    ActivateT0Hists();
}


void MuSRFitform::GoFit()
{
    my %All=CreateAllInput();
    musrfit_tabs->setCurrentPage(1);
    my $Answer=CallMSRCreate();
    if ($Answer) {
	my $FILENAME=$All{"FILENAME"}.".msr";
	if (-e $FILENAME) {
	    my $cmd="musrfit -t $FILENAME";
	    my $pid = open(FTO,"$cmd 2>&1 |");
	    while (<FTO>) {
		FitTextOutput->append("$_");
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
    }
	
    return;
}

void MuSRFitform::GoPlot()
{
    my %All=CreateAllInput();
    my $Answer=CallMSRCreate();
    my $FILENAME=$All{"FILENAME"}.".msr";
    
    if ($Answer) {
	if (-e $FILENAME) {
	    my $cmd="musrview $FILENAME &";
	    my $pid = system($cmd);
	} else {
	    FitTextOutput->append("Cannot find MSR file!");
	    FitTextOutput->append("-----------------------------------------------------------------------------------------------------------------------------");
	}
    }
    return;
}


void MuSRFitform::ShowMuSRT0()
{
    my %All=CreateAllInput();
    musrfit_tabs->setCurrentPage(6);
# Create MSR file and then run musrt0
    my $Answer=CallMSRCreate();
    
    if ($Answer) {
	my $FILENAME=$All{"FILENAME"}.".msr";
	if (-e $FILENAME) {
	    my $cmd="musrt0 $FILENAME &";
	    my $pid = system($cmd);
	} else {
	    print STDERR "Cannot find MSR file!\n";
	}
    }
    return;
}


void MuSRFitform::T0Update()
{
    my %All = CreateAllInput();
    my @Hists = split(/,/, $All{"LRBF"} );
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","Bg1","Bg2","Data1","Data2") {
	    my $Name = "$_$NHist";
	    my $tmp=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
	    child($Name)->setText($tmp);
	}
	$NHist++
	    }
    
}


void MuSRFitform::RunSelectionToggle()
{
    my $ManualFile= ManualFile->isOn();
    if ($ManualFile) {
# Manual RUN selection
	RUNSMan->setEnabled(1);
	RUNSMan->setHidden(0);
	RunNumbers->setText("");
	RUNSAuto->setEnabled(0);
	RUNSAuto->setHidden(1);
    } else {
# Auto RUN selection
	RUNSMan->setEnabled(0);
	RUNSMan->setHidden(1);
	RunFiles->setText("");
	RUNSAuto->setEnabled(1);
	RUNSAuto->setHidden(0);
    }
}

void MuSRFitform::fileBrowse()
{
    my $RunFiles=RunFiles->text();
    print "Runs:$RunFiles\n";
    my $files_ref=Qt::FileDialog::getOpenFileNames(
	    "Data files (*.root *.bin)",
	    "./",
	    this,
	    "open files dialog",
	    "Select one or more files to fit");
    my @files = @$files_ref;
    if ($RunFiles eq "") {
# We started with an empty list	
	$RunFiles=join(",",@files);
    } else {
# Add files to existing list
	$RunFiles=join(",",$RunFiles,@files);
    }
    RunFiles->setText($RunFiles);
}
