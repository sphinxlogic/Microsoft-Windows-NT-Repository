#! perl

package TieENV;

BEGIN {
    use vars       qw($VERSION @ISA @EXPORT @EXPORT_OK %EXPORT_TAGS);

    # set the version for version checking
    $VERSION     = 0.1;

    @ISA         = qw();
    @EXPORT      = qw();
    %EXPORT_TAGS = ( );     # eg: TAG => [ qw!name1 name2! ],
}



sub new {
    my $class = shift;
    my $basename = shift;
    my $self = {
        VALUE => {},
        STATIC => {},
        FAKE => {},
        VOLATILE => {},
        PERSISTENT => {},
        CHANGED => {},
        BASENAME => $basename,
        TYPE => 'OBJECT',
    };

    bless $self, $class;

    tie(%{$basename."::ENV"}, 'TieENV', $self);
    return $self;
}



sub static {
    my $self = shift;
    my $k, $k2;

    foreach $k2 (@_) {
        $k = uc($k2);
        $self->{VALUE}->{$k} = $ENV{$k};
        $self->{STATIC}->{$k} = 1;
        delete($self->{FAKE}->{$k});
        delete($self->{VOLATILE}->{$k});
        delete($self->{PERSISTENT}->{$k});
    }
}

sub fake {
    my $self = shift;
    my %hash = (@_);
    my ($k, $k2, $v);

    while (($k2,$v) = each(%hash)) {
        $k = uc($k2);
        $self->{VALUE}->{$k} = $v;
        $self->{FAKE}->{$k} = $v;
        delete($self->{STATIC}->{$k});
        delete($self->{VOLATILE}->{$k});
        delete($self->{PERSISTENT}->{$k});
    }
}

sub volatile {
    my $self = shift;
    my $k, $k2;

    foreach $k2 (@_) {
        $k = uc($k2);
        $self->{VALUE}->{$k} = $ENV{$k};
        $self->{VOLATILE}->{$k} = $ENV{$k};
        delete($self->{STATIC}->{$k});
        delete($self->{FAKE}->{$k});
        delete($self->{PERSISTENT}->{$k});
        $self->{CHANGED}->{$k} = 0;
    }
}

sub persistent {
    my $self = shift;
    my $k, $k2;

    foreach $k2 (@_) {
        $k = uc($k2);
        $self->{VALUE}->{$k} = $ENV{$k};
        $self->{PERSISTENT}->{$k} = 1;
        delete($self->{STATIC}->{$k});
        delete($self->{FAKE}->{$k});
        delete($self->{VOLATILE}->{$k});
    }
}


sub reset {
    my $self = shift;
    my $k;

    foreach $k (keys(%{$self->{VOLATILE}})) {
        if ($self->{CHANGED}->{$k}) {
            if (defined($self->{VOLATILE}->{$k})) {
                $ENV{$k} = $self->{VOLATILE}->{$k};
            } else {
                delete($ENV{$k});
            }
        }
    }
    $self->{CHANGED} = {};

    foreach $k (keys(%{$self->{VALUE}})) {
        next if exists($self->{STATIC}->{$k});
        next if exists($self->{FAKE}->{$k});
        next if exists($self->{VOLATILE}->{$k});
        next if exists($self->{PERSISTENT}->{$k});
        delete($self->{VALUE}->{$k});
    }
}

sub initialize {
    my $self = shift;
    my $k;

    foreach $k (keys(%{$self->{FAKE}})) {
        $self->{VALUE}->{$k} = $self->{FAKE}->{$k};
    }

    foreach $k (keys(%{$self->{STATIC}})) {
        $self->{VALUE}->{$k} = $ENV{$k};
    }

}



sub TIEHASH {
    my $class = shift;
    my $parent = shift;
    my $self = {
        PARENT => $parent,
        TYPE => 'HASH',
    };

    bless $self, $class;
    return $self;
}

sub FETCH {
    my $self = shift;
    my $k = uc(shift);

    if (exists($self->{PARENT}->{VOLATILE}->{$k}) or
        exists($self->{PARENT}->{PERSISTENT}->{$k})) {
        $self->{PARENT}->{VALUE}->{$k} = $ENV{$k};
    }
    return $self->{PARENT}->{VALUE}->{$k};
}

sub STORE {
    my $self = shift;
    my $k = uc(shift);
    my $v = shift;

    $self->{PARENT}->{VALUE}->{$k} = $v;
    if (exists($self->{PARENT}->{VOLATILE}->{$k})) {
        $ENV{$k} = $v;
        $self->{PARENT}->{CHANGED}->{$k} = 1;
    } elsif (exists($self->{PARENT}->{PERSISTENT}->{$k})) {
        $ENV{$k} = $v;
    }
}

sub DELETE {
    my $self = shift;
    my $k = uc(shift);

    delete($self->{PARENT}->{VALUE}->{$k});
    delete($ENV{$k}) if exists($self->{PARENT}->{DYNAMIC}->{$k});
}

sub CLEAR {
    my $self = shift;
    my $k;

    foreach $k (keys(%{$self->{PARENT}->{VALUE}})) {
        delete($self->{PARENT}->{VALUE}->{$k});
    }
}

sub EXISTS
{
    my $self = shift;
    my $k = uc(shift);

    return exists($self->{PARENT}->{VALUE}->{$k});
}

sub FIRSTKEY
{
    my $self = shift;

    my $a = keys %{$self->{PARENT}->{VALUE}};    # reset each() iterator
    return each %{$self->{PARENT}->{VALUE}}
}

sub NEXTKEY
{
    my $self = shift;

    return each %{$self->{PARENT}->{VALUE}}
}

sub DESTROY
{
    my $self;

    if ($self->{TYPE} eq 'HASH') {
        $self->{PARENT}->reset();
    } else {
        $self->reset();
    }
}


1;
