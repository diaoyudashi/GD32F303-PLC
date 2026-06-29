cmake_minimum_required(VERSION 3.22)
enable_language(C ASM)

set(DEFINES USE_STDPERIPH_DRIVER GD32F30X_HD $<$<CONFIG:Debug>:DEBUG>)

set(INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/GD/GD32F30x/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bsp
)

set(APP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/gd32f30x_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bsp/bsp_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_gd32f30x_hd.S
)

set(DRV_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/GD/GD32F30x/Source/system_gd32f30x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_rcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_misc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_exti.c
)

add_library(gd32f30x INTERFACE)
target_include_directories(gd32f30x INTERFACE ${INC_DIRS})
target_compile_definitions(gd32f30x INTERFACE ${DEFINES})
add_library(GD32_Drivers OBJECT)
target_sources(GD32_Drivers PRIVATE ${DRV_SRC})
target_link_libraries(GD32_Drivers PUBLIC gd32f30x)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${APP_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} GD32_Drivers ${TOOLCHAIN_LINK_LIBRARIES})
