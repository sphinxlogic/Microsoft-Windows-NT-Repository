
package Script;

BEGIN {
    use Exporter   ();
    use Safe;
    use TieENV;
    use vars       qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS);

    # set the version for version checking
    $VERSION     = 1.10;

    @ISA         = qw(Exporter Safe);
    @EXPORT      = qw(FindScript);
    %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],
}


$safeid = 0;
$module_debug  = 0;

sub new {
    my $pkg = shift;
    my $script = shift;
    my $obj;
    my ($safename) = sprintf('SAFE%08d',$safeid++);

    $obj = new Safe $safename;
    $obj->{SCRIPT} = $script;
    $obj->{INC} = {};
    $obj->{MODTIME} = (stat($script))[9];
    $obj->{DEBUG} = $^D;
    $obj->{WARN} = $^W;
    $obj->{VIRGIN} = 1;
    $obj->{TIMEOUT} = 0;
    $obj->{DATA} = undef;
    $obj->{ENV} = new TieENV $safename;

    bless $obj, $pkg;
    $ScriptIndex{$script} = $obj;
    return $obj;
}

sub DESTROY {
    my $self = shift;

    print STDERR "In SCRIPT::DESTROY, for ",$self->{SCRIPT},"\n" if $module_debug;
    delete($ScriptIndex{$self->{SCRIPT}});
    Safe::DESTROY($self);
}

sub FindScript {
    my $script = shift;

    if (exists($ScriptIndex{$script})) {
        return $ScriptIndex{$script};
    } else {
        return undef;
    }
}


sub renew {
    my $self = shift;
    my $script = $self->{SCRIPT};
    my $modtime = (stat($script))[9];

    return ($modtime > $self->{MODTIME});
}


sub run {
    my $self = shift;
    my $file = $self->{SCRIPT};
    my $root = $self->{Root};
    my $debug = $self->{DEBUG};
    my $warn = $self->{WARN};
    my $timeout = $self->{TIMEOUT};
    my $status;
    my $F;
    $DIEfile = $file;
    my (@INC0) = @INC;
    local @INC = @INC0;
    local %INC = %{$self->{INC}};

    if ($self->{VIRGIN}) {
        print STDERR "reloading script\n" if $module_debug;
        my $line;
        my $ialarm = '';
        my $falarm = '';
        if ($timeout) {
            $ialarm = 'local $SIG{ALRM} = sub { die "timeout\n"}; alarm '.$timeout.';' ;
            $falarm = '; alarm 0';
        }

        local $SIG{__DIE__} = \&DIE_Handler;
        $DIEmsg = undef;

        $F = VMS::Stdio::vmsopen("<$file","shr=get");
        die("unable to open script file $file") unless defined($F);
        my @p;
        $self->{DATA} = undef;
        while (defined($_ = <$F>)) {
            if (/^\s*__(DATA|END)__\s*\n$/) {
                $self->{DATA} = $F;
                last;
            }
            push @p, $_;
        }
        close($F) unless defined($self->{DATA});
        *{$root."::DATA"} = $F;

        if ($p[0] =~ /^\#\!\s*/) {
            $p[0] = '';
            my $opts = $';
            chomp($opts);
            my (@opts) = split(' ',$opts);
            while (defined($_ = shift(@opts))) {
                last if (/perl(\.[^\/\.\]\>\:]*)?$/i) ;
            }
            while (defined($_ = shift(@opts))) {
                last unless /^\-/;
                last if $_ eq '--';
                if (/^\-I/) {
                    unshift(@INC,$') if $';
                    print STDERR "adding $' to \@INC\n" if $module_debug;
                }
                if (/^\-D/) {
                    $debug = $';
                }
                if ($_ eq '-w') {
                    $warn = 1;
                }
            }
        }

        @{$root."::INC"} = @INC;
        %{$root."::INC"} = %INC;
        my $expr = join('',@p);
        $line  = sprintf('package %s; use subs qw(exit); sub { %s ', $root, $ialarm);
        $line .= 'local $^A; local $^C; local $^F; local $^I; local $^P;';
#        $line .= 'local $#;';      ### causes weird problems ###
        $line .= 'local $%;';
        $line .= 'local $,;';
        $line .= 'local $-; local $=; local $?; local $\; local $^; local $~;';
        $line .= sprintf('local $^D = %d; local $^W = %d;',$debug, $warn);
        $line .= 'local $^H = '.$^H.';';
        $line .= 'local $[ = 0;';
        $line .= '$/ = qq(\n);';
        $line .= 'local $" = q( );';
        $line .= 'local $| = 1;';

        $line .= 'eval $expr;'.$falarm.'}';

        print STDERR " eval ($line)\n" if ($module_debug > 1);
        print STDERR "\@INC = (",join(', ',@INC),")\n" if $module_debug;
        $self->{SUB} = eval($line);
        %{$self->{INC}} = %INC;

        if ($DIEmsg) {
            print STDERR $DIEmsg,"\n";
            $@ = $DIEmsg;
            return undef;
        }
        $self->{VIRGIN} = 0;
    }

    $status = $self->rdo2();
    %{$self->{INC}} = %INC;
    $^O = 'VMS' if $^O ne 'VMS';

    my $Reuse = defined(${$root."::CRINOID::Reuse"}) && ${$root."::CRINOID::Reuse"};
    $Reuse ||= defined(${$root."::SQUID::Reuse"}) && ${$root."::SQUID::Reuse"};

    if (defined($self->{DATA}) && (eof($self->{DATA}) || !$Reuse)) {
        close($self->{DATA});
        $self->{DATA} = undef;
    }
    return $status;
}

sub rdo2 {
    my $self = shift;
    my $root = $self->{Root};
    local $SIG{__DIE__} = \&DIE_Handler;
    $DIEmsg = undef;
    $^T = time;
    $_ = @_ = undef;
    my $status = Opcode::_safe_call_sv($root, $self->{Mask}, $self->{SUB});
    if ($DIEmsg) {
        $@ = '';
        if ($DIEmsg =~ /^exit\((\d+)\)/) {
            return $status if $1 == 0;
            my (@m) = split(/\|/,$DIEmsg);
            $DIEmsg = $m[0];
        } elsif ($DIEmsg =~ /at \/CRINOID_ROOT\/lib\/script.pm/i) {
            $DIEmsg = $`;
        }
        print STDERR $DIEmsg," status = $status\n";
        $@ = $DIEmsg;
        $status = undef;
    }
    return $status;
}

sub init_stash {
    my $self = shift;
    my $modname = shift;

    $modname =~ s#::$##;
    if ($modname =~ /::/) {
        $self->reval('$'.$modname.'::_CRINOID_initstash_dummy_=1;');
    }
}

sub share_module {
    my $self = shift;
    my $modname = shift;
#    my ($filename, $k, $v);
    my $root = $self->{Root};

    $modname =~ s#::$##;
    *{$root."::".$modname."::"} = \*{$modname."::"};

    $filename = $modname;
    $filename =~ s#::#/#g;
    $filename .= '.pm';
    while (($k,$v) = each(%INC)) {
        if (uc($k) eq uc($filename)) {
            $self->{INC}->{$k} = $v;
            last;
        }
    }
}

sub DIE_Handler {
    $DIEmsg = shift;
    $DIEmsg =~ s/\(eval \d+\)/$DIEfile/;
    die($DIEmsg);
}


sub DEBUG {
    my $s = shift;
    my $comment = shift;
    my $root = $s->{Root};

    my %h = %{$root."::"};
    my $c = $h{'CGI::'};
    my $nk = 0;

#    print STDERR "symbol table, at $comment\n";
    foreach (keys(%{$c})) {
        $nk++;
#        print STDERR "$comment: $root","::CGI::{'$_'} \n"
    }
    print STDERR "$comment $root","::CGI ... $nk symbols\n";

    foreach (keys(%main::INC)) {
        print STDERR "$comment: \$main::INC{$_}\n";
    }

    foreach (keys(%{$s->{INC}})) {
        print STDERR "$comment: $root","::INC{$_}\n";
    }
}
1;

