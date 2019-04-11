/*
 * Copyright 2018, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/** @file
 * Defines internal configuration of the BCM943362WCD8 board
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/* MCU constants and options */
//#define CPU_CLOCK_HZ                   (123000000)
//#define PERIPHERAL_CLOCK_HZ            (123000000)
//
///* SDIO_4_BIT */
///* WAIT_MODE_SUPPORT */
///* WAIT_MODE_ENTER_DELAY_CYCLES = 3 */
///* WAIT_MODE_EXIT_DELAY_CYCLES  = 34 */
//#define __SAM4S16B__
//
///* ASF required defines */
//#define ARM_MATH_CM4                   (true)
//#define CONFIG_SYSCLK_SOURCE           (SYSCLK_SRC_PLLACK)
//#define CONFIG_SYSCLK_PRES             (SYSCLK_PRES_1)
//#define CONFIG_PLL0_SOURCE             (PLL_SRC_MAINCK_XTAL)
//#define CONFIG_PLL0_MUL                (41)
//#define CONFIG_PLL0_DIV                (4)
//#define BOARD                          (USER_BOARD)
//#define BOARD_FREQ_SLCK_XTAL           (32768)
//#define BOARD_FREQ_SLCK_BYPASS         (32768)
//#define BOARD_FREQ_MAINCK_XTAL         (12000000)
//#define BOARD_FREQ_MAINCK_BYPASS       (12000000)
//#define BOARD_OSC_STARTUP_US           (2000)
//#define BOARD_MCK                      (123000000)
//#define TRACE_LEVEL                    (0)
//#define TRACE_LEVEL                    (0)

/******************************************************
 *  Wi-Fi Options
 ******************************************************/

/*  GPIO pins are used to bootstrap Wi-Fi to SDIO or gSPI mode */
#define WICED_WIFI_USE_GPIO_FOR_BOOTSTRAP_0
#define WICED_WIFI_USE_GPIO_FOR_BOOTSTRAP_1

/*  Wi-Fi GPIO0 pin is used for out-of-band interrupt */
#define WICED_WIFI_OOB_IRQ_GPIO_PIN  ( 0 )

/*  Wi-Fi power pin is present */
#define WICED_USE_WIFI_POWER_PIN

/*  Wi-Fi reset pin is present */
#define WICED_USE_WIFI_RESET_PIN

/*  OTA */
#define PLATFORM_HAS_OTA

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

#ifdef __cplusplus
} /* extern "C" */
#endif

