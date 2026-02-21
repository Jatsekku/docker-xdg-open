# Server executable
add_executable(docker-xdg-open-server
    server.c
)

if(BUILD_STATIC_SERVER)
    target_link_options(docker-xdg-open-server PRIVATE -static-libgcc -static)
endif()

install(PROGRAMS $<TARGET_FILE:docker-xdg-open-server>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT server)

# Client executable
add_executable(docker-xdg-open-client
    client.c
)

if(BUILD_STATIC_CLIENT)
    target_link_options(docker-xdg-open-client PRIVATE -static-libgcc -static)
endif()

install(PROGRAMS $<TARGET_FILE:docker-xdg-open-client>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        RENAME xdg-open
        COMPONENT client)

