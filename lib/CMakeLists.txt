project (altair)

set (LIB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src/")

file(GLOB_RECURSE LIB_SRCS_CXX ${CMAKE_CURRENT_SOURCE_DIR} true *.h *.cpp)

add_library (${PROJECT_NAME} ${LIB_SRCS_CXX})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -masm=intel")

target_include_directories (${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)
