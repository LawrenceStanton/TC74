project(TC74_Lib)

enable_language(CXX)

set(LIBRARY ${PROJECT_NAME})

add_library(${LIBRARY} STATIC
	Src/TC74_I2C.cpp
)

target_include_directories(${LIBRARY} PUBLIC
	Inc
)

target_link_libraries(${LIBRARY} PUBLIC
	TC74
	STM32CubeF0_HAL
)