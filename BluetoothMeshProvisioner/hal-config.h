/***************************************************************************//**
 * @file
 * @brief HAL configuration file
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

#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#include "board_features.h"
#include "hal-config-board.h"

#if defined(FEATURE_IOEXPANDER)
#include "hal-config-ioexp.h"
#endif

#if defined(FEATURE_FEM)
#include "hal-config-fem.h"
#endif

#ifdef BSP_CLK_LFXO_CTUNE
#undef BSP_CLK_LFXO_CTUNE
#endif
#define BSP_CLK_LFXO_CTUNE                            (32)

#define HAL_EXTFLASH_FREQUENCY                        (1000000)

#define HAL_PA_ENABLE                                 (1)
#define HAL_PA_RAMP                                   (10)
#define HAL_PA_2P4_LOWPOWER                           (0)
#define HAL_PA_POWER                                  (252)
#define HAL_PA_CURVE_HEADER                            "pa_curves_efr32.h"
#ifdef FEATURE_PA_HIGH_POWER
#define HAL_PA_VOLTAGE                                (3300)
#else // FEATURE_PA_HIGH_POWER
#define HAL_PA_VOLTAGE                                (1800)
#endif // FEATURE_PA_HIGH_POWER

#define HAL_PTI_ENABLE                                (1)
#define HAL_PTI_MODE                                  (HAL_PTI_MODE_UART)
#define HAL_PTI_BAUD_RATE                             (1600000)

#define HAL_SPIDISPLAY_ENABLE                         (1)
#define HAL_SPIDISPLAY_EXTCOMIN_CALLBACK
#if defined(FEATURE_IOEXPANDER)
#define HAL_SPIDISPLAY_EXTMODE_EXTCOMIN               (0)
#else
#define HAL_SPIDISPLAY_EXTMODE_EXTCOMIN               (1)
#endif
#define HAL_SPIDISPLAY_EXTMODE_SPI                    (0)
#define HAL_SPIDISPLAY_EXTCOMIN_USE_PRS               (0)
#define HAL_SPIDISPLAY_EXTCOMIN_USE_CALLBACK          (0)
#define HAL_SPIDISPLAY_FREQUENCY                      (1000000)
#define HAL_VCOM_ENABLE                               (1)
#define HAL_I2CSENSOR_ENABLE                          (0)

#endif
