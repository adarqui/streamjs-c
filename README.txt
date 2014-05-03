lazy list evaluation library in C.

	make deps
	make test


Experimenting with lazy evaluation in C... Attempted to port streamjs to C -> proved a little more difficult than I initially thought. Plenty of lazyness achieved (check test.c for examples/usage). However, need to re-think several aspects of this and figure out what I want to do.

I may also port this to go, which may prove easier with closures/scope/more generous memory allocation/garbage collection.. Perhaps then streamjs-go might look very similar to stream.js.

I will drop one example in here. This is an example of lazy eval of a stream_t type using stream_range() & stream_member(). First we create a stream from 1 to 500,000 (long's). However, this stream only contains one element instead of 500,000. stream_member() will lazily evaluate the stream until it finds 500,000.

void test_stream_member(void) {
	stream_t *st;

	st = stream_range(1, 500000, 1);
	CU_ASSERT(stream_member(st, (void *)500000) == RET_BOOL_TRUE);

	return;
}

Ok one more example. You can chain 'elements' and 'lazy iterators' together. Here you can see we chain together a string ("HELLO"), a lazy list of 1000 numbers, another string ("GET TO THE CHOPPA"), and another lazy list of 10 numbers.

	st = stream_new((void *)"HELLO", NULL, NULL,
			stream_new((void *)1, stream_generic_adder, (void *)1000,
				stream_new((void *)"GET TO THE CHOPPA", NULL, NULL,
					stream_new((void *)1, stream_generic_adder, (void *)10, NULL))));

	Thus, we'll end up with a lazy list of 4 elements which lazily evluate to 1012 elements.


Supported functions:

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

ret_t stream_generic_adder(void *, void *);
ret_t stream_generic_subber(void *, void *);
ret_t stream_generic_incr(void *, void *);
ret_t stream_generic_decr(void *, void *);
ret_t stream_generic_scaler(void *, void *);

ret_t stream_generic_filter_even(void *, void *);


pc
