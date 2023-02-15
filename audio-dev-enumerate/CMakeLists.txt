# CMakeList.txt : CMake project for audio-dev-enumerate, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (audio-dev-enumerate "audio-dev-enumerate.cpp" "audio-dev-enumerate.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET audio-dev-enumerate PROPERTY CXX_STANDARD 20)
endif()

target_include_directories( audio-dev-enumerate PRIVATE ".")

target_link_libraries(audio-dev-enumerate PRIVATE
    ole32.lib
)

# TODO: Add tests and install targets if needed.
