The following is a shell script from Ronald Jeppesen to show
how to automate fixing makefiles for your own environment.

Thanks Ronald!

----------------------------------------------------------

From: RONJ.AN@com.saic.site007 (Jeppesen, Ronald)
To: jacs@edinburgh.aiai (Julian Smart)
Subject: Re: wxWindows 1.50 beta
Date: Mon, 30 Aug 93 09:11
Sender: RONJ.AN@com.saic.site007
Status: REO

I have found the following script useful -- it automatically converts
your makefile.unx into Makefiles that are appropriate for my setup.
It is a little bit out of date but you get the idea.  Something
like this might be useful in the distribution (or changing the 
distribution makefiles to be less specific to your installation)

The script is a little dated but basically it does the following:
(1) redefines WXDIR (this is no longer necessary)
(2) turns of debugging
(3) sets compiler to gcc rather than gcc-2.1
(4) changes Xview references in -I and -L to /usr/openwin/{include,lib}
(5) removes the second set of -I and -L (for motif?)
(6) removes reference to makefile.unx since new makefiles are "Makefile"
A better script would have defines at the top for VXIEW_HOME and
MOTIF_HOME, etc.  Anyhow, it was just a thought since fixMakefiles
is the first thing I run on a distribution.

#!/home/gnu/bin/bash -f
for f in `find . -name makefile.unx -print` ; do
     fnew=`echo $f | sed -e "s/makefile.unx/Makefile/"`
     sed -e 's;/project/2/hardy/wx/develop;/home/ronj/wxwin;' \
          -e 's/-ggdb/#-ggdb/' -e 's/gcc-2.1/gcc/' \
          -e 's;/aiai/packages/X.V11R5;/usr/openwin;' \
          -e 's;-I/aiai/packages/prokappa2.0/X/usr/include;;' \
          -e 's;-L/aiai/packages/prokappa2.0/X/usr/lib;;' \
          -e 's/-f *makefile.unx//' \
          <$f >$fnew
#
#         -e 's;$(WXDIR)/src/x/objects_ol/sb_scrol.o;;' \
done



