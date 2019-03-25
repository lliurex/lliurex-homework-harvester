cmake_minimum_required(VERSION 3.0)

find_package(Gettext)

project(lliurex-homework-harvester)
add_subdirectory(src)

install(DIRECTORY install-files/usr DESTINATION /)

gettext_process_po_files(es ${CMAKE_CURRENT_SOURCE_DIR}/lliurex-po/es.po)