#! perl
#
#   example of using PGPLOT and CGI
#
#   This script does a linear regression fit of data entered via a form.
#   Fit parameters and a plot are displayed.
#
#   HTML NOTE: &#177;  -> +-
#
#   you need to modify location of $WWDIR for your own setup.
#
#   Author: C. Lane   lane@duphy4.physics.drexel.edu
#
use CGI;
use PGPLOT;

$DEBUG = 0;  # if set, writes plot to file and keeps datafile
$CRINOID::Reuse = 1;
$WWDIR = 'sys$scratch:';    # world-writable directory

$q = new CGI;

%SYM = ('+'=>2, 'o'=>4, 'x'=>5, '*'=>12) unless defined(%SYM);
$N    = $N2   = 0;
$XLOG = $YLOG = 0;
undef(@X);
undef(@Y);
undef(@X2);
undef(@Y2);
$XLAB   = 'X';
$YLAB   = 'Y';
$TITLE  = 'Linear Regression Plot';
$OLEG  = $PLEG  = $XLEG  = $SLEG  = '';


#
#   if we have an ID, then form was processed and we're making a plot
#

if ($q->param('ID') ne '') {
    do_plot($q);
} else {
    do_form($q);
}


sub do_plot
{
    my $q = shift;
    my ($xmax, $xmin, $ymax, $ymin, $xlo, $xhi, $ylo, $yhi);
    my (@u, @v);
    my $legy = -2;
    my $legx = 0.1;
    my $legj = 0.0;

    ReadParams($q->param('ID'),1);      # read & delete file

    print "Content-type: image/gif\n\n";
    PGPLOT::pgbeg(0,$DEBUG? 'linear.gif/GIF' : '-/GIF',1,1);
    PGPLOT::pgpap(450./85.,1.);
    PGPLOT::pgscr(0,1.,1.,1.);
    PGPLOT::pgscr(1,0.,0.,0.);

    ($xmax,$xmin) = maxmin(@X2);
    ($ymax,$ymin) = maxmin(@Y2);
    if ($xmax <= $xmin) {$xmax = $xmin + 0.1; $xmin -= 0.1;}
    if ($ymax <= $ymin) {$ymax = $ymin + 0.1; $ymin -= 0.1;}

    PGPLOT::pgrnge($xmin,$xmax,$xlo,$xhi);
    PGPLOT::pgrnge($ymin,$ymax,$ylo,$yhi);
    PGPLOT::pgenv($xlo,$xhi,$ylo,$yhi,0,0 + ($XLOG? 10:0)+($YLOG? 20:0));
    PGPLOT::pglab($XLAB,$YLAB,$TITLE);
    PGPLOT::pgpnts($N2,\@X2,\@Y2,\@S2,$N2);

    my ($m, $b) = regress($N2,\@X2,\@Y2);
    if (defined($m)) {
        @u = ($xlo, $xhi);
        @v = ($m*$xlo+$b, $m*$xhi+$b);
        PGPLOT::pgline(2,\@u,\@v);

        $legx = 0.5 if $m < 0;

        if (!$XLOG && !$YLOG) {
            PGPLOT::pgmtxt('T',$legy--, $legx, $legj, sprintf('Fit: \\fiy\\fn = (%5.3g) \\fix\\fn + (%5.3g)',$m,$b));
        } elsif ($XLOG && !$YLOG) {
            PGPLOT::pgmtxt('T',$legy--, $legx, $legj, sprintf('Fit: \\fiy\\fn = (%5.3g) log(\\fix\\fn) + (%5.3g)',$m,$b));
        } elsif (!$XLOG && $YLOG) {
            PGPLOT::pgmtxt('T',$legy--, $legx, $legj, sprintf('Fit: \\fiy\\fn = (%5.3g) (%5.3g)\\u\\fix\\fn\\d',10**$b,10**$m));
        } else {
            PGPLOT::pgmtxt('T',$legy--, $legx, $legj, sprintf('Fit: \\fiy\\fn = (%5.3g) \\fix\\fn\\u (%5.3g)\\d',10**$b,$m));
        }
    }

    PGPLOT::pgmtxt('T',$legy--,$legx,$legj,"\\m4 $OLEG") if $OLEG ne '';
    PGPLOT::pgmtxt('T',$legy--,$legx,$legj,"\\m2 $PLEG") if $PLEG ne '';
    PGPLOT::pgmtxt('T',$legy--,$legx,$legj,"\\m5 $XLEG") if $XLEG ne '';
    PGPLOT::pgmtxt('T',$legy--,$legx,$legj,"\\m12 $SLEG") if $SLEG ne '';

    PGPLOT::pgend;
}




sub process_data
{
    my $q = shift;
    my ($j, $n, $tx);

    $j = $N = $N2 = 0;

    if ($q->param) {
        $TITLE = $q->param('title');
        $XLAB  = $q->param('xlabel');
        $YLAB  = $q->param('ylabel');
        $XLOG  = $q->param('xscale') eq 'log';
        $YLOG  = $q->param('yscale') eq 'log';

        $OLEG  = $q->param('oleg');
        $PLEG  = $q->param('pleg');
        $XLEG  = $q->param('xleg');
        $SLEG  = $q->param('sleg');

        while (defined($q->param("X$j"))) {
            if ($q->param("delete_$j") || $q->param("X$j") eq '' || $q->param("Y$j") eq '') {
                $j++;
                next;
            }
            $EM[$N] = '';

            $tx = $q->param("X$j");
            if (validate_number($tx)>0) {
                $X[$N] = sprintf("%g",$tx);
                if ($XLOG && $tx <= 0) {
                    $EM[$N] = 'Need X>0 for log scale ';
                }
            } else {
                $EM[$N] = 'Error parsing X';
            }

            $tx = $q->param("Y$j");
            if (validate_number($tx)>0) {
                $Y[$N] = sprintf("%g",$tx);
                if ($YLOG && $tx <= 0) {
                    $EM[$N] .= 'Need Y>0 for log scale ';
                }
            } else {
                $EM[$N] .= 'Error parsing Y ';
            }

            $S[$N] = $q->param("S$j");

            if ($EM[$N] eq '') {
                $X2[$N2] = $XLOG ? log10($X[$N]) : $X[$N];
                $Y2[$N2] = $YLOG ? log10($Y[$N]) : $Y[$N];
                $S2[$N2] = $SYM{$S[$N]};
                $N2++;
            }

            $j++;
            $N++;
        }
    }
    return $N;
}




sub do_form
{
    my $q = shift;

    process_data($q);       # process any input data first

    print $q->header;
    print $q->start_html("Linear Regression Utility");
    print "<H1>Linear Regression Utility</H1>\n";

    print $q->startform;

    print "<TABLE>\n";
    print "<TR><TD>Plot title:<TD>",$q->textfield(-name=>'title', -override=>1, -default=>$TITLE, -size=>20, -maxlength=>64),"\n";
    print "<TR><TD>X-axis label:<TD>",$q->textfield(-name=>'xlabel', -override=>1, -default=>$XLAB, -size=>20, -maxlength=>64),"<TD>",
           $q->radio_group(-name=>'xscale', -values=>['linear', 'log'], -default=>'linear', -columns=>2),"\n";
    print "<TR><TD>Y-axis label:<TD>",$q->textfield(-name=>'ylabel', -override=>1, -default=>$YLAB, -size=>20, -maxlength=>64),"<TD>",
           $q->radio_group(-name=>'yscale', -values=>['linear', 'log'], -default=>'linear', -columns=>2),"\n";
    print '<TR><TD>o legend:<TD>',$q->textfield(-name=>'oleg', -override=>1, -default=>$OLEG, -size=>20, -maxlength=>64);
    print '<TR><TD>+ legend:<TD>',$q->textfield(-name=>'pleg', -override=>1, -default=>$PLEG, -size=>20, -maxlength=>64);
    print '<TR><TD>x legend:<TD>',$q->textfield(-name=>'xleg', -override=>1, -default=>$XLEG, -size=>20, -maxlength=>64);
    print '<TR><TD>* legend:<TD>',$q->textfield(-name=>'sleg', -override=>1, -default=>$SLEG, -size=>20, -maxlength=>64);

    print "</TABLE>\n";

    print "<TABLE BORDER>\n";
    print "<TR><TD>Pt#<TD>X<TD>Y<TD>Symbol\n";

    for ($j = 0; $j < $N; $j++) {
        if ($EM[$j] eq '') {
            print $q->hidden(-name=>"X$j", -override=>1, -value=>$X[$j]);
            print $q->hidden(-name=>"Y$j", -override=>1, -value=>$Y[$j]);
            print $q->hidden(-name=>"S$j", -override=>1, -value=>$S[$j]);
            print '<TR><TD>',$j+1,'<TD>',$X[$j],'<TD>',$Y[$j],'<TD>',$S[$j];
            print '<TD>';
        } else {
           print '<TR><TD>',$j+1;
           print '<TD>',$q->textfield(-name=>"X$j",-default=>$X[$j],-override=>1,-size=>10,-maxlength=>10);
           print '<TD>',$q->textfield(-name=>"Y$j",-default=>$Y[$j],-override=>1,-size=>10,-maxlength=>10);
           print '<TD>',$q->radio_group(-name=>"S$j", -values=>['o', '+', 'x', '*'], -default=>'o'),"\n";
           print '<TD>',$EM[$j];
        }
        print '<TD>',$q->checkbox(-name=>"delete_$j",-override=>1,-label=>"Delete?");
    }

    for ($j = $N; $j < $N+4; $j++) {
        print "<TR><TD>",$j+1,"<TD>";
        print $q->textfield(-name=>"X$j",-default=>'',-override=>1,-size=>10,-maxlength=>10);
        print "<TD>";
        print $q->textfield(-name=>"Y$j",-default=>'',-override=>1,-size=>10,-maxlength=>10);
        print '<TD>',$q->radio_group(-name=>"S$j", -values=>['o', '+', 'x', '*'], -default=>'o'),"\n";
        print "<TD><TD>";
    }
    print '<TR><TD>',$q->submit(-name=>'Update');
    print '<TD>',$q->reset;
    print "</TABLE>\n";
    print $q->endform;



    if ($N2 > 1) {
        my ($m,$b) = regress($N2,\@X2,\@Y2);

        print "<HR>\n<H2>Linear Regression:</H2>\n";
        if (defined($m)) {
            print '<TABLE><TR><TD>',$YLOG? 'log(y)':'y','= m ',$XLOG? 'log(x)':'x',' + b';
            print '<TR><TD>Slope (m):<TD>',sprintf("%5.3g\n",$m);
            print '<TR><TD>Intercept (b):<TD>',sprintf("%5.3g\n",$b);
            print "</TABLE>\n";
        } else {
            print "<B>Error! Infinite slope</B>\n";
        }
    }


    if ($N2 > 2) {
        print "<HR>\n";
        $ID = SaveParams($q);
        print '<IMG SRC="',$q->url(),'?ID=',$ID,'" ALT="Fit Plot">',"\n";
    }
    print $q->end_html;
}


sub ReadParams {
    my $ID = shift;                 # ID of saved data
    my $dflag = shift;              # deletion flag
    local *SAVEFILE;
    return unless defined($ID);
    return unless open(SAVEFILE, "<$WWDIR$ID");

    my $q2 = new CGI SAVEFILE;

    process_data($q2);

    close SAVEFILE;
    unlink "$WWDIR$ID" if !$DEBUG && defined($dflag) && $dflag;
    return $ID;
}

sub SaveParams {
    my $q = shift;
    local *SAVEFILE;
    srand unless defined($DidSrand);
    $DidSrand = 1;

    my $ID = sprintf('LINEAR_%08.8x',rand(0xFFFFFF));  # generate random ID

    if (open(SAVEFILE,">$WWDIR$ID")) {
        $q->save(SAVEFILE);
        close(SAVEFILE);
    } else {
        print "<B>Error Saving CGI state!</B>\n";
        return undef;
    }
    return $ID;
}

#
#   checks that text number is valid format and within limits
#

sub validate_number {
    my ($v, $ll, $ul) = @_;
    my ($v2,$ret);

    $v2 = $v;
    $v =~ s/\s*//g;
    return -1 if (length($v) == 0);
    $ret = 1;
    if ($v =~ /^[+-]?\d*(\.\d*)?([Ee][+-]?\d+)?$/) {
        if ((defined($ll) && $v < $ll) || (defined($ul) && $v > $ul)) {
            $ret = 0;
        }
    } else {
        $ret = -1;
    }
    return $ret;
}



#
#   do linear-regression, assumes all data points have same error
#

sub regress {
    my $n = shift;
    my $x = shift;  # ref
    my $y = shift;  # ref

    my $Sum1  = $n;
    my $SumX  = 0.;
    my $SumY  = 0.;
    my $SumXX = 0.;
    my $SumXY = 0.;
    my $SumYY = 0.;

    my ($j, $tx, $ty);

    for ($j = 0; $j < $n; $j++) {
        $tx = $x->[$j];
        $ty = $y->[$j];
        $SumX  += $tx;
        $SumY  += $ty;
        $SumXX += $tx*$tx;
        $SumXY += $tx*$ty;
        $SumYY += $ty*$ty;
    }

    my $D = $Sum1*$SumXX - $SumX*$SumX;
    my $b = ($SumY * $SumXX - $SumXY * $SumX);
    my $m = ($SumXY*$Sum1 - $SumY*$SumX);
    return undef if abs($D) < 1.e-10;
    return ($m/$D, $b/$D);
}


sub log10 {
    $ILOG10 = 1./log(10.) unless defined($ILOG10);
    return log(shift)*$ILOG10;
}


sub maxmin {
    my (@v) = @_;
    my ($max, $min);

    $max = $min = $v[0];
    foreach (@v) {
        $max = $max < $_ ? $_ : $max;
        $min = $min > $_ ? $_ : $min;
    }
    return ($max,$min);
}

