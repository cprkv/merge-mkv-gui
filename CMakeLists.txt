cmake_minimum_required(VERSION 3.24)
project(merge-mkv-gui-project)

option(wxBUILD_SHARED _ OFF)
option(wxUSE_RC_MANIFEST _ ON)
add_subdirectory(deps/wxWidgets)

add_subdirectory(src)