/**
 * \file
 *
 * \brief 4-wire resistive touch driver general implementation.
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

#include <assert.h>
#include <interrupt.h>
#include <pmic.h>
#include <intc.h>

#include <touch/touch.h>
#include <cpu/touch/resistive/touch.h>

#ifdef CONFIG_SOFTIRQ
# include <softirq.h>
#endif

/**
 * \ingroup touch_driver_group
 * \defgroup touch_driver_4wres_group 4-wire resistive touch driver\
 * general implementation
 *
 * This driver makes use of low level interrupts for touch detection,
 * and an ADC for touch position measurements.\n
 *
 * @{
 */

//! Driver state names.
enum touch_states {
	TOUCH_DISABLED,    //!< Driver is disabled, no sampling will occur.
	TOUCH_NOT_TOUCHED, //!< Driver enabled, but panel not touched.
	TOUCH_READING_X,   //!< Waiting for samples for X coordinate.
	TOUCH_READING_Y,   //!< Waiting for samples for Y coordinate.
	TOUCH_PROCESSING,  //!< Waiting for processing of samples.
	TOUCH_TOUCHED,     //!< Panel is currently touched.
};

//! Current driver state.
static enum touch_states touch_state;
//! Last detected touch event.
static struct touch_event touch_last_event;

//! Temporary raw sample value for X surface.
static uint16_t touch_raw_x;
//! Temporary raw sample value for Y surface.
static uint16_t touch_raw_y;

//! Calibration matrix for the driver.
static struct touch_calibration_matrix touch_matrix;
//! Current touch event handler (set to NULL if no handler should be called).
touch_event_handler_t touch_event_handler;

static void touch_priv_port_handler(void *int_data);
static void touch_priv_adc_handler(void *int_data);
static void touch_priv_process_samples(void *int_data);

/**
 * \name Definition of interrupt handlers for touch detection and reading
 * @{
 */
INTC_DEFINE_HANDLER(CONFIG_TOUCH_PORT_IRQ_ID, touch_priv_port_handler,
		CONFIG_TOUCH_PORT_INTLVL);
INTC_DEFINE_HANDLER(CONFIG_TOUCH_ADC_IRQ_ID, touch_priv_adc_handler,
		CONFIG_TOUCH_ADC_INTLVL);
//@}

/**
 * \brief Initialize the touch driver.
 *
 * Initializes the GPIO pins and ADC for touch detection and measurements, as
 * well as the driver states.\n
 * Must be called before any other function in this driver.
 *
 * \note The touch driver is left \a disabled, and must be explicitly enabled by
 * use of \ref touch_enable().
 */
void touch_init(void)
{
	// Start with no event handler.
	touch_event_handler = NULL;

	touch_priv_adc_init();
	touch_priv_port_init();

#ifdef CONFIG_SOFTIRQ
	// Trigger processing of touch measurements with soft IRQ if available.
	softirq_set_handler(SOFTIRQ_TOUCH_PROCESS, touch_priv_process_samples, NULL);
#endif
	// Reset touch driver and event states.
	touch_state = TOUCH_NOT_TOUCHED;
	touch_last_event.type = TOUCH_NO_EVENT;

	touch_disable();
}


/**
 * \brief Enable touch detection and sampling.
 *
 * Resets the driver and touch state, configures GPIO pins for touch detection
 * and enables interrupts for GPIO and the ADC.
 */
void touch_enable(void)
{
	touch_state = TOUCH_NOT_TOUCHED;
	touch_last_event.type = TOUCH_NO_EVENT;

	touch_priv_port_set_detection();

	/* Clear the ADC and GPIO interrupts' flags before enabling them,
	 * to avoid false detects or premature measurements.
	 */
	touch_priv_adc_clear_int_flag();
	touch_priv_adc_enable_int();

	touch_priv_port_clear_int_flag();
	touch_priv_port_enable_int();
}


/**
 * \brief Disable touch detection and sampling.
 *
 * Disables interrupts for the GPIO and the ADC.
 */
void touch_disable(void)
{
	/* Global interrupts must be disabled during this in case the driver
	 * is currently doing measurements.
	 */
	irqflags_t irqflags = cpu_irq_save();

	touch_priv_port_disable_int();
	touch_priv_adc_disable_int();

	cpu_irq_restore(irqflags);
}


/**
 * \brief Get last detected touch event from driver to buffer.
 *
 * \param buffer Buffer to store touch event in.
 */
void touch_get_event(struct touch_event *buffer)
{
	assert(buffer != NULL);

	// Disable interrupt to prevent data from changing during copying.
	irqflags_t irqflags = cpu_irq_save();

	*buffer = touch_last_event;

	cpu_irq_restore(irqflags);
}


/**
 * \brief Return current state of touch driver.
 *
 * \retval true  Driver is sensing a touch.
 * \retval false Driver is \a not sensing a touch.
 */
bool touch_is_touched(void)
{
	return ((touch_last_event.type == TOUCH_PRESS)
			|| (touch_last_event.type == TOUCH_MOVE));
}


/**
 * \brief Set touch event handler for driver.
 *
 * \param handler Address of new handler function.
 */
void touch_set_event_handler(touch_event_handler_t handler)
{
	// Disable interrupts until new address has been stored.
	irqflags_t irqflags = cpu_irq_save();

	touch_event_handler = handler;

	cpu_irq_restore(irqflags);
}


/**
 * \brief Get current touch event handler from driver.
 *
 * \return Address of the currently set handler function.
 */
touch_event_handler_t touch_get_event_handler(void)
{
	return (touch_event_handler);
}


/**
 * \brief Assign a calibration matrix to the driver.
 *
 * \param matrix Address of calibration matrix for the driver to copy.
 */
void touch_set_calibration_matrix(
		const struct touch_calibration_matrix *matrix)
{
	assert(matrix != NULL);

	touch_matrix = *matrix;
}

/**
 * \brief Copy calibration matrix from driver to a buffer.
 *
 * \param buffer Address of buffer to copy calibration matrix to.
 */
void touch_get_calibration_matrix(struct touch_calibration_matrix *buffer)
{
	assert(buffer != NULL);

	*buffer = touch_matrix;
}


/**
 * \brief Compute a calibration matrix.
 *
 * Computes the calibration matrix that is used to get the panel coordinates
 * from raw measurements of the touch surface.\n
 * This requires that the
 *
 * \note Reference: http://www.embedded.com/story/OEG20020529S0046
 *
 * \param points Pointer to array containing calibration points.
 * \param matrix Pointer to storage buffer for the calibration matrix.
 */
void touch_compute_calibration_matrix(
		const touch_calibration_points_t points,
		struct touch_calibration_matrix *matrix)
{
	assert(points != NULL);
	assert(matrix != NULL);

	// Local copies of touch readings for convenience.
	int32_t x_r1 = points[0].raw_x;
	int32_t y_r1 = points[0].raw_y;

	int32_t x_r2 = points[1].raw_x;
	int32_t y_r2 = points[1].raw_y;

	int32_t x_r3 = points[2].raw_x;
	int32_t y_r3 = points[2].raw_y;

	// Local copies of panel coordinates for convenience.
	int32_t x_p1 = points[0].panel_x;
	int32_t y_p1 = points[0].panel_y;

	int32_t x_p2 = points[1].panel_x;
	int32_t y_p2 = points[1].panel_y;

	int32_t x_p3 = points[2].panel_x;
	int32_t y_p3 = points[2].panel_y;

	// Compute coefficients for X calibration.
	matrix->a = ((x_p1 - x_p3) * (y_r2 - y_r3))
			- ((x_p2 - x_p3) * (y_r1 - y_r3));
	matrix->b = ((x_r1 - x_r3) * (x_p2 - x_p3))
			- ((x_p1 - x_p3) * (x_r2 - x_r3));
	matrix->c = y_r1 * ((x_r3 * x_p2) - (x_r2 * x_p3))
			+ y_r2 * ((x_r1 * x_p3) - (x_r3 * x_p1))
			+ y_r3 * ((x_r2 * x_p1) - (x_r1 * x_p2));

	// Compute coefficients for Y calibration.
	matrix->d = ((y_p1 - y_p3) * (y_r2 - y_r3))
			- ((y_p2 - y_p3) * (y_r1 - y_r3));
	matrix->e = ((x_r1 - x_r3) * (y_p2 - y_p3))
			- ((y_p1 - y_p3) * (x_r2 - x_r3));
	matrix->f = y_r1 * ((x_r3 * y_p2) - (x_r2 * y_p3))
			+ y_r2 * ((x_r1 * y_p3) - (x_r3 * y_p1))
			+ y_r3 * ((x_r2 * y_p1) - (x_r1 * y_p2));

	// Compute common denominator.
	matrix->k = ((x_r1 - x_r3) * (y_r2 - y_r3))
			- ((x_r2 - x_r3) * (y_r1 - y_r3));
}

/**
 * \brief Process touch samples.
 *
 * Uses the calibration matrix to compute touch position in panel coordinates
 * from the current raw touch measurements.\n
 * The driver and touch states are updated according to the event, and its
 * handler (if set) is called if a new event was in fact detected.
 *
 * \param int_data Not used here.
 */
void touch_priv_process_samples(void *int_data)
{
	// Assume no need to call handler for now.
	bool send_event = false;

	/* Catch divide-by-zero in case matrix is not initialized, e.g.,
	 * during calibration.
	 */
	int32_t temp_k = touch_matrix.k;
	if (temp_k == 0) {
		temp_k = 1;
	}

	/* Scale sample values down according to the oversampling to avoid
	 * overflow in computations.
	 */
	touch_raw_x >>= CONFIG_TOUCH_OVERSAMPLING;
	touch_raw_y >>= CONFIG_TOUCH_OVERSAMPLING;

	// Compute panel X coordinate of touch.
	int32_t panel_x = (touch_matrix.a * touch_raw_x)
			+ (touch_matrix.b * touch_raw_y)
			+ touch_matrix.c;
	panel_x /= temp_k;

	// Compute panel Y coordinate of touch.
	int32_t panel_y = (touch_matrix.d * touch_raw_x)
			+ (touch_matrix.e * touch_raw_y)
			+ touch_matrix.f;
	panel_y /= temp_k;

	/* GPIO pins should already be configured for touch detection.
	 * If the interrupt flag gets set immediately after clearing it, we can
	 * assume the panel is still touched.
	 */
	touch_priv_port_clear_int_flag();

	if (touch_priv_port_is_int_flag_set() == false) {
		touch_state = TOUCH_NOT_TOUCHED;

		/* If the last event indicates that the panel was touched, it
		 * has now been released.
		 */
		if ((touch_last_event.type == TOUCH_PRESS)
				|| (touch_last_event.type == TOUCH_MOVE)) {
			touch_last_event.type = TOUCH_RELEASE;

			// Call event handler if one is set.
			if (touch_event_handler != NULL) {
				touch_event_handler(&touch_last_event);
			}
		}
	} else {
		touch_state = TOUCH_TOUCHED;

		/* If the last event is non-existent or was a release, the
		 * current event is a touch.
		 */
		if ((touch_last_event.type == TOUCH_NO_EVENT)
				|| (touch_last_event.type == TOUCH_RELEASE)) {
			touch_last_event.type = TOUCH_PRESS;
			send_event = true;
		}
		/* Otherwise, if the touch panel coordinates have changed, the
		 * event is a move of the touch.
		 */
		else if ((touch_last_event.point.panel_x != panel_x)
				|| (touch_last_event.point.panel_y != panel_y)) {
			touch_last_event.type = TOUCH_MOVE;
			send_event = true;
		}

		/* If a new event was detected, update the event data and call
		 * the handler, if one has been set.
		 */
		if (send_event) {
			touch_last_event.point.raw_x = touch_raw_x;
			touch_last_event.point.raw_y = touch_raw_y;
			touch_last_event.point.panel_x = panel_x;
			touch_last_event.point.panel_y = panel_y;

			if (touch_event_handler != NULL) {
				touch_event_handler(&touch_last_event);
			}
		}
	}

	// Re-enable touch detection to trigger new measurements.
	touch_priv_port_enable_int();
}


/**
 * \brief Interrupt handler for touch detection.
 *
 * Disables itself and triggers the first touch position measurement.\n
 * The first measurements are done on the X surface of the touch panel.
 *
 * \param int_data Not used here.
 */
void touch_priv_port_handler(void *int_data)
{
	assert((touch_state == TOUCH_NOT_TOUCHED)
			|| (touch_state == TOUCH_TOUCHED));

	// Disable this interrupt until processing of touch is complete.
	touch_priv_port_disable_int();

	// Reset X and Y measurements here due to oversampling.
	touch_raw_x = 0;
	touch_raw_y = 0;

	// Update driver state and commence measurements of X.
	touch_state = TOUCH_READING_X;
	touch_priv_port_set_gradient_x();
	touch_priv_adc_set_surface_x();

	touch_priv_adc_start();
}


/**
 * \brief Interrupt handler for AD conversions.
 *
 * Handles and triggers raw measurements of position on the touch surfaces.\n
 * After measurements are completed, a soft IRQ is either raised or
 * \ref touch_priv_process_samples() called directly, depending on configuration.
 *
 * \param int_data Not used here.
 */
void touch_priv_adc_handler(void *int_data)
{
	static uint8_t sample_count = 0;

	switch (touch_state) {
		case TOUCH_READING_X:
			// Sum raw X position measurements from the ADC.
			touch_raw_x += touch_priv_adc_get_x();

			// Change state and start ADC reading of X
			// when enough samples have been taken.
			++sample_count;

			if (sample_count < (1 << CONFIG_TOUCH_OVERSAMPLING)) {
				touch_priv_adc_start();
			} else {
				sample_count = 0;

				touch_state = TOUCH_READING_Y;
				touch_priv_port_set_gradient_y();
				touch_priv_adc_set_surface_y();
				touch_priv_adc_start();
			}
			break;


		case TOUCH_READING_Y:
			// Sum raw X position measurements from the ADC.
			touch_raw_y += touch_priv_adc_get_y();

			// Change state and start ADC reading of Y
			// when enough samples have been taken.
			++sample_count;

			if (sample_count < (1 << CONFIG_TOUCH_OVERSAMPLING)) {
				touch_priv_adc_start();
			} else {
				sample_count = 0;

				touch_state = TOUCH_PROCESSING;

				// Prepare touch detection again.
				touch_priv_port_set_detection();
#ifdef CONFIG_SOFTIRQ
				softirq_raise(SOFTIRQ_TOUCH_PROCESS);
#else
				touch_priv_process_samples(NULL);
#endif
			}
			break;

		default:
			assert(0); // Always fail on illegal state.
			break;
	}
}

//@}

