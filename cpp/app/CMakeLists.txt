set (APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${altair_SOURCE_DIR}/include "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(APP_SRCS
	${APP_SRC_DIR}/main.cpp
)

target_link_libraries(
	altair
)

add_executable (app ${APP_SRCS})