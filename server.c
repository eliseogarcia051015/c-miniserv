    // server.c - Super simple HTTP server
    #include <sys/socket.h>   // socket, bind, listen, accept
    #include <netinet/in.h>   // sockaddr_in, INADDR_ANY
    #include <fcntl.h>        // open, O_RDONLY
    #include <sys/sendfile.h> // sendfile
    #include <unistd.h>       // close, read
    #include <string.h>       // strchr
    #include <stdio.h>        // printf
    #include <arpa/inet.h>

    void edithtml(){
        char msg[1024];
        printf("Enter something: ");
        fflush(stdout);
        fgets(msg, sizeof(msg), stdin); // safer than scanf("%s")
        printf("You typed: %s\n", msg);
    }

    int main() {
        edithtml();
        fflush(stdout);

        char buffer[1024] = {0};
        int port = 8080; //can be changed

        // 1. Create TCP socket (IPv4)
        int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd < 0) { perror("socket"); return 1; }

        // 2. Prepare address struct
        struct sockaddr_in addr = {
            .sin_family = AF_INET,
            .sin_port = htons(port),             // Port 8080
            .sin_addr = { .s_addr = INADDR_ANY },// Bind all interfaces
            .sin_zero = {0}                      // Padding
        };

        // 3. Bind socket to address
        if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("bind"); return 1;
        }

        // 4. Listen for incoming connections
        if (listen(server_fd, 10) < 0) { perror("listen"); return 1; }

        const char *default_file = "file.html"; // default file to show
        printf("Server listening on port %d...\n", port);
        printf("\nGo to this link: http://localhost:%d/%s\n", port, default_file); 
        fflush(stdout); // make sure it prints immediately

        // 5. Accept a client connection
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) { perror("accept"); return 1; }

        // 6. Receive data from client
        ssize_t received = recv(client_fd, buffer, sizeof(buffer), 0);
        if (received < 0) { perror("recv"); return 1; }
        buffer[received] = 0; // null-terminate

        // 7. Very basic GET parsing: "GET /file.html"
        if (strncmp(buffer, "GET ", 4) == 0) {
            char* f = buffer + 4;
            char* space = strchr(f, ' ');
            if (space) *space = 0; // terminate filename
            if (f[0] == '/') f++;   // skip leading '/'

            // 8. Open requested file
            int file_fd = open(f, O_RDONLY);
            if (file_fd >= 0) {
                sendfile(client_fd, file_fd, NULL, 1024); // send file
                close(file_fd);
            }
        }


        // 9. Close client and server sockets
        close(client_fd);
        close(server_fd);

        return 0;
    }
