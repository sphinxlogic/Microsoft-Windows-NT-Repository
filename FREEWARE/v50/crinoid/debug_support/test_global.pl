#!  perl
use Safe;

$s = new Safe 'FOO';
$s->permit(':all');

print "--------------------------------------------------------------\n";

for ($c = 1; $c <127; $c++) {
    next if ($c >= ord('A') && $c <= ord('Z'));
    next if ($c >= ord('a') && $c <= ord('z'));
    next if ($c == ord(' '));
    if ($c < ord(' ')) {
        $sym = '^'.chr(ord('@')+$c);
    } else {
        $sym = chr($c);
    }

    $script = '$val = $'.$sym.';';
    $s->reval($script);
    eval('$diff = ($FOO::val != $'.$sym.');');
    eval('$diff2 = ($FOO::val ne $'.$sym.');');

    if ($diff || $diff2) {
        print '$'.$sym." default value does not propagate into Safe::\n";
    }
}

print "--------------------------------------------------------------\n";



for ($c = 1; $c <127; $c++) {
    next if ($c >= ord('A') && $c <= ord('Z'));
    next if ($c >= ord('a') && $c <= ord('z'));
    next if ($c == ord(' '));
    if ($c < ord(' ')) {
        $sym = '^'.chr(ord('@')+$c);
    } else {
        $sym = chr($c);
    }
    if ($sym eq '^D') {     # know it's global, too much output!!
        print '$'.$sym." localizable global\n" ;
        next;
    }

    $script = "\$".$sym." = 'BAD';";
    eval('$saveval = $'.$sym.';');
    $s->reval($script);
    eval('$diff = ($'.$sym.' eq q(BAD)); $'.$sym.' = $saveval;');

    if ($diff) {
        $script = 'local $'.$sym.'; $'.$sym.' = q(BAD);';
        eval('$saveval = $'.$sym.';');
        $s->reval($script);
        eval('$diff = ($'.$sym.' eq q(BAD)); $'.$sym.' = $saveval;');
        if (!$diff) {
            $result =  '$'.$sym." localizable global";
            $s->reval('local $'.$sym.'; $val = $'.$sym.';');
            eval('$val = $'.$sym.';');
            if ($FOO::val != $val && $FOO::val ne $val) {
                    $result .= ' but non-default value';
            }
            print $result,"\n";
            next;
        }
    }

    $script = "\$".$sym." = 666;";
    eval('$saveval = $'.$sym.';');
    $s->reval($script);
    eval('$diff = ($'.$sym.' == 666); $'.$sym.' = $saveval;');

    if ($diff) {
        $script = 'local $'.$sym.'; $'.$sym.' = 666;';
        eval('$saveval = $'.$sym.';');
        $s->reval($script);
        eval('$diff = ($'.$sym.' == 666); $'.$sym.' = $saveval;');
        if (!$diff) {
            print '$'.$sym." localizable global\n";
            next;
        }
    }
}

print "-------------------------------------------------------\n";

for ($c = 1; $c <127; $c++) {
    next if ($c >= ord('A') && $c <= ord('Z'));
    next if ($c >= ord('a') && $c <= ord('z'));
    next if ($c == ord(' '));
    if ($c < ord(' ')) {
        $sym = '^'.chr(ord('@')+$c);
    } else {
        $sym = chr($c);
    }
    if ($sym eq '^D') {     # know it's global, too much output!!
        next;
    }

    $script = '$diff = ($'.$sym.' eq q(BAD));';
    $@ = '';
    eval('$saveval = $'.$sym.'; $'.$sym.' = q(BAD);');
    if ($@) { print '$'.$sym." not settable\n"; next; }
    $s->reval($script);
    $diff = $FOO::diff;
    eval('$'.$sym.' = $saveval;');

    if (!$diff) {
        $script = '$diff = ($'.$sym.' eq 666);';
        eval('$saveval = $'.$sym.'; $'.$sym.' = 666;');
        $s->reval($script);
        $diff = $FOO::diff;
        eval('$'.$sym.' = $saveval;');
    }
    if (!$diff) {
        print '$'.$sym." value does not propagate into Safe::\n";
    }
}


