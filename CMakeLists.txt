cmake_minimum_required (VERSION 2.6)
project (EasterEdge)

find_package( Boost 1.52 COMPONENTS system thread graph REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

include_directories("../crow/crow/include")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++1y -pedantic -Wextra")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(EasterEdge main.cpp)

target_link_libraries(EasterEdge
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
)

add_custom_target(run
	COMMAND EasterEdge
	DEPENDS EasterEdge
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# Add "tags" target and make my_project depending on this target.
#### get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
#### 
#### set_source_files_properties(tags PROPERTIES GENERATED true)
#### add_custom_target(tags
####     COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
#### 	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} dir ${dirs})
#### add_dependencies(EasterEdge tags)

# Add "tags" target and make my_project depending on this target.
### set_source_files_properties(tags PROPERTIES GENERATED true)
### add_custom_target(tags
###     COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q .
###     WORKING_DIRECTORY /usr/local/include/c++/4.8.5/ /usr/local/include/)
### add_dependencies(EasterEdge tags)
