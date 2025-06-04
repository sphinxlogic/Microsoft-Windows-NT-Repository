#! perl
#
#   this script generates a form; when the form is submitted back
#   to the script it also prints out the data entered in the form.
#   Lightly editted version of the "tryit.cgi" script that comes with
#   the CGI.pm module.
#
$CRINOID::Reuse = 1;            # this is a reusable script

use CGI qw(:standard);

print header;
print start_html('A Simple Example'),
    h1('A Simple Form'),
    start_form,
    "What's your name? ",textfield('name'),
    p,
    "What's the combination?",
    p,
    checkbox_group(-name=>'words',
                   -values=>['eenie','meenie','minie','moe'],
                   -defaults=>['eenie','minie']),
    p,
    "What's your favorite color? ",
    popup_menu(-name=>'color',
               -values=>['red','green','blue','chartreuse']),
    p,
    submit,
    end_form,
    hr;

if (param()) {
    print
        "Your name is: ",em(param('name')),
        p,
        "The keywords are: ",em(join(", ",param('words'))),
        p,
        "Your favorite color is: ",em(param('color')),
        hr;
}
print a({href=>'http://stein.cshl.org/WWW/software/CGI/'},'Go to the documentation');
print end_html;
