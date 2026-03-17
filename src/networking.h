#pragma once

#include <arpa/inet.h>
#include <stdint.h>

/* -------------------------------------------- API -------------------------------------------- */

int networking_initUDPSocket(void);
bool networking_socketAddressFromString(const char *const socket_address_string,
                                        uint32_t *const ipv4_address,
				                        uint16_t *const port);
ssize_t networking_sendStringBlocking(int tx_socket_fd,
                                      const struct sockaddr_in *const rx_socket_address,
                                      const char *const string);

