/**
 * \file
 *
 * \brief External Static Memory Interface
 *
 * Copyright (C) 2010 Atmel Corporation. All rights reserved.
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
#ifndef BUS_EBI_SRAM_H_INCLUDED
#define BUS_EBI_SRAM_H_INCLUDED

#include <chip/ebi_sram.h>

/**
 * \ingroup ebi_group
 * \defgroup ebi_sram_group External Static Memory Interface
 *
 * The External Static Memory Interface (aka SRAM) is an asynchronous
 * parallel bus used for high-speed communication with external chips.
 * The bus consists of 8, 16 or 32 data lines, a configurable number of
 * address lines, and a handful of control lines.
 *
 * Each chip on the SRAM bus is identified by a <em>chip select</em>
 * line, which is associated with a specific physical address range. Any
 * access within this range will cause the associated chip to be
 * selected.
 *
 * Since the bus is asynchronous, setting up correct bus timings is
 * essential. The correct timings for each bus transaction phase is
 * given by the data sheet of the external chip. The SRAM controller
 * must be set up to comply with these timings before any access to the
 * external device can be made.
 *
 * The configuration functions in this module operate on a
 * platform-specific <em>configuration object</em> represented by a
 * struct ebi_sram_params. Configuration of the SRAM controller can be
 * broken down into the following steps:
 *   -# Initialize the configuration object by calling
 *      ebi_sram_params_init_defaults() or ebi_sram_params_read().
 *   -# Change one or more parameters by calling the "setter" functions.
 *   -# Commit the updated configuration to hardware by calling
 *      ebi_sram_params_write().
 *
 * Although this requires a little bit more code than if the functions
 * operated directly on the hardware registers, it will be a lot more
 * efficient since the compiler will be able to combine multiple updates
 * to a register into one write. Also, in the vast majority cases, the
 * whole configuration object will live in CPU registers or as
 * constants, so while the resulting machine code may end up writing
 * predefined constants to the hardware registers, the C code will still
 * be readable and easy to change.
 *
 * \todo Insert timing diagram showing how setup, pulse and cycle values
 * relate to each other.
 *
 * @{
 */

//! \name Timing Parameters in Nanoseconds
//@{

/**
 * \brief Set timing parameters for the \em setup phase, given in
 * nanoseconds.
 *
 * \param params Configuration parameters to be altered.
 * \param cs_rd_ns Chip Select setup time for read access in
 * nanoseconds.
 * \param oe_ns Output Enable strobe setup time in nanoseconds.
 * \param cs_wr_ns Chip Select setup time for write access in
 * nanoseconds.
 * \param we_ns Write Enable strobe setup time in nanoseconds.
 */
static inline void ebi_sram_set_setup_ns(
		struct ebi_sram_params *params,
		unsigned int cs_rd_ns, unsigned int oe_ns,
		unsigned int cs_wr_ns, unsigned int we_ns)
{
	uint32_t bus_khz;

	bus_khz = ebi_sram_get_bus_hz() / 1000;
	ebi_sram_set_setup_cycles(params,
			div_ceil(bus_khz * cs_rd_ns, 1000000),
			div_ceil(bus_khz * oe_ns, 1000000),
			div_ceil(bus_khz * cs_wr_ns, 1000000),
			div_ceil(bus_khz * we_ns, 1000000));
}

/**
 * \brief Set timing parameters for the \em pulse phase, given in
 * nanoseconds.
 *
 * \param params Configuration parameters to be altered.
 * \param cs_rd_ns Chip Select pulse time for read access in
 * nanoseconds.
 * \param oe_ns Output Enable strobe pulse time in nanoseconds.
 * \param cs_wr_ns Chip Select pulse time for write access in
 * nanoseconds.
 * \param we_ns Write Enable strobe pulse time in nanoseconds.
 */
static inline void ebi_sram_set_pulse_ns(
		struct ebi_sram_params *params,
		unsigned int cs_rd_ns, unsigned int oe_ns,
		unsigned int cs_wr_ns, unsigned int we_ns)
{
	uint32_t bus_khz;

	bus_khz = ebi_sram_get_bus_hz() / 1000;
	ebi_sram_set_pulse_cycles(params,
			div_ceil(bus_khz * cs_rd_ns, 1000000),
			div_ceil(bus_khz * oe_ns, 1000000),
			div_ceil(bus_khz * cs_wr_ns, 1000000),
			div_ceil(bus_khz * we_ns, 1000000));
}

/**
 * \brief Set the total read and write cycle time in nanoseconds
 *
 * \param params Configuration parameters to be altered.
 * \param read_ns Read cycle time in nanoseconds.
 * \param write_ns Write cycle time in nanoseconds.
 */
static inline void ebi_sram_set_total_ns(
		struct ebi_sram_params *params,
		unsigned int read_ns, unsigned int write_ns)
{
	uint32_t bus_khz;

	bus_khz = ebi_sram_get_bus_hz() / 1000;
	ebi_sram_set_total_cycles(params,
			div_ceil(bus_khz * read_ns, 1000000),
			div_ceil(bus_khz * write_ns, 1000000));
}

//@}

//! \name Initialization and Hardware Access
//@{
/**
 * \struct ebi_sram_params
 * \brief Configuration Object for the Static Memory Controller
 *
 * The contents of this structure is platform-specific, so applications
 * and drivers should never access its fields directly.
 */
/**
 * \fn void ebi_sram_enable_cfg_clock(void)
 * \brief Enable the clock to the configuration interface of the Static
 * Memory Controller.
 *
 * This function must be called before any access to the SMC registers
 * can be made. ebi_sram_params_read() and ebi_sram_params_write() do
 * this automatically, so it is only necessary when accessing the
 * registers directly.
 */
/**
 * \fn void ebi_sram_disable_cfg_clock(void)
 * \brief Disable the clock to the configuration interface of the Static
 * Memory Controller.
 *
 * This function may be called after all accesses to the SMC registers
 * have been done, in order to save power. ebi_sram_params_read() and
 * ebi_sram_params_write() do this automatically, so it is only
 * necessary when accessing the registers directly.
 */
/**
 * \fn void ebi_sram_params_init_defaults(
 *              struct ebi_sram_params *params,
 *              unsigned int cs)
 * \brief Initialize configuration parameters to default values.
 *
 * \param params The parameters to be initialized
 * \param cs The chip select number from which to obtain the defaults.
 * Normally, this parameter is ignored, but certain applications or
 * chips may use different defaults for different chip selects.
 */
/**
 * \fn void ebi_sram_params_read(
 *              struct ebi_sram_params *params,
 *              unsigned int cs)
 * \brief Read configuration parameters from hardware.
 *
 * \param params Configuration object into which to store the parameters.
 * \param cs The chip select number from which to read the parameters.
 */
/**
 * \fn void ebi_sram_params_write(
 *              const struct ebi_sram_params *params,
 *              unsigned int cs)
 * \brief Write configuration parameters to hardware.
 *
 * \param params Configuration parameters to be written.
 * \param cs The chip select number to be updated with the new
 * configuration.
 */
//@}

//! \name Timing Parameters in Cycles
//@{
/**
 * \fn uint32_t ebi_sram_get_bus_hz(void)
 * \brief Get the clock frequency of the external SRAM controller
 *
 * \return The clock frequency of the SRAM controller in Hz. This number
 * can be used to convert between clock cycles and nanoseconds when
 * using the other functions in this module.
 */
/**
 * \fn void ebi_sram_set_setup_cycles(
 *              struct ebi_sram_params *params,
 *              uint8_t cs_rd_cycles, uint8_t oe_cycles,
 *              uint8_t cs_wr_cycles, uint8_t we_cycles)
 * \brief Set timing parameters for the \em setup phase, given in
 * clock cycles.
 *
 * \param params Configuration parameters to be altered.
 * \param cs_rd_cycles Chip Select setup time for read access in clock
 * cycles.
 * \param oe_cycles Output Enable strobe setup time in clock cycles.
 * \param cs_wr_cycles Chip Select setup time for write access in clock
 * cycles.
 * \param we_cycles Write Enable strobe setup time in clock cycles.
 */
/**
 * \fn void ebi_sram_set_pulse_cycles(
 *              struct ebi_sram_params *params,
 *              uint8_t cs_rd_cycles, uint8_t oe_cycles,
 *              uint8_t cs_wr_cycles, uint8_t we_cycles)
 * \brief Set timing parameters for the \em pulse phase, given in
 * clock cycles.
 *
 * \param params Configuration parameters to be altered.
 * \param cs_rd_cycles Chip Select pulse time for read access in clock
 * cycles.
 * \param oe_cycles Output Enable strobe pulse time in clock cycles.
 * \param cs_wr_cycles Chip Select pulse time for write access in clock
 * cycles.
 * \param we_cycles Write Enable strobe pulse time in clock cycles.
 */
/**
 * \fn void ebi_sram_set_total_cycles(
 *              struct ebi_sram_params *params,
 *              uint16_t read_cycles, uint16_t write_cycles)
 * \brief Set total read and write cycle time in clock cycles.
 *
 * \param params Configuration parameters to be altered.
 * \param read_cycles Total number of clock cycles for read access.
 * \param write_cycles Total number of clock cycles for write access.
 */
//@}

//! \name Non-Timing Related Parameters
//@{
/**
 * \enum ebi_sram_bus_width
 * \brief Enumeration of the possible external SRAM bus widths that the
 * chip supports.
 */
/**
 * \fn void ebi_sram_set_bus_width(
 *              struct ebi_sram_params *params,
 *              enum ebi_sram_bus_width width,
 *              bool use_byte_sel)
 * \brief Set the number of data lines.
 *
 * \param params Configuration parameters to be altered.
 * \param width The number of data lines to be used.
 * \param use_byte_sel When true, enable the Byte Select control lines
 * and use a common Write Enable signal for all byte lanes. When false,
 * disable the Byte Select control lines and use a separate Write Enable
 * signal for each byte lane. If \a width is 8 bits, the former mode is
 * always used, regardless of this parameter.
 */
//@}
//! @}

#endif /* BUS_EBI_SRAM_H_INCLUDED */
