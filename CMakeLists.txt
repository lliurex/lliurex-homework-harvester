cmake_minimum_required(VERSION 3.0)
project(lliurex-homework-harvester)


add_subdirectory(src)

install(DIRECTORY install-files/usr DESTINATION /)
install(FILES n4d/TeacherShare.json n4d/TeacherShareManager.json DESTINATION /etc/n4d/conf.d)
install(FILES n4d/TeacherShare.py n4d/TeacherShareManager.py DESTINATION share/n4d/python-plugins)
install(FILES rsrc/lliurex-homework-harvester.svg DESTINATION share/icons/hicolor/scalable/apps)

add_custom_target(
    poqml
    COMMAND xgettext -kde -ki18nd:2 ${CMAKE_CURRENT_SOURCE_DIR}/rsrc/*.qml  -o ${CMAKE_CURRENT_SOURCE_DIR}/messages.po
    COMMENT "extracting po..." 
)
