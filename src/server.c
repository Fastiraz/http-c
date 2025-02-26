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

typedef struct Response {
  char *content;
  int size;
} res;

req parse_request (char*);
res read_page (const char* path);

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

      res page = read_page(result.path);

      char response[BUFFER_SIZE] = {0};
      int response_length = snprintf(response, BUFFER_SIZE,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %d\r\n"
        "\r\n"
        "%s",
        page.size, page.content ? page.content : "File not found");

      send(client_fd, response, sizeof(response) - 1, 0);
    } else {
      warn("No data received or read failed.");
    }

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

res read_page (const char *path) {
  res page = { NULL, 0 };
  FILE *file = fopen(path +1, "rb");

  fseek(file, 0, SEEK_END);
  page.size = ftell(file);
  rewind(file);  /* same as fseek(file, 0, SEEK_SET); */

  page.content = malloc(page.size + 1);
  fread(page.content, page.size, 1, file);
  page.content[page.size] = '\0';

  fclose(file);
  return page;
}
