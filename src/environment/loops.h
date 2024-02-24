#ifndef LOOPS_H
#define LOOPS_H

enum type
{
    LOOP_NOTHING = 0,
    LOOP_BREAK,
    LOOP_CONTINUE,
};

struct loop_info
{
    int loop_level;
    int loop_remaining;
    enum type action;
};

struct loop_info *get_loop_info(void);

void loop_append_level(void);

void loop_reduce_level(void);

void loop_set_action(enum type action, int remaining);

void loop_reset_action(void);

void loop_reset_default(void);

void loop_restore(struct loop_info info);

#endif /* ! LOOPS_H */
