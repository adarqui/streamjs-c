#include "streamjs.h"

stream_t * stream_new(void *head, ret_t (*promise)(void *, void *), void *arg, stream_t *next) {
	stream_t *st;

	st = (stream_t *) calloc(1, sizeof(stream_t));

	st->headValue = st->originalValue = head;
	st->arg = arg;
	st->promise = promise;
	st->next = next;

	return st;
}

stream_t * stream_make(void *v, ...) {
	stream_t *p, *p2, *st;
	va_list ap;
	
	p = st = NULL;
	va_start(ap, v);
	while(v != NULL) {
		p = stream_new(v, NULL, NULL, NULL);
		v = va_arg(ap, void *);
		if(!st) {
			st = p2 = p;
		} else {
			stream_append(p2, p);
			p2 = p;
		}
	}
	va_end(ap);

	return st;
}

bool_t stream_empty(stream_t *st) {
//	if(!st || !st->headValue) return BOOL_TRUE;
	if(!st) return BOOL_TRUE;

	return BOOL_FALSE;
}

void * stream_head(stream_t *st) {
	return st->headValue;
}

stream_t * stream_tail(stream_t *st) {
	RET_INIT;

	if(!st || stream_empty(st) == BOOL_TRUE) {
		return NULL;
	}

	if(st->promise) {
		_r = st->promise(st->headValue, st->arg);
		if (RET_ISBREAK == RET_BOOL_TRUE) {
			if(st->next) {
				return stream_tail(st->next);
			} else {
				return NULL;
			}
		}
		st->headValue = RET_V;
	} else {
		return st->next;
	}

	return st;
}


void * stream_item(stream_t *st, long n) {

	if(n < 0 || stream_empty(st)) {
		return NULL;
	}

	while(n != 0) {
		--n;
		st = stream_tail(st);
	}

	if(st) {
		return stream_head(st);
	}

	return NULL;
}

long stream_length(stream_t *st) {
	/* requires finite stream */
	long len = 0;

	while(stream_empty(st) != BOOL_TRUE) {
		len++;
		st = stream_tail(st);
	}

	return len;
}

void stream_reset(stream_t *st) {
	while(stream_empty(st) != BOOL_TRUE) {
		st->headValue = st->originalValue;
		st = st->next;
	}
	return;
}

stream_t * stream_last(stream_t *st) {
	stream_t * p;
	if(!st) return NULL;
	for(p = st; p->next != NULL; p = p->next) {
	}
	return p;
}

stream_t * stream_append(stream_t *st, stream_t *new) {
	stream_t *middle;

	if(stream_empty(st) == BOOL_TRUE) {
		return new;
	}

	middle = st->next;
	new->next = middle;
	st->next = new;

	return st;
}

/*
stream_t * stream_map(stream_t *st, ret_t (*f)(void *, void *), void *arg) {
	RET_INIT;
	void *v;

	v = stream_head(st);
	if(!arg) {
		arg = st->arg;
	}

	_r = f(v, arg);
	st->headValue = RET_V;
	return st;
}
*/

stream_t * stream_map(stream_t *st, ret_t (*f)(void *, void *), void *arg) {
	RET_INIT;
	void *v;

	v = stream_head(st);
	if(!arg) {
		arg = st->arg;
	}

	_r = f(v, arg);
	st->headValue = RET_V;
	return st;
}

stream_t * stream_reduce(stream_t *st, ret_t (*f)(void *, void *), void *arg) {
	RET_INIT;
	void *v;

	v = stream_head(st);
	if(!arg) {
		arg = st->arg;
	}

	_r = f(v, arg);
	st->headValue = RET_V;
	return st;
}

stream_t * stream_scale(stream_t *st, long factor) {
	/*
	void * v;
	v = stream_head(st);
	*/
	return NULL;
}

//stream_t * stream_apply(stream_t *st, 

ret_t stream_range_incr(void *k, void *arg) {
	RET_INIT;
	stream_range_t *r = (stream_range_t *) arg;
	if(r->dir != (-1)) {
		/* low to high */
		if((void *)k >= (void *)r->high) {
			RET_BREAK;
		}
		k = k + r->by;
		RET_OK(k);
	} else {
		/* high to low */
		if((void *)k <= (void *)r->high) {
			RET_BREAK;
		}
		k = k - r->by;
		RET_OK(k);
	}
}

stream_t * stream_range(long low, long high, long by) {
	stream_t * st;
	stream_range_t *r;

	if(!by) {
		errx(1, "stream_range: by can't be 0");
	}

	r = calloc(1, sizeof(stream_range_t));
	r->low = low;
	r->high = high;
	r->by = by;

	if(r->low > r->high) {
		r->dir = (-1);
	}

	st = stream_new((void *)low, stream_range_incr, r, NULL);
	return st;
}


stream_t * stream_take(stream_t *st, long howmany) {
	stream_t *p, *n, *n2;
	void *v;

	if(!st) {
		return NULL;
	}

	p = n = n2 = NULL;
	v = stream_head(st);
	while(howmany > 0) {
		n = stream_append(n, stream_new(v, NULL, NULL, NULL));
		if(!n2) {
			n2 = n;
		}
		p = stream_tail(st);
		if(!p) break;
		howmany--;
	}

	return n2;
}

stream_t * stream_drop(stream_t *st, long howmany) {
	stream_t *p;

	if(!st) {
		return NULL;
	}

	p = NULL;
	while(howmany > 0) {
		p = stream_tail(st);
		if(!p) break;
		howmany--;
	}

	return p;
}

RET_BOOL_t stream_member(stream_t *st, void *memb) {
	while(!stream_empty(st)) {
		if(stream_head(st) == memb) {
			return RET_BOOL_TRUE;
		}
		st = stream_tail(st);
	}
	return RET_BOOL_FALSE;
}

stream_t * stream_filter(stream_t *st, ret_t (*filter)(void *, void *), void *arg) {
	RET_INIT;
	void *narg;

	if(stream_empty(st)) {
		return NULL;
	}

	while(1) {
		if(!arg) {
			narg = st->arg;
		}
		_r = filter(stream_head(st), narg);
		if((void *)RET_V == (void *)RET_BOOL_TRUE) {
			return st;
		}
		st = stream_tail(st);
	}

	return st;
}

stream_t * stream_filter_dup(stream_t *st, ret_t (*filter)(void *, void *), void *arg) {
	stream_t *p, *n, *nst;

	p = n = nst = NULL;

	while(!stream_empty(st)) {
		p = stream_filter(st, filter, arg);
		if(!p) {
			break;
		}
		n = stream_new(stream_head(p), NULL, NULL, NULL);
		nst = stream_append(nst, n);
		st = stream_tail(st);
	}

	return nst;
}

void stream_dump(stream_t *st, void (*prlonger)(void *)) {
	if(!st) {
		return;
	}
	printf("Dumping. Length=%lu\n", stream_length(st));
	while (stream_empty(st) != BOOL_TRUE) {
		if(prlonger) {
			prlonger(stream_head(st));
		} else {
			printf("\thead: %p\n", stream_head(st));
		}
		st = stream_tail(st);
	}
	return;
}

ret_t stream_generic_adder(void *k, void *arg) {
	RET_INIT;
	if(arg) {
		long lim = (long)arg;
		if((long)k > lim) {
			RET_BREAK;
		}
	}
	k = k + 1;
	RET_OK(k);
}


ret_t stream_generic_subber(void *k, void *arg) {
	RET_INIT;
	if(arg) {
		long lim = (long)arg;
		if((long)k < lim) {
			RET_BREAK;
		}
	}
	k = k - 1;
	RET_OK(k);
}


ret_t stream_generic_incr(void *k, void *arg) {
	RET_INIT;
	long i = 1;
	if(arg) {
		i = (long)arg;
	}
	k = k + i;
	RET_OK(k);
}

ret_t stream_generic_decr(void *k, void *arg) {
	RET_INIT;
	long i = 1;
	if(arg) {
		i = (long)arg;
	}
	k = k - i;
	RET_OK(k);
}

ret_t stream_generic_scaler(void *k, void *arg) {
	RET_INIT;
	long v = (long) k, factor = (long) arg;
	if(arg) {
		long factor = (long)arg;
		v = v * factor;
		RET_OK((void *)v);
	}
	RET_ERROR(k);
}


ret_t stream_generic_filter_even(void *k, void *arg) {
	RET_INIT;

	if(((long)k % 2) == 0) {
		RET_OK((void *)RET_BOOL_TRUE);
	}

	RET_OK(RET_BOOL_FALSE);
}
