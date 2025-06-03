#!/bin/csh
# Test httpd in privileged mode, must be invoked from root.  Starts up server 
# on port 80, management port (for privrequest) set to 931.  After startup,
# program changes uid/gid to that given by environment variables
# HTTP_USER_ID and HTTP_GROUP_ID.  Id's can be specified by name or by
# number (precede with #).  If HTTP_GROUP_ID absent, program will take
# gid from user specified in HTTP_USER_ID if not numeric.
#
setenv HTTP_DEFAULT_HOST $HOST
setenv HTTP_MANAGE_PORT 931
setenv HTTP_CLIENT_LIMIT 16
setenv HTTP_CACHE_SIZE 0
setenv HTTP_MIN_AVAILABLE 1
#
setenv http_log_level 1
httpd http_server.log osf_sample_home.conf 80
