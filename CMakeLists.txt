cmake_minimum_required(VERSION 3.0)
project(lliurex-homework-harvester)

find_package(Gettext)

gettext_process_po_files(es ALL INSTALL_DESTINATION usr/share/locale/lliurex-homework-harvester PO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lliurex-po/lliurex-homework-harvester/es.po)
gettext_process_po_files(ca_ES@valencia ALL INSTALL_DESTINATION usr/share/locale/lliurex-homework-harvester PO_FILES ${CMAKE_CURRENT_SOURCE_DIR}/lliurex-po/lliurex-homework-harvester/ca_ES@valencia.po)

add_subdirectory(src)

install(DIRECTORY install-files/usr DESTINATION /)

