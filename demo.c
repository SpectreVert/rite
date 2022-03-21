/*
 * Created by Costa Bushnaq
 *
 * 03-08-2021 @ 11:12:20
*/

#include "rite.h"

#define NO_LEG_DAY 1

int main()
{
    plan_tests(8);

    ok(1, "Zonzibar");
    ok(1, 0x0);
    skip(NO_LEG_DAY, "legs dead");
    ok(1, "P + C");
    ok(0, "KOWABUNGA %d", 69);
    ok(1, "Hello Sailor!");
    ok(0, "no");
    ok1(0);
    skip_end();
    todo("I'll get this done later");
    ok(1, "I should not be skipped!!!");
    todo_end();

    // Uncomment to see what happens when the plan is wrong...
    // ok(1, "This test was not planned");

    return exit_status();
}
