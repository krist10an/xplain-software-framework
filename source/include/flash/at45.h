/**
 * \file
 *
 * \brief AT45 DataFlash(R) Definions
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
#ifndef FLASH_AT45_H_INCLUDED
#define FLASH_AT45_H_INCLUDED

/**
 * \ingroup flash_group
 * \defgroup at45_flash_group AT45 DataFlash
 *
 * The following types of DataFlash are supported:
 *   - DB011D(2):  256 +  8 B/page, 8 pages/block, 128 pages/sector,  4 sectors
 *   - DB021D(3):  256 +  8 B/page, 8 pages/block, 128 pages/sector,  8 sectors
 *   - DB041D(4):  256 +  8 B/page, 8 pages/block, 256 pages/sector,  8 sectors
 *   - DB081D(5):  256 +  8 B/page, 8 pages/block, 256 pages/sector, 16 sectors
 *   - DB161D(6):  512 + 16 B/page, 8 pages/block, 256 pages/sector, 16 sectors
 *   - DB321D(7):  512 + 16 B/page, 8 pages/block, 128 pages/sector, 64 sectors
 *   - DB642D(8): 1024 + 32 B/page, 8 pages/block, 256 pages/sector, 32 sectors
 *
 * @{
 */

/**
 * \name AT45 segmentation defines
 *
 * This can be used to build up a singel byte of information containing
 * information on page size, sector size and device size.
 *
 * @{
 */
#define AT45_PS_256     (0 << 0) //!< Page size 256 bytes
#define AT45_PS_512     (1 << 0) //!< Page size 512 bytes
#define AT45_PS_1024    (2 << 0) //!< Page size 1024 bytes
#define AT45_PS_MASK    (7 << 0) //!< Page size mask bits
#define AT45_SS_128P    (0 << 3) //!< Sector size 128 pages
#define AT45_SS_256P    (1 << 3) //!< Sector size 256 bytes
#define AT45_SS_MASK    (3 << 3) //!< Sector size mask bits
#define AT45_DS_4S      (0 << 4) //!< Device size 4 sectors
#define AT45_DS_8S      (1 << 4) //!< Device size 8 sectors
#define AT45_DS_16S     (2 << 4) //!< Device size 16 sectors
#define AT45_DS_32S     (3 << 4) //!< Device size 32 sectors
#define AT45_DS_64S     (4 << 4) //!< Device size 64 sectors
#define AT45_DS_MASK    (7 << 4) //!< Device size mask bits
//! @}

/**
 * \name AT45 devices size information bitmask
 *
 * Associates AT45 device type to a bitmask with size information
 *
 * @{
 */
//! Size definitions for AT45DB011D
#define AT45DB011D	(AT45_PS_256 | AT45_SS_128P | AT45_DS_4S)
//! Size definitions for AT45DB021D
#define AT45DB021D	(AT45_PS_256 | AT45_SS_128P | AT45_DS_8S)
//! Size definitions for AT45DB031D
#define AT45DB041D	(AT45_PS_256 | AT45_SS_256P | AT45_DS_8S)
//! Size definitions for AT45DB081D
#define AT45DB081D	(AT45_PS_256 | AT45_SS_256P | AT45_DS_16S)
//! Size definitions for AT45DB161D
#define AT45DB161D	(AT45_PS_512 | AT45_SS_256P | AT45_DS_16S)
//! Size definitions for AT45DB321D
#define AT45DB321D	(AT45_PS_512 | AT45_SS_128P | AT45_DS_64S)
//! Size definitions for AT45DB641D
#define AT45DB642D	(AT45_PS_1024 | AT45_SS_256P | AT45_DS_32S)
//! @}

//! AT45 Commands
enum at45_cmd {
	//! Read continous data
	AT45_CMD_CONTINOUS_ARRAY_READ                    = 0x0b,
	//! Transfer from flash to buffer 1
	AT45_CMD_MAIN_MEMORY_TO_BUFFER_1_TRANSFER        = 0x53,
	//! Program buffer 1 into flash with built-in erase
	AT45_CMD_BUFFER_1_MAIN_MEMORY_PROGRAM_WITH_ERASE = 0x83,
	//! Write to buffer 1
	AT45_CMD_BUFFER_1_WRITE                          = 0x84,
	//! Read manufacturer and device ID
	AT45_CMD_READ_ID                                 = 0x9f,
	//! Read status register
	AT45_CMD_READ_STATUS_REG                         = 0xd7,
};

//! AT45 status register bits
enum at45_status_bit {
	AT45_STATUS_PAGE_SIZE = 0, //!< Page size (1: power-of-2 size)
	AT45_STATUS_PROTECT   = 1, //!< Sector protection
	AT45_STATUS_COMP      = 6, //!< Result of compare operation
	AT45_STATUS_RDY       = 7, //!< Ready for next command
};

//! Mask for AT45 page address (13-bit)
#define AT45_PAGE_ADDR_MASK             ((1 << 13) - 1)

//! Mask for AT45 page position (11-bit)
#define AT45_PAGE_POS_MASK              ((1 << 11) - 1)

//! Atmel AT45 JEDEC ID
#define AT45_ATMEL_JEDEC_ID             0x1f

//! DataFlash familty code
#define AT45_DATAFLASH_FAMILY_CODE      0x20

//! Mask for AT45 family code
#define AT45_FAMILY_CODE_MASK           0xe0

//! Minimum AT45 density code
#define AT45_DENSITY_CODE_MIN           2

//! Maximum AT45 density code
#define AT45_DENSITY_CODE_MAX           8

//! Mask for AT45 density code
#define AT45_DENSITY_CODE_MASK          0x1f

/**
 * \brief The device size in bytes for density code 0
 *
 * Not a known density code, but usefull for calculation since for every step
 * up in density code doubles the deviedc size. So it can be used as a base to
 * be shifted.
 */
#define AT45_DENSITY_CODE_0_DEVICE_SIZE 0x8000L

//! The page size in bytes for page size id 0
#define AT45_PAGE_SIZE_ID_0_SIZE        256

/**
 * \brief AT45 density code to device size information translation table
 *
 * The device density code indicates a specific device, and this table
 * translates from this to the device size information value.
 */
static const uint8_t at45_density_table[] = {
	[2] = AT45DB011D,
	[3] = AT45DB021D,
	[4] = AT45DB041D,
	[5] = AT45DB081D,
	[6] = AT45DB161D,
	[7] = AT45DB321D,
	[8] = AT45DB642D,
};

/**
 * \brief Test if AT45 mamufacturer ID is Atmel's
 *
 * \param manuf_id Manufacturer ID
 * \retval true  Is equal
 * \retval false Is different
 */
static inline bool at45_is_atmel_jedec_id(uint8_t manuf_id)
{
	return manuf_id == AT45_ATMEL_JEDEC_ID;
}

/**
 * \brief Test if AT45 device ID is DataFlash family
 *
 * \param device_id1 Device ID part 1
 * \retval true  Is equal
 * \retval false Is different
 */
static inline bool at45_is_dataflash_family_code(uint8_t device_id1)
{
	return (device_id1 & AT45_FAMILY_CODE_MASK)
			== AT45_DATAFLASH_FAMILY_CODE;
}

/**
 * \brief Get AT45 density code from Device ID 1
 *
 * \param device_id1 Device ID part 1
 * \return Density code
 */
static inline uint8_t at45_get_density_code(uint8_t device_id1)
{
	return device_id1 & AT45_DENSITY_CODE_MASK;
}

/**
 * \brief Test if AT45 device ID have a valid density code
 *
 * \param device_id1 Device ID part 1
 * \retval true  Is equal
 * \retval false Is different
 */
static inline bool at45_is_valid_density_code(uint8_t device_id1)
{
	uint8_t density_code = at45_get_density_code(device_id1);

	return (density_code >= AT45_DENSITY_CODE_MIN)
			&& (density_code <= AT45_DENSITY_CODE_MAX);
}

/**
 * \brief Test if AT45 device ID is DataFlash family
 *
 * \param manuf_id   Manufacturer ID
 * \param device_id1 Device ID part 1
 * \retval true  Is equal
 * \retval false Is different
 */
static inline bool at45_is_valid_id(uint8_t manuf_id, uint8_t device_id1)
{
	return at45_is_atmel_jedec_id(manuf_id)
			&& at45_is_dataflash_family_code(device_id1)
			&& at45_is_valid_density_code(device_id1);
}

/**
 * \brief Get AT45 device size from Device ID 1
 *
 * \param device_id1 Device ID part 1
 * \return Size of device
 */
static inline uint32_t at45_get_size(uint8_t device_id1)
{
	// Every step up in density code doubles the size so just shift it
	return AT45_DENSITY_CODE_0_DEVICE_SIZE
			<< at45_get_density_code(device_id1);
}

/**
 * \brief Get AT45 device page size from Device ID 1
 *
 * \param device_id1 Device ID part 1
 * \return Page size
 */
static inline uint32_t at45_get_page_size(uint8_t device_id1)
{
	uint8_t density;

	density = at45_density_table[at45_get_density_code(device_id1)];

	/* Every step up in id for page size doubles the page size so just
	 * shift it.
	 */
	return AT45_PAGE_SIZE_ID_0_SIZE << (density & AT45_PS_MASK);
}

//! @}

#endif /* FLASH_AT45_H_INCLUDED */
