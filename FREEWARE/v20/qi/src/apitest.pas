program apitest (input, output);

label
  10;

const
  godot = false;

type
  $bool  = [bit(1), unsafe] boolean;
  $ubyte = [byte] 0..255;
  $uword = [word] 0..65535;

  stringptr = ^string;
  string    = packed array [1..65535] of char;

  descriptor = record
    dsc$w_length  : $uword;
    dsc$b_dtype   : $ubyte;
    dsc$b_class   : $ubyte;
    dsc$a_pointer : stringptr;
  end; (* descriptor *)
  descriptor_array     = packed array [1..10000000] of descriptor;
  descriptor_array_ptr = ^descriptor_array;

  $aflags = record
    dummy0            : [pos(0)] $bool;
    dummy1            : [pos(1)] $bool;
    dummy2            : [pos(2)] $bool;
    dsc$v_fl_binscale : [pos(3)] $bool;
    dsc$v_fl_redim    : [pos(4)] $bool;
    dsc$v_fl_column   : [pos(5)] $bool;
    dsc$v_fl_coeff    : [pos(6)] $bool;
    dsc$v_fl_bounds   : [pos(7)] $bool;
  end; (* $aflags *)

  dsc$descriptor_a = record
    dsc$w_length  : $uword;
    dsc$b_dtype   : $ubyte;
    dsc$b_class   : $ubyte;
    dsc$a_pointer : stringptr;
    dsc$b_scale   : $ubyte;
    dsc$b_digits  : $ubyte;
    dsc$b_aflags  : $aflags;
    dsc$b_dimct   : $ubyte;
    dsc$l_arsize  : unsigned;
  end; (* dsc$descriptor_a *)

  array_descriptor = record
    dsc$a_handle  : dsc$descriptor_a;
    dsc$a_a0      : descriptor_array_ptr;
    dsc$l_m       : integer;
    dsc$l_l       : integer;
    dsc$l_u       : integer;
  end; (* array_descriptor *)
  array_descriptor_ptr = ^array_descriptor;

var
  cmd     : varying [256] of char;
  ctx     : integer_address := 0;
  i, stat : integer;
  result  : array_descriptor_ptr := nil;

function qi_command (
  var context : integer_address;
  command     : [class_s] packed array [l2..u2 : integer] of char;
  var result  : array_descriptor_ptr) : integer; extern;

function LIB$STOP (%immed stat : integer) : integer; extern;

begin (* apitest *)
  repeat
    write ('Enter qi command: ');
    readln (cmd);
    stat := qi_command (ctx, cmd, result);
    if not odd (stat) then LIB$STOP (stat);            (* this is a bad thing *)
    if cmd.length <= 0 then goto 10;      (* we shut down on the null command *)

    (* print the results *)
    with result^ do
      for i := 1 to dsc$l_m do
        writeln (substr (dsc$a_a0^[i].dsc$a_pointer^, 1,
                         dsc$a_a0^[i].dsc$w_length));
  until godot;
10:
end. (* apitest *)
