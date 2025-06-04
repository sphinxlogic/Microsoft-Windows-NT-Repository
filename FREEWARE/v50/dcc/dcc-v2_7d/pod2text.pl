# This script is provided with the dcc distribution because we needed
# an output sligthly different from the default pod2text implementation
use Pod::Text;

open(FI,"$ARGV[0]") || die "Syntax is : pod2text.pl InputFile\n";
open(FO,">temp.pod") || die "Could not open temp file\n";

while ($line = <FI>){
  print FO $line;
  
  if( $line =~ /^=head1/){
    # Add "---" separator . Trim the line for accurate length.
    $line =~ s/^\s*(.*?)\s*$/$1/;
    for($i = length($line)-7 ; $i > 0 ; $i--){
      print FO "-";
    }
    print FO "\n\n";
  }
}
close(FI);
close(FO);

# Pipe to pod2text() function now
pod2text("temp.pod");

while( -e "temp.pod"){
  unlink("temp.pod");
}
