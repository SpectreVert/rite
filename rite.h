/*
 * Created by Costa Bushnaq
 *
 * 18-03-2022 @ 12:16:48
 *
 * see LICENSE
*/

#ifndef OG_RITE_H_
#define OG_RITE_H_

//----------------------------

#define OG_RITE_VERSION "1.0.0"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/param.h>

typedef int32_t  s32;
typedef uint8_t   u8;
typedef uint32_t u32;

enum {
    RTF_SKIP  = 1,
    RTF_TODO,
    RTF_DEBUG, // @Unused

    RTF_END  = 8,
};
typedef struct {
    u8  flags;
    u32 planned;
    u32 done;
    u32 failed;
    char const *skip_buffer;
    char const *todo_buffer;
} Rite_Context;

#define RT_WRITE_MSG(fmt) do { \
    if (fmt) { \
        va_list list_args; \
        va_start(list_args, fmt); \
        vprintf(fmt, list_args); \
        va_end(list_args); \
    } } while (0)

#define RT_DO_TEST(expr, fmt) do { \
    u8 is_ok = 1; \
    if (the_rite_ctx.flags & ((u8)1 << RTF_SKIP)) is_ok = 1; \
    else if (the_rite_ctx.flags & ((u8)1 << RTF_TODO)) is_ok = expr; \
    printf("%s %d%s", is_ok ? "ok" : "not ok", ++the_rite_ctx.done, \
           fmt ? " - " : ""); \
    RT_WRITE_MSG(fmt); \
    if (the_rite_ctx.flags & ((u8)1 << RTF_SKIP)) { \
        if (the_rite_ctx.skip_buffer) \
            printf(" # SKIP %s", the_rite_ctx.skip_buffer); \
    } else if (the_rite_ctx.flags & ((u8)1 << RTF_TODO)) { \
        if (the_rite_ctx.todo_buffer) \
            printf(" # TODO %s", the_rite_ctx.todo_buffer); \
    } else { \
        if (!expr) the_rite_ctx.failed += 1; \
    } \
    printf("\n"); } while (0)

#define RT_CHECK_FAILED do { \
    if (the_rite_ctx.failed) { \
        diag("Failed %d test%s out of %d run", the_rite_ctx.failed, \
        the_rite_ctx.failed == 1 ? "" : "s", the_rite_ctx.done); \
    } } while (0)

// ---------------------------------------------------------------

static Rite_Context the_rite_ctx = {0};

static void diag(char const* fmt, ...)
{
    va_list list_args;

    va_start(list_args, fmt);
    printf("# ");
    vprintf(fmt, list_args);
    printf("\n");
    va_end(list_args);
}

static s32 exit_status()
{
    if (!the_rite_ctx.planned) {
        printf("1..%d\n", the_rite_ctx.done);
        RT_CHECK_FAILED;
        return MIN(254, the_rite_ctx.failed);
    } else if (the_rite_ctx.planned == the_rite_ctx.done) {
        RT_CHECK_FAILED;
        return MIN(254, the_rite_ctx.failed);
    } else {
        diag("Planned %d test%s but %d run", the_rite_ctx.planned,
        the_rite_ctx.planned == 1 ? "" : "s", the_rite_ctx.done);
        return 255;
    }
}

static void bail_out(char const *fmt, ...)
{
    va_list list_args;

    va_start(list_args, fmt);
    printf("Bail out! ");
    vprintf(fmt, list_args);
    printf("\n");
    va_end(list_args);
    exit(255);
}

static void plan_tests(u32 nb_tests)
{
    assert(nb_tests > 0);
    setbuf(stdout, 0);
    if (the_rite_ctx.planned)
        bail_out("Already set a plan");
    the_rite_ctx.planned = nb_tests;
    printf("1..%d\n", nb_tests);
}

static void plan_skip_all(char const *fmt, ...)
{
    va_list list_args;

    if (the_rite_ctx.planned)
        bail_out("Already set a plan");

    va_start(list_args, fmt);
    printf("1..0 # SKIP ");
    vprintf(fmt, list_args);
    printf("\n");
    va_end(list_args);
    exit(0);
}

static u8 ok(u8 expr, char const *fmt, ...)
{
    RT_DO_TEST(expr, fmt);

    return expr;
}

static u8 ok1(u8 expr)
{
    return ok(expr, 0x0);
}

static u8 pass(char const *fmt, ...)
{
    RT_DO_TEST(1, fmt);

    return 1;
}

static u8 fail(char const *fmt, ...)
{
    RT_DO_TEST(0, fmt);

    return 0;
}

static void skip(u8 expr, char const *skip_message)
{
    if (!expr) return;
    the_rite_ctx.flags |= ((u8)1 << RTF_SKIP);
    the_rite_ctx.skip_buffer = skip_message;
    // As explained in README, we leave memory managment to the user.
}

static void skip1(char const *skip_message)
{
    skip(1, skip_message);
}

static void skip_end()
{
    the_rite_ctx.flags &= ~((u8)1 << RTF_SKIP);
    the_rite_ctx.skip_buffer = 0x0;
}

static void todo(char const *todo_message)
{
    the_rite_ctx.flags |= ((u8)1 << RTF_TODO);
    the_rite_ctx.todo_buffer = todo_message;
    // As explained in README, we leave memory managment to the user.
}

static void todo_end()
{
    the_rite_ctx.flags &= ~((u8)1 << RTF_TODO);
    the_rite_ctx.todo_buffer = 0x0;
}

#endif /* OG_RITE_H_ */
