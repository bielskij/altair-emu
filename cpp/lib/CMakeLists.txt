project(altair-lib)

set (LIB_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src/")

set(LIB_SRCS
	${LIB_SRC_DIR}/Core.cpp
	${LIB_SRC_DIR}/Core/Instruction.cpp
	${LIB_SRC_DIR}/Core/InstructionDecoder.cpp
)

add_library (${PROJECT_NAME} ${LIB_SRCS})

target_include_directories (${PROJECT_NAME}
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)