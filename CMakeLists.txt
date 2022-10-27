cmake_minimum_required (VERSION 3.14)

include (FetchContent)

set (FETCHCONTENT_QUIET FALSE)

project ("refinement-schemes-editor")

# cinolib

set (CINOLIB_HEADER_ONLY OFF)
set (CINOLIB_USES_OPENGL_GLFW_IMGUI ON)

FetchContent_Declare (
    cinolib
    GIT_REPOSITORY "https://github.com/francescozoccheddu/cinolib.git"
    GIT_TAG 5d9a8cde4ab5c5341b7f031ac3126d97d8945d25
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable (cinolib)

# cpputils

FetchContent_Declare (
    cpputils
    GIT_REPOSITORY "https://github.com/francescozoccheddu/cpputils.git"
    GIT_TAG 36f6a36f1863dc8db82e905fca9134b862007041
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable (cpputils)

# main

add_executable (main
    "src/main.cpp"
    "src/RSE/App.cpp"
    "src/RSE/Grid.cpp"
    "src/RSE/CppExporter.cpp"
    "src/RSE/SourceControl.cpp"
    "src/RSE/AxesGuiItem.cpp"
    "src/RSE/AppSidebarItem.cpp"
    "src/RSE/ChildControl.cpp"
    "src/RSE/Style.cpp"
)

set_target_properties (main PROPERTIES
    CXX_STANDARD 20
    CXX_EXTENSIONS OFF
    CXX_STANDARD_REQUIRED ON
)

target_include_directories (main
    PRIVATE "include"
)

target_link_libraries (main 
    PUBLIC cinolib
    PUBLIC cpputils
)