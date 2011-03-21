/**
 * \file
 *
 * \brief SCSI Command Descriptor Block (CDB) helper functions
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
#ifndef SCSI_CDB_H_INCLUDED
#define SCSI_CDB_H_INCLUDED

#include <assert.h>
#include <types.h>
#include <unaligned.h>

#include <scsi/spc_protocol.h>

static inline uint16_t scsi_cdb_get_u16(const uint8_t *cdb, unsigned int offset)
{
	return read_unaligned_be16(cdb + offset);
}

static inline uint32_t scsi_cdb_get_u32(const uint8_t *cdb, unsigned int offset)
{
	return read_unaligned_be32(cdb + offset);
}

static inline uint8_t scsi_cdb_get_opcode(const uint8_t *cdb)
{
	return cdb[0];
}

static inline uint8_t scsi_cdb_get_group_code(const uint8_t *cdb)
{
	return scsi_cdb_get_opcode(cdb) >> 5;
}

static inline uint32_t scsi_cdb6_get_lba(const uint8_t *cdb)
{
	uint32_t	lba;

	assert(scsi_cdb_get_group_code(cdb) == 0);

	lba = (uint32_t)(cdb[1] & 0x1f) << 16;
	lba |= (uint32_t)cdb[2] << 8;
	lba |= (uint32_t)cdb[3];

	return lba;
}

static inline uint32_t scsi_cdb10_get_lba(const uint8_t *cdb)
{
	assert(scsi_cdb_get_group_code(cdb) == 1
			|| scsi_cdb_get_group_code(cdb) == 2);

	return scsi_cdb_get_u32(cdb, 2);
}

static inline uint32_t scsi_cdb6_get_xfer_len(const uint8_t *cdb)
{
	uint32_t	len;

	/* REVISIT: This may not be correct for all CDBs, but it does
	 * the trick for READx and WRITEx */

	assert(scsi_cdb_get_group_code(cdb) == 0);
	len = cdb[4];
	if (!len)
		len = 256;

	return len;
}


static inline uint32_t scsi_cdb10_get_xfer_len(const uint8_t *cdb)
{
	assert(scsi_cdb_get_group_code(cdb) == 1
			|| scsi_cdb_get_group_code(cdb) == 2);

	return scsi_cdb_get_u16(cdb, 7);
}

static inline uint32_t scsi_cdb6_get_alloc_len(const uint8_t *cdb)
{
	return cdb[4];
}

static inline uint32_t scsi_cdb10_get_alloc_len(const uint8_t *cdb)
{
	return scsi_cdb_get_u16(cdb, 7);
}

static inline bool scsi_cdb10_bytchk_is_set(const uint8_t *cdb)
{
	return cdb[1] & (1 << 1);
}

#endif /* SCSI_CDB_H_INCLUDED */
