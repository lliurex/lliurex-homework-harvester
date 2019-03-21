
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTOMOC ON)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5KIO REQUIRED)

kcoreaddons_add_plugin(harvesterteacherplugin SOURCES harvesterteacherplugin.cpp INSTALL_NAMESPACE "kf5/kfileitemaction")
kcoreaddons_desktop_to_json(harvesterteacherplugin harvesterteacherplugin.desktop) # generate the json file

target_link_libraries(harvesterteacherplugin KF5::KIOWidgets)