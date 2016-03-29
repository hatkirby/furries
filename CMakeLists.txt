cmake_minimum_required (VERSION 3.1)
project (furries)

find_package(PkgConfig)
pkg_check_modules(sqlite3 sqlite3 REQUIRED)

add_subdirectory(vendor/twitcurl/libtwitcurl)
add_subdirectory(vendor/verbly)
add_subdirectory(vendor/yaml-cpp EXCLUDE_FROM_ALL)

include_directories(vendor/twitcurl/libtwitcurl ${sqlite3_INCLUDE_DIR} vendor/verbly/lib vendor/yaml-cpp/include)
add_executable(furries furries.cpp)
set_property(TARGET furries PROPERTY CXX_STANDARD 11)
set_property(TARGET furries PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(furries verbly ${sqlite3_LIBRARIES} yaml-cpp twitcurl curl)
