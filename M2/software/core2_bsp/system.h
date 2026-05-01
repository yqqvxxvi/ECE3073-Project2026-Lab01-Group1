/*
 * system.h - SOPC Builder system and BSP software package information
 *
 * Machine generated for CPU 'nios2_core2' in SOPC Builder design 'dual_processor'
 * SOPC Builder design path: ../../dual_processor.sopcinfo
 *
 * Generated: Fri May 01 14:02:11 SGT 2026
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
 * CORE_RAM2 configuration
 *
 */

#define ALT_MODULE_CLASS_CORE_RAM2 altera_avalon_onchip_memory2
#define CORE_RAM2_ALLOW_IN_SYSTEM_MEMORY_CONTENT_EDITOR 0
#define CORE_RAM2_ALLOW_MRAM_SIM_CONTENTS_ONLY_FILE 0
#define CORE_RAM2_BASE 0x10000
#define CORE_RAM2_CONTENTS_INFO ""
#define CORE_RAM2_DUAL_PORT 0
#define CORE_RAM2_GUI_RAM_BLOCK_TYPE "AUTO"
#define CORE_RAM2_INIT_CONTENTS_FILE "dual_processor_CORE_RAM2"
#define CORE_RAM2_INIT_MEM_CONTENT 1
#define CORE_RAM2_INSTANCE_ID "NONE"
#define CORE_RAM2_IRQ -1
#define CORE_RAM2_IRQ_INTERRUPT_CONTROLLER_ID -1
#define CORE_RAM2_NAME "/dev/CORE_RAM2"
#define CORE_RAM2_NON_DEFAULT_INIT_FILE_ENABLED 0
#define CORE_RAM2_RAM_BLOCK_TYPE "AUTO"
#define CORE_RAM2_READ_DURING_WRITE_MODE "DONT_CARE"
#define CORE_RAM2_SINGLE_CLOCK_OP 0
#define CORE_RAM2_SIZE_MULTIPLE 1
#define CORE_RAM2_SIZE_VALUE 40960
#define CORE_RAM2_SPAN 40960
#define CORE_RAM2_TYPE "altera_avalon_onchip_memory2"
#define CORE_RAM2_WRITABLE 1


/*
 * CPU configuration
 *
 */

#define ALT_CPU_ARCHITECTURE "altera_nios2_gen2"
#define ALT_CPU_BIG_ENDIAN 0
#define ALT_CPU_BREAK_ADDR 0x00020820
#define ALT_CPU_CPU_ARCH_NIOS2_R1
#define ALT_CPU_CPU_FREQ 100000000u
#define ALT_CPU_CPU_ID_SIZE 1
#define ALT_CPU_CPU_ID_VALUE 0x00000001
#define ALT_CPU_CPU_IMPLEMENTATION "fast"
#define ALT_CPU_DATA_ADDR_WIDTH 0x1a
#define ALT_CPU_DCACHE_BYPASS_MASK 0x80000000
#define ALT_CPU_DCACHE_LINE_SIZE 32
#define ALT_CPU_DCACHE_LINE_SIZE_LOG2 5
#define ALT_CPU_DCACHE_SIZE 2048
#define ALT_CPU_EXCEPTION_ADDR 0x00010020
#define ALT_CPU_FLASH_ACCELERATOR_LINES 0
#define ALT_CPU_FLASH_ACCELERATOR_LINE_SIZE 0
#define ALT_CPU_FLUSHDA_SUPPORTED
#define ALT_CPU_FREQ 100000000
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
#define ALT_CPU_INST_ADDR_WIDTH 0x19
#define ALT_CPU_NAME "nios2_core2"
#define ALT_CPU_NUM_OF_SHADOW_REG_SETS 0
#define ALT_CPU_OCI_VERSION 1
#define ALT_CPU_RESET_ADDR 0x00010000


/*
 * CPU configuration (with legacy prefix - don't use these anymore)
 *
 */

#define NIOS2_BIG_ENDIAN 0
#define NIOS2_BREAK_ADDR 0x00020820
#define NIOS2_CPU_ARCH_NIOS2_R1
#define NIOS2_CPU_FREQ 100000000u
#define NIOS2_CPU_ID_SIZE 1
#define NIOS2_CPU_ID_VALUE 0x00000001
#define NIOS2_CPU_IMPLEMENTATION "fast"
#define NIOS2_DATA_ADDR_WIDTH 0x1a
#define NIOS2_DCACHE_BYPASS_MASK 0x80000000
#define NIOS2_DCACHE_LINE_SIZE 32
#define NIOS2_DCACHE_LINE_SIZE_LOG2 5
#define NIOS2_DCACHE_SIZE 2048
#define NIOS2_EXCEPTION_ADDR 0x00010020
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
#define NIOS2_INST_ADDR_WIDTH 0x19
#define NIOS2_NUM_OF_SHADOW_REG_SETS 0
#define NIOS2_OCI_VERSION 1
#define NIOS2_RESET_ADDR 0x00010000


/*
 * Define for each module class mastered by the CPU
 *
 */

#define __ALTERA_AVALON_JTAG_UART
#define __ALTERA_AVALON_MUTEX
#define __ALTERA_AVALON_NEW_SDRAM_CONTROLLER
#define __ALTERA_AVALON_ONCHIP_MEMORY2
#define __ALTERA_AVALON_PIO
#define __ALTERA_AVALON_SPI
#define __ALTERA_NIOS2_GEN2
#define __ALTERA_UP_AVALON_ACCELEROMETER_SPI
#define __ALTERA_UP_AVALON_VIDEO_PIXEL_BUFFER_DMA


/*
 * HEX012 configuration
 *
 */

#define ALT_MODULE_CLASS_HEX012 altera_avalon_pio
#define HEX012_BASE 0x21030
#define HEX012_BIT_CLEARING_EDGE_REGISTER 0
#define HEX012_BIT_MODIFYING_OUTPUT_REGISTER 0
#define HEX012_CAPTURE 0
#define HEX012_DATA_WIDTH 24
#define HEX012_DO_TEST_BENCH_WIRING 0
#define HEX012_DRIVEN_SIM_VALUE 0
#define HEX012_EDGE_TYPE "NONE"
#define HEX012_FREQ 100000000
#define HEX012_HAS_IN 0
#define HEX012_HAS_OUT 1
#define HEX012_HAS_TRI 0
#define HEX012_IRQ -1
#define HEX012_IRQ_INTERRUPT_CONTROLLER_ID -1
#define HEX012_IRQ_TYPE "NONE"
#define HEX012_NAME "/dev/HEX012"
#define HEX012_RESET_VALUE 0
#define HEX012_SPAN 16
#define HEX012_TYPE "altera_avalon_pio"


/*
 * HEX345 configuration
 *
 */

#define ALT_MODULE_CLASS_HEX345 altera_avalon_pio
#define HEX345_BASE 0x21020
#define HEX345_BIT_CLEARING_EDGE_REGISTER 0
#define HEX345_BIT_MODIFYING_OUTPUT_REGISTER 0
#define HEX345_CAPTURE 0
#define HEX345_DATA_WIDTH 24
#define HEX345_DO_TEST_BENCH_WIRING 0
#define HEX345_DRIVEN_SIM_VALUE 0
#define HEX345_EDGE_TYPE "NONE"
#define HEX345_FREQ 100000000
#define HEX345_HAS_IN 0
#define HEX345_HAS_OUT 1
#define HEX345_HAS_TRI 0
#define HEX345_IRQ -1
#define HEX345_IRQ_INTERRUPT_CONTROLLER_ID -1
#define HEX345_IRQ_TYPE "NONE"
#define HEX345_NAME "/dev/HEX345"
#define HEX345_RESET_VALUE 0
#define HEX345_SPAN 16
#define HEX345_TYPE "altera_avalon_pio"


/*
 * Key01 configuration
 *
 */

#define ALT_MODULE_CLASS_Key01 altera_avalon_pio
#define KEY01_BASE 0x21080
#define KEY01_BIT_CLEARING_EDGE_REGISTER 0
#define KEY01_BIT_MODIFYING_OUTPUT_REGISTER 0
#define KEY01_CAPTURE 0
#define KEY01_DATA_WIDTH 2
#define KEY01_DO_TEST_BENCH_WIRING 0
#define KEY01_DRIVEN_SIM_VALUE 0
#define KEY01_EDGE_TYPE "NONE"
#define KEY01_FREQ 100000000
#define KEY01_HAS_IN 1
#define KEY01_HAS_OUT 0
#define KEY01_HAS_TRI 0
#define KEY01_IRQ 2
#define KEY01_IRQ_INTERRUPT_CONTROLLER_ID 0
#define KEY01_IRQ_TYPE "LEVEL"
#define KEY01_NAME "/dev/Key01"
#define KEY01_RESET_VALUE 0
#define KEY01_SPAN 16
#define KEY01_TYPE "altera_avalon_pio"


/*
 * SDRAM_MUTEX configuration
 *
 */

#define ALT_MODULE_CLASS_SDRAM_MUTEX altera_avalon_mutex
#define SDRAM_MUTEX_BASE 0x2021080
#define SDRAM_MUTEX_IRQ -1
#define SDRAM_MUTEX_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SDRAM_MUTEX_NAME "/dev/SDRAM_MUTEX"
#define SDRAM_MUTEX_OWNER_INIT 0
#define SDRAM_MUTEX_OWNER_WIDTH 16
#define SDRAM_MUTEX_SPAN 8
#define SDRAM_MUTEX_TYPE "altera_avalon_mutex"
#define SDRAM_MUTEX_VALUE_INIT 0
#define SDRAM_MUTEX_VALUE_WIDTH 16


/*
 * SHARED_SDRAM configuration
 *
 */

#define ALT_MODULE_CLASS_SHARED_SDRAM altera_avalon_new_sdram_controller
#define SHARED_SDRAM_BASE 0x1000000
#define SHARED_SDRAM_CAS_LATENCY 3
#define SHARED_SDRAM_CONTENTS_INFO
#define SHARED_SDRAM_INIT_NOP_DELAY 0.0
#define SHARED_SDRAM_INIT_REFRESH_COMMANDS 2
#define SHARED_SDRAM_IRQ -1
#define SHARED_SDRAM_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SHARED_SDRAM_IS_INITIALIZED 1
#define SHARED_SDRAM_NAME "/dev/SHARED_SDRAM"
#define SHARED_SDRAM_POWERUP_DELAY 100.0
#define SHARED_SDRAM_REFRESH_PERIOD 15.625
#define SHARED_SDRAM_REGISTER_DATA_IN 1
#define SHARED_SDRAM_SDRAM_ADDR_WIDTH 0x17
#define SHARED_SDRAM_SDRAM_BANK_WIDTH 2
#define SHARED_SDRAM_SDRAM_COL_WIDTH 8
#define SHARED_SDRAM_SDRAM_DATA_WIDTH 16
#define SHARED_SDRAM_SDRAM_NUM_BANKS 4
#define SHARED_SDRAM_SDRAM_NUM_CHIPSELECTS 1
#define SHARED_SDRAM_SDRAM_ROW_WIDTH 13
#define SHARED_SDRAM_SHARED_DATA 0
#define SHARED_SDRAM_SIM_MODEL_BASE 0
#define SHARED_SDRAM_SPAN 16777216
#define SHARED_SDRAM_STARVATION_INDICATOR 0
#define SHARED_SDRAM_TRISTATE_BRIDGE_SLAVE ""
#define SHARED_SDRAM_TYPE "altera_avalon_new_sdram_controller"
#define SHARED_SDRAM_T_AC 5.5
#define SHARED_SDRAM_T_MRD 3
#define SHARED_SDRAM_T_RCD 20.0
#define SHARED_SDRAM_T_RFC 70.0
#define SHARED_SDRAM_T_RP 20.0
#define SHARED_SDRAM_T_WR 14.0


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
#define ALT_STDERR "/dev/jtag_uart_2"
#define ALT_STDERR_BASE 0x21090
#define ALT_STDERR_DEV jtag_uart_2
#define ALT_STDERR_IS_JTAG_UART
#define ALT_STDERR_PRESENT
#define ALT_STDERR_TYPE "altera_avalon_jtag_uart"
#define ALT_STDIN "/dev/jtag_uart_2"
#define ALT_STDIN_BASE 0x21090
#define ALT_STDIN_DEV jtag_uart_2
#define ALT_STDIN_IS_JTAG_UART
#define ALT_STDIN_PRESENT
#define ALT_STDIN_TYPE "altera_avalon_jtag_uart"
#define ALT_STDOUT "/dev/jtag_uart_2"
#define ALT_STDOUT_BASE 0x21090
#define ALT_STDOUT_DEV jtag_uart_2
#define ALT_STDOUT_IS_JTAG_UART
#define ALT_STDOUT_PRESENT
#define ALT_STDOUT_TYPE "altera_avalon_jtag_uart"
#define ALT_SYSTEM_NAME "dual_processor"


/*
 * accelerometer_spi_0 configuration
 *
 */

#define ACCELEROMETER_SPI_0_BASE 0x21098
#define ACCELEROMETER_SPI_0_IRQ 0
#define ACCELEROMETER_SPI_0_IRQ_INTERRUPT_CONTROLLER_ID 0
#define ACCELEROMETER_SPI_0_NAME "/dev/accelerometer_spi_0"
#define ACCELEROMETER_SPI_0_SPAN 2
#define ACCELEROMETER_SPI_0_TYPE "altera_up_avalon_accelerometer_spi"
#define ALT_MODULE_CLASS_accelerometer_spi_0 altera_up_avalon_accelerometer_spi


/*
 * core2_rx configuration
 *
 */

#define ALT_MODULE_CLASS_core2_rx altera_avalon_pio
#define CORE2_RX_BASE 0x21070
#define CORE2_RX_BIT_CLEARING_EDGE_REGISTER 0
#define CORE2_RX_BIT_MODIFYING_OUTPUT_REGISTER 0
#define CORE2_RX_CAPTURE 0
#define CORE2_RX_DATA_WIDTH 1
#define CORE2_RX_DO_TEST_BENCH_WIRING 0
#define CORE2_RX_DRIVEN_SIM_VALUE 0
#define CORE2_RX_EDGE_TYPE "NONE"
#define CORE2_RX_FREQ 100000000
#define CORE2_RX_HAS_IN 1
#define CORE2_RX_HAS_OUT 0
#define CORE2_RX_HAS_TRI 0
#define CORE2_RX_IRQ 4
#define CORE2_RX_IRQ_INTERRUPT_CONTROLLER_ID 0
#define CORE2_RX_IRQ_TYPE "LEVEL"
#define CORE2_RX_NAME "/dev/core2_rx"
#define CORE2_RX_RESET_VALUE 0
#define CORE2_RX_SPAN 16
#define CORE2_RX_TYPE "altera_avalon_pio"


/*
 * core2_tx configuration
 *
 */

#define ALT_MODULE_CLASS_core2_tx altera_avalon_pio
#define CORE2_TX_BASE 0x21060
#define CORE2_TX_BIT_CLEARING_EDGE_REGISTER 0
#define CORE2_TX_BIT_MODIFYING_OUTPUT_REGISTER 0
#define CORE2_TX_CAPTURE 0
#define CORE2_TX_DATA_WIDTH 1
#define CORE2_TX_DO_TEST_BENCH_WIRING 0
#define CORE2_TX_DRIVEN_SIM_VALUE 0
#define CORE2_TX_EDGE_TYPE "NONE"
#define CORE2_TX_FREQ 100000000
#define CORE2_TX_HAS_IN 0
#define CORE2_TX_HAS_OUT 1
#define CORE2_TX_HAS_TRI 0
#define CORE2_TX_IRQ -1
#define CORE2_TX_IRQ_INTERRUPT_CONTROLLER_ID -1
#define CORE2_TX_IRQ_TYPE "NONE"
#define CORE2_TX_NAME "/dev/core2_tx"
#define CORE2_TX_RESET_VALUE 0
#define CORE2_TX_SPAN 16
#define CORE2_TX_TYPE "altera_avalon_pio"


/*
 * hal configuration
 *
 */

#define ALT_INCLUDE_INSTRUCTION_RELATED_EXCEPTION_API
#define ALT_MAX_FD 4
#define ALT_SYS_CLK none
#define ALT_TIMESTAMP_CLK none


/*
 * jtag_uart_2 configuration
 *
 */

#define ALT_MODULE_CLASS_jtag_uart_2 altera_avalon_jtag_uart
#define JTAG_UART_2_BASE 0x21090
#define JTAG_UART_2_IRQ 3
#define JTAG_UART_2_IRQ_INTERRUPT_CONTROLLER_ID 0
#define JTAG_UART_2_NAME "/dev/jtag_uart_2"
#define JTAG_UART_2_READ_DEPTH 64
#define JTAG_UART_2_READ_THRESHOLD 8
#define JTAG_UART_2_SPAN 8
#define JTAG_UART_2_TYPE "altera_avalon_jtag_uart"
#define JTAG_UART_2_WRITE_DEPTH 64
#define JTAG_UART_2_WRITE_THRESHOLD 8


/*
 * spi_0 configuration
 *
 */

#define ALT_MODULE_CLASS_spi_0 altera_avalon_spi
#define SPI_0_BASE 0x21000
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
#define SPI_0_IRQ 1
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
 * spi_ss configuration
 *
 */

#define ALT_MODULE_CLASS_spi_ss altera_avalon_pio
#define SPI_SS_BASE 0x21050
#define SPI_SS_BIT_CLEARING_EDGE_REGISTER 0
#define SPI_SS_BIT_MODIFYING_OUTPUT_REGISTER 0
#define SPI_SS_CAPTURE 0
#define SPI_SS_DATA_WIDTH 1
#define SPI_SS_DO_TEST_BENCH_WIRING 0
#define SPI_SS_DRIVEN_SIM_VALUE 0
#define SPI_SS_EDGE_TYPE "NONE"
#define SPI_SS_FREQ 100000000
#define SPI_SS_HAS_IN 0
#define SPI_SS_HAS_OUT 1
#define SPI_SS_HAS_TRI 0
#define SPI_SS_IRQ -1
#define SPI_SS_IRQ_INTERRUPT_CONTROLLER_ID -1
#define SPI_SS_IRQ_TYPE "NONE"
#define SPI_SS_NAME "/dev/spi_ss"
#define SPI_SS_RESET_VALUE 0
#define SPI_SS_SPAN 16
#define SPI_SS_TYPE "altera_avalon_pio"


/*
 * us_counter configuration
 *
 */

#define ALT_MODULE_CLASS_us_counter altera_avalon_pio
#define US_COUNTER_BASE 0x2021070
#define US_COUNTER_BIT_CLEARING_EDGE_REGISTER 0
#define US_COUNTER_BIT_MODIFYING_OUTPUT_REGISTER 0
#define US_COUNTER_CAPTURE 0
#define US_COUNTER_DATA_WIDTH 32
#define US_COUNTER_DO_TEST_BENCH_WIRING 0
#define US_COUNTER_DRIVEN_SIM_VALUE 0
#define US_COUNTER_EDGE_TYPE "NONE"
#define US_COUNTER_FREQ 100000000
#define US_COUNTER_HAS_IN 1
#define US_COUNTER_HAS_OUT 0
#define US_COUNTER_HAS_TRI 0
#define US_COUNTER_IRQ -1
#define US_COUNTER_IRQ_INTERRUPT_CONTROLLER_ID -1
#define US_COUNTER_IRQ_TYPE "NONE"
#define US_COUNTER_NAME "/dev/us_counter"
#define US_COUNTER_RESET_VALUE 0
#define US_COUNTER_SPAN 16
#define US_COUNTER_TYPE "altera_avalon_pio"


/*
 * video_pixel_buffer_dma_0 configuration
 *
 */

#define ALT_MODULE_CLASS_video_pixel_buffer_dma_0 altera_up_avalon_video_pixel_buffer_dma
#define VIDEO_PIXEL_BUFFER_DMA_0_BASE 0x21040
#define VIDEO_PIXEL_BUFFER_DMA_0_IRQ -1
#define VIDEO_PIXEL_BUFFER_DMA_0_IRQ_INTERRUPT_CONTROLLER_ID -1
#define VIDEO_PIXEL_BUFFER_DMA_0_NAME "/dev/video_pixel_buffer_dma_0"
#define VIDEO_PIXEL_BUFFER_DMA_0_SPAN 16
#define VIDEO_PIXEL_BUFFER_DMA_0_TYPE "altera_up_avalon_video_pixel_buffer_dma"

#endif /* __SYSTEM_H_ */
