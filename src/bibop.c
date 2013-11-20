/* -*- mode: C; c-basic-offset: 4 -*- */
/* ex: set shiftwidth=4 tabstop=4 expandtab: */
/*
 * Copyright (c) 2013, Georgia Tech Research Corporation
 * All rights reserved.
 *
 * Author(s): Neil T. Dantam <ntd@gatech.edu>
 * Georgia Tech Humanoid Robotics Lab
 * Under Direction of Prof. Mike Stilman <mstilman@cc.gatech.edu>
 *
 *
 * This file is provided under the following "BSD-style" License:
 *
 *
 *   Redistribution and use in source and binary forms, with or
 *   without modification, are permitted provided that the following
 *   conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 *   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 *   USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *   ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *   POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* BiBoP-style small-object allocator */

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>


typedef uint16_t bibop_page_size_type;
typedef uint8_t bibop_obj_size_type;

#define BIBOP_PAGE_SIZE 4096
#define BIBOP_PAGE_MASK 0xFFF

struct bibop_page_header;

struct bibop_page_header {
    struct bibop_page_header *next;
    struct bibop_page_header *prev;
    /* Use offset rather than pointers to reduce memory overhead */
    bibop_page_size_type inc_off;
    bibop_page_size_type free_off;
    uint16_t top_index;
    uint16_t sub_index;
    bibop_obj_size_type size;
};

struct bibop_free_item {
    /** List of pages with free space
     */
    struct bibop_page_header *free;

    /** List of maybe full pages.  Possibly unnecessary.
     */
    struct bibop_page_header *full;

    bibop_obj_size_type obj_size;
};


struct bibop_cx {
    /** Reverse lookup table to check if page belongs to this bibop */
    struct bibop_page_header ***pages;

    /** Free list array of length max_size */
    struct bibop_free_item **free_list;

    /** Maximum size of allocations from this bibop */
    bibop_obj_size_type max_size;
    uint16_t top_size;
    uint16_t sub_size;
};

_Bool
bibop_page_belongs( struct bibop_cx *cx, struct bibop_page_header *page );

void *bibop_malloc_cx( struct bibop_cx *cx, bibop_obj_size_type size );

void bibop_free_cx( struct bibop_cx *cx, void *ptr );

/* Check if this page a member of this bibop context */
_Bool
bibop_page_belongs( struct bibop_cx *cx, struct bibop_page_header *page )
{
    uint16_t i = page->top_index;
    uint16_t j = page->sub_index;

    return ( i < cx->top_size        &&
             j < cx->sub_size        &&
             NULL != cx->pages[i]    &&
             page == cx->pages[i][j] );
}

void *bibop_malloc_cx( struct bibop_cx *cx, bibop_obj_size_type size )
{
    if( size >= cx->max_size ) return NULL;

    struct bibop_free_item *item = cx->free_list[size];

    if( NULL == item->free ) {
        /* TODO: cons up a new page and add it */
        return NULL;
    }

    struct bibop_page_header *page = item->free;

    assert( size <= item->obj_size );
    size = item->obj_size;

    void *ptr;

    if( page->inc_off ) {
        bibop_page_size_type new_off = page->inc_off + size;
        ptr = (uint8_t*)page + page->inc_off;
        page->inc_off =  (BIBOP_PAGE_SIZE - new_off > size) ? new_off : 0;
    } else {
        assert( page->free_off );
        ptr = (uint8_t*)page + page->free_off;
        page->free_off = *(bibop_page_size_type*)ptr;
    }

    if( ! page->free_off && ! page->inc_off ) {
        item->free = page->next;
        page->next = item->full;
        item->full = page;
    }

    return ptr;
}

void bibop_free_cx( struct bibop_cx *cx, void *ptr )
{
    /* assume page belongs in the context */
    struct bibop_page_header *page = (void*) ( (intptr_t)ptr & BIBOP_PAGE_MASK );
    _Bool was_full = (0 == page->inc_off) && (0 == page->free_off);
    bibop_page_size_type new_off = (intptr_t)ptr - (intptr_t)page;
    *(bibop_page_size_type*)ptr = page->free_off;
    page->free_off = new_off;

    if( was_full ) {
        struct bibop_free_item *item = cx->free_list[page->size];
        /* remove from full */
        if( page->prev ) page->prev = page->next;
        if( page->next ) page->next->prev = page->prev;
        /* add to free */
        page->next = item->free;
        if( page->next ) page->next->prev = page;
        item->free = page;
    }
}
