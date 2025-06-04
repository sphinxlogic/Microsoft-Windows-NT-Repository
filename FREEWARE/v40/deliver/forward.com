$ GoTo SkipComments
$!
$!    FORWARD.COM - Command procedure to forward mail and notify
$!    sender of new address, etc.
$!
$!    SYS$LOGIN:NEWADDRESS.TXT should contain the text body of
$!    a message that will be mailed to the sender.
$!
$!    SYS$LOGIN:NOTIFIED.LIST will contain a list of senders
$!    that have already been notified.
$!
$!    The symbol NEW_ADDRESS defined below should be edited to reflect
$!    the new electronic mail address to forward mail to.
$!
$! Provided for use with DELIVER by:
$!
$!    Dan Ehrlich
$!    4-Sep-1986
$!    Schlumberger-Doll Research
$!
$SkipComments:
$!
$!    Set up new address
$!    Note: All double quotes internal to the address must be QUADRUPLED!
$!
$ NEW_ADDRESS = "IN%""""ned@ymir.bitnet"""""
$!
$!    If the list of people we have already notified does
$!    not exist then create it.
$!
$ If F$Search("SYS$LOGIN:NOTIFIED.LIST") .eqs. "" -
    Then -
        $ Create Sys$Login:Notified.List
$!
$!    See if we have already notified this sender
$!
$ Search/NoOutput    Sys$Login:Notified.List    "''QFROM'"
$ If $Status .eq. 1 -
    Then -
        $ GoTo ForwardMail
$!
$!    Add this sender to the list
$!
$ Open/Append    TR    Sys$Login:Notified.List
$ Write    TR    FROM
$ Close    TR
$!
$!    Send notification of new address
$!
$ Mail/NoSelf/Subject="New Address" Sys$Login:NewAddress.Txt -
  "''F$Element(0," ",QFROM)'"
$!
$!    Forward message to new address
$!
$ForwardMail:
$ Mail/NoSelf/Subject="''SUBJECT'" 'MESSAGE_FILE' "''NEW_ADDRESS'"
$ Purge/NoLog    Mail*.*
$ MESSAGE_DELETE == "YES"
