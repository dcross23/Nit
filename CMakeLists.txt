cmake_minimum_required(VERSION 3.18)

project(Nit)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(${PROJECT_NAME} main.c nit.c utils.c)
target_link_libraries(${PROJECT_NAME} OpenSSL::Crypto ZLIB::ZLIB)

#add_custom_command(TARGET Nit POST_BUILD COMMAND echo "hola")
