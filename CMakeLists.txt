cmake_minimum_required (VERSION 3.8)

project ("lvglMreDemo")

set(APP_NAME "lvglMreDemo")
set(DEVELOPER_NAME "XimikBoda")
set(RAM "1500" CACHE STRING "Ram requirement (in kb)")
set(IMSI "1234567890" CACHE STRING "Imsi of first SIM-card")
set(API "File SIM card ProMng" CACHE STRING "Api for app")
option(BACKGROUND "App can work background" OFF)

set(MRE_SDK "$ENV{MRE_SDK}" CACHE STRING "MRE SDK dir")
set(TinyMRESDK "$ENV{TinyMRESDK}" CACHE STRING "TinyMRESDK dir")

if(BACKGROUND)
    add_definitions(-DSUPPORT_BG)
endif()

add_subdirectory(resourses)
add_subdirectory(core)
add_subdirectory(mreapi)
add_subdirectory(vxp)
add_subdirectory(run)

add_definitions(-DLV_CONF_INCLUDE_SIMPLE)
include_directories(./include)
set(BUILD_SHARED_LIBS FALSE)
add_subdirectory(lvgl)

add_subdirectory(lv_port_mre)

add_subdirectory(main)
