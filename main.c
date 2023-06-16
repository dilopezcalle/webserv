#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 2

int main() {
    int server_socket1, server_socket2, new_socket, activity;
    struct sockaddr_in address1, address2, client_address;
    socklen_t client_length = sizeof(client_address);
    int client_sockets[MAX_CLIENTS] = {0};
    fd_set readfds;
    int max_sd, sd, i, valread;
    char buffer[1024] = {0};

    // Crear el primer socket
    if ((server_socket1 = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket 1");
        exit(EXIT_FAILURE);
    }

    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = INADDR_ANY;
    address1.sin_port = htons(8080);

    if (bind(server_socket1, (struct sockaddr *)&address1, sizeof(address1)) < 0) {
        perror("Error en el bind del socket 1");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket1, 5) < 0) {
        perror("Error en el listen del socket 1");
        exit(EXIT_FAILURE);
    }

    // Crear el segundo socket
    if ((server_socket2 = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Error al crear el socket 2");
        exit(EXIT_FAILURE);
    }

    address2.sin_family = AF_INET;
    address2.sin_addr.s_addr = INADDR_ANY;
    address2.sin_port = htons(9090);

    if (bind(server_socket2, (struct sockaddr *)&address2, sizeof(address2)) < 0) {
        perror("Error en el bind del socket 2");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket2, 5) < 0) {
        perror("Error en el listen del socket 2");
        exit(EXIT_FAILURE);
    }

    // Limpiar el conjunto de descriptores de archivo (file descriptor set)
    FD_ZERO(&readfds);

    while (1) {
        // Añadir los sockets a monitorear al conjunto de descriptores de archivo
        FD_SET(server_socket1, &readfds);
        FD_SET(server_socket2, &readfds);

        // Obtener el descriptor de archivo más grande
        max_sd = (server_socket1 > server_socket2) ? server_socket1 : server_socket2;

        // Esperar a que ocurra una actividad en alguno de los sockets
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("Error en el select");
            exit(EXIT_FAILURE);
        }

        // Verificar si hay actividad en el primer socket
        if (FD_ISSET(server_socket1, &readfds)) {
            if ((new_socket = accept(server_socket1, (struct sockaddr *)&client_address, &client_length)) < 0) {
                perror("Error al aceptar la conexión del socket 1");
                exit(EXIT_FAILURE);
            }

            printf("Nuevo cliente en el primer servidor: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

            // Agregar el nuevo socket a la matriz de sockets de clientes
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Verificar si hay actividad en el segundo socket
        if (FD_ISSET(server_socket2, &readfds)) {
            if ((new_socket = accept(server_socket2, (struct sockaddr *)&client_address, &client_length)) < 0) {
                perror("Error al aceptar la conexión del socket 2");
                exit(EXIT_FAILURE);
            }

            printf("Nuevo cliente en el segundo servidor: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

            // Agregar el nuevo socket a la matriz de sockets de clientes
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    break;
                }
            }
        }

        // Verificar si hay actividad en los sockets de clientes
        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_sockets[i];

            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, 1024)) == 0) {
                    // Cliente desconectado
                    printf("Cliente desconectado\n");
                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    // Procesar los datos recibidos
                    // ...

                    // Enviar respuesta al cliente
                    send(sd, buffer, strlen(buffer), 0);
                    memset(buffer, 0, sizeof(buffer));
                }
            }
        }
    }

    return 0;
}