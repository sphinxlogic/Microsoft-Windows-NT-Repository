: see end of file for revision information
case $CONFIG in
    '') . ./config.sh ;;
esac
echo "Extracting newsnews (with variable substitutions)"
cat >newsnews <<!GROK!THIS!
			*** NEWS NEWS ***

Welcome to trn.  There are more options to trn than you want to think about, so
we won't list them here.  If you want to find out about them, read the manual
page(s).  There are some important things to remember, though:

  * Trn is an extension of rn.  Where possible, the command syntax is the same.
  * To access all the new features, specify the options -x and -X.  These
    options MAY be on by default, but it won't hurt to be redundant.
  * Trn runs in cbreak mode.  This means you don't have to type carriage return
    on single character commands.  (There are some multi-character commands.)
  * At ANY prompt, you may type 'h' for help.  There are many different help
    menus, depending on the context.  Also, typing <esc>h in the middle of a
    multi-character command will list escape substitutions.
  * Typing a space to any prompt means to do the normal thing.  You could
    spend all day reading news and never hit anything but the space bar.

This particular message comes from $rnlib/newsnews, and will only
appear once.  If your news administrator puts a new message in here, it
will be displayed for you the first time you run trn after the change.

Wayne Davison	davison@borland.com
!GROK!THIS!
$eunicefix newsnews
