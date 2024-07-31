###############################
# Raspberry Pi Pico W
###############################

pico_sdk_init()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(
        crystal-pi-pico
        Crystal.cpp
        HttpServer.h
        HttpServer.cpp
)

target_include_directories(crystal-pi-pico PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(
        crystal-pi-pico
        pico_stdlib
        pico_cyw43_arch_lwip_poll)

pico_add_extra_outputs(crystal-pi-pico)
