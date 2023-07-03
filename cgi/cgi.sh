#!/bin/bash

replace_string() {
	input_string=$1
	replaced_string="${input_string//host_here/$CONFIG_HOST}"
	echo "${replaced_string//port_here/$CONFIG_PORT}"
}

# Se supone que está comprobado que el archivo no existe
if [ "$REQUEST_METHOD" == "GET" ]
then
    # Verificar si el path es un directorio
    if [ -d "$1" ]; then
        # Generar el listado de directorios y archivos en un HTML
        echo "HTTP/1.1 $REQUEST_STATUS OK"
        echo "Content-Type: text/html"
        echo ""
        echo "<html>"
        echo "<head>"
        echo "<title>Directory Listing: $1</title>"
        echo "</head>"
        echo "<body>"
        echo "<h1>Directory Listing: $1</h1>"
        echo "<ul>"
        
        # Recorrer los archivos y carpetas dentro del directorio
        for file in "$1"/*; do
            filename=$(basename "$file")
            
            # Verificar si es un archivo o un directorio
            if [ -d "$file" ]; then
                echo "<li><strong>$filename/</strong></li>"
            else
                echo "<li>$filename</li>"
            fi
        done
        
        echo "</ul>"
        echo "</body>"
        echo "</html>"
    else
        filename=$(basename "$1")
        extension="${filename##*.}"

        # Si el path no es un directorio, continuar con el código existente para manejar archivos
        content_type=""
        
        # Establecer el tipo de contenido en función de la extensión
        case "$extension" in
            jpg|jpeg)
                content_type="image/jpeg"
                ;;
            png)
                content_type="image/png"
                ;;
            gif)
                content_type="image/gif"
                ;;
            # Agrega más extensiones y tipos de contenido según sea necesario

            # *)
            #     content_type=""
            #     ;;
        esac

        if [ -z "$content_type" ]; then
            content=$(cat "$1")
            modified_content=$(replace_string "$content")
            size=${#modified_content}
            echo "HTTP/1.1 $REQUEST_STATUS OK"
            echo "Content-Length: $size"
            echo ""
            echo "$modified_content"
        else
            size=$(wc -c < "$1")
            echo "HTTP/1.1 $REQUEST_STATUS OK"
            echo "Content-Type: $content_type"
            echo "Content-Length: $size"
            echo ""
            cat "$1"
        fi
    fi
fi

if [ "$REQUEST_METHOD" == "POST" ]; then
    echo "HTTP/1.1 $REQUEST_STATUS OK"
    echo "Content-Type: text/plain"
    echo "Content-Length: 24"
    echo ""
    echo "El archivo se ha subido con éxito."
fi



if [ "$REQUEST_METHOD" == "DELETE" ]
then
	rm -rf "$REQUEST_ROUTE"
	echo "HTTP/1.1 $REQUEST_STATUS OK"
	echo ""
fi

