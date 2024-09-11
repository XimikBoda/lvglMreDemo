cmake_minimum_required (VERSION 3.8)

set(PROJECT_NAME "lv_port_mre")

add_library(${PROJECT_NAME} STATIC 
    lv_port_disp_mre.c lv_port_disp_mre.h
    lv_port_indev_mre.c lv_port_indev_mre.h
    lv_port_fs_mre.c lv_port_fs_mre.h
)

target_include_directories(${PROJECT_NAME} PUBLIC .)

target_link_libraries(${PROJECT_NAME} mreapi lvgl)