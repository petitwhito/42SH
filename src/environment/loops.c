#include "loops.h"

#include "environment.h"

static struct loop_info loop_info = { 0, -1, LOOP_NOTHING };

struct loop_info *get_loop_info(void)
{
    return &loop_info;
}

void loop_append_level(void)
{
    loop_info.loop_level++;
}

void loop_reduce_level(void)
{
    loop_info.loop_level--;
}

void loop_set_action(enum type action, int remaining)
{
    // Do nothing in case that there is no loop active
    if (loop_info.loop_level == 0)
        return;

    loop_info.action = action;
    if (remaining > loop_info.loop_level)
        remaining = loop_info.loop_level;
    loop_info.loop_remaining = remaining;
}

void loop_reset_action(void)
{
    loop_info.action = LOOP_NOTHING;
    loop_info.loop_remaining = -1;
}

void loop_reset_default(void)
{
    loop_info = (struct loop_info){ 0, -1, LOOP_NOTHING };
}

void loop_restore(struct loop_info info)
{
    loop_info = info;
}
