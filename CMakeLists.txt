# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("FileSearch")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED CMAKE_C_STANDARD)

if (WIN32)
	set(DIRENT_DIR "C:\\dirent-1.24")
	include_directories(${DIRENT_DIR}/include)
endif()

add_executable (${PROJECT_NAME} "src/main.c;src/ListLib.c")
