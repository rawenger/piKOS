/*
 * vm_kernel.c
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *  Copyright (C) 2023 Ryan Wenger
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

#include <assert.h>
#include "vm_kernel.h"
#include "types.h"
#include "mmio.h"
#include "armv8mmu.h"
#include "util/memorymap.h"
#include "util/utils.h"

// https://developer.arm.com/documentation/ddi0595/2021-12/AArch64-Registers/MAIR-EL1--Memory-Attribute-Indirection-Register--EL1-?lang=en#fieldset_0-63_0
//#define ARMv8MMU_MAIR_KERN      0xFF // inner/outer write-back non-transient, allocating; don't test MMU with this on
#define ARMv8MMU_MAIR_KERN      0x44 // normal memory, outer/inner non-cacheable (default behavior when MMU is off)
#define ARMv8MMU_MAIR_MMIO      0x0  // Device nGnRnE

#define KERNEL_MAIR_IDX         1
#define MMIO_MAIR_IDX           2

static void map_kernel();
static void map_mmio();

// helper to convert descriptor pointer to field to put in parent table descriptor
static inline u64 get_next_lvl_bits_tab(void *pDesc)
{
	u64 val = (u64) pDesc;
	/* we use bits 47:12 for a 4K granule size */
	return ((val & ~(((u64) -1L) << 48)) >> 12);
}

static inline u64 get_next_lvl_bits_block2(void *pDesc)
{
	u64 val = (u64) pDesc;
	/* we use bits 47:21 for a 4K granule size */
	return ((val & ~(((u64) -1L) << 48)) >> 21);
}

static inline u64 get_next_lvl_bits_block1(void *pDesc)
{
	u64 val = (u64) pDesc;
	/* we use bits 47:21 for a 4K granule size */
	return ((val & ~(((u64) -1L) << 48)) >> 30);
}

extern void *pg_root;

/* physical address of kernel base page table(s) */
union armv8mmu_lvl0_desc *kern_pt_base_pm =
	(union armv8mmu_lvl0_desc *) (PAGETABLE_START_PHYS);

/* virtual address of kernel base page table(s) */
//union armv8mmu_lvl0_desc *kern_pt_base_vm =
//	(union armv8mmu_lvl0_desc *) &pg_root;

/*
 * This function sets up and configures initial EL1&0 kernel address translation.
 * Basically it just maps the kernel image, stack, and MMIO to 3 2MB sections, respectively.
 * We are running very bare-bones here, and I decided to implement this in this way late
 * (i.e: after I'd spent hours digging through documentation and creating all those nice
 * page table definitions over in armv8mmu.h), so I'm doing this in C rather than assembler
 * for that reason.
 */
void EL2_MMU_bootstrap(void)
{
	/* TODO: refactor most of this */
	// zero the initial ptable area in the kernel image
	memset(kern_pt_base_pm, 0, KERN_PGDIR_SIZE);

	union armv8mmu_lvl1_desc *kern_l1 =
		(union armv8mmu_lvl1_desc *) (kern_pt_base_pm + PAGESIZE);

	union armv8mmu_lvl2_desc *kern_l2 =
		(union armv8mmu_lvl2_desc *) (kern_pt_base_pm + 2 * PAGESIZE);

	size_t table_idx;

	/* build the page tables */
	struct armv8mmu_lvl0_table_desc root = {
		.valid = 1, .type = D_Table,
		.next_addr = get_next_lvl_bits_tab(kern_l1),
		.PXNTable = 0, .XNTable = 1,
		.APTable = ARMv8MMU_AP_RW, .NSTable = 1, // NSTable = 1 overwrites lower levels
	};
	table_idx = ((armv8_vaddr) KERN_VM_BASE).L0;
	kern_pt_base_pm[table_idx].table = root;

	/* level 1 pagetable #0 (used to map kernel RAM) */
	struct armv8mmu_lvl1_table_desc lvl1_0 = {
		.valid = 1, .type = D_Table,
		.next_addr = get_next_lvl_bits_tab(kern_l2),
		.PXNTable = 0, .XNTable = 1,
		.APTable = ARMv8MMU_AP_RW, .NSTable = 1, // NSTable = 1 overwrites lower levels
	};
	table_idx = ((armv8_vaddr) KERN_VM_BASE).L1;
	kern_l1[table_idx].table = lvl1_0;

	/* level 1 pagetable #511 (used to map MMIO peripherals)
	 * Technically this also maps the 1GiB physical memory in which
	 * the MMIO is contained as well (so all of phys mem on Pi 3B),
	 * but it's a temporary solution. For testing. And will eventually
	 * be done away with.
	 */

	struct armv8mmu_lvl1_block_desc devmem = {
		.valid = 1, .type = D_Block,
		.AttrIdx = MMIO_MAIR_IDX,
		.NS = 1, .AP = ARMv8MMU_AP_RW,
		.SH = 2, // outer shareable
		.AF = 1, .nG = 0,
		.addr_o = get_next_lvl_bits_block1((void *) MMIO_BASE),
		.PXN = 1, .XN = 1,
	};

	/* map MMIO_VM_BASE to MMIO_PHYS_BASE */
	table_idx = ((armv8_vaddr) MMIO_VM_OFFSET).L1;
	assert(table_idx == 511);
	kern_l1[table_idx].block = devmem;

	/* 2MB block descriptor for kernel image. Can maybe consider changing this
	 * to map .data separately, but it would require lots of tampering with
	 * the linker script and potentially some runtime analysis to determine mappings.
	 */
	struct armv8mmu_lvl2_block_desc kimg = {
		.valid = 1, .type = D_Block,
		.AttrIdx = KERNEL_MAIR_IDX,
		.NS = 1, .AP = ARMv8MMU_AP_RW,
		.SH = 0, /* since we are single-core, we can set this to non-shareable. */
		.AF = 1, .nG = 0,
		.addr_o = get_next_lvl_bits_block2((void *) KERN_IMG_START_PHYS),
		.PXN = 0, .XN = 1,
	};

	// TODO: change this to individual pages
	struct armv8mmu_lvl2_block_desc kstack = {
		.valid = 1, .type = D_Block,
		.AttrIdx = KERNEL_MAIR_IDX,
		.NS = 1, .AP = ARMv8MMU_AP_RW,
		.SH = 0,
		.AF = 1, .nG = 0,
		.addr_o = get_next_lvl_bits_block2((void *) KERN_STACK_BASE_PHYS),
		.PXN = 1, .XN = 1,
	};

	// TODO: map these page tables somewhere, because currently EL1 cannot access them,
	//  which is a problem.
	struct armv8mmu_lvl2_block_desc kern_pgdir = {

	};

	/* map KERN_VM_BASE to &_start (0x80000) */
	table_idx = ((armv8_vaddr) KERN_VM_BASE).L2;
	kern_l2[table_idx].block = kimg;

	/* map kernel stack (temporary, for testing) */
	table_idx = ((armv8_vaddr) KERN_STACK_BASE_VM).L2;
	kern_l2[table_idx].block = kstack;

	// setup memory attributes in MAIR
	union armv8_mair_el1 mairEL1 = {0};
	mairEL1.fields[KERNEL_MAIR_IDX] = ARMv8MMU_MAIR_KERN;
	mairEL1.fields[MMIO_MAIR_IDX] = ARMv8MMU_MAIR_MMIO;
	asm volatile ("msr mair_el1, %0" : : "r" (mairEL1.val));

	asm volatile ("msr ttbr1_el1, %0" : : "r" (kern_pt_base_pm));

	u64 tcr_el1;
	asm volatile ("mrs %0, tcr_el1" : "=r" (tcr_el1));
	tcr_el1 &= ~(
		0
	); // TODO
	tcr_el1 |= (
//		(63 - 48) | // t0 size
		(16 << 16) | // T1SZ = 16
		//(0 << 14) | // TG0 = 4K
		(2L << 30) // TG1 = 4K
	); // TODO: clean this up/revisit/add #defines

	asm volatile ("msr tcr_el1, %0" : : "r" (tcr_el1));
	asm volatile ("isb" : : : "memory");

	u64 sctlr_el1;
	asm volatile ("mrs %0, sctlr_el1" : "=r" (sctlr_el1));
	sctlr_el1 &= ~(( 1<< 19) | (1<<1) | (1<<2) | (1<<12));
	sctlr_el1 |= 1; // enable MMU
	asm volatile ("msr sctlr_el1, %0" : : "r" (sctlr_el1));
	asm volatile ("isb" : : : "memory");

}

void init_mmu(void *x0)
{



//	map_kernel();
}

static void map_kernel()
{}
