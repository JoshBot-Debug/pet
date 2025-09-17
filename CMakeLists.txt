cmake_minimum_required(VERSION 3.22)
project(ssrd)

# === Options ===
option(ENABLE_STL_DEBUG "Enable STL debug mode and DEBUG macro." OFF)
option(ENABLE_THREAD_SANITIZER "Enable ThreadSanitizer (disables AddressSanitizer)" OFF)

# === Compiler Flags ===
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === Source Files ===
file(GLOB_RECURSE SRC ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE GLAD_SRC ${CMAKE_SOURCE_DIR}/glad/*.c)
file(GLOB_RECURSE GIF_SRC ${CMAKE_SOURCE_DIR}/giflib/*.c)

add_executable(pet ${SRC} ${GLAD_SRC} ${GIF_SRC})

# === Libraries ===
find_package(PkgConfig REQUIRED)
pkg_check_modules(OPENGL REQUIRED opengl)
pkg_check_modules(GLFW3 REQUIRED glfw3)

message(STATUS "OPENGL_LIBRARIES = ${OPENGL_LIBRARIES}")
message(STATUS "GLFW3_LIBRARIES = ${GLFW3_LIBRARIES}")

# === Libraries ===
target_link_libraries(pet PRIVATE
  ${OPENGL_LIBRARIES}
  ${GLFW3_LIBRARIES}
)

# === Include Directories ===
target_include_directories(pet PRIVATE 
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/glad/include
  ${CMAKE_SOURCE_DIR}/giflib/include
  ${CMAKE_SOURCE_DIR}/cli11/include
)

# === Optional: ccache (speed up rebuilds) ===
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
  set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
  message(STATUS "ccache is ENABLED.")
endif()

# === Debug Configuration ===
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG -D_GLIBCXX_DEBUG)

  if(ENABLE_STL_DEBUG)
    target_compile_options(ssrd PRIVATE
      -Wall
      -Wextra
      -Wpedantic
      -Wshadow
      -Wconversion
      -Wsign-conversion
      -Wnull-dereference
      -Wdouble-promotion
      -Wformat=2
      -Wunused
    )

    message(STATUS "STL debug mode and DEBUG macro is ENABLED.")

    if(ENABLE_THREAD_SANITIZER)
      set(SANITIZE_FLAGS "-fsanitize=thread")
      message(STATUS "Using ThreadSanitizer (no ASan/UBSan).")
    else()
      set(SANITIZE_FLAGS "-fsanitize=undefined,address -fno-omit-frame-pointer")
      message(STATUS "Using AddressSanitizer + UndefinedBehaviorSanitizer.")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${SANITIZE_FLAGS}")
    set(CMAKE_LINKER_FLAGS_DEBUG "${SANITIZE_FLAGS}")
  else()
    message(STATUS "STL debug mode and DEBUG macro is DISABLED.")
  endif()
endif()

# === Release Configuration ===
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto=auto -funroll-loops -fomit-frame-pointer -g0 -s -ffast-math")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-flto=auto")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "-flto=auto")