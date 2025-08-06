#!/usr/bin/python3

# ==============================================================================
#  TEST 1: Script Crash (Syntax Error)
#  This script will cause the Python interpreter to fail immediately.
#  Your server should detect this because the script will exit with a non-zero
#  status code, which your CGIHandler's waitpid() check will catch.
# ==============================================================================

# This is an intentional syntax error to make the script crash.
# print("Content-Type: text/html")
# print()
# print("This line will never be reached.")
# this_is_a_syntax_error

# ==============================================================================
#  TEST 2: Invalid Response (Missing Content-Type)
#  To use this test, comment out TEST 1 above and uncomment this section.
#  This script runs successfully but produces an invalid CGI response because
#  it's missing the mandatory "Content-Type" header. Your client::onEvent
#  code should detect this and generate a 502 error.
# =================================e=============================================

# print("Content-Type: text/html") # <--- The crucial header is commented out.
# print()
print("This should not be displayed")
