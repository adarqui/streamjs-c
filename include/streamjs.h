#ifndef STREAMJS_H
#define STREAMJS_H

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ret.h>

typedef enum {
	BOOL_FALSE,
	BOOL_TRUE
} bool_t;

typedef enum {
	STATUS_OK = 0,
	STATUS_ERR = -1
} STATUS;

typedef struct stream {
	void * headValue;
	void * originalValue;
	void * arg;
	struct stream * next;
	ret_t (*promise)(void *, void *);
} stream_t;

typedef struct stream_range_type {
	long low;
	long high;
	long by;
	long dir;
} stream_range_t;

stream_t * stream_new(void *, ret_t (*cb)(void *, void *), void *, stream_t *);
stream_t * stream_make(void *, ...);
stream_t * stream_append(stream_t *, stream_t *);
bool_t stream_empty(stream_t *);
long stream_length(stream_t *);
void * stream_head(stream_t *);
stream_t * stream_tail(stream_t *);
void * stream_item(stream_t *, long);
RET_BOOL_t stream_member(stream_t *, void *);
stream_t * stream_map(stream_t *, ret_t (*f)(void *, void *), void *);
void stream_reset(stream_t *);
ret_t stream_range_incr(void *, void *);
stream_t * stream_range(long, long, long);
stream_t * stream_take(stream_t *, long);
stream_t * stream_drop(stream_t *, long);
stream_t * stream_filter(stream_t *, ret_t (*filter)(void *, void *), void *);
stream_t * stream_filter_dup(stream_t *, ret_t (*filter)(void *, void *), void *);

void stream_dump(stream_t *, void (*prlonger)(void *));

/*
 * some generic iterators
 */
ret_t stream_generic_adder(void *, void *);
ret_t stream_generic_subber(void *, void *);
ret_t stream_generic_incr(void *, void *);
ret_t stream_generic_decr(void *, void *);
ret_t stream_generic_scaler(void *, void *);

ret_t stream_generic_filter_even(void *, void *);

#endif
