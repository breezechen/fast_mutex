#ifndef _BR_MUTEX_LOCKER_H_
#define _BR_MUTEX_LOCKER_H_
#include <windows.h>

namespace br {
#if defined(__GNUC__) || defined(__GNUG__)
#define __br_compare_and_swap(x, o, n) __sync_val_compare_and_swap(x, o, n)
#define __br_fetch_and_add(x, v) __sync_fetch_and_add(x, v)
#define __br_fetch_and_sub(x, v) __sync_fetch_and_sub(x, v)
#elif defined(_MSC_VER)
#define __br_compare_and_swap(x, o, n) InterlockedCompareExchange(x, n, o)   
#define __br_fetch_and_add(x, v) InterlockedExchangeAdd(x, v)
#define __br_fetch_and_sub(x, v) InterlockedExchangeAdd(x, -v) 
#endif
    struct mutex
    {
        volatile LONG sem;
        volatile bool askwrite;
        mutex() :sem(0), askwrite(false) {}
    };

    class locker
    {
    public:
        locker(mutex& m, bool shared = false) : _m(m), _write(!shared) {
            if (_write) { // big brother
                while (__br_compare_and_swap(&_m.sem, 0, -1) != 0) { //someone taking my toy
                    _m.askwrite = true; // I need the toy right now!!!
                    Sleep(1); // wait
                }
                _m.askwrite = false; // your buddies can competing for it now
            } else {
                while(1) {
                    if (_m.askwrite) { // the big brother is looking for it, stand by
                        Sleep(1);
                    } else if (__br_fetch_and_add(&_m.sem, 2) <= -1) { // big brother is enjoying
                        __br_fetch_and_sub(&_m.sem, 2); // sorry, I'm not mean it, step back
                        Sleep(1);
                    } else { // we can share it now
                        break;
                    }
                };
            }
        }
        ~locker() {
            if (_write) {
                __br_fetch_and_add(&_m.sem, 1);
            } else {
                __br_fetch_and_sub(&_m.sem, 2);
            }
        }
    private:
        mutex& _m;
        bool _write;
    };
}

#endif