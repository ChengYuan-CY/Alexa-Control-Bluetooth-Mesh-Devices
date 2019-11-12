/***************************************************************************//**
 * @file
 * @brief Displays text on the LCD
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************************
   Public Function Declarations
***************************************************************************************************/

/***********************************************************************************************//**
 *  \brief  Initializes the graphics stack
 *  \param[in]  header  Header Text on display
 **************************************************************************************************/
void graphInit(char* header);

/***********************************************************************************************//**
 *  \brief  display a string on the LCD center aligned
 *  \param[in]  string  String to be displayed
 **************************************************************************************************/
void graphWriteString(char *string);

#ifdef __cplusplus
}
#endif

#endif /* GRAHPHICS_H */
