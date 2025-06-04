#! perl
#
#   add a user, if necessary create password file and user-directory
#
#   Author:  C. Lane  lane@duphy4.physics.drexel.edu

use CGI;
use VMS::Stdio;
use Digest::MD5 qw(md5_hex);

$CRINOID::Reuse = 1;
$PWDfile = 'sys$login:roaming_access.pwd';
$root   = '../netscape_roam';
$MINPWDLENGTH = 8;
$MAXPWDLENGTH = 32;

$q = new CGI;
####################
#
#   must be a secure connection...redirect if not
#
$url = $q->url;
$redirect = $goturl = 0;
if ($url =~ /^([a-z0-9]+)\:\/\/([a-z0-9\_\-\.]+)(\:\d+)?\//i) {
    $goturl = 1;
    $redirect = lc($1) ne 'https';
    $url = 'https://'.$2.'/'.$';
}

if ($goturl) {
    if ($redirect){
        print $q->redirect($url);
    } else {
        print $q->header("text/html");
        if ($q->param('newpwd1')) {
            try_change($q);
        } else {
            show_form($q);
        }
        print $q->end_html;
    }
} else {
    print "Content-type: text/plain\n\n";
    print "Error: something about the url $url is peculiar.\n";
    print "Please inform the owner of this page\n";
}

sub try_change {
    my $q = shift;
    print $q->start_html("pwdcreate/result");

    my $user = lc($q->param('user'));
    my $n1 = $q->param('newpwd1');
    my $n2 = $q->param('newpwd2');
    my $found = 0;

    $user =~ tr/[a-zA-Z0-9\_\-\$\.]/\?/c;
    if (index($user,'?') != -1) {
        print "Invalid username<BR>";
        print '<a href="',$url,'">Try again</a>';
        return;
    }
    $home = $user;
    $home =~ tr/\./\$2E/;
    $home = '/'.$home;

    if ($n1 ne $n2) {
        print "The new password you entered wasn't the same both times<br>";
        print '<a href="',$url,'?user=',$user,'">Try again</a>';
        return;
    }

    if (length($n1) < $MINPWDLENGTH) {
        print "What you entered for a new password is too short. ";
        print "Your new password should be at least $MINPWDLENGTH ";
        print "characters long.<br>";
        print '<a href="',$url,'?user=',$user,'">Try again</a>';
        return;
    }

    if (length($n1) > $MAXPWDLENGTH) {
        print "What you entered for a new password is too long. ";
        print "Your new password should shorter than ",$MAXPWDLENGTH+1;
        print "characters long.<br>";
        print '<a href="',$url,'?user=',$user,'">Try again</a>';
        return;
    }


    $n2 = md5_hex(lc($n2));


    if (!open(FH,"<$PWDfile")) {
        if (! -e $PWDfile) {
            print "Creating new file<BR>";
            goto newfile;
        }
        print "There was an error opening the password file...<br>";
        print "Please report this to the owner of this page";
        return;

    }

    my $j = 0;
    my @save;
    my ($u, $p, $d);


    while (<FH>) {
        $save[$j++] = $_;
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
        $found++;
        $save[$j-1] = $user.' '.$n2.' '.$home."\n";
    }
    close FH;

newfile:
    if ($found > 1) {
        print "Username found $found times in the password file. <br>";
        print "Please bring this to the attention of the owner of this page!<br>";
        print "This problem must be fixed before you can change your password";
        return;
    }
    if ($found) {
        print "User '$user' alread in the password file; updating entry<BR>";
    }

    if (!$found) {
        $save[$j] = $user.' '.$n2.' '.$home."\n";
        if (!mkdir($root.$home,0700)) {
            print "Unable to create home directory $root$home<BR>";
            return;
        }
    }

    if (!open(FH,">$PWDfile-2")) {
        print "Unable to open password file for writing!";
        print "Please bring this to the attention of the owner of this page!";
        return;
    }
    foreach (@save) {
        print FH $_;
    }
    print FH "# $user PWDCREATE at ",scalar(localtime),"\n";
    close FH;
    unlink($PWDfile);
    rename("$PWDfile-2",$PWDfile);
    chmod (0700, $PWDfile);

    print "User add successful!<br>";
}



sub show_form {
    my $q = shift;

    print $q->start_html("pwdcreate/query");

    print $q->startform();
    print '<table>';
    print "<TR><TD>Username:<TD>",$q->textfield(-name=>'user',-value=>'',-force=>1,-size=>32);
    print '<tr><td>New password:<td>',$q->password_field(-name=>'newpwd1',-value=>'', -force=>1, -size=>32);
    print '<tr><td>New password (again):<td>',$q->password_field(-name=>'newpwd2',-value=>'', -force=>1, -size=>32);
    print '</table>';
    print $q->submit(-name=>'Change'),$q->reset();

}





