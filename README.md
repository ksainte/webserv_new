# II

TODO

request.cpp     add _queryString for query parameter

config-file     implement max body size

connection.cpp:handleError  errorPages must be also set in location block, not only in serverBlock;

handleError doesnt need a return value

access    if root is not set, we must also check relative path

# webserv_new

# CGI

exemple de configuration 

location /upload {
    root /usr/documents
    cgi_pass /upload.py;
    cgi_params CONTENT_LENGTH 120;
    cgi_params CONTENT_TYPE multipart/formdata;
    cgi_params FILENAME file;
    cgi_params FILENAME file2; => doit remplacer ou discard
}

Verifier le chemin d'acces au script => root + / + cgi_pass

check avec stat() et IS_DIR() -> si c'est un repertoire, retourne error
check avec access() l'existence et l'executable (FLAG X_OK)

si il existe, 
Recuperer les variables d'environnment => location->getCgiParams();

Creer l'environnement

iterer a travers la liste -> it->first = nom de la variable et it->second = valeur de la variable

creer la variable -> it->first + "=" + it->second

verifier qu'il n'y a pas de doublons

executer le script

appeler execv(root + / +cgi_pass, argv[], env[]);





    
