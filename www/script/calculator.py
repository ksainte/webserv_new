#!//usr/bin/python3

import os
import sys
import cgitb
import io
import re
from urllib.parse import parse_qs

# Enable error reporting
cgitb.enable()

def safe_eval(expression):
    """
    Safely evaluate mathematical expressions with basic operations only.
    Only allows numbers, basic operators, and parentheses.
    """
    # Remove all whitespace
    expression = expression.replace(' ', '')
    
    # Check for valid characters only
    if not re.match(r'^[0-9+\-*/().]+$', expression):
        return None, "Invalid characters in expression"
    
    # Check for balanced parentheses
    if expression.count('(') != expression.count(')'):
        return None, "Unbalanced parentheses"
    
    # Check for consecutive operators
    if re.search(r'[+\-*/]{2,}', expression):
        return None, "Invalid expression: consecutive operators"
    
    # Check for division by zero
    if '/0' in expression or '/0.0' in expression:
        return None, "Division by zero"
    
    try:
        # Use eval with restricted globals and locals
        result = eval(expression, {"__builtins__": {}}, {})
        
        # Check if result is a number
        if not isinstance(result, (int, float)):
            return None, "Invalid expression"
        
        return result, None
    except ZeroDivisionError:
        return None, "Division by zero"
    except Exception as e:
        return None, f"Evaluation error: {str(e)}"

def generate_html(expression, result, error=None):
    """Generate the HTML content for the calculator response."""
    html_content = io.StringIO()
    
    html_content.write("<!DOCTYPE html>")
    html_content.write("<html>")
    html_content.write("<head>")
    html_content.write("<title>CGI Calculator</title>")
    html_content.write("<style>")
    html_content.write("body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }")
    html_content.write(".calculator { background: white; padding: 20px; border: 1px solid #ccc; max-width: 400px; margin: 0 auto; }")
    html_content.write("h1 { color: #333; text-align: center; margin-bottom: 20px; }")
    html_content.write(".expression { background: #f9f9f9; padding: 10px; border: 1px solid #ddd; margin: 10px 0; font-family: monospace; }")
    html_content.write(".result { background: #d4edda; color: #155724; padding: 10px; border: 1px solid #c3e6cb; margin: 10px 0; }")
    html_content.write(".error { background: #f8d7da; color: #721c24; padding: 10px; border: 1px solid #f5c6cb; margin: 10px 0; }")
    html_content.write(".info { background: #d1ecf1; color: #0c5460; padding: 10px; border: 1px solid #bee5eb; margin: 10px 0; }")
    html_content.write("</style>")
    html_content.write("</head>")
    html_content.write("<body>")
    html_content.write("<div class='calculator'>")
    html_content.write("<h1>CGI Calculator</h1>")
    
    html_content.write("<div class='expression'>")
    html_content.write(f"<strong>Expression:</strong> {expression}")
    html_content.write("</div>")
    
    if error:
        html_content.write("<div class='error'>")
        html_content.write(f"<strong>Error:</strong> {error}")
        html_content.write("</div>")
    else:
        html_content.write("<div class='result'>")
        html_content.write(f"<strong>Result:</strong> {result}")
        html_content.write("</div>")
    
    html_content.write("<div class='info'>")
    html_content.write("<strong>Supported Operations:</strong><br>")
    html_content.write("• Addition (+)<br>")
    html_content.write("• Subtraction (-)<br>")
    html_content.write("• Multiplication (*)<br>")
    html_content.write("• Division (/)<br>")
    html_content.write("• Parentheses for grouping<br>")
    html_content.write("• Decimal numbers")
    html_content.write("</div>")
    
    html_content.write("</div>")
    html_content.write("</body>")
    html_content.write("</html>")
    
    return html_content.getvalue()

# Get the query string from environment
query_string = os.getenv('QUERY_STRING', '')
expression = ""

if query_string:
    # Parse the query string
    parsed = parse_qs(query_string)
    expression = parsed.get('expression', [''])[0]

# If no expression provided, use a default
if not expression:
    expression = "2+3*4"

# Evaluate the expression
result, error = safe_eval(expression)

# Generate HTML content
content = generate_html(expression, result, error)
content_length = len(content.encode('utf-8'))

# Print HTTP headers
print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html; charset=UTF-8\r")
print(f"Content-Length: {content_length}\r")
print("Connection: close\r")
print("\r")

# Print the HTML content
print(content)