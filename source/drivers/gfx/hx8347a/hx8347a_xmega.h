/**
 * \file
 *
 * \brief AVR XMEGA-specific bits of the HX8347A display driver
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
#ifndef DRIVERS_GFX_HX8347A_HX8347A_XMEGA_H_INCLUDED
#define DRIVERS_GFX_HX8347A_HX8347A_XMEGA_H_INCLUDED

/** \todo remove dependency on toolchain header files. */
#if defined(__GNUC__)
# include <avr/io.h>
#elif defined(__ICCAVR__)
# include <ioavr.h>
#endif

#include <board/hx8347a.h>

#define gfx_select_chip()   (GFX_CS_PORT.OUTCLR = GFX_CS_PINMASK)
#define gfx_deselect_chip() (GFX_CS_PORT.OUTSET = GFX_CS_PINMASK)

#define gfx_wait_comms() \
	do {} while ((GFX_USART_MODULE.STATUS & USART_TXCIF_bm) == 0x00); \
	GFX_USART_MODULE.STATUS = USART_TXCIF_bm;

#define gfx_send_byte(value) \
	GFX_USART_MODULE.DATA = (value); \
	gfx_wait_comms();

#define gfx_send_dummy_byte() \
	gfx_send_byte(0xff);

#define gfx_read_byte() \
	(GFX_USART_MODULE.DATA)

#define gfx_enable_receive() \
	GFX_USART_MODULE.CTRLB |= USART_RXEN_bm;

#define gfx_disable_receive() \
	GFX_USART_MODULE.CTRLB &= ~USART_RXEN_bm;

//! \internal Send display command to select register address.
static void gfx_select_register(uint8_t address)
{
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEIDX);
	gfx_send_byte(address);
	gfx_deselect_chip();
}


//! \internal Send display command to write a byte to a specific register.
static void gfx_write_register(uint8_t address, uint8_t value)
{
	gfx_select_register(address);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEREG);
	gfx_send_byte(value);
	gfx_deselect_chip();
}


//! \internal Send display command to read a byte from a specific register.
static uint8_t gfx_read_register(uint8_t address)
{
	uint8_t value;

	gfx_select_register(address);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_READREG);
	gfx_enable_receive();
	gfx_send_dummy_byte();
	value = gfx_read_byte();
	gfx_disable_receive();
	gfx_deselect_chip();

	return value;
}

static gfx_color_t gfx_read_gram(void)
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;

	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_READREG);
	gfx_send_dummy_byte(); // First return byte is useless.
	gfx_enable_receive();

	gfx_send_dummy_byte();
	red = gfx_read_byte();
	gfx_send_dummy_byte();
	green = gfx_read_byte();
	gfx_send_dummy_byte();
	blue = gfx_read_byte();

	gfx_disable_receive();
	gfx_deselect_chip();

	// Convert to 16-bit color format.
	return gfx_color(red, green, blue);
}

static void gfx_write_gram(gfx_color_t color)
{
	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEREG);
	gfx_send_byte(color & 0xff);
	gfx_send_byte(color >> 8);
	gfx_deselect_chip();
}

//! \internal Initialize communication interface to display.
static void gfx_init_comms(void)
{
	// Enable DMA.
	sysclk_enable_module(SYSCLK_PORT_GEN, SYSCLK_DMA);
	DMA.CTRL = DMA_ENABLE_bm;
	// Setup DMA destination address.
	DMA.CH0.DESTADDR0 = ((uintptr_t)&(GFX_USART_MODULE.DATA)) & 0xff;
	DMA.CH0.DESTADDR1 = (((uintptr_t)&(GFX_USART_MODULE.DATA)) >> 8) & 0xff;
	DMA.CH0.DESTADDR2 =
		(((uint32_t)(uintptr_t)&(GFX_USART_MODULE.DATA)) >> 16) & 0xff;

	sysclk_enable_module(SYSCLK_PORT_D, SYSCLK_USART1);

	gfx_deselect_chip();
	GFX_CS_PORT.DIRSET = GFX_CS_PINMASK;

	// Init TXD pin to output
	GFX_USART_PORT.DIRSET = GFX_TXD_PINMASK;
	//Init RXD pin to input
	GFX_USART_PORT.DIRCLR = GFX_RXD_PINMASK;
	// Init XCK pin to output
	GFX_USART_PORT.DIRSET = GFX_XCK_PINMASK;

	GFX_USART_MODULE.CTRLC = USART_CMODE_MSPI_gc;
	GFX_USART_MODULE.BAUDCTRLA = 1; // 8 MHz @ 32 MHz CPU.
	GFX_USART_MODULE.CTRLB = USART_RXEN_bm | USART_TXEN_bm;
}

//! \internal Setup digital control interface to display.
static void gfx_setup_interface(void)
{
	// GFX_TE signal from display is input without any pull resistors
	port_select_gpio_pin(GFX_TE_PIN, PORT_DIR_INPUT);

	// FIXME: This is supposed to generate a PWM signal for the
	// backlight, but it doesn't appear to work.
	TCF0.CTRLB |= TC0_CCAEN_bm | TC_WGMODE_DS_T_gc;
	TCF0.CCA = 75;
	TCF0.PER = 100;
	TCF0.CTRLA = TC_CLKSEL_DIV1024_gc;

	// Backlight pin (PWM) for display is output
	port_select_gpio_pin(GFX_BACKLIGHT_PIN,
			PORT_DIR_OUTPUT | PORT_INIT_HIGH);

	// Reset pin for display is output
	port_select_gpio_pin(GFX_RESET_PIN,
			PORT_DIR_OUTPUT | PORT_INIT_HIGH);
}

void gfx_duplicate_pixel(gfx_color_t color, uint32_t count)
{
	// Sanity check. Count should not exceed 24 bit, and not be zero.
	assert((count >> 24) == 0);
	assert(count > 0);

	// Prepare HIMAX driver for data.
	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEREG);

	// Setup DMA source address.
	DMA.CH0.SRCADDR0 = ((uintptr_t)&color) & 0xff;
	DMA.CH0.SRCADDR1 = (((uintptr_t)&color) >> 8) & 0xff;
	DMA.CH0.SRCADDR2 = (((uint32_t)(uintptr_t)&color) >> 16) & 0xff;

	// Setup proper pixel byte count.
	DMA.CH0.TRFCNT = sizeof(gfx_color_t);

	// Read pixel bytes and rewind, always write to same IO register.
	DMA.CH0.ADDRCTRL =
		(uint8_t)DMA_CH_SRCRELOAD_BLOCK_gc |
		(uint8_t)DMA_CH_SRCDIR_INC_gc |
		(uint8_t)DMA_CH_DESTRELOAD_NONE_gc |
		(uint8_t)DMA_CH_DESTDIR_FIXED_gc;
	DMA.CH0.TRIGSRC = GFX_USART_TRIGGER;

	// Write as many blocks of 255 pixels as possible, using DMA repeat.
	while (count >= 255) {
		DMA.CH0.REPCNT = 255;
		DMA.CH0.CTRLA =
			DMA_CH_ENABLE_bm |
			DMA_CH_REPEAT_bm |
			DMA_CH_SINGLE_bm |
			DMA_CH_TRFREQ_bm |
			DMA_CH_BURSTLEN_1BYTE_gc;

		// FIXME: Why are we using DMA again...?
		do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
		gfx_wait_comms();
		count -= 255;
	}

	// Write remaning pixels, less-than-255-pixel block.
	if ((count & 0xff) > 0) {
		DMA.CH0.REPCNT = count & 0xff;
		DMA.CH0.CTRLA =
			DMA_CH_ENABLE_bm |
			DMA_CH_REPEAT_bm |
			DMA_CH_SINGLE_bm |
			DMA_CH_TRFREQ_bm |
			DMA_CH_BURSTLEN_1BYTE_gc;
		do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
		gfx_wait_comms();
	}

	gfx_deselect_chip();
}

void gfx_copy_pixels_to_screen(const gfx_color_t *pixels, uint32_t count)
{
	uint32_t byte_count;
	uint16_t remainder_count;
	uint8_t block_count;

	// Sanity check.
	assert(pixels != NULL);
	assert(count > 0);

	// Prepare HIMAX driver for data.
	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEREG);

	// Setup DMA source address.
	DMA.CH0.SRCADDR0 = ((uintptr_t)pixels) & 0xff;
	DMA.CH0.SRCADDR1 = (((uintptr_t)pixels) >> 8) & 0xff;
	DMA.CH0.SRCADDR2 = (((uint32_t)(uintptr_t)pixels) >> 16) & 0xff;

	// Read pixel bytes, always write to same IO register.
	DMA.CH0.ADDRCTRL =
		(uint8_t)DMA_CH_SRCRELOAD_NONE_gc |
		(uint8_t)DMA_CH_SRCDIR_INC_gc |
		(uint8_t)DMA_CH_DESTRELOAD_NONE_gc |
		(uint8_t)DMA_CH_DESTDIR_FIXED_gc;
	DMA.CH0.TRIGSRC = GFX_USART_TRIGGER;

	// Compute byte count.
	byte_count = count * sizeof(gfx_color_t);
	assert((byte_count >> 24) == 0x00);
	block_count = byte_count >> 16;
	remainder_count = byte_count & 0xffff;

	// Write as many 64K byte blocks as possible.
	if (block_count > 0) {
		DMA.CH0.TRFCNT = 0; // Equals 65536.
		DMA.CH0.REPCNT = block_count;
		DMA.CH0.CTRLA =
			DMA_CH_ENABLE_bm |
			DMA_CH_REPEAT_bm |
			DMA_CH_SINGLE_bm |
			DMA_CH_BURSTLEN_1BYTE_gc;
		do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
		gfx_wait_comms();
	}

	// Write remaining bytes.
	if (remainder_count > 0) {
		DMA.CH0.TRFCNT = remainder_count;
		DMA.CH0.CTRLA =
			DMA_CH_ENABLE_bm |
			DMA_CH_SINGLE_bm |
			DMA_CH_BURSTLEN_1BYTE_gc;
		do {} while ((DMA.CH0.CTRLB & DMA_CH_CHBUSY_bm) != 0x00);
		gfx_wait_comms();
	}

	gfx_deselect_chip();
}

void gfx_copy_progmem_pixels_to_screen(const gfx_color_t __progmem_arg *pixels,
		uint32_t count)
{
	uint32_t byte_count;
	uint8_t __progmem_arg *byte_ptr;

	// Sanity check.
	assert(pixels != NULL);
	assert(count > 0);

	// Prepare HIMAX driver for data.
	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEREG);

	// Copy bytes from Flash to display.
	byte_count = count * sizeof(gfx_color_t);
	byte_ptr = (uint8_t __progmem_arg *)pixels;

	while (byte_count > 0) {
		gfx_send_byte(progmem_read8(byte_ptr));
		++byte_ptr;
		--byte_count;
	}

	gfx_deselect_chip();
}

void gfx_copy_hugemem_pixels_to_screen(const hugemem_ptr_t pixels,
		uint32_t count)
{
	uint32_t byte_count;
	hugemem_ptr_t byte_ptr;

	// Sanity check.
	assert(pixels);
	assert(count > 0);

	// Prepare HIMAX driver for data.
	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_WRITEREG);

	// Copy bytes from hugemem to display.
	byte_count = count * sizeof(gfx_color_t);
	byte_ptr = pixels;

	while (byte_count > 0) {
		gfx_send_byte(hugemem_read8(byte_ptr));
		byte_ptr = (hugemem_ptr_t)((uint32_t)byte_ptr + 1);
		--byte_count;
	}

	gfx_deselect_chip();
}

void gfx_copy_pixels_from_screen(gfx_color_t *pixels, uint32_t count)
{
	// Sanity check.
	assert(pixels != NULL);
	assert(count > 0);

	// Prepare HIMAX driver for read, ignoring first dummy byte.
	gfx_select_register(HX8347A_SRAMWRITE);
	gfx_select_chip();
	gfx_send_byte(HX8347A_START_READREG);
	gfx_send_dummy_byte();
	gfx_enable_receive();

	// Read 3-byte pixel data and create gfx_color_t data as we go.
	while (count > 0) {
		uint8_t red;
		uint8_t green;
		uint8_t blue;

		gfx_send_dummy_byte();
		red = gfx_read_byte();
		gfx_send_dummy_byte();
		green = gfx_read_byte();
		gfx_send_dummy_byte();
		blue = gfx_read_byte();

		*pixels = gfx_color(red, green, blue);

		++pixels;
		--count;
	}

	gfx_disable_receive();
	gfx_deselect_chip();
}

#endif /* DRIVERS_GFX_HX8347A_HX8347A_XMEGA_H_INCLUDED */
