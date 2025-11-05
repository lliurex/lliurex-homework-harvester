
#find_package(ECM 1.0.0 REQUIRED NO_MODULE)
find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(KF6CoreAddons REQUIRED)
find_package(KF6KIO REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Quick)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fexceptions")
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -foperator-names")

find_package(EdupalsBase REQUIRED)

include(KDEInstallDirs)

set(TEACHER_PLUGIN "harvesterteacherplugin")
add_library(${TEACHER_PLUGIN} MODULE harvesterteacherplugin.cpp)

#kcoreaddons_add_plugin(harvesterteacherplugin SOURCES harvesterteacherplugin.cpp INSTALL_NAMESPACE ${KDE_INSTALL_PLUGIN_DIR})

target_link_libraries(${TEACHER_PLUGIN} PRIVATE KF6::CoreAddons KF6::KIOWidgets Edupals::Base)

set(TEACHER_KFILEINFORMATION_INSTALL_DIR "${KDE_INSTALL_PLUGINDIR}/kf6/kfileitemaction")

#kcoreaddons_add_plugin(harvesterstudentplugin SOURCES harvesterstudentplugin.cpp INSTALL_NAMESPACE ${KDE_INSTALL_PLUGIN_DIR})

set(STUDENT_PLUGIN "harvesterstudentplugin")
add_library(${STUDENT_PLUGIN} MODULE harvesterstudentplugin.cpp)

target_link_libraries(${STUDENT_PLUGIN} PRIVATE KF6::CoreAddons KF6::KIOWidgets Edupals::Base)

set(STUDENT_KFILEINFORMATION_INSTALL_DIR "${KDE_INSTALL_PLUGINDIR}/kf6/kfileitemaction")

add_executable(llx-homework-student studentapp.cpp ${CMAKE_SOURCE_DIR}/rsrc/resources.qrc)
target_link_libraries(llx-homework-student Qt6::Core Qt6::Widgets Qt6::Quick Edupals::Base)

add_executable(llx-homework-teacher teacherapp.cpp ${CMAKE_SOURCE_DIR}/rsrc/resources.qrc)
target_link_libraries(llx-homework-teacher Qt6::Core Qt6::Widgets Qt6::Quick Edupals::Base)

install(TARGETS llx-homework-student llx-homework-teacher RUNTIME DESTINATION bin)
install(TARGETS ${TEACHER_PLUGIN} DESTINATION ${TEACHER_KFILEINFORMATION_INSTALL_DIR})
install(TARGETS ${STUDENT_PLUGIN} DESTINATION ${STUDENT_KFILEINFORMATION_INSTALL_DIR})
add_custom_target(
    po
    COMMAND xgettext --keyword=T --language=C++ ${CMAKE_CURRENT_SOURCE_DIR}/harvesterstudentplugin.cpp ${CMAKE_CURRENT_SOURCE_DIR}/harvesterteacherplugin.cpp -o ${CMAKE_CURRENT_SOURCE_DIR}/messages.po
    COMMENT "extracting po..." 
)
