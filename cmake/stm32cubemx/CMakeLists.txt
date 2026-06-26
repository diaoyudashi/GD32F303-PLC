cmake_minimum_required(VERSION 3.22)
enable_language(C ASM)

set(GD32_Defines USE_STDPERIPH_DRIVER GD32F30X_HD $<$<CONFIG:Debug>:DEBUG>)

set(GD32_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/GD/GD32F30x/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../app/io
    ${CMAKE_CURRENT_SOURCE_DIR}/../../app/protocol
)

set(GD32_App_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/gd32f30x_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../bsp/bsp_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../app/protocol/gxworks.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_gd32f30x_hd.S
)

set(GD32_Drv_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/GD/GD32F30x/Source/system_gd32f30x.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_rcu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_misc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_timer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_usart.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_adc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_can.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_spi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_dac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_pmu.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_bkp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_rtc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_crc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_wwdgt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_fwdgt.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/GD32F30x_StdPeriph/Source/gd32f30x_fmc.c
)

add_library(gd32f30x INTERFACE)
target_include_directories(gd32f30x INTERFACE ${GD32_Include_Dirs})
target_compile_definitions(gd32f30x INTERFACE ${GD32_Defines})

add_library(GD32_Drivers OBJECT)
target_sources(GD32_Drivers PRIVATE ${GD32_Drv_Src})
target_link_libraries(GD32_Drivers PUBLIC gd32f30x)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE ${GD32_App_Src})
target_link_libraries(${CMAKE_PROJECT_NAME} GD32_Drivers ${TOOLCHAIN_LINK_LIBRARIES})
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES ADDITIONAL_CLEAN_FILES ${CMAKE_PROJECT_NAME}.map)
