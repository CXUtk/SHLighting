# CMakeList.txt: CMakeTest 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project (SHLighting VERSION 1.0)

file (GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

file (GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/src/*.h")

# 将源代码添加到此项目的可执行文件。
add_executable (SHLighting ${SRC_FILES} ${HEADER_FILES})

set_target_properties (SHLighting PROPERTIES
                      CXX_STANDARD 17
                      CXX_EXTENSIONS OFF
                      )
  
include_directories (${PROJECT_SOURCE_DIR}/src)
include_directories (${PROJECT_SOURCE_DIR}/deps)

include_directories (${PROJECT_SOURCE_DIR}/deps/GLFW/include)

if (UNIX)
    FIND_PACKAGE (glfw3)
    if (glfw3_FOUND)
        target_link_libraries(SHLighting PRIVATE glfw)
    else()
        add_subdirectory("${PROJECT_SOURCE_DIR}/deps/GLFW")
        target_link_libraries(SHLighting PRIVATE glfw)
    endif()
    target_link_libraries (SHLighting PRIVATE dl X11 pthread)
else()
    add_subdirectory("${PROJECT_SOURCE_DIR}/deps/GLFW")
    target_link_libraries(SHLighting PRIVATE glfw)
endif()

find_package(OpenGL REQUIRED)
target_link_libraries(SHLighting PRIVATE OpenGL::GL)


add_subdirectory ("deps/glad")
add_subdirectory ("deps/lodepng")
add_subdirectory ("deps/imgui")

target_link_libraries (SHLighting PRIVATE glad lodepng imgui)

add_custom_command(TARGET SHLighting PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${PROJECT_SOURCE_DIR}/Resources  ${PROJECT_BINARY_DIR}/Resources)



# TODO: 如有需要，请添加测试并安装目标。
