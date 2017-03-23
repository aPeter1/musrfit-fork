#!/usr/bin/perl

use strict;
use warnings;

use QtCore4;
use QtGui4;
use MuSRFit4;

sub main {
    my $app = Qt::Application(\@ARGV);
    my $musrfit = MuSRFit4();
    $musrfit->show();
    exit $app->exec();
}

main();
