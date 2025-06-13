#!/usr/bin/python3

import os
import sys

print("Content-Type: text/html\r")
print("\r")
print("<!DOCTYPE html>")
print("<html>")
print("<head><title>Python CGI Test</title></head>")
print("<body>")
print("<h1>Python CGI Script Executed Successfully!</h1>")
print("<p>This confirms that .py extension checking is working.</p>")
print("<p>Request method:", os.environ.get('REQUEST_METHOD', 'Unknown'), "</p>")
print("<p>Script path:", os.environ.get('PATH_INFO', 'Unknown'), "</p>")
print("</body>")
print("</html>") 