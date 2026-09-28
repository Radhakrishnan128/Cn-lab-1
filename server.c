#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT 8080

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        perror("Socket failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(1);
    }

    listen(server_fd, 5);

    printf("Server waiting for clients...\n");

    while (1) {
        client_fd = accept(server_fd,
                           (struct sockaddr *)&client_addr,
                           &addr_len);

        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            char buffer[100];
            char response[200];
            int id;
            FILE *fp;
            int found = 0;

            close(server_fd);

            read(client_fd, buffer, sizeof(buffer));
            id = atoi(buffer);

            fp = fopen("data.txt", "r");

            if (fp == NULL) {
                strcpy(response, "File not found");
            } else {
                char line[100];
                int record_id;

                while (fgets(line, sizeof(line), fp)) {
                    sscanf(line, "%d", &record_id);

                    if (record_id == id) {
                        snprintf(response, sizeof(response),
                                 "Record Found : %s", line);
                        found = 1;
                        break;
                    }
                }

                fclose(fp);

                if (!found) {
                    strcpy(response, "Record Not Found");
                }
            }

            write(client_fd, response, strlen(response) + 1);
            close(client_fd);
            exit(0);
        }

        close(client_fd);
    }

    close(server_fd);
    return 0;
}
