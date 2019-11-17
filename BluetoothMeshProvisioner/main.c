/***************************************************************************//**
 * @file   main.c
 * @brief  BT Mesh provisioner example for SDK v1.4
 *
 *  Simple provisioner example that can be dropped on top of the soc-btmesh-light example, by replacing
 *  the main.c and app.c with these files.
 *
 *  Additional changes needed:
 *  - Configuration Client model needs to be added into the DCD
 *  - adjust following parameters in the memory configuration (in the DCD editor)
 *     - Max Provisioned Devices,
 *     - Max Provisioned Device Netkeys
 *     - Max Foundation Client Cmds
 *     Default value for these is zero. Must use non-zero values to enable provisioning and configuration
 *     of devices.
 *  - After modifying the DCD and the memory config, remember to press Generate button to re-generate the dcd.c source
 *  - If need to control the nodes via provisioner, the relative model should be added, for e.g. Generic OnOff Client
 *  - If need to subscribe go some groups, the relative model should be added, for e.g. Generic OnOff Server
 *
 *  This example can recognize the generic on/off and light lightness models used in the
 *  lighting demo and configure the switch and light nodes so that light control works (on/off and dimming commands).
 *
 *  Configuration of vendor models is optional and it is disabled by default. To enable vendor model
 *  config symbol CONFIGURE_VENDOR_MODEL must be defined (see below)
 *
 *  Known issues and limitations:
 *   - this is an initial provisioner example code with limited testing and features
 *   - code cleanup and better error handling TBD.
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

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "board_features.h"
#include "retargetserial.h"

/* Bluetooth stack headers */
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

/* Libraries containing default Gecko configuration values */
#include <em_gpio.h>

/* Device initialization header */
#include "hal-config.h"

/* Display Interface header */
#include "display_interface.h"

/* LED driver with support for PWM dimming */
#include "led_driver.h"

/* Application code */
#include "app.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif

/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup app
 * @{
 **************************************************************************************************/

bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

/// Maximum number of simultaneous Bluetooth connections
#define MAX_CONNECTIONS 2
#define BUFFER_SIZE 300
/// Heap for Bluetooth stack
uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

/// Bluetooth advertisement set configuration
///
/// At minimum the following is required:
/// * One advertisement set for Bluetooth LE stack (handle number 0)
/// * One advertisement set for Mesh data (handle number 1)
/// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
/// * One advertisement set for Mesh unprovisioned URI (handle number 3)
/// * N advertisement sets for Mesh GATT service advertisements
/// (one for each network key, handle numbers 4 .. N+3)
///
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)


/// Priorities for bluetooth link layer operations
static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

/// Bluetooth stack configuration
const gecko_configuration_t config =
{
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};


/* Include files for usart*/
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_chip.h"

/***************************************************************************//**
 * Button initialization. Configure pushbuttons PB0, PB1 as inputs.
 ******************************************************************************/
static void button_init(void)
{
  // configure pushbutton PB0 and PB1 as inputs, with pull-up enabled
  GPIO_PinModeSet(BSP_BUTTON0_PORT, BSP_BUTTON0_PIN, gpioModeInputPull, 1);
  GPIO_PinModeSet(BSP_BUTTON1_PORT, BSP_BUTTON1_PIN, gpioModeInputPull, 1);
}

/***************************************************************************//**
 * Main function.
 ******************************************************************************/
static void uart_init(void)
{

	  /* Configure GPIO pins */
	  CMU_ClockEnable(cmuClock_GPIO, true);
	  /* To avoid false start, configure output as high */
	  GPIO_PinModeSet(gpioPortF, 4, gpioModePushPull, 1);//P30 of the main board
	  GPIO_PinModeSet(gpioPortF, 5, gpioModeInputPull, 1);//P32 of the main board

	  USART_TypeDef           *usart_2 = USART2;
	  USART_InitAsync_TypeDef init_2   = USART_INITASYNC_DEFAULT;

	  /* Enable DK RS232/UART switch */
	  RETARGET_PERIPHERAL_ENABLE();

	  CMU_ClockEnable(cmuClock_USART2, true);

	  /* Configure USART for basic async operation */
	  init_2.enable = usartDisable;
	  USART_InitAsync(usart_2, &init_2);


	  /* Enable pins at correct UART/USART location. */
	  usart_2->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
	  usart_2->ROUTELOC0 = (usart_2->ROUTELOC0
	                      & ~(_USART_ROUTELOC0_TXLOC_MASK
	                          | _USART_ROUTELOC0_RXLOC_MASK) )
	                     | (17 << _USART_ROUTELOC0_TXLOC_SHIFT)
	                     | (17 << _USART_ROUTELOC0_RXLOC_SHIFT);

	  /* Clear previous RX interrupts */
	  USART_IntClear(USART2, USART_IF_RXDATAV);
	  NVIC_ClearPendingIRQ(USART2_RX_IRQn);

	  /* Enable RX interrupts */
	  USART_IntEnable(USART2, USART_IF_RXDATAV);
	  NVIC_EnableIRQ(USART2_RX_IRQn);

	  /* Clear previous RX interrupts */
	  USART_IntClear(USART2, USART_IEN_TXC);
	  NVIC_ClearPendingIRQ(USART2_TX_IRQn);

	  /* Enable RX interrupts */
	  USART_IntEnable(USART2, USART_IEN_TXC);
	  NVIC_EnableIRQ(USART2_TX_IRQn);

	  /* Finally enable it */
	  USART_Enable(usart_2, usartEnable);
}


int main(void)
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);
  gecko_bgapi_classes_init();

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

  RETARGET_SerialInit();

  /* initialize LEDs and buttons. Note: some radio boards share the same GPIO for button & LED.
   * Initialization is done in this order so that default configuration will be "button" for those
   * radio boards with shared pins. LEDS_init() is called later as needed to (re)initialize the LEDs
   * */
  // don't initialize the LEDs, otherwise the usart2 for ESP32 communication will not work.
//  LEDS_init();
  button_init();
  uart_init();

  // Display Interface initialization
  DI_Init();

  while (1) {
	setCallBack();
    struct gecko_cmd_packet *evt = gecko_wait_event();
    bool pass = mesh_bgapi_listener(evt);
    if (pass) {
      handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
    }
  }
}
