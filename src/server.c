#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 8000
#define BUFFER_SIZE 4096

#define okay(msg, ...) printf("\x1b[48;5;235m\x1b[92m\x1b[1m[+]\x1b[0m " msg "\n", ##__VA_ARGS__)
#define erro(msg, ...) printf("\x1b[48;5;235m\x1b[91m\x1b[1m[-]\x1b[0m " msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf("\x1b[48;5;235m\x1b[38;5;215m\x1b[1m[*]\x1b[0m " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("\x1b[48;5;235m\x1b[38;5;117m\x1b[1m[i]\x1b[0m " msg "\n", ##__VA_ARGS__)

typedef struct Request {
  char *method;
  char *path;
} req;

req parse_request (char*);

int main(int argc, char *argv[], char *envp[]) {
  if (argc != 1)
    return EXIT_FAILURE;

  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  okay("Socket created successfully");

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  bind(socket_fd, (struct sockaddr *)&server, sizeof(server));
  okay("Binding successful");

  listen(socket_fd, 3);
  info("Listening on port %d", PORT);

  while (1) {
    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int client_fd = accept(socket_fd, (struct sockaddr *)&client, &client_len);
    okay("New connection accepted from %s:%d", inet_ntoa(client.sin_addr), ntohs(client.sin_port));

    char buffer[BUFFER_SIZE] = {0};

    if (read(client_fd, buffer, BUFFER_SIZE - 1) > 0) {
      okay("Received request:\n%s", buffer);
      req result = parse_request(buffer);
      info("Method: %s\n", result.method);
      info("Path: %s\n", result.path);
    } else {
      warn("No data received or read failed.");
    }

    char response[] =
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 19\r\n"
      "\r\n"
      "HTTP from scratch!\n";

    send(client_fd, response, sizeof(response) - 1, 0);
    close(client_fd);
    info("Connection closed");
  }

  close(socket_fd);
  return EXIT_SUCCESS;
}

req parse_request(char *request) {
  req result;

  char *method = strtok(request, " ");
  char *path = strtok(NULL, " ");

  result.method = strdup(method);
  result.path = strdup(path);

  return result;
}
