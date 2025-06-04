DCLHolidays, SYSTEM_MGMT, Calculation of holidays in OpenVMS DCL

The sample DCL procedure HOLIDAYS.COM uses a special form auf the
Gaussian Easter Algorithm (see reference in the procedure itself) to
calculate the date of Easter sunday and from there the dates of other
holidays depending on it. Included is the handling of holidays with
fixed dates, christmas for example.

This procedure has been written in Germany with respect to the local
holidays; it may and will require some modifications for other
countries. It uses _only_ DCL and _no_ other programming language so
it can be used on any OpenVMS system without a need for a separate
compiler and its license.

This procedure may be usefull if backup or similar operations have to
be skipped on such days. (No operator available to handle tape
cartridges, for example.)

Usage: Include the "check_date" subroutine in your main procedure,
call it with the date to be checked as a parameter (OpenVMS absolute
date format) and evaluate the value of the symbol(s) $STATUS and/or
$SEVERITY. The possible return codes are listed in the subroutine
itself.

Michael Unger

