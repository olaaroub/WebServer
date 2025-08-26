#!/usr/bin/python3
import sys
import os

# This script is designed to test the server's CGI timeout.
# When a GET request is made, it has no body to send to the script's stdin.
# The script will block on the sys.stdin.read() line, waiting for data that will never come.
# This should cause the webserv's CGI timeout to trigger, killing the script.
# If a POST request is sent, it will simply echo the body back.

# --- Get environment variables for debugging ---
request_method = os.environ.get("REQUEST_METHOD", "Not Set")
content_length = os.environ.get("CONTENT_LENGTH", "0")

# --- Read the request body from standard input ---
# This is the line that will block on a GET request.
post_data = sys.stdin.read()

# --- Send the HTTP response headers ---
# CORRECT WAY: Use sys.stdout.write for precise control over output.
# This avoids the extra newline added by the print() function.
sys.stdot.write("Content-Type: text/html\r\n")
sys.stdout.write("\r\n") # This blank line is crucial to separate headers from the body.
sys.stdout.flush() # Ensure headers are sent immediately.


# --- Send the HTML body ---
# We can use print() for the body, as extra newlines don't matter here.
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>CGI Hang Test</title></head>")
print("<body>")
print("<h1>CGI Script Execution</h1>")
print(f"<p><strong>Request Method:</strong> {request_method}</p>")
print(f"<p><strong>Declared Content-Length:</strong> {content_length}</p>")

if request_method == "POST":
    if post_data:
        print("<h2>Data Received from Parent (stdin):</h2>")
        print("<pre>")
        print(post_data)
        print("</pre>")
    else:
        print("<p>This was a POST request, but no data was received on stdin.</p>")
else:
    # This part will now be reached on a GET request.
    print("<p>This was a GET request. The script finished as expected because the server closed the input pipe.</p>")

print("</body>")
print("</html>")
