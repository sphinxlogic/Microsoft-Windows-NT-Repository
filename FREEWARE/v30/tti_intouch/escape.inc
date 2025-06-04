12000   !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        ! P R I N T E R   E S C
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        !
        ! Brief description:
        !
        ! Expected:
        !
        ! Locals:
        !
        ! Results:
        !
        !%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        routine printer_esc

        dblt$ = chr$(27%)+"[1m"+chr$(27%)+"#3"  !double high double wide top
        dblb$ = chr$(27%)+"[1m"+chr$(27%)+"#4"  !double high double wide bot

        !hplaser print size font sequences

        hp_norm$ = chr$(27%)+"(10U"+chr$(27%)+"(s0p10.00h12.0v0s0b3T"
        hp_bold$ = chr$(27%)+"(10U"+chr$(27%)+"(s0p10.00h12.0v0s3b3T"
        hp_ital$ = chr$(27%)+"(10U"+chr$(27%)+"(s0p10.00h12.0v1s0b3T"
        hp_smal$ = chr$(27%)+"(10U"+chr$(27%)+"(s0p16.67h8.5v0s0b0T"

        !hplaser underline sequence

        hp_undr$   = chr$(27%) + "&dD"

        !digital print size font sequences

        dec_norm$ = chr$(27%) + "[0w"
        dec_bold$ = chr$(27%) + "[1m"
        dec_ital$ = chr$(27%) + "[3m"
        dec_smal$ = chr$(27%) + "[4w"
        dec_big$  = chr$(27%) + "[5w"

        !digital underline sequence

        dec_uon$    = chr$(27%) + "[4m"

        end routine



       ! Escape sequences for different printers

