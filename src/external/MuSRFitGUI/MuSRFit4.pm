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
#    fileOpen => [],
    fileSave => [],
    fileChangeDir => [],
#    filePrint => [],
    fileExit => [],
    parametersExport => [],
    parametersAppend => [],
#    editUndo => [],
#    editRedo => [],
#    editCut => [],
#    editCopy => [],
#    editPaste => [],
#    helpIndex => [],
#    helpContents => [],
    helpAbout => [],
#    CreateAllInput => [],
#    UpdateMSRFileInitTable => [],
#    ActivateT0Hists => [],
    ActivateShComp => [],
#    InitializeTab => [],
    TabChanged => [],
    GoFit => [],
    GoPlot => [],
    ShowMuSRT0 => [],
    t0Update => [],
    RunSelectionToggle => [],
#    fileBrowse => [],
    AppendToFunctions => [],
    InitializeFunctions => [],
    addFitType => [],
    addSharingComp => [],
    runsLineNotEmpy => [],
    t0UpdateClicked => [];
use Ui_MuSRFit4;
use MSR;

our $self = "";
our %All = ();

sub NEW {
    my ( $class, $parent ) = @_;
    $class->SUPER::NEW($parent);
    $self = Ui_MuSRFit4->setupUi(this);
    %All=CreateAllInput();      
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
    my @RUNS = ();
    if ($All{"RUNSType"} ) {
	@RUNS = split( /,/, $All{"RunFiles"});
    } else {
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	@RUNS = split( /,/, MSR::ExpandRunNumbers($All{"RunNumbers"}) );	
    }
# Add also a flag for header
    $All{"Header"}=1;
    my $StupidName = $RUNS[0]."+global_tmpl.msr";
    my $FILENAME=$All{"FILENAME"}.".dat";
    my $MSR = $All{"FILENAME"}.".msr";
    my $file=Qt::FileDialog::getSaveFileName(
	this,
	"Export parameters to file",
	"$FILENAME",
	"Data Files (*.dat)");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
	if ($All{"FitAsyType"} eq "Asymmetry") {
	    # Update All from GUI
	    %All=CreateAllInput();      
	    # my style
	    my $Text = MSR::ExportParams(\%All);
	    open( DATF,q{>},"$file" );
	    print DATF $Text;
	    close(DATF);
	} else {
	    # msr2data style
	    my $RunList = join(" ",@RUNS);
	    my $cmd = "cp $MSR $StupidName; msr2data \[".$RunList."\] "." _tmpl new global data -o $file";
	    my $pid = open(FTO,"$cmd 2>&1 |");
	    while (<FTO>) {
		$self->fitTextOutput->append("$_");
	    }
	}
    }
}


sub parametersAppend()
{
# Appends the fit parameters for a table format file
# This works only after a fit call, i.e. a plot call is not sufficient!
    my @RUNS = ();
    if ($All{"RUNSType"} ) {
	@RUNS = split( /,/, $All{"RunFiles"});
    } else {
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	@RUNS = split( /,/, MSR::ExpandRunNumbers($All{"RunNumbers"}) );	
    }
# Add also a flag for header
    $All{"Header"}=0;
    my $StupidName = $RUNS[0]."+global_tmpl.msr";
    my $FILENAME=$All{"FILENAME"}.".dat";
    my $MSR = $All{"FILENAME"}.".msr";
    my $file=Qt::FileDialog::getOpenFileName(
	this,
	"Append parameters to file",
	"./",
	"Data Files (*.dat)");
    
# If the user gave a filename the copy to it
    if ($file ne "") {
	if ($All{"FitAsyType"} eq "Asymmetry") {
	    # Update All values from GUI
	    %All=CreateAllInput();
	    my $Text = MSR::ExportParams(\%All);
	    open( DATF,q{>>},"$file" );
	    print DATF $Text;
	    close(DATF);
	} else {
	    # msr2data style
	    my $RunList = join(" ",@RUNS);
	    my $cmd = "cp $MSR $StupidName; msr2data \[".$RunList."\] "." _tmpl global data -o $file";
	    my $pid = open(FTO,"$cmd 2>&1 |");
	    while (<FTO>) {
		$self->fitTextOutput->append("$_");
	    }
	    close(FTO);
	}
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
    # From RUNS Tab
    # Run data file
    $All{"RunNumbers"} = $self->runNumbers->text();
    $All{"RunFiles"} = $self->runFiles->text();
    $All{"BeamLine"} = $self->beamLine->currentText;
    $All{"RUNSType"} = $self->manualFile->isChecked();
    $All{"YEAR"} = $self->year->currentText;
    if (!defined($All{"YEAR"}) || $All{"YEAR"} eq "") {
# If year combobox is empty fill it up from 2004 up to current year
	    my ($second, $minute, $hour, $dayOfMonth, $month, $yearOffset, $dayOfWeek, $dayOfYear, $daylightSavings) = localtime();
	    my $current_year = 1900 + $yearOffset;
#	    my @years = ($current_year..2004);
#	    $self->year->addItems(@years);
	    for (my $i=$current_year;$i>=2004;$i--) {
		$self->year->addItem($i);
	    }
    }
# Time range and BINS
    $All{"Tis"} = $self->tis->text;
    $All{"Tfs"} = $self->tfs->text;
    $All{"BINS"} = $self->bins->text;
    $All{"FitAsyType"} = $self->fitAsyType->currentText;
    $All{"FitAsyType"} =~ s/ //g;
    $All{"LRBF"} = $self->histsLRBF->text;
    my @Hists = split(/,/, $All{"LRBF"} );
# Lifetime corrections in enabled/visible only for SingleHis fits
    if ( $All{"FitAsyType"} eq "SingleHist" ) {
	$self->ltc->setHidden(0);
	$All{"fittype"}=0;
    } else {
	$self->ltc->setHidden(1);
	$All{"fittype"}=2;
    }
   
# From Fitting Tab
# Plot range
    $All{"Xi"}=$self->xi->text;
    $All{"Xf"}=$self->xf->text;
    $All{"Yi"}=$self->yi->text;
    $All{"Yf"}=$self->yf->text;
    $All{"ViewBin"}=$self->viewBin->text;
# Life time correction   
    if ($self->ltc->isChecked()) {
	$All{"ltc"}="y";
    } else {
	$All{"ltc"}="n";
    }
# Minuit commands    
    if ( !defined($All{"go"}) || $All{"go"} eq "" ) {
	$All{"go"}="PLOT";
    }   
# Get minimization process
    $All{"Minimization"} = $self->minimization->currentText();
    $All{"go"}=$All{"Minimization"};
    
# Get Error calculation process
    $All{"ErrorCalc"} = $self->errorCalc->currentText();
    $All{"go"}=$All{"ErrorCalc"};
    
# Commands block
    $All{"CommandsBlock"}= $All{"Minimization"}."\n".$All{"ErrorCalc"};

    RunSelectionToggle();
    my @RUNS = ();
    if ($All{"RUNSType"} ) {
	@RUNS = split( /,/, $All{"RunFiles"});
    } else {
	$All{"RunNumbers"} =~ s/[\ \.\~\/\&\*\[\;\>\<\^\$\(\)\`\|\]\'\@]/,/g;
	@RUNS = split( /,/, MSR::ExpandRunNumbers($All{"RunNumbers"}) );	
    }
        
# From MSR File Tab
    $All{"TITLE"}= $self->title->text;
    $All{"FILENAME"}= $self->fileName->text;

# From Fourier Tab
    $All{"FUNITS"}= $self->funits->currentText;
    $All{"FAPODIZATION"}= $self->fapodization->currentText;
    $All{"FPLOT"}= $self->fplot->currentText;
    $All{"FPHASE"}=$self->fphase->text;
# Fourier range
    $All{"FrqMin"}=$self->frqMin->text;
    $All{"FrqMax"}=$self->frqMax->text;
 
# Rotating reference frame parameters
    $All{"RRFFrq"}=$self->rrfFrq->text;
    $All{"RRFPack"}=$self->rrfPack->text;
    $All{"RRFPhase"}=$self->rrfPhase->text;
    $All{"RRFUnits"}=$self->rrfUnits->currentText;
    
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

    # Check the number of components
    my $numComps = 0;
    my $FT1=0;
    my $FT2=0;
    my $FT3=0;
    my @FitTypes =();
    my $FT=0;
    if (defined($self->numComps)) {
	# new style, check the number of components
	$numComps = $self->numComps->value;
	if ($numComps != $self->columnView->count() ) {
	    addFitType();
	}
    } else {
	# old style, only 3 componenets
	$numComps = 3;
    }
    $All{"numComps"}=$numComps;
	    
    # loop over fitTypes
    for (my $i=1;$i<=$numComps;$i++) {
	my $FTi=child("Qt::ComboBox","fitType$i");
	if ($FTi == 0) {
	    $All{"FitType$i"} = "None";
	} else {
	    $All{"FitType$i"} = $FTs{$FTi->currentIndex};
	}
	if ( $All{"FitType$i"} ne "None" ) {
	    push( @FitTypes, $All{"FitType$i"} );	    
	}
    }
    
# Also theory block and paramets list
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
    $All{"Full_T_Block"}=$Full_T_Block;    
    $All{"Paramcomp_ref"}=$Paramcomp_ref;
    my @Paramcomp = @$Paramcomp_ref;
    
# Functions block 
    $All{"FunctionsBlock"}=$self->functionsBlock->toPlainText;
# and the associated theory block
    $All{"Func_T_Block"}=$self->theoryBlock->toPlainText;
    
# Shared settings are detected here
    $All{"EnableSharing"} = $self->buttonGroupSharing->isChecked();
    # Make sure all sharing boxes exist
    #addSharingComp();


    my $Shared = 0; 
    my $PCount =0;
    my $Component=1;
    foreach my $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );
	
	if ( $Component == 1 && ($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB" )) {
	    unshift( @Params, "Alpha" );
	} elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    if (grep(/Phi/,@Params)) {
		# remove the Phi and put it back for each histogram
		@Params = grep ! /Phi/,@Params;
		# for each histogram
		foreach my $Hist (@Hists) {
		    # take only first histogram from sum
		    ($Hist,my  $tmp) = split(/ /,$Hist);
		    # Doesn't make sense to share No or NBg!
		    # unshift( @Params, ( "N0$Hist", "NBg$Hist", "Phi$Hist" ) );
		    push(@Params,"Phi$Hist");
		}
	    }
	}
	
# This is the counter for parameters of this component
	my $NP=1;
	$Shared = 0;
# Change state/label of parameters
	foreach my $Param (@Params) {
	    my $Param_ORG = $Param;
# TODO: I need to take care of single hist fits here
#	    if ( $All{"FitAsyType"} eq "SingleHist" ) {
#		$Param=$Param.$Hists[0];	    
#	    }
	    if ( $#FitTypes != 0 && (   $Param ne "Alpha" && $Param ne "No" && $Param ne "NBg" ) ){
		$Param = join( "", $Param, "_", $Component);
	    }
	    
# Is there any point of sharing, multiple runs?
	    if ( $#RUNS == 0 ) {
		$Shared = 1;
	    } else {	
# Check if shared or not, construct name of checkbox, find its handle and then 
# check if it is checked
		my $ChkName="shParam_".$Component."_".$NP;
		my $ChkBx = child("Qt::Widget",$ChkName);
		if ($ChkBx != 0) {$Shared = $ChkBx->isChecked();}
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
    my $QTable=$self->initParamTable;
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
    # Check if the option for checking for existing files is selected    
    my $FileExistCheck= $self->fileExistCheck->isChecked();
    my $FILENAME=$All{"FILENAME"}.".msr";
    my $Answer=0;    
    if ($All{"RunNumbers"} ne ""  || $All{"RunFiles"} ne "") {
	if ( $FileExistCheck==1 ) {
	    if (-e $FILENAME) {
		# Warning: MSR file exists
		my $Warning = "Warning: MSR file $FILENAME Already exists!\nDo you want to overwrite it?";
		# $Answer =1,0 for yes and no
		$Answer= Qt::MessageBox::warning( this, "Warning",$Warning, "&No", "&Yes", undef, 1,1);
	    }
	} else {
	    # Just overwrite file
	    $Answer=1;
	}
	
	if ($Answer) {
	    if ( $All{"FitAsyType"} eq "Asymmetry") {
		if ($All{"RUNSType"}) {
		    my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSRUni(\%All);
		    # Open output file FILENAME.msr
		    open( OUTF,q{>},"$FILENAME" );
		    print OUTF ("$FullMSRFile");
		    close(OUTF);
		} else {
		    my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSRUni(\%All);
		    # Open output file FILENAME.msr
		    open( OUTF,q{>},"$FILENAME" );
		    print OUTF ("$FullMSRFile");
		    close(OUTF);
		}
	    } else {
		my ($Full_T_Block,$Paramcomp_ref,$FullMSRFile)= MSR::CreateMSRGLB(\%All);
		my @RUNS = split( /,/,MSR::ExpandRunNumbers($All{"RunNumbers"}));
		my @Hists = split( /,/, $All{"LRBF"} );
		# Write to template file 
		open( OUTF,q{>},"$RUNS[0]_tmpl.msr" );
		print OUTF ("$FullMSRFile");
		close(OUTF);

		# Change runs line in the final global fit
		my $NSpectra = ($#RUNS+1)*($#Hists+1);
		my $NewRunLine = "runs ".join(" ",(1...$NSpectra));
		# Use msr2data to generate global fit MSR file
		my $RunList = join(" ",@RUNS);
		my $StupidName = $RUNS[0]."+global_tmpl.msr";
		my $cmd = "cp $RUNS[0]_tmpl.msr $StupidName";
		#if ($#RUNS != 0) {
		    # For multiple runs create global file
		    $cmd = "msr2data \[".$RunList."\] "." _tmpl msr-".$RUNS[0]." global";
		#} 
		print $cmd."\n";
		my $pid = open(FTO,"$cmd 2>&1 |");
		while (<FTO>) {
		    $self->fitTextOutput->append("$_");
		}
		close(FTO);
		# change the stupid name
		# change stupid default runs line
		$cmd = "cp $StupidName $FILENAME; perl -pi -e 's/runs.*?(?=\n)/$NewRunLine/s' $FILENAME";
		$pid = open(FTO,"$cmd 2>&1 |");
		while (<FTO>) {
		    $self->fitTextOutput->append("$_");
		}
		close(FTO);
		# feed in values of parameters if they exist
		my $wholefile = "";
		my $newline = "";
		{open (MSRF,q{<},"$FILENAME" );
		 local $/ = undef;
		 $wholefile = <MSRF>;
		 close(MSRF);}
		my %PTable=MSR::PrepParamTable(\%All);
		my $NParam=keys( %PTable );
		for (my $iP=0;$iP<$NParam;$iP++) {
		    my ($Param,$value,$error,$minvalue,$maxvalue,$RUNtmp) = split(/,/,$PTable{$iP});
		    if (defined($All{"$Param"})) {
			# replace the corresponding $Param  line
			if ( $All{"$Param\_min"} == $All{"$Param\_max"} ) {
			    $All{"$Param\_min"} = "";
			    $All{"$Param\_max"} = "";
			}
			$newline = join("    ",($Param,$All{"$Param"},$All{"d$Param"},$All{"d$Param"},$All{"$Param\_min"},$All{"$Param\_max"}));
			$wholefile =~ s/$Param.*/$newline/;
		    }
		}
		# Now rewrite the msr file
		open (MSRF,q{>},"$FILENAME" );
		print MSRF $wholefile;
		close(MSRF);
	    }
	    UpdateMSRFileInitTable();
	}
    }
    return $Answer;
}

sub UpdateMSRFileInitTable()
{
    my $FILENAME=$All{"FILENAME"};
    my @lines=();
    if (-e "$FILENAME.msr") {
	open (MSRF,q{<},"$FILENAME.msr" );
	@lines = <MSRF>;
	close(MSRF);
    }
    $self->textMSROutput->setText("");
    foreach my $line (@lines) {
	$self->textMSROutput->insertPlainText("$line");
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
	my $name=$Param[2];
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
	my $QTable = $self->initParamTable;
#	print "PCount=$PCount and value=$value\n";
	if (defined($QTable->item($PCount,0)) & defined($QTable->item($PCount,1)) & defined($QTable->item($PCount,2)) & defined($QTable->item($PCount,3))) {
	    $QTable->setItem($PCount,0,Qt::TableWidgetItem());
	    $QTable->setItem($PCount,1,Qt::TableWidgetItem());
	    $QTable->setItem($PCount,2,Qt::TableWidgetItem());
	    $QTable->setItem($PCount,3,Qt::TableWidgetItem());
	    # Set also label or row
	    $QTable->verticalHeaderItem($PCount)->setText($name);
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
    $All{"LRBF"}=$self->histsLRBF->text;
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
    # Update All from GUI
    %All=CreateAllInput();
    my @RUNS = split( /,/, MSR::ExpandRunNumbers($All{"RunNumbers"}) );
    my @Hists = split( /,/, $All{"LRBF"} );
    
    # Make sure all sharing boxes exist
    addSharingComp();

    my $NShComps = $self->horizontalLayout->count();
    my $sharingComp = "";
    my $i = 1;
    # Hide all sharing components
    for ($i = 1; $i<=$NShComps;$i++) {
	$sharingComp = child("Qt::GroupBox","sharingComp$i");
	$sharingComp->setHidden(1);
	$sharingComp->setEnabled(0);
    }
    
    my @FitTypes =();
    for ($i=1;$i<=$All{"numComps"};$i++)  {
	if ( $All{"FitType$i"} ne "None" ) {
	    push( @FitTypes, $All{"FitType$i"} );	    
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
	
	    if ( $Component == 1 && ($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB")) {
		unshift( @Params, "Alpha" );
	    } elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
		if (grep(/Phi/,@Params)) {
		    # remove the Phi and put it back for each histogram
		    @Params = grep ! /Phi/,@Params;
		    # for each histogram
		    foreach my $Hist (@Hists) {
			# take only first histogram from sum
			($Hist,my $tmp) = split(/ /,$Hist);
			# Doesn't make sense to share No or NBg!
			# unshift( @Params, ( "N0$Hist", "NBg$Hist", "Phi$Hist" ) );
			push(@Params,"Phi$Hist");
		    }
		}
		# unshift( @Params, ( "No", "NBg" ) );
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
	    for ($i=1; $i<=9;$i++) {		
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
    # Update All values from GUI
    %All=CreateAllInput();
    my $QTable = $self->initParamTable;
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

# Fill the table with labels and values of parameter 
    for (my $PCount=0;$PCount<$NParam;$PCount++) {
	my ($Param,$value,$error,$minvalue,$maxvalue,$RUN) = split(/,/,$PTable{$PCount});
#	print $PTable{$PCount}."\n";
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
    my $curTab = $self->musrfit_tabs->currentIndex();
    if ($curTab >= 2 && $curTab <= 4) {
	# First make sure we have sharing initialized    
	ActivateShComp();
	# Here we need to apply sharing if selected...
	InitializeTab();
	UpdateMSRFileInitTable();
    } elsif ($curTab == 6) {
	# And also setup T0 and Bg bins
	ActivateT0Hists();
    } elsif ($curTab == 7) {
	# Initialize FUNCTIONS block only if it has not been initialized yet
	if ($All{"FunctionsBlock"} eq "" ) {
	    InitializeFunctions();
	}
    }
}


sub GoFit()
{
    # This function should be able to do both plot and fit, check who called you.
     %All=CreateAllInput();
# Check here is the number of histograms makes sense
# other wise give error.
    my @Hists = split( /,/, $All{"LRBF"} );
    if (($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB") && $#Hists != 1) {
# we have a problem here send error message
	my $Warning = "Error: The number of histograms should be 2 for an asymmetry fit!";
	my $WarningWindow = Qt::MessageBox::information( this, "Error",$Warning);
    } else {
	$self->musrfit_tabs->setCurrentIndex(1);
	my $Answer=CallMSRCreate();
	my $FILENAME=$All{"FILENAME"}.".msr";
	if ($Answer) {
	    if (-e $FILENAME) {
		my $cmd="musrfit -t $FILENAME; musrview $FILENAME &";
		my $pid = open(FTO,"$cmd 2>&1 |");
		while (<FTO>) {
		    $self->fitTextOutput->insertPlainText("$_");
		}
		close(FTO);
	    } else {
		$self->fitTextOutput->insertPlainText("Cannot find MSR file!");
	    }
	    $self->fitTextOutput->insertPlainText("-----------------------------------------------------------------------------------------------------------------------------");
# update MSR File tab and initialization table
	    UpdateMSRFileInitTable();
	}
    }
    return;
}

sub GoPlot()
{
    # This function should be able to do both plot and fit, check who called you.
    %All=CreateAllInput();
# Check here is the number of histograms makes sense
# other wise give error.
    my @Hists = split( /,/, $All{"LRBF"} );
    if (($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB") && $#Hists != 1) {
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
		$self->fitTextOutput->insertPlainText("Cannot find MSR file!");
		$self->fitTextOutput->insertPlainText("-----------------------------------------------------------------------------------------------------------------------------");
	    }
	}
    }
    return;
}


sub ShowMuSRT0()
{
    # Open musrt0 to check and adjust t0 , Bg and Data bins
    $self->musrfit_tabs->setCurrentIndex(6);
    # Create MSR file and then run musrt0
    my $Answer=CallMSRCreate();
    
    if ($Answer) {
	my $FILENAME=$All{"FILENAME"}.".msr";
	if (-e $FILENAME) {
	    my $cmd="musrt0 $FILENAME &";
	    my $pid = system($cmd);
	    $self->t0Update->setEnabled(1)
	} else {
	    print STDERR "Cannot find MSR file!\n";
	}
    }
    return;
}


sub t0Update()
{
    $All{"LRBF"}=$self->histsLRBF->text;
    my @Hists = split(/,/, $All{"LRBF"} );
    
# Get values of t0 and Bg/Data bins if given
    my $NHist = 1;
    foreach my $Hist (@Hists) {
	foreach ("t0","Bg1","Bg2","Data1","Data2") {
	    my $Name = "$_$NHist";
	    my $tmp=MSR::T0BgData($_,$Hist,$All{"BeamLine"});
	}
	$NHist++
    }
}


sub RunSelectionToggle()
{
    my $ManualFile= $self->manualFile->isChecked();
    if ($ManualFile) {
# Manual RUN selection
	$self->runsMan->setEnabled(1);
	$self->runsMan->setHidden(0);
	$self->runNumbers->setText("");
	$self->runsAuto->setEnabled(0);
	$self->runsAuto->setHidden(1);
    } else {
# Auto RUN selection
	$self->runsMan->setEnabled(0);
	$self->runsMan->setHidden(1);
	$self->runFiles->setText("");
	$self->runsAuto->setEnabled(1);
	$self->runsAuto->setHidden(0);
    }
    
}

sub fileBrowse()
{
    my $RunFiles=$self->runFiles->text();
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
    $self->runFiles->setText($RunFiles);
}

sub AppendToFunctions()
{
    my $ParName=$self->cParamsCombo->currentText();
    my $Full_T_Block="";
    my $Constraint="";
    if (defined($self->theoryBlock->toPlainText)) {
	$Full_T_Block=$self->theoryBlock->toPlainText;
    }
    if (defined($self->constraintLine->text)) {
	$Constraint=$self->constraintLine->text;
    }
# Then clear the text
    $self->constraintLine->setText("");
    
# Check how many constraints (lines) in FUNCTIONS Block    
#    my $i=$self->functionsBlock->blockCount();
    my $fun_lines=$self->functionsBlock->toPlainText();
    my $i= ($fun_lines =~ tr/\n//)+1;
    my $ConstLine="fun$i = $Constraint\n";
    $self->functionsBlock->insertPlainText($ConstLine);
    
# Replace parameter in theory block with fun$i
    $Full_T_Block=~ s/$ParName/fun$i/;
    $self->theoryBlock->setText($Full_T_Block);
}

sub InitializeFunctions()
{
    # Update All values from GUI
    %All=CreateAllInput();
    my @RUNS = split( /,/, MSR::ExpandRunNumbers($All{"RunNumbers"}) );
    my @Hists = split( /,/, $All{"LRBF"} );
    my @FitTypes =();
    for (my $i=1;$i<=$All{"numComps"};$i++)  {
	if ( $All{"FitType$i"} ne "None" ) {
	    push( @FitTypes, $All{"FitType$i"} );	    
	}
    }
    
# Get number of parameters to determine the size of the table 
    my ($Full_T_Block,$Paramcomp_ref)= MSR::CreateTheory(@FitTypes);
    my @Paramcomp = @$Paramcomp_ref;
    $Full_T_Block= $All{"Full_T_Block"};
    
# Initialize Parameters List in function block (constraints).    
    my $ParametersList="";
    $self->parametersList->setText("");
# Counter for function block (with out Alpha etc.)
    my $ParCount=0;
    $self->cParamsCombo->clear();
    
# Possibly use the parameters block to axtract names for the dropdown menu
# this makes sense if we can use fun in map line. Check!
    my $Component=1;
    foreach my $FitType (@FitTypes) {
	my $Parameters=$Paramcomp[$Component-1];
	my @Params = split( /\s+/, $Parameters );	

# Alpha, No and NBg are counted in the parameters
	if ( $Component == 1 && ($All{"FitAsyType"} eq "Asymmetry" || $All{"FitAsyType"} eq "AsymmetryGLB" )) {
	    unshift( @Params, "Alpha" );
	} elsif ( $Component == 1 && $All{"FitAsyType"} eq "SingleHist" ) {
	    if (grep(/Phi/,@Params)) {
		# remove the Phi and put it back for each histogram
		@Params = grep ! /Phi/,@Params;
		# for each histogram
		foreach my $Hist (@Hists) {
		    # take only first histogram from sum
		    ($Hist,my $tmp) = split(/ /,$Hist);
		    # Doesn't make sense to share No or NBg!
		    # unshift( @Params, ( "N0$Hist", "NBg$Hist", "Phi$Hist" ) );
		    push(@Params,"Phi$Hist");
		}
	    }
	    # unshift( @Params, ( "No", "NBg" ) );
	}
	
# Add list to the constraints drop down menu
	for (my $i=1; $i<=9;$i++) {		
	    if (defined($Params[$i-1])) {
		my $CParam = $Params[$i-1]."_".$Component;
		if ($Params[$i-1] ne "Alpha" && $Params[$i-1] ne "No" && $Params[$i-1] ne "NBg") {
		    $self->cParamsCombo->addItem($CParam);
		    $Full_T_Block=~ s/\b$Params[$i-1]\b/$CParam/;
		}
# also enumerate the parameters as should be used in the FUNCTIONS Block
		$ParCount++;
		$ParametersList=$ParametersList."$CParam \t is \t par$ParCount\n";
		$self->parametersList->setText($ParametersList);
	    }
	}
	$Component++;
    }  
# Set theory block in Constraints    
    $self->theoryBlock->setText($Full_T_Block);
# Then clear the text
    $self->constraintLine->setText("");
    $self->functionsBlock->setText("");
}




sub t0UpdateClicked()
{
    # Read MSR file and get new values of t0,Bg and Data
    #%All=CreateAllInput();      
    my $FILENAME=$All{"FILENAME"};
    open (MSRF,q{<},"$FILENAME.msr" );
    my @lines = <MSRF>;
    close(MSRF);
    
    my @T0s = grep {/t0 /} @lines;
    my @Bgs = grep {/background /} @lines;
    my @Datas = grep {/data /} @lines;

    my @Hists = split(/,/, $All{"LRBF"} );
    my $NHist = $#Hists+1;
    
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
    $self->t0Update->setEnabled(0);
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

# Return handle on widget
    return($Attrib);
}

sub addFitType {
    # Input number of components
    my $numComps = $self->numComps->value;
    # count number of exisitng components
    my $NButtons = $self->columnView->count();
    # create ComboBox in theoryFunction parent
    my $fitType = Qt::ComboBox( $self->theoryFunction );

    # do we have less components that we need
    if ($numComps > $NButtons) {
	# add as needed
	for (my $i=$NButtons+1;$i<=$numComps;$i++) {
	    my $nameComp = "fitType$i";
	    $self->columnView->addWidget( $fitType );
	    $fitType->setObjectName( $nameComp );
	    $fitType->setMinimumSize( Qt::Size(0, 25) );
	    $fitType->setMaximumSize( Qt::Size(255, 25) );
	    $fitType->insertItems(0,
	      ["Exponential",
	       "Gaussian",
	       "Stretch Exp.",
	       "Exponential Cos",
	       "Gaussian Cos",
	       "Stretch Cos",
	       "Lorentzian Dynamic KT",
	       "Gaussian Dynamic KT",
	       "Background",
	       "Lorentzian Kubo-Toyabe LF x Exp",
	       "Gaussian Kubo-Toyabe LF x Exp",
	       "Lorentzian Kubo-Toyabe LF x Str Exp",
	       "Gaussian Kubo-Toyabe LF x Str Exp",
	       "MolMag",
	       "Meissner State Model",
	       "Lor-Gss combi KT",
	       "Lor-Gss combi KT x Exp",
	       "Lor-Gss combi KT x SExp",
	       "None",]);
	    if ($i==1) {
		$fitType->setCurrentIndex(0);
		# remove the "None" option for the 1st component
		$fitType->removeItem(18);
	    } else {
		$fitType->setCurrentIndex( 18 );
	    }
	}
    } else {
	for (my $i=$NButtons;$i > $numComps ;$i--) {
	    # component is there, delete it
	    my $widget = $self->columnView->itemAt($i-1)->widget();
	    $widget -> hide();
	    $self->columnView->removeWidget($widget);
	    undef $widget;
	}
    }		
}

sub addSharingComp {
    # Input number of components
    my $numComps = $self->numComps->value;
    # count number of exisitng components
    my $NShComps = $self->horizontalLayout->count();
    my @sharingComps = ();
    for (my $i=$NShComps+1;$i<=$numComps;$i++) {
	$sharingComps[$i] = Qt::GroupBox( $self->buttonGroupSharing );
	my $sizePolicy = Qt::SizePolicy( Qt::SizePolicy::Expanding(), Qt::SizePolicy::MinimumExpanding() );
	$self->{$sizePolicy} = $sizePolicy;
	$sizePolicy->setHorizontalStretch( 0 );
	$sizePolicy->setVerticalStretch( 0 );
	$sizePolicy->setHeightForWidth( $sharingComps[$i]->sizePolicy()->hasHeightForWidth() );
	# check is this component exists
	if (child("Qt::GroupBox","sharingComp$i") == 0) {
	    $sharingComps[$i]->setObjectName( "sharingComp$i" );
	    $sharingComps[$i]->setEnabled( 0 );
	    $sharingComps[$i]->setSizePolicy( $sizePolicy );
	    $sharingComps[$i]->setAlignment( Qt::AlignLeading() );
	    $sharingComps[$i]->setTitle("Component $i");
	    my $layout = Qt::Widget( $sharingComps[$i] );
	    $layout->setObjectName( "layoutSh$i" );
	    $layout->setGeometry( Qt::Rect(40, 60, 81, 266) );
	    my $shBoxLayout = Qt::VBoxLayout( $layout );
	    $self->{shBoxLayout} = $shBoxLayout;
	    $shBoxLayout->setSpacing( 0 );
	    $shBoxLayout->setMargin( 0 );
	    $shBoxLayout->setObjectName( "shBoxLayout$i" );
	    $shBoxLayout->setContentsMargins(0, 0, 0, 0 );
	    my @shParam = ();
	    for (my $j=1;$j<=9;$j++) {
		$shParam[$j] = Qt::CheckBox( $layout );
		$self->{shParam} = $shParam[$j];
		$shParam[$j]->setObjectName( "shParam_".$i."_$j" );
		$shParam[$j]->setText("Param$j");
		$shBoxLayout->addWidget( $shParam[$j] );
	    }
		
	    my $compShLabel = Qt::Label( $sharingComps[$i] );
	    $self->{compShLabel} = $compShLabel;
	    $compShLabel->setObjectName( "comp".$i."ShLabel" );
	    $compShLabel->setGeometry( Qt::Rect(2, 30, 141, 20) );
	    $compShLabel->setWordWrap( 0 );
	
	    $self->horizontalLayout->addWidget( $sharingComps[$i] );
	    # Connect slot
	    Qt::Object::connect($sharingComps[$i], SIGNAL 'currentIndexChanged()' , $self, SLOT 'InitializeFunctions()' );
	    $compShLabel->setText("FitType$i");
	} else {
	    print "Exists, skip component $i\n";
	}
    }
}

sub runsLineNotEmpy()
{
}

1;
