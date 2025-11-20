#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#define MAX_EVENTS 64
#define XOR_KEY 0x55
#define MAX_DEVICES 1

typedef struct {
  int fd;
  struct sockaddr_in addr;
} client_t;
int client_count = 0;
client_t client[MAX_DEVICES];


pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void xor(char *data, int len, int key) {
  for (int i = 0;i < len;i++) {
    data[i] ^= key;
  }
}



int sock_setup() {
int sock = 0;
int reuse_addr = 1; 
sock = socket(AF_INET, SOCK_STREAM, 0);
if (sock == -1) {
  printf("socket fd %d %s\n", errno, strerror(errno));
  exit(EXIT_FAILURE);
}
struct sockaddr_in device_addr;
device_addr.sin_family = AF_INET;
device_addr.sin_port = htons(4444);
  if (inet_pton(AF_INET, "192.168.1.1", &device_addr.sin_addr.s_addr) == -1) {
    printf("inet_pton  %d %s\n", errno, strerror(errno));
	  exit(EXIT_FAILURE);
  }
  socklen_t device_addr_len = sizeof(device_addr);
  if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof(reuse_addr)) == -1) {
	  printf("setsockopt socket %d %s\n", errno, strerror(errno));
	  exit(EXIT_FAILURE);
  }
  if (bind(sock, (struct sockaddr *) &device_addr, device_addr_len) == -1) {
	  printf("bind socket %d %s\n", errno, strerror(errno));
	  exit(EXIT_FAILURE);
  }
  if (listen(sock, 3) == -1) {
    printf("listen socket %d %s\n", errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
return sock;

}
void *sock_listen(void *s) {
struct sockaddr_in device;
socklen_t device_len = sizeof(device);
int sock = *(int *)s;
int epollfd = epoll_create1(0);

  if (epollfd == -1) {
	  printf("epoll_create1 listen sock%d %s\n", errno, strerror(errno));
	  exit(EXIT_FAILURE);
  }

struct epoll_event ev;
struct epoll_event events[MAX_EVENTS];
ev.data.fd = sock;
ev.events = EPOLLIN;

  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, sock, &ev) == -1) {
    printf("epoll_ctl sock %d %s\n", errno, strerror(errno));
	  exit(EXIT_FAILURE);
  }
while (1) {
  int nfds = 0;
  nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
  if (nfds == -1) {
	  printf("epoll_wait sock%d %s\n", errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  if (nfds > 0) {
    for (int i = 0;i < nfds;i++) {
	    if (events[i].events & EPOLLIN) {
		    if (events[i].data.fd == sock) {

			    if (client[client_count].fd = accept(sock, (struct sockaddr *) &device, &device_len)) {
            pthread_mutex_lock(&lock);
				    client[client_count].addr = device;
				    client_count++;
				    close(epollfd);
            pthread_mutex_unlock(&lock);
				    return NULL;
			      } else {
			        printf("accept client connection %d %s\n", errno, strerror(errno));
		  	    }
		      }
	      }
      }
    }
  }
}

void *sock_input(void *device_cli) {
  client_t dev = *(client_t *)device_cli;
  int device_fd = dev.fd;
  int epollfd = -1;
 
  epollfd = epoll_create1(0);
  if (epollfd == -1) {
    printf("epoll create cli%d %s\n");
    exit(EXIT_FAILURE);
  }

  struct epoll_event ev;
  struct epoll_event events[MAX_EVENTS];
  ev.data.fd = device_fd;
  ev.events = EPOLLIN | EPOLLHUP;
  if (epoll_ctl(epollfd, EPOLL_CTL_ADD, device_fd, &ev) == -1) {
    printf("epoll ctl cli%d %s\n");
    exit(EXIT_FAILURE);
  }
  while (1) {
    int nfds = 0;
    nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
    if (nfds == -1) {
      printf("%d %s");
      exit(EXIT_FAILURE);
    }
    if (nfds > 0) {
	    for (int i = 0;i < nfds;i++) {
		    if (events[i].data.fd == device_fd) {
			    if (events[i].events & EPOLLIN) {
			      char device_buffer[8096] = {0};
			      ssize_t read_device_bytes = 0;
			      read_device_bytes = read(device_fd, device_buffer, sizeof(device_buffer));
			      xor(device_buffer, strlen(device_buffer), XOR_KEY);
			      if (read_device_bytes == -1) {
				      printf("read neg device %d %s\n");
			      } else {
			        ssize_t write_stdout_bytes = 0;
			        write_stdout_bytes = write(STDOUT_FILENO, device_buffer, read_device_bytes);
			        fflush(stdout);
			        if (write_stdout_bytes == -1) {
				        printf("stdout neg device %d %s\n");
			        }
	          }
		    } if (events[i].events & EPOLLHUP) {
		        socklen_t dev_len = sizeof(dev.addr);
		        char IP[INET_ADDRSTRLEN];
		        printf("IP: %s disconnect \n", inet_ntop(dev_len, &dev.addr.sin_addr.s_addr, IP, INET_ADDRSTRLEN));
		      }       
		    }
      }
    } 
  }
}

void *sock_output(void *device_cli) {
  client_t dev = *(client_t *)device_cli;
  int device_fd = dev.fd;
  int epollfd = epoll_create1(0);
  if (epollfd == -1) {
    printf("epollfd create%d %s\n");
    exit(EXIT_FAILURE);
  }
 
  struct epoll_event ev;
  struct epoll_event events[MAX_EVENTS];
  ev.events = EPOLLIN;
  ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
      printf("epoll ctl add sock output%d %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }

  char stdin_buffer[8096] = {0};
    if (fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK) == -1) {
      printf("F_SETFL stdin%d %s\n");
      exit(EXIT_FAILURE);
    }
    while(1) {
    int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1); 
    if (nfds == -1) {
      printf("NFDS EPOLL WAIT%d %s\n");
      exit(EXIT_FAILURE);   
    }
    if (nfds > 0) {
      for (int i = 0;i < nfds;i++) {
        if (events[i].data.fd == STDIN_FILENO)  {
          if (events[i].events & EPOLLIN) {  
          ssize_t read_stdin_bytes = 0;
          read_stdin_bytes = read(STDIN_FILENO, stdin_buffer, sizeof(stdin_buffer));
	        xor(stdin_buffer, strlen(stdin_buffer), XOR_KEY);
	          if (read_stdin_bytes == -1) {
		          printf("read stdin %d %s\n");
	          } else {
	            ssize_t write_device_bytes = 0;
			        write_device_bytes = write(device_fd, stdin_buffer, read_stdin_bytes);
			          if (write_device_bytes == -1) {
			            printf("write device bytes %d %s\n");
		            }
            }	
          }
        }
      }
    }
  }
}
int main() {
pthread_t sockthr;
int s = 0;
  s = sock_setup();
    if (pthread_create(&sockthr, NULL, sock_listen, &s) != 0) {
      printf("pthread_create sockthr%d %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }
    if (pthread_join(sockthr, NULL) != 0) {   
      printf("pthread_join sockthr%d %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }

    pthread_t sockinthr;

    if (pthread_create(&sockinthr, NULL, sock_input, &client[0]) != 0) {   
      printf("pthread_create sockinthr%d %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }
    pthread_t sockoutthr;
    if (pthread_create(&sockoutthr, NULL, sock_output, &client[0]) != 0) {   
      printf("pthread_create sockoutthr%d %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }
    if (pthread_join(sockoutthr, NULL) != 0) {   
      printf("pthread_join sockthr%d %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }
    return 0;
}

