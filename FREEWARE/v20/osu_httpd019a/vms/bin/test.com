$!
$! copy gif file.
$ write net_link "<DNETCGI>"
$ crlf = f$fao("!/")
$ write net_link "Content-type: image/gif", crlf, crlf
$ mcr www_system:copy_bin www_root:[documents]nagel23a.gif
$ write net_link "</DNETCGI>"
