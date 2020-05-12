cmake_minimum_required(VERSION 3.5.1)
project(opengl)

find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Freetype REQUIRED)

include_directories(
	include
	${OPENGL_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
)
file(GLOB SOURCES "source/*.c" "source/*.cpp" "res/*")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app ${SOURCES})

target_link_libraries(app glfw)
target_link_libraries(app ${OPENGL_gl_LIBRARY})
target_link_libraries(app ${FREETYPE_LIBRARIES})