#!/bin/csh
# Test httpd in non-privileged mode.  Starts up server on port 8000,
# managment port (for privrequest) set to 9310:
#
#    privrequest 9310 help 8000
#
setenv HTTP_DEFAULT_HOST $HOST
setenv HTTP_MANAGE_PORT 9310
setenv HTTP_CLIENT_LIMIT 16
setenv HTTP_CACHE_SIZE 0
setenv HTTP_MIN_AVAILABLE 1
#
setenv http_log_level 1
httpd http_server.log osf_sample_home.conf 8000
