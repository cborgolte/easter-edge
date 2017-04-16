cmake_minimum_required (VERSION 2.6)
project (EasterEdge)

find_package( Boost 1.52 COMPONENTS system thread graph REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

include_directories("../crow/crow/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -pedantic -Wextra")

#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -lboost_system" )

add_executable(EasterEdge main.cpp)

target_link_libraries(EasterEdge
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
)
