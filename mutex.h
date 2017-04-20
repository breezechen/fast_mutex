#ifndef _BR_MUTEX_LOCKER_H_
#define _BR_MUTEX_LOCKER_H_

#if defined(__GNUC__) || defined(__GNUG__)
#include <time.h>
#define __br_compare_and_swap(x, o, n) __sync_val_compare_and_swap(x, o, n)
#define __br_fetch_and_add(x, v) __sync_fetch_and_add(x, v)
#define __br_fetch_and_sub(x, v) __sync_fetch_and_sub(x, v)
#define __br_set(x,v)   __sync_lock_test_and_set(x, v)
#elif defined(_MSC_VER)
#include <windows.h>
#include <time.h>
#define __br_compare_and_swap(x, o, n) InterlockedCompareExchange(x, n, o)
#define __br_fetch_and_add(x, v) InterlockedExchangeAdd(x, v)
#define __br_fetch_and_sub(x, v) InterlockedExchangeAdd(x, -v)
#define __br_set(x,v)   InterlockedExchange(x, v)
#endif

static void msleep(int ms)
{
#ifdef WIN32
    Sleep(ms);
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}

struct mutex {
    volatile long sem;
    volatile long askwrite;
};

static void init_mutex(struct mutex* m)
{
    m->sem = 0;
    m->askwrite = 0;
}

static void lock_for_read(struct mutex* m)
{
    while (1) {
        if (m->askwrite) { // the big brother is looking for it, stand by
            msleep(1);
        } else if (__br_fetch_and_add(&m->sem, 2) <= -1) { // big brother is enjoying
            __br_fetch_and_sub(&m->sem, 2); // sorry, I'm not mean it, step back
            msleep(1);
        } else { // we can share it now
            break;
        }
    };
}

static void lock_for_write(struct mutex* m)
{
    while (__br_compare_and_swap(&m->sem, 0, -1) != 0) { //someone taking my toy
        m->askwrite = 1; // I need the toy right now!!!
        msleep(1); // wait
    }
    m->askwrite = 0; // your buddies can competing for it now
}

static void lock(struct mutex* m)
{
    while (__br_compare_and_swap(&m->sem, 0, -1) != 0) { //someone taking my toy
        m->askwrite = 1; // I need the toy right now!!!
        msleep(1); // wait
    }
    m->askwrite = 0; // your buddies can competing for it now
}

static void unlock_for_read(struct mutex* m)
{
    __br_fetch_and_sub(&m->sem, 2);
}

static void unlock_for_write(struct mutex* m)
{
    __br_fetch_and_add(&m->sem, 1);
}

static void unlock(struct mutex* m)
{
    __br_fetch_and_add(&m->sem, 1);
}


struct condition_variable
{
    volatile long count;
    volatile long signal;
};

static void init_condition_variable(struct condition_variable* cond)
{
    cond->count = 0;;
    cond->signal = 0;
}

static void destory_condition_variable(struct condition_variable* cond)
{
    while (cond->count > 0) {
        __br_fetch_and_add(&cond->signal, 1);
    }
}

static int wait(struct condition_variable* cond, unsigned long timeout_ms)
{
    int ret = 0;
    __br_fetch_and_add(&cond->count, 1);
    clock_t start = clock();
    while (__br_fetch_and_sub(&cond->signal, 1) <= 0) {
        __br_fetch_and_add(&cond->signal, 1);
        clock_t now = clock();
        if ((unsigned long)((now - start) * 1000 / CLOCKS_PER_SEC) > timeout_ms) {
            ret = 1;
            break;
        }
        msleep(1);
    }
    __br_fetch_and_sub(&cond->count, 1);
    return ret;
}

static void notify_one(struct condition_variable* cond)
{
    __br_fetch_and_add(&cond->signal, 1);
}

static void notify_all(struct condition_variable* cond)
{
    __br_set(&cond->signal, cond->count);
}

#endif