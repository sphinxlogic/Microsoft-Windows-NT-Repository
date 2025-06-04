function var = stostr(var, loc, string)
var(loc,1:max(size(toascii(string)))) = toascii(string);
