/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'nios2_gen2_0' in SOPC Builder design 'nios_system_Dram'
 * SOPC Builder design path: ../../nios_system_Dram.sopcinfo
 *
 * Generated: Sun May 03 14:31:52 SGT 2026
 */

/*
 * DO NOT MODIFY THIS FILE
 *
 * Changing this file will have subtle consequences
 * which will almost certainly lead to a nonfunctioning
 * system. If you do modify this file, be aware that your
 * changes will be overwritten and lost when this file
 * is generated again.
 *
 * DO NOT MODIFY THIS FILE
 */

/*
 * License Agreement
 *
 * Copyright (c) 2008
 * Altera Corporation, San Jose, California, USA.
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * This agreement shall be governed in all respects by the laws of the State
 * of California and by the laws of the United States of America.
 */

#ifndef __SYSTEM_H_
#define __SYSTEM_H_

/* Include definitions from linker script generator */
#include "linker.h"


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x08040820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 50000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000000
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x1c
#define ALT_CPU_DCACHE_BYPASS_MASK 0x80000000
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 2048
#define ALT_CPU_EXCEPTION_ADDR 0x04000020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 50000000
#define ALT_CPU_HARDWARE_DIVIDE_PRESENT 0
#define ALT_CPU_HARDWARE_MULTIPLY_PRESENT 1
#define ALT_CPU_HARDWARE_MULX_PRESENT 0
#define ALT_CPU_HAS_DEBUG_CORE 1
#define ALT_CPU_HAS_DEBUG_STUB
#define ALT_CPU_HAS_EXTRA_EXCEPTION_INFO
#define ALT_CPU_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define ALT_CPU_HAS_JMPI_INSTRUCTION
#define ALT_CPU_ICACHE_LINE_SIZE 32
#define ALT_CPU_ICACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_ICACHE_SIZE 4096
#define ALT_CPU_INITDA_SUPPORTED
#define ALT_CPU_INST_ADDR_WIDTH 0x1c
#define ALT_CPU_NAME "nios2_gen2_0"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x04000000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x08040820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 50000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000000
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x1c
#define NIOS2_DCACHE_BYPASS_MASK 0x80000000
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_EXCEPTION_ADDR 0x04000020
#define NIOS2_FLASH_ACCELERATOR_LINES 0
#define NIOS2_FLASH_ACCELERATOR_LINE_SIZE 0
#define NIOS2_FLUSHDA_SUPPORTED
#define NIOS2_HARDWARE_DIVIDE_PRESENT 0
#define NIOS2_HARDWARE_MULTIPLY_PRESENT 1
#define NIOS2_HARDWARE_MULX_PRESENT 0
#define NIOS2_HAS_DEBUG_CORE 1
#define NIOS2_HAS_DEBUG_STUB
#define NIOS2_HAS_EXTRA_EXCEPTION_INFO
#define NIOS2_HAS_ILLEGAL_INSTRUCTION_EXCEPTION
#define NIOS2_HAS_JMPI_INSTRUCTION
#define NIOS2_ICACHE_LINE_SIZE 32
#define NIOS2_ICACHE_LINE_SIZE_LOG2 5
#define NIOS2_ICACHE_SIZE 4096
#define NIOS2_INITDA_SUPPORTED
#define NIOS2_INST_ADDR_WIDTH 0x1c
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x04000000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_SPI
#define __ALTERA_AVALON_TIMER
#define __ALTERA_AVALON_UART
#define __ALTERA_NIOS2_GEN2
#define __ALTERA_UP_AVALON_ACCELEROMETER_SPI


/*
 * GPIO_2 configuration
 *
 */

#define ALT_MODULE_CLASS_GPIO_2 altera_avalon_pio
#define GPIO_2_BASE 0x8041160
#define GPIO_2_BIT_CLEARING_EDGE_REGISTER 0
#define GPIO_2_BIT_MODIFYING_OUTPUT_REGISTER 0
#define GPIO_2_CAPTURE 0
#define GPIO_2_DATA_WIDTH 1
#define GPIO_2_DO_TEST_BENCH_WIRING 0
#define GPIO_2_DRIVEN_SIM_VALUE 0
#define GPIO_2_EDGE_TYPE "NONE"
#define GPIO_2_FREQ 50000000
#define GPIO_2_HAS_IN 0
#define GPIO_2_HAS_OUT 1
#define GPIO_2_HAS_TRI 0
#define GPIO_2_IRQ -1
#define GPIO_2_IRQ_INTERRUPT_CONTROLLER_ID -1
#define GPIO_2_IRQ_TYPE "NONE"
#define GPIO_2_NAME "/dev/GPIO_2"
#define GPIO_2_RESET_VALUE 0
#define GPIO_2_SPAN 16
#define GPIO_2_TYPE "altera_avalon_pio"


/*
 * LEDR_8 configuration
 *
 */

#define ALT_MODULE_CLASS_LEDR_8 altera_avalon_pio
#define LEDR_8_BASE 0x8041170
#define LEDR_8_BIT_CLEARING_EDGE_REGISTER 0
#define LEDR_8_BIT_MODIFYING_OUTPUT_REGISTER 0
#define LEDR_8_CAPTURE 0
#define LEDR_8_DATA_WIDTH 8
#define LEDR_8_DO_TEST_BENCH_WIRING 0
#define LEDR_8_DRIVEN_SIM_VALUE 0
#define LEDR_8_EDGE_TYPE "NONE"
#define LEDR_8_FREQ 50000000
#define LEDR_8_HAS_IN 0
#define LEDR_8_HAS_OUT 1
#define LEDR_8_HAS_TRI 0
#define LEDR_8_IRQ -1
#define LEDR_8_IRQ_INTERRUPT_CONTROLLER_ID -1
#define LEDR_8_IRQ_TYPE "NONE"
#define LEDR_8_NAME "/dev/LEDR_8"
#define LEDR_8_RESET_VALUE 0
#define LEDR_8_SPAN 16
#define LEDR_8_TYPE "altera_avalon_pio"


/*
 * System configuration
 *
 */

#define ALT_DEVICE_FAMILY "MAX 10"
#define ALT_IRQ_BASE NULL
#define ALT_LEGACY_INTERRUPT_API_PRESENT
#define ALT_LOG_PORT "/dev/null"
#define ALT_LOG_PORT_BASE 0x0
#define ALT_LOG_PORT_DEV null
#define ALT_LOG_PORT_TYPE ""
#define ALT_NUM_EXTERNAL_INTERRUPT_CONTROLLERS 0
#define ALT_NUM_INTERNAL_INTERRUPT_CONTROLLERS 1
#define ALT_NUM_INTERRUPT_CONTROLLERS 1
#define ALT_STDERR "/dev/jtag_uart_0"
#define ALT_STDERR_BASE 0x8041188
#define ALT_STDERR_DEV jtag_uart_0
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart_0"
#define ALT_STDIN_BASE 0x8041188
#define ALT_STDIN_DEV jtag_uart_0
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart_0"
#define ALT_STDOUT_BASE 0x8041188
#define ALT_STDOUT_DEV jtag_uart_0
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "nios_system_Dram"


/*
 * accelerometer_spi_0 configuration
 *
 */

#define ACCELEROMETER_SPI_0_BASE 0x8041190
#define ACCELEROMETER_SPI_0_IRQ 1
#define ACCELEROMETER_SPI_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define ACCELEROMETER_SPI_0_NAME "/dev/accelerometer_spi_0"
#define ACCELEROMETER_SPI_0_SPAN 2
#define ACCELEROMETER_SPI_0_TYPE "altera_up_avalon_accelerometer_spi"
#define ALT_MODULE_CLASS_accelerometer_spi_0 altera_up_avalon_accelerometer_spi


/*
 * camera_ready configuration
 *
 */

#define ALT_MODULE_CLASS_camera_ready altera_avalon_pio
#define CAMERA_READY_BASE 0x8041080
#define CAMERA_READY_BIT_CLEARING_EDGE_REGISTER 0
#define CAMERA_READY_BIT_MODIFYING_OUTPUT_REGISTER 0
#define CAMERA_READY_CAPTURE 0
#define CAMERA_READY_DATA_WIDTH 1
#define CAMERA_READY_DO_TEST_BENCH_WIRING 0
#define CAMERA_READY_DRIVEN_SIM_VALUE 0
#define CAMERA_READY_EDGE_TYPE "NONE"
#define CAMERA_READY_FREQ 50000000
#define CAMERA_READY_HAS_IN 1
#define CAMERA_READY_HAS_OUT 0
#define CAMERA_READY_HAS_TRI 0
#define CAMERA_READY_IRQ -1
#define CAMERA_READY_IRQ_INTERRUPT_CONTROLLER_ID -1
#define CAMERA_READY_IRQ_TYPE "NONE"
#define CAMERA_READY_NAME "/dev/camera_ready"
#define CAMERA_READY_RESET_VALUE 0
#define CAMERA_READY_SPAN 16
#define CAMERA_READY_TYPE "altera_avalon_pio"


/*
 * hal configuration
 *
 */

#define ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#define ALT_MAX_FD 4
#define ALT_SYS_CLK none
#define ALT_TIMESTAMP_CLK none


/*
 * img_address configuration
 *
 */

#define ALT_MODULE_CLASS_img_address altera_avalon_pio
#define IMG_ADDRESS_BASE 0x8041070
#define IMG_ADDRESS_BIT_CLEARING_EDGE_REGISTER 0
#define IMG_ADDRESS_BIT_MODIFYING_OUTPUT_REGISTER 0
#define IMG_ADDRESS_CAPTURE 0
#define IMG_ADDRESS_DATA_WIDTH 17
#define IMG_ADDRESS_DO_TEST_BENCH_WIRING 0
#define IMG_ADDRESS_DRIVEN_SIM_VALUE 0
#define IMG_ADDRESS_EDGE_TYPE "NONE"
#define IMG_ADDRESS_FREQ 50000000
#define IMG_ADDRESS_HAS_IN 0
#define IMG_ADDRESS_HAS_OUT 1
#define IMG_ADDRESS_HAS_TRI 0
#define IMG_ADDRESS_IRQ -1
#define IMG_ADDRESS_IRQ_INTERRUPT_CONTROLLER_ID -1
#define IMG_ADDRESS_IRQ_TYPE "NONE"
#define IMG_ADDRESS_NAME "/dev/img_address"
#define IMG_ADDRESS_RESET_VALUE 0
#define IMG_ADDRESS_SPAN 16
#define IMG_ADDRESS_TYPE "altera_avalon_pio"


/*
 * jtag_uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart_0 altera_avalon_jtag_uart
#define JTAG_UART_0_BASE 0x8041188
#define JTAG_UART_0_IRQ 0
#define JTAG_UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_0_NAME "/dev/jtag_uart_0"
#define JTAG_UART_0_READ_DEPTH 64
#define JTAG_UART_0_READ_THRESHOLD 8
#define JTAG_UART_0_SPAN 8
#define JTAG_UART_0_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_0_WRITE_DEPTH 64
#define JTAG_UART_0_WRITE_THRESHOLD 8


/*
 * new_sdram_controller_0 configuration
 *
 */

#define ALT_MODULE_CLASS_new_sdram_controller_0 altera_avalon_new_sdram_controller
#define NEW_SDRAM_CONTROLLER_0_BASE 0x4000000
#define NEW_SDRAM_CONTROLLER_0_CAS_LATENCY 3
#define NEW_SDRAM_CONTROLLER_0_CONTENTS_INFO
#define NEW_SDRAM_CONTROLLER_0_INIT_NOP_DELAY 0.0
#define NEW_SDRAM_CONTROLLER_0_INIT_REFRESH_COMMANDS 2
#define NEW_SDRAM_CONTROLLER_0_IRQ -1
#define NEW_SDRAM_CONTROLLER_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define NEW_SDRAM_CONTROLLER_0_IS_INITIALIZED 1
#define NEW_SDRAM_CONTROLLER_0_NAME "/dev/new_sdram_controller_0"
#define NEW_SDRAM_CONTROLLER_0_POWERUP_DELAY 100.0
#define NEW_SDRAM_CONTROLLER_0_REFRESH_PERIOD 15.625
#define NEW_SDRAM_CONTROLLER_0_REGISTER_DATA_IN 1
#define NEW_SDRAM_CONTROLLER_0_SDRAM_ADDR_WIDTH 0x19
#define NEW_SDRAM_CONTROLLER_0_SDRAM_BANK_WIDTH 2
#define NEW_SDRAM_CONTROLLER_0_SDRAM_COL_WIDTH 10
#define NEW_SDRAM_CONTROLLER_0_SDRAM_DATA_WIDTH 16
#define NEW_SDRAM_CONTROLLER_0_SDRAM_NUM_BANKS 4
#define NEW_SDRAM_CONTROLLER_0_SDRAM_NUM_CHIPSELECTS 1
#define NEW_SDRAM_CONTROLLER_0_SDRAM_ROW_WIDTH 13
#define NEW_SDRAM_CONTROLLER_0_SHARED_DATA 0
#define NEW_SDRAM_CONTROLLER_0_SIM_MODEL_BASE 0
#define NEW_SDRAM_CONTROLLER_0_SPAN 67108864
#define NEW_SDRAM_CONTROLLER_0_STARVATION_INDICATOR 0
#define NEW_SDRAM_CONTROLLER_0_TRISTATE_BRIDGE_SLAVE ""
#define NEW_SDRAM_CONTROLLER_0_TYPE "altera_avalon_new_sdram_controller"
#define NEW_SDRAM_CONTROLLER_0_T_AC 5.5
#define NEW_SDRAM_CONTROLLER_0_T_MRD 3
#define NEW_SDRAM_CONTROLLER_0_T_RCD 20.0
#define NEW_SDRAM_CONTROLLER_0_T_RFC 70.0
#define NEW_SDRAM_CONTROLLER_0_T_RP 20.0
#define NEW_SDRAM_CONTROLLER_0_T_WR 14.0


/*
 * onchip_memory2_0 configuration
 *
 */

#define ALT_MODULE_CLASS_onchip_memory2_0 altera_avalon_onchip_memory2
#define ONCHIP_MEMORY2_0_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define ONCHIP_MEMORY2_0_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define ONCHIP_MEMORY2_0_BASE 0x8020000
#define ONCHIP_MEMORY2_0_CONTENTS_INFO ""
#define ONCHIP_MEMORY2_0_DUAL_PORT 0
#define ONCHIP_MEMORY2_0_GUI_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEMORY2_0_INIT_CONTENTS_FILE "nios_system_Dram_onchip_memory2_0"
#define ONCHIP_MEMORY2_0_INIT_MEM_CONTENT 0
#define ONCHIP_MEMORY2_0_INSTANCE_ID "NONE"
#define ONCHIP_MEMORY2_0_IRQ -1
#define ONCHIP_MEMORY2_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define ONCHIP_MEMORY2_0_NAME "/dev/onchip_memory2_0"
#define ONCHIP_MEMORY2_0_NON_DEFAULT_INIT_FILE_ENABLED 0
#define ONCHIP_MEMORY2_0_RAM_BLOCK_TYPE "AUTO"
#define ONCHIP_MEMORY2_0_READ_DURING_WRITE_MODE "DONT_CARE"
#define ONCHIP_MEMORY2_0_SINGLE_CLOCK_OP 0
#define ONCHIP_MEMORY2_0_SIZE_MULTIPLE 1
#define ONCHIP_MEMORY2_0_SIZE_VALUE 131072
#define ONCHIP_MEMORY2_0_SPAN 131072
#define ONCHIP_MEMORY2_0_TYPE "altera_avalon_onchip_memory2"
#define ONCHIP_MEMORY2_0_WRITABLE 1


/*
 * pio_0 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_0 altera_avalon_pio
#define PIO_0_BASE 0x8041150
#define PIO_0_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_0_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_0_CAPTURE 1
#define PIO_0_DATA_WIDTH 1
#define PIO_0_DO_TEST_BENCH_WIRING 0
#define PIO_0_DRIVEN_SIM_VALUE 0
#define PIO_0_EDGE_TYPE "RISING"
#define PIO_0_FREQ 50000000
#define PIO_0_HAS_IN 1
#define PIO_0_HAS_OUT 0
#define PIO_0_HAS_TRI 0
#define PIO_0_IRQ 5
#define PIO_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define PIO_0_IRQ_TYPE "EDGE"
#define PIO_0_NAME "/dev/pio_0"
#define PIO_0_RESET_VALUE 0
#define PIO_0_SPAN 16
#define PIO_0_TYPE "altera_avalon_pio"


/*
 * pio_1 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_1 altera_avalon_pio
#define PIO_1_BASE 0x8041140
#define PIO_1_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_1_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_1_CAPTURE 0
#define PIO_1_DATA_WIDTH 1
#define PIO_1_DO_TEST_BENCH_WIRING 0
#define PIO_1_DRIVEN_SIM_VALUE 0
#define PIO_1_EDGE_TYPE "NONE"
#define PIO_1_FREQ 50000000
#define PIO_1_HAS_IN 1
#define PIO_1_HAS_OUT 0
#define PIO_1_HAS_TRI 0
#define PIO_1_IRQ -1
#define PIO_1_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_1_IRQ_TYPE "NONE"
#define PIO_1_NAME "/dev/pio_1"
#define PIO_1_RESET_VALUE 0
#define PIO_1_SPAN 16
#define PIO_1_TYPE "altera_avalon_pio"


/*
 * pio_12 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_12 altera_avalon_pio
#define PIO_12_BASE 0x80410e0
#define PIO_12_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_12_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_12_CAPTURE 0
#define PIO_12_DATA_WIDTH 1
#define PIO_12_DO_TEST_BENCH_WIRING 0
#define PIO_12_DRIVEN_SIM_VALUE 0
#define PIO_12_EDGE_TYPE "NONE"
#define PIO_12_FREQ 50000000
#define PIO_12_HAS_IN 0
#define PIO_12_HAS_OUT 1
#define PIO_12_HAS_TRI 0
#define PIO_12_IRQ -1
#define PIO_12_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_12_IRQ_TYPE "NONE"
#define PIO_12_NAME "/dev/pio_12"
#define PIO_12_RESET_VALUE 0
#define PIO_12_SPAN 16
#define PIO_12_TYPE "altera_avalon_pio"


/*
 * pio_13 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_13 altera_avalon_pio
#define PIO_13_BASE 0x80410d0
#define PIO_13_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_13_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_13_CAPTURE 0
#define PIO_13_DATA_WIDTH 1
#define PIO_13_DO_TEST_BENCH_WIRING 0
#define PIO_13_DRIVEN_SIM_VALUE 0
#define PIO_13_EDGE_TYPE "NONE"
#define PIO_13_FREQ 50000000
#define PIO_13_HAS_IN 0
#define PIO_13_HAS_OUT 1
#define PIO_13_HAS_TRI 0
#define PIO_13_IRQ -1
#define PIO_13_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_13_IRQ_TYPE "NONE"
#define PIO_13_NAME "/dev/pio_13"
#define PIO_13_RESET_VALUE 0
#define PIO_13_SPAN 16
#define PIO_13_TYPE "altera_avalon_pio"


/*
 * pio_14 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_14 altera_avalon_pio
#define PIO_14_BASE 0x80410c0
#define PIO_14_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_14_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_14_CAPTURE 0
#define PIO_14_DATA_WIDTH 1
#define PIO_14_DO_TEST_BENCH_WIRING 0
#define PIO_14_DRIVEN_SIM_VALUE 0
#define PIO_14_EDGE_TYPE "NONE"
#define PIO_14_FREQ 50000000
#define PIO_14_HAS_IN 0
#define PIO_14_HAS_OUT 1
#define PIO_14_HAS_TRI 0
#define PIO_14_IRQ -1
#define PIO_14_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_14_IRQ_TYPE "NONE"
#define PIO_14_NAME "/dev/pio_14"
#define PIO_14_RESET_VALUE 0
#define PIO_14_SPAN 16
#define PIO_14_TYPE "altera_avalon_pio"


/*
 * pio_15 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_15 altera_avalon_pio
#define PIO_15_BASE 0x80410b0
#define PIO_15_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_15_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_15_CAPTURE 0
#define PIO_15_DATA_WIDTH 1
#define PIO_15_DO_TEST_BENCH_WIRING 0
#define PIO_15_DRIVEN_SIM_VALUE 0
#define PIO_15_EDGE_TYPE "NONE"
#define PIO_15_FREQ 50000000
#define PIO_15_HAS_IN 0
#define PIO_15_HAS_OUT 1
#define PIO_15_HAS_TRI 0
#define PIO_15_IRQ -1
#define PIO_15_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_15_IRQ_TYPE "NONE"
#define PIO_15_NAME "/dev/pio_15"
#define PIO_15_RESET_VALUE 0
#define PIO_15_SPAN 16
#define PIO_15_TYPE "altera_avalon_pio"


/*
 * pio_2 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_2 altera_avalon_pio
#define PIO_2_BASE 0x8041130
#define PIO_2_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_2_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_2_CAPTURE 0
#define PIO_2_DATA_WIDTH 1
#define PIO_2_DO_TEST_BENCH_WIRING 0
#define PIO_2_DRIVEN_SIM_VALUE 0
#define PIO_2_EDGE_TYPE "NONE"
#define PIO_2_FREQ 50000000
#define PIO_2_HAS_IN 0
#define PIO_2_HAS_OUT 1
#define PIO_2_HAS_TRI 0
#define PIO_2_IRQ -1
#define PIO_2_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_2_IRQ_TYPE "NONE"
#define PIO_2_NAME "/dev/pio_2"
#define PIO_2_RESET_VALUE 0
#define PIO_2_SPAN 16
#define PIO_2_TYPE "altera_avalon_pio"


/*
 * pio_3 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_3 altera_avalon_pio
#define PIO_3_BASE 0x8041120
#define PIO_3_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_3_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_3_CAPTURE 0
#define PIO_3_DATA_WIDTH 24
#define PIO_3_DO_TEST_BENCH_WIRING 0
#define PIO_3_DRIVEN_SIM_VALUE 0
#define PIO_3_EDGE_TYPE "NONE"
#define PIO_3_FREQ 50000000
#define PIO_3_HAS_IN 0
#define PIO_3_HAS_OUT 1
#define PIO_3_HAS_TRI 0
#define PIO_3_IRQ -1
#define PIO_3_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_3_IRQ_TYPE "NONE"
#define PIO_3_NAME "/dev/pio_3"
#define PIO_3_RESET_VALUE 0
#define PIO_3_SPAN 16
#define PIO_3_TYPE "altera_avalon_pio"


/*
 * pio_4 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_4 altera_avalon_pio
#define PIO_4_BASE 0x8041110
#define PIO_4_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_4_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_4_CAPTURE 0
#define PIO_4_DATA_WIDTH 1
#define PIO_4_DO_TEST_BENCH_WIRING 0
#define PIO_4_DRIVEN_SIM_VALUE 0
#define PIO_4_EDGE_TYPE "NONE"
#define PIO_4_FREQ 50000000
#define PIO_4_HAS_IN 0
#define PIO_4_HAS_OUT 1
#define PIO_4_HAS_TRI 0
#define PIO_4_IRQ -1
#define PIO_4_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_4_IRQ_TYPE "NONE"
#define PIO_4_NAME "/dev/pio_4"
#define PIO_4_RESET_VALUE 0
#define PIO_4_SPAN 16
#define PIO_4_TYPE "altera_avalon_pio"


/*
 * pio_5 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_5 altera_avalon_pio
#define PIO_5_BASE 0x8041100
#define PIO_5_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_5_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_5_CAPTURE 0
#define PIO_5_DATA_WIDTH 1
#define PIO_5_DO_TEST_BENCH_WIRING 0
#define PIO_5_DRIVEN_SIM_VALUE 0
#define PIO_5_EDGE_TYPE "NONE"
#define PIO_5_FREQ 50000000
#define PIO_5_HAS_IN 1
#define PIO_5_HAS_OUT 0
#define PIO_5_HAS_TRI 0
#define PIO_5_IRQ -1
#define PIO_5_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_5_IRQ_TYPE "NONE"
#define PIO_5_NAME "/dev/pio_5"
#define PIO_5_RESET_VALUE 0
#define PIO_5_SPAN 16
#define PIO_5_TYPE "altera_avalon_pio"


/*
 * pio_6 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_6 altera_avalon_pio
#define PIO_6_BASE 0x80410a0
#define PIO_6_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_6_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_6_CAPTURE 0
#define PIO_6_DATA_WIDTH 24
#define PIO_6_DO_TEST_BENCH_WIRING 0
#define PIO_6_DRIVEN_SIM_VALUE 0
#define PIO_6_EDGE_TYPE "NONE"
#define PIO_6_FREQ 50000000
#define PIO_6_HAS_IN 0
#define PIO_6_HAS_OUT 1
#define PIO_6_HAS_TRI 0
#define PIO_6_IRQ -1
#define PIO_6_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_6_IRQ_TYPE "NONE"
#define PIO_6_NAME "/dev/pio_6"
#define PIO_6_RESET_VALUE 0
#define PIO_6_SPAN 16
#define PIO_6_TYPE "altera_avalon_pio"


/*
 * pio_8 configuration
 *
 */

#define ALT_MODULE_CLASS_pio_8 altera_avalon_pio
#define PIO_8_BASE 0x80410f0
#define PIO_8_BIT_CLEARING_EDGE_REGISTER 0
#define PIO_8_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIO_8_CAPTURE 0
#define PIO_8_DATA_WIDTH 9
#define PIO_8_DO_TEST_BENCH_WIRING 0
#define PIO_8_DRIVEN_SIM_VALUE 0
#define PIO_8_EDGE_TYPE "NONE"
#define PIO_8_FREQ 50000000
#define PIO_8_HAS_IN 1
#define PIO_8_HAS_OUT 0
#define PIO_8_HAS_TRI 0
#define PIO_8_IRQ -1
#define PIO_8_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIO_8_IRQ_TYPE "NONE"
#define PIO_8_NAME "/dev/pio_8"
#define PIO_8_RESET_VALUE 0
#define PIO_8_SPAN 16
#define PIO_8_TYPE "altera_avalon_pio"


/*
 * pixel_data configuration
 *
 */

#define ALT_MODULE_CLASS_pixel_data altera_avalon_pio
#define PIXEL_DATA_BASE 0x8041090
#define PIXEL_DATA_BIT_CLEARING_EDGE_REGISTER 0
#define PIXEL_DATA_BIT_MODIFYING_OUTPUT_REGISTER 0
#define PIXEL_DATA_CAPTURE 0
#define PIXEL_DATA_DATA_WIDTH 4
#define PIXEL_DATA_DO_TEST_BENCH_WIRING 0
#define PIXEL_DATA_DRIVEN_SIM_VALUE 0
#define PIXEL_DATA_EDGE_TYPE "NONE"
#define PIXEL_DATA_FREQ 50000000
#define PIXEL_DATA_HAS_IN 0
#define PIXEL_DATA_HAS_OUT 1
#define PIXEL_DATA_HAS_TRI 0
#define PIXEL_DATA_IRQ -1
#define PIXEL_DATA_IRQ_INTERRUPT_CONTROLLER_ID -1
#define PIXEL_DATA_IRQ_TYPE "NONE"
#define PIXEL_DATA_NAME "/dev/pixel_data"
#define PIXEL_DATA_RESET_VALUE 0
#define PIXEL_DATA_SPAN 16
#define PIXEL_DATA_TYPE "altera_avalon_pio"


/*
 * spi_0 configuration
 *
 */

#define ALT_MODULE_CLASS_spi_0 altera_avalon_spi
#define SPI_0_BASE 0x8041000
#define SPI_0_CLOCKMULT 1
#define SPI_0_CLOCKPHASE 0
#define SPI_0_CLOCKPOLARITY 0
#define SPI_0_CLOCKUNITS "Hz"
#define SPI_0_DATABITS 8
#define SPI_0_DATAWIDTH 16
#define SPI_0_DELAYMULT "1.0E-9"
#define SPI_0_DELAYUNITS "ns"
#define SPI_0_EXTRADELAY 0
#define SPI_0_INSERT_SYNC 0
#define SPI_0_IRQ 2
#define SPI_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define SPI_0_ISMASTER 1
#define SPI_0_LSBFIRST 0
#define SPI_0_NAME "/dev/spi_0"
#define SPI_0_NUMSLAVES 1
#define SPI_0_PREFIX "spi_"
#define SPI_0_SPAN 32
#define SPI_0_SYNC_REG_DEPTH 2
#define SPI_0_TARGETCLOCK 128000u
#define SPI_0_TARGETSSDELAY "0.0"
#define SPI_0_TYPE "altera_avalon_spi"


/*
 * timer_0 configuration
 *
 */

#define ALT_MODULE_CLASS_timer_0 altera_avalon_timer
#define TIMER_0_ALWAYS_RUN 0
#define TIMER_0_BASE 0x8041020
#define TIMER_0_COUNTER_SIZE 32
#define TIMER_0_FIXED_PERIOD 0
#define TIMER_0_FREQ 50000000
#define TIMER_0_IRQ 4
#define TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define TIMER_0_LOAD_VALUE 49999
#define TIMER_0_MULT 0.001
#define TIMER_0_NAME "/dev/timer_0"
#define TIMER_0_PERIOD 1
#define TIMER_0_PERIOD_UNITS "ms"
#define TIMER_0_RESET_OUTPUT 0
#define TIMER_0_SNAPSHOT 1
#define TIMER_0_SPAN 32
#define TIMER_0_TICKS_PER_SEC 1000
#define TIMER_0_TIMEOUT_PULSE_OUTPUT 0
#define TIMER_0_TYPE "altera_avalon_timer"


/*
 * uart_0 configuration
 *
 */

#define ALT_MODULE_CLASS_uart_0 altera_avalon_uart
#define UART_0_BASE 0x8041040
#define UART_0_BAUD 9600
#define UART_0_DATA_BITS 8
#define UART_0_FIXED_BAUD 1
#define UART_0_FREQ 50000000
#define UART_0_IRQ 3
#define UART_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define UART_0_NAME "/dev/uart_0"
#define UART_0_PARITY 'N'
#define UART_0_SIM_CHAR_STREAM ""
#define UART_0_SIM_TRUE_BAUD 0
#define UART_0_SPAN 32
#define UART_0_STOP_BITS 1
#define UART_0_SYNC_REG_DEPTH 2
#define UART_0_TYPE "altera_avalon_uart"
#define UART_0_USE_CTS_RTS 0
#define UART_0_USE_EOP_REGISTER 0


/*
 * wren configuration
 *
 */

#define ALT_MODULE_CLASS_wren altera_avalon_pio
#define WREN_BASE 0x8041060
#define WREN_BIT_CLEARING_EDGE_REGISTER 0
#define WREN_BIT_MODIFYING_OUTPUT_REGISTER 0
#define WREN_CAPTURE 0
#define WREN_DATA_WIDTH 1
#define WREN_DO_TEST_BENCH_WIRING 0
#define WREN_DRIVEN_SIM_VALUE 0
#define WREN_EDGE_TYPE "NONE"
#define WREN_FREQ 50000000
#define WREN_HAS_IN 0
#define WREN_HAS_OUT 1
#define WREN_HAS_TRI 0
#define WREN_IRQ -1
#define WREN_IRQ_INTERRUPT_CONTROLLER_ID -1
#define WREN_IRQ_TYPE "NONE"
#define WREN_NAME "/dev/wren"
#define WREN_RESET_VALUE 0
#define WREN_SPAN 16
#define WREN_TYPE "altera_avalon_pio"

#endif /* __SYSTEM_H_ */
