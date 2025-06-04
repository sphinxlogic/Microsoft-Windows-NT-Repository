#! perl

#
# test of file uploading
#   CGI.pm versions before 2.74 may require a patch to make file
#   uploading work on VMS.
#

use CGI;
$CRINOID::Reuse = 1;

$q = new CGI;

print $q->header('text/html');
print $q->start_html('Test File uploading');
ask_upload($q);
proc_upload($q);
print $q->end_html;

sub ask_upload {
    my $q = shift;

    print "<H2>Request upload</h2>",$q->start_multipart_form();
    print "Client file to upload: ",
          $q->filefield(-name=>'uploaded_file',
                        -default=>'starting value',
                        -size=>20,
                        -maxlength=>80);
    print "<P>requested filename on server: ";
    print $q->textfield(-name=>'filename', -default=>'', -size=>20, -maxlength=>80);
    print "   ",$q->submit(-name=>'Upload'),$q->endform(),"<P>\n";

}

#
#   works now, with mod to OPCODE.XS to handle _  & recent changes
#   in CGI.pm
#
sub proc_upload {
    my $q = shift;

    if (!defined($q->param('uploaded_file'))) {
        print "<HR>No file upload request\n";
        return 1;
    }

    my $fn = $q->param('filename');
    my $f  = $q->param('uploaded_file');

    print "<HR><table>\n";
    print "<tr><td>Filename on client:<td>$f\n";
    print "<tr><td>Filename on server:<td>$fn</table>\n";

    print "<hr><h2>File contents:</h2><pre>\n";
    while (<$f>) {
        s/\&/&#38;/g;           ### escape any html stuff
        s/\</&#60;/g;           ###
        print  $_;
    }
    print "</pre><hr>\n";

    my $tmp = $q->tmpFileName($f);      ## MUST clean up afterwards!
    close $f;                           # close the i/o
    unlink $tmp if $tmp;                # get rid of tempfile
    return 0;
}

