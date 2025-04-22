#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080

void *connection_handler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    free(socket_desc);

    char buffer[2048];
    char method[16], url[1024], protocol[16];

    // Receive the HTTP request from client
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        perror("recv failed");
        close(sock);
        return NULL;
    }

    buffer[bytes_received] = '\0';  // Null-terminate the buffer

    // Parse the request line
    sscanf(buffer, "%s %s %s", method, url, protocol);

    // Build the HTTP response
    char response[4096];
    snprintf(response, sizeof(response),
        "HTTP/1.1 200 OK\n"
        "Content-Type: text/html\n"
        "Content-Length: 12\n"
        "Method: %s\n"
        "URL: %s\n"
        "Protocol: %s\n\n"
        "Hello, world!\n\n",
        method, url, protocol);

    // Send the response
    write(sock, response, strlen(response));
    printf("Response sent\n");

    close(sock);
    return NULL;
}

int main() {
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
    
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    while(1) {
        
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
    
        pthread_t thread;
        new_sock = (int*) malloc(sizeof(int)); // ✅ FIXED
        *new_sock = new_socket;
    
        if (pthread_create(&thread, NULL, connection_handler, (void*)new_sock) < 0) {
            perror("could not create thread");
            exit(EXIT_FAILURE);
        }
    
        pthread_detach(thread);
    }
    
    
    return 0;
}
