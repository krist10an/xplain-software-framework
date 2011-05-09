/**
 * \file
 *
 * \brief Gauge widget implementation
 *
 * Copyright (C) 2009 - 2010 Atmel Corporation. All rights reserved.
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
 
/**
 * \defgroup gfx_trigtable Trigonometric sine table
 *
 * Contains a table holding trigonometric sine values from zero to ninety
 * degrees. Useful for drawing i.e. circles. 
 *
 * Contains two different functions for getting sin- and cos-values
 *
 * Best suited for small drawings, as the resolution of the table is quite low
 * @{
 */

#ifndef WTK_TRIG_TABLE_LENGTH
//! Trigtable length.
# define WTK_TRIG_TABLE_LENGTH        128
#endif

#ifndef WTK_TRIG_TABLE_MAX_VALUE
//! Trigtable max value.
# define WTK_TRIG_TABLE_MAX_VALUE     255
#endif

#include <stdint.h>
#include <progmem.h>

uint8_t wtk_trigtable_sin(uint8_t angle);
uint8_t wtk_trigtable_cos(uint8_t angle);

//! The trigtable values that are loaded into progmem
DEFINE_PROGMEM (uint8_t, trigtable[WTK_TRIG_TABLE_LENGTH]) = {
		  0,  3,  6,  9, 13, 16, 19, 22, 25, 28, 31, 34, 37, 41, 44, 47,
		 50, 53, 56, 59, 62, 65, 68, 71, 74, 77, 80, 83, 86, 89, 92, 95,
		 98,100,103,106,109,112,115,117,120,123,126,128,131,134,136,139,
		142,144,147,149,152,154,157,159,162,164,167,169,171,174,176,178,
		180,183,185,187,189,191,193,195,197,199,201,203,205,207,208,210,
		212,214,215,217,219,220,222,223,225,226,228,229,231,232,233,234,
		236,237,238,239,240,241,242,243,244,245,246,247,247,248,249,249,
		250,251,251,252,252,253,253,253,254,254,254,255,255,255,255,255
	};
	
//! Returns the sine value of the given angle
inline uint8_t wtk_trigtable_sin(uint8_t angle)
{
	return progmem_read8(&(trigtable[angle]));
}

//! Returns the cosine value of the given angle
inline uint8_t wtk_trigtable_cos(uint8_t angle)
{
	return progmem_read8(&(trigtable[(WTK_TRIG_TABLE_LENGTH - 1) - angle]));
}



//! @}
