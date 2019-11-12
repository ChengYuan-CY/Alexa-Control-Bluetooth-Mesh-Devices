/***************************************************************************//**
 * @file
 * @brief Led control header file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef LED_CONTROL_H
#define LED_CONTROL_H

#include <stdint.h>

/***********************************************************************************************//**
 * \defgroup led_control Led Control Interface
 * \brief Control LED hardware.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup LED
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup led_control
 * @{
 **************************************************************************************************/

#define MIN_BRIGHTNESS  0x0001 /**< min usable value for the timer CC register */
#define MAX_BRIGHTNESS  0xFFFE /**< max usable value for the timer CC register */

/***************************************************************************//**
 * Initialize LEDs hardware.
 ******************************************************************************/
void LEDS_control_init(void);

/***************************************************************************//**
 * Set LED color based on lightness and temperature.
 *
 * @param[in] level        Lightness level.
 * @param[in] temperature  Color temperature in Kelvins.
 *
 * @note On single color LEDs only lightness is changed.
 ******************************************************************************/
void LEDS_SetColor(uint16_t level, uint16_t temperature);

/** @} (end addtogroup led_control) */
/** @} (end addtogroup LED) */

#endif /* LED_CONTROL_H */
