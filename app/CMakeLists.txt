project (application)

set (APP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package (Threads)

include_directories(${altair_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(APP_SRCS
	${APP_SRC_DIR}/main.cpp
)

add_executable (${PROJECT_NAME} ${APP_SRCS})

target_link_libraries(${PROJECT_NAME}
	Threads::Threads
	altair
)
