#!//usr/bin/python3

import os
import sys
import platform
import datetime
import cgitb
import io

# Enable error reporting
cgitb.enable()

# Capture the HTML content to calculate Content-Length
html_content = io.StringIO()

html_content.write("<!DOCTYPE html>")
html_content.write("<html>")
html_content.write("<head>")
html_content.write("<title>System Information</title>")
html_content.write("<style>")
html_content.write("body { font-family: Arial, sans-serif; margin: 20px; }")
html_content.write("h1 { color: #2c3e50; }")
html_content.write(".info-section { background: #f8f9fa; padding: 15px; margin: 10px 0; border-radius: 5px; }")
html_content.write(".label { font-weight: bold; color: #34495e; }")
html_content.write("</style>")
html_content.write("</head>")
html_content.write("<body>")
html_content.write("<h1>🖥️ System Information</h1>")

html_content.write("<div class='info-section'>")
html_content.write("<h2>Python Environment</h2>")
html_content.write(f"<p><span class='label'>Python Version:</span> {sys.version}</p>")
html_content.write(f"<p><span class='label'>Python Executable:</span> {sys.executable}</p>")
html_content.write(f"<p><span class='label'>Platform:</span> {platform.platform()}</p>")
html_content.write("</div>")

html_content.write("<div class='info-section'>")
html_content.write("<h2>System Information</h2>")
html_content.write(f"<p><span class='label'>System:</span> {platform.system()}</p>")
html_content.write(f"<p><span class='label'>Machine:</span> {platform.machine()}</p>")
html_content.write(f"<p><span class='label'>Processor:</span> {platform.processor()}</p>")
html_content.write(f"<p><span class='label'>Current Time:</span> {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}</p>")
html_content.write("</div>")

html_content.write("</body>")
html_content.write("</html>")

# Get the content and calculate length
content = html_content.getvalue()
content_length = len(content.encode('utf-8'))

# Print proper HTTP headers with Content-Length
print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html; charset=UTF-8\r")
print(f"Content-Length: {content_length}\r")
print("Connection: close\r")
print("\r")

# Print the HTML content
print(content)