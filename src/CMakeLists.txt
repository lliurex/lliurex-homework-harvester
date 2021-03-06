
find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(KF5CoreAddons REQUIRED)
find_package(KF5KIO REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

find_package(EdupalsBase REQUIRED)
find_package(EdupalsN4D REQUIRED)
find_package(EdupalsN4DAgent REQUIRED)

include(KDEInstallDirs)

kcoreaddons_add_plugin(harvesterteacherplugin SOURCES harvesterteacherplugin.cpp INSTALL_NAMESPACE "kf5/kfileitemaction")
kcoreaddons_desktop_to_json(harvesterteacherplugin harvesterteacherplugin.desktop) # generate the json file

target_link_libraries(harvesterteacherplugin KF5::KIOWidgets)

kcoreaddons_add_plugin(harvesterstudentplugin SOURCES harvesterstudentplugin.cpp INSTALL_NAMESPACE "kf5/kfileitemaction")
kcoreaddons_desktop_to_json(harvesterstudentplugin harvesterstudentplugin.desktop) # generate the json file

target_link_libraries(harvesterstudentplugin KF5::KIOWidgets)

add_executable(llx-homework-student studentapp.cpp studentwindow.cpp studentframe1.ui login.ui ${CMAKE_SOURCE_DIR}/rsrc/resources.qrc)
target_link_libraries(llx-homework-student Qt5::Core Qt5::Widgets Edupals::Base Edupals::N4D -lpthread)

add_executable(llx-homework-teacher teacherapp.cpp teacherwindow.cpp)
target_link_libraries(llx-homework-teacher Qt5::Core Qt5::Widgets Edupals::Base Edupals::N4D Edupals::N4D::Agent -lpthread)


add_custom_target(
    po
    COMMAND xgettext --keyword=T --language=C++ ${CMAKE_CURRENT_SOURCE_DIR}/harvesterstudentplugin.cpp ${CMAKE_CURRENT_SOURCE_DIR}/harvesterteacherplugin.cpp -o ${CMAKE_CURRENT_SOURCE_DIR}/messages.po
    COMMENT "extracting po..." 
)
