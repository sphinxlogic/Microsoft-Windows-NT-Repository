#! perl
#
#   a little cgi that prints out the %ENV and POST data submitted
#   from a form, useful for debugging.
#
$CRINOID::Reuse = 1;
print "Content-type: text/html\n\n";

print "<html><head><title>Form Debugging Info</title></head>\n";

print "<body>\n";
print "<h1>Form Debugging Info</h1>\n";

print "<h2>CGI ENV info</h2>\n";
print "<Table>\n";
foreach $k (sort(keys(%ENV))) {
    print "<TR><TD>$k<TD>\"$ENV{$k}\"</TR>\n";
}
print "</table>\n";

#if ($ENV{'REQUEST_METHOD'} eq 'POST') {
    print "<h2>Information passed via POST</h2>\n";
    while (<STDIN>) {
        chomp;
        print "$_<BR>\n";
    }
#}
print "<HR>\n";

print "This script was run at ",scalar(localtime),"\n";
print "</body></html>\n";
