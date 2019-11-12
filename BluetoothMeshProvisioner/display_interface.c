/***************************************************************************//**
 * @file
 * @brief Display interface implementation
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
#include "bg_types.h"
#include "display_interface.h"

/***********************************************************************************************//**
 * @addtogroup disp_interface
 * @{
 **************************************************************************************************/

/// Display Interface
display_interface_t display_interface = DEFAULT_DISPLAY_INTERFACE;

/*******************************************************************************
 * Configure Display Interface.
 *
 * @param[in] init   Pointer to display initialization function.
 * @param[in] print  Pointer to display print function.
 ******************************************************************************/
void DI_Config(init_func_t init, print_func_t print)
{
  display_interface.init = init;
  display_interface.print = print;
}

/*******************************************************************************
 * Initialize Display Interface.
 ******************************************************************************/
void DI_Init(void)
{
  if (display_interface.init == NULL) {
    printf("DI_Init: Display interface is NULL.\r\n");
    return;
  }

  display_interface.init();
}

/*******************************************************************************
 * Print to Display Interface.
 *
 * @param[in] str  Pointer to string which is displayed in the specified row.
 * @param[in] row  Selects which line of display is written,
 *                 possible values are defined as DI_ROW_xxx.
 ******************************************************************************/
void DI_Print(char *str, uint8 row)
{
  if (display_interface.print == NULL) {
    printf("DI_Print: %s\r\n", str);
    return;
  }

  display_interface.print(str, row);
}

/** @} (end addtogroup disp_interface) */
