[inherit ('sys$share:Starlet')] program setload;
 var
	chan:[word] 0..65535;
	status: integer;
 begin
	status:= $ASSIGN (chan:= chan, devnam:= 'KILL0');
	if Odd(status) then
		status:= $QIOW (chan:= chan, func:= IO$_INITIALIZE);
	$EXIT (status);
 end
.
