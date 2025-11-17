/* client.c code below */

/*
Questions to answer at top of client.c:
(You should not need to change the code in client.c)
1. What is the address of the server it is trying to connect to (IP address and port number).
   - IP address: 127.0.0.1
   - Port number: 8000

2. Is it UDP or TCP? How do you know?
   - It is TCP becuase the socket call uses SOCK_STREAM:
   - socket(AF_INET, SOCK_STREAM, 0);

3. The client is going to send some data to the server. Where does it get this data from? How can you tell in the code?
   server reads data when user type on keyboard we can see in code here:
   - read(STDIN_FILENO, buf, BUF_SIZE);
      so the data comes from whatever the user types

4. How does the client program end? How can you tell that in the code?
   -the client stops when user input spmething, stop typing or pree ctrl + D and read() returns <= 1
   it makes  this line (num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1 return <= 1 and ends the
   loop.
   After that, it closes the socket with close(sfd);
   and then calls exit(EXIT_SUCCESS); which ends the program.
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR "127.0.0.1"

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }

  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }

  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}

/* server.c code below           */




#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define PORT 8000
#define LISTEN_BACKLOG 32

#define handle_error(msg)                      \
  do {                                         \
    perror(msg);                               \
    exit(EXIT_FAILURE);                        \
  } while (0)

// Shared counters for: total # messages, and counter of clients (used for
// assigning client IDs)
int total_message_count = 0;
int client_id_counter = 1;

// Mutexs to protect above global state.
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};

void *handle_client(void *arg) {
  struct client_info *client = (struct client_info *)arg;
  int cfd = client->cfd;
  int client_id = client->client_id;
  char buffer[BUF_SIZE];
  ssize_t num_read;

  printf("New client created! ID %d on socket FD %d\n", client_id, cfd);

  free(client);

  while ((num_read = read(cfd, buffer, BUF_SIZE - 1)) > 0) {
    buffer[num_read] = '\0'; 

    pthread_mutex_lock(&count_mutex);
    total_message_count++;
    int message_index = total_message_count;
    pthread_mutex_unlock(&count_mutex);

    printf("Msg #%4d; Client ID %d: %s", message_index, client_id, buffer);
  }

  printf("Ending thread for client %d\n", client_id);
  close(cfd);

  return NULL;
}

int main() {
  struct sockaddr_in addr;
  int sfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error("listen");
  }

  for (;;) {
    int cfd = accept(sfd, NULL, NULL);
    if (cfd == -1) {
      handle_error("accept");
    }

    pthread_mutex_lock(&client_id_mutex);
    int assigned_id = client_id_counter++;
    pthread_mutex_unlock(&client_id_mutex);

    struct client_info *info = malloc(sizeof(struct client_info));
    info->cfd = cfd;
    info->client_id = assigned_id;

    pthread_t tid;
    if (pthread_create(&tid, NULL, handle_client, info) != 0) {
      perror("pthread_create");
      free(info);
      close(cfd);
      continue;
    }

    pthread_detach(tid);
  }

  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}




