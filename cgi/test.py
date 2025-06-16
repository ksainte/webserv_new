#!/usr/bin/env python3

import os
import sys

html_content = """
<html>
<head><title>CGI Test</title></head>
<body>
<h1>CGI Test Page</h1>
<p>This is a test CGI script.</p>
<p>Environment variables:</p>
<ul>
"""

for key, value in os.environ.items():
    html_content += f"<li>{key}: {value}</li>\n"

html_content += """
</ul>
</body>
</html>
"""

print("HTTP/1.1 200 OK")
print("Content-Type: text/html")
print(f"Content-Length: {len(html_content)}")
print()
print(html_content, end='') 