cmake_minimum_required(VERSION 3.22)
enable_language(C ASM)

set(DEFINES USE_STDPERIPH_DRIVER STM32F10X_HD HSE_VALUE=8000000U $<$<CONFIG:Debug>:DEBUG>)

set(INC_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../app/protocol
    ${CMAKE_CURRENT_SOURCE_DIR}/../../app/io
)

set(APP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f10x_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../app/protocol/gxworks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_gd32f30x_hd.S
)

set(DRV_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/system_stm32f10x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/misc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_flash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F10x_StdPeriph/src/stm32f10x_exti.c
)

add_library(stm32f10x INTERFACE)
target_include_directories(stm32f10x INTERFACE ${INC_DIRS})
target_compile_definitions(stm32f10x INTERFACE ${DEFINES})
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE ${DRV_SRC})
target_link_libraries(STM32_Drivers PUBLIC stm32f10x)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${APP_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME} STM32_Drivers ${TOOLCHAIN_LINK_LIBRARIES})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)
