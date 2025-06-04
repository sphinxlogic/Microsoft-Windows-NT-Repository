GMAKE, Build_Tools, the FSF GNU Make Tool for OpenVMS

  GNU Make provides the ability to selectively build part or
  all of an OpenVMS application, based on analysis of the
  source files and comparision with a user-created source
  code module dependency (makefile) module.

  Thanks to Hartmut Becker of Compaq, here is the OpenVMS/Alpha
  V7.2 image resulting from a port of the GNU Make. This image is
  based on the FSF 3.79.1 baselevel. The associated source files
  are available from ftp://ftp.gnu.org/gnu/make/. They can be used
  for other VMS versions, VAX and Alpha, as well. There are VMS
  specific instructions how to build the images. Unfortunately
  there is a regression in the VMS code. In the module VARIABLE.C
  the two lines with 'listp' need to be removed. A newer version
  of GNU make is expected to have this bug fixed.

      3    8081           scnt = 0;
      3    8082
      3    8083           if (listp)
        ..............1
%CC-E-UNDECLARED, (1) In this statement, "listp" is not declared.

      3    8084             *listp = current_variable_set_list;
      3    8085
      3    8086           while ((sptr = strchr (sptr, '$')))
      4    8087             {

