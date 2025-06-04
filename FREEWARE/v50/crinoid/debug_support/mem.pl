#! perl
#
#       script to process CRINOID.LOG files, matching up malloc's with
#       free's and realloc's
#
#       output is log file with text appended to appropriate lines.
#
#   usage:      perl mem.pl <crinoid.log >processed.log

#(0001AD32 20-SEP-2000 09:23:23.03): [CRINOID 000040:3]malloc(36) = 0048C0E8
#(0001AD32 20-SEP-2000 09:23:23.07): [CRINOID 000040:3]free(0048C0E8)
#(0001B125 20-SEP-2000 11:49:02.74): [CRINOID 000040:3][movedown_level] realloc(00630A08,17) = 00672008

$line = -1;
while (<STDIN>) {
    chomp;
    $line++;
    $file[$line] = $line.': '.$_;
    $status[$line] = '';
    if (/malloc\((\d+)\)\s+=\s+([\da-f]+)/i) {
        $alloc{$2} = $line;
    } elsif (/free\(([\da-f]+)\)/i) {
        if (exists($alloc{$1})) {
            $file[$alloc{$1}] .= " freed line $line";
            delete($alloc{$1});
        } else {
            $file[$line] .= " not allocated!";
        }
    } elsif (/realloc\(([\da-f]+),(\d+)\)\s+=\s+([\da-f]+)/i) {
        if (exists($alloc{$1})) {
            $file[$alloc{$1}] .= " freed by realloc line $line";
            delete($alloc{$1});
        }
        $alloc{$3} = $line;
    }
}

foreach (@file) {
    print $_."\n";
}
