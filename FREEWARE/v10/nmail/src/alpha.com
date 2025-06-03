$! temp to get /ALPHA commands on my VAX
$ set command sys$library:cross_alpha_librarian
$ set command sys$library:cross_alpha_link
$ set command sys$library:cross_alpha_macro
$ set command sys$library:cross_alpha_message
