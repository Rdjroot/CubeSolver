cmake_minimum_required(VERSION 3.21)

project(CubeSolver VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(MINGW)
    add_compile_options(-Wa,-mbig-obj)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui imgcodecs videoio)
find_package(OpenGL REQUIRED)

qt_standard_project_setup()

set(CUBESOLVER_SOURCES
    CubeSolver/buildcube.cpp
    CubeSolver/camerathread.cpp
    CubeSolver/cubedetect.cpp
    CubeSolver/cubiecube.cpp
    CubeSolver/main.cpp
    CubeSolver/mainwindow.cpp
    CubeSolver/solver.cpp
    CubeSolver/welcome.cpp
)

set(CUBESOLVER_HEADERS
    CubeSolver/buildcube.h
    CubeSolver/camerathread.h
    CubeSolver/cubedetect.h
    CubeSolver/cubiecube.h
    CubeSolver/mainwindow.h
    CubeSolver/solver.h
    CubeSolver/welcome.h
)

set(CUBESOLVER_FORMS
    CubeSolver/buildcube.ui
    CubeSolver/cubedetect.ui
    CubeSolver/mainwindow.ui
    CubeSolver/welcome.ui
)

set(CUBESOLVER_RESOURCES
    CubeSolver/resource.qrc
)

qt_add_executable(CubeSolver
    MANUAL_FINALIZATION
    ${CUBESOLVER_SOURCES}
    ${CUBESOLVER_HEADERS}
    ${CUBESOLVER_FORMS}
    ${CUBESOLVER_RESOURCES}
)

target_include_directories(CubeSolver PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/CubeSolver
)

target_compile_definitions(CubeSolver PRIVATE
    CUBESOLVER_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(CubeSolver PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    ${OpenCV_LIBS}
    OpenGL::GL
)

set_target_properties(CubeSolver PROPERTIES
    WIN32_EXECUTABLE $<$<NOT:$<CONFIG:Debug>>:TRUE>
)

if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

    if(MINGW)
        set(WINDEPLOYQT_COMPILER_RUNTIME_ARG --no-compiler-runtime)
    else()
        set(WINDEPLOYQT_COMPILER_RUNTIME_ARG --compiler-runtime)
    endif()

    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET CubeSolver POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                ${WINDEPLOYQT_COMPILER_RUNTIME_ARG}
                --no-translations
                "$<TARGET_FILE:CubeSolver>"
            COMMAND_EXPAND_LISTS
        )
    endif()

    if(MINGW)
        get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        set(MINGW_RUNTIME_DLLS
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
        )

        add_custom_command(TARGET CubeSolver POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                ${MINGW_RUNTIME_DLLS}
                "$<TARGET_FILE_DIR:CubeSolver>"
            COMMAND_EXPAND_LISTS
        )
    endif()

    get_filename_component(OpenCV_INSTALLED_DIR "${OpenCV_DIR}/../.." ABSOLUTE)
    set(OpenCV_RUNTIME_DIR "${OpenCV_INSTALLED_DIR}/$<IF:$<CONFIG:Debug>,debug/bin,bin>")

    add_custom_command(TARGET CubeSolver POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${OpenCV_RUNTIME_DIR}"
            "$<TARGET_FILE_DIR:CubeSolver>"
        COMMAND_EXPAND_LISTS
    )
endif()

qt_finalize_executable(CubeSolver)
