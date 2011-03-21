/**
 * \file
 *
 * \brief General touch driver
 *
 * Copyright (C) 2009 Atmel Corporation. All rights reserved.
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 * Atmel AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef TOUCH_TOUCH_H
#define TOUCH_TOUCH_H

#include <stdint.h>

/**
 * \defgroup touch_driver_group General touch driver
 *
 * For resistive touch the driver implementation utilizes GPIO pin interrupts,
 * if needed, for touch detection and the ADC for position measurements.\n
 * If \a CONFIG_SOFTIRQ is defined, the driver will make use of the soft IRQ
 * with ID \a SOFTIRQ_TOUCH_PROCESS for processing of measurements and
 * calling the current touch event handler function.
 *
 * \pre
 * For 4-wire resistive touch with XMEGA, the driver needs the following
 * configuration symbols to be defined:
 * - \a CONFIG_TOUCH_PORT_BASE
 * - \a CONFIG_TOUCH_PORT_IRQ_ID
 * - \a CONFIG_TOUCH_PORT_INTLVL
 * - \a CONFIG_TOUCH_ADC_IRQ_ID
 * - \a CONFIG_TOUCH_ADC_INTLVL
 * - \a CONFIG_TOUCH_ADC_PRP
 * - \a CONFIG_TOUCH_XL_PIN
 * - \a CONFIG_TOUCH_XR_PIN
 * - \a CONFIG_TOUCH_YD_PIN
 * - \a CONFIG_TOUCH_YU_PIN
 * - \a CONFIG_TOUCH_OVERSAMPLING
 *
 * \note See pin configuration for details on which pins are driven low or
 * high during touch measurements.
 *
 * @{
 */

/**
 * \def CONFIG_TOUCH_PORT_BASE
 *
 * \brief Base address of port to which touch lines are connected.
 */

/**
 * \def CONFIG_TOUCH_PORT_IRQ_ID
 *
 * \brief ID of IRQ for GPIO pin interrupts.
 */

/**
 * \def CONFIG_TOUCH_PORT_INTLVL
 *
 * \brief Interrupt level for GPIO pin interrupts
 */

/**
 * \def CONFIG_TOUCH_ADC_BASE
 *
 * \brief Base address of ADC to which touch lines are connected.
 */

/**
 * \def CONFIG_TOUCH_ADC_IRQ_ID
 *
 * \brief ID of IRQ for completed AD conversions.
 */

/**
 * \def CONFIG_TOUCH_ADC_INTLVL
 *
 * \brief Interrupt level for ADC interrupts.
 */

/**
 * \name X and Y touch surface pin configuration for 4-wire sensing
 * @{
 */

/**
 * \def CONFIG_TOUCH_XL_PIN
 *
 * \brief Number of "left" pin on X surface.
 *
 * This is driven low during measurements on the X surface.
 */

/**
 * \def CONFIG_TOUCH_XR_PIN
 *
 * \brief Number of "right" pin on X surface.
 *
 * This is driven high during measurements on the X surface.
 */

/**
 * \def CONFIG_TOUCH_YD_PIN
 *
 * \brief Number of "down" pin on Y surface.
 *
 * This is driven low during measurements on the Y surface.
 */

/**
 * \def CONFIG_TOUCH_YU_PIN
 *
 * \brief Number of "up" pin on Y surface.
 *
 * This is driven low during measurements on the Y surface.
 */

//@}

/**
 * \def CONFIG_TOUCH_OVERSAMPLING
 *
 * \brief Number of touch position measurements to make.
 *
 * This number defines how many measurements, as an exponent of 2, that the
 * driver should do for each touch.\n
 * For a single sample it must be set to 0; for two samples it must be set
 * to 1; for four samples it must be set to 2, etc.\n
 *
 * \note If too high oversampling is configured, overflows may occur during
 * sampling. The maximum allowable setting depends on the maximum sample value,
 * which in turn is limited by the ADC resolution.\n The driver accumulates the
 * samples in unsigned 16-bit integers, meaning this setting should not be
 * higher than 16 - \a N_bits (number of bits in maximum sample value).
 */


//! Panel coordinate and measurement data of a single touch.
struct touch_point {
	int16_t panel_x; //!< Calibrated panel X coordinate.
	int16_t panel_y; //!< Calibrated panel Y coordinate.
	uint16_t raw_x;  //!< Raw X sample value.
	uint16_t raw_y;  //!< Raw Y sample value.
};

//! Number of calibration points needed for driver.
#define TOUCH_NR_CALIBRATION_POINTS  3

/**
 * \brief Array to hold calibration points
 *
 * Meant for use with \ref touch_compute_calibration_matrix().
 */
typedef struct touch_point
		touch_calibration_points_t[TOUCH_NR_CALIBRATION_POINTS];

/**
 * \brief Calibration matrix coefficients
 *
 * Should be computed by \ref touch_compute_calibration_matrix().
 */
struct touch_calibration_matrix {
	int32_t a;    //!< Coefficient for X panel coordinate transform.
	int32_t b;    //!< Coefficient for X panel coordinate transform.
	int32_t c;    //!< Offset for X panel coordinate.
	int32_t d;    //!< Coefficient for Y panel coordinate transform.
	int32_t e;    //!< Coefficient for Y panel coordinate transform.
	int32_t f;    //!< Offset for Y panel coordinate.
	int32_t k;    //!< Common denominator.
};


//! Enumeration of different touch event types for the \ref touch_event struct.
enum touch_event_types {
	TOUCH_NO_EVENT,
	TOUCH_PRESS,
	TOUCH_MOVE,
	TOUCH_RELEASE,
};


//! Contains touch event type and touch point data.
struct touch_event {
	enum touch_event_types type;    //!< Type of touch event.
	struct touch_point point;       //!< Touch point data for event.
};


//! Pointer to touch event handler function.
typedef void (* touch_event_handler_t)(const struct touch_event *event);


void touch_init(void);
void touch_enable(void);
void touch_disable(void);
void touch_get_event(struct touch_event *buffer);
bool touch_is_touched(void);
void touch_set_calibration_matrix(
		const struct touch_calibration_matrix *matrix);
void touch_get_calibration_matrix(struct touch_calibration_matrix *buffer);
void touch_compute_calibration_matrix(
		const touch_calibration_points_t points,
		struct touch_calibration_matrix *matrix);
void touch_set_event_handler(touch_event_handler_t handler);
touch_event_handler_t touch_get_event_handler(void);

//@}

#endif /* TOUCH_TOUCH_H */
