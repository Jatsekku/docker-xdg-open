#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "networking.h"

#define RX_SOCKET_ADDRESS_ENV "DOCKER_XDG_OPEN_SOCKET_ADDRESS"
#define DEFAULT_RX_SOCKET_ADDRESS "host.docker.internal:9000"

static bool establishRxSocketAddress(struct sockaddr_in *const rx_socket_address) {
	assert((NULL != rx_socket_address)
	       && "socket_address cannot be NULL");
    // Read socket address from env variable
    const char *rx_socket_address_str = getenv(RX_SOCKET_ADDRESS_ENV);

    // Fallback to default socket address
    if (rx_socket_address_str == NULL) {
        rx_socket_address_str = DEFAULT_RX_SOCKET_ADDRESS;
    }

    // Resolve string
    uint16_t port;
    uint32_t ipv4;
    const bool ret = networking_socketAddressFromString(rx_socket_address_str, &ipv4, &port);
    if (!ret) {
        return false;
    }

    printf("RX socket: %d:%d\n", ipv4, port);
    fflush(stdout);

    memset(rx_socket_address, 0, sizeof(*rx_socket_address));
    rx_socket_address->sin_family = AF_INET;
    rx_socket_address->sin_port = htons(port);
    rx_socket_address->sin_addr.s_addr = htonl(ipv4);

    return true;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    const char *const message = argv[1];

    printf("Message: %s\n", message);

    // Prepare RX socket address
    struct sockaddr_in rx_socket_address;
    establishRxSocketAddress(&rx_socket_address);

    // Create TX socket
    const int tx_socket_fd = networking_initUDPSocket();

    // Send message
    networking_sendStringBlocking(tx_socket_fd, &rx_socket_address, message);


    return 0;
}

