cmake_minimum_required(VERSION 3.18)

project(Nit)

add_executable(Nit main.c nit.c utils.c)


#add_custom_command(TARGET Nit POST_BUILD COMMAND echo "hola")
