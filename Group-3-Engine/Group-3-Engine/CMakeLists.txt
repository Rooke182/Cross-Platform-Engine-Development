# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("group_3_engine_android")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.

include_directories(
        "libraries/freetype/include"
        "libraries/freetype/include/freetype"
        "libraries/glm-1.0.1/"
        "libraries/glm-1.0.1/glm"
        "libraries/stb"
        "libraries/ImGUI"
        "libraries/nlohmann"
        "libraries/AL"
        "libraries"
)

add_library( AL STATIC IMPORTED )
set_target_properties( AL PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libraries/ALlib/android/${ANDROID_ABI}/libopenal.a )

add_library( freetype STATIC IMPORTED )
set_target_properties( freetype PROPERTIES IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libraries/freetype/android/${ANDROID_ABI}/libfreetype.a )

#add_library(freetype SHARED libraries/freetype/libft2.so)

file(GLOB SRC_FILES
        "*.cpp"
)
file(GLOB HEADER_FILES
        "*.h"
)

#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
        # List C/C++ source files with relative paths to this CMakeLists.txt.
        ${SRC_FILES}
        ${HEADER_FILES}
        TextRenderComponent.cpp
        PostProcess.cpp
        PostProcessRenderComponent.cpp
        InputObserverComponent.cpp
        libraries/ImGUI/imgui.cpp
        libraries/ImGUI/imgui_demo.cpp
        libraries/ImGUI/imgui_draw.cpp
        libraries/ImGUI/imgui_impl_android.cpp
        libraries/ImGUI/imgui_impl_opengl3.cpp
        libraries/ImGUI/imgui_stdlib.cpp
        libraries/ImGUI/imgui_tables.cpp
        libraries/ImGUI/imgui_widgets.cpp
)


# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        android
        z
        freetype
        AL
        GLESv3
        log
)