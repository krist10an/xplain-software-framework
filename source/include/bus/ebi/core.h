/**
 * \file
 *
 * \brief External Bus Interface: Core Functionality
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
#ifndef BUS_EBI_CORE_H_INCLUDED
#define BUS_EBI_CORE_H_INCLUDED

#include <chip/ebi_core.h>

/**
 * \defgroup ebi_group External Bus Interface
 *
 * The External Bus Interface (EBI) is an interface for communicating
 * with various chips using a parallel bus interface. Such devices
 * include SRAM, SDRAM, and any devices which have an interface which is
 * compatible with either of those.
 *
 * Devices connected through the EBI are accessed using standard memory
 * access instructions by the CPU (see \ref mmio_group). Before this
 * will work, however, the EBI must be initialized for the device in
 * question, and the EBI core clock must be enabled by calling
 * ebi_enable_clock(). The initialization sequence is different
 * depending on whether the SRAM or SDRAM interface is being used;
 * please see the appropriate submodule for details.
 *
 * @{
 */
/**
 * \defgroup ebi_params_group EBI Slave Parameters
 *
 * This module contains functions for setting up the EBI to communicate
 * with specific devices on the bus.
 */
/**
 * \fn void ebi_enable_clock(void)
 * \brief Enable the EBI bus clock
 *
 * This function must be called before any EBI access is made.
 */
/**
 * \fn void ebi_disable_clock(void)
 * \brief Disable the EBI bus clock
 *
 * This function may be called when no more EBI accesses are to be made,
 * in order to save power.
 */
//! @}

#endif /* BUS_EBI_CORE_H_INCLUDED */
