#!/usr/bin/python3
# File: cgi-bin/python/env_test.py

import os

# --- 1. Send the HTTP headers ---
print("Content-Type: text/html\r\n")

# --- 2. Send the HTML body ---
print("<!DOCTYPE html>")
print("<html lang='en'>")
print("<head><title>CGI Environment Variables</title></head>")
print("<body>")
print("<h1>CGI Environment Variables</h1>")
print("<p>This script displays all the environment variables passed to it by the web server.</p>")
print("<table border='1' cellpadding='5'>")
print("<tr><th>Variable Name</th><th>Value</th></tr>")

# --- 3. Loop through all environment variables and print them ---
for key, value in os.environ.items():
    print(f"<tr><td>{key}</td><td>{value}</td></tr>")

print("</table>")
print("</body>")
print("</html>")
