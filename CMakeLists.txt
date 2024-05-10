
cmake_minimum_required(VERSION 3.5)

project(qtpyandassets VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#add_definitions(-DQTPYANDASSETS_DEBUG)

file(GLOB PYTHON38_IMPORTLIB_PROJECT_SOURCES
    "src/*.h"
    "src/*.cpp"
)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

add_library(${PROJECT_NAME} SHARED ${PYTHON38_IMPORTLIB_PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND python3 "${CMAKE_SOURCE_DIR}/script/package.py" android)

add_definitions(-D_HAS_STD_BYTE=0)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS AndroidExtras)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS AndroidExtras)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::AndroidExtras)

# =============================================

set(python38_platform "android")
set(python38_include ${CMAKE_CURRENT_SOURCE_DIR}/../qtlib-python38/output/include/${python38_platform}/python3.8)
include_directories(${python38_include})
set(python38_include__internal ${CMAKE_CURRENT_SOURCE_DIR}/../qtlib-python38/output/include/${python38_platform}/python3.8/internal)
include_directories(${python38_include__internal})

set(python38_lib_dir ${CMAKE_CURRENT_SOURCE_DIR}/../qtlib-python38/output/lib/${python38_platform}/python3.8)
set(python38_lib1 ${python38_lib_dir}/libpython3.8.so)
set(python38_lib2 ${python38_lib_dir}/libpython3.so)
target_link_directories(${PROJECT_NAME} PRIVATE ${python38_lib_dir})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${python38_lib1}" "${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}/libpython3.8.so")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${python38_lib2}" "${CMAKE_CURRENT_BINARY_DIR}/android-build/libs/${ANDROID_ABI}/libpython3.so")
#ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND python3 "${CMAKE_SOURCE_DIR}/script/change_android_settings.py") # PRE_BUILD | PRE_LINK| POST_BUILD
target_link_libraries(${PROJECT_NAME} PRIVATE ${python38_lib1})
target_link_libraries(${PROJECT_NAME} PRIVATE ${python38_lib2})

#set(python38_liba ${python38_lib_dir}/libpython3.8.a)
#target_link_directories(${PROJECT_NAME} PRIVATE ${python38_lib_dir})
#target_link_libraries(${PROJECT_NAME} PRIVATE ${python38_liba})


# =============================================
