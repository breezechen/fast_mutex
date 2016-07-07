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
            if (_write) {
                while (InterlockedCompareExchange(&_m.sem, -1, 0) != 0) {
                    _m.askwrite = true;
                    Sleep(1);
                }
                _m.askwrite = false;
            }
            else {
                do {
                    if (_m.askwrite) {
                        Sleep(1);
                    }
                    else if (InterlockedExchangeAdd(&_m.sem, 2) <= -1) {
                        InterlockedExchangeAdd(&_m.sem, -2);
                        Sleep(1);
                    }
                    else {
                        break;
                    }
                } while (1);
            }
        }
        ~locker() {
            if (_write) {
                InterlockedExchangeAdd(&_m.sem, 1);
            }
            else {
                InterlockedExchangeAdd(&_m.sem, -2);
            }
        }
    private:
        mutex& _m;
        bool _write;
    };
}

#endif