#!/usr/bin/env python3

import os
import sys

print("Content-Type: text/html")
print()
print("<html>")
print("<head><title>CGI Test</title></head>")
print("<body>")
print("<h1>CGI Test Page</h1>")
print("<p>This is a test CGI script.</p>")
print("<p>Environment variables:</p>")
print("<ul>")
for key, value in os.environ.items():
    print(f"<li>{key}: {value}</li>")
print("</ul>")
print("</body>")
print("</html>") 