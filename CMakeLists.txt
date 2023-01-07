cmake_minimum_required(VERSION 3.21)
project(merge-mkv-gui-project)

option(wxBUILD_SHARED _ OFF)
add_subdirectory(deps/wxWidgets)

add_subdirectory(src)