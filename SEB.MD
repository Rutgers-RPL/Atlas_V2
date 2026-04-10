# Project Context

## Target Hardware
- **Primary MCU/Processor**: STM32F411RETx (STM32F411RET6TR, LQFP64 package)
- **Secondary Processors**: None
- **Dev Board / Custom PCB**: Custom PCB (Atlas_V2)
- **Board Revision**: V2

## Clock Configuration
- **HSE/HSI**: HSE 24 MHz external oscillator
- **PLL Config**: PLLM=12, PLLN=72, PLLQ=3 (VCO Input=2 MHz, VCO Output=144 MHz, PLL Output=72 MHz)
- **System Clock Frequency**: 24 MHz (HSE direct, SYSCLK source = HSE)
  - AHB: 24 MHz
  - APB1: 12 MHz (DIV2)
  - APB2: 24 MHz
  - USB OTG FS: 48 MHz (from PLL Q)

## Pin Mapping / GPIO Assignments
- **PA2**: PYRO (GPIO Output)
- **PA5**: LC_MISO (GPIO Output)
- **PA6**: LC_SCK (GPIO Input)
- **PA11**: USB_OTG_FS_DM
- **PA12**: USB_OTG_FS_DP
- **PA13**: SYS_JTMS-SWDIO (SWD)
- **PA14**: SYS_JTCK-SWCLK (SWD)
- **PA15**: FLASH_CS (GPIO Output, high speed, default high)
- **PB6**: RADIO_TX (USART1_TX)
- **PB7**: RADIO_RX (USART1_RX)
- **PB12**: LED (GPIO Output)
- **PB14**: TEMP_MISO (SPI2_MISO)
- **PB15**: SPI2_MOSI
- **PC0**: MULT_S1 (GPIO Output)
- **PC1**: MULT_S2 (GPIO Output)
- **PC2**: MULT_S3 (GPIO Output)
- **PC3**: PT_OUT (ADC1_IN13)
- **PC6**: T1_CS (GPIO Output)
- **PC7**: TEMP_SCK (SPI2_SCK)
- **PC8**: T2_CS (GPIO Output)
- **PC10**: FLASH_SCK/SD_CLK (SPI3_SCK)
- **PC11**: FLASH_MISO/SD_MISO (SPI3_MISO)
- **PC12**: FLASH_MOSI/SD_MOSI (SPI3_MOSI)
- **PD2**: SD_CS (GPIO Output, high speed, default high)
- **PH0**: OSC_IN (HSE)
- **PH1**: OSC_OUT (HSE)

## Memory Layout
- **Flash Size**: 512 KB (0x08000000 - 0x0807FFFF)
- **RAM Size**: 128 KB (0x20000000 - 0x2001FFFF)
- **Linker Script Regions**:
  - FLASH (rx): ORIGIN = 0x08000000, LENGTH = 512K
  - RAM (xrw): ORIGIN = 0x20000000, LENGTH = 128K
  - Heap Size: 0x2000 (8 KB)
  - Stack Size: 0x2000 (8 KB)

## Peripherals & Interfaces
- **ADC1**: Channel 13 (PC3 - PT_OUT), 3-cycle sampling
- **SPI2**: Full-duplex master (temperature sensors)
  - SCK: PC7 (TEMP_SCK)
  - MISO: PB14 (TEMP_MISO)
  - MOSI: PB15
  - CS: PC6 (T1_CS), PC8 (T2_CS)
- **SPI3**: Full-duplex master (flash memory / SD card)
  - SCK: PC10 (FLASH_SCK/SD_CLK)
  - MISO: PC11 (FLASH_MISO/SD_MISO)
  - MOSI: PC12 (FLASH_MOSI/SD_MOSI)
  - CS: PA15 (FLASH_CS), PD2 (SD_CS)
- **USART1**: Asynchronous mode (radio communication)
  - TX: PB6 (RADIO_TX)
  - RX: PB7 (RADIO_RX)
- **USB_OTG_FS**: Device mode (PA11/PA12)
- **CRC**: Enabled
- **FATFS**: Middleware enabled (with MKFS support)

## External Devices & Sensors
- **Flash Memory**: SPI flash on SPI3 (CS: PA15)
- **SD Card**: SPI mode on SPI3 (CS: PD2)
- **Temperature Sensors**: Two devices on SPI2 (T1_CS: PC6, T2_CS: PC8)
- **Radio Module**: UART interface on USART1
- **Pressure Transducer**: Analog input on ADC1_IN13 (PC3)
- **Multiplexer**: 3-bit select lines (MULT_S1/S2/S3: PC0/PC1/PC2)
- **Load Cell**: MISO/SCK signals (PA5/PA6)

## Communication Protocols
- **SPI2**: Full-duplex master mode (temperature sensors)
- **SPI3**: Full-duplex master mode (flash/SD card)
- **USART1**: Asynchronous mode (radio)
- **USB**: Full-speed device mode
- **ADC**: Single conversion mode

## Software Stack
- **Build System**: STM32CubeIDE managed build (Eclipse CDT)
- **Compile Command**: Build via STM32CubeIDE (arm-none-eabi-gcc)
- **HAL / Framework**: STM32Cube HAL (STM32Cube FW_F4 V1.28.2)
- **RTOS**: None (bare metal)
- **Toolchain**: arm-none-eabi-gcc (GNU ARM Embedded Toolchain)
- **Compiler Flags**:
  - Debug level: -g3
  - Optimization: -O0 (default debug)
  - FPU: -mfpu=fpv4-sp-d16
  - Float ABI: -mfloat-abi=hard
  - Defines: DEBUG, USE_HAL_DRIVER, STM32F411xE
- **External Libraries**: FatFs middleware (Third_Party/FatFs)

## Development Environment
- **Host OS**: Windows
- **IDE**: STM32CubeIDE
- **Debug Interface (SWD/JTAG)**: SWD (PA13/PA14)
- **Debug Probe (ST-Link/J-Link/OpenOCD)**: ST-Link (via STM32CubeIDE)

## Debug Configuration
<!-- Auto-learned during debug sessions. Edit these directly to change debug settings. -->
- **Debug Probe**: ST-Link
- **Runner Method**: STM32CubeIDE
- **GDB Server Command**: 
- **GDB Executable**: arm-none-eabi-gdb
- **Firmware ELF Path**: Debug/Atlas_V2.elf
- **GDB Server Port**: 
- **West Board**: 
- **West Build Directory**: 
- **West Venv Path**: 

## Power Management
- **Sleep Modes**: Not configured
- **Power Domains**: Standard STM32F4 power domains
- **Battery / Power Source**: External power supply

## Boot Configuration
- **Bootloader**: None (direct flash boot)
- **OTA Mechanism**: None

## Interrupt Configuration
- **NVIC Priority Groups**: NVIC_PRIORITYGROUP_4 (4 bits preemption, 0 bits sub-priority)
- **Key ISRs**:
  - SysTick: Priority 7 (preemption)
  - PendSV: Priority 15 (preemption)
  - BusFault, HardFault, MemoryManagement, UsageFault, DebugMonitor, SVCall: Priority 0

## Compliance & Standards
- None specified

## Supporting Devices
- Status LED on PB12
- Pyrotechnic control output on PA2

## Project Notes
- Project appears to be a data acquisition/telemetry system with:
  - Dual temperature sensor monitoring
  - Pressure transducer input via ADC
  - Load cell interface
  - Radio telemetry via USART
  - Data logging to SD card and/or SPI flash
  - USB connectivity for configuration/data download
  - Analog multiplexer for multi-channel sensing
  - Pyrotechnic firing capability
- System clock runs at 24 MHz (HSE direct) rather than using PLL for maximum speed
- FPU enabled with hard float ABI for efficient floating-point operations
