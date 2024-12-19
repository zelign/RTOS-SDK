/**
 * NOTE:This linker script defined some memory layouts as follow:
 * RAM layout:
 * +---------------------+ low address:0x20000000
 * |       .data         |
 * +---------------------+
 * |        .bss         |
 * +---------------------+
 * |   .stack & .heap    |
 * +---------------------+ high address
 * 
 * FLASH layout:
 * +---------------------+ low address:0x8000000
 * |     vector table    |
 * +---------------------+
 * |       .text         |
 * +---------------------+
 * |      .rodata        |
 * +---------------------+
 * |    .init_array      |
 * +---------------------+
 * |    .fini_array      |
 * +---------------------+
 * |       .data         |
 * +---------------------+
 * |       .ccmram       |
 * +---------------------+ high address
 *
 */

PSP_Size = 0x4000;
MSP_Size = 0x5000;

/* Point to the end of CCMRAM */
_ccm_stack_start = ORIGIN(CCMRAM) + LENGTH(CCMRAM);
__msp_start = _ccm_stack_start - MSP_Size;
MEMORY
{
	RAM (xrw): ORIGIN = 0x20000000, LENGTH = 128K
	CCMRAM (xrw): ORIGIN = 0X10000000, LENGTH = 64K
	FLASH (rx): ORIGIN = 0X8000000, LENGTH = 1024K
}

/*Define the entry point for process*/
ENTRY(Vector_Table)

SECTIONS
{
	_vector_start_load = LOADADDR(.vector);
	.vector :
	{
		. = ALIGN(4);
		PROVIDE(_flash_img_start = .);
		_vector_start = .;
		KEEP(*(.vector))
		. = ALIGN(4);
		_vector_end = .;
	} >FLASH

	.text : 
	{
		. = ALIGN(4);
		PROVIDE(_text_start = .);
		*(.text)
		*(.text*)
		PROVIDE(_crti = .);
		KEEP (*(.init))
   		KEEP (*(.fini))
		. = ALIGN(4);
		PROVIDE(_text_end = .);
	} >FLASH

	.rodata : 
	{
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
        . = ALIGN(4);
	} >FLASH
	.init_call :
	{
		. = ALIGN(4);
		_boot_init_start = .;
		KEEP(*(.init_call.boot_1))
		KEEP(*(.init_call.boot_2))
		KEEP(*(.init_call.boot_3))
		_boot_init_end = .;
        . = ALIGN(4);
		_app_init_start = .;
		KEEP(*(.init_call.application_1))
		KEEP(*(.init_call.application_2))
		KEEP(*(.init_call.application_3))
		_app_init_end = .;
		PROVIDE(_flash_img_end = .);
		. = ALIGN(4);
	} >FLASH


	_data_start_load = LOADADDR(.data);
	.data : 
	{
		. = ALIGN(4);
		_data_start = .;
		*(.data)
		*(.data*)
		. = ALIGN(4);
		_data_end = .;
	} >RAM AT>FLASH

	_ccmram_start_load = LOADADDR(.ccmram); /* The ccmram is used for MSP */
	.ccmram :
	{
		. = ALIGN(4);
		_ccmram_start = .;
		*(.ccmram)
		*(.ccmram*)
		. = ALIGN(4);
		_ccmram_end = .;
	} >CCMRAM AT>FLASH

	.rel.dyn : {
		. = ALIGN(4);
    	KEEP(*(.rela*))
    	. = ALIGN(4);
	} >FLASH

	.bss (NOLOAD) : 
	{
		. = ALIGN(4);
		_bss_start = .;
		*(.bss)
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_bss_end = .;
	} >RAM

	/* The .stack is used for PSP */
	.psp : 
	{
		. = ALIGN(4);
		_psp_end = .;
		. += PSP_Size;
		. = ALIGN(4);
		_psp_start = .;
	} >RAM

	.msp __msp_start :
	{
		. = ALIGN(4);
		_msp_end = .;
		. += MSP_Size;
		. = ALIGN(4);
		_msp_start = .;
	} >CCMRAM

	.heap : 
	{
		. = ALIGN(4);
		PROVIDE ( _heap_start = .);
		_heap_len = ABSOLUTE(ORIGIN(RAM) + LENGTH(RAM) - _heap_start);
		. += _heap_len;
		. = ALIGN(4);
		PROVIDE ( _heap_end = .);
	} >RAM
	/* /DISCAED/ : 
	{
		*(*)
	} */
	.ARM.attributes : 
	{
		*(.ARM.attributes)
	}
}
