# Networking library
add_library(networking STATIC
    networking.c    
)

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

# Sender executable
add_executable(docker-xdg-open-sender
    sender.c
)

target_link_libraries(docker-xdg-open-sender PRIVATE networking)

if(BUILD_STATIC_CLIENT)
    target_link_options(docker-xdg-open-sender PRIVATE -static-libgcc -static)
endif()

install(PROGRAMS $<TARGET_FILE:docker-xdg-open-sender>
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        RENAME xdg-open
        COMPONENT client)

