#! perl
#
#   script to implement Netscape Roaming access
#   note that you'll need to change $root and $PWDfile.
#   The scripts PWDCHANGE.CGI and PWDCREATE.CGI can be used
#   to help manage the password file
#
#   Author:  C. Lane  lane@duphy4.physics.drexel.edu
#
use MIME::Base64;
use Digest::MD5 qw(md5_hex);

$CRINOID::Reuse = 1;
$Verbose = 0;

$PWDfile = 'sys$login:roaming_access.pwd';
$root   = '../netscape_roam';

$method = $ENV{'REQUEST_METHOD'};
$what   = $root.$ENV{'PATH_INFO'};

process_request();
print STDERR "ROAM.CGI: request completed\n" if $Verbose;
done: $x = 1;

sub process_request {
    my $authorized = 0;
    my $t = $ENV{'HTTP_AUTHORIZATION'};
    my ($up, $CTL, $TSPECIALS, $user, $pass, $u, $p, $d);
    local *FH;

    if ($t =~ /^Basic\s+(\w+)/i) {
        $up = MIME::Base64::decode_base64($1);

        $CTL = '\000-\017\177';
        $TSPECIALS = '\(\)\<\>\@\,\;\:\\\"\/\[\]\?\=\}\{\s';

        if ($up =~ /^([^$CTL$TSPECIALS]*)\:/) {
            $user = lc($1);
            $pass = md5_hex(lc($'));

            if (open(FH,"<$PWDfile")) {

                while (<FH>) {
                    chomp;
                    s/#.*//;
                    s/^\s+//;
                    s/\s+$//;
                    s/\s+/ /;
                    next if $_ eq '';
                    ($u,$p,$d) = split;
                    $u =~ tr/[a-zA-Z0-9\_\-\$\.]/\?/c;
                    next if (index($u,'?') != -1);
                    next if (lc($u) ne $user);
                    if ($p eq $pass) {
                        $authorized = 1;
                    }
                    last;
                }
                close(FH);
            }
        }
    }

    if (!$authorized) {
        print "Status: 401 need authorization\n";
        print "WWW-Authenticate: Basic realm=\"Roam\"\n\n";
        return;
    }

    $what   = $root.$d.$ENV{'PATH_INFO'};


    my (@s);
    @dow = ('Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat');
    @mon = ('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec');


    if ($method eq 'GET' || $method eq 'HEAD') {
        if (!-e $what) {
            print "Status: 404  File not found\n\n";
            return;
        }
        @s = stat($what);
        print "Status: 200 OK\n";
        print "Content-Type: text/plain\n";
        print "Content-Length: $s[7]\n";
        @g = gmtime($s[9]);
        $when = $dow[$g[6]].' '.$g[3].' '.$mon[$g[4]].' ';
        $when .= sprintf("%4.4d %2.2d:%2.2d:%2.2d",$g[5]+1900,$g[2],$g[1],$g[0]);

        print "Last-Modified: $when GMT\n\n";

        if ($method eq 'GET') {
            open(FILE,"<$what") || return;
            $sent = 0;
            while(1) {
                $got = read(FILE,$buf,1024,0);
                if ($got > 0) {
                    print $buf;
                    $sent += $got;
                } else {
                    last;
                }
            }
            print STDERR "ROAM.CGI: GET $what ($sent bytes)\n" if $Verbose;
            close FILE;
        } else {
            print STDERR "ROAM.CGI: HEAD $what\n" if $Verbose;
        }
    } elsif ($method eq 'DELETE') {
        if (-e $what) {
            while (-e $what) {unlink $what;}
            print STDERR "ROAM.CGI: DELETE $what (OK)\n" if $Verbose;
        } else {
            print "Status: 404 File not found\n";
            print "Content-type: text/plain\n\n";
            print "Attemp to delete non-existant file\n";
            print STDERR "ROAM.CGI: DELETE $what (ERR)\n" if $Verbose;
        }
    } elsif ($method eq 'PUT') {
        $old = -e $what;
        $stat = "200: OK";
        $stat = "201: Created"; # if !$old;
        unlink($what) if $old;
        if (open(FILE,">$what")) {
            $remain = $ENV{'CONTENT_LENGTH'};
            $j = 0;
            while ($remain > 0) {
                $n = $remain > 1024 ? 1024 : $remain;
                $got = read(STDIN,$buf,$n, 0);
                $remain -= $got;
                print FILE $buf;
            }
            close FILE;
            print "Status: $stat\n\n";
            print STDERR "ROAM.CGI: PUT $what ($j bytes)\n" if $Verbose;
        } else {
            print "Status: 500 Error opening file\n\n";
        }
    } elsif ($method eq 'MOVE') {
        if (!-e $what) {
            print "Status: 404 file not found\n";
            print "Content-type: text/plain\n\n";
            print "Error 404: file not found\n";
            return;
        }
        $new = $ENV{'HTTP_NEW_URI'};  # '/cgi/~lane/roam.pl/cookies'
        @x = split('/',$new);
        $new = pop(@x);
        if (index($new,'.') == -1) { $new .= '.'; }
        $new = $root.$d.'/'.$new;
        while (-e $new) {unlink($new);}
        if (rename($what,$new)) {
            print "Status: 200 OK\n\n";
            print STDERR "ROAM.CGI: MOVE $what -> $new (OK)\n" if $Verbose;
        } else {
            print "Status: 500 Rename error\n";
            print "Content-Type: text/plain\n\n";
            print "Error renaming from $what to $new\n";
            print STDERR "ROAM.CGI: MOVE $what -> $new (ERR)\n" if $Verbose;
        }
    } else {
        print "Status: 405 Method not implemented\n\n";
    }
}

