/***************************************************************************//**
 * @file
 * @brief Defines for LED routing location register
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

#ifndef LED_ROUTELOC_H
#define LED_ROUTELOC_H

/* radio board type ID is defined here */
#include "ble-configuration.h"

/* xG12 or xG13 radio boards with LEDS mapped to pins PF4, PF5 */
#if (EMBER_AF_BOARD_TYPE == BRD4104A)  \
  || (EMBER_AF_BOARD_TYPE == BRD4305A) \
  || (EMBER_AF_BOARD_TYPE == BRD4305C) \
  || (EMBER_AF_BOARD_TYPE == BRD4158A) \
  || (EMBER_AF_BOARD_TYPE == BRD4159A) \
  || (EMBER_AF_BOARD_TYPE == BRD4167A) \
  || (EMBER_AF_BOARD_TYPE == BRD4168A) \
  || (EMBER_AF_BOARD_TYPE == BRD4305D) \
  || (EMBER_AF_BOARD_TYPE == BRD4305E) \
  || (EMBER_AF_BOARD_TYPE == BRD4103A) \
  || (EMBER_AF_BOARD_TYPE == BRD4161A) \
  || (EMBER_AF_BOARD_TYPE == BRD4162A) \
  || (EMBER_AF_BOARD_TYPE == BRD4163A) \
  || (EMBER_AF_BOARD_TYPE == BRD4164A) \
  || (EMBER_AF_BOARD_TYPE == BRD4170A) \
  || (EMBER_AF_BOARD_TYPE == BRD4172A) \
  || (EMBER_AF_BOARD_TYPE == BRD4172B) \
  || (EMBER_AF_BOARD_TYPE == BRD4173A) \
  || (EMBER_AF_BOARD_TYPE == BRD4174A) \
  || (EMBER_AF_BOARD_TYPE == BRD4174B) \
  || (EMBER_AF_BOARD_TYPE == BRD4175A)

#define LED0_ROUTELOC   TIMER_ROUTELOC0_CC0LOC_LOC28
#define LED1_ROUTELOC   TIMER_ROUTELOC0_CC1LOC_LOC28

#endif

/* xG12 or xG13 radio boards with LEDS mapped to pins PF6, PF7 */
#if (EMBER_AF_BOARD_TYPE == BRD4306A)  \
  || (EMBER_AF_BOARD_TYPE == BRD4306B) \
  || (EMBER_AF_BOARD_TYPE == BRD4306C) \
  || (EMBER_AF_BOARD_TYPE == BRD4306D) \
  || (EMBER_AF_BOARD_TYPE == BRD4304A)

#define LED0_ROUTELOC   TIMER_ROUTELOC0_CC0LOC_LOC30
#define LED1_ROUTELOC   TIMER_ROUTELOC0_CC1LOC_LOC30
#endif

/* xG12 or xG13 radio boards with LEDS mapped to pins PD10, PD11 */
#if (EMBER_AF_BOARD_TYPE == BRD4165A) \
  || (EMBER_AF_BOARD_TYPE == BRD4165B)

#define LED0_ROUTELOC   TIMER_ROUTELOC0_CC0LOC_LOC18
#define LED1_ROUTELOC   TIMER_ROUTELOC0_CC1LOC_LOC18
#endif

/* Thunderboard Sense 2 board with one LED mapped to pin PD9, second LED not mapped as TIMER0 could not be mapped to pin PD8 */
#if (EMBER_AF_BOARD_TYPE == BRD4166A)

#define LED0_ROUTELOC   TIMER_ROUTELOC0_CC0LOC_LOC17
#define LED1_ROUTELOC   TIMER_ROUTELOC0_CC1LOC_DEFAULT
#endif

#endif /* LED_ROUTELOC_H */
