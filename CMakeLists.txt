cmake_minimum_required(VERSION 3.21)

project(qt_slots VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(slot_game_logic
    src/gamelogic.cpp
    src/gamelogic.h
    src/gameconfig.h
)
target_link_libraries(slot_game_logic PUBLIC Qt6::Core)
target_include_directories(slot_game_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

qt_add_executable(qt_slots
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/leverbutton.cpp
    src/leverbutton.h
    src/reelwidget.cpp
    src/reelwidget.h
)
target_link_libraries(qt_slots PRIVATE Qt6::Widgets slot_game_logic)

include(CTest)
if(BUILD_TESTING)
    find_package(Qt6 REQUIRED COMPONENTS Test)
    qt_add_executable(game_logic_tests tests/gamelogic_test.cpp)
    target_link_libraries(game_logic_tests PRIVATE Qt6::Test slot_game_logic)
    add_test(NAME game_logic_tests COMMAND game_logic_tests)
endif()

install(TARGETS qt_slots RUNTIME DESTINATION bin)
