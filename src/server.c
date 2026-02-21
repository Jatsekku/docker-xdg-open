#include <arpa/inet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define RX_BUFFER_SIZE (2048)

#define PORT_ENV "DOCKER_XDG_OPEN_PORT"

#define DEFAULT_PORT (9000)

static inline void prepareSocketAddress(struct sockaddr_in *const socket_address) {
	assert((NULL != socket_address)
	       && "socket_address cannot be NULL");

    memset(socket_address, 0, sizeof(*socket_address)); 

    // Establish port
    const char *port_str = getenv(PORT_ENV);
    const uint16_t port = (port_str != NULL) ? (uint16_t)atoi(port_str) : DEFAULT_PORT;

    socket_address->sin_family = AF_INET;
    socket_address->sin_port = htons(port);

    socket_address->sin_addr.s_addr = htonl(INADDR_LOOPBACK);
}

static inline int initSocket(const struct sockaddr_in *const socket_address) {
    assert((NULL != socket_address)
	       && "socket_address cannot be NULL");
    
    // Create new socket
    const int new_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    assert((-1 != new_socket_fd)
	       && "Failed to create socket");

    // Allow address reuse
    const int one = 1;
    int ret = setsockopt(new_socket_fd,
                         SOL_SOCKET,
                         SO_REUSEADDR,
                         &one, 
                         sizeof(one));
    assert((-1 != ret)
           && "Failed to set SO_REUSEADDR option");

    // Bind socket with address
    ret = bind(new_socket_fd,
               (const struct sockaddr*)socket_address,
               sizeof(struct sockaddr));
	assert((-1 != ret)
	       && "Failed to bind socket");

    // Start listening on socket
    ret = listen(new_socket_fd, 1);
	assert((-1 != ret)
	       && "Failed to listen on socket");

    return new_socket_fd;
}

static inline int awaitConnection(int socket_fd) {
    const int connection_fd = accept(socket_fd, NULL, NULL);
    return connection_fd;
}

static inline ssize_t receiveAllData(int connection_fd, void *const rx_buffer, size_t rx_buffer_size) {
    assert((0 <= connection_fd)
           && "Invaliid connection_fd");
    assert((NULL != rx_buffer)
           && "rx_buffer cannot be NULL");

    if (rx_buffer_size == 0) {
        return 0;
    }

    size_t rx_size_total = 0;

    while(rx_size_total < rx_buffer_size) {
        ssize_t ret = read(connection_fd,
                           (char *const)rx_buffer + rx_size_total,
                           rx_buffer_size - rx_size_total);
       
        if (ret > 0) {
            rx_size_total += ret;
        } else if (ret == 0) {
            // No more data 
            break;
        } else {
            // Error
            return -1;
        }
    }

    return (ssize_t)rx_size_total;
}

static ssize_t receiveString(int connection_fd, char *const rx_buffer, size_t rx_buffer_size) {
    const ssize_t ret = receiveAllData(connection_fd, rx_buffer, rx_buffer_size - 1);

    if (ret < 0) {
        return -1;
    }

    rx_buffer[ret] = '\0';
    return ret;
}

static void runXdgOpen(const char *const url) {
    assert((NULL != url)
	       && "url cannot be NULL");

    if (strlen(url) == 0) {
        return;
    }

    pid_t pid = fork();

    if (pid == 0) {
        execlp("xdg-open", "xdg-open", url, (char *)NULL);
    }
}

int main(void) {
    char rx_buffer[RX_BUFFER_SIZE];

    struct sockaddr_in socket_address;
    prepareSocketAddress(&socket_address);
    const int socket_fd = initSocket(&socket_address);

    while(true) {
        // Wait for client
        const int connection_fd = awaitConnection(socket_fd);
        ssize_t rx_size = receiveString(connection_fd, rx_buffer, sizeof(rx_buffer));
        runXdgOpen(rx_buffer);
        
        close(connection_fd);
    }
}
