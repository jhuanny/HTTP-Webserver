This program implements a simple webserver that supports a subset of the Hypertext Transfer Protocol (HTTP). It handles multiple client requests by forking child processes for each request and supports program invocation similar to the Common Gateway Interface (CGI) approach. Key functionalities and features include:

## Client Request Handling: 
The server handles multiple client requests by spawning child processes, each responsible for processing a single request.
## HTTP Support: 
The server supports two HTTP request types: HEAD and GET. It reads the requested file and sends the file contents back to the client for GET requests, and only the file information for HEAD requests.
## Error Handling: 
The server responds to erroneous requests with appropriate HTTP error responses, such as 400 Bad Request, 403 Permission Denied, 404 Not Found, etc.
## Resource Management:
The server manages resources efficiently to ensure it runs continuously without memory leaks or file descriptor issues.

# Key Features
## Command-Line Argument:
The program, named httpd, takes one command-line argument specifying the port number (between 1024 and 65535) on which to listen for connections.

## Multi-Process Handling:
The server spawns child processes to handle each client request.
Unneeded file descriptors are cleaned up, and child processes are managed using a signal handler.

## HTTP Request Types:
GET: The server reads the requested file and sends its contents back to the client.
HEAD: The server sends only the file information without the contents.
Requests follow the format: TYPE filename HTTP/version (e.g., GET /index.html HTTP/1.1).

## HTTP Response:
The response includes a header with information about the response type, content type, and content length, ending with a blank line.
Example header for a valid request:

HTTP/1.0 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 5686\r\n
\r\n
<---- contents here ---->

The stat system call is used to determine the content length.

## Error Responses:
The server handles erroneous requests with appropriate HTTP error responses, including a content type of text/html and an HTML snippet in the response body.
Common error responses include:
HTTP/1.0 400 Bad Request
HTTP/1.0 403 Permission Denied
HTTP/1.0 404 Not Found
HTTP/1.0 500 Internal Error
HTTP/1.0 501 Not Implemented

## Resource Management:
The program ensures proper management of memory and file descriptors.
Use the top program to monitor memory usage during execution.
