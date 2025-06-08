#!/usr/bin/python3

import sys
import time
import os

def main():
    # Print CGI headers
    print("Content-Type: text/html")
    print("Content-Length: 1000")  # Approximate content length
    print()  # Empty line to end headers
    
    # Print HTML start
    print("<!DOCTYPE html>")
    print("<html><head><title>Infinite Loop Test</title></head>")
    print("<body>")
    print("<h1>Testing Infinite Loop for Timeout</h1>")
    print("<p>This script will run forever unless killed by timeout...</p>")
    print("<div id='counter'>")
    
    # Flush output so headers are sent immediately
    sys.stdout.flush()
    
    # Infinite loop - should be killed by timeout after 60 seconds
    counter = 0
    while True:
        counter += 1
        print(f"<p>Loop iteration: {counter} (Time: {time.time()})</p>")
        
        # Flush output so we can see progress
        sys.stdout.flush()
        
        # Sleep for 1 second to make it easier to observe
        time.sleep(1)
        
        # This loop will continue forever unless killed by the timeout mechanism

if __name__ == "__main__":
    main() 