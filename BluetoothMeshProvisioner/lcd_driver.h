/***************************************************************************//**
 * @file
 * @brief LCD driver header file
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

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include "hal-config.h"

#if (HAL_SPIDISPLAY_ENABLE == 1)

#include "bg_types.h"

/***********************************************************************************************//**
 * \defgroup lcd_driver LCD Driver
 * \brief Driver for SPI LCD Display.
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup disp_interface
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup lcd_driver
 * @{
 **************************************************************************************************/

/*******************************************************************************
 * LCD content can be updated one row at a time using function LCD_write().
 * Row number is passed as parameter, the possible values are defined below.
 ******************************************************************************/
#define LCD_ROW_NAME         1    /**< 1st row, device name */
#define LCD_ROW_STATUS       2    /**< 2nd row, node status */
#define LCD_ROW_CONNECTION   3    /**< 3rd row, connection status */

#define LCD_ROW_LIGHTNESS    5    /**< 5th row, lightness level */
#define LCD_ROW_PROV_STATUS  6    /**< 6th row, HSL hue */
#define LCD_ROW_ESP_STATUS   7    /**< 7th row, HSL saturation */
#define LCD_ROW_LOCAL_TEST   8	  /**< 8th row, local test */
#define LCD_ROW_MAX          8    /**< total number of rows used */

#define LCD_ROW_LEN         32    /**< up to 32 characters per each row */

/***************************************************************************//**
 * LCD initialization, called once at startup.
 ******************************************************************************/
void LCD_init(void);

/***************************************************************************//**
 * This function is used to write one line in the LCD.
 *
 * @param[in] str  Pointer to string which is displayed in the specified row.
 * @param[in] row  Selects which line of LCD display is written,
 *                 possible values are defined as LCD_ROW_xxx.
 ******************************************************************************/
void LCD_write(char *str, uint8 row);

/** @} (end addtogroup lcd_driver) */
/** @} (end addtogroup disp_interface) */

#endif /* HAL_SPIDISPLAY_ENABLE */

#endif /* LCD_DRIVER_H */
