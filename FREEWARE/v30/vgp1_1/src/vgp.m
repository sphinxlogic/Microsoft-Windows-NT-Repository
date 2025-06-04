function vgp(...)
% Usage: vgp(...)

i = 0;
vgpcmd = ['vgp'];
nargin = nargin+1;
filename = 0;

unwind_protect

while (--nargin)
  i++;
  vec = va_arg ();
  s = size(vec);

  if(~any(s == 3))
    error('All vgp input must be three space vectors.');
    break;
  end

  if(s(1) == 3)
    vec = vec';
  end

  filename = stostr(filename, i, octave_tmp_file_name);
  cmd = ['save -ascii ' setstr(filename(i,:)) ' vec'];
  eval(cmd)

  i++;
  filename = stostr(filename, i, octave_tmp_file_name);
  cmd = ['tail -' int2str(max(s)) ' ' ...
          setstr(filename(i-1,:)) ' > ' setstr(filename(i,:))];
  system(cmd)
  vgpcmd = [vgpcmd ' ' setstr(filename(i,:))];
end


vgpcmd = [vgpcmd ' > /dev/null &'];


system(vgpcmd);
system('sleep 3')  %%% Wait for file read before deleting files


unwind_protect_cleanup

for j = 1:rows(filename)
  cmd = ['rm -f ' setstr(filename(j,:))];
  system(cmd)
end

end_unwind_protect

