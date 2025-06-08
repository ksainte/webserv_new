#!//usr/bin/python3

import io
import os
import sys
from http.client import parse_headers

def process_multipart_form_data(environ, input_stream):
    response_parts = []
    try:
        content_type = environ.get('CONTENT_TYPE', '')
        if not content_type.startswith('multipart/form-data'):
            return "Error: Expected multipart/form-data"

        boundary_bytes = f'--{content_type.split("boundary=")[-1]}'.encode()
        body = input_stream.read()
        parts = body.split(boundary_bytes)

        for part in parts:
            part = part.strip(b'\r\n')
            if not part or part == b'\x00':
                continue

            try:
                headers_part, data_part = part.split(b'\r\n\r\n', 1)
                headers = parse_headers(io.BytesIO(headers_part.strip()))
                content_disposition = headers.get('Content-Disposition')

                if content_disposition:
                    disposition_params = {}
                    elements = content_disposition.split(';')
                    disposition_type = elements[0].strip()
                    for element in elements[1:]:
                        if '=' in element:
                            key, value = element.strip().split('=', 1)
                            disposition_params[key.strip()] = value.strip().strip('"')

                    name = disposition_params.get('name')

                    if 'filename' in disposition_params:
                        filename = disposition_params['filename']
                        file_content = data_part.strip(b'\r\n--')
                        try:
                            sanitized_filename = os.path.basename(filename)
                            upload_dir = "uploads"
                            os.makedirs(upload_dir, exist_ok=True)  # Ensure the uploads directory exists
                            filepath = os.path.join(upload_dir, f"{sanitized_filename}")
                            with open(filepath, 'wb') as f:
                                f.write(file_content)
                            response_parts.append(f"Uploaded file '{name}', saved as '{filepath}', size={len(file_content)} bytes.")
                        except Exception as e:
                            response_parts.append(f"Error saving file '{filename}': {e}")
                    elif name:
                        value = data_part.strip(b'\r\n--').decode('utf-8', errors='ignore')
                        response_parts.append(f"Received field '{name}' with value: '{value}'")
            except ValueError:
                continue

        return "\n".join(response_parts)

    except Exception as e:
        return f"Error processing form data: {e}"

if __name__ == '__main__':
    
        with open("cgi_env.log", "w") as f:
            for key, value in os.environ.items():
                f.write(f"{key}={value}\n")

        # content_length = int(os.environ.get("content-length"))
        content_length = int(os.environ.get("CONTENT_LENGTH"))

        body = sys.stdin.buffer.read(content_length)

        input_stream_example = io.BytesIO(body)

        process_multipart_form_data(os.environ, input_stream_example)

        body = """
        <!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>upload</title>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    margin: 40px;
                    background-color: #f4f4f4;
                    color: #333;
                    text-align: center;
                }
                h1 {
                    color: #d63384; /* A different color for no-Flask version */
                }
                p {
                    line-height: 1.6;
                }
                .container {
                    background-color: #ffffff;
                    padding: 20px;
                    border-radius: 8px;
                    box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
                    display: inline-block;
                    max-width: 600px;
                }
                footer {
                    margin-top: 30px;
                    font-size: 0.9em;
                    color: #666;
                }
            </style>
        </head>
        <body>
            <div class="container">
                <h1>Webserv</h1>
                <p>File uploaded successfully</p>
            </div>
        </body>
        </html>
        """

        headers = [
            "HTTP/1.1 200 OK",
            "Content-Type: text/html",
            f"Content-Length: {len(body)}",
            "\r\n"
        ]

        response = "\r\n".join(headers) + body

        print(response)
