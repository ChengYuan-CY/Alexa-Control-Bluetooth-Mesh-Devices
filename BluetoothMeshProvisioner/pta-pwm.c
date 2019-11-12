/*-----------------------------------------------------------------------------*/
// pta-pwm.c
//
// Companion extension C source file for AN1017/AN1017-NDA Rev 1.0 Section 5.5
//
// Section/Figure references in comments correlate to AN1017/AN1017-NDA
// Sections/Figures
//
// Author: Silicon Laboratories Inc.
// Version: 1.0.0
//
// (C) Copyright 2017 Silicon Laboratories, http://www.silabs.com
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
//-----------------------------------------------------------------------------

/*
- Add near top of file:
*/
#include "hal-config.h"
#include <em_cmu.h>

// bring in EFR32 PRS definitions
#include "platform/emlib/inc/em_prs.h"

/*---------------------------------------------------------------------------//
5.5.3.2 Multi-EFR32 Radio

For multi-EFR32 radio applications, the shared REQUEST signal is used to
arbitrate which EFR32 radio has access to the 2.4GHz ISM band when GRANT is
asserted from the Wi-Fi/PTA.  If the PWM signal is applied to the shared
REQUEST pin, other EFR32 radios interpret the REQUEST as busy when it actually
may just be forcing Wi-Fi TX idle to allow any of the IoT radios to detect an
in-coming packet.  The shared REQUEST and PWM extension features can both be
accommodated by separating the shared REQUEST and, if enabled, shared PRIORITY
signals from the REQUEST||PWM and PRIORITY||PWM signals.

In the example below, the PTA coexistence configuration for zigbee, Thread, and
BLE radio can be the same as the multi-EFR32 radio with typical 3-Wire PTA
example shown in Figure XX and in example #3 of Section 4.2.1.  However, only
one radio needs to implement the PWM and OR the PWM signal with REQUEST and
PRIORITY.  In the example below, the zigbee radio implements the PWM feature
and the Thread and BLE radios implement multi-EFR32 coexistence exactly as
before.
 
The PWM implementation need only be executed on the EFR32 directly driving the
Wi-Fi/PTA device and, except for possible different GPIO and PRS channel
selection, that implementation is identical to the single-EFR32 radio case
described in previous section.

The multi-EFR32 radio case with PWM extension feature can be evaluated using
the SLWSTK-COEXBP EVB by:
1. Implementing the multi-EFR32 coexistence-configuration plugin as before on
   CoEx1, CoEx2, and CoEx3 (if needed)
2. Implementing the PWM feature on the EFR32 radio installed in CoEx1 position
  - Drive REQUEST||PWM to PC10, which can be driven by the following PRS
    channels/locations: CH0/LOC12, CH9/LOC10, CH10/LOC4, and CH11/LOC3
  - Drive PRIORITY||PWM to PD12, which can be driven by the following PRS
    channels/locations: CH3/LOC11, CH4/LOC3, CH5/LOC2, and CH6/LOC14
    Note: For GW, drive PRIORITY||PWM to PC11
    channels/locations: CH0/LOC13, CH9/LOC11, CH10/LOC5, and CH11/LOC4

    Note: PC8 and PC9 is the back-channel REQUEST and PPIORITY signals

3. Change SLWSTK-COEXBP J18 and J20 jumpers to unselect REQ_PWM and PRI_PWM
  - For multi-EFR32 radio implementing PWM using typical 3-Wire PTA, REQUEST
    (active-high), PRIORITY (active-high), and GRANT (active-low)
//---------------------------------------------------------------------------*/

// PTA PWM defaults:
#define PWM_PERIOD_DEFAULT 23398 // Period=39ms
#define PWM_PULSE_DEFAULT 4680 // Duty Cycle=20% (7.8ms)

// Select PRS[i] and PRS[j]
#ifdef MIJIA_GW
// setup PRS[i] for TIMER1 output to PRS[i+1] ORPREV
// setup PRS[i+1] for REQUEST PC8 input and REQUEST||TIMER1 PC10 output
#define REQ_PRS_I 8
#define REQ_PRS_Ip1 (REQ_PRS_I + 1)
#define REQ_IN_PORT gpioPortC
#define REQ_IN_PIN 8
#define REQ_IN_INT 8
#define REQ_PRS_SOURCE PRS_CH_CTRL_SOURCESEL_GPIOH
#define REQ_PRS_SIGNAL PRS_CH_CTRL_SIGSEL_GPIOPIN8
#define REQ_OUT_PORT gpioPortC
#define REQ_OUT_PIN 10
#define REQ_PRS_ROUTELOC_REG ROUTELOC2
#define REQ_PRS_ROUTELOC_MASK ~(0x3FUL << 8)
#define REQ_PRS_ROUTELOC_VALUE PRS_ROUTELOC2_CH9LOC_LOC15
#define REQ_PRS_ROUTEPEN PRS_ROUTEPEN_CH9PEN
// setup PRS[j] for TIMER1 output to PRS[j+1] ORPREV
// setup PRS[j+1] for PRIORITY PC9 input and PRIORITY||TIMER1 PC11 output
#define PRI_PRS_J 10
#define PRI_PRS_Jp1 (PRI_PRS_J + 1)
#define PRI_IN_PORT gpioPortC
#define PRI_IN_PIN 9
#define PRI_IN_INT 9
#define PRI_PRS_SOURCE PRS_CH_CTRL_SOURCESEL_GPIOH
#define PRI_PRS_SIGNAL PRS_CH_CTRL_SIGSEL_GPIOPIN9
#define PRI_OUT_PORT gpioPortC
#define PRI_OUT_PIN 11
#define PRI_PRS_ROUTELOC_REG ROUTELOC2
#define PRI_PRS_ROUTELOC_MASK ~(0x3FUL << 24)
#define PRI_PRS_ROUTELOC_VALUE PRS_ROUTELOC2_CH11LOC_LOC4
#define PRI_PRS_ROUTEPEN PRS_ROUTEPEN_CH11PEN
#else
// setup PRS[i] for TIMER1 output to PRS[i+1] ORPREV
// setup PRS[i+1] for REQUEST PC11 input and REQUEST||TIMER1 PC10 output
#define REQ_PRS_I 8
#define REQ_PRS_Ip1 (REQ_PRS_I + 1)
#define REQ_IN_PORT gpioPortC
#define REQ_IN_PIN 11
#define REQ_IN_INT 11
#define REQ_PRS_SOURCE PRS_CH_CTRL_SOURCESEL_GPIOH
#define REQ_PRS_SIGNAL PRS_CH_CTRL_SIGSEL_GPIOPIN11
#define REQ_OUT_PORT gpioPortC
#define REQ_OUT_PIN 10
#define REQ_PRS_ROUTELOC_REG ROUTELOC2
#define REQ_PRS_ROUTELOC_MASK ~(0x3FUL << 8)
#define REQ_PRS_ROUTELOC_VALUE PRS_ROUTELOC2_CH9LOC_LOC15
#define REQ_PRS_ROUTEPEN PRS_ROUTEPEN_CH9PEN
// setup PRS[j] for TIMER1 output to PRS[j+1] ORPREV
// setup PRS[j+1] for PRIORITY PC9 input and PRIORITY||TIMER1 PD12 output
#define PRI_PRS_J 3
#define PRI_PRS_Jp1 (PRI_PRS_J + 1)
#define PRI_IN_PORT gpioPortC
#define PRI_IN_PIN 9
#define PRI_IN_INT 9
#define PRI_PRS_SOURCE PRS_CH_CTRL_SOURCESEL_GPIOH
#define PRI_PRS_SIGNAL PRS_CH_CTRL_SIGSEL_GPIOPIN9
#define PRI_OUT_PORT gpioPortD
#define PRI_OUT_PIN 12
#define PRI_PRS_ROUTELOC_REG ROUTELOC1
#define PRI_PRS_ROUTELOC_MASK ~(0x3FUL << 0)
#define PRI_PRS_ROUTELOC_VALUE PRS_ROUTELOC1_CH4LOC_LOC3
#define PRI_PRS_ROUTEPEN PRS_ROUTEPEN_CH4PEN
#endif

void ptaMinimumRxAirTime(void)
{
// To address coexistence with ~100% WiFi TX/RX activity
// it is necessary to open-up periodic WiFi TX inactive time windows, which
// allows the coex BTmesh an opportunity to receive a weak remote BTmesg
// message and resends

// Following code accomplishes this periodic WiFi TX inactivity by asserting
// REQUEST at high PRIORITY from timer1 PWM ORed with normal PTA
// REQUEST/PRIORITY activity:

// 1. Enables TIMER1 to create a PWM output
//
// 2. Maps TIMER1 PWM output to PRS I input
// 3. Maps REQUEST pin selected on Coexistence Configuration plugin to PRS I+1
//    input
// 4. Maps PRS I output to ORPREV input on PRS I+1
// 5. Maps PRS I+1 output to EVB desired REQUEST||PWM GPIO pin and enables GPIO
//    output
//
// 6. Maps TIMER1 PWM output to PRS J input
// 7. Maps PRIORITY pin selected on Coexistence Configuration plugin to PRS J+1
//    input
// 8. Maps PRS J output to ORPREV input on PRS J+1
// 9. Maps PRS J+1 output to EVB desired PRIORITY||PWM GPIO pin and enables
//    GPIO output
  
// TIMER1 setup ---------------------------------------------------------------
    CMU_ClockEnable(cmuClock_TIMER1, true); // turn on clock to TIMER1
    TIMER1->CMD = 2; // stop timer1
    
    // Setup TIMER1 for PWM, defined pulse with defined period
    // coist at reload/start, prescale32, prescaled HFPERFCLK, run in debug,
    // count-up
    TIMER1->CTRL = 0x26000040UL;
    // period => CEIL(600kHz*PERIOD-1)
    TIMER1->TOP = TIMER1->TOPB = PWM_PERIOD_DEFAULT;
    // reset count
    TIMER1->CNT = 0;
    // set PRS to track CC out level, set on compare, clear on overflow, clear
    // on start, invert, and PWM
    TIMER1->CC[0].CTRL = 0x10000B07;
    // pulse => PWM_PERIOD - CEIL(600kHz*PWM_PULSE-1)
    TIMER1->CC[0].CCV = TIMER1->CC[0].CCVB = PWM_PERIOD_DEFAULT - PWM_PULSE_DEFAULT;

    TIMER1->CMD = 1; // start timer1
    
// REQUEST setup --------------------------------------------------------------
    CMU_ClockEnable(cmuClock_PRS, true); // enable clock to PRS
    
    // Setup PRS[i] input as PWM from TIMER1 CC0
    PRS_SourceAsyncSignalSet(REQ_PRS_I, PRS_CH_CTRL_SOURCESEL_TIMER1,
                                        PRS_CH_CTRL_SIGSEL_TIMER1CC0);
    
    // Setup PRS[i+1] input as REQUEST GPIO input(map port/pin to PRS GPIOPINx)
    GPIO_ExtIntConfig(REQ_IN_PORT, REQ_IN_PIN, REQ_IN_INT, false, false,false);
    PRS_SourceAsyncSignalSet(REQ_PRS_Ip1, REQ_PRS_SOURCE, REQ_PRS_SIGNAL);

#ifdef PTA_MINIMUM_RX_AIR_TIME_DEFAULT_ENABLED
    // Enable ORPREV
    PRS->CH[REQ_PRS_Ip1].CTRL |= PRS_CH_CTRL_ORPREV;
#else
    // make sure ORPREV is disabled
    PRS->CH[REQ_PRS_Ip1].CTRL &= ~PRS_CH_CTRL_ORPREV;
#endif

    // Enable PRS[i+1] to REQUEST||PWM pin
    // enable REQUEST||PWM output pin with initial value of 0
    GPIO_PinModeSet(REQ_OUT_PORT, REQ_OUT_PIN, gpioModePushPull, 0);

    // Route PRS CH/LOC to REQUEST||PWM GPIO output
    PRS->REQ_PRS_ROUTELOC_REG = (PRS->REQ_PRS_ROUTELOC_REG
                                                       & REQ_PRS_ROUTELOC_MASK)
                                                      | REQ_PRS_ROUTELOC_VALUE;
    PRS->ROUTEPEN |= REQ_PRS_ROUTEPEN;
    
// PRIORITY setup -------------------------------------------------------------
    // Setup PRS[j] input as PWM from TIMER1 CC0
    PRS_SourceAsyncSignalSet(PRI_PRS_J, PRS_CH_CTRL_SOURCESEL_TIMER1,
                                        PRS_CH_CTRL_SIGSEL_TIMER1CC0);

    //Setup PRS[j+1] input as PRIORITY GPIO input(map port/pin to PRS GPIOPINx)
    GPIO_ExtIntConfig(PRI_IN_PORT, PRI_IN_PIN, PRI_IN_INT, false, false,false);
    PRS_SourceAsyncSignalSet(PRI_PRS_Jp1, PRI_PRS_SOURCE, PRI_PRS_SIGNAL);
    
#ifdef PTA_MINIMUM_RX_AIR_TIME_DEFAULT_ENABLED
    // Enable ORPREV
    PRS->CH[PRI_PRS_Jp1].CTRL |= PRS_CH_CTRL_ORPREV;
#else
    // make sure ORPREV is disabled
    PRS->CH[PRI_PRS_Jp1].CTRL &= ~PRS_CH_CTRL_ORPREV;
#endif

    // Enable PRS[j+1] to PRIORITY||PWM pin
    // enable PRIORITY||PWM output pin with initial value of 0
    GPIO_PinModeSet(PRI_OUT_PORT, PRI_OUT_PIN, gpioModePushPull, 0);

    // Route PRS CH/LOC to PRIORITY||PWM GPIO output
    PRS->PRI_PRS_ROUTELOC_REG = (PRS->PRI_PRS_ROUTELOC_REG
                                                       & PRI_PRS_ROUTELOC_MASK)
                                                      | PRI_PRS_ROUTELOC_VALUE;
    PRS->ROUTEPEN |= PRI_PRS_ROUTEPEN;
}

// Get PWM period and duty cycle from actual TIMER1 configuration and PRS[i+1]
// ORPREV
void getPwmState(uint8_t *pwmPeriod_ms, uint8_t *pwmPulse_DC)
{
  uint16_t pwmPeriod;
  uint16_t pwmPulse;

  pwmPeriod = TIMER1->TOPB;
  *pwmPeriod_ms = (uint8_t)(((uint32_t)pwmPeriod + 1) * 1000 / (uint32_t)600000);
  pwmPulse = ((PRS->CH[REQ_PRS_Ip1].CTRL & PRS_CH_CTRL_ORPREV)
                 == PRS_CH_CTRL_ORPREV) ? (pwmPeriod - TIMER1->CC[0].CCVB) : 0;
  *pwmPulse_DC = pwmPeriod ? ((uint8_t)(((uint32_t)pwmPulse + 1) * 100
                                             / ((uint32_t)pwmPeriod + 1))) : 0;
}

// Set PWM period (5ms to 109ms in ms resolution)
// and duty cycle% (0=DISABLE, 1% to 95%)
// And setup TIMER1 and PRS[i+1] and PRS[j+1] ORPREV as requested
int setPwmState(uint8_t pwmPeriod_ms, uint8_t pwmPulse_DC)
{
  uint16_t pwmPeriod;
  uint16_t pwmPulse;

  // confirm period and pulse are valid (period 5ms to 109ms, and DC 0% to 95%)
  if ((pwmPeriod_ms < 5) || (pwmPeriod_ms > 109) || (pwmPulse_DC > 95))
    return -1; // report error

  else {
    pwmPeriod = (uint16_t)((uint32_t)pwmPeriod_ms * (uint32_t)600000 / 1000 - 1);
    pwmPulse = (uint16_t)((uint32_t)pwmPulse_DC * ((uint32_t)pwmPeriod + 1) / 100 - 1);
    
    // Disable ORPREV to both channels
    PRS->CH[REQ_PRS_Ip1].CTRL &= ~PRS_CH_CTRL_ORPREV;
    PRS->CH[PRI_PRS_Jp1].CTRL &= ~PRS_CH_CTRL_ORPREV;

    // update TIMER1 period/pulse
    TIMER1->CMD = 2; // stop timer1
    // period => CEIL(600kHz*PERIOD-1)
    TIMER1->TOP = TIMER1->TOPB = pwmPeriod;
    // reset count
    TIMER1->CNT = 0;
    // pulse => PWM_PERIOD - CEIL(600kHz*PWM_PULSE-1)
    TIMER1->CC[0].CCV = TIMER1->CC[0].CCVB = pwmPeriod - pwmPulse;
    TIMER1->CMD = 1; // start timer1
    
    // Re-enable ORPREV to both channels? (leave disabled if DC=0)
    if (pwmPulse_DC) {
      PRS->CH[REQ_PRS_Ip1].CTRL |= PRS_CH_CTRL_ORPREV;
      PRS->CH[PRI_PRS_Jp1].CTRL |= PRS_CH_CTRL_ORPREV;
    }
  }

  return 0;
}
