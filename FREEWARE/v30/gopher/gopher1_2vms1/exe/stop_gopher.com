$! This command procedure will attempt to gracefully stop the VMS Gopher server.  
$!
$! @stop_gopher p1 
$!
$! Parameters:
$!    p1 : optional text to be displayed as server shuts down.  If
$!		RESTART, server will reload and restart.
$!    example:
$!	to restart server, 
$!		@stop_gopher RESTART		
$!	to shut down server,
$!		@stop_gopher SHUTDOWN
$!
$ If ( p1 .nes. "" )
$  Then
$   Define/system Gopher_Restart_70 'p1
$  Else
$   Define/system Gopher_Restart_70  "Shutdown"
$ Endif
$!
$! Change dev:[dir] in the following line to the location of your
$! gopher client.
$ gopher := $dev:[dir]gopher	!Quick connect/disconnect to server to force
$ define/user sys$output nl:	!the server to acknowledge the logical.  
$!
$! Change the following line to point to the server you want to 
$! stop with this file.
$ gopher psulias.psu.edu 70	!null output, eof input keeps it quiet
$!
$ exit
