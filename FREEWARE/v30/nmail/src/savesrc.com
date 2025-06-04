$ set noon
$ @ident ! determine Nmail version number (sets nm$ident and nm$vvu)
$ BACKUP/LOG/VERIFY/COMMENT="''nm$ident' sources" -
  *.B32,*.R32,*.MAR,*.MSG,*.CLD,*.COM,*.DAT,*.RND,*.RNH,*.RNO/EXCLUDE=IDENT.* -
  NMAILSRC'nm$vvu'.SAV/SAVE
$ delete/symbol/global nm$ident
$ delete/symbol/global nm$vvu
