/**
 * \file
 *
 * \brief High-speed Bus (HSB) Configuration Helpers
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
#ifndef BUS_HSB_H_INCLUDED
#define BUS_HSB_H_INCLUDED

#include <io.h>
#include <stdint.h>

#include <chip/memory-map.h>
#include <chip/hsb.h>
#include <regs/hmatrix.h>

/**
 * \defgroup hsb_group High-Speed Bus Matrix Configuration
 *
 * The High-Speed Bus Matrix (HMATRIX) is a multi-layer bus which
 * connects all the HSB master and slave interfaces on the chip. A HSB
 * master (e.g. the CPU) is a device which is capable of initiating an
 * access to a HSB slave (e.g. an internal memory bank) on the same bus.
 * The HMATRIX is the device which connects the two together, and
 * arbitrates between multiple masters that try to access the same slave
 * device.
 *
 * The functions and definitions in this module can be divided into
 * three groups:
 *   - HSB Master configuration for configuring special properties
 *     associated with one or more HSB master interfaces.
 *   - HSB Slave configuration for configuring special properties
 *     associated with one or more HSB slave interfaces.
 *   - HSB Priority configuratin for configuring the priority between
 *     all HSB master-slave links.
 *
 * Each of these configurations are represented by a structure holding
 * an internal representation of the selected configuration. This
 * configuration can be changed by calling one or more functions on it,
 * each changing one particular parameter. An initial configuration may
 * be obtained either from a default set of parameters ("init"), or by
 * reading the current configuration from the hardware registers
 * ("read"). After all the necessary modifications have been made, the
 * configuration may be committed to one or more targets ("write").
 *
 * The API is designed to allow maximum flexibility while still
 * producing close to optimal code. The client code becomes fairly
 * verbose, but also mostly self-documenting; each call apart from the
 * initialization and commit calls usually corresponds to one particular
 * system requirement.
 *
 * @{
 */

#ifndef __ASSEMBLY__

#include <assert.h>

/**
 * \brief HSB Master Undefined-Length Burst Type
 * \see hsb_master_set_undef_burst_len()
 */
enum hsb_ulbt {
	HSB_ULBT_INF            = HMATRIX_ULBT_INF,     //!< Infinite
	HSB_ULBT_1              = HMATRIX_ULBT_1,       //!< Single Access
	HSB_ULBT_4              = HMATRIX_ULBT_4,       //!< 4-beat Burst
	HSB_ULBT_8              = HMATRIX_ULBT_8,       //!< 8-beat Burst
	HSB_ULBT_16             = HMATRIX_ULBT_16,      //!< 16-beat Burst
};

/**
 * \brief HSB Slave arbitration policy
 * \see hsb_slave_set_arbitration_type
 */
enum hsb_arbitration_type {
	HSB_ARBT_ROUND_ROBIN    = HMATRIX_ARBT_ROUND_ROBIN, //!< Round-Robin
	HSB_ARBT_FIXED_PRIO     = HMATRIX_ARBT_FIXED_PRIO,  //!< Fixed Priority
};

//! \name HSB Master configuration
//@{

/**
 * \brief HSB Master configuration
 *
 * This structure holds track of the configuration parameters for a
 * master on the HSB bus. It can be committed to any number of HSB
 * masters through the hsb_write_master_config() function.
 */
struct hsb_master_config {
	uint32_t value;         //!< \internal MCFG register state
};

/**
 * \brief Initialize master configuration to default values
 *
 * This will initialize the master configuration in \a mcfg as follows:
 *   - Allow re-arbitration of undefined-length bursts every four beats
 *
 * \param mcfg The configuration parameters to be initialized
 */
static inline void hsb_master_init_config(
		struct hsb_master_config *mcfg)
{
	mcfg->value = HMATRIX_BF(MCFG_ULBT, HSB_ULBT_4);
}

/**
 * \brief Retrieve the current configuration of a HSB master
 * \param mcfg Structure for holding configuration parameters
 * \param master Master ID from which to retrieve the configuration
 */
static inline void hsb_master_read_config(
		struct hsb_master_config *mcfg,
		enum hsb_master_id master)
{
	assert(master < HSB_NR_MASTERS);

	mcfg->value = hmatrix_read_mcfg(master);
}

/**
 * \brief Commit the current configuration of a HSB master
 * \param mcfg Structure holding new configuration parameters
 * \param master Master ID for which the configuration is to be updated
 */
static inline void hsb_master_write_config(
		const struct hsb_master_config *mcfg,
		enum hsb_master_id master)
{
	assert(master < HSB_NR_MASTERS);

	hmatrix_write_mcfg(master, mcfg->value);
}

/**
 * \brief Set Undefined-Length Burst Type in a master configuration
 *
 * When a master requests an undefined-length burst (INCR), the Bus
 * Matrix may provide re-arbitration points at regular intervals. This
 * function may be used to specify the boundary at which such bursts may
 * be broken.
 *
 * \param mcfg Structure holding configuration parameters to be changed
 * \param ulbt Desired burst type to be used for undefined-length bursts
 */
static inline void hsb_master_set_undef_burst_len(
		struct hsb_master_config *mcfg,
		enum hsb_ulbt ulbt)
{
	assert(ulbt < (1 << HMATRIX_MCFG_ULBT_SIZE));

	mcfg->value = HMATRIX_BFINS(MCFG_ULBT, ulbt, mcfg->value);
}

//@}

//! \name HSB Slave configuration
//@{

/**
 * \brief HSB Slave configuration
 *
 * This structure holds track of the configuration parameters for a
 * slave on the HSB bus. It can be committed to any number of HSB slaves
 * through the hsb_write_slave_config() function.
 */
struct hsb_slave_config {
	uint32_t value;         //!< \internal SCFG register state
};

/**
 * \brief Initialize slave configuration to default values
 *
 * This will initialize the slave configuration in \a scfg as follows:
 *   - 16 cycles slot cycle limit
 *   - No default master
 *   - Round-robin arbitration policy
 *
 * \param scfg The configuration parameters to be initialized
 */
static inline void hsb_slave_init_config(struct hsb_slave_config *scfg)
{
	scfg->value = HMATRIX_BF(SCFG_SLOT_CYCLE, 16)
		| HMATRIX_BF(SCFG_DEFMSTR_TYPE, HMATRIX_DEFMSTR_TYPE_NONE)
		| HMATRIX_BF(SCFG_ARBT, HSB_ARBT_ROUND_ROBIN);
}

/**
 * \brief Retrieve the current configuration of a HSB slave
 * \param scfg Structure for holding configuration parameters
 * \param slave Slave ID from which to retrieve the configuration
 */
static inline void hsb_slave_read_config(struct hsb_slave_config *scfg,
		enum hsb_slave_id slave)
{
	assert(slave < HSB_NR_SLAVES);

	scfg->value = hmatrix_read_scfg(slave);
}

/**
 * \brief Commit the current configuration of a HSB slave
 * \param scfg Structure holding new configuration parameters
 * \param slave Slave ID for which the configuration is to be updated
 */
static inline void hsb_slave_write_config(struct hsb_slave_config *scfg,
		enum hsb_slave_id slave)
{
	assert(slave < HSB_NR_SLAVES);

	hmatrix_write_scfg(slave, scfg->value);
}

/**
 * \brief Set the Slot Cycle limit in a slave configuration
 *
 * Set the number of bus cycles after which an ongoing burst access to
 * the slave may be broken. This may help reducing the access latency to
 * a slow slave when long bursts are used.
 *
 * \warning Setting this value too low may cause all bursts to be broken
 * and the Bus Matrix to re-arbitrate without performing any data
 * transfer.
 *
 * \param scfg The slave configuration to be modified
 * \param count The new slot cycle limit
 */
static inline void hsb_slave_set_slot_cycle(
		struct hsb_slave_config *scfg, uint8_t count)
{
	scfg->value = HMATRIX_BFINS(SCFG_SLOT_CYCLE, count, scfg->value);
}

/**
 * \brief Set <em>no default master</em> in a slave configuration
 *
 * When no default master is used, the slave will disconnect from all
 * masters when no request is pending. This results in a one cycle
 * latency on the next access for all masters.
 *
 * \param scfg The slave configuration to be modified
 */
static inline void hsb_slave_set_no_default_master(
		struct hsb_slave_config *scfg)
{
	scfg->value = HMATRIX_BFINS(SCFG_DEFMSTR_TYPE,
			HMATRIX_DEFMSTR_TYPE_NONE, scfg->value);
}

/**
 * \brief Set <em>last default master</em> in a slave configuration
 *
 * When <em>last default master</em> is used, the slave will stay
 * connected to the last master that accessed it if there are no other
 * requests pending. This results in no latency on the next access for
 * the last master that accessed the slave and a one cycle latency for
 * all other masters.
 *
 * \param scfg The slave configuration to be modified
 */
static inline void hsb_slave_set_last_default_master(
		struct hsb_slave_config *scfg)
{
	scfg->value = HMATRIX_BFINS(SCFG_DEFMSTR_TYPE,
			HMATRIX_DEFMSTR_TYPE_LAST, scfg->value);
}

/**
 * \brief Set a <em>fixed default master</em> in a slave configuration
 *
 * When <em>fixed default master</em> is used, the slave will connect to
 * \a master when there are no requests pending. This results in no
 * latency on the next access for \a master and a one cycle latency for
 * all other masters.
 *
 * \param scfg The slave configuration to be modified
 * \param master The Master ID of the fixed default master
 */
static inline void hsb_slave_set_fixed_default_master(
		struct hsb_slave_config *scfg,
		enum hsb_master_id master)
{
	uint32_t value = scfg->value;

	assert(master < (1 << HMATRIX_SCFG_FIXED_DEFMSTR_SIZE));

	value = HMATRIX_BFINS(SCFG_DEFMSTR_TYPE,
			HMATRIX_DEFMSTR_TYPE_FIXED, value);
	value = HMATRIX_BFINS(SCFG_FIXED_DEFMSTR, master, value);
	scfg->value = value;
}

/**
 * \brief Set the arbitration policy in a slave configuration
 *
 * \param scfg The slave configuration to be modified
 * \param arbt The desired arbitration policy
 */
static inline void hsb_slave_set_arbitration_type(
		struct hsb_slave_config *scfg,
		enum hsb_arbitration_type arbt)
{
	assert(arbt < (1 << HMATRIX_SCFG_ARBT_SIZE));

	scfg->value = HMATRIX_BFINS(SCFG_ARBT, arbt, scfg->value);
}

//@}

/**
 * \name HSB Priority Configuration
 *
 * Note that the slave arbitration type must be set to
 * #HSB_ARBT_FIXED_PRIO for the priority configuration to have any
 * effect.
 */
//@{

/**
 * \brief HSB Priority configuration for all masters on one slave.
 */
struct hsb_slave_priority {
	uint32_t pras;          //!< \internal Priority register A state
	uint32_t prbs;          //!< \internal Priority register B state
};

/**
 * \brief Initialize priority configuration to default values.
 *
 * This will initialize the priority configuration in \a prio with all
 * masters at lowest priority.
 *
 * \param prio The priority configuration to be initialized.
 */
static inline void hsb_priority_init(struct hsb_slave_priority *prio)
{
	prio->pras = 0;
	if (HSB_NR_MASTERS > 8)
		prio->prbs = 0;
}

/**
 * \brief Retrieve the current priority configuration of a HSB slave.
 * \param prio Structure for holding priority configuration.
 * \param slave Slave ID from which to retrieve the configuration.
 */
static inline void hsb_priority_read(struct hsb_slave_priority *prio,
		enum hsb_slave_id slave)
{
	assert(slave < HSB_NR_SLAVES);

	prio->pras = hmatrix_read_pras(slave);
	if (HSB_NR_MASTERS > 8)
		prio->prbs = hmatrix_read_prbs(slave);
}

/**
 * \brief Commit a priority configuration to a HSB slave.
 * \param prio Structure holding the priority configuration.
 * \param slave Slave ID on which to commit the configuration.
 */
static inline void hsb_priority_write(struct hsb_slave_priority *prio,
		enum hsb_slave_id slave)
{
	assert(slave < HSB_NR_SLAVES);

	hmatrix_write_pras(slave, prio->pras);
	if (HSB_NR_MASTERS > 8)
		hmatrix_write_prbs(slave, prio->prbs);
}

/**
 * \brief Set the priority of \a master when accessing \a slave
 *
 * The priority is specified as a number from 0 to 15. The master with
 * the highest priority number is serviced first.
 *
 * \param prio The slave priority configuration
 * \param master The ID of the master doing the access
 * \param priority The priority of \a master when accessing a slave
 * configured with \a prio
 */
static inline void hsb_priority_set(struct hsb_slave_priority *prio,
		enum hsb_master_id master, unsigned int priority)
{
	uint32_t value;

	assert(priority < 16);
	assert(master < HSB_NR_MASTERS);

	/*
	 * If the chip has 8 HSB masters or less, the first of these
	 * blocks gets optimized away along with the condition itself.
	 */
	if (HSB_NR_MASTERS > 8 && master > 7) {
		value = prio->prbs;
		value &= ~HMATRIX_PRBS_MPR_MASK(master);
		value |= HMATRIX_PRBS_MPR(master, priority);
		prio->prbs = value;
	} else {
		value = prio->pras;
		value &= ~HMATRIX_PRAS_MPR_MASK(master);
		value |= HMATRIX_PRAS_MPR(master, priority);
		prio->pras = value;
	}
}

//@}

#endif /* __ASSEMBLY__ */

//! @}

#endif /* BUS_HSB_H_INCLUDED */
