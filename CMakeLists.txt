cmake_minimum_required (VERSION 3.8)

project ("refinement-schemes-editor")

# cinolib

set (CINOLIB_HEADER_ONLY ON)
set (CINOLIB_USES_OPENGL_GLFW_IMGUI ON)
set (cinolib_DIR "external/cinolib")
find_package (cinolib REQUIRED)

# cpputils

add_subdirectory ("external/cpputils")

# main

add_executable (main
    "src/main.cpp"
    "src/RSE/App.cpp"
    "src/RSE/Grid.cpp"
    "src/RSE/Refinement.cpp"
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