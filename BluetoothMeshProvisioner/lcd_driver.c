/***************************************************************************//**
 * @file
 * @brief LCD driver implementation file
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

#include <stdio.h>
#include <string.h>
#include "graphics.h"
#include "lcd_driver.h"

#if (HAL_SPIDISPLAY_ENABLE == 1)

/***********************************************************************************************//**
 * @addtogroup disp_interface
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup lcd_driver
 * @{
 **************************************************************************************************/

/// 2D array for storing the LCD content
static char LCD_data[LCD_ROW_MAX][LCD_ROW_LEN];

/***************************************************************************//**
 * Call a callback function at the given frequency.
 *
 * @param[in] pFunction  Pointer to function that should be called at the
 *                       given frequency.
 * @param[in] argument   Argument to be given to the function.
 * @param[in] frequency  Frequency at which to call function at.
 *
 * @return  Status code of the operation.
 *
 * @note This is needed by the LCD driver
 ******************************************************************************/
int rtcIntCallbackRegister(void (*pFunction)(void*),
                           void* argument,
                           unsigned int frequency)
{
  return 0;
}

/*******************************************************************************
 * LCD initialization, called once at startup.
 ******************************************************************************/
void LCD_init(void)
{
  memset(&LCD_data, 0, sizeof(LCD_data));

  graphInit("SILICON LABORATORIES\nAlexa BTmesh Bridge\n\n");

  LCD_write("initializing", LCD_ROW_STATUS);
}

/*******************************************************************************
 * This function is used to write one line in the LCD.
 *
 * @param[in] str  Pointer to string which is displayed in the specified row.
 * @param[in] row  Selects which line of LCD display is written,
 *                 possible values are defined as LCD_ROW_xxx.
 ******************************************************************************/
void LCD_write(char *str, uint8 row)
{
  char LCD_message[LCD_ROW_MAX * LCD_ROW_LEN];
  char *pRow;
  int i;

  if (row > LCD_ROW_MAX) {
    return;
  }

  pRow  = &(LCD_data[row - 1][0]);

  sprintf(pRow, str);

  if (row == LCD_ROW_LIGHTNESS) {
    strcat(pRow, "%");
  }

  LCD_message[0] = 0;

  for (i = 0; i < LCD_ROW_MAX; i++) {
    pRow  = &(LCD_data[i][0]);
    strcat(LCD_message, pRow);
    strcat(LCD_message, "\n"); // add newline at end of reach row
  }

  graphWriteString(LCD_message);
}

/** @} (end addtogroup lcd_driver) */
/** @} (end addtogroup disp_interface) */

#endif /* HAL_SPIDISPLAY_ENABLE */
