project (unit-test)

file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.hpp *.cpp)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src/")

set(SOURCES ${TEST_SOURCES})

add_executable(${PROJECT_NAME} ${TEST_SOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC altair)

add_custom_target(
# add_custom_command(
	TARGET ${PROJECT_NAME}
	COMMENT "Running tests"
	POST_BUILD
	COMMAND ${PROJECT_NAME}
)
