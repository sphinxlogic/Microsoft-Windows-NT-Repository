$! delete old ns-servers from database.* and add new ns_servers.dat
$! this assumes that the ID range for ns_servers is 999900000 - 999999999
$! see qir_offset in ns_servers.com
$
$ build :== $gopher_root:[server]qi_build
$
$ build ns_servers.dat /start=999900000/end=999999999/merge/delete -
  /output=database/data/config=database
$ exit
