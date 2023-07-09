#!/bin/bash

replace_string() {
	input_string=$1
	replaced_string="${input_string//host_here/$CONFIG_HOST}"
	echo "${replaced_string//port_here/$CONFIG_PORT}"
}

# Se supone que está comprobado que el archivo no existe
if [ "$REQUEST_METHOD" == "GET" ]
then
    if [ -n "$REQUEST_REDIR" ]; then
        echo "HTTP/1.1 301 Moved Permanently"
        echo "Location: $REQUEST_REDIR"
    # Verificar si el path es un directorio
    elif [ -d "$1" ]; then
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
            head -c 50000 "$1"
        fi
    fi
fi

# Ya esta comprobado que el archivo no existe
if [ "$REQUEST_METHOD" == "POST" ]; then
    # Obtener la extensión del archivo
    filename=$(basename "$REQUEST_FILE")
    extension="${filename##*.}"

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

    content_length=$(wc -c < "$REQUEST_FILE")
    echo "HTTP/1.1 $REQUEST_STATUS OK"
    if [ -n "$content_type" ]; then
        echo "Content-Type: $content_type"
    fi
    echo "Content-Length: $content_length"
    echo ""
    head -c 50000 "$REQUEST_FILE"
fi

if [ "$REQUEST_METHOD" == "DELETE" ]
then
    rm -rf "$REQUEST_ROUTE"
    echo "HTTP/1.1 $REQUEST_STATUS OK"
    echo ""
fi

