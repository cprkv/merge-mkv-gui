project(merge-mkv-gui)
add_executable(${PROJECT_NAME} WIN32)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.cpp *.hpp *.inl *.rc)
target_sources(${PROJECT_NAME} PRIVATE ${SOURCE_FILES})
message("target: ${PROJECT_NAME} source files: ${SOURCE_FILES}")

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp)
target_compile_definitions(${PROJECT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
target_link_libraries(${PROJECT_NAME} PRIVATE wxcore wxbase) # CONAN_PKG::boost
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
  LINKER_LANGUAGE CXX
)
target_link_options(${PROJECT_NAME} PRIVATE 
  /MANIFESTINPUT:${CMAKE_CURRENT_SOURCE_DIR}/../deps/wxWidgets/include/wx/msw/wx_dpi_aware_pmv2.manifest
)