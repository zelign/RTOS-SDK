# RTOS_SDK
The RTOS_SDK provide rapid development components for rtos kernels, like freertos. It supports multiple architectures, such as arm, riscv, 8051, etc, and supports various components, such as fs, cli, and more.
## Features
### Architectures
- ARM
### Drivers
- USART
- SPI
- CLOCK
- FLASH(NOR) + Command
- GPIO
- KEY
- LED
### Supported Boards
- STM32F407ZGT6
### Conponents
- FreeRTOS CLI
- File System
	- FileX
- LibC (Not STD)
### Build System
- Makefile
- Kconfig + memuconfig

## How to get this
### Environment configure
```
#Toolchain install for Ubuntu or others Debian system
sudo apt update
sudo apt install gcc-arm-none-eabi
```
### Build RTOS SDK
```
source scripts/select_prj.sh
Available projects:
 1.  8051 c51 stc89c51 demo
 2.  8051 c51 stc89c52 demo
 3.  8051 stc stc12 demo
 4.  8051 stc stc15 demo
 5.  8051 stc stc32g demo
 6.  arm nxp i.mx6ull demo
 7.  arm stm32 f407zgt6 demo
Choice your project: # Choose your projects

make #The make command will invoke `make menuconfig`, allowing you configure it according your requirements.
#The executable file is stored in output/xxx/package, and this directory also contains .map and .lst file for you debugging purpose.
```
The more detailler informations please reference to /doc/technical_manual.md

	
