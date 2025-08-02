#!/usr/bin/python3

import cgi
import os
import html

# The cgi.FieldStorage() class reads the form data from stdin for POST requests.
form = cgi.FieldStorage()

# Check if the 'message' field was sent via POST.
# The 'form' object will be populated only if the form was submitted.
message = form.getvalue('message')

# --- HTTP Headers ---
# This is a mandatory part of the CGI response.
print("Content-Type: text/html")
print() # A blank line must separate headers from the body.

# --- HTML Body ---
print("<!DOCTYPE html>")
print("<html>")
print("<head>")
print("<title>Simple Python POST Test</title>")
print("""
    <style>
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
            background-color: #f0f2f5;
        }
        .container {
            text-align: center;
            background-color: white;
            padding: 40px;
            border-radius: 10px;
            box-shadow: 0 4px 12px rgba(0,0,0,0.1);
        }
        textarea {
            width: 300px;
            height: 100px;
            padding: 10px;
            font-size: 16px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }
        input[type="submit"] {
            display: block;
            width: 100%;
            padding: 10px 20px;
            margin-top: 10px;
            font-size: 16px;
            border: none;
            border-radius: 5px;
            background-color: #28a745;
            color: white;
            cursor: pointer;
        }
    </style>
""")
print("</head>")
print("<body>")
print("<div class='container'>")

if message:
    # If a message was posted, display it.
    # Use html.escape() to prevent XSS.
    print("<h1>Message Received!</h1>")
    print(f"<p>You sent the following message via POST:</p>")
    print(f"<pre style='background-color: #eee; padding: 15px; border-radius: 5px;'>{html.escape(message)}</pre>")
    # Get the script name from environment variables to create a link to go back.
    script_name = os.environ.get("SCRIPT_NAME", "")
    print(f"<p><a href='{script_name}'>Send another message.</a></p>")
else:
    # If no message was posted, show the form.
    print("<h1>Send a Message via POST</h1>")
    script_name = os.environ.get("SCRIPT_NAME", "")
    print(f"<form action='{script_name}' method='post'>")
    print("<textarea name='message' placeholder='Type your message here...'></textarea>")
    print("<input type='submit' value='Submit POST Request'>")
    print("</form>")

print("</div>")
print("</body>")
print("</html>")
