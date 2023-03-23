/*
 * kmalloc.c - kernel heap allocator
 *
 * piKOS: a minimal OS for Raspberry Pi 3 & 4
 *     Copyright (C) 2023 Ryan Wenger
 *
 * Code is pretty much verbatim from one of my old undergrad assignments,
 * except for calloc() and realloc(), which were added for piKOS.
 *   Copyright (c) 2022-2023 Ryan Wenger
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

#include "assert.h"

#include "kmalloc.h"
#include "util/memorymap.h"
#include "util/utils.h"

#define RECORD_SIZE             (sizeof(struct malloc_stc))
#define MAX_MALLOC_SIZE         KERN_HEAP_MAXSIZE

// TODO: (consider) changing this to a singly-linked list.
//  Also consider removing the `buf` field since we know
//  the "buffer" will immediately follow the header.
struct malloc_stc {
        struct malloc_stc *prev;
        struct malloc_stc *next;
        void *buf;
        size_t size;
} __attribute__((packed));

static struct malloc_stc *FreeList;
static void *MemBuff = (void *) KERN_HEAP_START;

static void insert_in_FreeList(struct malloc_stc *record, struct malloc_stc *next);
static void remove_from_FreeList(struct malloc_stc *record);

static void *allocate(size_t size, struct malloc_stc *blockptr);
static void coalesce(struct malloc_stc *blockptr);

void init_kmalloc(void)
{
        FreeList = (struct malloc_stc *) MemBuff;
        FreeList->prev = FreeList->next = NULL;
        FreeList->size = sizeof(MemBuff) - RECORD_SIZE;
        FreeList->buf = (uint8_t *) (MemBuff + RECORD_SIZE);
}

void *kmalloc(size_t size)
{
        if (FreeList == NULL)
                return NULL;

	if (size >= MAX_MALLOC_SIZE - RECORD_SIZE)
		return NULL;

        if (size < 1)
                return NULL;

        if ((size & 7) != 0)
                size = ((size >> 3) | 1) << 3;

        // first pass -- look for a block that can be split without hijacking
        for (struct malloc_stc *record = FreeList; record != NULL; record = record->next) {
                if (record->size >= size + RECORD_SIZE + 8)
                        return allocate(size, record);
                // uncomment to implement "first fit" with hijacking (to match autograder tests)
//              else if (record->size >= size)
//                      return allocate(record->size, record);
        }
        // if no such blocks exist, look for one we can hijack
        // second pass -- look for a block large enough to hijack (but not large enough to split)
        for (struct malloc_stc *record = FreeList; record != NULL; record = record->next) {
                if (record->size >= size)
                        return allocate(record->size, record);
        }
        return NULL;
}

void kfree(void *ptr)
{
        struct malloc_stc *to_free = ptr - RECORD_SIZE;
//        assert(to_free->buf != ptr);

        insert_in_FreeList(to_free, NULL);

#ifndef NO_COALESCE
        coalesce(to_free);
#endif
}

void *kcalloc(size_t count, size_t size)
{
	size *= count;
	if (size == 0)
		size = 1;
//	assert(size >= count);

	void *result = kmalloc(size);
	if (result != NULL)
		memset(result, 0, size);

	return result;
}

void *krealloc(void *ptr, size_t size)
{
	if (ptr == NULL)
		return kmalloc(size);

	if (size == 0) {
		kfree(ptr);
		return NULL;
	}

	struct malloc_stc *realloc_blk = ptr - RECORD_SIZE;
	if (realloc_blk->size >= size)
		return ptr;

	/* Not implemented: try to see if memory at the end of our bucket is on the FreeList and can be merged
	 *  with us to get a larger allocation without having to do any copying
	 *  (not currently implemented as I'm not sure how often it'd actually get used)
	 */

	void *newblk = kmalloc(size);
	if (newblk == NULL)
		return NULL;

	memcpy(newblk, realloc_blk->buf, realloc_blk->size);
	kfree(ptr);

	return newblk;
}

static void *allocate(size_t size, struct malloc_stc *blockptr)
{
        // size will either be == blockptr->size or < blockptr->size - sizeof(malloc_stc) - 8
//	assert(size != blockptr->size);
//	assert(size > blockptr->size - RECORD_SIZE - 8);

        if (size < blockptr->size) { // create a new record in the remaining free space
                struct malloc_stc *new_block = (struct malloc_stc *) (blockptr->buf + size);
                new_block->buf = (uint8_t *) (new_block + 1);
                new_block->size = blockptr->size - size - RECORD_SIZE;
                blockptr->size = size;
                insert_in_FreeList(new_block, blockptr->next);
        }

        remove_from_FreeList(blockptr);

        return blockptr->buf;
}

/**
 * @brief Attempt to merge the record at `*blockptr` with any adjacent free blocks
 * @param blockptr Pointer to the record of the block to be merged with its neighbors
 */
static void coalesce(struct malloc_stc *blockptr)
{
        // `void *` casts here to simply silence compiler warnings about conflicting ptr types
        if (blockptr->next == blockptr->buf + blockptr->size) {
                blockptr->size += RECORD_SIZE + blockptr->next->size;
                remove_from_FreeList(blockptr->next);
        }

        if (blockptr->prev && blockptr == blockptr->prev->buf + blockptr->prev->size) {
                blockptr->prev->size += RECORD_SIZE + blockptr->size;
                remove_from_FreeList(blockptr);
        }
}

/**
 * @brief Inserts `record` into the position prior to the `next` parameter in `FreeList`
 * @note A value of `NULL` for the `next` parameter will cause the function to search
 *      for the correct position in the sorted order of the list
 * @param record The record to insert
 * @param next The node behind which to insert the record
 */
static void insert_in_FreeList(struct malloc_stc *record, struct malloc_stc *next)
{
        if (FreeList == NULL) {
                FreeList = record;
                record->next = record->prev = NULL;
                return;
        }

        /* Search for the node to insert the record behind.
         * We want the list to be sorted according to each records' buffer,
         * but we can also use the records themselves since each immediately precedes its buffer
         */
        if (next == NULL)
                for (next = FreeList; next->next && next < record; next = next->next) {}
                        // NOTE: could add check for double-free here by testing next == record

        if (next < record) { // special case -- append to the very end of the list
                next->next = record;
                record->prev = next;
                record->next = NULL;
        } else { // insert prior to the `next` node
                record->prev = next->prev;

                if (next == FreeList)
                        FreeList = record;
                else
                        next->prev->next = record;

                record->next = next;
                next->prev = record;
        }
}

static void remove_from_FreeList(struct malloc_stc *record)
{
        if (record->next)
                record->next->prev = record->prev;

        if (record->prev)
                record->prev->next = record->next;
        else
                FreeList = record->next;

        record->next = record->prev = NULL;
}

#if 0
#include "peripherals/mini_uart.h"
/* test code from Rich Wolski's CS170 Lab 0 */
void test_kmalloc(void)
{
	int size;
	void *ptr[10];
	int i;
	/*
	 * try mallocing four pieces, each 1/4 of total size
	 */
	size = MEM_HEAP_MAXSIZE / 4;

	ptr[0] = kmalloc(size);
	if(ptr[0] == NULL)
	{
		muart_send_str("malloc of ptr[0] failed for size %d\r\n");
		return;
	}

	ptr[1] = kmalloc(size);
	if(ptr[1] == NULL)
	{
		muart_send_str("malloc of ptr[1] failed for size %d\r\n");
		return;
	}

	ptr[2] = kmalloc(size);
	if(ptr[2] == NULL)
	{
		muart_send_str("malloc of ptr[2] failed for size %d\r\n");
		return;
	}

	/*
	 * this one should fail due to rounding
	 */
	ptr[3] = kmalloc(size);
	if(ptr[3] == NULL)
	{
		muart_send_str("malloc of ptr[3] fails correctly for size %d\r\n");
	}
	/*
	 * free the first block
	 */
	kfree(ptr[0]);

	/*
	 * free the third block
	 */
	kfree(ptr[2]);

	/*
	 * now free second block
	 */
	kfree(ptr[1]);

	/*
	 * re-malloc first pointer
	 */
	ptr[0] = kmalloc(size);
	if(ptr[0] == NULL)
	{
		muart_send_str("re-malloc of ptr[0] failed for size %d\r\n");
		return;
	}

	/*
	 * try splitting the second block
	 */
	ptr[1] = kmalloc(size/2);
	if(ptr[1] == NULL)
	{
		muart_send_str("split second block ptr[1] failed for size %d\r\n");
		return;
	}

	/*
	 * free first block and split of second
	 */
	kfree(ptr[0]);
	kfree(ptr[1]);

	/*
	 * try mallocing a little less to make sure no split occurs
	 * first block from previous print should not be split
	 */
	ptr[0] = kmalloc(size-1);
	if(ptr[0] == NULL)
	{
		muart_send_str("slightly smaller malloc of ptr[0] failed for size %d\r\n");
		return;
	}

	/*
	 * free it and make sure it comes back as the correct size
	 */
	kfree(ptr[0]);

	/*
	 * okay, now see if multiples work
	 */
	for(i=0; i < 6; i++)
	{
		ptr[i] = kmalloc(100);
	}

	/*
	 * free first block, third block, fifth block
	 */
	kfree(ptr[0]);
	kfree(ptr[2]);
	kfree(ptr[4]);

	/*
	 * now, free second block -- first, second, third blocks
	 * should coalesce
	 */
	kfree(ptr[1]);

	/*
	 * free the sixth block and it should merge with the last
	 * block leaving two
	 */
	kfree(ptr[5]);

	/*
	 * now free fourth block and they should all be together
	 */
	kfree(ptr[3]);

	muart_send_str("made it -- passed kmalloc test\r\n");
}
#endif