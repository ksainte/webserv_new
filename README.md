# II

TODO

prepare envcgi  change status code to the correct one and handle exception

access & isDir  verify that user has permission, otherwise return 403

verify status code 

cgi env    check that env var name contains only alphanumeric character

post    send a 200 Ok response even when there is no cgi

execve send response when cgi file not found 

# Redirection

This directive told the server to send a HTTP redirect response containing
a given status code and a url.

Syntax

return <status code> <url>;

ex : 

return 301 example.com;

# Parsing

Must contain 2 arguments 

Can be present in server block and/or location block

return => status code => url => semicolon

Must check the status code validity: 299 < status code <= 399

# Config file loading

This directive will be stored inside the location directive map
When calling its getter function, the return value will be a vector
of string containing two value: the status code and the url;

# Execution

The return directive takes precedence over all other directive,
meaning the first instruction during response generation must be 
to check if the return directive is present for the given location;

If It's present, the server must generate an appropriate response containing
the status code and :

-   in case of a redirection, the header Location must be set with the correct url

The server must then send this response to the client and close the connection.
    





    
