/**
 * \file
 *
 * \brief AVR32 system register definitions
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
#ifndef ARCH_SYSREG_H_INCLUDED
#define ARCH_SYSREG_H_INCLUDED

/*
 * Some registers are CPU-specific, some others have CPU-specific
 * semantics. Get the definitions of those.
 */
#include <cpu/sysreg.h>

/**
 * \defgroup avr32_sysreg AVR32 System Registers
 * @{
 */
#define SYSREG_SR			0x0000	//!< Status Register
# define SYSREG_SR_C_BIT		0		//!< Carry
# define SYSREG_SR_C			(1 << SYSREG_SR_C_BIT)
# define SYSREG_SR_Z_BIT		1		//!< Zero
# define SYSREG_SR_Z			(1 << SYSREG_SR_Z_BIT)
# define SYSREG_SR_N_BIT		2		//!< Negative
# define SYSREG_SR_N			(1 << SYSREG_SR_N_BIT)
# define SYSREG_SR_V_BIT		3		//!< Overflow
# define SYSREG_SR_V			(1 << SYSREG_SR_V_BIT)
# define SYSREG_SR_Q_BIT		4		//!< Saturation
# define SYSREG_SR_Q			(1 << SYSREG_SR_Q_BIT)
# define SYSREG_SR_L_BIT		5		//!< Lock
# define SYSREG_SR_L			(1 << SYSREG_SR_L_BIT)
# define SYSREG_SR_T_BIT		14		//!< Scratch
# define SYSREG_SR_T			(1 << SYSREG_SR_T_BIT)
# define SYSREG_SR_R_BIT		15		//!< Remap
# define SYSREG_SR_R			(1 << SYSREG_SR_R_BIT)
# define SYSREG_SR_GM_BIT		16		//!< Global Int Mask
# define SYSREG_SR_GM			(1 << SYSREG_SR_GM_BIT)
# define SYSREG_SR_IM_BIT(n)		(17 + (n)) 	//!< INTn Mask
# define SYSREG_SR_IM(n)		(1 << SYSREG_SR_IM_BIT(n))
# define SYSREG_SR_IxM			(0xf << 17)
# define SYSREG_SR_EM_BIT		21		//!< Exception Mask
# define SYSREG_SR_EM			(1 << SYSREG_SR_EM_BIT)
# define SYSREG_SR_MODE_BIT(x)		((x) + 22)	//!< Execution Mode
# define SYSREG_SR_MODE(x)		((x) << SYSREG_SR_MODE_BIT(0))
# define SYSREG_SR_GET_MODE(reg)	(((reg) >> 22) & 0x07)
# define SYSREG_SR_D_BIT		26		//!< Debug State
# define SYSREG_SR_D			(1 << SYSREG_SR_D_BIT)
# define SYSREG_SR_DM_BIT		27		//!< Debug Mask
# define SYSREG_SR_DM			(1 << SYSREG_SR_DM_BIT)
# define SYSREG_SR_J_BIT		28		//!< Java State
# define SYSREG_SR_J			(1 << SYSREG_SR_J_BIT)
# define SYSREG_SR_H_BIT		29		//!< Java Handle
# define SYSREG_SR_H			(1 << SYSREG_SR_H_BIT)
#define SYSREG_EVBA			0x0004	//!< Exception Vector Base Addr
#define SYSREG_ACBA			0x0008	//!< ACALL Base Address
#define SYSREG_CPUCR			0x000c	//!< CPU Control Register
#define SYSREG_ECR			0x0010	//!< Exception Cause Register
#define SYSREG_RSR_SUP			0x0014	//!< Return Status: Supervisor
#define SYSREG_RSR_INT0			0x0018	//!< Return Status: INT0
#define SYSREG_RSR_INT1			0x001c	//!< Return Status: INT1
#define SYSREG_RSR_INT2			0x0020	//!< Return Status: INT2
#define SYSREG_RSR_INT3			0x0024	//!< Return Status: INT3
#define SYSREG_RSR_EX			0x0028	//!< Return Status: Exception
#define SYSREG_RSR_NMI			0x002c	//!< Return Status: NMI
#define SYSREG_RSR_DBG			0x0030	//!< Return Status: Debug
#define SYSREG_RAR_SUP			0x0034	//!< Return Address: Supervisor
#define SYSREG_RAR_INT0			0x0038	//!< Return Address: INT0
#define SYSREG_RAR_INT1			0x003c	//!< Return Address: INT1
#define SYSREG_RAR_INT2			0x0040	//!< Return Address: INT2
#define SYSREG_RAR_INT3			0x0044	//!< Return Address: INT3
#define SYSREG_RAR_EX			0x0048	//!< Return Address: Exception
#define SYSREG_RAR_NMI			0x004c	//!< Return Address: NMI
#define SYSREG_RAR_DBG			0x0050	//!< Return Address: Debug
#define SYSREG_JECR			0x0054	//!< Java ECR
#define SYSREG_JOSP			0x0058	//!< Java Op Stack Pointer
#define SYSREG_JAVA_LV0			0x005c	//!< Java Local Variable 0
#define SYSREG_JAVA_LV1			0x0060	//!< Java Local Variable 1
#define SYSREG_JAVA_LV2			0x0064	//!< Java Local Variable 2
#define SYSREG_JAVA_LV3			0x0068	//!< Java Local Variable 3
#define SYSREG_JAVA_LV4			0x006c	//!< Java Local Variable 4
#define SYSREG_JAVA_LV5			0x0070	//!< Java Local Variable 5
#define SYSREG_JAVA_LV6			0x0074	//!< Java Local Variable 6
#define SYSREG_JAVA_LV7			0x0078	//!< Java Local Variable 7
#define SYSREG_JTBA			0x007c	//!< Java Trap Base Address
#define SYSREG_JBCR			0x0080	//!< Java Write Barrier Ctrl
#define SYSREG_CONFIG0			0x0100	//!< Config register 0
# define SYSREG_CONFIG0_R		(1 <<  0)	//!< RMW Instructions
# define SYSREG_CONFIG0_D		(1 <<  1)	//!< DSP Instructions
# define SYSREG_CONFIG0_S		(1 <<  2)	//!< SIMD Instructions
# define SYSREG_CONFIG0_O		(1 <<  3)	//!< On-Chip Debug
# define SYSREG_CONFIG0_P		(1 <<  4)	//!< Perf Counters
# define SYSREG_CONFIG0_J		(1 <<  5)	//!< Java Extension
# define SYSREG_CONFIG0_F		(1 <<  6)	//!< FPU
# define SYSREG_CONFIG0_GET_MMUT(reg)	(((reg) >> 7) & 7) //!< MMU Type
#  define SYSREG_CONFIG0_MMUT_NONE	0		//!< No MMU or MPU
#  define SYSREG_CONFIG0_MMUT_SEPARATE	1		//!< ITLB and DTLB
#  define SYSREG_CONFIG0_MMUT_SHARED	2		//!< Shared TLB
#  define SYSREG_CONFIG0_MMUT_MPU	3		//!< MPU
# define SYSREG_CONFIG0_GET_AR(reg)	(((reg) >> 10) & 7) //!< Arch Revision
# define SYSREG_CONFIG0_GET_AT(reg)	(((reg) >> 13) & 7) //!< Arch Type
# define SYSREG_CONFIG0_GET_PR(reg)	(((reg) >> 16) & 15) //!< CPU Rev
# define SYSREG_CONFIG0_GET_PT(reg)	(((reg) >> 24) & 0xff) //!< CPU Type
#define SYSREG_CONFIG1			0x0104	//!< Config register 1
  //!< DCache Associativity
# define SYSREG_CONFIG1_GET_DASS(reg)	((reg) & 7)
  //!< DCache Line Size
# define SYSREG_CONFIG1_GET_DLSZ(reg)	(((reg) >> 3) & 7)
  //!< Number of DCache sets
# define SYSREG_CONFIG1_GET_DSET(reg)	(((reg) >> 6) & 15)
  //!< ICache Associativity
# define SYSREG_CONFIG1_GET_IASS(reg)	(((reg) >> 10) & 7)
  //!< ICache Line Size
# define SYSREG_CONFIG1_GET_ILSZ(reg)	(((reg) >> 13) & 7)
  //!< Number of ICache sets
# define SYSREG_CONFIG1_GET_ISET(reg)	(((reg) >> 16) & 15)
  //!< Number of DTLB or shared TLB entries
# define SYSREG_CONFIG1_GET_DMMUSZ(reg)	(((reg) >> 20) & 0x3f)
  //!< Number of ITLB entries
# define SYSREG_CONFIG1_GET_IMMUSZ(reg)	(((reg) >> 26) & 0x3f)
#define SYSREG_COUNT			0x0108	//!< Cycle Counter
#define SYSREG_COMPARE			0x010c	//!< Cycle Compare
#define SYSREG_TLBEHI			0x0110	//!< TLB Entry High
# define SYSREG_TLBEHI_ASID(x)		(x)		//!< Address Space ID
# define SYSREG_TLBEHI_I		(  1 <<  8)	//!< I-TLB
# define SYSREG_TLBEHI_V		(  1 <<  9)	//!< Valid
# define SYSREG_VPN(v)			((v) << 10)	//!< Virtual Page No
#define SYSREG_TLBELO			0x0114	//!< TLB Entry Low
# define SYSREG_TLBELO_W		(  1 <<  0)	//!< Write-Through
# define SYSREG_TLBELO_D		(  1 <<  1)	//!< Dirty
# define SYSREG_TLBELO_SZ_1K		(  0 <<  2)	//!< 1K page size
# define SYSREG_TLBELO_SZ_4K		(  1 <<  2)	//!< 4K page size
# define SYSREG_TLBELO_SZ_64K		(  2 <<  2)	//!< 64K page size
# define SYSREG_TLBELO_SZ_1M		(  3 <<  2)	//!< 1M page size
# define SYSREG_TLBELO_AP_X		(  1 <<  4)	//!< Execute allowed
# define SYSREG_TLBELO_AP_W		(  1 <<  5)	//!< Write allowed
# define SYSREG_TLBELO_AP_U		(  1 <<  6)	//!< Unprivileged
# define SYSREG_TLBELO_B		(  1 <<  7)	//!< Bufferable
# define SYSREG_TLBELO_G		(  1 <<  8)	//!< Global
# define SYSREG_TLBELO_C		(  1 <<  9)	//!< Cacheable
# define SYSREG_TLBELO_PFN(p)		((p) << 10)	//!< Physical Frame No
#define SYSREG_PTBR			0x0118	//!< Page Table Base Register
#define SYSREG_TLBEAR			0x011c	//!< TLB Exception Addr
#define SYSREG_MMUCR			0x0120	//!< MMU Control Register
# define SYSREG_MMUCR_E			(  1 <<  0)	//!< Paging Enable
# define SYSREG_MMUCR_M			(  1 <<  1)	//!< Shared Mode
# define SYSREG_MMUCR_I			(  1 <<  2)	//!< Invalidate
# define SYSREG_MMUCR_N			(  1 <<  3)	//!< Not Found
# define SYSREG_MMUCR_S			(  1 <<  4)	//!< Segments Enable
# define SYSREG_MMUCR_DLA(x)		((x) <<  8)	//!< DTLB Lockdown
# define SYSREG_MMUCR_DRP(x)		((x) << 14)	//!< DTLB Replacement
# define SYSREG_MMUCR_ILA(x)		((x) << 20)	//!< ITLB Lockdown
# define SYSREG_MMUCR_IRP(x)		((x) << 26)	//!< ITLB Replacement
#define SYSREG_TLBARLO			0x0124	//!< TLB Accessed Low
#define SYSREG_TLBARHI			0x0128	//!< TLB Accessed High
#define SYSREG_PCCNT			0x012c	//!< Perf Clock Counter
#define SYSREG_PCNT0			0x0130	//!< Perf Counter 0
#define SYSREG_PCNT1			0x0134	//!< Perf Counter 1
#define SYSREG_PCCR			0x0138	//!< Perf Counter Control
# define SYSREG_PCCR_E			(  1 <<  0)	//!< Enable
# define SYSREG_PCCR_R			(  1 <<  1)	//!< Perf Ctr Reset
# define SYSREG_PCCR_C			(  1 <<  2)	//!< Clock Ctr Reset
# define SYSREG_PCCR_S			(  1 <<  3)	//!< Div by 64
# define SYSREG_PCCR_IE(mask)		((mask) <<  4)	//!< Interrupt Enable
# define SYSREG_PCCR_F(mask)		((mask) <<  8)	//!< Interrupt Flag
# define SYSREG_PCCR_CONF0(evt)		((evt)  << 12)	//!< PC0 Event
# define SYSREG_PCCR_CONF1(evt)		((evt)  << 18)	//!< PC1 Event
#define SYSREG_BEAR			0x013c	//!< Bus Error Address
#define SYSREG_MPUAR0			0x0140	//!< MPU Addr 0
#define SYSREG_MPUAR1			0x0144	//!< MPU Addr 1
#define SYSREG_MPUAR2			0x0148	//!< MPU Addr 2
#define SYSREG_MPUAR3			0x014c	//!< MPU Addr 3
#define SYSREG_MPUAR4			0x0150	//!< MPU Addr 4
#define SYSREG_MPUAR5			0x0154	//!< MPU Addr 5
#define SYSREG_MPUAR6			0x0158	//!< MPU Addr 6
#define SYSREG_MPUAR7			0x015c	//!< MPU Addr 7
#define SYSREG_MPUPSR0			0x0160	//!< MPU Privilege Select 0
#define SYSREG_MPUPSR1			0x0164	//!< MPU Privilege Select 1
#define SYSREG_MPUPSR2			0x0168	//!< MPU Privilege Select 2
#define SYSREG_MPUPSR3			0x016c	//!< MPU Privilege Select 3
#define SYSREG_MPUPSR4			0x0170	//!< MPU Privilege Select 4
#define SYSREG_MPUPSR5			0x0174	//!< MPU Privilege Select 5
#define SYSREG_MPUPSR6			0x0178	//!< MPU Privilege Select 6
#define SYSREG_MPUPSR7			0x017c	//!< MPU Privilege Select 7
#define SYSREG_MPUCRA			0x0180	//!< MPU Cacheable Reg A
#define SYSREG_MPUCRB			0x0184	//!< MPU Cacheable Reg B
#define SYSREG_MPUBRA			0x0188	//!< MPU Bufferable Reg A
#define SYSREG_MPUBRB			0x018c	//!< MPU Bufferable Reg B
#define SYSREG_MPUAPRA			0x0190	//!< MPU Access Perm A
#define SYSREG_MPUAPRB			0x0194	//!< MPU Access Perm B
#define SYSREG_MPUCR			0x0198	//!< MPU Control Register

/* Register access macros */
#ifdef __GNUC__
#define sysreg_read(reg)	 __builtin_mfsr(SYSREG_##reg)
#define sysreg_write(reg, value) __builtin_mtsr(SYSREG_##reg, value)
#elif defined(__ICCAVR32__)
#define sysreg_read(reg)	__get_system_register(SYSREG_##reg)
#define sysreg_write(reg)	__set_system_register(SYSREG_##reg, value)
#endif

//! @}

#endif /* SYSREG_H_INCLUDED */
