package MuSRFit4;

use strict;
use warnings;
use QtCore4;
use QtGui4;
use QtCore4::isa qw( Qt::MainWindow );
# This is redundant
#use QtCore4::isa qw( Qt::Widget );
use QtCore4::debug qw(ambiguous);
use QtCore4::slots
    fileOpen => [],
    fileSave => [],
    fileChangeDir => [],
    filePrint => [],
    fileExit => [],
    parametersExport => [],
    parametersAppend => [],
    editUndo => [],
    editRedo => [],
    editCut => [],
    editCopy => [],
    editPaste => [],
    helpIndex => [],
    helpContents => [],
    helpAbout => [],
    CreateAllInput => [],
    CallMSRCreate => [],
    UpdateMSRFileInitTable => [],
    ActivateT0Hists => [],
    ActivateShComp => [],
    InitializeTab => [],
    TabChanged => [],
    GoFit => [],
    GoPlot => [],
    ShowMuSRT0 => [],
    t0Update => [],
    RunSelectionToggle => [],
    fileBrowse => [],
    AppendToFunctions => [],
    InitializeFunctions => [],
    t0UpdateClicked => [];
use Ui_MuSRFit4;

sub NEW {
    my ( $class, $parent ) = @_;
    $class->SUPER::NEW($parent);
    this->{ui} = Ui_MuSRFit4->setupUi(this);
    my %All=CreateAllInput();      
}

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

sub fileOpen()
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


sub fileSave()
{
    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"}.".msr";
    my $file=Qt::FileDialog::getSaveFileName(
	this,
	"Save file",
	"$FILENAME",
	"MSR Files (*.msr *.mlog)");
    
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

sub fileChangeDir()
{
    my $newdir=Qt::FileDialog::getExistingDirectory(
	this,
	"Change work directory",
	"./",
	"");
    chdir ("$newdir");
}

sub filePrint()
{
    
}


sub fileExit()
{
    my $Ans = Qt::MessageBox::question( this, "Quit?","Are you sure you want to quit?","&Yes","&No","",0,1);
    if ($Ans==0) {
# Then quit
	Qt::Application::exit( 0 );
    }
# Otherwize go back
}

sub parametersExport()
{
# Exports the fit parameters for a table format file
# This works only after a fit call, i.e. a plot call is not sufficient!
    my %All=CreateAllInput();      
# Add also a flag for header
    $All{"Header"}=1;
    my $FILENAME=$All{"FILENAME"}.".dat";
    my $file=Qt::FileDialog::getSaveFileName(
	this,
	"Export parameters to file",
	"$FILENAME",
	"Data Files (*.dat)");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
	my $Text = MSR::ExportParams(\%All);
	open( DATF,q{>},"$file" );
	print DATF $Text;
	close(DATF);
    }
}


sub parametersAppend()
{
# Appends the fit parameters for a table format file
# This works only after a fit call, i.e. a plot call is not sufficient!
    my %All=CreateAllInput();
# Add also a flag for header
    $All{"Header"}=0;
    my $FILENAME=$All{"FILENAME"}.".dat";
    my $file=Qt::FileDialog::getOpenFileName(
	this,
	"Append parameters to file",
	"./",
	"Data Files (*.dat)");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
	my $Text = MSR::ExportParams(\%All);
	open( DATF,q{>>},"$file" );
	print DATF $Text;
	close(DATF);
    }
}


sub editUndo()
{
    
}


sub editRedo()
{
    
}


sub editCut()
{
    
}


sub editCopy()
{
    
}


sub editPaste()
{
    
}


sub helpIndex()
{
    
}


sub helpContents()
{
    
}

sub helpAbout()
{
    my $AboutText="
This is a GUI that uses the musrfit binary to fit
muSR spectra. 
 
MuSRFitGUI is free software: you can redistribute it
and/or modify it under the terms of the GNU General 
Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at 
your option) any later version.
	  
MuSRFitGUI is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A 
PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
  
You should have received a copy of the GNU General Public
License along with MuSRFitGUI.  If not, see 
<http://www.gnu.org/licenses/>.
		  
Copyright 2009-2017 by Zaher Salman 
<zaher.salman\@psi.ch>.
";
    my $AboutWindow = Qt::MessageBox::information( this, "About MuSRFit GUI",$AboutText);
}

sub CreateAllInput()
{
    my %All=();
    
# From RUNS Tab
# Run data file
    $All{"RunNumbers"} = this->{ui}->runNumbers->text();
    $All{"RunFiles"} = this->{ui}->runFiles->text();
    $All{"BeamLine"} = this->{ui}->beamLine->currentText;
    $All{"RUNSType"} = this->{ui}->manualFile->isChecked();
    $All{"YEAR"} = this->{ui}->year->currentText;
    if (!defined($All{"YEAR"}) || $All{"YEAR"} eq "") {
# If year combobox is empty fill it up from 2004 up to current year
	    my ($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
	    my $current_year = 1900 + $yearOffset;
#	    my @years = ($current_year..2004);
#	    this->{ui}->year->addItems(@years);
	    for (my $i=$current_year;$i>=2004;$i--) {
		this->{ui}->year->addItem($i);
	    }
    }
# Time range and BINS
    $All{"Tis"} = this->{ui}->tis->text;
    $All{"Tfs"} = this->{ui}->tfs->text;
    $All{"BINS"} = this->{ui}->bins->text;
    $All{"FitAsyType"} = this->{ui}->fitAsyType->currentText;
    $All{"LRBF"} = this->{ui}->histsLRBF->text;
    my @Hists = split(/,/, $All{"LRBF"} );
# Lifetime corrections in enabled/visible only for SingleHis fits
    if ( $All{"FitAsyType"} eq "Asymmetry" ) {
	this->{ui}->ltc->setHidden(1);
    }
    elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
	this->{ui}->ltc->setHidden(0);
    }
   
# From Fitting Tab
# Plot range
    $All{"Xi"}=this->{ui}->xi->text;
    $All{"Xf"}=this->{ui}->xf->text;
    $All{"Yi"}=this->{ui}->yi->text;
    $All{"Yf"}=this->{ui}->yf->text;
    $All{"ViewBin"}=this->{ui}->viewBin->text;
# Life time correction   
    if (this->{ui}->ltc->isChecked()) {
	$All{"ltc"}="y";
    } else {
	$All{"ltc"}="n";
    }
# Minuit commands    
    if ( !defined($All{"go"}) || $All{"go"} eq "" ) {
	$All{"go"}="PLOT";
    }   
# Get minimization process
    $All{"Minimization"} = this->{ui}->minimization->currentText();
    $All{"go"}=$All{"Minimization"};
    
# Get Error calculation process
    $All{"ErrorCalc"} = this->{ui}->errorCalc->currentText();
    $All{"go"}=$All{"ErrorCalc"};
    
    RunSelectionToggle();
    my @RUNS = ();
    if ($All{"RUNSType"} ) {
	@RUNS = split( /,/, $All{"RunFiles"});
    } else {
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	@RUNS = split( /,/, $All{"RunNumbers"} );	
    }
        
# From MSR File Tab
    $All{"TITLE"}= this->{ui}->title->text;
    $All{"FILENAME"}= this->{ui}->fileName->text;

# From Fourier Tab
    $All{"FUNITS"}= this->{ui}->funits->currentText;
    $All{"FAPODIZATION"}= this->{ui}->fapodization->currentText;
    $All{"FPLOT"}= this->{ui}->fplot->currentText;
    $All{"FPHASE"}=this->{ui}->fphase->text;
# Fourier range
    $All{"FrqMin"}=this->{ui}->frqMin->text;
    $All{"FrqMax"}=this->{ui}->frqMax->text;
 
# Rotating reference frame parameters
    $All{"RRFFrq"}=this->{ui}->rrfFrq->text;
    $All{"RRFPack"}=this->{ui}->rrfPack->text;
    $All{"RRFPhase"}=this->{ui}->rrfPhase->text;
    $All{"RRFUnits"}=this->{ui}->rrfUnits->currentText;
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","bg1","bg2","data1","data2") {
	    my $Name = "$_$NHist";
	    $All{$Name}=child("Qt::LineEdit",$Name)->text;
# TODO: If empty fill with defaults
	    if ($All{$Name} eq "") {
		$All{$Name}=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
		child("Qt::LineEdit",$Name)->setText($All{$Name});
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
	     13,"MolMag",
	     14,"Meissner",
	     15,"LGKT",
	     16,"LGKTExp",
	     17,"LGKTSExp",
	     18,"None"
	     );
    
    my $FT1=this->{ui}->fitType1->currentIndex;
    my $FT2=this->{ui}->fitType2->currentIndex;
    my $FT3=this->{ui}->fitType3->currentIndex;
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
    
# Functions block 
    $All{"FunctionsBlock"}=this->{ui}->functionsBlock->toPlainText;
# and the associated theory block
    $All{"Func_T_Block"}=this->{ui}->theoryBlock->toPlainText;
    
# Shared settings are detected here
    $All{"EnableSharing"} = this->{ui}->buttonGroupSharing->isChecked();

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
	    unshift( @Params, ( "No", "NBg" ) );
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
	    if ( $#FitTypes != 0 && (   $Param ne "Alpha" && $Param ne "No" && $Param ne "NBg" ) ){
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
		my $ChkName="shParam_".$Component."_".$NP;
		my $ChkBx = child("Qt::Widget",$ChkName);
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
    if (defined($RUNS[0])) {
	if ( $All{"FILENAME"} eq ""  && !$All{"RUNSType"}) {
	    $All{"FILENAME"}=$RUNS[0]."_".$All{"BeamLine"}."_".$All{"YEAR"};
	    if ($All{"BeamLine"} eq "LEM (PPC)") {
		$All{"FILENAME"}=$RUNS[0]."_LEM_".$All{"YEAR"};
	    }
	}
    } else {
	$All{"FILENAME"}="TMP";
    }
    
    
# This has to be at the end of CreateAll    
    my %PTable=MSR::PrepParamTable(\%All);
    
# Setup the table with the right size    
    my $NParam=keys( %PTable );
    
# Read initial values of paramets from tabel
    my $erradd = "d";
    my $minadd = "_min";
    my $maxadd = "_max";
    my $QTable=this->{ui}->initParamTable;
# TODO: Should not go over all rows, only on parameters.
    if ($NParam > 0) {
# Set appropriate number of rows
	$QTable->setRowCount($NParam);
	for (my $i=0;$i<=$NParam;$i++) {
	    # Make sure the row items exist/defined
	    if (defined($QTable->verticalHeaderItem($i)) && defined($QTable->item($i,0))  && defined($QTable->item($i,1)) && defined($QTable->item($i,2)) && defined($QTable->item($i,3))) {
		# Take label of row, i.e. name of parameter
		my $Param=$QTable->verticalHeaderItem($i)->text();
		# Then take the value, error, max and min (as numbers)
		$All{"$Param"}=1.0*$QTable->item($i,0)->text();
		$All{"$erradd$Param"}=1.0*$QTable->item($i,1)->text();
		$All{"$Param$minadd"}=1.0*$QTable->item($i,2)->text();
		$All{"$Param$maxadd"}=1.0*$QTable->item($i,3)->text();
	    }
	}
    }
# Return Hash with all important values
    return %All;  
}

sub CallMSRCreate()
{
    use MSR;
    my %All=CreateAllInput();
    
# Check if the option for checking for existing files is selected    
    my $FileExistCheck= this->{ui}->fileExistCheck->isChecked();
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
		if ($All{"RUNSType"}) {
		    my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSR(\%All);
# Open output file FILENAME.msr
		    open( OUTF,q{>},"$FILENAME" );
		    print OUTF ("$FullMSRFile");
		    close(OUTF);
		} else {
#		    my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSRSh(\%All);
		    my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSR(\%All);
# Open output file FILENAME.msr
		    open( OUTF,q{>},"$FILENAME" );
		    print OUTF ("$FullMSRFile");
		    close(OUTF);
		    
# if it is multiple runs then the produced file is a template
		    my $FILENAME=$All{"FILENAME"}.".msr";
		    my $Extension = "_".$All{"BeamLine"}."_".$All{"YEAR"};
		    if ($All{"BeamLine"} eq "LEM (PPC)") {
			$Extension = "_LEM_".$All{"YEAR"};
		    }
		    
		    if (-e $FILENAME) {
			my $RUN0 = $FILENAME;
			$RUN0 =~ s/$Extension//g;
			$RUN0 =~ s/.msr//g;
			my $cmd = $All{"RunNumbers"};
			$cmd =~ s/,/ /g;
			$cmd = "msr2data \[".$cmd."\] ".$Extension." msr-".$RUN0." global";
			$cmd = $cmd."; mv $RUN0+global$Extension.msr ".$FILENAME;
			print $cmd."\n";
			my $pid = open(FTO,"$cmd 2>&1 |");
			while (<FTO>) {
			    this->{ui}->fitTextOutput->append("$_");
			}
			close(FTO);
		    }
		}
	    }
	    elsif ( $All{"FitAsyType"} eq "SingleHist" ) {
		my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSRSingleHist(\%All);
# Open output file FILENAME.msr
		open( OUTF,q{>},"$FILENAME" );
		print OUTF ("$FullMSRFile");
		close(OUTF);
	    }
	    UpdateMSRFileInitTable();
	}
    }
    return $Answer;
}

sub UpdateMSRFileInitTable()
{
    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"};
    my @lines=();
    if (-e "$FILENAME.msr") {
	open (MSRF,q{<},"$FILENAME.msr" );
	@lines = <MSRF>;
	close(IFILE);
    }
    this->{ui}->textMSROutput->setText("");
    foreach my $line (@lines) {
	this->{ui}->textMSROutput->insertPlainText("$line");
    }
    
    (my $TBlock_ref, my $FPBlock_ref)=MSR::ExtractBlks(@lines);
    my @FPBloc = @$FPBlock_ref;
    
    my $PCount=0;
    foreach my $line (@FPBloc) {
	my @Param=split(/\s+/,$line);
	
# Depending on how many elements in @Param determine what they mean
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
#	for (my $i=0;$i<=$#Param;$i++) { print "$i - $Param[$i]\n";}
	if ($#Param == 4 || $#Param == 5) {
	    $minvalue=0.0;
	    $maxvalue=0.0;
	}
	elsif ($#Param == 6) {
	    $minvalue=1.0*$Param[5];
	    $maxvalue=1.0*$Param[6];
	}
	elsif ($#Param == 7) {
	    $minvalue=1.0*$Param[6];
	    $maxvalue=1.0*$Param[7];
	}	    
# Now update the initialization tabel
	my $QTable = this->{ui}->initParamTable;
#	print "PCount=$PCount and value=$value\n";
	if (defined($QTable->item($PCount,0)) & defined($QTable->item($PCount,1)) & defined($QTable->item($PCount,2)) & defined($QTable->item($PCount,3))) {
	    $QTable->setItem($PCount,0,Qt::TableWidgetItem());
	    $QTable->setItem($PCount,1,Qt::TableWidgetItem());
	    $QTable->setItem($PCount,2,Qt::TableWidgetItem());
	    $QTable->setItem($PCount,3,Qt::TableWidgetItem());
	    $QTable->item($PCount,0)->setText($value);
	    $QTable->item($PCount,1)->setText($error);
	    $QTable->item($PCount,2)->setText($minvalue);
	    $QTable->item($PCount,3)->setText($maxvalue);
	}
# Set bg color to mark different runs
	$PCount++;
    }
    return;
}

sub ActivateT0Hists()
{
    my %All=CreateAllInput();
    my @Hists = split(/,/, $All{"LRBF"} );
    my $HistBox = "";
    for (my $iHist=1; $iHist<=4; $iHist++) {
	$HistBox="groupHist$iHist";
	my $HistBoxHandle = child("Qt::Widget",$HistBox);
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

sub ActivateShComp()
{
    my %All=CreateAllInput();
    my @RUNS = split( /,/, $All{"RunNumbers"} );
    
# Hide all sharing components
    this->{ui}->sharingComp1->setHidden(1);
    this->{ui}->sharingComp2->setHidden(1);
    this->{ui}->sharingComp3->setHidden(1);
    this->{ui}->sharingComp1->setEnabled(0);
    this->{ui}->sharingComp2->setEnabled(0);
    this->{ui}->sharingComp3->setEnabled(0);
    
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
    $Full_T_Block= $All{"Full_T_Block"};
    
# Possible to share only if sharing is enabled altogether
    my $EnableSharing = $All{"EnableSharing"};
    if ($EnableSharing) {
	my $Component=1;
	foreach my $FitType (@FitTypes) {
	    my $Parameters=$Paramcomp[$Component-1];
	    my @Params = split( /\s+/, $Parameters );
	
	    if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
		unshift( @Params, "Alpha" );
	    }
	    elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
		unshift( @Params, ( "No", "NBg" ) );
	    }
	
	
# Make the component appear first (only if we have multiple runs)
	    my $ShCompG="sharingComp".$Component;
	    my $ShCG = child("Qt::Widget",$ShCompG);
	    if ($#RUNS>0) {
		$ShCG->setHidden(0);
		$ShCG->setEnabled(1);
	    }
	    my $CompShLabel = "comp".$Component."ShLabel";
	    my $CompShL = child("Qt::Widget",$CompShLabel);
	    $CompShL->setText($All{"FitType$Component"});
	
# Change state/label of parameters
	    for (my $i=1; $i<=9;$i++) {		
		my $ParamChkBx="shParam_".$Component."_".$i;
		my $ChkBx = child("Qt::Widget",$ParamChkBx);
		if (defined($Params[$i-1])) {
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
}

sub InitializeTab()
{
    my %All=CreateAllInput();
    my $QTable = this->{ui}->initParamTable;
    my $NRows = $QTable->rowCount();
    
# Remove any rows in table
    if ($NRows > 0) {
	for (my $i=0;$i<$NRows;$i++) {
# TODO: Better remove the row rather than hide it.
	    $QTable->hideRow($i);
#	    $QTable->removeRow($i);
	}
    }
    
    my %PTable=MSR::PrepParamTable(\%All);
    
# Setup the table with the right size    
#    my $NParam=scalar keys( %PTable );
    my $NParam=keys( %PTable );
    if ($NParam>$NRows) {	
	$QTable->setNumRows($NParam);
    }

#    for (my $i=0;$i<$NParam;$i++) {print "Line=$PTable{$i}\n";}

    
# Fill the table with labels and values of parameter 
    for (my $PCount=0;$PCount<$NParam;$PCount++) {
	my ($Param,$value,$error,$minvalue,$maxvalue,$RUN) = split(/,/,$PTable{$PCount});
# Now make sure we have no nans
	if ($error eq "nan") { $error=0.1;}
	# Make sure items exist before addressing them
	$QTable->setVerticalHeaderItem($PCount,Qt::TableWidgetItem());
	# Make sure that the row exists
	$QTable->showRow($PCount);
	$QTable->setItem($PCount,0,Qt::TableWidgetItem());
	$QTable->setItem($PCount,1,Qt::TableWidgetItem());
	$QTable->setItem($PCount,2,Qt::TableWidgetItem());
	$QTable->setItem($PCount,3,Qt::TableWidgetItem());
	# Fill in the values...
	$QTable->verticalHeaderItem($PCount)->setText($Param);
	$QTable->item($PCount,0)->setText($value);
	$QTable->item($PCount,1)->setText($error);
	$QTable->item($PCount,2)->setText($minvalue);
	$QTable->item($PCount,3)->setText($maxvalue);
    }
}

sub TabChanged()
{
# TODO: First check if there are some runs given, otherwise disbale
    my %All=CreateAllInput();
    
# First make sure we have sharing initialized    
    ActivateShComp();
# Here we need to apply sharing if selected...
    InitializeTab();
    UpdateMSRFileInitTable();
# And also setup T0 and Bg bins
    ActivateT0Hists();
    
# Initialize FUNCTIONS block only if it has not been initialized yet
    if ($All{"Func_T_Block"} eq "" ) {
	InitializeFunctions();
    }    
}


sub GoFit()
{
    my %All=CreateAllInput();
# Check here is the number of histograms makes sense
# other wise give error.
    my @Hists = split( /,/, $All{"LRBF"} );
    if ($All{"FitAsyType"} eq "Asymmetry" && $#Hists != 1) {
# we have a problem here send error message
	my $Warning = "Error: The number of histograms should be 2 for an asymmetry fit!";
	my $WarningWindow = Qt::MessageBox::information( this, "Error",$Warning);
    } else {
	this->{ui}->musrfit_tabs->setCurrentIndex(1);
	my $Answer=CallMSRCreate();
	if ($Answer) {
	    my $FILENAME=$All{"FILENAME"}.".msr";
	    if (-e $FILENAME) {
		my $cmd="musrfit -t $FILENAME";
		my $pid = open(FTO,"$cmd 2>&1 |");
		while (<FTO>) {
		    this->{ui}->fitTextOutput->insertPlainText("$_");
		}
		close(FTO);
		$cmd="musrview $FILENAME &";
		$pid = system($cmd);
	    } else {
		this->{ui}->fitTextOutput->insertPlainText("Cannot find MSR file!");
	    }
	    this->{ui}->fitTextOutput->insertPlainText("-----------------------------------------------------------------------------------------------------------------------------");
# update MSR File tab and initialization table
	    UpdateMSRFileInitTable();
	}
    }
    return;
}

sub GoPlot()
{
    my %All=CreateAllInput();
# Check here is the number of histograms makes sense
# other wise give error.
    my @Hists = split( /,/, $All{"LRBF"} );
    if ($All{"FitAsyType"} eq "Asymmetry" && $#Hists != 1) {
# we have a problem here send error message
	my $Warning = "Error: The number of histograms should be 2 for an asymmetry fit!";
	my $WarningWindow = Qt::MessageBox::information( this, "Error",$Warning);
    } else {
	my $Answer=CallMSRCreate();
	my $FILENAME=$All{"FILENAME"}.".msr";
	if ($Answer) {
	    if (-e $FILENAME) {
		my $cmd="musrview $FILENAME &";
		my $pid = system($cmd);
	    } else {
		this->{ui}->fitTextOutput->insertPlainText("Cannot find MSR file!");
		this->{ui}->fitTextOutput->insertPlainText("-----------------------------------------------------------------------------------------------------------------------------");
	    }
	}
    }
    return;
}


sub ShowMuSRT0()
{
# Open musrt0 to check and adjust t0 , Bg and Data bins
    my %All=CreateAllInput();
    this->{ui}->musrfit_tabs->setCurrentIndex(6);
# Create MSR file and then run musrt0
    my $Answer=CallMSRCreate();
    
    if ($Answer) {
	my $FILENAME=$All{"FILENAME"}.".msr";
	if (-e $FILENAME) {
	    my $cmd="musrt0 $FILENAME &";
	    my $pid = system($cmd);
	    this->{ui}->t0Update->setEnabled(1)
	} else {
	    print STDERR "Cannot find MSR file!\n";
	}
    }
    return;
}


sub t0Update()
{
    my %All = CreateAllInput();
    my @Hists = split(/,/, $All{"LRBF"} );
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","Bg1","Bg2","Data1","Data2") {
	    my $Name = "$_$NHist";
	    my $tmp=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
	    if (defined(child("Qt::Widget",$Name))) {
		child("Qt::Widget",$Name)->setText($tmp);
	    }
	}
	$NHist++
    }
}


sub RunSelectionToggle()
{
    my $ManualFile= this->{ui}->manualFile->isChecked();
    if ($ManualFile) {
# Manual RUN selection
	this->{ui}->runsMan->setEnabled(1);
	this->{ui}->runsMan->setHidden(0);
	this->{ui}->runNumbers->setText("");
	this->{ui}->runsAuto->setEnabled(0);
	this->{ui}->runsAuto->setHidden(1);
    } else {
# Auto RUN selection
	this->{ui}->runsMan->setEnabled(0);
	this->{ui}->runsMan->setHidden(1);
	this->{ui}->runFiles->setText("");
	this->{ui}->runsAuto->setEnabled(1);
	this->{ui}->runsAuto->setHidden(0);
    }
    
}

sub fileBrowse()
{
    my $RunFiles=this->{ui}->runFiles->text();
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
    this->{ui}->runFiles->setText($RunFiles);
}

sub AppendToFunctions()
{
    my $ParName=this->{ui}->cParamsCombo->currentText();
    my $Full_T_Block="";
    my $Constraint="";
    if (defined(this->{ui}->theoryBlock->toPlainText)) {
	$Full_T_Block=this->{ui}->theoryBlock->toPlainText;
    }
    if (defined(this->{ui}->constraintLine->text)) {
	$Constraint=this->{ui}->constraintLine->text;
    }
# Then clear the text
    this->{ui}->constraintLine->setText("");
    
# Check how many constraints (lines) in FUNCTIONS Block    
#    my $i=this->{ui}->functionsBlock->blockCount();
    my $fun_lines=this->{ui}->functionsBlock->toPlainText();
    my $i= ($fun_lines =~ tr/\n//)+1;
    my $ConstLine="fun$i = $Constraint\n";
    this->{ui}->functionsBlock->insertPlainText($ConstLine);
    
# Replace parameter in theory block with fun$i
    $Full_T_Block=~ s/$ParName/fun$i/;
    this->{ui}->theoryBlock->setText($Full_T_Block);
}

sub InitializeFunctions()
{
    my %All=CreateAllInput();
    my @RUNS = split( /,/, $All{"RunNumbers"} );
    
    my @FitTypes =();
    foreach my $FitType ($All{"FitType1"}, $All{"FitType2"}, $All{"FitType3"}) {
	if ( $FitType ne "None" ) {
	    push( @FitTypes, $FitType );	    
	}
    }
    
# Get number of parameters to determine the size of the table 
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
    my @Paramcomp = @$Paramcomp_ref;
    $Full_T_Block= $All{"Full_T_Block"};
    
# Initialize Parameters List in function block (constraints).    
    my $ParametersList="";
    this->{ui}->parametersList->setText("");
# Counter for function block (with out Alpha etc.)
    my $ParCount=0;
    this->{ui}->cParamsCombo->clear();
    
# Possibly use the parameters block to axtract names for the dropdown menu
# this makes sense if we can use fun in map line. Check!
    my $Component=1;
    foreach my $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );	

# Alpha, No and NBg are counted in the parameters
	if ( $Component == 1 && $All{"FitAsyType"} eq "Asymmetry" ) {
	    unshift( @Params, "Alpha" );
	}
	elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    unshift( @Params, ( "No", "NBg" ) );
	}
	
# Add list to the constraints drop down menu
	for (my $i=1; $i<=9;$i++) {		
	    if (defined($Params[$i-1])) {
		my $CParam = $Params[$i-1]."_".$Component;
		if ($Params[$i-1] ne "Alpha" && $Params[$i-1] ne "No" && $Params[$i-1] ne "NBg") {
		    this->{ui}->cParamsCombo->addItem($CParam);
		    $Full_T_Block=~ s/\b$Params[$i-1]\b/$CParam/;
		}
# also enumerate the parameters as should be used in the FUNCTIONS Block
		$ParCount++;
		$ParametersList=$ParametersList."$CParam \t is \t par$ParCount\n";
		this->{ui}->parametersList->setText($ParametersList);
	    }
	}
	$Component++;
    }  
# Set theory block in Constraints    
    this->{ui}->theoryBlock->setText($Full_T_Block);
# Then clear the text
    this->{ui}->constraintLine->setText("");
    this->{ui}->functionsBlock->setText("");
}




sub t0UpdateClicked()
{
# Read MSR file and get new values of t0,Bg and Data
    my %All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"};
    open (MSRF,q{<},"$FILENAME.msr" );
    my @lines = <MSRF>;
    close(IFILE);
    
    my @T0s = grep {/t0 /} @lines;
    my @Bgs = grep {/background /} @lines;
    my @Datas = grep {/data /} @lines;

    my @Hists = split(/,/, $All{"LRBF"} );
    my $NHist = $#Hists+1;
    print "Histograms: $NHist\n";
    
    my $FinHist = 1;
# First T0s
    while ($FinHist) {
	my $counter=0;
	(my $tmp,my @SplitT0) = split( /\s+/, $T0s[$counter]);
	($tmp,my @SplitBg) = split( /\s+/, $Bgs[$counter]);
	($tmp,my @SplitData) = split( /\s+/, $Datas[$counter]);
	if ($#SplitBg>0) {
	    foreach (@SplitBg) {
		print $_."\n";
	    }
	}
	$counter++;
	if ($counter>=$#Bgs) {$FinHist=0;}
    }	
    
# Finally, disable the update button
    this->{ui}->t0Update->setEnabled(0);
#    t0Update->setText("musrt0")
}

# Function: return widget attribute given its type and name
sub child {
# Take type and name from input
    my ( $object, $name ) = @_;
    
    my $Attrib = this->findChildren($object,$name);
    if (@$Attrib) {
	$Attrib = @$Attrib[0];
    } else {
	$Attrib = 0;
    }
#    print "name = $name and attrib = $Attrib\n";

# Return handle on widget
    return($Attrib);
}


1;