/*
 * Copyright (c) 2022-2023 Simon, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 */
/*
 * The boot function i want it has similar functionality with bootloader. I hope it can initialise some necessary hardware, then load and verify the image file.
 * Finally it will jump to execute image, it is important that before jump to kernel, it must set the kernel startup function is Thread mode with unpriviliged and use PSP,
 * it can be implemented by exception return mechanism.
 */

.syntax unified
.cpu cortex-m4
.fpu softvfp
.thumb

.global Boot_init
.global Vector_Table

.section .text.Boot_init
.weak Boot_init
.type Boot_init, %function

/*
 * In the cortex-m4 architecture, the processor will read 8 byte data from the vector table, the first 4 byte is the 
 * main stack address and the second 4 byte is the reset handler pointer. The vector table address is stored in VTOR(Vector Table Offset Register).
 * When execute the Boot_init function, the processor is the handle mode with privilige and usb MSP, so i set that the
 * MSP is the ccram space, the PSP is the ram space. 
*/
Boot_init:
	/* load data segment to RAM*/
	ldr r0, =_data_start_load
	ldr r1, =_data_start
	ldr r2, =_data_end
	mov r3, #0
	b Copy_Data_Seg
Loop_Copy_DataSeg:
	ldr r4, [r0, r3]
	str r4, [r1, r3]
	add r3, r3, #4
Copy_Data_Seg:
	add r4, r1, r3
	cmp r4, r2
	bcc Loop_Copy_DataSeg /* if r4-r1 */

	/* Zero fill the bss segment. */
	ldr r0, =_bss_start
	ldr r1, =_bss_end
	mov r2, #0
	mov r3, #0
	b Loop_Clear_Bss

Loop_Clear_Bss:
	str r3, [r0, r2]
	add r2, r2, #4
Clear_Bss:
	add r4, r0, r2
	cmp r4, r1
	bcc Loop_Clear_Bss /* r4 < r1 */
Boot_initcall_init:
	bl boot_init
Boot_main:
	ldr r1, =_psp_start
	msr psp, r1

	/*configurate the main function is privilege with psp 
	mrs r0, control
	orr r0, r0, #3
	msr control, r0
	isb
	dsb */

	/* exit the handle mode and jump into main function */
	ldr r0, =main
	bx r0

Endless_Loop:
    b Endless_Loop

.section .vector, "a"

Vector_Table:
    .word  _msp_start
    .word  Boot_init
    .word  NMI_Handler
    .word  HardFault_Handler
    .word  MemManage_Handler
    .word  BusFault_Handler
    .word  UsageFault_Handler
    .word  0
    .word  0
    .word  0
    .word  0
    .word  SVC_Handler
    .word  DebugMon_Handler
    .word  0
    .word  PendSV_Handler
    .word  SysTick_Handler
	.word  WWDG_IRQHandler
	.word  PVD_IRQHandler  
	.word  TAMP_STAMP_IRQHandler    
	.word  RTC_WKUP_IRQHandler   
	.word  FLASH_IRQHandler  
	.word  RCC_IRQHandler    
	.word  EXTI0_IRQHandler   
	.word  EXTI1_IRQHandler   
	.word  EXTI2_IRQHandler   
	.word  EXTI3_IRQHandler   
	.word  EXTI4_IRQHandler   
	.word  DMA1_Stream0_IRQHandler    
	.word  DMA1_Stream1_IRQHandler    
	.word  DMA1_Stream2_IRQHandler    
	.word  DMA1_Stream3_IRQHandler    
	.word  DMA1_Stream4_IRQHandler    
	.word  DMA1_Stream5_IRQHandler    
	.word  DMA1_Stream6_IRQHandler    
	.word  ADC_IRQHandler    
	.word  CAN1_TX_IRQHandler    
	.word  CAN1_RX0_IRQHandler   
	.word  CAN1_RX1_IRQHandler   
	.word  CAN1_SCE_IRQHandler   
	.word  EXTI9_5_IRQHandler    
	.word  TIM1_BRK_TIM9_IRQHandler   
	.word  TIM1_UP_TIM10_IRQHandler   
	.word  TIM1_TRG_COM_TIM11_IRQHandler
	.word  TIM1_CC_IRQHandler    
	.word  TIM2_IRQHandler   
	.word  TIM3_IRQHandler   
	.word  TIM4_IRQHandler   
	.word  I2C1_EV_IRQHandler    
	.word  I2C1_ER_IRQHandler    
	.word  I2C2_EV_IRQHandler    
	.word  I2C2_ER_IRQHandler   
	.word  SPI1_IRQHandler   
	.word  SPI2_IRQHandler   
	.word  USART1_IRQHandler    
	.word  USART2_IRQHandler    
	.word  USART3_IRQHandler    
	.word  EXTI15_10_IRQHandler  
	.word  RTC_Alarm_IRQHandler  
	.word  OTG_FS_WKUP_IRQHandler    
	.word  TIM8_BRK_TIM12_IRQHandler  
	.word  TIM8_UP_TIM13_IRQHandler   
	.word  TIM8_TRG_COM_TIM14_IRQHandler 
	.word  TIM8_CC_IRQHandler    
	.word  DMA1_Stream7_IRQHandler  
	.word  FSMC_IRQHandler   
	.word  SDIO_IRQHandler   
	.word  TIM5_IRQHandler   
	.word  SPI3_IRQHandler   
	.word  UART4_IRQHandler  
	.word  UART5_IRQHandler  
	.word  TIM6_DAC_IRQHandler    
	.word  TIM7_IRQHandler   
	.word  DMA2_Stream0_IRQHandler    
	.word  DMA2_Stream1_IRQHandler    
	.word  DMA2_Stream2_IRQHandler    
	.word  DMA2_Stream3_IRQHandler    
	.word  DMA2_Stream4_IRQHandler    
	.word  ETH_IRQHandler    
	.word  ETH_WKUP_IRQHandler   
	.word  CAN2_TX_IRQHandler    
	.word  CAN2_RX0_IRQHandler   
	.word  CAN2_RX1_IRQHandler   
	.word  CAN2_SCE_IRQHandler   
	.word  OTG_FS_IRQHandler    
	.word  DMA2_Stream5_IRQHandler    
	.word  DMA2_Stream6_IRQHandler    
	.word  DMA2_Stream7_IRQHandler    
	.word  USART6_IRQHandler  
	.word  I2C3_EV_IRQHandler    
	.word  I2C3_ER_IRQHandler    
	.word  OTG_HS_EP1_OUT_IRQHandler  
	.word  OTG_HS_EP1_IN_IRQHandler   
	.word  OTG_HS_WKUP_IRQHandler   
	.word  OTG_HS_IRQHandler    
	.word  DCMI_IRQHandler   
	.word  CRYP_IRQHandler   
	.word  HASH_RNG_IRQHandler  
	.word  FPU_IRQHandler 	


.weak   NMI_Handler
.thumb_set NMI_Handler,Endless_Loop

.weak   HardFault_Handler
.thumb_set HardFault_Handler,Endless_Loop

.weak   MemManage_Handler
.thumb_set MemManage_Handler,Endless_Loop

.weak   BusFault_Handler
.thumb_set BusFault_Handler,Endless_Loop

.weak   UsageFault_Handler
.thumb_set UsageFault_Handler,Endless_Loop

.weak   SVC_Handler
.thumb_set SVC_Handler,Endless_Loop

.weak   DebugMon_Handler
.thumb_set DebugMon_Handler,Endless_Loop

.weak   PendSV_Handler
.thumb_set PendSV_Handler,Endless_Loop

.weak   SysTick_Handler
.thumb_set SysTick_Handler,Endless_Loop

.weak  WWDG_IRQHandler    			// Window WatchDog    
.weak  PVD_IRQHandler  				// PVD through EXTI Line detection   
.weak  TAMP_STAMP_IRQHandler    	// Tamper and TimeStamps through the EXTI line   
.weak  RTC_WKUP_IRQHandler  		 // RTC Wakeup through the EXTI line  
.weak  FLASH_IRQHandler  			 // FLASH    
.weak  RCC_IRQHandler  				// RCC   
.weak  EXTI0_IRQHandler   // EXTI Line0   
.weak  EXTI1_IRQHandler   // EXTI Line1   
.weak  EXTI2_IRQHandler   // EXTI Line2   
.weak  EXTI3_IRQHandler   // EXTI Line3   
.weak  EXTI4_IRQHandler   // EXTI Line4   
.weak  DMA1_Stream0_IRQHandler  // DMA1 Stream 0  
.weak  DMA1_Stream1_IRQHandler  // DMA1 Stream 1  
.weak  DMA1_Stream2_IRQHandler  // DMA1 Stream 2  
.weak  DMA1_Stream3_IRQHandler  // DMA1 Stream 3  
.weak  DMA1_Stream4_IRQHandler  // DMA1 Stream 4  
.weak  DMA1_Stream5_IRQHandler  // DMA1 Stream 5  
.weak  DMA1_Stream6_IRQHandler  // DMA1 Stream 6  
.weak  ADC_IRQHandler  // ADC1, ADC2 and ADC3s    
.weak  CAN1_TX_IRQHandler    // CAN1 TX   
.weak  CAN1_RX0_IRQHandler   // CAN1 RX0  
.weak  CAN1_RX1_IRQHandler   // CAN1 RX1  
.weak  CAN1_SCE_IRQHandler   // CAN1 SCE  
.weak  EXTI9_5_IRQHandler    // External Line[9:5]s   
.weak  TIM1_BRK_TIM9_IRQHandler    // TIM1 Break and TIM9    
.weak  TIM1_UP_TIM10_IRQHandler    // TIM1 Update and TIM10  
.weak  TIM1_TRG_COM_TIM11_IRQHandler  // TIM1 Trigger and Commutation and TIM11
.weak  TIM1_CC_IRQHandler    // TIM1 Capture Compare  
.weak  TIM2_IRQHandler    // TIM2  
.weak  TIM3_IRQHandler    // TIM3  
.weak  TIM4_IRQHandler    // TIM4  
.weak  I2C1_EV_IRQHandler    // I2C1 Event   
.weak  I2C1_ER_IRQHandler    // I2C1 Error   
.weak  I2C2_EV_IRQHandler    // I2C2 Event   
.weak  I2C2_ER_IRQHandler    // I2C2 Error  
.weak  SPI1_IRQHandler    // SPI1  
.weak  SPI2_IRQHandler    // SPI2  
.weak  USART1_IRQHandler  // USART1   
.weak  USART2_IRQHandler  // USART2   
.weak  USART3_IRQHandler  // USART3   
.weak  EXTI15_10_IRQHandler  // External Line[15:10]s    
.weak  RTC_Alarm_IRQHandler  // RTC Alarm (A and B) through EXTI Line   
.weak  OTG_FS_WKUP_IRQHandler   // USB OTG FS Wakeup through EXTI line   
.weak  TIM8_BRK_TIM12_IRQHandler   // TIM8 Break and TIM12   
.weak  TIM8_UP_TIM13_IRQHandler    // TIM8 Update and TIM13  
.weak  TIM8_TRG_COM_TIM14_IRQHandler  // TIM8 Trigger and Commutation and TIM14
.weak  TIM8_CC_IRQHandler    // TIM8 Capture Compare  
.weak  DMA1_Stream7_IRQHandler  // DMA1 Stream7    
.weak  FSMC_IRQHandler    // FSMC  
.weak  SDIO_IRQHandler    // SDIO  
.weak  TIM5_IRQHandler    // TIM5  
.weak  SPI3_IRQHandler    // SPI3  
.weak  UART4_IRQHandler   // UART4    
.weak  UART5_IRQHandler   // UART5    
.weak  TIM6_DAC_IRQHandler   // TIM6 and DAC1&2 underrun errors    
.weak  TIM7_IRQHandler    // TIM7    
.weak  DMA2_Stream0_IRQHandler  // DMA2 Stream 0  
.weak  DMA2_Stream1_IRQHandler  // DMA2 Stream 1  
.weak  DMA2_Stream2_IRQHandler  // DMA2 Stream 2  
.weak  DMA2_Stream3_IRQHandler  // DMA2 Stream 3  
.weak  DMA2_Stream4_IRQHandler  // DMA2 Stream 4  
.weak  ETH_IRQHandler  // Ethernet    
.weak  ETH_WKUP_IRQHandler   // Ethernet Wakeup through EXTI line    
.weak  CAN2_TX_IRQHandler    // CAN2 TX   
.weak  CAN2_RX0_IRQHandler   // CAN2 RX0  
.weak  CAN2_RX1_IRQHandler   // CAN2 RX1  
.weak  CAN2_SCE_IRQHandler   // CAN2 SCE  
.weak  OTG_FS_IRQHandler  // USB OTG FS  
.weak  DMA2_Stream5_IRQHandler  // DMA2 Stream 5  
.weak  DMA2_Stream6_IRQHandler  // DMA2 Stream 6  
.weak  DMA2_Stream7_IRQHandler  // DMA2 Stream 7  
.weak  USART6_IRQHandler  // USART6    
.weak  I2C3_EV_IRQHandler    // I2C3 event   
.weak  I2C3_ER_IRQHandler    // I2C3 error   
.weak  OTG_HS_EP1_OUT_IRQHandler   // USB OTG HS End Point 1 Out    
.weak  OTG_HS_EP1_IN_IRQHandler    // USB OTG HS End Point 1 In  
.weak  OTG_HS_WKUP_IRQHandler   // USB OTG HS Wakeup through EXTI    
.weak  OTG_HS_IRQHandler  // USB OTG HS  
.weak  DCMI_IRQHandler    // DCMI  
.weak  CRYP_IRQHandler    // CRYP crypto    
.weak  HASH_RNG_IRQHandler   // Hash and Rng
.weak  FPU_IRQHandler  // FPU
