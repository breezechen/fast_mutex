#ifndef _BR_MUTEX_LOCKER_H_
#define _BR_MUTEX_LOCKER_H_
#include <windows.h>

namespace br {
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
                while (InterlockedCompareExchange(&_m.sem, -1, 0) != 0) { //someone taking my toy
                    _m.askwrite = true; // I need the toy right now!!!
                    Sleep(1); // wait
                }
                _m.askwrite = false; // your buddies can competing for it now
            } else {
                while(1) {
                    if (_m.askwrite) { // the big brother is looking for it, stand by
                        Sleep(1);
                    } else if (InterlockedExchangeAdd(&_m.sem, 2) <= -1) { // big brother is enjoying
                        InterlockedExchangeAdd(&_m.sem, -2); // sorry, I'm not mean it, step back
                        Sleep(1);
                    } else { // we can share it now
                        break;
                    }
                };
            }
        }
        ~locker() {
            if (_write) {
                InterlockedExchangeAdd(&_m.sem, 1);
            } else {
                InterlockedExchangeAdd(&_m.sem, -2);
            }
        }
    private:
        mutex& _m;
        bool _write;
    };
}

#endif