cmake_minimum_required (VERSION 2.6)
project (furries)

add_subdirectory(vendor/twitcurl/libtwitcurl)

find_package(PkgConfig)
pkg_check_modules(YamlCpp yaml-cpp REQUIRED)
pkg_check_modules(sqlite3 sqlite3 REQUIRED)
find_package(libxml2 REQUIRED)

add_library(verbly verbly/data.cpp verbly/adjective.cpp verbly/noun.cpp verbly/verb.cpp verbly/adverb.cpp verbly/token.cpp verbly/word.cpp)
set_property(TARGET verbly PROPERTY CXX_STANDARD 11)
set_property(TARGET verbly PROPERTY CXX_STANDARD_REQUIRED ON)

include_directories(vendor/twitcurl/libtwitcurl ${LIBXML2_INCLUDE_DIR} ${sqlite3_INCLUDE_DIR})
add_executable(furries furries.cpp)
set_property(TARGET furries PROPERTY CXX_STANDARD 11)
set_property(TARGET furries PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(furries ${sqlite3_LIBRARIES} ${YamlCpp_LIBRARIES} twitcurl curl verbly)

add_executable(generator generator.cpp)
set_property(TARGET generator PROPERTY CXX_STANDARD 11)
set_property(TARGET generator PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(generator ${sqlite3_LIBRARIES} ${LIBXML2_LIBRARIES})
