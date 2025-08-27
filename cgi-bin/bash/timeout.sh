#!/bin/bash
# File: cgi-bin/bash/timeout.sh

# This script intentionally sleeps for a very long time.
# It should be terminated by the server's CGI timeout logic.
# It does not need to produce any output, as it will be killed
# before it can finish.

sleep 1000
