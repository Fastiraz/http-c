#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8000

#define okay(msg, ...) printf("\x1b[48;5;235m\x1b[92m\x1b[1m[+]\x1b[0m " msg "\n", ##__VA_ARGS__)
#define erro(msg, ...) printf("\x1b[48;5;235m\x1b[91m\x1b[1m[-]\x1b[0m " msg "\n", ##__VA_ARGS__)
#define warn(msg, ...) printf("\x1b[48;5;235m\x1b[38;5;215m\x1b[1m[*]\x1b[0m " msg "\n", ##__VA_ARGS__)
#define info(msg, ...) printf("\x1b[48;5;235m\x1b[38;5;117m\x1b[1m[i]\x1b[0m " msg "\n", ##__VA_ARGS__)

int main(int argc, char *argv[]) {
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

  close(socket_fd);
  return EXIT_SUCCESS;
}
