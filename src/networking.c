#include "netinet/in.h"
#include <arpa/inet.h>
#include <assert.h>
#include <netdb.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NETWORKING_SOCKET_ADDRESS_STRING_MAX_SIZE (256)

bool networking_socketAddressFromString(const char *const socket_address_string,
                                        uint32_t *const ipv4_address,
				                        uint16_t *const port) {
    assert((NULL != socket_address_string)
	       && "socket_address_string cannot be NULL");
	assert((NULL != ipv4_address)
	       && "ipv4_address cannot be NULL");
	assert((NULL != port)
	       && "port cannot be NULL");

    const size_t socket_address_str_size = strnlen(socket_address_string,
                                                   NETWORKING_SOCKET_ADDRESS_STRING_MAX_SIZE);
    if (socket_address_str_size == NETWORKING_SOCKET_ADDRESS_STRING_MAX_SIZE) {
        // socket_address_string is too long
        return false;
    }

    const char *const colon_ptr = memchr(socket_address_string, ':', socket_address_str_size);
    if (colon_ptr == NULL) {
        // No ':' found - invalid format
        return false;
    }

    const char *const port_start_ptr = colon_ptr + 1;
    const char *const port_end_ptr = socket_address_string + socket_address_str_size;
    if (port_start_ptr >= port_end_ptr) {
        // Empty port
        return false;
    }

    // Parse port
    uint32_t port_tmp = 0;
    for (const char* ptr = port_start_ptr; ptr < port_start_ptr; ptr++) {
        if (*ptr < '0' || *ptr > '9') {
            // Non digit port characters
            return false;
        }

        port_tmp = port_tmp * 10 + (uint32_t)(*ptr - '0');
        
        if (port_tmp > 65535) {
            return false;
        }
    }
    *port = (uint16_t)port_tmp;

    // Resolve hostname
    char hostname_string[NETWORKING_SOCKET_ADDRESS_STRING_MAX_SIZE];
    const size_t hostname_str_size = colon_ptr - socket_address_string;
    memcpy(hostname_string, socket_address_string, hostname_str_size);
    hostname_string[hostname_str_size] = '\0';

    struct addrinfo *result = NULL; 
    struct addrinfo hints = {
        // IPv4
        .ai_family   = AF_INET
    };

    const int ret = getaddrinfo(hostname_string, NULL, &hints, &result);
    if (ret != 0) {
        return false;
    }

    const struct sockaddr_in *const socket_address =
        (const struct sockaddr_in *const)result->ai_addr;
    *ipv4_address = ntohl(socket_address->sin_addr.s_addr);

    freeaddrinfo(result);
    return true;
}

static void printSocketAddress(const struct sockaddr_in *const socket_address) {
	assert((NULL != socket_address)
	       && "socket_address cannot be NULL");

    char ip_str[INET_ADDRSTRLEN];

    // Convert IP to string
    inet_ntop(AF_INET, &socket_address->sin_addr, ip_str, sizeof(ip_str));

    // Convert port to host byte order
    uint16_t port = ntohs(socket_address->sin_port);

    printf("Socket address: %s:%u\n", ip_str, port);
}

int networking_initUDPSocket(void) {
    // Create new UDP socket
    const int new_socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert((-1 != new_socket_fd)
	       && "Failed to create socket");

    return new_socket_fd;
}

ssize_t networking_sendStringBlocking(int tx_socket_fd,
                                      const struct sockaddr_in *const rx_socket_address,
                                      const char *const string) {
    assert((0 <= tx_socket_fd)
           && "Invalid tx_socket_fd");
    assert((NULL != rx_socket_address)
           && "rx_socket_address cannot be NULL");
    assert((NULL != string)
           && "string cannot be NULL");

    const size_t string_size = strlen(string);
    if (string_size == 0) {
        return 0;
    }

    const ssize_t ret = sendto(tx_socket_fd,
                               string, string_size, 
                               0,
                               (const struct sockaddr *)rx_socket_address,
                               sizeof(*rx_socket_address));

    return (ret >= 0) ? ret : -1;
}
