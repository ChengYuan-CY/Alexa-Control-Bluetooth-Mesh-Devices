/***************************************************************************//**
 * @file
 * @brief Led driver header file
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

#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include <stdint.h>

/***********************************************************************************************//**
 * \defgroup led_driver Led Driver Interface
 * \brief Manage LED states and transitions.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup LED
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup led_driver
 * @{
 **************************************************************************************************/

/*******************************************************************************
 *  State of the LEDs is updated by calling LED_set_state().
 *  The new state is passed as parameter, possible values are defined below.
 ******************************************************************************/
#define LED_STATE_OFF    0   /**< light off (both LEDs turned off) */
#define LED_STATE_ON     1   /**< light on (both LEDs turned on) */
#define LED_STATE_PROV   2   /**< provisioning (LEDs blinking) */

/* Temperature parameters */
#define DEFAULT_TEMPERATURE  6500   /**< Default temperature of LED */
#define DEFAULT_DELTAUV      0      /**< Default delta UV of LED */

#define MIN_TEMPERATURE      0x0320 /**< Minimum temperature of LED */
#define MAX_TEMPERATURE      0x4e20 /**< Maximum temperature of LED */

/// This signal is used to notify main application when LED lightness is changed
#define EXT_SIGNAL_LED_LEVEL_CHANGED        0x1
/// This signal is used to notify main application when LED temperature is changed
#define EXT_SIGNAL_LED_TEMPERATURE_CHANGED  0x2

/***************************************************************************//**
 * Initialize LEDs driver.
 ******************************************************************************/
void LEDS_init(void);

/***************************************************************************//**
 * Set LED lightness level in given transition time.
 *
 * @param[in] level          Lightness level.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void LEDS_SetLevel(uint16_t level, uint32_t transition_ms);

/***************************************************************************//**
 * Set LED temperature and delta UV in given transition time.
 *
 * @param[in] temperature    Temperature of color.
 * @param[in] deltauv        Delta UV value.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void LEDS_SetTemperature(uint16_t temperature, int16_t deltauv, uint32_t transition_ms);

/***************************************************************************//**
 * Set LEDs state. Possible states are defined in macros.
 *
 * @param[in] state  State to set.
 ******************************************************************************/
void LEDS_SetState(int state);

/***************************************************************************//**
 * Function for retrieving actual lightness level.
 *
 * @return Actual lightness level.
 ******************************************************************************/
uint16_t LEDS_GetLevel(void);

/***************************************************************************//**
 * Function for retrieving actual temperature level.
 *
 * @return Actual temperature level.
 ******************************************************************************/
uint16_t LEDS_GetTemperature(void);

/***************************************************************************//**
 * Function for retrieving actual delta UV level.
 *
 * @return Actual delta UV level.
 ******************************************************************************/
int16_t LEDS_GetDeltaUV(void);

/** @} (end addtogroup led_driver) */
/** @} (end addtogroup LED) */

#endif /* LED_DRIVER_H */
