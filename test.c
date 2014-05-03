#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <ret.h>
#include "streamjs.h"
#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

int init_suite(void);
int clean_suite(void);
void test_init(CU_pSuite);
void test_stream_new(void);
void test_stream_head(void);
void test_stream_tail(void);
void test_stream_length(void);
void test_stream_map(void);
void test_stream_reset(void);
void test_stream_item(void);
void test_stream_range(void);
void test_stream_take(void);
void test_stream_drop(void);
void test_stream_member(void);
void test_stream_filter(void);
void test_stream_filter_dup(void);
void test_example_1(void);
void test_example_2(void);

int init_suite(void) {
	return 0;
}

int clean_suite(void) {
	return 0;
}

/*
 * specific cunit tests
 */

void test_init(CU_pSuite p) {
	CU_add_test(p, "test stream_new", test_stream_new);
	CU_add_test(p, "test stream_head", test_stream_head);
	CU_add_test(p, "test stream_tail", test_stream_tail);
	CU_add_test(p, "test stream_map", test_stream_map);
	CU_add_test(p, "test stream_length", test_stream_length);
	CU_add_test(p, "test stream_item", test_stream_item);
	CU_add_test(p, "test stream_reset", test_stream_reset);
	CU_add_test(p, "test stream_range", test_stream_range);
	CU_add_test(p, "test stream_take", test_stream_take);
	CU_add_test(p, "test stream_drop", test_stream_drop);
	CU_add_test(p, "test stream_member", test_stream_member);
	CU_add_test(p, "test stream_filter", test_stream_filter);
	CU_add_test(p, "test stream_filter_dup", test_stream_filter_dup);
	CU_add_test(p, "test example 1", test_example_1);
	CU_add_test(p, "test example 2", test_example_2);
	return;
}

void test_stream_new(void) {
	stream_t * st;

	st = stream_new((void *)1, NULL, NULL, NULL);
	CU_ASSERT(st != NULL);

	st = stream_new(
			(void *)1, NULL, NULL,
				stream_new((void *)2, NULL, NULL,
					stream_new((void *)3, NULL, NULL, NULL)));

	CU_ASSERT(st != NULL);

	st = stream_new((void *)1, stream_generic_adder, NULL, NULL);
}

void test_stream_head(void) {
	stream_t *st;
	long i;

	st = stream_new((void *)100, stream_generic_adder, NULL, NULL);
	for (i = 0; i < 100; i++) {
		st = stream_tail(st);
		if(!st) break;
	}
	CU_ASSERT(stream_head(st) == (long *) 200);

	st = stream_new((void *)1, NULL, NULL, NULL);
	CU_ASSERT(stream_head(st) == (long *) 1);
}

void test_stream_tail(void) {
	return;
}

void test_stream_map(void) {
	stream_t *st;
	st = stream_new((void *)1, stream_generic_adder, NULL, NULL);
	st = stream_map(st, stream_generic_adder, NULL);
	CU_ASSERT((void *)stream_head(st) == (void *)2);
	st = stream_map(st, stream_generic_incr, (void *)2);
	CU_ASSERT((void *)stream_head(st) == (void *)4);
	return;
}

void test_stream_length(void) {
	stream_t *st;
	
	st = stream_new((void *)"HELLO", NULL, NULL, NULL);
	CU_ASSERT(stream_length(st) == 1);

	st = stream_new((void *)1, stream_generic_adder, (void *)1000, NULL);
	CU_ASSERT(stream_length(st) == 1001);

	st = stream_new((void *)"HELLO", NULL, NULL,
			stream_new((void *)1, stream_generic_adder, (void *)1000, NULL));
	CU_ASSERT(stream_length(st) == 1002);

	st = stream_new((void *)"HELLO", NULL, NULL,
			stream_new((void *)1, stream_generic_adder, (void *)1000,
				stream_new((void *)"GET TO THE CHOPPA", NULL, NULL,
					stream_new((void *)1, stream_generic_adder, (void *)10, NULL))));

	CU_ASSERT(stream_length(st) == 1013);
	stream_reset(st);
	CU_ASSERT(stream_length(st) == 1013);
}

void test_stream_item(void) {
	stream_t *st;
	st = stream_new((void *)1, stream_generic_adder, (void *)100, NULL);
	CU_ASSERT((void *)stream_item(st, 49) == (void *)50);
	stream_reset(st);
	CU_ASSERT((void *)stream_item(st, 49) == (void *)50);
	return;
}

void test_stream_append(void) {
	return;
}

void test_stream_reset(void) {
	return;
}

void test_stream_range(void) {
	stream_t *st;

	st = stream_range(1, 1000, 1);
	CU_ASSERT(stream_length(st) == 1000);
	stream_reset(st);

	st = stream_range(2, 1000, 2);
	CU_ASSERT(stream_length(st) == 500);
	stream_reset(st);

	st = stream_range(1000, 1, 1);
	CU_ASSERT(stream_length(st) == 1000);
	stream_reset(st);

	return;
}

void test_stream_take(void) {
	stream_t *st;
	
	st = stream_range(1, 1000, 1);
	st = stream_take(st, 10);
	CU_ASSERT(stream_length(st) == 10);
	
	return;
}

void test_stream_drop(void) {
	stream_t *st;

	st = stream_range(1, 1000, 1);
	st = stream_drop(st, 10);
	CU_ASSERT(stream_length(st) == 990);

	return;
}

void test_stream_member(void) {
	stream_t *st;

	st = stream_range(1, 500000, 1);
	CU_ASSERT(stream_member(st, (void *)500000) == RET_BOOL_TRUE);

	return;
}

void test_stream_filter(void) {
	stream_t *st;
	long count = 0;
	
	puts("Testing even filter (0-10):");
	st = stream_range(0, 10, 1);
	while(!stream_empty(st)) {
		st = stream_filter(st, stream_generic_filter_even, NULL);
		if(!st) break;
		printf("\t%p\n", (void *)stream_head(st));
		CU_ASSERT((void *)stream_head(st) == (void *)count);
		count += 2;
		st = stream_tail(st);
	}

	return;
}

void test_stream_filter_dup(void) {
	stream_t *st;

	st = stream_filter_dup(stream_range(1, 100, 1), stream_generic_filter_even, NULL);
	CU_ASSERT(stream_length(st) == 50);

	return;
}

void test_example_1(void) {
	stream_t *st;

	st = stream_make((void *)10, (void *)20, (void *)30, NULL);
	CU_ASSERT((void *)stream_item(st, 0) == (void *)10);
	CU_ASSERT((void *)stream_item(st, 1) == (void *)20);
	CU_ASSERT((void *)stream_item(st, 2) == (void *)30);
}

void test_example_2(void) {
	stream_t *st;

	st = stream_make((void *)10, (void *)20, (void *)30, NULL);
	CU_ASSERT((void *)stream_head(st) == (void *)10);
	st = stream_tail(st);
	CU_ASSERT((void *)stream_head(st) == (void *)20);
	st = stream_tail(st);
	CU_ASSERT((void *)stream_head(st) == (void *)30);

	return;
}

int main(int argc, char *argv[]) {
	CU_pSuite p = NULL;
	if(CU_initialize_registry() != CUE_SUCCESS) {
		errx(1, "CU_initialize_registry: Failed: %i\n", CU_get_error());
	}

	p = CU_add_suite("streamjs-c tests", init_suite, clean_suite);
	if(p == NULL) {
		CU_cleanup_registry();
		errx(1, "CU_add_suite: Failed: %i\n", CU_get_error());
	}

	test_init(p);
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	CU_cleanup_registry();

	return CU_get_error();
}
