# Set minimum version of CMake required to cnfigure the project
cmake_minimum_required(VERSION 3.21)

# Project name, version and languages used
project(docker-xdg-open
    VERSION 0.1.0
    LANGUAGES C
)

# Include GNUInstallDirs to use standard directory variables
include(GNUInstallDirs)

#---------------------------------- Options -----------------------------------
# Dynamic/Static executables switch
option(BUILD_STATIC_CLIENT "Build static client executable" ON)
option(BUILD_STATIC_SERVER "Build static server executable" OFF)

#------------------------------- Subdirectories -------------------------------
add_subdirectory(src)

