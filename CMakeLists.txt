cmake_minimum_required(VERSION 3.24)
project(merge-mkv-gui-project)

option(wxBUILD_SHARED _ OFF)
option(wxUSE_RC_MANIFEST _ ON)
add_subdirectory(deps/wxWidgets)

add_library(subprocess INTERFACE)
target_include_directories(subprocess INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/deps/subprocess)

add_subdirectory(src)


function(vy_static_link target)
  if(MSVC)
    set_target_properties(${target} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    target_link_options(${target} PRIVATE /NODEFAULTLIB:msvcrtd /NODEFAULTLIB:msvcrt)
  endif()
endfunction()

vy_static_link(merge-mkv-gui)
vy_static_link(wxcore)
vy_static_link(wxbase)
vy_static_link(wxpng)
vy_static_link(wxzlib)
