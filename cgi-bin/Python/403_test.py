#!/usr/bin/python3

# The first line of output MUST be the Content-Type header.
print("Content-Type: text/html")
# This blank line separating the header from the body is mandatory.
print()

# Now, print the HTML body of the page.
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>Simple Python CGI Test</title></head>")
print("<body>")
print("<h1>Hello from Python CGI!</h1>")
print("<p>If you can see this message, your webserver's Python CGI execution is working correctly.</p>")
print("</body>")
print("</html>")

