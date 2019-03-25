
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTOMOC ON)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5KIO REQUIRED)

kcoreaddons_add_plugin(harvesterteacherplugin SOURCES harvesterteacherplugin.cpp INSTALL_NAMESPACE "kf5/kfileitemaction")
kcoreaddons_desktop_to_json(harvesterteacherplugin harvesterteacherplugin.desktop) # generate the json file

target_link_libraries(harvesterteacherplugin KF5::KIOWidgets)

message(WARNING  ${KDE_INSTALL_PLUGINDIR})
kcoreaddons_add_plugin(harvesterstudentplugin SOURCES harvesterstudentplugin.cpp INSTALL_NAMESPACE "kf5/kfileitemaction")
kcoreaddons_desktop_to_json(harvesterstudentplugin harvesterstudentplugin.desktop) # generate the json file

target_link_libraries(harvesterstudentplugin KF5::KIOWidgets)

add_custom_target(
    po
    COMMAND xgettext --keyword=T --language=C++ ${CMAKE_CURRENT_SOURCE_DIR}/harvesterstudentplugin.cpp ${CMAKE_CURRENT_SOURCE_DIR}/harvesterteacherplugin.cpp -o ${CMAKE_CURRENT_SOURCE_DIR}/messages.po
    COMMENT "extracting po..." 
)