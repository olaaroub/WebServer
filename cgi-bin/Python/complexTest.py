#!/usr/bin/python3

import os
import sys
import cgi
import html
from datetime import datetime

# --- Test 1: Relative Path File Access ---
# This is a critical test from the subject. We try to open a file
# that should be in the same directory as this script.
try:
    with open('data.txt', 'r') as f:
        file_content = f.read().strip()
    file_read_status = "✅ Success"
except FileNotFoundError:
    # If data.txt is not found, the script itself will generate a 404 response.
    # This is a great test to see if your server correctly parses the "Status:"
    # header from a CGI script's output.
    print("Status: 404 Not Found")
    print("Content-Type: text/html;charset=utf-8")
    print() # Mandatory blank line
    print("<html><head><title>404 Not Found</title>")
    # Copying styles from the main page for consistency
    print("""
    <style>
        body { font-family: sans-serif; background-color: #f4f4f9; color: #333; margin: 0; padding: 20px; }
        .container { max-width: 900px; margin: auto; background-color: #fff; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); padding: 20px; }
        h1, h2 { color: #dc3545; }
        table { width: 100%; border-collapse: collapse; margin-top: 15px; }
        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; word-break: break-all; }
        th { background-color: #f2f2f2; }
    </style>
    """)
    print("</head><body><div class='container'>")
    print("<h1>404 - Not Found by CGI Script</h1>")
    print("<p>The CGI script executed successfully, but it could not find its required resource file (data.txt).</p>")

    # --- ADDED: Display Environment and HTTP Headers on the 404 page ---
    print('<h2>Environment Variables Passed to Script</h2>')
    print("<table><tr><th>Variable Name</th><th>Value</th></tr>")
    for key, value in sorted(os.environ.items()):
        print(f"<tr><td>{html.escape(key)}</td><td>{html.escape(value)}</td></tr>")
    print("</table>")

    print('<h2>HTTP Request Headers Passed to Script</h2>')
    print("<table><tr><th>Header Name (from ENV)</th><th>Value</th></tr>")
    http_headers_found = False
    for key, value in sorted(os.environ.items()):
        if key.startswith("HTTP_"):
            http_headers_found = True
            header_name = key[5:].replace('_', '-').title()
            print(f"<tr><td>{html.escape(header_name)}</td><td>{html.escape(value)}</td></tr>")
    if not http_headers_found:
        print("<tr><td colspan='2'>No HTTP headers found in environment variables.</td></tr>")
    print("</table>")

    print("</div></body></html>")
    sys.exit(0) # Stop the script here
except Exception as e:
    file_content = f"Error: {e}"
    file_read_status = "❌ Failure"


# --- Test 2: Custom Status/Header (Redirect) ---
# Check if the query string asks for a redirect test.
form = cgi.FieldStorage()
if "redirect_test" in form:
    # If so, send a 302 redirect header and stop.
    # Your server must correctly parse this and send the redirect to the browser.
    print("Status: 302 Found")
    print("Location: https://www.google.com/search?q=webserv+project")
    print("Content-Type: text/html;charset=utf-8")
    print() # Mandatory blank line
    sys.exit(0) # Stop the script here


# --- Main Response Generation ---

# Mandatory CGI header
print("Content-Type: text/html;charset=utf-8")
print() # Mandatory blank line

# --- HTML Head and Styling ---
print("""
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Comprehensive CGI Test</title>
    <style>
        body { font-family: sans-serif; background-color: #f4f4f9; color: #333; margin: 0; padding: 20px; }
        .container { max-width: 900px; margin: auto; background-color: #fff; border-radius: 8px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .header { background-color: #4a4a8a; color: white; padding: 20px; border-radius: 8px 8px 0 0; }
        .header h1 { margin: 0; }
        .content { padding: 20px; }
        h2 { border-bottom: 2px solid #4a4a8a; padding-bottom: 10px; color: #4a4a8a; }
        table { width: 100%; border-collapse: collapse; margin-top: 15px; }
        th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; word-break: break-all; }
        th { background-color: #f2f2f2; }
        .card { background-color: #f9f9f9; border: 1px solid #ddd; border-radius: 8px; padding: 20px; margin-top: 20px; }
        .status { font-weight: bold; }
        .success { color: #28a745; }
        .failure { color: #dc3545; }
        form { margin-top: 15px; }
        textarea { width: 100%; padding: 10px; border-radius: 4px; border: 1px solid #ccc; box-sizing: border-box; }
        input[type="submit"] { background-color: #4a4a8a; color: white; padding: 10px 15px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; margin-top: 10px;}
        a { color: #4a4a8a; }
    </style>
</head>
<body>
    <div class="container">
        <div class="header"><h1>🚀 Webserv Comprehensive CGI Test 🚀</h1></div>
        <div class="content">
""")

# --- Test 1 Results: Display File Access ---
print(f"""
<div class="card">
    <h2>Test 1: Relative Path File Access</h2>
    <p>Attempting to read <code>data.txt</code> from the script's directory.</p>
    <p><strong>Status:</strong> <span class="status {'success' if file_read_status == '✅ Success' else 'failure'}">{file_read_status}</span></p>
    <p><strong>Content:</strong></p>
    <pre><code>{html.escape(file_content)}</code></pre>
</div>
""")


# --- Test 3: Display GET and POST Data ---
print('<div class="card"><h2>Test 3: GET & POST Data</h2>')
if not form.keys():
    print("<p>No GET or POST data was submitted.</p>")
else:
    print("<table><tr><th>Key</th><th>Value</th></tr>")
    for key in form.keys():
        value = html.escape(form.getvalue(key))
        print(f"<tr><td>{html.escape(key)}</td><td>{value}</td></tr>")
    print("</table>")
print('</div>')


# --- Test 4: Display Environment Variables ---
print('<div class="card"><h2>Test 4: Environment Variables</h2>')
print("<table><tr><th>Variable Name</th><th>Value</th></tr>")
for key, value in sorted(os.environ.items()):
    print(f"<tr><td>{html.escape(key)}</td><td>{html.escape(value)}</td></tr>")
print("</table></div>")

# --- Test 5: Display HTTP Headers ---
print('<div class="card"><h2>Test 5: HTTP Request Headers</h2>')
print("<table><tr><th>Header Name (from ENV)</th><th>Value</th></tr>")
http_headers_found = False
for key, value in sorted(os.environ.items()):
    if key.startswith("HTTP_"):
        http_headers_found = True
        # Format the header name to be more readable (e.g., HTTP_USER_AGENT -> User-Agent)
        header_name = key[5:].replace('_', '-').title()
        print(f"<tr><td>{html.escape(header_name)}</td><td>{html.escape(value)}</td></tr>")
if not http_headers_found:
    print("<tr><td colspan='2'>No HTTP headers found in environment variables.</td></tr>")
print("</table></div>")


# --- Forms and Links for Interactive Testing ---
script_name = os.environ.get("SCRIPT_NAME", "")
print(f"""
<div class="card">
    <h2>Interactive Tests</h2>
    <p>Use the form and link below to test specific functionalities.</p>

    <h3>Test POST Method</h3>
    <form action="{script_name}" method="post">
        <label for="message">POST Message:</label><br>
        <textarea id="message" name="message" rows="3">This is a test message sent via POST.</textarea><br>
        <input type="submit" value="Submit POST Request">
    </form>

    <h3>Test Redirect (302 Status)</h3>
    <p><a href="{script_name}?redirect_test=true">Click here to test a redirect to Google.</a></p>
</div>
""")

# --- Footer ---
print("""
        </div>
    </div>
</body>
</html>
""")
