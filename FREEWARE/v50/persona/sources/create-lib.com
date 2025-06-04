$! Create an object library for subsequent relinking of PERSONA. It
$! is assumed the existing object library has been deleted. (23-Jan-2001)
$
$   If f$getsyi("HW_MODEL") .ge. 1024
$    Then
$      library/create/log/insert -
       [-.axp]persona.olb -
       persona,utility,persona_cld,cmdlin
$    Else
$      library/create/log/insert -
       [-.vax]persona.olb -
       persona,utility,persona_cld,cmdlin
$   Endif
