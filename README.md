# II

TODO

prepare envcgi  change status code to the correct one and handle exception

listdir   afficher seulement les fichier et repertoire
listdir   update href to contruct the abspath correctly when entering subdir

access & isDir  verify that user has permission, otherwise return 403

verify status code 

add cgi timeout check

add request timeout

cgi env    check that env var name contains only alphanumeric character

post    send a 200 Ok response even when there is no cgi

# Eval 

## Config File


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





    
