/*
 * Created by Costa Bushnaq
 *
 * 01-04-2021 @ 09:31:13
 *
 * see LICENSE
*/

#ifndef _TAP_H_
#define _TAP_H_

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	_Bool got_plan;
	_Bool todo;
	_Bool skip;
	unsigned int plan;
	unsigned int done;
	unsigned int failed;
	char const *msg;
} tap;

static tap tap_stat = { 0 };

static unsigned int
diag(char const* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	putchar('\n');

	return 0;
}

static int
exit_status()
{
	int ret = 0;

	if (tap_stat.plan == 0)
	{
		printf("1..%u\n", tap_stat.done);
	}
	else if (tap_stat.plan != tap_stat.done)
	{
		diag("Looks like you planned %d tests but ran %d", \
		     tap_stat.plan, tap_stat.done);
		ret = 2;
	}
	if (tap_stat.failed)
	{
		diag("Looks like you failed %d test%s of %d run", \
		     tap_stat.failed, tap_stat.plan > 1 ? "s" : "", \
		     tap_stat.done);
		ret = 1;
	}
		
	return ret;
}

static void
bail_out(char const* fmt, ...)
{
	va_list ap;

	printf("Bail out! ");
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	putchar('\n');

	exit(0);
}

static int
plan_tests(unsigned int nb)
{
	assert(nb > 0);

	if (tap_stat.got_plan)
		bail_out("Already set a plan");

	tap_stat.got_plan = 1;
	tap_stat.plan = nb;

	printf("1..%u\n", nb);
	return 0;
}

static int
plan_no_plan(void)
{
	if (tap_stat.got_plan)
		bail_out("Already set a plan");

	tap_stat.got_plan = 1;
	return 0;
}

static int
plan_skip_all(char const* fmt, ...)
{
	va_list ap;

	printf("1..0 # skip ");
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	putchar('\n');	
	
	/* Get rid of potential warnings */
	exit(0); return 0;
}

static unsigned int
ok(_Bool expression, char const* fmt, ...)
{
	va_list ap;

	printf("%s %d - ", expression ? "ok":"not ok", ++tap_stat.done);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	
	if (tap_stat.todo)
	{
		assert(!tap_stat.skip);
		printf(" # todo %s\n", tap_stat.msg);
	}
	else if (tap_stat.skip)
	{
		assert(!tap_stat.todo);
		printf(" # skip %s\n", tap_stat.msg);
	}
	else
	{
		putchar('\n');
	}

	expression ? (void) ap : ++tap_stat.failed;
	return expression;
}

static unsigned int
ok1(int expression)
{
	printf("%s %d\n", expression ? "ok":"not ok", ++tap_stat.done);
	
	expression ? (void) expression : ++tap_stat.failed;
	return expression;
}

static unsigned int
pass(char const* fmt, ...)
{
	va_list ap;

	printf("ok %d - ", ++tap_stat.done);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	putchar('\n');
	
	return 0;
}

static unsigned int
fail(char const* fmt, ...)
{
	va_list ap;

	printf("not ok %d - ", ++tap_stat.done);
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	putchar('\n');

	++tap_stat.failed;
	return 0;
}

static void
skip_start(char const* msg)
{
	tap_stat.skip = 1;
	tap_stat.msg = msg;
}

static void
skip_stop()
{
	tap_stat.todo = 0;
}

static int
skip(unsigned int nb, char const* fmt, ...)
{
	unsigned int i = 0;
	va_list ap;

	for (; i != nb; ++i)
	{
		printf("ok %d - # skip ", ++tap_stat.done);
		va_start(ap, fmt);
		vprintf(fmt, ap);
		va_end(ap);
		putchar('\n');
	}

	return 0;
}

/* TODO: check if plan called before ok etc */

static void
todo_start(char const* msg)
{
	tap_stat.todo = 1;
	tap_stat.msg = msg;
}

static void
todo_stop()
{
	tap_stat.todo = 0;
}

#endif /* _TAP_H_ */