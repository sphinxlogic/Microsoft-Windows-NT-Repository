$set noon
$deassign /table=lnm$system_directory vwsbanner
$create/name_table/parent=lnm$system_directory/prot=(s:rwed,g:rwed,o:rwed,w:rwed) VWSBANNER
