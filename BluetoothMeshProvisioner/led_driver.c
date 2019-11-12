/***************************************************************************//**
 * @file
 * @brief Led driver for managing led state
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

#include "led_driver.h"

#include "em_timer.h"
#include "em_cmu.h"

#include "hal-config-board.h"

#include "native_gecko.h"

#include "led_control.h"

/***********************************************************************************************//**
 * @addtogroup LED
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup led_driver
 * @{
 **************************************************************************************************/

static uint16_t current_level; ///< current lightness level
static uint16_t start_level;   ///< starting level of lightness transition
static uint16_t target_level;  ///< target level of lightness transition

static uint32_t level_transtime_ticks;   ///< lightness transition time in timer ticks
static uint32_t level_transtime_elapsed; ///< time elapsed from lightness transition start
static uint8_t level_transitioning;      ///< non-zero if lightness transition is active

static uint16_t current_temperature = DEFAULT_TEMPERATURE; ///< current temperature level
static uint16_t start_temperature;  ///< starting level of temperature transition
static uint16_t target_temperature; ///< target level of temperature transition

static int16_t current_deltauv; ///< current delta UV level
static int16_t start_deltauv;   ///< starting level of delta UV transition
static int16_t target_deltauv;  ///< target level of delta UV transition

static uint32_t temp_transtime_ticks;   ///< temperature transition time in timer ticks
static uint32_t temp_transtime_elapsed; ///< time elapsed from temperature transition start
static uint8_t temp_transitioning;      ///< non-zero if temperature transition is active

/***************************************************************************//**
 * Initialization of transition timer (TIMER1).
 ******************************************************************************/
static void transition_timer_init(void)
{
  TIMER_Init_TypeDef sTimerInit = TIMER_INIT_DEFAULT;
  CMU_ClockEnable(cmuClock_TIMER1, true);
  TIMER_Init(TIMER1, &sTimerInit);

  TIMER_TopSet(TIMER1, 0x9600); //Timer period is ~1ms
  TIMER_Enable(TIMER1, 1);

  NVIC_ClearPendingIRQ(TIMER1_IRQn);
  NVIC_EnableIRQ(TIMER1_IRQn);
}

/***************************************************************************//**
 * Check if there is active transition.
 *
 * @return Zero, if there is no transition in progress.
 *         One, if there is transition in progress.
 ******************************************************************************/
static uint8_t transition_in_progress(void)
{
  return (level_transitioning || temp_transitioning);
}

/***************************************************************************//**
 * Handler for TIMER1. It manages LEDs transitions.
 ******************************************************************************/
void TIMER1_IRQHandler(void)
{
  static uint8_t sig_delay_level = 0;
  static uint8_t sig_delay_temp = 0;

  TIMER_IntClear(TIMER1, TIMER_IF_OF);

  if (!transition_in_progress()) {
    TIMER_IntDisable(TIMER1, TIMER_IEN_OF);
    return;
  }

  if (level_transitioning) {
    level_transtime_elapsed++;

    if (level_transtime_elapsed >= level_transtime_ticks) {
      // transition complete
      level_transitioning = 0;
      current_level = target_level;
      sig_delay_level = 0;
      gecko_external_signal(EXT_SIGNAL_LED_LEVEL_CHANGED);
    } else {
      // calculate current PWM duty cycle based on elapsed transition time
      if (target_level >= start_level) {
        current_level = start_level + (target_level - start_level) * (uint64_t)level_transtime_elapsed / level_transtime_ticks;
      } else {
        current_level = start_level - (start_level - target_level) * (uint64_t)level_transtime_elapsed / level_transtime_ticks;
      }
      // when transition is ongoing, generate an event to application once every 100 iterations (~ 100ms)
      // the event is used to update display status and therefore the rate should not be too high
      if (sig_delay_level++ == 0) {
        gecko_external_signal(EXT_SIGNAL_LED_LEVEL_CHANGED);
      }
      if (sig_delay_level >= 100) {
        sig_delay_level = 0;
      }
    }
  }

  if (temp_transitioning) {
    temp_transtime_elapsed++;

    if (temp_transtime_elapsed >= temp_transtime_ticks) {
      // transition complete
      temp_transitioning = 0;
      current_temperature = target_temperature;
      current_deltauv = target_deltauv;
      sig_delay_temp = 0;
      gecko_external_signal(EXT_SIGNAL_LED_TEMPERATURE_CHANGED);
    } else {
      // calculate current temperature based on elapsed transition time
      if (target_temperature >= start_temperature) {
        current_temperature = start_temperature + (target_temperature - start_temperature) * (uint64_t)temp_transtime_elapsed / temp_transtime_ticks;
      } else {
        current_temperature = start_temperature - (start_temperature - target_temperature) * (uint64_t)temp_transtime_elapsed / temp_transtime_ticks;
      }

      if (target_deltauv >= start_deltauv) {
        current_deltauv = start_deltauv + (target_deltauv - start_deltauv) * (uint64_t)temp_transtime_elapsed / temp_transtime_ticks;
      } else {
        current_deltauv = start_deltauv - (start_deltauv - target_deltauv) * (uint64_t)temp_transtime_elapsed / temp_transtime_ticks;
      }
      // when transition is ongoing, generate an event to application once every 100 iterations (~ 100ms)
      // the event is used to update display status and therefore the rate should not be too high
      if (sig_delay_temp++ == 0) {
        gecko_external_signal(EXT_SIGNAL_LED_TEMPERATURE_CHANGED);
      }
      if (sig_delay_temp >= 100) {
        sig_delay_temp = 0;
      }
    }
  }

  LEDS_SetColor(current_level, current_temperature);
}

/*******************************************************************************
 * Initialize LEDs driver.
 ******************************************************************************/
void LEDS_init(void)
{
  LEDS_control_init();
  transition_timer_init();
}

/*******************************************************************************
 * Set LED lightness level in given transition time.
 *
 * @param[in] level          Lightness level.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void LEDS_SetLevel(uint16_t level, uint32_t transition_ms)
{
  if (transition_ms == 0) {
    current_level = level;

    LEDS_SetColor(current_level, current_temperature);

    /* if a transition was in progress, cancel it */
    if (level_transitioning) {
      level_transitioning = 0;
      if (!transition_in_progress()) {
        TIMER_IntDisable(TIMER1, TIMER_IEN_OF);
      }
    }

    gecko_external_signal(EXT_SIGNAL_LED_LEVEL_CHANGED);
    return;
  }

  level_transtime_ticks = transition_ms;

  start_level = current_level;
  target_level = level;

  level_transtime_elapsed = 0;
  level_transitioning = 1;

  // enabling timer IRQ -> the PWM level is adjusted in timer interrupt
  // gradually until target level is reached.
  TIMER_IntEnable(TIMER1, TIMER_IEN_OF);

  return;
}

/*******************************************************************************
 * Set LED temperature and delta UV in given transition time.
 *
 * @param[in] temperature    Temperature of color.
 * @param[in] deltauv        Delta UV value.
 * @param[in] transition_ms  Transition time in milliseconds.
 ******************************************************************************/
void LEDS_SetTemperature(uint16_t temperature, int16_t deltauv, uint32_t transition_ms)
{
  if (temperature < MIN_TEMPERATURE) {
    temperature = MIN_TEMPERATURE;
  } else if (temperature > MAX_TEMPERATURE) {
    temperature = MAX_TEMPERATURE;
  }

  if (transition_ms == 0) {
    current_temperature = temperature;
    current_deltauv = deltauv;

    LEDS_SetColor(current_level, current_temperature);

    /* if a transition was in progress, cancel it */
    if (temp_transitioning) {
      temp_transitioning = 0;
      /* check if any transition is ongoing */
      if (!transition_in_progress()) {
        TIMER_IntDisable(TIMER1, TIMER_IEN_OF);
      }
    }

    gecko_external_signal(EXT_SIGNAL_LED_TEMPERATURE_CHANGED);
    return;
  }

  temp_transtime_ticks = transition_ms;

  start_temperature = current_temperature;
  target_temperature = temperature;

  start_deltauv = current_deltauv;
  target_deltauv = deltauv;

  temp_transtime_elapsed = 0;
  temp_transitioning = 1;

  // enabling timer IRQ -> the temperature is adjusted in timer interrupt
  // gradually until target temperature is reached.
  TIMER_IntEnable(TIMER1, TIMER_IEN_OF);

  return;
}

/*******************************************************************************
 * Set LEDs state. Possible states are defined in macros.
 *
 * @param[in] state  State to set.
 ******************************************************************************/
void LEDS_SetState(int state)
{
  static int toggle = 0;

  switch (state) {
    case LED_STATE_OFF:
      LEDS_SetLevel(MIN_BRIGHTNESS, 0);
      break;
    case LED_STATE_ON:
      LEDS_SetLevel(MAX_BRIGHTNESS, 0);
      break;
    case LED_STATE_PROV:
      if (++toggle % 2) {
        LEDS_SetLevel(MIN_BRIGHTNESS, 0);
      } else {
        LEDS_SetLevel(MAX_BRIGHTNESS, 0);
      }
      break;

    default:
      break;
  }
}

/*******************************************************************************
 * Function for retrieving actual lightness level.
 *
 * @return Actual lightness level.
 ******************************************************************************/
uint16_t LEDS_GetLevel(void)
{
  return(current_level);
}

/*******************************************************************************
 * Function for retrieving actual temperature level.
 *
 * @return Actual temperature level.
 ******************************************************************************/
uint16_t LEDS_GetTemperature(void)
{
  return(current_temperature);
}

/*******************************************************************************
 * Function for retrieving actual delta UV level.
 *
 * @return Actual delta UV level.
 ******************************************************************************/
int16_t LEDS_GetDeltaUV(void)
{
  return(current_deltauv);
}

/** @} (end addtogroup led_driver) */
/** @} (end addtogroup LED) */
