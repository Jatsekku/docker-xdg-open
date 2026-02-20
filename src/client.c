#include <arpa/inet.h>
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define IP_ENV "DOCKER_XDG_OPEN_IP"
#define PORT_ENV "DOCKER_XDG_OPEN_PORT"

#define DEFAULT_IP "host.docker.internal"
#define DEFAULT_PORT (9000)

static inline void prepareSocketAddress(struct sockaddr_in *const socket_address) {
	assert((NULL != socket_address)
	       && "socket_address cannot be NULL");

    memset(socket_address, 0, sizeof(*socket_address)); 
   
    // Establish ip
    const char *ip = getenv(IP_ENV);
    if (ip == NULL) {
        ip = DEFAULT_IP;
    }

    // Establish port
    const char *port_str = getenv(PORT_ENV);
    const uint16_t port = (port_str != NULL) ? (uint16_t)atoi(port_str) : DEFAULT_PORT;

    socket_address->sin_family = AF_INET;
    socket_address->sin_port = htons(port);

    const int ret = inet_pton(AF_INET, ip, &socket_address->sin_addr);
    assert((ret == 1)
           && "Invalid IP address");
}

static inline int initSocket(const struct sockaddr_in *const socket_address) {
	assert((NULL != socket_address)
	       && "socket_address cannot be NULL");

    // Create new socket
    const int new_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert((-1 != new_socket_fd)
	       && "Failed to create socket");

    // Connect to given address
    const int ret = connect(new_socket_fd,
                            (const struct sockaddr *)socket_address,
                            sizeof(struct sockaddr_in));
    assert((-1 != ret)
	       && "Failed to connect to server");

    return new_socket_fd;
}

static inline void sendString(int socket_fd, const char *const message) {
    assert((0 <= socket_fd)
           && "Invalid socket fd");
    assert((NULL != message)
           && "message cannot be NULL");

    // Send string
    const size_t message_size = strlen(message);
    // It may fail but for now I don't care
    write(socket_fd, message, message_size);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    const char *const url = argv[1];

    struct sockaddr_in socket_address;
    prepareSocketAddress(&socket_address);
    const int socket_fd = initSocket(&socket_address);

    sendString(socket_fd, url);
    
    close(socket_fd);
    return 0;
}

