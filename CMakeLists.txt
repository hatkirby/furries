cmake_minimum_required (VERSION 2.6)
project (furries)

add_subdirectory(vendor/twitcurl/libtwitcurl)

find_package(PkgConfig)
pkg_check_modules(YamlCpp yaml-cpp REQUIRED)

include_directories(vendor/twitcurl/libtwitcurl)
add_executable(furries furries.cpp)
target_link_libraries(furries ${YamlCpp_LIBRARIES} mysqlclient twitcurl curl)
