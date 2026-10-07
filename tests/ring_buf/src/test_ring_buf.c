/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	int ret;

	ret = rb_push(99);
	zassert_equal(ret, 0, "rb_push(99) should succeed");

	ret = rb_init(4);
	zassert_equal(ret, 0, "rb_init(4) should succeed");

	zassert_true(rb_is_empty(), "Buffer must be empty after reinit");
	zassert_equal(rb_count(), 0, "Buffer count must be 0 after reinit");
}
/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int v;
	int ret;

	ret = rb_push(42);
	zassert_equal(ret, 0, "rb_push(42) should succeed");

	ret = rb_pop(&v);
	zassert_equal(ret, 0, "rb_pop(&v) should succeed");
	zassert_equal(v, 42, "Popped value must be 42");

	zassert_true(rb_is_empty(), "Buffer must be empty after pop");
}
ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int v;
	int ret;

	ret = rb_push(1);
	zassert_equal(ret, 0, "rb_push(1) should succeed");

	ret = rb_push(2);
	zassert_equal(ret, 0, "rb_push(2) should succeed");

	ret = rb_push(3);
	zassert_equal(ret, 0, "rb_push(3) should succeed");

	ret = rb_pop(&v);
	zassert_equal(ret, 0, "First pop should succeed");
	zassert_equal(v, 1, "First pop must return 1");

	ret = rb_pop(&v);
	zassert_equal(ret, 0, "Second pop should succeed");
	zassert_equal(v, 2, "Second pop must return 2");

	ret = rb_pop(&v);
	zassert_equal(ret, 0, "Third pop should succeed");
	zassert_equal(v, 3, "Third pop must return 3");

	zassert_true(rb_is_empty(), "Buffer must be empty after three pops");
}
ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	int ret;

	ret = rb_push(1);
	zassert_equal(ret, 0, "rb_push(1) should succeed");

	ret = rb_push(2);
	zassert_equal(ret, 0, "rb_push(2) should succeed");

	ret = rb_push(3);
	zassert_equal(ret, 0, "rb_push(3) should succeed");

	ret = rb_push(4);
	zassert_equal(ret, 0, "rb_push(4) should succeed");

	zassert_true(rb_is_full(), "Buffer must be full after four pushes");

	ret = rb_push(99);
	zassert_equal(ret, -ENOSPC,
		      "Push past capacity must return -ENOSPC");

	zassert_equal(rb_count(), 4,
		      "Rejected push must not change count");
}
/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int v;
	int ret;

	ret = rb_push(7);
	zassert_equal(ret, 0, "rb_push(7) should succeed");

	ret = rb_peek(&v);
	zassert_equal(ret, 0, "First peek should succeed");
	zassert_equal(v, 7, "First peek must return 7");

	ret = rb_peek(&v);
	zassert_equal(ret, 0, "Second peek should succeed");
	zassert_equal(v, 7, "Second peek must return 7");

	zassert_equal(rb_count(), 1,
		      "Peek must not consume the buffer element");
}
ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	int ret;

	ret = rb_pop(NULL);
	zassert_equal(ret, -EINVAL,
		      "rb_pop(NULL) must return -EINVAL");
}
ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	int ret;

	ret = rb_push(1);
	zassert_equal(ret, 0, "rb_push(1) should succeed");

	ret = rb_push(2);
	zassert_equal(ret, 0, "rb_push(2) should succeed");

	ret = rb_push(3);
	zassert_equal(ret, 0, "rb_push(3) should succeed");

	ret = rb_push(4);
	zassert_equal(ret, 0, "rb_push(4) should succeed");

	zassert_true(rb_is_full(), "Buffer must be full");
	zassert_equal(rb_count(), 4, "Buffer count must be 4");
}
