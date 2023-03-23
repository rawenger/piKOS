/*
 * armv8mmu.h - MMU-related structure definitions for ARMv8-A
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
 *
 * References for field definitions:
 * https://armv8-ref.codingbelief.com/en/chapter_d4/d43_1_vmsav8-64_translation_table_descriptor_formats.html#
 * and
 * https://armv8-ref.codingbelief.com/en/chapter_d4/d43_2_armv8_translation_table_level_3_descriptor_formats.html
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "util/memorymap.h"

#define ARMv8MMU_GRANULE_SIZE                   PAGESIZE /* 4K page size */
#if (ARMv8MMU_GRANULE_SIZE == (4 * KILOBYTE))
	#define ARMv8MMU_MAX_LVL0_TABLE_ENTRIES 512
	#define ARMv8MMU_MAX_LVL1_TABLE_ENTRIES 512
	#define ARMv8MMU_MAX_LVL2_TABLE_ENTRIES 512
	#define ARMv8MMU_LVL3_TABLE_ENTRIES     512 /* must have exactly this number */
#elif (ARMv8MMU_GRANULE_SIZE == (16 * KILOBYTE))
	#define ARMv8MMU_MAX_LVL0_TABLE_ENTIRES 2
	#define ARMv8MMU_MAX_LVL1_TABLE_ENTIRES 2048
	#define ARMv8MMU_MAX_LVL2_TABLE_ENTIRES 2048
	#define ARMv8MMU_LVL3_TABLE_ENTIRES     2048 /* must have exactly this number */
#elif (ARMv8MMU_GRANULE_SIZE == (64 * KILOBYTE))
/* #define ARMv8MMU_MAX_LVL0_TABLE_ENTRIES */ /* no L0 tables */
	#define ARMv8MMU_MAX_LVL1_TABLE_ENTRIES 64
	#define ARMv8MMU_MAX_LVL2_TABLE_ENTRIES 8192
	#define ARMv8MMU_LVL3_TABLE_ENTRIES     8192 /* must have exactly this number */
#endif

typedef union {
	/* little-endian! */
	struct {
		uint64_t offset: 12,
			 L3: 9,
			 L2: 9,
			 L1: 9,
			 L0: 9,
			 reserved: 12;
	} __attribute__((packed));
	uint64_t addr;
} armv8_vaddr;
static_assert(sizeof(armv8_vaddr) == 8);

enum LVL_0_1_2_desc_type {
	D_Block = 0,
	D_Table = 1,
};

enum LVL_3_desc_type {
	D_Reserved = 0,
	D_Page = 1,
};

struct armv8mmu_invalid_desc {
	uint64_t valid:         1, /* MUST be 0 */
		 ignored:       63;
} __attribute__((packed));
static_assert(sizeof(struct armv8mmu_invalid_desc) == 8);

#define m 12
struct armv8mmu_lvl0_table_desc {
	uint64_t valid:         1,
		 type:          1, /* D_Table */
		 ignore_m_1_2:  m - 1 - 2 + 1,
		 next_addr:     47 - m + 1, // corresponding bits of lvl1 table; LSB's are 0
		 res_51_48:     4,
		 ignore_58_52:  7,
		 /* Following 4 fields apply to (and can override) subsequent levels of lookup.
		  * They specify a limit for that subsequent value, rather than the value itself.
		  */
		 PXNTable:      1, // PXN limit
		 XNTable:       1, // XN limit/UXN limit
		 APTable:       2, // access permissions limit
		 NSTable:       1; // security state limit
} __attribute__((packed));
#undef m
static_assert(sizeof(struct armv8mmu_lvl0_table_desc) == 8);

union armv8mmu_lvl0_desc {
	struct armv8mmu_lvl0_table_desc         table;
	struct armv8mmu_invalid_desc            invalid;
};

#define m 12
struct armv8mmu_lvl1_table_desc {
	uint64_t valid:         1,
		 type:          1, /* D_Table */
		 ignore_m_1_2:  m - 1 - 2 + 1,
		 next_addr:     47 - m + 1, // corresponding bits of lvl2 table; LSB's are 0
		 res_51_48:     4,
		 ignore_58_52:  7,
		 /* Following 4 fields apply to (and can override) subsequent levels of lookup.
		  * They specify a limit for that subsequent value, rather than the value itself.
		  */
		 PXNTable:      1, // PXN limit
		 XNTable:       1, // XN limit/UXN limit
		 APTable:       2, // access permissions limit
		 NSTable:       1; // security state limit
} __attribute__((packed));
#undef m
static_assert(sizeof(struct armv8mmu_lvl1_table_desc) == 8);

#define n 30
struct armv8mmu_lvl1_block_desc {
	uint64_t valid:         1,
		 type:          1,  /* D_Block */

	/* lower attributes */
		 AttrIdx:       3, // stage 1 memory attributes index field, for the MAIR_ELx
		 NS:            1, // non-secure bit
		 AP:            2, // access permissions limit for subsequent levels of lookup
	/* NOTE: The ARMv8 translation table descriptor format defines AP[2:1] as
	 * the Access Permissions bits, and does not define an AP[0] bit.
	 */
		 SH:            2, // shareability field
		 AF:            1, // access flag
		 nG:            1, // not-global (TLB ignore ASID field); valid only in EL1&0

		 res_n_1_12:    n - 1 - 12 + 1, // 9
		 addr_o:        47 - n + 1, // corresponding bits of 1GiB block

	/* upper attributes */
		 contiguous:    1, // hint indicating if entry is part of a contiguous set (i.e: cache in single TLB entry)
		 PXN:           1, // privileged execute never; ignored EL2 and EL3
		 XN:            1, // execute-never; in EL1&0 translation, UXN
		 RSW:           4, // reserved for software use
		 ignored:       4;
} __attribute__((packed));
static_assert(sizeof(struct armv8mmu_lvl1_block_desc) == 8);
#undef n

union armv8mmu_lvl1_desc {
	struct armv8mmu_lvl1_block_desc         block;
	struct armv8mmu_lvl1_table_desc         table;
	struct armv8mmu_invalid_desc            invalid;
};

#define m 12
struct armv8mmu_lvl2_table_desc {
	uint64_t valid:         1,
		 type:          1, /* D_Table */
		 ignore_m_1_2:  m - 1 - 2 + 1,
		 next_addr:     47 - m + 1, // corresponding bits of lvl3 table; LSB's are 0
		 res_51_48:     4,
		 ignore_58_52:  7,
		 /* Following 4 fields apply to (and can override) subsequent levels of lookup.
		  * They specify a limit for that subsequent value, rather than the value itself.
		  */
		 PXNTable:      1, // PXN limit
		 XNTable:       1, // XN limit/UXN limit
		 APTable:       2, // access permissions limit
		 NSTable:       1; // security state limit
} __attribute__((packed));
#undef m
static_assert(sizeof(struct armv8mmu_lvl2_table_desc) == 8);

#define n 21
struct armv8mmu_lvl2_block_desc {
	uint64_t valid:         1,
		 type:          1,  /* D_Block */

	/* lower attributes */
		 AttrIdx:       3, // stage 1 memory attributes index field, for the MAIR_ELx
		 NS:            1, // non-secure bit
		 AP:            2, // access permissions
	/* NOTE: The ARMv8 translation table descriptor format defines AP[2:1] as
	 * the Access Permissions bits, and does not define an AP[0] bit.
	 */
		 SH:            2, // shareability field
	/* 00: Non-shareable
	 * 01: UNPREDICTABLE
	 * 10: Outer Shareable
	 * 11: Inner Shareable
	 */
		 AF:            1, // access flag
		 nG:            1, // not-global (TLB ignore ASID field); valid only in EL1&0

		 res_n_1_12:    n - 1 - 12 + 1, // 9
		 addr_o:        47 - n + 1, // corresponding bits of 2MiB block

	/* upper attributes */
		 contiguous:    1, // hint indicating if entry is part of a contiguous set (i.e: cache in single TLB entry)
		 PXN:           1, // privileged execute never; ignored EL2 and EL3
		 XN:            1, // execute-never; in EL1&0 translation, UXN
		 RSW:           4, // reserved for software use
		 ignored:       4;
} __attribute__((packed));
static_assert(sizeof(struct armv8mmu_lvl2_block_desc) == 8);
#undef n

union armv8mmu_lvl2_desc {
	struct armv8mmu_lvl2_block_desc         block;
	struct armv8mmu_lvl2_table_desc         table;
	struct armv8mmu_invalid_desc            invalid;
};

struct armv8mmu_lvl3_page_desc {
	uint64_t valid:         1,
		 type:          1,  /* D_Page */

	/* lower attributes */
		 AttrIdx:       3, // stage 1 memory attributes index field, for the MAIR_ELx
		 NS:            1, // non-secure bit
		 AP:            2, // access permissions limit for subsequent levels of lookup
	/* NOTE: The ARMv8 translation table descriptor format defines AP[2:1] as
	 * the Access Permissions bits, and does not define an AP[0] bit.
	 */
		 SH:            2, // shareability field
		 AF:            1, // access flag
		 nG:            1, // not-global (TLB ignore ASID field); valid only in EL1&0

		 addr_o:        36, // bits[47:12] of paddr
		 res_51_48:     4,

	/* upper attributes */
		 contiguous:    1, // hint indicating if entry is part of a contiguous set (i.e: cache in single TLB entry)
		 PXN:           1, // privileged execute never; ignored EL2 and EL3
		 XN:            1, // execute-never; in EL1&0 translation, UXN
		 RSW:           4, // reserved for software use
		 ignored:       4;
} __attribute__((packed));
static_assert(sizeof(struct armv8mmu_lvl3_page_desc) == 8);

union armv8mmu_lvl3_desc {
	struct armv8mmu_lvl3_page_desc          page;
	struct armv8mmu_invalid_desc            invalid;
};

union armv8_mair_el1 {
	uint8_t fields[8];
	uint64_t val;
};

#define ARMv8MMU_DESC_SIZE      (sizeof(union armv8mmu_lvl0_desc))

/* Access permissions for EL1&0 translations. (AP[2:1])
 * bit 2: set read-only
 * bit 1: allow EL0 access
 * bit 0 is undefined and not present in the descriptor
 */
#define ARMv8MMU_AP_EL0         1
/* Holy fuck throwback to when I spent 2 days debugging because I'd accidentally switched these two
 * values. Make sure you read the docs carefully, kids. Ugh. I even wrote it correctly in the comment
 * above.
 */
#define ARMv8MMU_AP_RW          (0 << 1)
#define ARMv8MMU_AP_RO          (1 << 1)


