/*
 * Created by Costa Bushnaq
 *
 * 06-12-2022 @ 10:02:36
 *
 * see LICENSE
*/

#ifndef ZEE_RITE_H
#define ZEE_RITE_H

#ifndef ZEE_BASIC_TYPES
#define ZEE_BASIC_TYPES
#include <stdint.h>
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif /* ZEE_BASIC_TYPES */

#ifdef __cplusplus
extern "C" {
#endif

extern void plan_tests(u32 nb_tests);
extern void plan_skip_all(const char *fmt, ...);
extern s32  exit_status(void);

extern void diag(const char *fmt, ...);
extern void bail_out(const char *fmt, ...);

extern s32  ok(s32 expr, char const *fmt, ...);
extern s32  ok1(s32 expr);
extern s32  pass(char const *fmt, ...);
extern s32  fail(char const *fmt, ...);

extern void skip(u32 nb_to_skip, char const *fmt, ...);
extern void todo_start(char const *fmt, ...);
extern void todo_end(void);

#ifdef __cplusplus
}
#endif

/* -- No Man's Land between header and source -- */

#ifdef ZEE_RITE_IMPLEMENTATION

/* Change this value to something more suitable if you plan
 * to generate long skip / todo messages.
*/
#ifndef ZEE_RITE_REASON_SIZE
#define ZEE_RITE_REASON_SIZE 4096
#endif

/* ------------- */

#include <stdarg.h>
#include <stdlib.h> // malloc, exit
#include <stdio.h>  // setvbuf, printf
#include <string.h> // memset

#ifndef ZEE_ASSERT
#include <assert.h>
#define ZEE_ASSERT(x) assert(x)
#endif

typedef struct Rite_Context Rite_Context;
struct Rite_Context {
    u8 todo;
    u32 planned;
    u32 done;
    u32 failed;
    char skip_msg[ZEE_RITE_REASON_SIZE];
    char todo_msg[ZEE_RITE_REASON_SIZE];
};

static Rite_Context *the_rite_ctx = 0x0;

extern void plan_tests(u32 nb_tests)
{
    assert(nb_tests > 0);
    if (!the_rite_ctx) {
        the_rite_ctx = (Rite_Context*) malloc(sizeof(Rite_Context));
        assert(the_rite_ctx);
        memset(the_rite_ctx, 0, sizeof(Rite_Context));
        setvbuf(stdout, 0, _IONBF, BUFSIZ);
        the_rite_ctx->planned = nb_tests;
        printf("1..%d\n", nb_tests);
    }
}

extern void plan_no_plan(void)
{
    if (!the_rite_ctx) {
        the_rite_ctx = (Rite_Context*) malloc(sizeof(Rite_Context));
        assert(the_rite_ctx);
        memset(the_rite_ctx, 0, sizeof(Rite_Context));
        setvbuf(stdout, 0, _IONBF, BUFSIZ);
    }
}

extern void plan_skip_all(const char *fmt, ...)
{
    va_list args;

    setvbuf(stdout, 0, _IONBF, BUFSIZ);
    va_start(args, fmt);
    printf("1..0 # skip ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    exit(0);
}

extern s32 exit_status(void)
{
#define DISPLAY_FAILED do { \
    if (the_rite_ctx->failed) { \
        diag("Failed %d test%d out of %d run", \
            the_rite_ctx->failed, \
            the_rite_ctx->failed == 1 ? "" : "s", \
            the_rite_ctx->done); \
    }} while (0)

    if (!the_rite_ctx->planned) {
        printf("1..%d\n", the_rite_ctx->done);
        DISPLAY_FAILED;
        return (the_rite_ctx->failed < 254 ? the_rite_ctx->failed : 254);
    } else if (the_rite_ctx->planned == the_rite_ctx->done) {
        DISPLAY_FAILED;
        return (the_rite_ctx->failed < 254 ? the_rite_ctx->failed : 254);
    } else {
        diag("Planned %d test%s but %d run",
            the_rite_ctx->planned,
            the_rite_ctx->planned == 1 ? "" : "s",
            the_rite_ctx->done);
        return 255;
    }
#undef DISPLAY_FAILED
}

extern void diag(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    printf("# ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

extern void bail_out(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    printf("Bail out! ");
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
    exit(255);
}

#define ZEE_RITE___TEST(expr, fmt) do { \
    s32 is_ok = 1; \
    if (!the_rite_ctx->todo) { is_ok = expr; } \
    printf("%s %d%s", \
        is_ok ? "ok" : "not ok", \
        ++the_rite_ctx->done, \
        fmt ? " - " : ""); \
    if (fmt) { \
        va_list args; \
        va_start(args, fmt); \
        vprintf(fmt, args); \
        va_end(args); \
    } \
    if (the_rite_ctx->todo) { \
        printf(" # todo %s", the_rite_ctx->todo_msg); \
    } else if (!expr) { \
        the_rite_ctx->failed += 1; \
    } \
    printf("\n"); \
    } while (0)


extern s32 ok(s32 expr, char const *fmt, ...)
{
    ZEE_RITE___TEST(expr, fmt);

    return expr;
}

extern s32 ok1(s32 expr)
{
    return ok(expr, 0x0);
}

extern s32 pass(char const *fmt, ...)
{
    ZEE_RITE___TEST(1, fmt);

    return 1;
}

extern s32 fail(char const *fmt, ...)
{
    ZEE_RITE___TEST(0, fmt);

    return 0;
}

extern void skip(u32 nb_to_skip, char const *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);
    if (vsnprintf(the_rite_ctx->skip_msg, ZEE_RITE_REASON_SIZE, fmt, args) < 0) {
        bail_out("Error on `vsnprintf` ; try changing `ZEE_RITE_REASON_SIZE`");
    }
    va_end(args);

    while (nb_to_skip-- > 0) {
        the_rite_ctx->done += 1;
        printf("ok %d # skip %s\n",
            the_rite_ctx->done, the_rite_ctx->skip_msg);
    }
}

#define skip_start(expr, n, ...) do { \
    if ((expr)) { \
        skip(n, __VA_ARGS__); \
        continue; \
    }

#define skip_end() \
    } while (0) 

extern void todo_start(char const *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    if (vsnprintf(the_rite_ctx->todo_msg, ZEE_RITE_REASON_SIZE, fmt, args) < 0) {
        bail_out("Error in `vsnprintf` ; try changing `ZEE_RITE_REASON_SIZE`");
    }
    va_end(args);
    the_rite_ctx->todo = 1;
}

extern void todo_end(void)
{
    the_rite_ctx->todo = 0;
}

#endif /* ZEE_RITE_IMPLEMENTATION */
#endif /* ZEE_RITE_H */
